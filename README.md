# 基于STM32的DCM模式下的BUCK变换器设计

- 本工程包括软件部分的STM32的代码以及硬件部分的Altium Designer设计的PCB
- STM32使用STM32F103RCT6并移植FreeRTOS操作系统，进行按键任务、ADC任务、PWM生成任务、PID任务的调度管理
- PCB包括控制电路STM32，驱动电路以及主电路BUCK电路

下面就两部分进行介绍：

## 5、硬件部分

### 5.1、STM32F103RCT6控制电路

Cortex-M3采用目前主流ARM V7-M架构，相比曾风靡一时的ARM V4T架构拥有更加强劲的性能，更高的代码密度，更高的性价比。Cortex-M3处理器结合多种突破性技术，在低功耗、低成本、高性能三方面具有突破性的创新，使其在这几年迅速在中低端单片机市场异军突起。

国内Cortex-M3市场，ST（意法半导体）公司的STM32无疑是最大赢家，作为Cortex-M3内核最先尝蟹的两个公司（另一个是Luminary（流明））之一，ST无论是在市场占有率，还是在技术支持方面，都是远超其他对手。在Cortex-M3芯片的选择上，STM32无疑是大家的首选。所以自从ST推出STM32之后，一股强劲的STM32学习开发风潮扑面而来。本书也因STM32的流行应运而生。

开发板选择的是STM32F103RCT6作为MCU，它拥有的资源包括：48KBSRAM、256KBFLASH、2个基本定时器、4个通用定时器、2个高级定时器、2个DMA控制器（共12个通道）、3个SPI、2个IIC、5个串口、1个USB、1个CAN、3个12位ADC、1个12位DAC、1个SDIO接口及51个通用IO口。该芯片性价比极高。

#### 5.1.1 MCU部分的原理图如图

<img src=".\IMAGE\MINI-STM32_页面_1.jpg"/>



#### 5.1.2 PCB如图

<img src=".\IMAGE\MINI-STM32_页面_2.jpg"/>

#### 5.1.3 PCB板的3D效果如图

<img src=".\IMAGE\STM32.png"/>

#### 5.1.4 BOM报表如下

<img src=".\IMAGE\MINI-STM32_页面_3.jpg"/>

上图中中上部的BOOT1用于设置STM32的启动方式，其对应启动模式如下表所示：

| BOOT0 | BOOT1 |    启动模式    |              说明               |
| :---: | :---: | :------------: | :-----------------------------: |
|   0   |   X   | 用户闪存存储器 | 用户闪存存储器，也就是FLASH启动 |
|   1   |   0   |   系统存储器   |  系统存储器启动，用于串口下载   |
|   1   |   1   |    SRAM启动    | SRAM启动，用于在SRAM中调试代码  |

一般情况下（即标准的ISP下载步骤）如果我们想用用串口下载代码，则必须先配置BOOT0为1，BOOT1为0，然后按复位键，最后再通过程序下载代码，下载完以后又需要将BOOT0设置为GND，以便每次复位后都可以运行用户代码。

#### 5.1.5 JTAG部分电路如下图：

<img src=".\IMAGE\TIM截图20181202202550.png" width="500px"/>

这里采用的是标准的JTAG接法，但是STM32还有SWD接口，SWD只需要最少2根线（SWCLK和SWDIO）就可以下载并调试代码了，这同我们使用串口下载代码差不多，而且速度更快，能调试。所以建议在设计产品的时候，可以留出SWD来下载调试代码，而摒弃JTAG。STM32的SWD接口与JTAG是共用的，只要接上JTAG，你就可以使用SWD模式了（其实SWD并不需要JTAG这么多线），JLINK V8/JLINKV7ULINK2以及STLINK等都支持SWD。这里，我们使用SWD模式。

#### 5.1.6 按键

STM32开发板总共有3个按键，其原理图如下：

<img src=".\IMAGE\TIM截图20181202203009.png" width="500px"/>

KEY0和KEY1用作普通按键输入，分别连接在PC1和PC13上，WKUP按键连接到PA0（STM32的WKUP引脚），它除了可以用作普通输入按键外，还可以用作STM32的唤醒输入。

#### 5.1.7 LED 

STM32开发板上总共有3个LED，其原理图如下：

<img src=".\IMAGE\2018-12-02_203312.gif" width="500px"/>

其中D2是开发板电源指示灯。LED0和LED1分别接在PA8和PD2上，PA8还可以通过TIM1的通道1的PWM输出来控制D0的亮度。

#### 5.1.8 无线模块
STM32 开发板板载了NRF24L01无线模块的接口。该接口用来连接NRF24L01等2.4G无线模块，从而实现开发板与其他设备的无线数据传输（注意：NRF24L01不能和蓝牙/WIFI连接）。NRF24L01无线模块的最大传输速度可以达到2Mbps，传输距离最大可以到30米左右（空旷地，无干扰）。有了这个接口，我们就可以做无线通信，以及其他很多的相关应用了。这部分原理图如下：

<img src=".\IMAGE\TIM截图20181202203608.png" width="500px"/>

#### 5.1.9 SPI FLASH 

STM32开发板载有SPIFLASH芯片W25Q64，该芯片的容量为8M字节，其原理图如下：

<img src=".\IMAGE\TIM截图20181202203720.png" width="500px"/>

