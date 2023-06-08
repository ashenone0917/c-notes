chromium内核代码一直在更新，最近又有了大动作。尤其是IPC通信部分，因为性能问题，传统的IPC已经被弃用，虽然不是完全舍弃，但除了严重依赖于时序关系的Navigate相关消息外，其他的所有IPC::Channel都被替换成了mojom。这就导致以前的FrameMsg_Navigate、ResourceHostMsg_RequestSource等IPC消息在chromium代码中不再可见了。因为大体的流程没有改变，有兴趣的可以回顾一下老罗的文章，这里只是从头梳理一下chromium69版本内核代码加载一个url的整个过程，以及该过程中涉及到的一些重要的类和方法。
(摘抄的文章使用的是69内核，但是使用观看文章对照的代码是114内核)
（个人理解得不够深刻，如有问题，请留言指教，不胜感激）  
用户在地址栏输入一个URL，浏览器加载该URL的过程，被称为“Navigate”，所以该过程中会有很多类和方法名中，包含有该单词，如有遇到，可以认为该类或方法很可能和URL加载过程相关。

### 第一阶段（发送请求）：
#### 1. WebContents
 在浏览器中代表一个页面的实体，它是一个抽象类，实现于WebContentsImpl。每个WebContents对象都会有一个NavigationController，它管理着URL的前进/后退列表，负责加载URL到WebContents中。所以，浏览器在导航URL的时候，会首先创建一个WebContentsImpl对象，调用其GetController()方法，获取到其NavigationController成员。获取到与WebContents关联的NavigationController后，浏览器一般会调用它的LoadURLWithParams()方法进行目标URL的加载工作。
 ```cpp
 class CONTENT_EXPORT WebContentsImpl : public WebContents,
                                       public FrameTree::Delegate,
                                       public RenderFrameHostDelegate,
                                       public RenderViewHostDelegate,
                                       public RenderWidgetHostDelegate,
                                       public RenderFrameHostManager::Delegate,
                                       public PageDelegate,
                                       public blink::mojom::ColorChooserFactory,
                                       public NavigationControllerDelegate,
                                       public NavigatorDelegate,
                                       public ui::NativeThemeObserver,
                                       public ui::ColorProviderSourceObserver {
                                            ...
                                       }
```
#### 2. NavigationController
每个WebContents里都关联着一个NavigationController对象，且每个NavigationController也只被关联在一个WebContents中，二者一一对应。NavigationController也是一个抽象类，它的实现类的类名，也是在后面加个"Impl"，即NavigationControllerImpl。当其成员函数LoadURLWithParams()方法被调用后，它会在进行简单的url判断后，调用成员函数NavigateWithoutEntry()继续处理。该函数实现里主要做了三件事：
```cpp
class CONTENT_EXPORT NavigationControllerImpl : public NavigationController {
  ...
}
```
1. 确定目标URL所要加载在FrameTree中的哪个node上，即得到一个FrameTreeNode对象:
```cpp
base::WeakPtr<NavigationHandle> NavigationControllerImpl::NavigateWithoutEntry(
    const LoadURLParams& params) {
  // Find the appropriate FrameTreeNode.
  FrameTreeNode* node = nullptr;
  if (params.frame_tree_node_id != RenderFrameHost::kNoFrameTreeNodeId ||
      !params.frame_name.empty()) {
    node = params.frame_tree_node_id != RenderFrameHost::kNoFrameTreeNodeId
               ? frame_tree_->FindByID(params.frame_tree_node_id)
               : frame_tree_->FindByName(params.frame_name);
    DCHECK(!node || &node->frame_tree() == &frame_tree());
  }

  // If no FrameTreeNode was specified, navigate the main frame.
  if (!node)
    node = frame_tree_->root();
  ...
}
```

