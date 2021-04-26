# 开源软件供应链点亮计划-暑期 2021  

本文是合璧操作系统社区参与中科院软件所和 openEuler 社区共同主办发起的“开源软件供应链点亮计划-暑期 2021”活动的项目说明。  

![SUMMER 2021](figures/summer2021.png)


若希望参与项目开发，请访问如下链接获得报名指南:  

<https://summer.iscas.ac.cn/help/student/>  

## 合璧操作系统设备端数据总线 hiBus 的增强-在客户端函数库中实现 WebSocket 的支持  

【项目标题】合璧操作系统设备端数据总线 hiBus 的增强-在客户端函数库中实现 WebSocket 的支持  
  
【项目描述】hiBus 是合璧操作系统提出的一个设备端数据总线技术，通过 hiBus 本机或者远程主机上的程序可以发起远程过程调用或者订阅特定事件。  

hiBus 为客户端提供了 Unix Domain Socket 和 WebSocket 两种连接方式。  

目前，hiBus 已完成 1.0 版本的开发。现有如下增强需求：  
- 在客户端函数库中实现 WebSocket 的支持。hiBus 服务器已实现对 WebSocket 的支持，可通过支持 WebSocket 的浏览器直接连入 hiBus 服务器。但客户端函数库中尚未完成对 WebSocket 的支持，希望完成相关实现。  

【项目难度】中  

【项目社区导师】魏永明  

【导师联系方式】vincent@minigui.org  

【项目产出要求】  

- 客户端函数可支持 WebSocket  

  在 libhibus.so 函数库中，使用 C 代码增加对 WebSocket 的支持。  

  针对远程主机连接，需要考虑如下主机名称相关的事宜：  

  - 如何定义 hiBus 服务器所在主机的主机名。  
  - 枚举已连接到 hiBus 的客户端主机，实现为内置行者的一个方法。  

【项目技术要求】  
- Linux开发环境及 C/C++ 编程
- MiniGUI 
- Unix domain socket   

【相关的开源软件仓库列表】  
- 仓库：<https://github.com/FMSoftCN/hibus>
- hiBus 协议：<https://github.com/FMSoftCN/hybridos/blob/master/docs/design/hybridos-data-bus-zh.md>  

## 合璧操作系统设备端数据总线 hiBus 的增强-执行本机或远程命令  
  
【项目标题】合璧操作系统设备端数据总线 hiBus 的增强 -执行本机或远程命令  
 
【项目描述】hiBus 是合璧操作系统提出的一个设备端数据总线技术，通过 hiBus 本机或者远程主机上的程序可以发起远程过程调用或者订阅特定事件。  
  
hiBus 为客户端提供了 Unix Domain Socket 和 WebSocket 两种连接方式。  
  
目前，hiBus 已完成 1.0 版本的开发。现有如下增强需求：  
  
- 增强 hiBus 服务器，使之可以动态装载一个内建行者，并开发一个 hiBus 内建行者，提供类似 BusyBox 的命令执行能力，从而可通过 hiBus 由本机或者远程的行者执行命令，并获得命令的输出结果。比如通过执行 ls *.c 获得当前路径下的所有 C 文件列表。  

【项目难度】中  

【项目社区导师】薛淑明  

【导师联系方式】xueshuming@minigui.org  

【项目产出要求】  
- 定义 hiBus 服务器端内置行者的接口  

  以操作函数指针的形式，定义 hiBus 服务器端动态装载内置行者的接口，该接口可用于注册一个不同于 builtin 的内置行者，并在 hiBus 中注册该行者对应的方法以及泡泡。  

- 将 BusyBox 的功能重构为 busybox 内置行者的方法  

  该行者应以动态库的形式实现，并在 hiBus 启动时装载。该行者可以以远程过程的方式执行 busybox 提供的命令行功能。注意如下要点：  

  1. 设计对应的远程过程调用和/或事件的接口。
  1. 以客户端行者为单位，维护一个上下文信息，主要用来维护当前路径等 Shell 常见的环境变量。
  1. 所有的命令，在 hiBus 子进程中执行。子进程应切换 euid（有效用户标识符）到调用者的用户标识符；对远程主机上的行者，使用 guest 用户作为其 euid。
  1. 命令行支持支持重定向和管道。
  1. 所有命令的标准输出和标准错误，在没有被重定向的前提下，作为该过程的返回值返回给调用者。  

【项目技术要求】  

- Linux开发环境及 C/C++ 编程
- MiniGUI 
- Unix domain socket  