#### 5.1.10 USB、电源
开发板的供电部分还引出了5V（VOUT2）和3.3V（VOUT1）的排针，可以用来为外部设备提供电源或者从外部引入电源，这在很多时候是非常有用的，有时候你突然要一个3.3V的电源，但找半天就是没这样的电源，而我们的板子则可直接向外部提供3.3V电源，有了它，你就可以给外部设备提供3.3V、5V电源了。注意电流不能太大！
开发板的USB接口（USB）通过独立的MiniUSB头引出，不和USB转串口（USB232）共用，这样不但可以同时使用，还可以给系统提供更大的电流。
这几个部分的原理图如下：

<img src=".\IMAGE\TIM截图20181202203911.png" width="500px"/>

### 5.2、驱动电路

#### 5.2.1 驱动电路原理图如下

<img src=".\IMAGE\Drive_Circuit_页面_1.jpg"/>

#### 5.2.2 PCB如下

<img src=".\IMAGE\Drive_Circuit_页面_2.jpg"/>

#### 5.2.3 3D效果图如下

<img src=".\IMAGE\Drive.png"/>

#### 5.2.4 BOM报表如下

<img src=".\IMAGE\Drive_Circuit_页面_3.jpg"/>

#### 5.2.5 电压检测电路

<img src=".\IMAGE\1.png"/>

将输出电压经过运放·358放大2倍，送入滤波电路后，进行电压限幅后送入单片机AD通道。因为输出电压一直为正，运放不会产生负压，所以358采用单电源供电，减少了-5V电源的产生电路。

#### 5.2.6 驱动电路

<img src=".\IMAGE\2.png"/>

我们在单片机的AD输出端口与IR2110外围模拟电路之间添加了TLP521光耦隔离，增强电路的安全性，减小电压的干扰。

另外，IR2110的不足是不能产生负偏压。在大功率IGBT驱动场合，各路驱动电源独立，集成驱动器一般都能产生-5 V负压，用于增强IGBT关断的可靠性，防止由于密勒效应而造成误导通。IR2110器件内部虽不能产生负压，但可通过外加无源器件产生负压。

在驱动电路中增加由电容和5V稳压管组成的负压电路。其工作原理为：电源电压Vcc为20V。在上电期间，电源通过R8为C13充电，C13保持5V电压。HIN输入高电平时，HO输出20V，加在VG1的电压为15 V。当HIN为低电平时，HO输出0 V，VG1电压为-5V。选择的C13，C14要大于IGBT栅极输入寄生电容Ciss。自举电容充电电路中的二极管D1必须是快恢复二极管，以保证在有限时间内快速导通。

#### 5.2.7 与模拟系统对比

数字系统采用单片机控制，电路需要添加隔离电路，相比较模拟系统的3525芯片及其外围电路，成本较高；而在闭环的PI调试环节，数字系统只需要更改程序中的PI参数，相比较模拟系统更改电位器的电阻值而言，更加的方便快捷。

### 5.3、主电路

#### 5.3.1 主电路原理图如下

<img src=".\IMAGE\BUCK_页面_1.jpg"/>

#### 5.3.2 PCB如下

<img src=".\IMAGE\BUCK_页面_2.jpg"/>

#### 5.3.3 3D效果图如下

<img src=".\IMAGE\BUCK.png"/>

#### 5.3.4BOM报表如下

<img src=".\IMAGE\BUCK_页面_3.jpg"/>

### 5.4、UART 串口屏幕

串口屏定义就是，带串口控制的液晶屏就是串口屏了

详细定义：一套由单片机或PLC带控制器的显示方案，显示方案中的通讯部分由串口通讯，UART串口或者SPI串口等；它由显示驱动板、外壳、LCD液晶显 示屏三部分构成。接收用户单片机串口发送过来的指令，完成在LCD上绘图的所有操作。

#### 5.4.1 发展：

市面上， 2010年前初期版本的串口屏，都是简单的通过一个上位机软件来进行界面下载，然后发送命令过去把各种需要显示的界面显示出来，以达到所需要的显示效果，触摸方面仅支持上传坐标。

2014年发展的版本中，串口都已经形成了一个产业链，串口屏 功能非常强大，如一些串口屏带组态功能，各样功能都不再是简单的使用图片来做的粗糙操作，界面操作上越来越接近一些手机的显示。

未来的发展中，串口屏将会出现各种版本，如无线通讯版本，系统组网等等各种强大的功能，只有我们想不到的。

#### 5.4.2 应用：

广泛应用于工业自动化、电力、电信、环保、医疗、金融、石油、化工、交通、能源、地质、冶金、公共查询与监控等数十个行业和领域，其中，在某些军工及航天领域，凭借可靠、稳定的产品性能已被列入合格供应商名录。完全杜绝部分控制器存在的雪花、乱码、时序不兼容、工作温度范围窄等问题。

配套使用软件：

各个厂家各有不同的配套软件，软件上可以实现各种复杂功能，操作简单方便，部分厂家中配套有虚拟串口屏，可以直接通过软件来直接评估实体屏实现的功能，节约生产成本。

通过配套的组态软件，拥有界面设计所需要的所有控件，用户单片机只要编写一点点代码就可以完成界面设计，真正体现“零代码”

#### 5.4.3 产品特点

1、使用字符串指令

