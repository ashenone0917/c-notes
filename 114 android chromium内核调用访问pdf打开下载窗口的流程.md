```cpp
void NavigationURLLoaderImpl::OnReceiveResponse(network::mojom::URLResponseHeadPtr head,
    mojo::ScopedDataPipeConsumerHandle response_body,
    absl::optional<mojo_base::BigBuffer> cached_metadata);
void NavigationURLLoaderImpl::CallOnReceivedResponse(
    network::mojom::URLResponseHeadPtr head,
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    bool is_download);
void NavigationURLLoaderImpl::NotifyResponseStarted(
    network::mojom::URLResponseHeadPtr response_head,
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    mojo::ScopedDataPipeConsumerHandle response_body,
    const GlobalRequestID& global_request_id,
    bool is_download);
void NavigationRequest::OnResponseStarted(
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    network::mojom::URLResponseHeadPtr response_head,
    mojo::ScopedDataPipeConsumerHandle response_body,
    GlobalRequestID request_id,
    bool is_download,
    net::NetworkAnonymizationKey network_anonymization_key,
    absl::optional<SubresourceLoaderParams> subresource_loader_params,
    EarlyHints early_hints);
void NavigationRequest::SelectFrameHostForOnResponseStarted(
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    bool is_download,
    absl::optional<SubresourceLoaderParams> subresource_loader_params);    
void NavigationRequest::WillProcessResponse();
void NavigationThrottleRunner::ProcessNavigationEvent(Event event);
void NavigationThrottleRunner::ProcessInternal();
void NavigationThrottleRunner::InformDelegate(
    const NavigationThrottle::ThrottleCheckResult& result);
void NavigationRequest::OnNavigationEventProcessed(
    NavigationThrottleRunner::Event event,
    NavigationThrottle::ThrottleCheckResult result);
void NavigationRequest::OnWillProcessResponseProcessed(
    NavigationThrottle::ThrottleCheckResult result);    
void NavigationRequest::OnWillProcessResponseChecksComplete(
    NavigationThrottle::ThrottleCheckResult result);
void DownloadManagerImpl::InterceptNavigation(
    std::unique_ptr<network::ResourceRequest> resource_request,
    std::vector<GURL> url_chain,
    network::mojom::URLResponseHeadPtr response_head,
    mojo::ScopedDataPipeConsumerHandle response_body,
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    net::CertStatus cert_status,
    int frame_tree_node_id,
    bool from_download_cross_origin_redirect);
void DownloadManagerImpl::InterceptNavigationOnChecksComplete(
    int frame_tree_node_id,
    std::unique_ptr<network::ResourceRequest> resource_request,
    std::vector<GURL> url_chain,
    net::CertStatus cert_status,
    network::mojom::URLResponseHeadPtr response_head,
    mojo::ScopedDataPipeConsumerHandle response_body,
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    bool is_download_allowed);
void InProgressDownloadManager::InterceptDownloadFromNavigation(
    std::unique_ptr<network::ResourceRequest> resource_request,
    int render_process_id,
    int render_frame_id,
    const std::string& serialized_embedder_download_data,
    const GURL& tab_url,
    const GURL& tab_referrer_url,
    std::vector<GURL> url_chain,
    net::CertStatus cert_status,
    network::mojom::URLResponseHeadPtr response_head,
    mojo::ScopedDataPipeConsumerHandle response_body,
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    std::unique_ptr<network::PendingSharedURLLoaderFactory>
        pending_url_loader_factory);
void CreateDownloadHandlerForNavigation(
    base::WeakPtr<InProgressDownloadManager> download_manager,
    std::unique_ptr<network::ResourceRequest> resource_request,
    int render_process_id,
    int render_frame_id,
    const std::string& serialized_embedder_download_data,
    const GURL& tab_url,
    const GURL& tab_referrer_url,
    std::vector<GURL> url_chain,
    net::CertStatus cert_status,
    network::mojom::URLResponseHeadPtr response_head,
    mojo::ScopedDataPipeConsumerHandle response_body,
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    std::unique_ptr<network::PendingSharedURLLoaderFactory>
        pending_url_loader_factory,
    const URLSecurityPolicy& url_security_policy,
    mojo::PendingRemote<device::mojom::WakeLockProvider> wake_lock_provider,
    const scoped_refptr<base::SingleThreadTaskRunner>& main_task_runner);
void ResourceDownloader::InterceptNavigationResponse(
    base::WeakPtr<UrlDownloadHandler::Delegate> delegate,
    std::unique_ptr<network::ResourceRequest> resource_request,
    int render_process_id,
    int render_frame_id,
    const std::string& serialized_embedder_download_data,
    const GURL& tab_url,
    const GURL& tab_referrer_url,
    std::vector<GURL> url_chain,
    net::CertStatus cert_status,
    network::mojom::URLResponseHeadPtr response_head,
    mojo::ScopedDataPipeConsumerHandle response_body,
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory,
    const URLSecurityPolicy& url_security_policy,
    mojo::PendingRemote<device::mojom::WakeLockProvider> wake_lock_provider,
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner)；
void ResourceDownloader::InterceptResponse(
    std::vector<GURL> url_chain,
    net::CertStatus cert_status,
    network::mojom::URLResponseHeadPtr response_head,
    mojo::ScopedDataPipeConsumerHandle response_body,
    network::mojom::URLLoaderClientEndpointsPtr endpoints);   
void DownloadResponseHandler::OnReceiveResponse(
    network::mojom::URLResponseHeadPtr head,
    mojo::ScopedDataPipeConsumerHandle body,
    absl::optional<mojo_base::BigBuffer> cached_metadata);
void DownloadResponseHandler::OnResponseStarted(
    mojom::DownloadStreamHandlePtr stream_handle);
void ResourceDownloader::OnResponseStarted(
    std::unique_ptr<DownloadCreateInfo> download_create_info,
    mojom::DownloadStreamHandlePtr stream_handle)；
void InProgressDownloadManager::OnUrlDownloadStarted(
    std::unique_ptr<DownloadCreateInfo> download_create_info,
    std::unique_ptr<InputStream> input_stream,
    URLLoaderFactoryProvider::URLLoaderFactoryProviderPtr
        url_loader_factory_provider,
    UrlDownloadHandlerID downloader,
    DownloadUrlParameters::OnStartedCallback callback);    
void InProgressDownloadManager::StartDownload(
    std::unique_ptr<DownloadCreateInfo> info,
    std::unique_ptr<InputStream> stream,
    URLLoaderFactoryProvider::URLLoaderFactoryProviderPtr
        url_loader_factory_provider,
    DownloadJob::CancelRequestCallback cancel_request_callback,
    DownloadUrlParameters::OnStartedCallback on_started);
void DownloadManagerImpl::StartDownloadItem(
    std::unique_ptr<download::DownloadCreateInfo> info,
    download::DownloadUrlParameters::OnStartedCallback on_started,
    download::InProgressDownloadManager::StartDownloadItemCallback callback);
void InProgressDownloadManager::StartDownloadWithItem(
    std::unique_ptr<InputStream> stream,
    URLLoaderFactoryProvider::URLLoaderFactoryProviderPtr
        url_loader_factory_provider,
    DownloadJob::CancelRequestCallback cancel_request_callback,
    std::unique_ptr<DownloadCreateInfo> info,
    DownloadItemImpl* download,
    bool should_persist_new_download);
void DownloadItemImpl::Start(
    std::unique_ptr<DownloadFile> file,
    DownloadJob::CancelRequestCallback cancel_request_callback,
    const DownloadCreateInfo& new_create_info,
    URLLoaderFactoryProvider::URLLoaderFactoryProviderPtr
        url_loader_factory_provider);
 void DownloadItemImpl::OnDownloadFileInitialized(DownloadInterruptReason result,
                                                 int64_t bytes_wasted);
void DownloadItemImpl::DetermineDownloadTarget();
void DownloadManagerImpl::DetermineDownloadTarget(
    download::DownloadItemImpl* item,
    DownloadTargetCallback callback);
bool ChromeDownloadManagerDelegate::DetermineDownloadTarget(
    DownloadItem* download,
    content::DownloadTargetCallback* callback);    
void DownloadTargetDeterminer::Start(
    download::DownloadItem* download,
    const base::FilePath& initial_virtual_path,
    DownloadPathReservationTracker::FilenameConflictAction conflict_action,
    DownloadPrefs* download_prefs,
    DownloadTargetDeterminerDelegate* delegate,
    CompletionCallback callback);
DownloadTargetDeterminer::DownloadTargetDeterminer(
    DownloadItem* download,
    const base::FilePath& initial_virtual_path,
    DownloadPathReservationTracker::FilenameConflictAction conflict_action,
    DownloadPrefs* download_prefs,
    DownloadTargetDeterminerDelegate* delegate,
    CompletionCallback callback);    
void DownloadTargetDeterminer::DoLoop();//状态机，执行顺序在最后
DownloadTargetDeterminer::Result DownloadTargetDeterminer::DoReserveVirtualPath(); //STATE_RESERVE_VIRTUAL_PATH
oid DownloadTargetDeterminer::ReserveVirtualPathDone(
    download::PathValidationResult result,
    const base::FilePath& path);
void DownloadTargetDeterminer::DoLoop();//状态机
DownloadTargetDeterminer::Result DownloadTargetDeterminer::DoRequestConfirmation(); //STATE_PROMPT_USER_FOR_DOWNLOAD_PATH
void ChromeDownloadManagerDelegate::RequestConfirmation(
    DownloadItem* download,
    const base::FilePath& suggested_path,
    DownloadConfirmationReason reason,
    DownloadTargetDeterminerDelegate::ConfirmationCallback callback);
void ChromeDownloadManagerDelegate::ShowDownloadDialog(
    gfx::NativeWindow native_window,
    int64_t total_bytes,
    DownloadLocationDialogType dialog_type,
    const base::FilePath& suggested_path,
    DownloadDialogBridge::DialogCallback callback);
void DownloadDialogBridge::ShowDialog(
    gfx::NativeWindow native_window,
    int64_t total_bytes,
    net::NetworkChangeNotifier::ConnectionType connection_type,
    DownloadLocationDialogType dialog_type,
    const base::FilePath& suggested_path,
    bool is_incognito,
    DialogCallback dialog_callback);
Java_DownloadDialogBridge_showDialog(
      env, java_obj_, native_window->GetJavaObject(),
      static_cast<long>(total_bytes), static_cast<int>(connection_type),
      static_cast<int>(dialog_type),
      base::android::ConvertUTF8ToJavaString(env,
                                             suggested_path.AsUTF8Unsafe()),
      is_incognito);//走jni调用java弹窗    

    
执行顺序：
STATE_GENERATE_TARGET_PATH
STATE_SET_INSECURE_DOWNLOAD_STATUS
STATE_NOTIFY_EXTENSIONS
STATE_RESERVE_VIRTUAL_PATH
STATE_PROMPT_USER_FOR_DOWNLOAD_PATH
STATE_DETERMINE_LOCAL_PATH
STATE_DETERMINE_MIME_TYPE
STATE_DETERMINE_IF_HANDLED_SAFELY_BY_BROWSER
STATE_DETERMINE_IF_ADOBE_READER_UP_TO_DATE
STATE_CHECK_DOWNLOAD_URL
STATE_CHECK_VISITED_REFERRER_BEFORE
STATE_DETERMINE_INTERMEDIATE_PATH
STATE_NONE
```    
    
    

