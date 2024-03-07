from interface_chat import InterfaceBackend

from lorem.text import TextLorem

# Lorem Ipsum Backend
class LoremBackend(InterfaceBackend):
    
    def __init__(self):
        self.lorem = TextLorem(srange=(4, 8), trange=(2, 20), prange=(1, 3))
    
    def get_reply(self, prompt, chat_history):
        return self.lorem.sentence()