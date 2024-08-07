import os
import pdfplumber
import pyperclip
import datetime
import pandas as pd
from tqdm import *

from langchain_community.document_loaders import PyPDFLoader, UnstructuredPDFLoader
from langchain_huggingface import HuggingFaceEmbeddings
from langchain_google_genai import ChatGoogleGenerativeAI
from langchain_community.vectorstores import FAISS

os.environ['GOOGLE_API_KEY'] = 'AIzaSyDHo-axU5FW2uItDTO26uINKDmPkeLqBxU'

print('Finding file...')
folder = '../../02Standard/fraction/MST'
filename = 'MST_w_additional.pdf'
filepath = os.path.join(folder, filename)
return_document = 25

# print(f'Extracting pages: {return_document}')
# print('Loading PDF...')
# loader = PyPDFLoader(filepath)
# pages = loader.load_and_split()

# print('Loading embeddings...')
# embeddings = HuggingFaceEmbeddings(model_name = 'all-MiniLM-L6-v2')
# db = FAISS.from_documents(pages, embeddings)

shared_information = ''
shared_information += 'Output should be pack together and be represented with hexadecimal, e.g. XX XX XX XX XX ...\n'
# shared_information += 'Beginning is Most Significant Bit, and the compiler uses little endian.\n'
# shared_information += 'Payload of other field not specified can be generated whatever you want. e.g. Random, ordered, etc.\n'

def parsing_pdf(filepath):
    with pdfplumber.open(filepath) as pdf:
        content =  ''
        for page in tqdm(pdf.pages):
            content += page.extract_text()
            
            tables = page.extract_tables()
            for i, table in enumerate(tables):
                if all(not cell for row in table for cell in row):
                    continue
                df = pd.DataFrame(table[1:], columns=table[0])
                df = df.dropna(axis=1, how='all')  # Remove columns with all None values
                df = df.dropna(axis=0, how='all')  # Remove rows with all None values
                content += df.to_string()
        return content

content = parsing_pdf(filepath)

def gen_LLCP_prompt():
    query = ''
    query += 'Please write a C code to generate the header of a USB4 Tunneled Packet from a Link Layer Control Packet (LLCP) and the header of a Link Layer Control Packet.\n'
    query += 'If the field is well-defined as a fixed value, please use the value in the code.\n'
    query += 'If the value of field can vary as user defined, please make it as a variable that can be fefined by the user and let it be the argv of the main function.\n'

    additional_information = 'Tunneled Link Layer Control Packet contains USB4 Tunneled Packet Header and Link Layer Control Packet.\n'
    additional_information += 'USB4 Tunneled Packet Header contains PDF, SuppID, Reserved, HopID, Length and HEC.\n'
    additional_information += 'Link Layer Control Packet contains Reserved, Gap Counter and Allocated Slots'
    additional_information += 'LLCP Packet Format should be as follows:\n'
    additional_information += '|    PDF=8  | SuppID | Rsvd |  HopID  | Length = 1Ch |    HEC   |'
    additional_information += '|---------------------------------------------------------------|'
    additional_information += '| Reserved      |          Gap Counter        | Allocated Slots |'
    additional_information += '|---------------------------------------------------------------|'
    additional_information += '|           ECF[15:0]           |  Reserved  | LLEI | HEI | ACT |'
    additional_information += '|---------------------------------------------------------------|'
    additional_information += '|                               ECF[47:16]                      |'
    additional_information += '|---------------------------------------------------------------|'
    additional_information += '|             LVP - Lane 0      |         ECF[63:48]            |'
    additional_information += '|---------------------------------------------------------------|'
    additional_information += '|             LVP - Lane 1      |         ECF[63:48]            |'
    additional_information += '|---------------|-----------------------------------------------|'
    additional_information += '|             LVP - Lane 2      |         ECF[63:48]            |'
    additional_information += '|-------------------------------------------------|-------------|'
    additional_information += '|             LVP - Lane 3      |         ECF[63:48]            |'
    additional_information += '|-------------------------------------------------|-------------|'

    qa_prompt = 'Use the following pieces of context to complete user requirements:\n'
    input_text = qa_prompt + 'Context: ' + content + '\nOther information: ' + shared_information + 'User\'s requirements: ' + query + additional_information + 'Thank you!'
    return input_text

def gen_CDP_prompt():
    query = ''
    query += 'Please write a C code to generate a USB4 Control and Data Packet.\n'
    query += 'If the field is well-defined as a fixed value, please use the value in the code.\n'
    query += 'If the value of field can vary as user defined, please make it as a variable that can be fefined by the user and let it be the argv of the main function.\n'
    query += 'Do not use any struct in the code.\n'

    additional_information = ''
    additional_information += 'A Control and Data Packet contains a USB4 Header and Several TU (Transfer Unit).\n'
    additional_information += 'A TU consists of a header and optionally Data Words.\n'
    additional_information += 'And there are two types of TU, which are Control TU and Data TU.\n'
    additional_information += 'Details of these two types of TU please refer to the context.\n'
    additional_information += 'A TU Header consists of Reserved[31:16], Count[15:8], Data TU[7], Reserved[6:4] and Control Type[3:0].\n'
    additional_information += 'User input should be <Params of TU 1> <Params of TU 2> ... <Params of TU n>\n'
    additional_information += 'If the TU is Data TU, params are <Count> since the control type should be set to 0 for Data TU Header.\n'
    additional_information += 'If the TU is Control TU, params are <Count> <Control Type>\n'


    qa_prompt = 'Use the following pieces of context to complete user requirements:\n'
    input_text = qa_prompt + 'Context: ' + content + '\nOther information: ' + shared_information + 'User\'s requirements: ' + query + additional_information + 'Thank you!'
    return input_text


print('Generating prompt...')
prompts = {
    'LLCP': gen_LLCP_prompt(),
    'CDP': gen_CDP_prompt()
}

packet_type = 'CDP'
output = prompts[packet_type]
# log_folder = 'log'
current_time = datetime.datetime.now().strftime("%m%d%H%M")
# filename = packet_type + '_' + current_time + '.txt'
# log_filepath = os.path.join(log_folder, filename)
# with open(log_filepath, 'a', encoding='utf-8') as f:
#     f.write(output)
# Copy the output to clipboard
pyperclip.copy(output)
print(f'Output has been copied to clipboard ({packet_type}).')
print('Execution time:', datetime.datetime.now().strftime("%m/%d %H:%M"))