2. 创建一个NavigationEntry对象，用于组建一个NavigationRequest；
```cpp
base::WeakPtr<NavigationHandle> NavigationControllerImpl::NavigateWithoutEntry(
    const LoadURLParams& params) {
    ...
    std::unique_ptr<NavigationRequest> request =
      CreateNavigationRequestFromLoadParams(
          node, params, override_user_agent, should_replace_current_entry,
          params.has_user_gesture, network::mojom::SourceLocation::New(),
          reload_type, pending_entry_, pending_entry_->GetFrameEntry(node),
          navigation_start_time);
    ...
    node->navigator().Navigate(std::move(request), reload_type);//frame tree node
    ...
}
```
3. 输送NavigationRequest到目标FrameTreeNode的NavigatorImpl对象的Navigate()方法中。
```cpp
base::WeakPtr<NavigationHandle> NavigationControllerImpl::NavigateWithoutEntry(
    const LoadURLParams& params) {
    ...
    node->navigator().Navigate(std::move(request), reload_type);//frame tree node
    ...
}
```
###### 2.1 FrameTree
FrameTree（帧树）是浏览器引擎中用于组织和管理网页框架（Frames）的数据结构。在浏览器中，一个网页可以包含多个嵌套的框架，每个框架都是一个独立的文档环境，可以加载和显示不同的网页内容(使用iframe)。

FrameTree 提供了一个层次结构，用于表示和管理这些嵌套的框架。它是一个树形结构，其中每个节点代表一个框架，节点之间通过父子关系进行连接。树的根节点表示整个浏览器窗口的主框架，而其他节点表示嵌套的子框架。主节点称作main frame

当浏览器解析到一个 <iframe> 元素时，它会创建一个对应的 Frame Tree 节点，并为该节点分配一个 Frame ID。随后，浏览器会继续解析该 <iframe> 元素的 src 属性，获取要加载的嵌套页面的地址。因此，在加载嵌套页面之前，每个 <iframe> 元素都会被分配一个唯一的 Frame ID。

对于主框架（主文档），由于它通常没有 <iframe> 标签，因此无法在解析 <iframe> 元素时为其分配 Frame ID。相反，对于主框架，浏览器会为其创建一个称为 "root" 的 Frame Tree 节点，并将其视为主框架的节点。

总结来说，在浏览器解析过程中，当遇到 <iframe> 元素时，会创建对应的 Frame Tree 节点并分配 Frame ID，然后继续解析该 <iframe> 元素的内容。而对于主框架，浏览器会创建一个根节点（root）表示主框架。
###### 2.2 FrameTreeNode
Frame Tree Node ID（帧树节点 ID）是由浏览器生成的标识符，用于唯一标识 Frame Tree（帧树）中的每个节点。它不是由网页指定的，而是由浏览器在解析和组织网页框架时生成的。每个 Frame Tree 节点都会被分配一个唯一的节点 ID，用于在内部进行引用和管理。
  
#### 3. Navigator
该类负责在一棵FrameTree的节点中执行URL导航操作，可以被同一棵FrameTree上的多个FrameTreeNode所共享(例如同一个标签页的多个iframe)，但不能被多棵FrameTree的子节点所共享(例如不同的浏览器窗口或标签页)。~~该类是一个抽象类，实现类为NavigatorImpl~~。Navigate()方法中，先判断当前指定的FrameTreeNode所代表的网页中是否有悬挂的BeforeUnload事件处理器需要执行，如果有，则先执行BeforeUnload事件处理程序，稍后派发NavigationRequest到FrameTreeNode；如果没有，则立即派发。派发形式如下：
```cpp
class CONTENT_EXPORT Navigator {
   ...
}
```
1. 调用FrameTreeNode的~~CreateNavigationRequest~~TakeNavigationRequest()方法，将NavigationRequest对象存储；
2. 调用FrameTreeNode中NavigationRequest对象的BeginNavigation()方法进行加载。