【相关的开源软件仓库列表】  

- 仓库：<https://github.com/FMSoftCN/hibus>
-  hiBus 协议：<https://github.com/FMSoftCN/hybridos/blob/master/docs/design/hybridos-data-bus-zh.md>  

## 合璧操作系统设备端应用通过数据总线 hiBus 操作硬件设备  
  
【项目标题】合璧操作系统设备端应用通过数据总线 hiBus 操作硬件设备  
 
【项目描述】合璧操作系统是数据驱动的，而hiBus 是合璧操作系统提出的一个设备端数据总线技术。用户界面通过 hiBus 总线，与守护进程通信，发送指令。守护进程操作硬件设备。  
  
用户交互与设备操控的分离，是合璧操作系统的基本编程思想。而 hibus 总线，则将两部分有效组织起来，完成预定的功能。  
  
【项目难度】中  

【项目社区导师】耿岳  

【导师联系方式】gengyue@minigui.org  

【项目产出要求】  
- 明确硬件设备的操作，设定硬件操作的预期行为，并编写操作硬件设备的守护进程  

  开发者根据自己的硬件设备，明确操作设备的行为。比如 LED 的开关，或者是 GPIO 的高低。然后编写该操控该硬件的守护进程。  

- 定义 hiBus 数据传输协议 

  使用 hibus 总线，通过用户交互界面，操作硬件设备，可以通过发送事件，或者远程过程调用的方式完成。具体使用哪种方式，由开发者自行决定。但无论哪种方式，必须首先明确 hibus 中的数据传输协议。

  hibus 的数据传输协议，必须符合 hibus 规范。  

- 用户交互界面应用（行者）

  通过用户交互界面（基于 MiniGUI 的进程），可操控具体的硬件设备。并且在用户界面上，能用图像、图形方式，客观反映出硬件状态的变化。

【项目技术要求】  

- Linux开发环境及 C/C++ 编程
- MiniGUI 
- hibus 协议  

【相关的开源软件仓库列表】  

- 仓库：<https://github.com/FMSoftCN/hibus>
-  hiBus 协议：<https://github.com/FMSoftCN/hybridos/blob/master/docs/design/hybridos-data-bus-zh.md>  

## 合璧操作系统设备端，硬件设备通过 hiBus 总线与用户界面通信  
  
【项目标题】合璧操作系统设备端，硬件设备通过 hiBus 总线与用户界面通信  
 
【项目描述】合璧操作系统是数据驱动的，而hiBus 是合璧操作系统提出的一个设备端数据总线技术。当守护进程侦知硬件设备状态发生变化时，通过 hiBus 总线向用户界面发送数据。用户界面能及时发应硬件状态的变化。  
  
用户交互与设备操控的分离，是合璧操作系统的基本编程思想。而 hibus 总线，则将两部分有效组织起来，完成预定的功能。  
  
【项目难度】中  

【项目社区导师】耿岳  

【导师联系方式】gengyue@minigui.org  

【项目产出要求】  
- 明确硬件设备的操作，编写硬件设备操作的守护进程

  开发者根据自己的硬件设备，明确操作设备的行为。比如开发板上的按键，或者是通信串口的输入数据。然后编写该操控该硬件的守护进程。

- 定义 hiBus 数据传输协议 

  硬件设备状态的变化，通过 hibus 总线，发往用户界面。可以通过发送事件，或者远程过程调用的方式完成。具体使用哪种方式，由开发者自行决定。但无论哪种方式，必须首先明确 hibus 中的数据传输协议。

  hibus 的数据传输协议，必须符合 hibus 规范。  

- 用户交互界面应用（行者）

  通过用户交互界面（基于 MiniGUI 的进程），能用图像、图形的方式，客观反映出硬件状态的变化。

【项目技术要求】  

- Linux开发环境及 C/C++ 编程
- MiniGUI 
- hibus 协议  

【相关的开源软件仓库列表】  

- 仓库：<https://github.com/FMSoftCN/hibus>
-  hiBus 协议：<https://github.com/FMSoftCN/hybridos/blob/master/docs/design/hybridos-data-bus-zh.md>  

## 合璧操作系统设备端基于rsvg技术的动画实现  
  
【项目标题】合璧操作系统设备端基于rsvg技术的动画实现  
 
【项目描述】在合璧操作系统中，整合了 rsvg 矢量图形技术，消除了图像显示的毛边、锯齿，提供了更好的用户体验。利用 rsvg 和 MiniGUI 提供的 MGEFF_ANIMATION 类型，完成 rsvg 图片的动画效果。  
  
