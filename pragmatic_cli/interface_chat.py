from abc import ABC, abstractmethod

# Define the generic interface class
class InterfaceBackend(ABC):
    
    @abstractmethod
    def get_reply(self, prompt, chat_history):
        pass