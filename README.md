<p align="center">
  <img src="./doxygen/Logo.jpg" alt="gplug logo"/>
</p>

# 1.Introduction

[![badge](https://img.shields.io/badge/license-Zlib-blue)](./LICENSE.txt)
[![badge](https://img.shields.io/badge/document-doxygen-brightgreen)](./doc)
[![badge](https://img.shields.io/badge/platform-windows%20%7C%20linux-green)](./README.md)

gplug (general plugin management library) is a general plugin management library.

gplug is used for unified management of plugins in applications. It is responsible for loading, initializing, deinitializing, and unloading individual plugins, and supports adding and removing plugins at runtime.

gplug is designed to separate the plug-in interface from the user function interface. The plugin manager obtains the plugin function interface through the plugin specification interface, and then can directly interact with the plugin.

![plugin](./docs/pic/plugin.png)

There are three main parts in gplug: plugin, plugin manager, and plugin configuration file.

## Plugin

Each plugin is identified by a unique ID (file key), and the plugin must implement an interface that conforms to the plugin specification.

## Plugin configuration file

The plugin manager uses a configuration file to manage the plugins in a unified manner. The configuration file contains parameters such as FileKey , delay loading.

## Plugin Manager

The plugin manager initializes the corresponding plugin according to the file ID (FileKey), obtains the pointer of the plugin function interface function according to the function ID (InterfaceKey), and calls the function interface of different plugins.

# 2.Interface

gplug documentation is generated using  [Doxygen](http://www.doxygen.org/).

The interface is divided into two parts, the plugin manager interface and the plugin specification interface.

A plugin needs to implement the plugin interface, and the application program calls the plugin manager interface to implement plugin management.

# 3.Development steps

The process of developing is as follows:

![process](./docs/pic/process.png)

[This](./docs/2.开发流程/README.md) is an example of developing a plug-in according to the flow.

# 4.Examples

Check out examples [here](./example/README.md)


# 5.Build


1. Clone the repository

```shell
git clone https://gitee.com/QWorkShop/gplug.git
```

2. Generate the necessary build files

   In this step, the third-party library will be cloned.

```cmake
cd EVHttpServer
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=/path/to/install -DCMAKE_BUILD_TYPE=Release
```

3. Compile the source code. In this step, third-party libraries, gplug library, samples, tests will be compiled.

```cmake
cmake --build build
```

4. Install to system

```cmake
cmake --install build
```

Now you can use the gplug library, include the header file in the code, link  the gplug library when compiling.