【项目难度】中  

【项目社区导师】薛淑明  

【导师联系方式】xueshuming@minigui.org  

【项目产出要求】  
- 使用多个 rsvg 图片，分别完成旋转、平移、缩放三种动画效果。 

- 使用 mgeff 所提供的 MGEFF_ANIMATION 类型及相关操作，用以实现各帧之间的时间间隔处理。  

- 两个 MiniGUI 应用程序，完成相同的动画效果，分别使用 MGEFF_ANIMATION 的同步、异步操作完成。  

【项目技术要求】  

- Linux开发环境及 C/C++ 编程
- MiniGUI 
- meeff 
- hirsvg 

【相关的开源软件仓库列表】  
   - <https://gitlab.fmsoft.cn/VincentWei/minigui.git -b rel-5-0>  
   - <https://gitlab.fmsoft.cn/VincentWei/mgeff.git -b rel-5-0>
   - <https://gitlab.fmsoft.cn/hybridos/hicairo.git -b minigui-backend>
   - <https://gitlab.fmsoft.cn/hybridos/hirsvg.git -b minigui-backend>

## HybridOS 图形栈增强

【项目标题】HybridOS 图形栈增强

【项目描述】HybridOS 是飞漫软件正在开发中的针对物联网的新一代操作系统。飞漫软件已基于 MiniGUI、Mesa、Cairo 等开源软件发布了 HybridOS 图形栈。在 Mesa 的 MiniGUI Backend 实现中，目前未提供对 EGL pixel buffer 的支持。本项目要求在现有 Mesa MiniGUI backend 的实现基础上，提供对 EGL pixel buffer 的支持。

【项目难度】中

【项目社区导师】魏永明

【导师联系方式】vincent@minigui.org

