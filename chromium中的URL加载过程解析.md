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
该类负责在一棵FrameTree的节点中执行URL导航操作，可以被同一棵FrameTree上的多个FrameTreeNode所共享，但不能被多棵FrameTree的子节点所共享。~~该类是一个抽象类，实现类为NavigatorImpl~~。Navigate()方法中，先判断当前指定的FrameTreeNode所代表的网页中是否有悬挂的BeforeUnload事件处理器需要执行，如果有，则先执行BeforeUnload事件处理程序，稍后派发NavigationRequest到FrameTreeNode；如果没有，则立即派发。派发形式如下：
```cpp
class CONTENT_EXPORT Navigator {
   ...
}
```
1. 调用FrameTreeNode的CreateNavigationRequest()方法，将NavigationRequest对象存储；
2. 调用FrameTreeNode中NavigationRequest对象的BeginNavigation()方法进行加载。
BeforeUnload事件的判断处理，是在第一步和第二步中间。