```cpp
void Navigator::Navigate(std::unique_ptr<NavigationRequest> request,
                         ReloadType reload_type) {
  ....
  // 检查是否需要触发 beforeunload 事件，并确定是否需要等待渲染器的响应
  bool no_dispatch_because_avoid_unnecessary_sync = false;
  bool should_dispatch_beforeunload =
      !NavigationTypeUtils::IsSameDocument(
          request->common_params().navigation_type) &&
      !request->common_params().is_history_navigation_in_new_child_frame &&
      frame_tree_node->current_frame_host()->ShouldDispatchBeforeUnload(
          false /* check_subframes_only */,
          &no_dispatch_because_avoid_unnecessary_sync);

  int nav_entry_id = request->nav_entry_id();
  bool is_pending_entry =
      controller_.GetPendingEntry() &&
      (nav_entry_id == controller_.GetPendingEntry()->GetUniqueID());
  //调用FrameTreeNode的TakeNavigationRequest()方法，将NavigationRequest对象存储；
  frame_tree_node->TakeNavigationRequest(std::move(request));
  DCHECK(frame_tree_node->navigation_request());

  // 如果需要触发 beforeunload 事件，则调度渲染器的 beforeunload 事件
  if (should_dispatch_beforeunload) {
    frame_tree_node->navigation_request()->SetWaitingForRendererResponse();
    frame_tree_node->current_frame_host()->DispatchBeforeUnload(
        RenderFrameHostImpl::BeforeUnloadType::BROWSER_INITIATED_NAVIGATION,
        reload_type != ReloadType::NONE);
  } else {
    // 如果避免了不必要的同步，则记录避免同步前的导航开始时间
    if (no_dispatch_because_avoid_unnecessary_sync) {
      LogNavigationStartToBeginWithAvoidUnnecessaryBeforeUnloadSync(
          base::TimeTicks::Now() - frame_tree_node->navigation_request()
                                       ->common_params()
                                       .navigation_start);
    }
    // 开始导航
    frame_tree_node->navigation_request()->BeginNavigation();
  }

  // 确保 RFH::Navigate 中不会清除挂起的导航条目
  if (is_pending_entry)
    CHECK_EQ(nav_entry_id, controller_.GetPendingEntry()->GetUniqueID());
}
```
#### 4. NavigationRequest
该类存在于UI线程，确保URL请求会在IO线程中的ResourceDispatcherHost中执行，描述UI线程和IO线程之间的交互。该类先对目标URL进行了内容安全策略检查，以及注册了各种NavigationThrottles对目标URL进行审批，最终会创建一个NavigationURLLoader对象。
```cpp
void NavigationThrottleRunner::RegisterNavigationThrottles() {
 ...
}
```
#### 5. NavigationURLLoader

该类实现类NavigationURLLoaderImpl构造函数中，进行线程调度，并在该类中调用ThrottlingURLLoader::CreateLoaderAndStart()方法创建了一个ThrottlingURLLoader对象。
#### 6. ThrottlingURLLoader

该类继承于network::mojom::URLLoaderClient，可以进行IPC通信，且Render进程和Browser进程都有其实例化对象。类名中带着"Throttling"的，且和URL加载相关的类，通常都会根据某些自定义规则，对网络数据进行拦截过滤处理，就像一个瓶塞一样。它的CreateLoaderAndStart()方法，创建完自身的一个实例对象后，调用其Start()方法。Start方法接收一个SharedURLLoaderFactory类实例(该实例是在NavigationURLLoaderImpl的StartWithNetworkService()方法中创建的)，并调用SharedURLLoaderFactory实例的CreateLoaderAndStart()。
```cpp
class BLINK_COMMON_EXPORT ThrottlingURLLoader
    : public network::mojom::URLLoaderClient {
     ...
 std::unique_ptr<ThrottlingURLLoader> ThrottlingURLLoader::CreateLoaderAndStart(
    scoped_refptr<network::SharedURLLoaderFactory> factory,
    std::vector<std::unique_ptr<URLLoaderThrottle>> throttles,
    int32_t request_id,
    uint32_t options,
    network::ResourceRequest* url_request,
    network::mojom::URLLoaderClient* client,
    const net::NetworkTrafficAnnotationTag& traffic_annotation,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner,
    absl::optional<std::vector<std::string>> cors_exempt_header_list) {
  DCHECK(url_request);
  std::unique_ptr<ThrottlingURLLoader> loader(new ThrottlingURLLoader(
      std::move(throttles), client, traffic_annotation));
  loader->Start(std::move(factory), request_id, options, url_request,
                std::move(task_runner), std::move(cors_exempt_header_list));
  return loader;
}
 
 void ThrottlingURLLoader::Start(
    scoped_refptr<network::SharedURLLoaderFactory> factory,
    int32_t request_id,
    uint32_t options,
    network::ResourceRequest* url_request,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner,
    absl::optional<std::vector<std::string>> cors_exempt_header_list) {
  ...

  start_info_ = std::make_unique<StartInfo>(factory, request_id, options,
                                            url_request, std::move(task_runner),
                                            std::move(cors_exempt_header_list));

  if (deferred)
    deferred_stage_ = DEFERRED_START;
  else
    StartNow();
  ...
}
 
 void ThrottlingURLLoader::StartNow() {
  ...
  DCHECK(start_info_->url_loader_factory);
  start_info_->url_loader_factory->CreateLoaderAndStart(
      url_loader_.BindNewPipeAndPassReceiver(start_info_->task_runner),
      start_info_->request_id, start_info_->options, start_info_->url_request,
      client_receiver_.BindNewPipeAndPassRemote(start_info_->task_runner),
      net::MutableNetworkTrafficAnnotationTag(traffic_annotation_));
  ...
}
}
```
### 以下都是原文，过了一遍，没时间对照源码了
#### 7. SingleRequestURLLoaderFactory
该类继承于network::SharedURLLoaderFactory，而SharedURLLoaderFactory又继承于mojom::URLLoaderFactory。URLLoaderFactory这一系列的近亲类(比如WebUIURLLoaderFactory、FileURLLoaderFactory、CORSURLLoaderFactory等)，都可以创建一个mojom::URLLoader对象，既可以跨进程加载url并得到返回数据，又可以同进程加载url，该性质来自于mojom的调用机制。
 