字符串指令比十六进制指令更方便开发，提升工程师的工作效率，使用字符串指令的源代码更易读，使用字符串指令在以后查阅单片机代码时会更清楚代码含义。

2、数据结构精简

产品使用指令结构如下：字符串指令+结束符

3、产品使用“C语言”指令

产品直接使用“C语言”指令，同行其他产品使用“汇编”指令。C语言指令比汇编语言更易读写，开发更方便。

4、控件属性赋值支持简易运算

比如j0控件的val属性赋值通常情况下是这么写j0.val=10
使用运算方式可以这么写：j0.val=j0.val+10
也可以这么写：j0.val=j0.val/2+10
还可以这么写：j0.val=j1.val-j0.val"2+dim（dim是系统变量）

5、屏幕固件自动升级

上位软件编译出来的资源文件就包含了最新的显示模组固件。任何功能性的升级或者bug修复，只需要使用最新的上位软件，编译出资源文件后下载到显示模组，显示模组固件立刻自动升级。上位软件有自动更新功能，只要电脑网络畅通，电脑防火墙没有隔离的软件，每次启动软件的时候软件检测到新版本会提示是否立即升级。

#### 5.4.4 功能简介

+ I/O接口：通过软件配置可以吧I/O配置成输入状态或输出状态。另外我们的产品/O可以绑定控件使用。

+ 支持通讯接口：TTL、RS232、RS485、CAN女触摸类型：电容触摸、电阻触摸、无触摸

+ 通电进入工作状态无需任何初始化设置

+ 提供用户数据存储空间（EEPROM）产品结构可根据客户需求进行定制

+ 可通过串口指令调整背光

+ 可通过串口指令画图

+ 支持RTC功能

+ 横竖可控

#### 5.4.5 原理图如下

<img src=".\IMAGE\TJC3224T028_011X_页面_2.jpg"/>

## 6、软件部分

### 6.1 FreeRTOS简介

我们看一下FreeRTOS的名字，可以分为两部分：Free和RTOS，Free就是免费的、自由的、不受约束的意思，RTOS全称是Real Time Operating System，中文名就是实时操作系统。可以看出FreeROTS就是一个免费的RTOS类系统。这里要注意，RTOS不是指某一个确定的系统，而是指一类系统。比如UCOS，FreeRTOS，RTX，RT-Thread等这些都是RTOS类操作系统。

操作系统允许多个任务同时运行，这个叫做多任务，实际上，一个处理器核心在某一时刻只能运行一个任务。操作系统中任务调度器的责任就是决定在某一时刻究竟运行哪个任务，任务调度在各个任务之间的切换非常快！这就给人们造成了同一时刻有多个任务同时运行的错觉。

操作系统的分类方式可以由任务调度器的工作方式决定，比如有的操作系统给每个任务分配同样的运行时间，时间到了就轮到下一个任务，Unix操作系统就是这样的。RTOS的任务调度器被设计为可预测的，而这正是嵌入式实时操作系统所需要的，实时环境中要求操作系统必须对某一个事件做出实时的响应，因此系统任务调度器的行为必须是可预测的。像FreeRTOS这种传统的RTOS类操作系统是由用户给每个任务分配一个任务优先级，任务调度器就可以根据此优先级来决定下一刻应该运行哪个任务。

FreeRTOS是RTOS系统的一种，FreeRTOS十分的小巧，可以在资源有限的微控制器中运行，当然了，FreeRTOS不仅局限于在微控制器中使用。但从文件数量上来看FreeRTOS要比UCOSII和UCOSII小的多。

### 6.2 为什么选择FreeRTOS？
上面我们说了RTOS类系统有很多，为什么要选择FreeRTOS呢？在UCOS教程中，我们说过学习RTOS首选UCOS，因为UCOS的资料很多，尤其是中文资料！但是FreeRTOS的资料少，而且大多数是英文的，我为何要选择它？原因如下：

1、FreeRTOS免费！这是最重要的一点，UCOs是要收费的，学习RTOS系统的话UCOS是首选，但是做产品的话就要考虑一下成本了。显而易见的，FreeRTOS在此时就是一个很好的选择，当然了也可以选择其他的免费的RTOS系统。

2、许多其他半导体厂商产品的SDK包就使用FreeRTOS作为其操作系统，尤其是WIFI、蓝牙这些带协议栈的芯片或模块。

3、许多软件厂商也使用FreeRTOS做本公司软件的操作系统，比如著名的TouchGFX，其所有的例程都是基于FreeRTOS操作系统的。ST公司的所有要使用到RTOS系统的例程也均采用了FreeRTOS，由此可见免费的力量啊！

4、简单，FreeRTOS的文件数量很少，这个在我们后面的具体学习中就会看到，和UCOS系统相比要少很多！

5、文档相对齐全，在FreeRTOS的官网（www.freertos.org）上可以找到所需的文档和源码，但是所有的文档都是英文版本的，而且下载pdf文档的时候是要收费的。

6、FreeRTOS被移植到了很多不同的微处理器上，比如我们使用的STM32，F1、F3、F4和最新的F7都有移植，这个极大的方便了我们学习和使用。

7、社会占有量很高，EEtimes 统计的2015年RTOS系统占有量中FreeRTOS已经跃升至第一位，如图所示。

<img src=".\IMAGE\TIM截图20181202205637.png"/>

### 6.3 FreeRTOS特点
FreeRTOS是一个可裁剪的小型RTOS系统，其特点包括：

