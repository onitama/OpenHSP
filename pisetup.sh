#!/bin/bash
set -eux
echo "Setup HSP3 for Raspberry Pi"
sudo apt-get update
sudo apt-get install --yes libgtk2.0-dev
sudo apt-get install --yes libglew-dev
sudo apt-get install --yes libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev
sudo apt-get install --yes libegl1-mesa-dev
make -f makefile.raspbian
sudo cp hsed.png /usr/share/pixmaps/
sudo cp hsed.desktop /usr/share/applications/
lxpanelctl restart
