应用程序通过插件管理器接口与插件管理器进行管理插件、获取插件信息等交互。

插件管理器通过插件规范接口与插件进行管理插件等交互。

在应用程序获取到插件功能接口集之后，应用程序将直接与插件进行交互。



插件管理器主要提供如下接口：

| GPlugMgr_Init                 | 初始化插件管理器。   |
| ----------------------------- | -------------------- |
| GPlugMgr_Deinit               | 反初始化插件管理器。 |
| GPlugMgr_CreateInstance       | 创建插件实例。       |
| GPlugMgr_DestroyInstance      | 销毁插件实例。       |
| GPlugMgr_QueryInterface       | 获取插件功能接口集。 |
| GPlugMgr_QueryConfigAttribute | 查询插件配置属性     |
| GPlugMgr_QueryAllFkeys        | 查询所有插件keys     |
| GPlugMgr_ReleaseAllFkeys      | 释放查询内存         |

插件提供如下规范接口：

| HPLUGIN_GetPluginInterface   | 获取插件与插件管理器交互的接口集合。 |
| ---------------------------- | ------------------------------------ |
| GPlugin_Init                 | 初始化插件。                         |
| GPlugin_Uninit               | 反初始化插件。                       |
| GPlugin_CreateInstance       | 创建插件实例。                       |
| GPlugin_DestroyInstance      | 销毁插件实例。                       |
| GPlugin_QueryInterface       | 获取插件功能接口集。                 |
| GPlugin_GetAllInterfaceIkeys | 获取插件支持的功能接口集标识列表。   |
| GPlugin_GetFileVersion       | 获取插件库文件版本字串。             |