+ FreeRTOS的内核支持抢占式，合作式和时间片调度。

+ SafeRTOS衍生自FreeRTOS，SafeRTOS在代码完整性上相比FreeRTOS更胜一筹。
+ 提供了一个用于低功耗的Tickless模式。

+ 系统的组件在创建时可以选择动态或者静态的RAM，比如任务、消息队列、信号量、软件定时器等等。

+ 已经在超过30种架构的芯片上进行了移植。
+ FreeRTOS-MPU支持Corex-M系列中的MPU单元，如STM32F103。

+ FreeRTOS系统简单、小巧、易用，通常情况下内核占用4k-9k字节的空间。
+ 高可移植性，代码主要C语言编写。
+ 支持实时任务和协程（co-routines也有称为合作式、协同程序，本教程均成为协程）。

+ 任务与任务、任务与中断之间可以使用任务通知、消息队列、二值信号量、数值型信号量、递归互斥信号量和互斥信号量进行通信和同步。

+ 创新的事件组（或者事件标志）。
+ 具有优先级继承特性的互斥信号量。
+ 高效的软件定时器。
+ 强大的跟踪执行功能。
+ 堆栈溢出检测功能。
+ 任务数量不限。
+ 任务优先级不限。

### 6.4 FreeRTOS任务基础知识
RTOS系统的核心就是任务管理，FreeRTOS也不例外，而且大多数学习RTOS系统的工程师或者学生主要就是为了使用RTOS的多任务处理功能，初步上手RTOS系统首先必须掌握的也是任务的创建、删除、挂起和恢复等操作，由此可见任务管理的重要性。由于任务相关的知识很多，所以接下来讲解FreeRTOS的任务基础知识。

#### 6.4.1 什么是多任务系统？

以前在使用51、AVR、STM32单片机裸机（未使用系统）的时候一般都是在main 函数里面用while（1）做一个大循环来完成所有的处理，即应用程序是一个无限的循环，循环中调用相应的函数完成所需的处理。有时候我们也需要中断中完成一些处理。相对于多任务系统而言，这个就是单任务系统，也称作前后台系统，中断服务函数作为前台程序，大循环while（1）作为后台程序，如图所示：

<img src=".\IMAGE\TIM截图20181202210433.png"/>

前后台系统的实时性差，前后台系统各个任务（应用程序）都是排队等着轮流执行，不管你这个程序现在有多紧急，没轮到你就只能等着！相当于所有任务（应用程序）的优先级都是一样的。但是前后台系统简单啊，资源消耗也少啊！在稍微大一点的嵌入式应用中前后台系统就明显力不从心了，此时就需要多任务系统出马了。

多任务系统会把一个大问题（应用）“分而治之”，把大问题划分成很多个小问题，逐步的把小问题解决掉，大问题也就随之解决了，这些小问题可以单独的作为一个小任务来处理。这些小任务是并发处理的，注意，并不是说同一时刻一起执行很多个任务，而是由于每个任务执行的时间很短，导致看起来像是同一时刻执行了很多个任务一样。多个任务带来了一个新的问题，究竟哪个任务先运行，哪个任务后运行呢？完成这个功能的东西在RTOS系统中叫做任务调度器。不同的系统其任务调度器的实现方法也不同，比如FreeRTOS是一个抢占式的实时多任务系统，那么其任务调度器也是抢占式的，运行过程如图所示：

<img src=".\IMAGE\TIM截图20181202210706.png"/>

在图中，高优先级的任务可以打断低优先级任务的运行而取得CPU的使用权，这样就保证了那些紧急任务的运行。这样我们就可以为那些对实时性要求高的任务设置一个很高的优先级，比如自动驾驶中的障碍物检测任务等。高优先级的任务执行完成以后重新把CPU的使用权归还给低优先级的任务，这个就是抢占式多任务系统的基本原理。

#### 6.4.2 FreeRTOS任务与协程

再FreeRTOS中应用既可以使用任务，也可以使用协程（Co-Routine），或者两者混合使用。但是任务和协程使用不同的API函数，因此不能通过队列（或信号量）将数据从任务发送给协程，反之亦然。协程是为那些资源很少的MCU准备的，其开销很小，但是FreeRTOS官方已经不打算再更新协程了，所以本教程只讲解任务。

##### 任务（Task）的特性
在使用RTOS的时候一个实时应用可以作为一个独立的任务。每个任务都有自己的运行环境，不依赖于系统中其他的任务或者RTOS调度器。任何一个时间点只能有一个任务运行，具体运行哪个任务是由RTOS调度器来决定的，RTOS调度器因此就会重复的开启、关闭每个任务。任务不需要了解RTOS调度器的具体行为，RTOS调度器的职责是确保当一个任务开始执行的时候其上下文环境（寄存器值，堆栈内容等）和任务上一次退出的时候相同。为了做到这一点，每个任务都必须有个堆栈，当任务切换的时候将上下文环境保存在堆栈中，这样当任务再次执行的时候就可以从堆栈中取出上下文环境，任务恢复运行。

任务特性：

1、简单。

2、没有使用限制。

3、支持抢占

4、支持优先级

5、每个任务都拥有堆栈导致了RAM使用量增大。

6、如果使用抢占的话的必须仔细的考虑重入的问题。

##### 协程（Co-routine）的特性

