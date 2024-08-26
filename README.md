<p align="center">
  <img src="./doxygen/Logo.jpg" alt="gplug logo"/>
</p>

# 1.介绍

[![badge](https://img.shields.io/badge/license-Zlib-blue)](./LICENSE.txt)
[![badge](https://img.shields.io/badge/document-doxygen-brightgreen)](./doc)
[![badge](https://img.shields.io/badge/platform-windows%20%7C%20linux-green)](./README.md)

gplug（通用插件管理库）是一个通用的插件管理库。

gplug用于统一管理应用程序中的插件。它负责加载、初始化、取消初始化和卸载插件，并支持在运行时添加和删除插件。

gplug旨在将插件接口与用户功能接口分离。插件管理器通过插件规范接口获取插件功能接口，然后可以直接与插件交互。

![plugin](./docs/markdown/pic/plugin.png)

gplug有三个主要部分：插件、插件管理器和插件配置文件。

## Plugin

每个插件都由一个唯一的ID（file key）标识，插件必须实现一个符合插件规范的接口。

## Plugin configuration file

插件管理器使用配置文件以统一的方式管理插件。配置文件包含FileKey、延迟加载等参数。

## Plugin Manager

插件管理器根据文件ID（FileKey）初始化相应的插件，根据接口ID（InterfaceKey）获取插件函数接口函数的指针，并调用不同插件的函数接口。

# 2.接口

gplug文档是使用[Doxygen]生成的(http://www.doxygen.org/).

接口分为两部分，插件管理器接口和插件规范接口。

插件需要实现插件接口，应用程序调用插件管理器接口来实现插件管理。

# 3.开发步骤

开发过程如下：

>定义插件功能接口
>        ↓ 
>实现插件功能接口
>        ↓ 
>导出插件统一接口
>        ↓ 
>创建配置文件
>        ↓ 
>使用PlugMgr接口调用Plugin函数接口

[这是](./docs/markdown/1.DevelopmentProcess/README.md)根据流程开发插件的示例。

# 4.例子

更多示例请看 [这里](./example/README.md).


# 5.编译


1. 克隆仓库

```shell
git clone https://gitee.com/QWorkShop/gplug.git
```

2. 生成必要的构建文件

   在此步骤中，将克隆第三方库。

```cmake
cd EVHttpServer
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=/path/to/install -DCMAKE_BUILD_TYPE=Release
```

3. 编译源代码。在此步骤中，将编译第三方库、gplug库、示例和测试。

```cmake
cmake --build build
```

4. 安装

```cmake
cmake --install build
```

现在，您可以使用gplug库了，在代码中包含头文件，在编译时链接gplug库。



