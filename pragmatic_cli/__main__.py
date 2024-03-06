import sys

import click
from lorem.text import TextLorem

# Initialize a Lorem Ipsum generator
lorem = TextLorem(srange=(4, 8), trange=(2, 20), prange=(1, 3))

@click.group()
def cli():
    pass

@cli.command()
def chat():
    """Starts a pragmatic chat that replies with Lorem Ipsum text."""
    click.echo("Welcome to Pragmatic Chat! Type something and press Enter to get a reply. Type 'exit' to quit.")

    while True:
        user_input = input("> ")
        if user_input.lower() == 'exit':
            click.echo("Goodbye!")
            break
        else:
            click.echo(lorem.sentence())

if __name__ == '__main__':
    cli()