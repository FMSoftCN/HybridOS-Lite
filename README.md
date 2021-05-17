# HybridOS Lite

HybridOS Lite 是 HybridOS（合壁操作系统）的精简版本，适用于对成本敏感的嵌入式设备，可运行在基于 Linux 内核，但只有 64MB RAM 或更低配置的设备上。

本仓库包含了 HybridOS Lite 的一个典型示例应用，以及相应的构建脚本。

**目录**

- [HybridOS Lite 的架构](#hybridos-lite-的架构)
- [HybridOS Lite 的技术特点](#hybridos-lite-的技术特点)
   + [多进程模式，解耦功能](#多进程模式解耦功能)
   + [数据驱动](#数据驱动)
   + [模块重组易如反掌](#模块重组易如反掌)
   + [使用 CSS，应用界面随意调整](#使用-css应用界面随意调整)
   + [小体积，大能力](#小体积大能力)
- [目录结构](#目录结构)
- [构建](#构建)
- [运行 HybridOS Lite 示例应用](#运行-hybridos-lite-示例应用)
- [系统开销统计](#系统开销统计)
- [版权声明](#版权声明)
   + [Special Statement](#special-statement)


## HybridOS Lite 的架构

下图给出了 HybridOS Lite 的架构：

```
 ---------------------------------------------------------------------------------
| DockerBar, StatusBar,   |                  |                 |                  |
| IndicatorBar,           |     GUI app1     |     GUI app2    |    GUI app ...   |        - main windows
| DescriptionBar ...      |                  |                 |                  |____
 ---------------------------------------------------------------------------------     |
|  mginit w/ compositor   |                        Wallpaper                      |____| 
 ---------------------------------------------------------------------------------     |
|                                                                                 |    |
|                 MiniGUI, hiCairo, hiSVG, hiBox, hiBus, hiDOMLayout ...          |    | hiBus
|                                                                                 |    |
 ---------------------------------------------------------------------------------     |
|                                hibusd and user daemons                          |____|
 ---------------------------------------------------------------------------------
|                               C/C++ runtime environment                         |
 ---------------------------------------------------------------------------------
|                               Linux Kernel/Drivers                              |
 ---------------------------------------------------------------------------------
```

如上图所示，HybridOS Lite 中包含了如下 HybridOS 软件栈中的函数库或组件：

- `MiniGUI`：我们使用 MiniGUI 5.0 版本提供的多进程模式、合成图式，用于提供现代窗口系统中常见的半透明效果，以及窗口切换时的动画效果等。
   - <https://gitlab.fmsoft.cn/VincentWei/build-minigui-5.0/>
- `hiBus`：HybridOS 数据总线，用于跨进程的远程过程调用以及事件分发功能。其中，`hibusd` 是 hiBus 的守护进程，`libhibus` 为应用提供了连接和访问数据总线的接口。
   - <https://gitlab.fmsoft.cn/hybridos/hibus>
- `hiBox`：一个 C 语言工具库，集成了对常见数据结构及算法的支持，如链表、哈希表等，也集成了 JSON 的解析接口。
   - <https://gitlab.fmsoft.cn/hybridos/hibox>
- `hiCairo`：二维矢量图形库 Cairo 的 HybridOS 派生版本，用于二维矢量图形的渲染。
   - <https://gitlab.fmsoft.cn/hybridos/hicairo>
- `hiSVG`：该函数库提供了对 SVG 的解析和渲染能力。这个函数库基于 `librsvg`，但做了重构，去除了不必要的接口，去除了对 Pango 的依赖，并基于 hiDOMLayout 增强了对 CSS 的支持。
   - <https://gitlab.fmsoft.cn/hybridos/hisvg>
- `hiDOMLayout`：该函数库提供了 DOM 树的维护功能以及 CSS 样式的解析功能，并可对 DOM 树执行基于 CSS 的风格化处理和布局处理。使用这个函数库，使得非 Web 前端应用，也可以利用 CSS 样式定义界面元素的样式及布局。
   - <https://gitlab.fmsoft.cn/hybridos/hidomlayout>

有关 HybridOS Lite 上的应用开发，可进一步阅读[《HybridOS Lite 应用开发指南》](hybridos-lite-app-developing-guide.md)。

## HybridOS Lite 的技术特点

### 多进程模式，解耦模块

与传统的嵌入式开发中，开发者通常会编写一个应用程序把所有事儿都做了。而 HybridOS Lite 鼓励开发者采用多进程模式，将系统分解成若干个功能单一的独立程序，然后通过 hiBus 数据总线将这些程序对应的进程连接在一起。这样，图形界面应用不直接操作硬件，而只提供人机交互能力。这种设计，可大幅度降低系统的耦合度，尤其在底层功能不变，而界面经常变动的情形下，可大幅降低维护工作量。

这种设计是 UNIX 哲学的具体体现：每个程序都短小精悍，功能单一，但组合起来可以完成复杂的功能。

另外，这种设计还可以增加系统的稳定性。HybridOS Lite 可以监听到各个模块的工作状态，当某一模块发生异常时，HybridOS Lite 能够自动重启或者重新初始化该模块，使该模块对系统的不良影响降到最低。

在 HybrdiOS Lite 的架构中，MiniGUI 扮演了重要的角色。MiniGUI 的多进程运行模式，为模块解耦提供了技术支撑。MiniGUI 5.0 的合成图式（`compositing schema`），则是创建酷炫界面和交互能力的一大利器。开发者通过修改默认合成器（compositor）一些方法，或者创建定制的合成器，就能够灵活地控制窗口的位置、大小、层叠关系，进而在应用、屏幕等切换时，实现动画效果。最终，在嵌入式系统资源有限的情况下，我们可以获得不亚于桌面系统的用户交互体验。

### 数据驱动

数据驱动，是 HybridOS 的核心思想之一。功能解耦，界面与数据逻辑分离后，hiBus 总线负责将所有模块连接起来，使之成为一个有机的整体。与传统的消息驱动机制相比，hiBus 总线做了几点重要改进：使用 `JSON` 格式封装数据，增强数据传输的灵活性；提供不同进程之间的事件订阅与远程调用，hiBus 总线不但是数据传输的通道，还是逻辑功能的载体；提供跨平台的网络接口，打通不同设备之间、本机业务与云服务之间的壁垒。

### 模块重组易如反掌

在 HybridOS Lite 的编程思想中，功能解耦后形成若干功能模块。类似组态软件，用户可对功能模块其进行任意组合。功能模块如何组合，如何显示，则由布局文件 `manifest.json` 决定。在诸如工业控制面板、家用电器、智能门锁、智能音箱等产品中，多以屏为单位组织应用。因此 HybridOS Lite 使用了这样的应用组织方式。在布局文件中，指定了每屏所包含的应用、各个应用的位置、应用之间的通信关系等。同时提供了动态切换布局的机制，使得同一产品能轻易展示出不同的系统样貌。

### 使用 CSS，应用界面随意调整

传统编程方式中，界面元素的调整，无外乎两种方式：第一，修改代码、重新编译；第二，写一个私有的配置文件，指定界面元素的属性。但是这两种方式，在 HybridOS Lite 中得到了彻底改变。HybridOS Lite 借鉴了 Web 前端开发中的 CSS 规范，利用其指定界面元素的诸多属性。由于有 CSS 规范可循，任一了解 CSS 规范的开发者，都可以在不了解渲染逻辑的情况下，通过修改 CSS 文件对界面元素进行修改，而非修改源代码。有规可循，极大地提高了代码迭代速度，降低了代码维护难度，也降低了对开发者的要求。

### 小体积，大能力

文后的[系统开销统计](#系统开销统计) 给出了 HybridOS Lite 函数库大小以及运行 20 个应用进程情况下的系统资源占用情况。整体上，HybridOS Lite 可在拥有 64MB Flash 和 64MB RAM （或更低）的系统中运行。

## 目录结构

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

如上图所示，显示了仓库中的主要目录，它们是：

- `cmake/` ：包含 `Find<Package>.cmake` 文件，用于检查软件包的依赖性及获得编译参数；
- `build-lib/`：包含了编译依赖库的脚本文件；
  - `fetch-all.sh`：获得依赖库脚本；
  - `build-all.sh`：编译依赖库脚本；
- `src/`：包含应用程序和配置文件；
  - `mginit/`：`mginit` 程序。展示了如何解析 `manifest.json` 文件，如何布局以及如何使用合成器，是 HybridOS Lite 的关键组件；
  - `wallpaper/`：动态壁纸应用；
  - `chgconfig/`：动态切换配置文件应用程序；
  - `svgshow/`：展示在应用程序中如何使用 SVG 图片；
  - `config`：包含了用户自定义的、与业务相关的数据文件；
  - `layout/`：包含了布局文件；
    - `manifest.json`：最重要的布局文件，其定义了每屏显示哪些应用程序，以及这些应用程序的布局；
    - `newconfig.json`：另外一个布局文件，用于展示如何动态切换布局；
    - `default_layout.css`：默认的 CSS 文件。其为每个重要的 `dom` 元素设置了默认的布局样式；
    - `svgshowx.css`：`svgshow` 应用的 CSS 文件。同一个应用显示在屏幕不同位置，大小不同时，需要不同的布局文件来指定各个元素的位置与大小；
    - `chgconfigx.css`：`chgconfig` 应用的 CSS 文件。

## 在 PC 上构建开发环境

我们假设您使用的是 Ubuntu Linux 18.04/20.04 LTS。

在 Ubuntu Linux 中，使用 `apt install <package_name>` 命令安装下面的软件包：

* 开发工具：
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
   * libpng-dev
   * libfreetype6-dev
   * libinput-dev
   * libdrm-dev
   * libsqlite3-dev
   * libxml2-dev
   * libssl1.0-dev

在 `Ubuntu 18.04/20.04` 下，可使用下面的命令，编译并安装 'HybridOS Lite`：

1. 获得源代码

```shell
$ cd hybridos-lite/build-lib
$ ./fetch-all.sh
```

1. 编译 HybridOS Lite 依赖库

```shell
$ ./build-all.sh
$ cd ..
```

1. 编译 HybridOS Lite

```shell
$ mkdir build
$ cd build
$ cmake ../
$ make
```

## 运行 HybridOS Lite 示例应用

运行 HybridOS Lite，需要两个终端。一个用来执行 `hibusd`，一个用来执行 `mginit`。

打开一个终端，执行下面的命令：

```shell
$ cd hybridos-lite/build-lib/hibus/src 
$ sudo hibusd -d
```

在另外一个终端，执行下面的命令：

```shell
$ cd hybridos-lite/bin 
$ ./mginit
```

在屏幕上，将出现如下的窗口

- `chgconfig` 应用。当点击应用中的图标后，HybridOS Lite 将动态切换布局文件。当动态切换布局文件时，可注意屏幕下方的 `indicator bar` 的变化。
![SUMMER 2021](summer2021/figures/chgconfig.png)

- `svgshow` 应用。 
![SUMMER 2021](summer2021/figures/gear.png)

- 当动态切换布局文件时，有一屏将同时显示16个应用。如下图：
![SUMMER 2021](summer2021/figures/multi.png)

- 用鼠标在屏幕上拖动，或者点击屏幕下方的 `indicator bar` 时，屏幕将在不同层间切换。
![SUMMER 2021](summer2021/figures/drag.png)

## 系统开销统计

下面是对 HybridOS Lite 主要依赖库的统计（除系统 C/C++ 库之外，ARM A8 架构）：

```

libffi.so                31 KB
libglib-2.0.so         1044 KB
libgio-2.0.so          1400 KB
libgmodule-2.0.so        14 KB
libgobject-2.0.so       302 KB
libgthread-2.0.so         5 KB
libgomp.so              162 KB

// essential libraries for MiniGUI
libjpeg.so              305 KB
libz.so                 100 KB
libpng16.so             170 KB
libpixman-1.so          575 KB
libharfbuzz.so          839 KB
libharfbuzz-subset.so   112 KB
libfreetype.so          530 KB
libfontconfig.so        219 KB
libminigui_procs.so    2724 KB
libmgeff.so              73 KB

// essential libraries for HybridOS Lite
libhicairo.so           794 KB
libhibox.so             109 KB
libhibus.so              39 KB
libhidomlayout.so       814 KB
libhisvg.so             257 KB
```

这些函数库大致体积为 10 MB。也即除去操作系统启动代码、内核、内核模块，以及系统启动必需的基础库外，HybridOS Lite 全部依赖库仅占用 10 MB 左右的存储空间。如果考虑使用中文 Truetype 字体，则还需大概 10 MB 左右存储空间（视字体大小而定）。

下面是对本例代码，交叉编译为 ARM 版本，在运行前对后内存使用状况的统计。在本例中，HybridOS Lite 共启动了 20 个应用，其中包含：mginit（必须）、hibusd（必须），以及 18 个用户应用进程：

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

从数据可以看出，目标开发板一共拥有 `128 MB RAM`。HybridOS Lite 启动 20 个应用之后，一共使用了 54 MB 内存，每个应用大致占用 2.5MB RAM 空间。通过 `top` 命令查询各个应用的 CPU 占用率，其中`mginit` 和 `hibusd` 一共占用 CPU 百分比为 1%。

## 版权声明

Copyright (C) 2021 Beijing FMSoft Technologies Co., Ltd.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

### Special Statement

The above open source or free software license does
not apply to any entity in the Exception List published by
Beijing FMSoft Technologies Co., Ltd.

If you are or the entity you represent is listed in the Exception List,
the above open source or free software license does not apply to you
or the entity you represent. Regardless of the purpose, you should not
use the software in any way whatsoever, including but not limited to
downloading, viewing, copying, distributing, compiling, and running.
If you have already downloaded it, you MUST destroy all of its copies.

The Exception List is published by FMSoft and may be updated
from time to time. For more information, please see
<https://www.fmsoft.cn/exception-list>.

Also note that the copyright of the resource assets included in the
demos might be reserved by the copyright owner. For more information,
please see the README files in the sub directories.
