#!/bin/bash
set -eux
echo "Setup HSP3 for Linux"
sudo apt update
sudo apt install -y libgtk2.0-dev
sudo apt install -y libglew-dev
sudo apt install -y libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
sudo apt install -y libgles2-mesa-dev libegl1-mesa-dev
sudo apt install -y libcurl4-openssl-dev
sudo apt install -y libgpiod2 libgpiod-dev
sudo apt install -y libffi-dev
make
