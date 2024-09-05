# 1.插件管理器配置文件说明

插件管理器通过扫描特定目录(gplugin)下的特定配置文件(gplugin.xml)来搜集需要管理的插件，在配置文件中描述了插件的一些基本属性。配置文件需要遵循以下约定：

1.所有配置文件必须放在gplug库文件模块目录下的gplugin目录下，或者该目录的子目录下。

2.gplugin目录下至少应该存在一个gplugin.xml配置文件。  

3.gplugin配置文件格式

```xml
<?xml version="1.0" encoding="utf-8"?>
<gplug>
	<plugin fkey="CatPlugin" file="../CatPlugin/libCatPlugin.so" delayload="false" />
</gplug>
```

其中的属性含义如下：

- fkey：插件文件标识
- file：插件库文件路径（相对于该配置文件所在目录）
- delayload：true-在调用接口时才加载库文件，false-初始化插件时就立即加载插件库文件

4.在一个配置文件中，可以配置多个插件，例如：

```xml
<?xml version="1.0" encoding="utf-8"?>
<gplug>
	<plugin fkey="DogPlugin" file="../DogPlugin/libDogPlugin.so" delayload="true" />
	<plugin fkey="CatPlugin" file="../CatPlugin/libCatPlugin.so" delayload="false" />
</gplug>
```

5.插件库文件路径配置规则

- 可以使用相对路径和绝对路径
- 如果使用相对路径时，是将gplugin.xml所在目录作为起始目录
- 如果使用相对路径、需以"./"，"../"开头，也可以使用window路径

