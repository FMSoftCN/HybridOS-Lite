# Building Samples for HybridOS Lite

This instruction assumes that you are using Ubuntu Linux 18.04/20.04 LTS.

HybridOS has a greate software stack to support its powerful functions. But for some simple device, such as IoT deivce, can not afford the consumption of resource, because of the limitation of hardware. So we select some libraries from HybridOS software stack, constitute HybridOS Lite, which is fit for those devices, which normally have only 64M RAM and 16M storage at the most.

The codes in this repositroy tell you how to program applications with HybridOS Lite.

## Features of HybridOS Lite  

- Multi-process programing mode, improve system reliability;
- Layout with CSS properties, not only for system screen, but also for elements in applications;
- HiBus is applied, it makes data driven possible, and separates GUI interaction from data logic;

## Architecture of this directory  

```
hybridos-lite/
├── build-lib
│   ├── fetch-all.sh
│   └── build-all.sh
├── src
│   ├── mginit
│   ├── wallpaper
│   ├── chgconfig
│   ├── gear
│   ├── include 
│   ├── layout
│   │     ├── manifest.json
│   │     ├── newconfig.json
│   │     ├── default_layout.css
│   │     ├── gear1.css
│   │     ├── gear2.css
│   │     ├── chgconfig1.css
│   │     └── chgconfig2.css
│   └── config
│         ├── gear1.ini
│         ├── gear2.ini
│         ├── chgconfig1.ini
│         └── chgconfig2.ini
└── cmake 
```

In the feature above, only displays the key directories and files. They are:

- `cmake/` the directory for cmake to find necessary libraries;
- `build-lib/` the directory for building dependent library;
  - `fetch-all.sh` the script for getting all source code of library;
  - `build-all.sh` the script for building all dependent libraries;
- `src/` contains source code and configure files;
  - `mginit/` The most important part of this sample. In this directory, illustrates how to parsing manifest file, how to layout in the screen, and how to control compositor. It is the core of HybridOS Lite.
  - `wallpaper/` sample code, illustrate how to build a dynamic wall paper;
  - `chgconfig/` sample code, illustrate how to change manifest files;
  - `gear/` sample code, tell you how to draw svg files with MiniGUI;
  - `config` the director contains user defined data for every application. 
  - `layout/` contains layout files;
    - `manifest.json` the main layout file for the whole screen;
    - `newconfig.json` the alternative layout file for change manifest.json;
    - `default_layout.css` contains default css for every dom element;
    - `gearx.css` css files for gear when runns with different width and height;
    - `chgconfigx.css` css files for chgconfig when runns with different width and height.

## How to building the samples  

### Prerequisites

You should run `apt install <package_name>` to install the following packages on your Ubuntu Linux.

* Building tools:
   * git
   * gcc/g++
   * binutils
   * autoconf/automake
   * libtool
   * make
   * cmake
   * pkg-config
   * gtk-doc-tools
* Dependent libraries:
   * libgtk2.0-dev
   * libjpeg-dev
   * libpng-dev (libpng12-dev on Ubuntu Linux 16.04 instead)
   * libfreetype6-dev
   * libinput-dev
   * libdrm-dev
   * libsqlite3-dev
   * libxml2-dev
   * libssl1.0-dev

You can run the following commands to install all above software packages on Ubuntu 18.04/20.04:

1. Clone this repo from websites:

```
$ cd hybridos-lite/build-lib
$ ./fetch-all.sh
```

1. Build all dependent libraries:

```
$ ./build-all.sh
$ cd ..
```

1. Build HybridOS Lite:

```
$ mkdir build
$ cd build
$ cmake ../
$ make
```

## How to run HybridOS Lite  

Two terminals are used, when runs HybridOS Lite. One is for hibusd, the other is for mginit.

In one terminal, you should type:

```
$ cd hybridos-lite/build-lib/hibus/src 
$ sudo hibusd -d
```

And type the command below in another terminal:
```
$ cd hybridos-lite/bin 
$ ./mginit
```

Then you can see the screen, just like the picutres below:

- It is chgconfig application. When you press the icons in the screen, HybridOS Lite will change a new manifest file. When you change manifest files, pay attention to the variation of indicator bar at the bottom of screen.
![SUMMER 2021](summer2021/figures/chgconfig.png)

- It is gear application. 
![SUMMER 2021](summer2021/figures/gear.png)

- When you change manifest file, the third screen is as blow. It has 16 applications.
![SUMMER 2021](summer2021/figures/multi.png)

- When you drag the screen, or press the dot in indicator bar, the screen will be switched between different layers.
![SUMMER 2021](summer2021/figures/drag.png)
