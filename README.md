# 从零开始逐步实现U–Boot

从零开始逐步实现 U–Boot 学习系列源码。

U-Boot 学习历程：从零开始，逐行研读并动手复现 U-Boot 启动流程中的各项核心功能：

> ***处理器上电后的第一条指令 → 汇编与硬件初始化 → C 语言运行环境搭建 → 开发板专属初始化 → 操作系统加载***

微信搜索 **STUDYWITHIOT**，关注公众号，获取更多分享内容。

博客网站 [STUDYWITHIOT](https://link.zhihu.com/?target=https%3A//uscxi.pages.dev/)

## 从零开始逐步实现U-Boot(一)——Kconfig/Kbuild配置构建系统搭建与主机程序编译

剥离 U-Boot 的繁杂源码，只保留构建系统核心。

```none
u-boot/
├── Kconfig             # 顶层配置入口
├── Makefile            # 顶层 Makefile，处理全局逻辑
├── config.mk/          # Makefile 相关变量
├── arch/               # 架构相关代码（存放 Kconfig）
│   └── arm/
├── scripts/            # Kconfig/Kbuild 核心脚本
│   ├── basic/          # 构建工具 fixdep 源码
│   ├── kconfig/        # Kconfig 配置工具源码
│   ├── dtc/            # 设备树工具 DTC 源码
│   ├── gcc-version.sh  # 相关脚本
│   ├── setlocalversion # 相关脚本
│   ├── mkmakefile      # 外部构建目录中生成辅助 Makefile
│   ├── Makefile        # 主机程序编译 Makefile
│   ├── Kbuild.include  # 定义通用 Makefile 函数
│   ├── Kbuild.lib      # Kbuild 文件中的对象列表处理
│   ├── Kbuild.host     # 用于编译主机程序（如mkimage）的规则
│   ├── Kbuild.autoconf # 处理自动生成的配置头文件
│   ├── Kbuild.clean    # 清理规则
│   ├── Kconfig.include # Kconfig 配置工具变量
│   └── Makefile.build  # 递归编译的核心脚本
├── include/            # 头文件
└── configs/            # 存放默认配置文件（defconfig）
```