协程是为那些资源很少的MCU而做的，但是随着MCU的飞速发展，性能越来越强大，现在协程几乎很少用到了！但是FreeRTOS目前还没有把协程移除的计划，但是FreeRTOS是绝对不会再更新和维护协程了，因此协程大家了解一下就行了。在概念上协程和任务是相似的，但是有如下根本上的不同：

1、堆栈使用

所有的协程使用同一个堆栈（如果是任务的话每个任务都有自己的堆栈），这样就比使用任务消耗更少的RAM。

2、调度器和优先级

协程使用合作式的调度器，但是可以在使用抢占式的调度器中使用协程。

3、宏实现

协程是通过宏定义来实现的。

4、使用限制

为了降低对RAM的消耗做了很多的限制。

#### 6.4.3 任务状态
FreeRTOS中的任务永远处于下面几个状态中的某一个：

+ 运行态
  当一个任务正在运行时，那么就说这个任务处于运行态，处于运行态的任务就是当前正在使用处理器的任务。如果使用的是单核处理器的话那么不管在任何时刻永远都只有一个任务处于运行态。·就绪态处于就绪态的任务是那些已经准备就绪（这些任务没有被阻塞或者挂起），可以运行的任务，但是处于就绪态的任务还没有运行，因为有一个同优先级或者更高优先级的任务正在运行！
+ 阻塞态
  如果一个任务当前正在等待某个外部事件的话就说它处于阻塞态，比如说如果某个任务调用了函数VTaskDelayO的话就会进入阻塞态，直到延时周期完成。任务在等待队列、信号量、事件组、通知或互斥信号量的时候也会进入阻塞态。任务进入阻塞态会有一个超时时间，当超过这个超时时间任务就会退出阻塞态，即使所等待的事件还没有来临！
+ 挂起态
  像阻塞态一样，任务进入挂起态以后也不能被调度器调用进入运行态，但是进入挂起态的任务没有超时时间。任务进入和退出挂起态通过调用函数vTaskSuspendO和xTaskResume0。
  任务状态之间的转换如图所示：

<img src=".\IMAGE\TIM截图20181202211151.png"/>

#### 6.4.4 任务优先级
每个任务都可以分配一个从0~（configMAX_PRIORITIES-1）的优先级，configMAX_PRIORITIES 在文件FreeRTOSConfig.h中有定义，前面我们讲解 FreeRTOS系统配置的时候已经讲过了。如果所使用的硬件平台支持类似计算前导零这样的指令（可以通过该指令选择下一个要运行的任务，Cortex-M处理器是支持该指令的），并且宏configUSE_PORT_OPTIMISED_TASK_SELECTION 也设置为了1，那么宏configMAX_PRIORITIES不能超过32！也就是优先级不能超过32级。其他情况下宏configMAX_PRIORITIES可以为任意值，但是考虑到RAM的消耗，宏configMAX_PRIORITIES最好设置为一个满足应用的最小值。

优先级数字越低表示任务的优先级越低，0的优先级最低，confieMAX_PRIORITIES-1的优先级最高。空闲任务的优先级最低，为0。

FreeRTOS调度器确保处于就绪态或运行态的高优先级的任务获取处理器使用权，换句话说就是处于就绪态的最高优先级的任务才会运行。当宏configUSE_TIME_SLICING定义为1的时候多个任务可以共用一个优先级，数量不限。默认情况下宏configUSE_TIME_SLICING在文件FreeRTOS.h中已经定义为1。此时处于就绪态的优先级相同的任务就会使用时间片轮转调度器获取运行时间。

#### 6.4.5 任务实现
在使用FreeRTOS的过程中，我们要使用函数xTaskCreate0或xTaskCreateStatic()来创建任务，这两个函数的第一个参数pxTaskCode，就是这个任务的任务函数。什么是任务函数？任务函数就是完成本任务工作的函数。我这个任务要干嘛？要做什么？要完成什么样的功能都是在这个任务函数中实现的。比如我要做个任务，这个任务要点个流水灯，那么这个流水灯的程序就是任务函数中实现的。

#### 6.4.6 任务控制块
FreeRTOS的每个任务都有一些属性需要存储，FreeRTOS把这些属性集合到一起用一个结构体来表示，这个结构体叫做任务控制块：TCB_t，在使用函数xTaskCreate0创建任务的时候就会自动的给每个任务分配一个任务控制块。在老版本的FreeRTOS中任务控制块叫做tskTCB，新版本重命名为TCBt，但是本质上还是tskTCB。

#### 6.4.7 任务堆栈
FreeRTOS 之所以能正确的恢复一个任务的运行就是因为有任务堆栈在保驾护航，任务调度器在进行任务切换的时候会将当前任务的现场（CPU寄存器值等）保存在此任务的任务堆栈中，等到此任务下次运行的时候就会先用堆栈中保存的值来恢复现场，恢复现场以后任务就会接着从上次中断的地方开始运行。

创建任务的时候需要给任务指定堆栈，如果使用的函数xTaskCreate0创建任务（动态方法）的话那么任务堆栈就会由函数xTaskCreate0自动创建，后面分析xTaskCreateO的时候会讲解。如果使用函数xTaskCreateStaticO创建任务（静态方法）的话就需要程序员自行定义任务堆栈，然后堆栈首地址作为函数的参数puxStackBuffer传递给函数。

