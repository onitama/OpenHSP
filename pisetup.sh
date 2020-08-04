#!/bin/bash
set -eux
echo "Setup HSP3 for Raspberry Pi"
sudo apt update
sudo apt install -y libgtk2.0-dev
sudo apt install -y libglew-dev
sudo apt install -y libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
sudo apt install -y libgles2-mesa-dev libegl1-mesa-dev
make -f makefile.raspbian
sudo cp hsed.png /usr/share/pixmaps/
sudo cp hsed.desktop /usr/share/applications/
lxpanelctl restart