SingleRequestURLLoaderFactory的CreateLoaderAndStart方法中执行回调函数，调用堆栈返回到了URLLoaderRequestController::CreateNonNetworkServiceURLLoader()，该方法调用ResourceDispatcherHostImpl类的BeginNavigationRequest()。
```cpp
class COMPONENT_EXPORT(NETWORK_CPP) SingleRequestURLLoaderFactory
    : public network::SharedURLLoaderFactory {
}
```
 
#### 8. ResourceDispatcherHostImpl
114内核没有这个对象
 
ResourceDispatcher和ResourseDispatcherHost分别是Render进程和Borwser进程进行资源分发的接口类。
 
在BeginNavigationRequest()方法中创建了一个URLRequest对象，在BeginRequestInternal()方法中创建了一个ResourceLoader对象，然后在StartLoading()方法中，调用ResourceLoader对象的StartRequest()方法开始加载请求。
 
#### 9. ResourceLoader
没找到，后面找有时间找找
该类集中接收转发URLRequest、SSLErrorHandler、SSLClientAuthHandler、ResourceHandler相关的事件。
该类StartRequestInternal()方法中，直接调用了URLRequest对象的Start()方法，至此结束了Navigate()的第一个过程。

### 第二阶段（数据响应）：

网络模块获取到响应头数据，数据流向及处理方法。
ResourceLoader类的ResponseCompleted()方法被调用，然后通过ResourceLoader成员变量handler的OnResponseCompleted()方法向上传递数据。主要的handler类有MimeSniffingResourceHandler、CrossSiteDocumentResourceHandler、InterceptingResourceHandler、MojoAsyncResourceHandler等，各个handler都可以对数据进行截获处理，最终NavigationRequest类的OnResponseStarted()方法被调用。该方法最终调用到RenderFrameHostImpl::CommitNavigation()，RenderFrameHostImpl发送了一个IPC消息到Render进程。

第三阶段（Render进程发起主要资源(一般指html文件)网络请求）：

1. RenderFrameImpl

      CommitNavigation()函数除了携带response_header、request_params等基本信息，还有mojom通信相关接口url_loader_client_endpoints和Browser进程目前所支持的URLLoaderFactory列表subresource_loader_factories。关于mojom接口的绑定过程，参考Converting Legacy Chrome IPC To Mojo一文，这里不详细赘述。参数subresource_loader_factories是一个Bundle，包裹着从Browser进程传递过来的各种URLLoaderFactory，前面说过，URLLoaderFactory可以跨进程进行资源请求，而不同的URLLoaderFactory用来请求不同scheme的资源。比如WebUIURLLoaderFactory用来请求浏览器内置页面，url格式一般类似于chrome://page；再比如FileSystemURLLoaderFactory用来请求本地资源，url格式类似于file:///C:\\test.txt。进行网络请求的时候，Render进程去factory列表里根据url的scheme里查找对应的URLLoaderFactory，调用它的CreateLoaderAndStart()方法进行资源请求。
      RenderFrameImpl类的CommitNavigation()方法被mojom消息调起，它根据消息携带的header信息、request参数信息以及url_loader_client_endpoints创建一个WebURLRequest对象，并调用成员变量frame_的CommitNavigation()将其传递过去。RenderFrameImpl的成员变量frame_指向了WebLocalFrameImpl，WebLocalFrameImpl接收到WebURLRequest对象后，将其转换成FrameLoadRequest类型，然后调用传递给FrameLoader类的CommitNavigation()函数。

