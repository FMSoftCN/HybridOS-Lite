# HybridOS Lite 简介

【主题】HybridOS Lite 简介  
【摘要】本文对 `HybridOS Lite` 的架构、代码目录、编译方法，进行了简要的介绍  
【版本】1.0  
【作者】耿岳  
【日期】2021 年 05 月  
【状态】初稿  

**目录**

- [HybridOS Lite 的技术特点](#hybridos-Lite-的技术特点)
- [HybridOS Lite 的架构](#hybridos-Lite-的架构)
- [代码的的目录结构](#代码的目录结构)
- [编译步骤](#编译步骤)
- [运行 HybridOS Lite](#运行-HybridOS-Lite)
- [HybridOS Lite 编程介绍](#HybridOS-Lite-编程介绍)
- [版权声明](#版权声明)


`HybridOS` 拥有丰富的功能，也就具有了庞大的软件栈。但是对于诸如 `IoT` 这样的嵌入式设备而言，通常情况下最多只有 64 MB RAM，及 64 MB 存储空间。由于硬件条件的限制，其无法负担 `HybridOS` 的资源开销。为了能在这些设备中使用 `HybridOS`，我们在 `HybridOS` 软件栈中，挑选了一些体积小、适用范围广的软件库，形成了 `HybridOS Lite`，从而特别适用于对成本敏感的嵌入式设备。下面对 `HybridOS Lite` 做一个简单的介绍。

## HybridOS Lite 的技术特点  

- 更小的体积，更少的系统开销：

下面是对 `HybridOS Lite` 主要依赖库的统计（`arm` 版本）：

``` bash
// for openssl
libcrypto.so           2083 KB         // 能否去掉 hibusd 中对 openssl的依赖
libssl.so               457 KB

// for libpcre
libpcre.so              108 KB
libpcreposix.so          10 KB
libpcrecpp.so            30 KB

// for libgerror
libgpg-error.so         113 KB
libgcrypt.so            879 KB
libgfortran.so          855 KB

// libffi for gobject
libffi.so                31 KB

// for glib
libglib-2.0.so         1044 KB
libgio-2.0.so          1400 KB
libgmodule-2.0.so        14 KB
libgobject-2.0.so       302 KB
libgthread-2.0.so         5 KB
libgomp.so              162 KB

// for libinput
libusb-1.0.so            96 KB
libmtdev.so              18 KB
libkmod.so               76 KB
libevdev.so              67 KB
libudev.so              117 KB
libinput.so             335 KB

// essential libraries
libjpeg.so              305 KB
libz.so                 100 KB
libpng16.so             170 KB

// essential libraries for MiniGUI
libxml2.so             1221 KB
libpixman-1.so          575 KB
libexslt.so              76 KB
libxslt.so              224 KB
libharfbuzz.so          839 KB
libharfbuzz-subset.so   112 KB
libfreetype.so          530 KB
libfontconfig.so        219 KB
libminigui_procs.so    2724 KB
libmgeff.so              73 KB

// essential libraries for HybridOS Lite
libhicairo.so           794 KB
libhicairo-script-interpreter.so    102 KB
libhicairo-gobject.so    26 KB
libhibox.so             109 KB
libhibus.so              39 KB
libhidomlayout.so       814 KB
libhisvg.so             257 KB
```

以上全部统计为 `15 MB`。也即除去操作系统启动代码、内核，以及系统启动必需的基础库外，`HybridOS Lite` 全部依赖库占用 `15 MB` 左右的存储空间。如果考虑使用中文 `Truetype` 字体，则还需大概 `10 MB` 左右存储空间（视字体大小而定）。

下面是对本例代码，交叉编译为 `ARM` 版本，在运行前对后内存使用状况的统计。在本例中，`HybridOS Lite` 共启动了 20 个应用，其中包含：mginit（必须）、hibusd（必须），以及 18 个用户应用：

``` bash
// 启动前
MemTotal:         110744 kB
MemFree:           89292 kB
MemAvailable:      91720 kB

// 启动后
Memotal:          110744 kB
MemFree:           35520 KB
MemvaIlable:       54248 kB
```

从数据可以看出，目标板一共拥有 `128 MB RAM`。`HybridOS Lite` 启动了 `20` 个应用，一共使用了 `54 MB` 内存。然后通过 `top` 命令查询各个应用的 `CPU` 占用率，其中`mginit` 和 `hibusd` 一共占用 `CPU` 百分比为 `1%`。剩余均为空闲，或者应用程序使用。

- 功能解耦，多进程的编程模式：

与传统嵌入式大进程、多线程的编程方式相比，`HybridOS Lite` 采用了多进程的编程模式。将系统需求分解成若干个功能单一的独立模块，每个模块用一个单独进程实现。由于降低了系统的耦合度，从而减少了各个功能之间的干涉，增加了系统的稳定性。`HybridOS Lite` 还可以监听到各个模块的工作状态，当某一模块发生异常时，`HybridOS Lite` 能够自动重启或者重新初始化该模块，使该模块对系统的不良影响降到最低；

- 数据驱动，跨越平台的 `HiBus` 总线：

数据驱动，是 `HybridOS Lite` 的核心思想之一。功能解耦，界面与数据逻辑分离后，`hibus` 总线负责将所有模块连接起来，使之成为一个有机的整体。与传统的消息驱动机制相比，`hibus` 总线做了几点重要改进：使用 `Json` 格式数据传递，增强数据传输的灵活性；提供不同进程之间的事件订阅与远程调用，`hibus` 总线不但是数据传输的通道，还是逻辑功能的载体；提供跨平台的网络接口，打通不同设备之间、本机业务与云服务之间的壁垒，为嵌入式设备参与云服务，提供了有效的技术支持；

- 布局文件，模块组合易如反掌：

在 `HybridOS Lite` 的编程思想中，功能解耦后形成若干功能模块。类似组态软件，用户可对功能模块其进行任意组合。功能模块如何组合，如何显示，则由布局文件 `manifest.json` 决定。在诸如工业控制面板、家用电器、智能门锁、智能音箱等产品中，多以屏为单位组织应用。因此 `HybridOS Lite` 使用了这样的应用组织方式。在布局文件中，指定了每屏所包含的应用、各个应用的位置、应用之间的通信关系等。同时提供了动态切换布局的机制，使得同一产品能轻易展示出不同的系统样貌；

- 使用 `CSS`，应用界面随意调整：

传统编程方式中，界面元素的调整，无外乎两种方式：修改代码、重新编译；写一个私有的配置文件，指定界面元素的属性。但是这两种方式，在 `HybridOS Lite` 中得到了彻底改变。`HybridOS Lite` 借鉴了 `Web` 开发中的 `CSS` 规范，利用其指定界面元素的诸多属性。由于有 `CSS` 规范可循，任一了解 `CSS` 规范的开发者，都可以在不了解渲染逻辑的情况下，通过修改 `CSS` 文件对界面元素进行修改，而非修改源代码。有规可循，极大的提高了代码迭代速度，降低了代码维护难度，也降低了对二次开发者的要求；

- 博采众长，`MiniGUI` 的扩展与创新：

`MiniGUI` 做为国内最成功的多窗口图形系统，此次推出 `HybridOS Lite`，不但继承了原有的多窗口管理系统，还在绘制中引入了 `cairo` 矢量图形库接口。使得开发者在编写 `MiniGUI API` 程序的时候，依然能够使用 `Cairo API`，极大的丰富了开发者的编程手段。与此同时，`MiniGUI 5.0` 版本推出了合成图式（`compositing schema`），这是增强界面用户友好度的一大利器。开发者通过修改默认合成器(`compositor`)的属性，或者创建自己的合成器，能够灵活的控制主窗口的位置、大小、层叠关系，进而在应用、屏幕等切换时，实现动画效果。在嵌入式系统资源有限的情况下，产生不亚于 `PC` 桌面系统的用户体验。


## HybridOS Lite 的构架  

下图给出了 `HybridOS Lite` 的架构：

```
 ---------------------------------------------------------------------------------
| DockerBar, StatusBar,   |                  |                 |                  |
| IndicatorBar,           |     GUI app1     |     GUI app2    |    GUI app ...   |        - main windows
| DescriptionBar ...      |                  |                 |                  |____
 ---------------------------------------------------------------------------------     |
|  mginit w/ compositor   |                        Wallpaper                      |____| 
 ---------------------------------------------------------------------------------     |
|                                                                                 |    |
|                 MiniGUI, hiCairo, hisvg, hibox, hibus, hidomlayout ...          |    | hiBus
|                                                                                 |    |
 ---------------------------------------------------------------------------------     |
|                                hibusd and user daemons                          |____|
 ---------------------------------------------------------------------------------
|                               C/C++ runtime environment                         |
 ---------------------------------------------------------------------------------
|                               Linux Kernel/Drivers                              |
 ---------------------------------------------------------------------------------
```

其中核心的应用和库，解释说明如下：
- `mginit`：`MiniGUI` 多进程版的核心程序，其负责多窗口系统的管理与消息的分发，各个层的创建与切换、合成器的管理与操作。同时提供了多个公用 `bar`，方便用户操作；
- `hibusd`：`hibus` 守护进程，负责 `hibus` 总线上数据的传输与分发；
- `hibox`：提供了对 `json` 格式文本的解析处理；
- `hibus`：向应用程序提供 `hibus` 总线功能，包括：事件的订阅与发送、远程过程调用的请求与执行结果的返回；
- `hicairo`：用于图形元素的渲染。该库是对基础软件库 `libcairo` 的扩展，添加了 `MiniGUI` 的接口，可以在 `MiniGUI` 应用中直接使用 `Cairo API` 进行渲染； 
- `hisvg`：用于 `svg` 图像元素的渲染。该库是对基础软件库 `librsvg` 的重构，去掉了诸如 `pango` 等不需要的部分，增添了对 `CSS` 的支持； 
- `hidomlayout`：用于多窗口的布局，以及同一窗口中图形、图像元素的布局处理。通过对 `CSS` 样式的解析，获得各个窗口及界面元素的大小、位置、字体等诸多属性。其使非浏览器应用，能够利用 `CSS` 样式，对界面进行布局处理。


## 代码的目录结构  

```
hybridos-lite/
├── build-lib
│   ├── fetch-all.sh
│   └── build-all.sh
├── src
│   ├── mginit
│   ├── wallpaper
│   ├── chgconfig
│   ├── svgshow 
│   ├── include 
│   ├── layout
│   │     ├── manifest.json
│   │     ├── newconfig.json
│   │     ├── default_layout.css
│   │     ├── svgshow1.css
│   │     ├── svgshow2.css
│   │     ├── chgconfig1.css
│   │     └── chgconfig2.css
│   └── config
│         ├── svgshow1.ini
│         ├── svgshow2.ini
│         ├── chgconfig1.ini
│         └── chgconfig2.ini
└── cmake 
```

如上图所示，显示了仓库中的主要目录，它们的是：

- `cmake/` ：包含 `Find<Package>.cmake` 文件，用于检查软件包的依赖性及获得编译参数；
- `build-lib/`：包含了编译依赖库的脚本文件；
  - `fetch-all.sh`：获得依赖库脚本；
  - `build-all.sh`：编译依赖库脚本；
- `src/`：包含应用程序和配置文件；
  - `mginit/`：`mginit` 程序。展示了如何解析 `manifest.json` 文件，如何布局以及如何使用合成器，是 `HybridOS Lite` 的关键组件；
  - `wallpaper/`：动态壁纸应用；
  - `chgconfig/`：动态切换配置文件应用程序；
  - `svgshow/`：展示在应用程序中如何使用 `svg` 图片；
  - `config`：包含了用户自定义的、与业务相关的数据文件；
  - `layout/`：包含了布局文件；
    - `manifest.json`：最重要的布局文件，其定义了每屏显示哪些应用程序，以及这些应用程序的布局；
    - `newconfig.json`：另外一个布局文件，用于展示如何动态切换布局；
    - `default_layout.css`：默认的 `CSS` 文件。其为每个重要的 `dom` 元素设置了默认的布局样式；
    - `svgshowx.css`：`svgshow` 应用的 `CSS` 文件。同一个应用显示在屏幕不同位置，大小不同时，需要不同的布局文件来指定各个元素的位置与大小；
    - `chgconfigx.css`：`chgconfig` 应用的 `CSS` 文件。

## 编译步骤  

### 前置条件

本文档假设用户使用的是 `Ubuntu Linux 18.04/20.04 LTS`。

在 `Ubuntu Linux` 中，用户使用 `apt install <package_name>` 命令，用以安装下面的软件包。

* 编译工具：
   * git
   * gcc/g++
   * binutils
   * autoconf/automake
   * libtool
   * make
   * cmake
   * pkg-config
   * gtk-doc-tools
* 依赖库：
   * libgtk2.0-dev
   * libjpeg-dev
   * libpng-dev (libpng12-dev on Ubuntu Linux 16.04 instead)
   * libfreetype6-dev
   * libinput-dev
   * libdrm-dev
   * libsqlite3-dev
   * libxml2-dev
   * libssl1.0-dev

在 `Ubuntu 18.04/20.04` 下，可使用下面的命令，编译并安装 'HybridOS Lite`：

1. 获得源代码

```
$ cd hybridos-lite/build-lib
$ ./fetch-all.sh
```

1. 编译 `HybridOS Lite` 依赖库

```
$ ./build-all.sh
$ cd ..
```

1. 编译 `HybridOS Lite`

```
$ mkdir build
$ cd build
$ cmake ../
$ make
```

## 运行 HybridOS Lite  

运行 `HybridOS Lite`，需要两个终端。一个用来执行 `hibusd`，一个用来执行 `mginit`。

打开一个终端，执行下面的命令：

```
$ cd hybridos-lite/build-lib/hibus/src 
$ sudo hibusd -d
```

在另外一个终端，执行下面的命令：
```
$ cd hybridos-lite/bin 
$ ./mginit
```

在屏幕上，将出现如下的窗口

- `chgconfig` 应用。当点击应用中的图标后，`HybridOS Lite` 将动态切换布局文件。当动态切换布局文件时，可注意屏幕下方的 `indicator bar` 的变化。
![SUMMER 2021](summer2021/figures/chgconfig.png)

- `svgshow` 应用。 
![SUMMER 2021](summer2021/figures/gear.png)

- 当动态切换布局文件时，有一屏将同时显示16个应用。如下图：
![SUMMER 2021](summer2021/figures/multi.png)

- 用鼠标在屏幕上拖动，或者点击屏幕下方的 `indicator bar` 时，屏幕将在不同层间切换。
![SUMMER 2021](summer2021/figures/drag.png)

## HybridOS Lite 编程介绍

在 `HybridOS Lite` 中，如何编程，请看《[HybridOS Lite 编程介绍](program-introduce.md)》

## 版权声明

`HybridOS Lite` 中的代码遵循 "Apache License, Version 2.0"
