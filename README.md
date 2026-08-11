# NUCLEO-F401RE 嵌入式项目集

基于 STM32 NUCLEO-F401RE 开发板与 Electronics Fun Kit 面包板元件包的个人嵌入式开发学习项目仓库。采用 STM32CubeMX + Keil MDK-ARM + STM32 HAL Library 技术栈，使用 C 语言开发，涵盖 STM32 各外设的实战练习。

## 硬件平台

| 项目 | 说明 |
|------|------|
| 主控板 | NUCLEO-F401RE（STM32F401RE，ARM Cortex-M4，84MHz，512KB Flash，96KB SRAM） |
| 元件包 | Electronics Fun Kit（含面包板、LED、电阻、电位器、杜邦线等 300+ 元件） |
| 调试器 | 板载 ST-Link/V2-1（USB 一线即可下载与调试） |

## 软件工具

| 工具 | 用途 |
|------|------|
| STM32CubeMX | 图形化引脚配置、时钟配置、工程生成 |
| Keil MDK-ARM | 代码编辑、编译、下载、在线调试 |
| STM32F4 HAL 库 | STM32F4 系列底层硬件驱动 |

## 项目结构

每个子项目位于独立文件夹内，均基于 CubeMX 生成的标准工程结构：

```
NUCLEO-F401RE/
├── <项目名>/              # 各子项目独立目录
│   ├── Core/              # 用户源码
│   ├── Drivers/           # CMSIS + STM32F4xx HAL 驱动
│   ├── MDK-ARM/           # Keil 工程文件与编译输出
│   └── <项目名>.ioc       # CubeMX 工程配置
├── 面包板基础元件包.md     # 元件包完整清单
└── README.md
```

## 快速开始

1. 安装 [STM32CubeMX](https://www.st.com) 和 [Keil MDK-ARM](https://www.keil.com)
2. 在 CubeMX 中安装 STM32F4 固件包；在 Keil Pack Installer 中安装 STM32F4 设备系列包
3. 安装 ST-Link USB 驱动（STSW-LINK009）
4. 用 USB 线连接 NUCLEO-F401RE 开发板，确认设备管理器识别到 STM32 STLink
5. 进入任意子项目目录，打开 `.uvprojx` 工程文件
6. 编译（F7）→ 下载（F8），观察运行效果

## 许可

本项目集基于 STM32CubeMX 生成的 HAL 库工程框架构建。ST 官方驱动文件（Drivers 目录）版权归 STMicroelectronics 所有。