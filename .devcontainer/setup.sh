#!/bin/bash

apt-get update
# apt-get install -y [your dependencies here]

if [ -f "./.devcontainer/welcome-message.txt" ]; then
	sudo cp --force ./.devcontainer/first-run-notice.txt /usr/local/etc/vscode-dev-containers/first-run-notice.txt
fi