from interface_chat import InterfaceBackend

from openai import OpenAI

def load_api_key(file_path='OAI'):
    """Load the OpenAI API key from a file."""
    try:
        with open(file_path, 'r') as file:
            return file.read().strip()
    except FileNotFoundError:
        raise FileNotFoundError(f"The file {file_path} was not found. Please ensure it exists and contains your OpenAI API key.")

# OpenAI Backend
class OpenAIBackend(InterfaceBackend):
    
    def __init__(self):
        self.client = OpenAI(api_key=load_api_key())
    
    def get_reply(self, prompt, chat_history):
        try:
            chat_history.append({"role": "user", "content": prompt})
            response = self.client.chat.completions.create(
                model="gpt-4-0125-preview", messages=chat_history)
            reply = response.choices[0].message.content.strip()
            chat_history.append({"role": "assistant", "content": reply})
            return reply
        except Exception as e:
            return f"An error occurred: {str(e)}"