2. FrameLoader

       CommitNavigation()接收到FrameLoadRequest后，直接调用了StartLoad()函数，在StartLoad()函数中，创建并用FrameLoadRequest参数初始化了一个DocumentLoader对象，然后调起DocumentLoader对象的StartLoading()方法。

3. DocumentLoader

       StartLoading()函数准备好request、fetcher等参数，调用RawResource类的静态方法FetchMainResource()去请求主要资源。

4. RawResource

      FetchMainResource()函数，根据Resoure::kMainResource类型去创建一个ResourceFactory对象，同FetchParameters对象一起作为参数，调起参数列表中fetcher的RequestResource()函数。

5. ResourceFetcher

    RequestResource()函数创建Resource对象，调用StartLoad()方法。StartLoad()方法创建一个ResourceLoader对象，调用loader的Start()方法。

6. ResourceLoader

   Start()方法调用ResourceLoaderScheduler::Request()，最终回调到ResourceLoader::StartWith()方法中。ResourceLoader的StartWith()调用WebURLLoaderImpl类的LoadAsynchronously()方法。

7. WebURLLoaderImpl

    LoadAsynchronously()通过自己的成员变量context_，对request进行加载。最终从WebURLLoaderImpl::Context::Start()方法中调用了ResourceDispatcher类的StartAsync()方法。

8. ResourceDispatcher

    调用ThrottlingURLLoader类的CreateLoaderAndStart()方法。

9. ThrolltingURLLoader

    Start()函数接收一个SharedURLLoaderFactory和一个ResourceRequest参数，调用factory的CreateLoaderAndStart()方法。

10. ChildURLLoaderFactoryBundle

   CreateLoaderAndStart()方法被调起，参数包含network::ResourceRequest和一个network::mojom::URLLoaderRequest对象，根据request.url获取对应的URLLoaderFactory，然后调用该factory的CreateLoaderAndStart()发送跨进程IPC消息到Browser Process。

 

第四阶段（请求主要资源）：

   Browser进程接收CreateLoaderAndStart()方法的跨进程调用的位置，是在ResourceMessageFilter类的同名方法CreateLoaderAndStart()。 该类有一个成员变量url_loader_factory_，指向CORSURLLoaderFactory，且该类的CreateLoaderAndStart()方法被调用。最终传递到URLLoaderFactoryImpl::CreateLoaderAndStart()，该方法获取全局的ResourceDispatcherHostImpl实例，调用其OnRequestResourceWithMojo()，代替以前的ResourceHostMsg_RequestResource消息。ResourceDispatcherHostImpl接收来自Render进程的网络请求相关参数，调用OnRequestResourceInternal()方法开始对该请求进行加载。过程同第一阶段相同。

    当数据请求有结果时，同样是几个Handler类的OnReadCompleted()方法最先被调用，然后通过network::mojom::URLLoaderClientProxy类的OnStartLoadingResponseBody()方法将数据结果跨进程通知回Render进程。

第五阶段（接收处理主要资源，发起子资源请求）：

同名方法OnStartLoadingResponseBody()被调用，分别经过以下几个类，最终到达HTMLTreeBuilder:
URLResponseBodyConsumer::OnReadable()
WebURLLoaderImpl::RequestPeerImpl::OnReceivedData()
WebURLLoaderImpl::Context::OnReceivedData()
ResourceLoader::DidReceviedData()
RawResource::AppendData()
DocumentLoader::DataReceived()   ::ProcessData()   ::CommitData()  ::InstallNewDocument()
HTMLDocumentParser::AppendBytes()  ::PumpPendingSpeculations()  ::ProcessTokenizedChunkFromBackgroundParser()
HTMLTreeBuilder::ConstructTree()

在构建DOM树的时候，如果发现一个子节点需要加载资源，比如css文件。则HTMLDocumentParser类的DocumentElementAvailable()方法会被调用，然后调用自身的资源预加载器preloader_的TakeAndPreload()对资源进行加载。之后的调用过程如下：
HTMLResourcePreloader::Preload()
PreloadRequest::Start()
DocumentLoader::StartPreload()  
CSSStyleSheetResource::Fetch() 
ResourceFetcher::RequestResource()
过程同上.....
ChildURLLoaderFactoryBundle::CreateLoaderAndStart()

 

 

