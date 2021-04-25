# Building Samples for HybridOS Lite

This instruction assumes that you are using Ubuntu Linux 18.04/20.04 LTS.

HybridOS has two versions. One is for intricate applications with a full-featured browser. Of course the system requirement is a little higher, such as over 256M storage, and over 1G RAM. But in IoT devices, no spare resources support this version, because of the limitation of hardware. So, another version, HybridOS Lite is the best chioce.

The codes in this repositroy tell you how to program applications with HybridOS Lite.

## Features of HybridOS Lite  


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
  - `build-all.sh` the script for building all dependent library;
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


## How to run the samples  


