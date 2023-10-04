#!/bin/bash

# Welcome message
if [ -f "./.devcontainer/first-run-notice.txt" ]; then
	sudo cp --force ./.devcontainer/first-run-notice.txt /usr/local/etc/vscode-dev-containers/first-run-notice.txt
fi

# Basic tools
sudo apt-get update
sudo apt-get install -y git python3

# Newer GCC is needed for C++20
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install -y gcc-10 g++-10
# Set as default compiler
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 60 --slave /usr/bin/g++ g++ /usr/bin/g++-10

# Attaching to certain proccesses requires sudo elevation
# Disable this so debugging is more effortless
echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope

# ---------------------------------------- CPython ---------------------------------------- #
# https://devguide.python.org/getting-started/setup-building/#linux

# Tools needed for building CPython
sudo apt-get build-dep python3
sudo apt-get install -y pkg-config

# Tools needed for building optional CPython modules
sudo apt-get install -y \
		build-essential gdb lcov pkg-config \
		libbz2-dev libffi-dev libgdbm-dev libgdbm-compat-dev liblzma-dev \
		libncurses5-dev libreadline6-dev libsqlite3-dev libssl-dev \
		lzma lzma-dev tk-dev uuid-dev zlib1g-dev