### 6.5 FreeRTOS队列

在实际的应用中，常常会遇到一个任务或者中断服务需要和另外一个任务进行“沟通交流”，这个“沟通交流”的过程其实就是消息传递的过程。在没有操作系统的时候两个应用程序进行消息传递一般使用全局变量的方式，但是如果在使用操作系统的应用中用全局变量来传递消息就会涉及到“资源管理”的问题。FreeRTOS对此提供了一个叫做“队列”的机制来完成任务与任务、任务与中断之间的消息传递。

#### 6.5.1 队列简介
队列是为了任务与任务、任务与中断之间的通信而准备的，可以在任务与任务、任务与中断之间传递消息，队列中可以存储有限的、大小固定的数据项目。任务与任务、任务与中断之间要交流的数据保存在队列中，叫做队列项目。队列所能保存的最大数据项目数量叫做队列的长度，创建队列的时候会指定数据项目的大小和队列的长度。由于队列用来传递消息的，所以也称为消息队列。FreeRTOS中的信号量的也是依据队列实现的！所以有必要深入的了解FreeRTOS的队列。

##### 数据存储
通常队列采用先进先出（FIFO）的存储缓冲机制，也就是往队列发送数据的时候（也叫入队）永远都是发送到队列的尾部，而从队列提取数据的时候（也叫出队）是从队列的头部提取的。但是也可以使用LIFO的存储缓冲，也就是后进先出，FreeRTOS中的队列也提供了LIFO的存储缓冲机制。

数据发送到队列中会导致数据拷贝，也就是将要发送的数据拷贝到队列中，这就意味着在队列中存储的是数据的原始值，而不是原数据的引用（即只传递数据的指针），这个也叫做值传递。学过UCOS的应该知道，UCOS的消息队列采用的是引用传递，传递的是消息指针。

采用引用传递的话消息内容就必须一直保持可见性，也就是消息内容必须有效，那么局部变量这种可能会随时被删掉的东西就不能用来传递消息，但是采用引用传递会节省时间啊！因为不用进行数据拷贝。

采用值传递的话虽然会导致数据拷贝，会浪费一点时间，但是一旦将消息发送到队列中原始的数据缓冲区就可以删除掉或者覆写，这样的话这些缓冲区就可以被重复的使用。FreeRTOS中使用队列传递消息的话虽然使用的是数据拷贝，但是也可以使用引用来传递消息啊，我直接往队列中发送指向这个消息的地址指针不就可以了！这样当我要发送的消息数据太大的时候就可以直接发送消息缓冲区的地址指针，比如在网络应用环境中，网络的数据量往往都很大的，采用数据拷贝的话就不现实。

1、多任务访问

队列不是属于某个特别指定的任务的，任何任务都可以向队列中发送消息，或者从队列中提取消息。

2、出队阻塞

当任务尝试从一个队列中读取消息的时候可以指定一个阻塞时间，这个阻塞时间就是当任务从队列中读取消息无效的时候任务阻塞的时间。出队就是就从队列中读取消息，出队阻塞是针对从队列中读取消息的任务而言的。比如任务A用于处理串口接收到的数据，串口接收到数据以后就会放到队列Q中，任务A从队列Q中读取数据。但是如果此时队列Q是空的，说明还没有数据，任务A这时候来读取的话肯定是获取不到任何东西，那该怎么办呢？任务A现在有三种选择，一：二话不说扭头就走，二：要不我在等等吧，等一会看看，说不定一会就有数据了，三：死等，死也要等到你有数据！选哪一个就是由这个阻塞时间决定的，这个阻塞时间单位是时钟节拍数。阻塞时间为0的话就是不阻塞，没有数据的话就马上返回任务继续执行接下来的代码，对应第一种选择。如果阻塞时间为0～portMAXDELAY，当任务没有从队列中获取到消息的话就进入阻塞态，阻塞时间指定了任务进入阻塞态的时间，当阻塞时间到了以后还没有接收到数据的话就退出阻塞态，返回任务接着运行下面的代码，如果在阻塞时间内接收到了数据就立即返回，执行任务中下面的代码，这种情况对应第二种选择。当阻塞时间设置为portMAX_DELAY的话，任务就会一直进入阻塞态等待，直到接收到数据为止！这个就是第三种选择。

3、入队阻塞

入队说的是向队列中发送消息，将消息加入到队列中。和出队阻塞一样，当一个任务向队列发送消息的话也可以设置阻塞时间。比如任务B向消息队列Q发送消息，但是此时队列Q是满的，那肯定是发送失败的。此时任务B就会遇到和上面任务A一样的问题，这两种情况的处理过程是类似的，只不过一个是向队列Q发送消息，一个是从队列Q读取消息而已。

4、队列操作过程图示

下面几幅图简单的演示了一下队列的入队和出队过程。

+ 创建队列

<img src=".\IMAGE\TIM截图20181202212052.png"/>

图中任务A要向任务B发送消息，这个消息是x变量的值。首先创建一个队列，并且指定队列的长度和每条消息的长度。这里我们创建了一个长度为4的队列，因为要传递的是x值，而x是个int类型的变量，所以每条消息的长度就是int类型的长度，在STM32中就是4字节，即每条消息是4个字节的。

+ 向队列发送第一个消息

<img src=".\IMAGE\TIM截图20181202212159.png"/>

