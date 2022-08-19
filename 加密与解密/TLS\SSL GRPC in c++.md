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

使用CA的私钥对客户端的CSR进行签名，并取回已签名证书
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

### 证书详细工作流
![](https://github.com/ashenone0917/image/blob/main/0952f1a7026a53dae87bd4b1d45b26d3.png)

1. 申请认证：服务器需自己生成公钥私钥对 pub_svr & pri_svr，同时根据 pub_svr 生成请求文件 csr, 提交给 CA 机构，csr 中含有公钥、组织信息、个人信息（域名）等信息。(图一中 server.req 就是 csr 请求文件)
2. 审核信息：CA 机构通过线上、线下等多种手段验证申请者提供信息的真实性，如组织是否存在、企业是否合法，是否拥有域名的所有权等。
3. 签发证书：如信息审核通过，CA 机构会向申请者签发认证文件：证书。
证书包含以下信息：申请者公钥、申请者的组织信息和个人信息、签发机构 CA 的信息、有效时间、证书序列号等信息的明文，同时包含一个签名。
签名的产生算法：首先，使用散列函数计算公开的明文信息的信息摘要，然后，采用 CA 的私钥对信息摘要进行加密，密文即签名。（图一中生成 server.crt）
4. 返回证书：client 如果请求验证服务器，服务器需返回证书文件。（图一中 handshake 传回 server.crt）
5. client验证证书：client 读取证书中的相关的明文信息，采用相同的散列函数计算得到信息摘要，然后，利用对应 CA 的公钥解密签名数据，对比证书的信息摘要，如果一致，则可以确认证书的合法性，即公钥合法。客户端然后验证证书相关的域名信息、有效时间是否吊销等信息。
客户端会内置信任 CA 的证书信息（包含公钥），如果 CA 不被信任，则找不到对应 CA 的证书，证书也会被判定非法。(图一中 check 可选，我们可以选择不验证服务器证书的有效性)
6. 秘钥协商：验证通过后，server 和 client 将进行秘钥协商。接下来 server 和 client 会采用对称秘钥加密。(对称加密时间性能优)（图一中 pre-master/change_cipher_spec/encrypted_handshake_message 过程）
7. 数据传输：SSL server 和 SSL client 采用对称秘钥加密解密数据。

### 私有部署模拟申请证书(双向认证)
1. 生成CA的私钥和自签名证书(ca.crt，本地模拟CA)
2. 生成服务器的私钥和证书签名请求(CSR)，使用CA的私钥对服务器的CSR进行签名，并取回已签名证书(此时已经有了服务端私钥和服务端签名证书(server.crt))
3. 客户端安装包的内置一份服务端的ca.crt作为根证书(ssl_opts.pem_root_certs参数)
4. 客户端登录，服务端返回签名证书(server.crt)
5. 客户端使用CA生成的ca.crt认证过服务器之后，生成公私钥对(client_pri,client_pub)，上传公钥给服务端(auth)
6. 服务端使用私钥为客户端公钥签发中间证书(client.crt)(可以类比为CA二级机构向申请者签发证书，链：ca.crt->server.crt->client.crt)
7. 客户端有了私钥和client.crt，可以为其他想要访问的服务端的连接自签证书用于双向认证
8. 如此以来，需要双向认证的服务端就可以认证客户端，客户端也可以认证服务端，形成双向认证(比如和VPN服务器建连,vpn服务器也经历上述的操作来获取证书(或者说是原本的服务端证书也可以))

### 证书校验
证书/证书链的可信性 trusted certificate path，方法如前文所述

证书是否吊销 revocation，有两类方式离线 CRL 与在线 OCSP，不同客户端行为会不同

有效期 expiry date，证书是否在有效时间范围

域名 domain，核查证书域名是否与当前的访问域名匹配 (CN 字段)
```
证书校验没有强制的过程，也就是校验严格和校验宽松通常都是可以配置的，由校验端来确定。
```

### 证书等格式说明
**crt/key/req/csr/pem/der 等拓展名都是什么东东？**

.crt 表示证书, .key 表示私钥, .req 表示请求文件,.csr 也表示请求文件, .pem 表示 pem 格式，.der 表示 der 格式。
```
文件拓展名你可以随便命名，只是为了理解需要而命名不同的拓展名。但文件中的信息是有格式的，和 exe，PE 格式一样。
```

证书有两种格式：pem 格式和 der 格式

所有证书，私钥等都可以是 pem, 也可以是 der 格式，取决于应用需要。
pem 和 der 格式可以互转:

```
openssl x509 -in ca.crt -outform DER -out ca.der  # pem -> der
openssl x509 -inform der -in ca.der -out ca.pem   # der -> pem
```
pem 格式：经过加密的文本文件，一般有下面几种开头结尾：
```
	-----BEGIN RSA PRIVATE KEY-----
	-----END RSA PRIVATE KEY-----
	or:
   -----BEGIN CERTIFICATE REQUEST-----
   -----END CERTIFICATE REQUEST-----
	or:
   ----BEGIN CERTIFICATE-----
  -----END CERTIFICATE-----
```
der 格式: 经过加密的二进制文件。

### 如何查看证书中有什么
证书中含有 申请者公钥、申请者的组织信息和个人信息、签发机构 CA 的信息、有效时间、证书序列号等信息的明文，同时包含一个签名。如查看百度证书详细信息。

a) 先下载百度证书
火狐浏览器访问 https://www.baidu.com/, 点击左上角绿色小锁，点击向右箭头，点击更多信息，点击查看证书，点击详细信息，点击导出。即可导出百度的证书 baiducom.crt

b) 命令查看证书详细信息
```
openssl x509 -noout -text -in baiducom.crt
```

![](https://github.com/ashenone0917/image/blob/main/1452c05e8b429f7fbe9250a759710848.png)

详细信息中，有一个字段： X509v3 Basic Constraints: CA: FALSE

该字段指出该证书是否是 CA 证书，还是一般性的非 CA 证书。详细描述见 RFC5280#section-4.2.1.9，同时 RFC5280 也详细描述证书工作方式等。

### SSL/TLS 和 OpenSSL, mbedTLS 是什么关系
SSL/TLS 是一种工作原理，OpenSSL 和 mbedTLS 是 SSL/TLS 的具体实现，很类似于 TCP/IP 协议和 socket 之间的关系。

[参考](https://blog.csdn.net/ustccw/article/details/76691248)

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
