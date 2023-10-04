#!/bin/bash

sudo apt-get update
sudo apt-get install -y git python3

git submodule update --init --recursive

if [ -f "./.devcontainer/first-run-notice.txt" ]; then
	sudo cp --force ./.devcontainer/first-run-notice.txt /usr/local/etc/vscode-dev-containers/first-run-notice.txt
fi