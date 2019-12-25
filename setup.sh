#!/bin/bash
set -eux
echo "Setup HSP3 for Raspberry Pi"
sudo apt update
sudo apt install -y libgtk2.0-dev
sudo apt install -y libglew-dev
sudo apt install -y libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev
sudo apt install -y libgles2-mesa-dev libegl1-mesa-dev
make
