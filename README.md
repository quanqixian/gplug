# 1.Introduction

gplug is a **general plugin management library**.

gplug is used for unified management of plugins in the application, responsible for loading, initializing, de-initializing, and unloading each plugin, and supports adding and deleting plugin libraries at runtime.

gplug is designed to separate the plugin interface and user function interface.

![plugin](./docs/pic/plugin.png)

插件管理器规定每个插件都有唯一的ID进行标示，应用程序根据ID通过插件管理器对特定的插件进行初始化，获取插件功能接口函数的指针，从而动态调用不同插件的接口函数。

插件管理器采用配置文件的方式对插件进行统一管理，配置文件中标有插件的ID和是否延迟加载等参数信息，而插件的实现必须要符合插件管理器的要求，插件管理器根据配置文件对插件进行加载管理，并通过规范规定各个插件必须实现的接口进行初始化、反初始化、获取功能接口集等操作。

# 2.接口

gplug documentation is generated using  [Doxygen](http://www.doxygen.org/).

接口分为两部分，分为插件管理器接口和插件规范接口。

应用程序通过插件管理器接口初始化各插件，创建插件实例。在应用程序获取到插件功能接口集之后，应用程序将直接与插件进行交互。

每一个插件都应该实现插件规范接口，这样插件管理器才能按照统一的接口调用插件。插件规范接口和插件功能接口是分离开的。插件管理器通过插件规范接口获取插件功能接口，然后就可以直接与插件进行交互。

# 3.Development steps

插件的开发和使用流程如下：

![process](./docs/pic/process.png)

# 4.示例

Check out examples [here](./example/README.md)


# 5.编译



# 6.局限性

gplug目前无法解决不同的插件库所依赖的库的版本冲突问题。