以下是堆栈调用的一个大概过程：

content::NavigationControllerImpl::LoadURLWithParams()  
content::NavigationControllerImpl::NavigateWithoutEntry()  
content::NavigatorImpl::Navigate()  
content::NavigationRequest::BeginNavigation()  
content::NavigationHandleImpl::WillStartRequest()  
content::NavigationRequest::OnStartCehcksComplete()  
content::NavigationURLLoader::Create()  
content::NavigationURLLoaderImpl::NavigationURLLoaderImpl()  
content::NavigationURLLoaderImpl::StartWithoutNetworkService()  
content::ThrottlingURLLoader::CreateLoaderAndStart()  
content::ThrottlingURLLoader::Start()  

content::ThrottlingURLLoader::StartNow()  
content::SingleRequestURLLoaderFactory::CreateLoaderAndStart()  
    
content::SingleRequestURLLoaderFactory::HandleRequest()  
content::NavigationURLLoaderImpl::URLLoaderRequestController::CreateNonNetworkServiceURLLoader();  
      
content::ResourceDispatcherHostImpl::BeginNavigationRequest()  
content::ResourceDispatcherHostImpl::BeginNavigationRequestInternal()   -->  content::ResourceLoader::ResourceLoader()  
content::ResourceDispatcherHostImpl::StartLoading()  

content::ResourceLoader::StartRequest()       
content::ResourceLoader::ScopedDeferral::~ScopedDeferral()   //判断状态  
content::ResourceLoader::Resume()        
content::ResourceLoader::StartRequestInternal()  
net::URLRequest::Start()   


content::NavigationURLLoaderImpl::OnReceiveResponse()  //receive response contain body
content::NavigationRequest::OnResponseStarted()  
content::NavigationHandleImpl::WillProcessResponse()  
content::NavigationRequest::OnWillProcessResponseChecksComplete()  
content::NavigationRequest::CommitNavigation()        
content::RenderFrameHostImpl::CommitNavigation()  
content::mojom::FrameNavigationControlProxy::CommitNavigation()         // Send IPC Message To Render Process


-----------------Render Process-------------------------------------
.........
content::RenderFrameImpl::CommitNavigation()  
blink::WebLocalFrameImpl::CommitNavigation()  
blink::FrameLoader::CommitNavigation()  
blink::FrameLoader::StartLoad()  
blink::DocumentLoader::StartLoading()  
blink::RawResource::FetchMainResource()  
blink::ResourceFetcher::RequestResource()  
    
blink::ResourceFetcher::StartLoad()  
blink::ResourceLoader::Start()  
blink::ResourceLoaderScheduler::Request()  
blink::ResourceLoaderScheduler::Run()  
blink::ResourceLoader::Run()  
blink::ResourceLoader::StartWith(blink::ResourceRequest& request)  
content::WebURLLoaderImpl::LoadAsynchronously()  
content::WebURLLoaderImpl::Context::Start()  
content::ResourceDispatcher::StartAsync()  
content::ThrottlingURLLoader::CreateLoaderAndStart()  
content::ThrottlingURLLoader::Start()  
content::ThrottlingURLLoader::StartNow()  
content::ChildURLLoaderFactoryBundle::CreateLoaderAndStart()  
network::mojom::URLLoaderFactoryProxy::CreateLoaderAndStart()      
    
    
-------------------------------------------------Browser Process-----------------
content::ResourceMessageFilter::CreateLoaderAndStart()  
network::cors::CORSURLLoaderFactory::CreateLoaderAndStart()  
content::URLLoaderFactoryImpl::CreateLoaderAndStart()  
content::ResourceDispatcherHostImpl::OnRequestResourceWithMojo()  
content::ResourceDispatcherHostImpl::OnRequestResourceInternal()  
content::ResourceDispatcherHostImpl::BeginRequest()  
content::ResourceDispatcherHostImpl::StartLoading()  
content::ResourceLoader::StartRequest()        
content::ResourceLoader::ScopedDeferral::~ScopedDeferral()   //判断状态  
content::ResourceLoader::Resume()        
content::ResourceLoader::StartRequestInternal()  
net::URLRequest::Start()      
    
    
content::LayeredResourceHandler::OnReadCompleted()  
content::InterceptingResourceHandler::OnReadCompleted()  
maxthon::MxResourceSnifferHandler::OnReadCompleted()  
content::MojoAsyncResourceHandler::OnReadCompleted()  
network::mojom::URLLoaderClientProxy::OnStartLoadingResponseBody()  


