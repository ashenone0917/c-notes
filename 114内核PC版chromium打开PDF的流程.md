```cpp
void NavigationURLLoaderImpl::OnReceiveResponse(
    network::mojom::URLResponseHeadPtr head,
    mojo::ScopedDataPipeConsumerHandle response_body,
    absl::optional<mojo_base::BigBuffer> cached_metadata);
void NavigationURLLoaderImpl::CheckPluginAndContinueOnReceiveResponse(
    network::mojom::URLResponseHeadPtr head,
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    bool is_download_if_not_handled_by_plugin,
    const std::vector<WebPluginInfo>& plugins);
if(stale == false) {
  void PluginServiceImpl::GetPlugins(GetPluginsCallback callback);
  void NavigationURLLoaderImpl::CheckPluginAndContinueOnReceiveResponse(
    network::mojom::URLResponseHeadPtr head,
    network::mojom::URLLoaderClientEndpointsPtr url_loader_client_endpoints,
    bool is_download_if_not_handled_by_plugin,
    const std::vector<WebPluginInfo>& plugins);
}
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
void NavigationRequest::WillProcessResponse();
void NavigationThrottleRunner::ProcessNavigationEvent(Event event);
void NavigationThrottleRunner::ProcessInternal();
void NavigationThrottleRunner::InformDelegate(
    const NavigationThrottle::ThrottleCheckResult& result);
void NavigationRequest::OnNavigationEventProcessed(
    NavigationThrottleRunner::Event event,
    NavigationThrottle::ThrottleCheckResult result);//和下面的是不同的event
void NavigationRequest::OnWillProcessResponseProcessed(
    NavigationThrottle::ThrottleCheckResult result);    
void NavigationRequest::OnWillProcessResponseChecksComplete(
    NavigationThrottle::ThrottleCheckResult result);
void NavigationRequest::RunCommitDeferringConditions();

.....
NavigationThrottle::ThrottleCheckResult ExecuteNavigationEvent(
    NavigationThrottle* throttle,
    NavigationThrottleRunner::Event event);
content::NavigationThrottle::ThrottleCheckResult PdfNavigationThrottle::WillStartRequest();
base::WeakPtr<NavigationHandle> NavigationControllerImpl::LoadURLWithParams(
    const LoadURLParams& params);
base::WeakPtr<NavigationHandle> NavigationControllerImpl::NavigateWithoutEntry(
    const LoadURLParams& params);
void Navigator::Navigate(std::unique_ptr<NavigationRequest> request,
                         ReloadType reload_type);
void NavigationRequest::BeginNavigation();
void NavigationRequest::BeginNavigationImpl();
void NavigationRequest::WillStartRequest();
void NavigationThrottleRunner::ProcessNavigationEvent(Event event);
void NavigationThrottleRunner::ProcessInternal();
void NavigationThrottleRunner::InformDelegate(
    const NavigationThrottle::ThrottleCheckResult& result);
void NavigationRequest::OnNavigationEventProcessed(
    NavigationThrottleRunner::Event event,
    NavigationThrottle::ThrottleCheckResult result);
void NavigationRequest::OnWillStartRequestProcessed(
    NavigationThrottle::ThrottleCheckResult result);
void NavigationRequest::OnStartChecksComplete(
    NavigationThrottle::ThrottleCheckResult result);
void NavigationURLLoaderImpl::Start();
void NavigationURLLoaderImpl::StartImpl(
    scoped_refptr<PrefetchedSignedExchangeCache>
        prefetched_signed_exchange_cache,
    scoped_refptr<SignedExchangePrefetchMetricRecorder>
        signed_exchange_prefetch_metric_recorder,
    mojo::PendingRemote<network::mojom::URLLoaderFactory> factory_for_webui,
    std::string accept_langs);
void NavigationURLLoaderImpl::Restart();
void NavigationURLLoaderImpl::MaybeStartLoader(
    NavigationLoaderInterceptor* interceptor,
    scoped_refptr<network::SharedURLLoaderFactory> single_request_factory);
void SignedExchangeRequestHandler::MaybeCreateLoader(
    const network::ResourceRequest& tentative_resource_request,
    BrowserContext* browser_context,
    LoaderCallback callback,producer_
    FallbackCallback fallback_callback);
void NavigationLoaderInterceptorBrowserContainer::MaybeCreateLoader(
      const network::ResourceRequest& tentative_resource_request,
      BrowserContext* browser_context,
      LoaderCallback callback,
      FallbackCallback fallback_callback);
void PdfURLLoaderRequestInterceptor::MaybeCreateLoader(
    const network::ResourceRequest& tentative_resource_request,
    content::BrowserContext* browser_context,
    content::URLLoaderRequestInterceptor::LoaderCallback callback);
content/browser/loader/navigation_url_loader_impl.cc:140;//lambda
void NavigationURLLoaderImpl::MaybeStartLoader(
    NavigationLoaderInterceptor* interceptor,
    scoped_refptr<network::SharedURLLoaderFactory> single_request_factory);
std::unique_ptr<ThrottlingURLLoader> ThrottlingURLLoader::CreateLoaderAndStart(
    scoped_refptr<network::SharedURLLoaderFactory> factory,
    std::vector<std::unique_ptr<URLLoaderThrottle>> throttles,
    int32_t request_id,
    uint32_t options,
    network::ResourceRequest* url_request,
    network::mojom::URLLoaderClient* client,
    const net::NetworkTrafficAnnotationTag& traffic_annotation,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner,
    absl::optional<std::vector<std::string>> cors_exempt_header_list);//static
void ThrottlingURLLoader::Start(
    scoped_refptr<network::SharedURLLoaderFactory> factory,
    int32_t request_id,
    uint32_t options,
    network::ResourceRequest* url_request,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner,
    absl::optional<std::vector<std::string>> cors_exempt_header_list);
void ThrottlingURLLoader::StartNow();
void SingleRequestURLLoaderFactory::CreateLoaderAndStart(
    mojo::PendingReceiver<mojom::URLLoader> loader,
    int32_t request_id,
    uint32_t options,
    const ResourceRequest& request,
    mojo::PendingRemote<mojom::URLLoaderClient> client,
    const net::MutableNetworkTrafficAnnotationTag& traffic_annotation);
void network::SingleRequestURLLoaderFactory::HandlerState::HandleRequest(const ResourceRequest& resource_request,
    mojo::PendingReceiver<mojom::URLLoader> loader,
    mojo::PendingRemote<mojom::URLLoaderClient> client);//services/network/public/cpp/single_request_url_loader_factory.cc:41
void CreateLoaderAndStart(
    const PdfStreamDelegate::StreamInfo& stream_info,
    const network::ResourceRequest& request,
    mojo::PendingReceiver<network::mojom::URLLoader> receiver,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client);//components/pdf/browser/pdf_url_loader_request_interceptor.cc:40
void PluginResponseWriter::Start(base::OnceClosure done_callback);
```