图中任务A的变量x值为10，将这个值发送到消息队列中。此时队列剩余长度就是3了。前面说了向队列中发送消息是采用拷贝的方式，所以一旦消息发送完成变量x就可以再次被使用，赋其他的值。

+ 向队列发送第二个消息

<img src=".\IMAGE\TIM截图20181202212249.png"/>

图中任务A又向队列发送了一个消息，即新的x的值，这里是20。此时队列剩余长度为2。

+ 从队列中读取消息

<img src=".\IMAGE\TIM截图20181202212402.png"/>

### 6.6 代码解析

#### mian.c（主函数） 

```c
//引用头文件
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "HMI.h"
#include "timer.h"
#include "adc.h"
#include "pid.h"
#include <math.h> 
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//开始任务参数设置
//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//ADC任务参数设置
//任务优先级
#define ADC_TASK_PRIO		3
//任务堆栈大小	
#define ADC_STK_SIZE 		256  
//任务句柄
TaskHandle_t ADCTask_Handler;
//任务函数
void adc_task(void *pvParameters);

//按键任务参数设置
//任务优先级
#define KEYPROCESS_TASK_PRIO 2
//任务堆栈大小	 
#define KEYPROCESS_STK_SIZE  128 
//任务句柄
TaskHandle_t Keyprocess_Handler;
//任务函数
void Keyprocess_task(void *pvParameters);

//PID调节任务参数设置
//任务优先级
#define PID_TASK_PRIO 4
//任务堆栈大小	 
#define PID_STK_SIZE  256 
//任务句柄
TaskHandle_t PID_Handler;
//任务函数
void pid_task(void *pvParameters);

//消息队列参数设置，按键与ADC值的队列都设置为20长度
#define KEYMSG_Q_NUM    20		//按键消息队列的数量 
#define MESSAGE_Q_NUM   20 	//发送数据的消息队列的数量
QueueHandle_t Key_Queue;   		//按键值消息队列句柄
QueueHandle_t Message_Queue;	//信息队列句柄

//进入低功耗模式前需要处理的事情
//ulExpectedIdleTime：低功耗模式运行时间
void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
	//关闭某些低功耗模式下不使用的外设时钟，此处只是演示性代码
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,DISABLE);
}

//退出低功耗模式以后需要处理的事情
//ulExpectedIdleTime：低功耗模式运行时间
void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
	//退出低功耗模式以后打开那些被关闭的外设时钟，此处只是演示性代码
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);	              
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	 
	uart_init(9600);					//初始化串口
	LED_Init();							//LED初始化
	PID_init();							//PID初始化
	KEY_Init();							//按键初始化
	TIM3_PWM_Init(1439,0);	 			//TIM3初始化，不分频。PWM频率=72000000/1440=50Khz
	Adc_Init();		  					//ADC初始化	
	my_mem_init(SRAMIN);            	//初始化内部内存池
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
	//创建消息队列
	Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(u16));        //创建消息Key_Queue
    Message_Queue=xQueueCreate(MESSAGE_Q_NUM,sizeof(u16)); //创建消息Message_Queue,队列项长度是串口接收缓冲区长度
	
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )adc_task,             
                (const char*    )"adc_task",           
                (uint16_t       )ADC_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )ADC_TASK_PRIO,        
                (TaskHandle_t*  )&ADCTask_Handler);  
				
	//创建TASK2任务
    xTaskCreate((TaskFunction_t )Keyprocess_task,     
                (const char*    )"keyprocess_task",   
                (uint16_t       )KEYPROCESS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )KEYPROCESS_TASK_PRIO,
                (TaskHandle_t*  )&Keyprocess_Handler);
				
    //创建TASK3任务
    xTaskCreate((TaskFunction_t )pid_task,     
                (const char*    )"pid_task",   
                (uint16_t       )PID_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )PID_TASK_PRIO,
                (TaskHandle_t*  )&PID_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
//ADC任务函数
void adc_task(void *pvParameters)
{
	u16 adcx,flo;
	
	BaseType_t err;									//定义错误返回接收句柄
	while(1)
	{
		adcx=Get_Adc_Average(ADC_Channel_1,10);		//ADC通道1采样10次并取平均值
		flo = (int)adcx*134.3/4096;				//串口屏要显示的实际电压值
		CurveCommand(1,0,flo);						//将电压值通过串口发送到串口屏幕显示
		if ((Message_Queue!=NULL) && (adcx))		//如果队列创建成功并且ADC采集到数据
		{
			err=xQueueSend(Message_Queue,&adcx,10);	//将数据装入队列，如果队列已满最多等10个时钟节拍，否则放弃本次入队操作
            if(err==errQUEUE_FULL)   				//如果入队失败
            {
                //printf("队列Message_Queue已满，数据发送失败!\r\n");
            }
		}
		vTaskDelay(10);                           	//延时10ms，也就是10个时钟节拍	
	}
}

//Keyprocess_task函数
void Keyprocess_task(void *pvParameters)
{
	u8 key;
	u16 pwm=0;
    BaseType_t err;
	while(1)
	{
		key=KEY_Scan(0);            				//扫描按键
		switch(key)
		{
			case WKUP_PRES:		//KEY_UP控制pwm值加90
				pwm += 90;
				break;
			case KEY1_PRES:		//KEY1控制pwm值加180
				pwm += 10;
				break;
			case KEY0_PRES:		//KEY0控制pwm值减90
				pwm = 556;
				break;
		}
		if (pwm>1440)			//限制pwm的值不能超过1440
		{
			pwm = 1440;
		}
		NumberCommand(0,(int)pwm*100/1440);		//串口屏幕显示当前输出电压的值
		NumberCommand(1,(int)pwm*24/1440);		//串口屏幕显示当前输出占空比的值
		if((Key_Queue!=NULL)&&(pwm!=0))   		//消息队列Key_Queue创建成功,并且按键被按下
        {
            err=xQueueSend(Key_Queue,&pwm,10);
            if(err==errQUEUE_FULL)   			//如果入队失败
            {
                //printf("队列Key_Queue已满，数据发送失败!\r\n");
            }
        }
		vTaskDelay(10);                           //延时10ms，也就是10个时钟节拍	
	}
	
}

//PID_task函数
void pid_task(void *pvParameters)
{
	u16 adcx,pwm;
	extern PID V_PID;
	while(1)
	{		
		if(Message_Queue!=NULL && Key_Queue!=NULL)	//如果按键队列和ADC消息队列都创建成功
        {
            if(xQueueReceive(Message_Queue,&adcx,portMAX_DELAY) && xQueueReceive(Key_Queue,&pwm,portMAX_DELAY))//请求消息Message_Queue和Key_Queue
            {
				//闭环
				V_PID.setpulse = pwm*4096/1440;		//按键给定值
				V_PID.backpulse = adcx/2;		//电压反馈值
				PWM_VAL1=V_PIDCalc(&V_PID);
				PWM_VAL2=V_PIDCalc(&V_PID);
				//开环
				//PWM_VAL1=pwm;
				//PWM_VAL2=pwm;
			}
		}
		vTaskDelay(10);      //延时10ms，也就是10个时钟节拍
	}
}


```

