import sys

import click
from lorem.text import TextLorem
import openai

# Initialize a Lorem Ipsum generator
lorem = TextLorem(srange=(4, 8), trange=(2, 20), prange=(1, 3))

def load_api_key(file_path='OAI'):
    """Load the OpenAI API key from a file."""
    try:
        with open(file_path, 'r') as file:
            return file.read().strip()
    except FileNotFoundError:
        raise FileNotFoundError(f"The file {file_path} was not found. Please ensure it exists and contains your OpenAI API key.")
    
# Configure your OpenAI API key here
openai.api_key = load_api_key()

@click.group()
def cli():
    pass

def get_lorem_reply():
    """Function to generate a Lorem Ipsum reply."""
    return lorem.sentence()

def get_openai_reply(prompt, chat_history):
    """Function to generate a reply using OpenAI's ChatGPT using the chat completions endpoint, maintaining conversation context."""
    try:
        chat_history.append({"role": "user", "content": prompt})
        response = openai.ChatCompletion.create(
            model="gpt-4-0125-preview",  # Adjust the model if needed
            messages=chat_history,
        )
        reply = response.choices[0].message['content'].strip()
        chat_history.append({"role": "assistant", "content": reply})
        return reply
    except Exception as e:
        return f"An error occurred: {str(e)}"

def get_input(prompt="> "):
    """Collects either single or multiline input from the user."""
    print(prompt, end="")
    initial_input = input()
    if initial_input.lower() == 'multiline':
        return get_multiline_input(prompt)
    else:
        return initial_input

def get_multiline_input(prompt="> "):
    """Collects multiline input from the user, stopping when '/end' is entered."""
    lines = []
    print("(Multiline mode: type '/end' on a new line to finish)")
    while True:
        print(prompt, end="")
        line = input()
        if line.strip() == '/end':
            break
        lines.append(line)
    return "\n".join(lines)

@cli.command()
@click.option('--backend', type=click.Choice(['lorem', 'openai'], case_sensitive=False), prompt="Please choose a backend", help="The backend to use for generating chat replies.")
def chat(backend):
    """Starts a pragmatic chat that replies with text based on the selected backend, maintaining conversation context for OpenAI backend, and allowing single or multiline inputs."""
    click.echo(f"Starting chat with {backend} backend. Type 'multiline' for multiline inputs or type and press Enter for single line inputs. Type 'exit' to quit.")
    chat_history = []

    while True:
        user_input = get_input()
        if user_input.lower() == 'exit':
            click.echo("Goodbye!")
            break
        elif backend.lower() == 'lorem':
            click.echo(get_lorem_reply())
        elif backend.lower() == 'openai':
            click.echo(get_openai_reply(user_input, chat_history))

if __name__ == '__main__':
    cli()