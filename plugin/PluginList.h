class PluginList
{
  //初始化一些信息
  void PluginList();
  //传入外部信息
  void init(MainWindowInfo* main_window_info);
  //读取xml
  BOOL parsePluginFile(CONST TCHAR *filename);
  //获取插件列表(xml)
  void downPluginList();
  //下载插件
  BOOL downPlugin(tstring& name);
  //安装插件
  BOOL installPlugin(tstring& name);
  //获取插件指针
  Plugin* getPlugin(tstring& name);
}