#### PID.c（PID调节代码）

```c
#include "pid.h"

PID V_PID;
void PID_init(void)
{	
  	V_PID.setpulse = 0 ;	      	//电压设定值
  	V_PID.backpulse = 0 ;			//电压反馈值		
  	V_PID.last_error = 0 ;	 
  	V_PID.pre_error = 0 ;	  
  	V_PID.P = Pv;
  	V_PID.I = Iv;
  	V_PID.D = Dv;	
  	V_PID.motorout = 0 ;		    //控制输出值
}

unsigned int V_PIDCalc( PID *pp )	//按照课本给定的PID公式进行操作
{
    int error;
    
	error = pp->setpulse - pp->backpulse;

  	pp->motorout +=( int) (pp->P*(error-pp->last_error) + pp->I*error + pp->D*(error-2*pp->last_error+pp->pre_error));
 		
 	pp->pre_error = pp->last_error;	 
 	pp->last_error = error;
    	
    if( pp->motorout >= D_MAX) 		
		   pp->motorout = D_MAX;		
    else if( pp->motorout <= D_MIN)	//电压PID，防止调节最低溢出 
		   pp->motorout = D_MIN;			
  	
  	return (pp->motorout);			// 返回预调节占空比
}
```

#### pid.h（PID调节代码）

```	c
#ifndef __PID_H
#define __PID_H

#include "delay.h"
#include "stm32f10x.h"

/////////////////PID调节////////////////////////////////////////////////////
#define  Pv  0.1  	//0.6//0.1		     振荡 0.05	 振荡至稳定0.1		   
#define  Iv  0.5	//0.08					      0.04			   0.03		   
#define  Dv  0

#define D_MAX 620	//占空比输出最大值
#define D_MIN 0 	//占空比输出最小值
//定义PID
typedef struct PID		
{
	int setpulse;		    //设定值
  	int backpulse;			//反馈值
	int last_error;  	
	int pre_error;	
	float P;		    	
	float I;		    
	float D;		    						
	int motorout;	  		//控制输出值	
}PID;

void PID_init(void);
unsigned int V_PIDCalc( PID *pp );		

#endif
```

#### timer.c（定时器与PWM代码）

```c
#include "timer.h"
#include "led.h"
#include "usart.h"
   	  
//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  , ENABLE);  //使能GPIOA模块时钟
	  
 
   //设置该引脚为复用输出功能,输出TIM3 CH1的PWM脉冲波形	GPIOA.6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
 	//上面两句中的OC1确定了是channle几，要是OC2则是channel 2  
	TIM3->CCR1 = 0;	    //初始化占空比

	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
    TIM3->CCR2 = 0;

	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	
}
```

#### adc.c（ADC采样代码）

```c
 #include "adc.h"
 #include "delay.h"	   
		   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	 
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   
  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}	

//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)			//采样times次并取平均值
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 
```

#### HMI.c（串口屏幕显示代码）

```c
#include "HMI.h"
#include "usart.h"

void CurveCommand(int ID, int channel, int value)		//串口屏幕绘图曲线指令
{
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	printf("add %d,%d,%d",ID,channel,value);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	
}
	
void NumberCommand(int ID,int num)						//串口屏幕绘制数字指令
{
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	printf("n%d.val=%d",ID,num);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束

}
```

#### key.c（按键代码）

```c
#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1|GPIO_Pin_13;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOE4,3

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0

}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(WK_UP==1)return WKUP_PRES;
	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	    
 	return 0;// 无按键按下
}
```



