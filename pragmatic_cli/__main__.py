import sys

import click

from openai_chat import OpenAIBackend
from lorem_chat import LoremBackend

# Continue with the previous code for get_input, get_multiline_input, and cli.command with modifications for backend selection
@click.group()
def cli():
    pass

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
    if backend.lower() == 'openai':
        backend_instance = OpenAIBackend()
    else:  # Default to LoremBackend if not OpenAI
        backend_instance = LoremBackend()
    
    click.echo(f"Starting chat with {backend} backend. Type 'multiline' for multiline inputs or type and press Enter for single line inputs. Type 'exit' to quit.")
    chat_history = []

    while True:
        user_input = get_input()
        if user_input.lower() == 'exit':
            click.echo("Goodbye!")
            break
        reply = backend_instance.get_reply(user_input, chat_history)
        click.echo(reply)

if __name__ == '__main__':
    cli()