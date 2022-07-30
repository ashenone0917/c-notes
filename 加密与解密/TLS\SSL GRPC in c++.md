## SSL/TLS 单向认证流程
![](https://github.com/ashenone0917/image/blob/main/4c68663a50cf42398572dc9053e29798.png)

## SSL/TLS 双向认证流程
![](https://github.com/ashenone0917/image/blob/main/0da88e66d2ce47e886122dde24bf4993.png)

## gRPC连接类型
- 第一个是不安全的连接。在这方面，客户端和服务器之间传输的所有数据都未加密。所以请不要在生产中使用它！
- 第二种类型是由服务器端 TLS 保护的连接。在这种情况下，所有数据都被加密，但只有服务器需要向客户端提供其 TLS 证书。如果服务器不关心哪个客户端正在调用其 API，则可以使用这种类型的连接。
- 第三种也是最强的类型是由双向 TLS 保护的连接。当服务器还需要验证谁在调用它的服务时，我们会使用它。所以在这种情况下，客户端和服务器都必须向对方提供他们的 TLS 证书。

## gRPC 双向认证
### TLS证书生成
![](https://github.com/ashenone0917/image/blob/main/b05863e452a0678db087e98ca16b36dd.png)

生成CA的私钥和自签名证书
```cpp
openssl req -x509 -newkey rsa:4096 -days 365 -nodes -keyout ca-key.pem -out ca-cert.pem -subj "/C=CN/ST=SC/L=ChenDu/O=AISHU/OU=Database/CN=*.aishu.cn/emailAddress=xxxxxx.xxx@gmail.com"
```

生成服务器的私钥和证书签名请求(CSR)
```cpp
openssl req -newkey rsa:4096 -nodes -keyout server-key.pem -out server-req.pem -subj "/C=CN/ST=SC/L=ChenDu/O=AISHU/OU=Database/CN=localhost/emailAddress=xxxxxx.xxx@gmail.com"
```

使用CA的私钥对服务器的CSR进行签名，并取回已签名证书
```cpp
openssl x509 -req -in server-req.pem -days 60 -CA ca-cert.pem -CAkey ca-key.pem -CAcreateserial -out server-cert.pem
```

生成客户端的私钥和证书签名请求(CSR)
```cpp
openssl req -newkey rsa:4096 -nodes -keyout client-key.pem -out client-req.pem -subj "/C=CN/ST=SC/L=ChenDu/O=AISHU/OU=Database/CN=localhost/emailAddress=xxxxxx.xxx@gmail.com"
```

使用CA的私钥对服务器的CSR进行签名，并取回已签名证书
```cpp
openssl x509 -req -in client-req.pem -days 60 -CA ca-cert.pem -CAkey ca-key.pem -CAcreateserial -out client-cert.pem
```

CA 校验证书测试
```cpp
openssl verify -CAfile ca-cert.pem server-cert.pem
openssl verify -CAfile ca-cert.pem client-cert.pem
```

**注意：生产环境中需要将 CN=localhost 替换成实际域名**

### 生成证书过程
SSL server 自己生成一个私钥/公钥对。server.key/server.pub // 私钥加密，公钥解密！
server.pub 生成一个请求文件 server.req. 请求文件中包含有 server 的一些信息，如域名/申请者/公钥等
server 将请求文件 server.req 递交给 CA 机构，CA 机构验明正身后，将用 ca.key 和请求文件加密生成 server.crt
由于 ca.key 和 ca.crt 是一对，于是 ca.crt 可以用来校验 server.crt
说明：为了简化 CA 校验证书的过程，本文只介绍了最基本的情况。在实际大多数情况下：

1. server 端的证书颁发机构 CA 和 client 端的证书颁发机构 CA 通常不同
2. 证书实际情况下，可以是证书链，也就是多个上级机构逐级下发证书的链
3. 证书校验时，CA 通常可以选择校验证书链的深度，最基础的情况是只校验一级

如果 SSL client 想要校验 SSL server. 那么 SSL server 必须要将他的证书 server.crt 传给 client. 然后 client 用 ca.crt 去校验 server.crt 的合法性。

如果 server 是一个钓鱼网站，那么 CA 机构是不会给他颁发合法 server.crt 证书的，这样 client 用 ca.crt 去校验，就会失败。

比如：浏览器作为一个 SSL client, 你想访问合法的淘宝网站 https://www.taobao.com, 结果不慎访问到 https://wwww.jiataobao.com, 那么浏览器将会检验到这个假淘宝钓鱼网站的非法性，提醒用户不要继续访问！这样就可以保证 client 的所有 https 访问都是经过安全检查的。


### gRPC 服务端实现
```cpp
#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include <grpc++/security/credentials.h>
#include <fstream>

#include "hello.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloResponse;
using helloworld::Greeter;

static std::string get_file_contents(const char *fpath)
{
  std::ifstream finstream(fpath);
  std::string contents((std::istreambuf_iterator<char>(finstream)),
                       std::istreambuf_iterator<char>());
  return contents;
}

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloResponse* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
};

void RunServer(char** argv) {
  std::string server_address("127.0.0.1:50051");
  GreeterServiceImpl service;

  auto client_ca_pem = get_file_contents(argv[1]); // for verifying clients
  auto my_key_pem = get_file_contents(argv[2]);//私钥参数应该是为了后续传输对称密钥的时候加密使用
  auto my_cert_pem = get_file_contents(argv[3]);

  grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp = {
    my_key_pem.c_str(), my_cert_pem.c_str()
  };

  grpc::SslServerCredentialsOptions ssl_opts(GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY);
  ssl_opts.pem_root_certs = client_ca_pem;
  ssl_opts.pem_key_cert_pairs.push_back(pkcp);
  std::cout << "pem_root_certs length: " << ssl_opts.pem_root_certs.length() << std::endl;

  std::shared_ptr<grpc::ServerCredentials> creds = grpc::SslServerCredentials(ssl_opts);

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, creds);
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer(argv);

  return 0;
}
```

### gRPC 客户端实现

```cpp
#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include <grpc++/security/credentials.h>
#include <fstream>

#include "hello.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloResponse;
using helloworld::Greeter;

static std::string get_file_contents(const char *fpath)
{
  std::ifstream finstream(fpath);
  std::string contents((std::istreambuf_iterator<char>(finstream)),
                       std::istreambuf_iterator<char>());
  return contents;
}

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).

  assert (argc == 4);
  auto server_ca_pem = get_file_contents(argv[1]);
  auto my_key_pem = get_file_contents(argv[2]);
  auto my_cert_pem = get_file_contents(argv[3]);

  grpc::SslCredentialsOptions ssl_opts;
  ssl_opts.pem_root_certs = server_ca_pem;
  ssl_opts.pem_private_key = my_key_pem;//应该和上述服务端作用相同
  ssl_opts.pem_cert_chain = my_cert_pem;//双向认证使用，private_key和cert_chain在单向认证情况下无需填写

  std::shared_ptr<grpc::ChannelCredentials> creds = grpc::SslCredentials(ssl_opts);
    grpc::ChannelArguments channel_args;
    channel_args.SetSslTargetNameOverride("foo.test.google.fr");

    GreeterClient greeter(grpc::CreateCustomChannel("localhost:50051", creds, channel_args));

//   GreeterClient greeter(grpc::CreateChannel(
//       "localhost:50051", creds));
  std::string user("world");
  std::string reply = greeter.SayHello(user);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
```

### 运行截图

服务端截图

![](https://github.com/ashenone0917/image/blob/main/ba56798515ae4933be0be1cf6602c42a.png)

客户端截图

![](https://github.com/ashenone0917/image/blob/main/ce59794957ad44d1b326e29e0a9cb0e8.png)
