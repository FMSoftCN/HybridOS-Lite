#!/bin/bash

# Step 1: install some packages you maybe use
sudo apt-get install libgdk-pixbuf2.0-dev
sudo apt-get install libglib2.0-dev

# Step 2: build MiniGUI
# gvfb
git clone https://github.com/VincentWei/gvfb.git
cd gvfb
cmake .
make -j4
sudo make install
cd ..

# MiniGUI
git clone https://gitlab.fmsoft.cn/VincentWei/minigui.git -b rel-5-0
cd minigui
./autogen.sh
./configure --enable-procs --enable-compositing --enable-virtualwindow --disable-Werror --disable-static --disable-gtk-doc --enable-videodrm
make -j4
sudo make install
cd ..

# mgplus
git clone https://gitlab.fmsoft.cn/VincentWei/mgplus.git -b rel-5-0
cd mgplus
./autogen.sh
./configure
make -j4
sudo make install
cd ..

# mgeff
git clone https://gitlab.fmsoft.cn/VincentWei/mgeff.git -b rel-5-0
cd mgeff
./autogen.sh
./configure
make -j4
sudo make install
cd ..

# minigui-res
git clone https://gitlab.fmsoft.cn/VincentWei/minigui-res.git -b rel-5-0
cd minigui-res
./autogen.sh
./configure
make -j4
sudo make install
cd ..


# Step 3: build software packages
# hicairo
git clone https://gitlab.fmsoft.cn/hybridos/hicairo.git -b minigui-backend
cd hicairo
./autogen.sh
./configure
make -j4
sudo make install
cd ..

# hirsvg
git clone https://gitlab.fmsoft.cn/hybridos/hirsvg.git -b minigui-backend
cd hicairo
./autogen.sh
./configure
make -j4
sudo make install
cd ..

# hibox
git clone https://gitlab.fmsoft.cn/hybridos/hibox.git
cd hibox
cmake .
make -j4
sudo make install
cd ..

# hibus
git clone https://gitlab.fmsoft.cn/hybridos/hibus.git -b rel-1-0
cd hibus
cmake .
make -j4
sudo make install
cd ..

# hidomlayout
git clone https://gitlab.fmsoft.cn/hybridos/hidomlayout.git -b master 
cd hidomlayout
cmake .
make -j4
sudo make install
cd ..


# Step 4: Build HybridOS-lite
cmake .
make
