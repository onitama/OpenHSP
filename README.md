# OpenHSP/Hot Soup Processor(HSP3)

OpenHSP/Hot Soup Processor (HSP3) is a scripting language system that anyone can easily use. You can easily build applications such as game programs, screen savers, tools and practical software using images just by writing a text script.
HSP3 can run the development environment on Microsoft Windows, Linux / Raspberry Pi . For detailed installation and usage, refer to the manual for each platform.

HSP3 Linux / Raspberry Pi version manual
https://www.onionsoft.net/hsp/v36en/doclib/hsp3linux_pi.html

HSP3 Windows Manual
https://www.onionsoft.net/hsp/v36en/doclib/hsp3win.html

OpenHSP/Hot Soup Processor (HSP3)は、誰でも手軽に使うことのできるスクリプト言語システムです。 テキストによるスクリプトを書くだけで、簡単に画像を使ったゲームプログラム、スクリーンセーバー、ツールや実用ソフトなどのアプリケーションを構築することができます。
https://www.onionsoft.net/hsp/v36en/index.htm


# Windows installation

You can download the original Windows version from the official HSP3 page.
You can choose between the installer (.exe) or the archive (.zip).
https://hsp.tv/make/downlist.html

The latest beta test version can be downloaded from this link.
https://www.onionsoft.net/wp/archives/category/hsp/download


# Linux and Raspberry Pi installation

HSP3 works in the Linux GUI environment (X Window System). Some features work using OpenGL and SDL libraries. If connected to the network, grab the repository using the git command.

```bash
git clone https://github.com/onitama/OpenHSP
cd OpenHSP
./setup.sh
```

If you have an archive, extract the contents to any directory and compile the source. To compile, you need `gcc` and `make` installed. The following libraries are required:

- OpenGL ES2.0 or later / EGL
- SDL2 / SDL2-mixer
- gtk+-2
- libcurl4
- libgpiod2
- libffi

For Debian/Ubuntu, you can install the required packages using your package manager:

```bash
sudo apt update
sudo apt install -y \
  libgtk2.0-dev \
  libglew-dev \
  libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev \
  libgles2-mesa-dev libegl1-mesa-dev \
  libcurl4-openssl-dev \
  libgpiod2 libgpiod-dev \
  libffi-dev
```

The repository only contains the source, so you'll need to compile it using make. (If it doesn't compile correctly depending on your Linux version or distribution, you'll need to fix this first.)
Use the “setup.sh” file to install the libraries and build this application, or run the make command from the terminal.

```bash
make
```

Now the necessary tools have been compiled, and HSP3 is ready for use.


# Legacy Raspberry Pi installation

HSP3 runs on Raspberry Pi OS.
Run it using Raspbian, and connect a keyboard and mouse in advance.
hsp3dish, hsp3gp (HGIMG4) and hsed (script editor) only work in GUI environments.
Extract the latest version from github to any directory and compile the source. To compile, you need an environment where you can run gcc and make.

Run:

```bash
git clone http://github.com/onitama/OpenHSP
cd OpenHSP
```


Additional libraries are required when compiling. Run the following command to install them while connected to the network.

```bash
./setup.sh
```

For devices up to Raspberry Pi 3, use “pisetup.sh” or run this command.
```bash
make -f makefile.raspbian
```

Run the `make` command in the directory where the contents of the archive are expanded. 

Now the necessary tools have been compiled, and HSP3 is ready for use.

* For Raspberry Pi 4 or later, please install with make with the same method as Linux.
* Raspberry Pi 4 or later does not support full-screen execution


# Usage

After installation, the following output will be generated.

```text
hsed script editor (simplified version)
hspcmp HSP3 code compiler
hsp3cl HSP3 command line runtime
hsp3dish HSP3Dish runtime
hsp3gp HGIMG4 runtime
```

Script Editor (Simplified Version) is a GUI application that allows you to write and execute HSP3 scripts. It has basic script editing and load / save functions. You can start it with the following command:

```bash
./hsed
```

You can run the script you are editing by pressing the `[F5]` key or selecting "Run" from the "HSP" menu. The current version uses hsp3dish as the standard runtime. The sample code corresponding to HSP3Dish is included in the sample folder. The character code of the script is treated as UTF-8. Note that this is different from the character code (SJIS) used by Windows.

For example, a simple script like this can be saved as "test.hsp" to test its behavior. This displays the text "Hello World".

```hsp
#include "hsp3cl.as"
mes "Hello World."
stop
```

If you want to know about the commands in the script, move the cursor to a word in the script editor ( for example "mes" ) and then press [F1]. Help about the instruction will be displayed.
The sample folder contains samples to test various functions; try scripting with GUI, sprites, and 3D.

If you want to execute the script from the command line, you'll need to create an HSP object file using hspcmp.

```bash
./hspcmp -d -i -u test.hsp
```

In the above example, the object file "test.ax" is generated from the "test.hsp" file. Pass this generated object file to the runtime for execution.

```bash
./hsp3cl test.ax
```

In the above example, "test.ax" is run at the HSP3 command line runtime. Similarly, you can execute scripts tailored to the runtime such as "hsp3dish" and "hsp3gp". (A GUI environment is required to execute "hsp3dish" and "hsp3gp".)

The Raspberry Pi version runs in full screen. To interrupt the execution, press [ctrl] + [C] or [esc]. Please note that you may not be able to interrupt if the keyboard is not recognized correctly. In addition to the GUI editor, it is also possible to execute in the form of "./hsp3dish ****.ax" from the command line.


# Copyright and license

HSP3 for Linux / Raspberry Pi is treated as a derivative of OpenHSP, and the license is also a modified BSD license compliant with OpenHSP / HSP3.

-------------------------------------------------------------------------------
Hot Soup Processor (HSP)
Copyright (C) 1997-2025, Onion Software/onitama, all rights reserved.
Made with base technology of OpenHSP.

OpenHSP
Copyright (C) 1997-2025, Onion Software/onitama, all rights reserved.

This software is provided by the copyright holders and contributors "as is" and
any express or implied warranties, including, but not limited to, the implied
warranties of merchantability and fitness for a particular purpose are disclaimed.