-----------------------Renderer Process-------------------------------------       
content::URLLoaderClientImpl::OnStartLoadingResponseBody()  
content::URLResponseBodyConsumer::OnReadable()        
content::WebURLLoaderImpl::RequestPeerImpl::OnReceivedData()        
content::WebURLLoaderImpl::Context::OnReceivedData()  
blink::ResourceLoader::DidReceiveData()        
blink::RawResource::AppendData()        
blink::Resource::AppendData()        
blink::DocumentLoader::DataReceived()  
blink::DocumentLoader::ProcessData()  
blink::DocumentLoader::CommitData()        
    blink::HTMLDocumentParser::AppendBytes()    
    blink::DocumentLoader::CommitNavigation()  
blink::DocumentLoader::InstallNewDocument()      
    
blink::HTMLDocumentParser::PumpPendingSpeculations()  
blink::HTMLDocumentParser::ProcessTokenizedChunkFromBackgroundParser()  
blink::HTMLTreeBuilder::ConstructTree()  
blink::HTMLTreeBuilder::ProcessToken()  
blink::HTMLTreeBuilder::ProcessStartTag()  
blink::HTMLConstructionSite::InsertHTMLHtmlStartTagBeforeHTML()  
blink::HTMLHtmlElement::InsertedByParser()  
blink::HTMLDocumentParser::DocumentElementAvailable()  
blink::ResourcePreloader::TakeAndPreload()  
blink::HTMLResourcePreloader::Preload()  
blink::PreloadRequest::Start()  
blink::DocumentLoader::StartPreload()        
blink::CSSStyleSheetResource::Fetch()   
blink::ResourceFetcher::RequestResource()  
blink::ResourceFetcher::StartLoad()  
blink::ResourceLoader::Start()  
blink::ResourceLoaderScheduler::Request() 
blink::ResourceLoaderScheduler::Run()  
blink::ResourceLoader::Run()  
blink::ResourceLoader::StartWith(blink::ResourceRequest& request)  
content::WebURLLoaderImpl::LoadAsynchronously()  
content::WebURLLoaderImpl::Context::Start()  
content::ResourceDispatcher::StartAsync()  
content::ThrottlingURLLoader::CreateLoaderAndStart()  
content::ThrottlingURLLoader::Start()  
content::ThrottlingURLLoader::StartNow()  
content::ChildURLLoaderFactoryBundle::CreateLoaderAndStart(n)  
network::mojom::URLLoaderFactoryProxy::CreateLoaderAndStart()    
 
 

从render_view_impl.cc开始说起。

1.     方法RenderViewImpl::Initialize中有：
WebLocalFrame* web_frame = WebLocalFrame::create(main_render_frame_.get()); 

这里会创建WebLocalFrame对象。

之后有webwidget_ = WebView::create(this); 这里会创建WebViewImpl对象。且后面将两者关联起来。

2. 我们来先来看看WebLocalFrame::create
    1. WebLocalFrameImpl的构造函数中，创建FrameLoaderClientImpl对象。


3. WebViewImpl的创建
WebViewImpl在创建中，会创建Page对象。一个WebViewImpl对应一个Page。

4.WebViewImpl的setMainFrame
     这里的调用逻辑如下：

WebViewImpl::setMainFrame
WebLocalFrameImpl::initializeCoreFrame
LocalFrame::LocalFrame

在LocalFrame的构造函数中，会创建FrameLoader对象和ScriptController对象
4. Document对象及相关逻辑
FrameLoader::init()和   FrameLoader::startLoad函数中，创建了多个DocumentLoader对象。

  

RawResource::didAddClient
DocumentLoader::dataReceived
DocumentLoader::commitData
DocumentLoader::ensureWriter
DocumentLoader::createWriterFor
(LocalDOMWindow::create)
LocalDOMWindow::installNewDocument
LocalDOMWindow::createDocument
Document::Document

5.归纳
    1. Local Frame主要处理逻辑，含有FrameView对象，用于处理内容显示。FrameView继承ScrollViewArea类，故，FrameView内容可超过屏幕。
      （未完待续）     

> https://blog.csdn.net/u011882998/article/details/26150415
>> https://www.cnblogs.com/bigben0123/p/14606972.html
