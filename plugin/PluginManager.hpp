//存储当前插件列表
std::share_ptr<PluginList> g_sPluginList;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  reasonForCall, 
                       LPVOID /*lpReserved*/ )
{
    switch (reasonForCall)
    {
        case DLL_PROCESS_ATTACH:
        {
          //初始化一些信息
        }	
        case DLL_PROCESS_DETACH:
        {
          //保存配置
          saveSettings();
        }
        case DLL_THREAD_ATTACH:
            break;
            
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}


//导出一个函数传入一些内部所需要的外界信息，包括外部窗口句柄等信息
//通过窗口句柄和SendMessage可以获得许多信息
extern "C" __declspec(dllexport) void setInfo(MainWindowInfo main_window_info){
  
}

//需要更新插件信息时调用此函数
extern "C" __declspec(dllexport) void beNotified(PluginManagerNotify* plugin_manager_notify){
  
}

//读取配置
void loadSettings(){
  
}

//保存配置
void saveSettings(){
  
}