【项目产出要求】
   - 在现有 Mesa MiniGUI backend 的实现基础上，提供符合 EGL 规范 pixel buffer 相关接口实现。
   - 在 [hiMesa](https://github.com/FMSoftCN/himesa) 基础上完成开发工作。
   - 继承 hiMesa 使用的 MIT 许可证。

【项目技术要求】
   - Linux 环境下的 C/C++ 编程
   - MiniGUI、Mesa 等软件

【相关的开源软件仓库列表】
   - <https://github.com/VincentWei/build-minigui-5.0>
   - <https://github.com/VincentWei/minigui>
   - <https://github.com/FMSoftCN/himesa>  

## 将 MiniGUI 5.0 运行到 RT-Thread 开发板上  

【项目标题】将 MiniGUI 5.0 运行到 RT-Thread 开发板上  

【项目描述】将 MiniGUI 5.0 (Standalone 或者 Threads 模式）移植到运行 RT-Thread 的开发板上。需要开发针对具体开发板的图形引擎、输入引擎等。  

【项目难度】中  

【项目社区导师】薛淑明 

【导师联系方式】xueshuming@minigui.com  

【项目产出要求】  

- 针对具体开发板的图形引擎及输入引擎。
- 运行 MiniGUI、mGNCS、mGNCS4Touch 示例程序。
- 如果内存配置允许，运行 mg-demos 中两个演示程序。
- 推荐使用 Apache 2.0 许可证。  

【项目技术要求】  

- RT-Thread 开发环境及 C/C++ 编程
- MiniGUI  

【相关的开源软件仓库列表】  

- <https://github.com/VincentWei/build-minigui-5.0>
- <https://github.com/VincentWei/minigui>
- <https://github.com/VincentWei/mgncs>
- <https://github.com/VincentWei/mgncs4touch>
- <https://github.com/VincentWei/mg-demos>  

## MiniGUI 的 DRM 加速图形引擎

【项目标题】MiniGUI 的 DRM 加速图形引擎

【项目描述】DRM 已经成为 Linux 环境中新一代的现代图形支持框架。自 4.0 版本以来，MiniGUI 支持 DRM，但目前只提供了一个针对早期 Intel i915 芯片的加速图形引擎。该项目要求您为某个支持 DRM 且包含基础 2D 加速能力的 GPU 开发一个 MiniGUI 的 DRM 加速图形引擎。该 GPU 可以是 PC 显卡，也可以是嵌入式 SoC，如全志、瑞芯或者展讯的 SoC。

【项目难度】中

【项目社区导师】魏永明

【导师联系方式】vincent@minigui.org

【项目产出要求】
   - 符合 MiniGUI 5.0 DRM 引擎接口的加速引擎，可编译成动态库供 MiniGUI 在运行时动态装载。
   - 提供对显存管理、矩形填充、位块传输（Blitting）等的加速支持。
   - 在 [hiDRMDrier](https://github.com/FMSoftCN/hidrmdrivers) 基础上完成开发工作。
   - 继承 hiDRMDriver 使用的 MIT 许可证。

【项目技术要求】
   - Linux 环境下的 C/C++ 编程
   - MiniGUI、Mesa 等软件

【相关的开源软件仓库列表】
   - <https://github.com/VincentWei/build-minigui-5.0>
   - <https://github.com/VincentWei/minigui>
   - <https://github.com/FMSoftCN/hidrmdrivers>
   - <https://github.com/FMSoftCN/himesa>
  
## 面向小屏幕智能设备的 MiniGUI 定制合成器  

【项目标题】面向小屏幕智能设备的 MiniGUI 合成器  

【项目描述】自 MiniGUI 5.0 起，MiniGUI 的多进程模式开始支持合成图式（compositing schema）。合成图式是现代桌面操作系统和智能手机操作系统的图形及窗口系统使用的技术，其基本原理很简单：系统中所有进程创建的每一个窗口都使用独立的缓冲区来各自渲染其内容，而系统中有一个扮演合成器（compositor）角色的进程，负责将这些内容根据窗口的 Z 序以及叠加效果（如半透明、模糊等）合成在一起并最终显示在屏幕上。本项目要求为 MiniGUI 5.0 的合成图式开发一个适合于小屏幕智能设备的定制合成器，实现自定义的窗口层叠效果（如阴影、模糊、不规则窗口等），以及窗口的切换动画等效果。

【项目难度】高  

【项目社区导师】耿岳  

【导师联系方式】gengyue@minigui.org  

【项目产出要求】
   - 可在 Ubuntu 18.04 上编译运行，使用 HybridOS 图形栈中的 Cairo 或者 Mesa (OpenGL ES）作为渲染引擎，实现一个定制的 MiniGUI 5.0 合成器，可参考效果：
     - 智能手机。
     - 中高档轿车中的车载娱乐系统。
   - 可整合 mgdemo、mguxdemo 等 MiniGUI 已有示例程序或者演示程序中的代码。注：这些代码均以 Apache 2.0 许可证发布。
   - 推荐使用 Apache 2.0 许可证。

【项目技术要求】
   - Linux 环境下的 C/C++ 编程
   - OpenGL ES 编程
   - MiniGUI、Cairo、Mesa 等软件

【相关的开源软件仓库列表】
   - <https://github.com/VincentWei/build-minigui-5.0>
   - <https://github.com/VincentWei/minigui>
   - <https://github.com/FMSoftCN/hicairo>
   - <https://github.com/FMSoftCN/himesa>

## 参考文档

   - [MiniGUI 5.0 发布说明](https://mp.weixin.qq.com/s?__biz=MzA5MTYwNTA3MA==&mid=2651104433&idx=1&sn=33f288b5129d7c4d35816664822b1cb4&chksm=8b89d590bcfe5c86b4551f843c6840194f67ea65388a87017bea01b9baaf5a1ee6c8e6d38f66&mpshare=1&scene=1&srcid=&sharer_sharetime=1590459093298&sharer_shareid=47aa6185bd050078ff321e79ad280097&exportkey=AYi6gDPJ%2BH5%2BwSHJ4VhUztI%3D&pass_ticket=fGB7Y%2BrNQRYaw0wXXaesS5W%2FMwNfZC7EYUJ31qr4M7MzqPbaYfkHpDmfp3974KS0#rd)
   - [MiniGUI Overview](https://gitlab.fmsoft.cn/VincentWei/minigui-docs/blob/master/MiniGUI-Overview.md)
   - [Writing DRM Engine Driver for Your GPU](https://gitlab.fmsoft.cn/VincentWei/minigui-docs/blob/master/supplementary-docs/Writing-DRM-Engine-Driver-for-Your-GPU.md)
   - [Using Compositing Schema](https://gitlab.fmsoft.cn/VincentWei/minigui-docs/blob/master/supplementary-docs/Using-Compositing-Schema.md)
   - [Integrating with GPU](https://gitlab.fmsoft.cn/VincentWei/minigui-docs/blob/master/programming-guide/MiniGUIProgGuidePart3Chapter05.md)

## 其他链接

   - [活动官方网址](https://summer.iscas.ac.cn/)
   - [学生指南](https://summer.iscas.ac.cn/help/student/)

