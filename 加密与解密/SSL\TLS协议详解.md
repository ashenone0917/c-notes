## SSL/TLS简介
单纯的使用HTTP协议传输数据是不安全的，因为HTTP协议往来的数据都是明文:
- 第三方可以获取到真实数据
- 第三方可以篡改数据
- 第三方可以猫中服务端或客户端

HTTPS的全程是**Hypertext Transfer Protocal Secure**，它用在网络上进行安全的数据交换，相当于HTTP的基础上加了一个Secure安全的词眼，可以认为HTTPS是HTTP协议的一种扩展，它的安全性由SSL(TLS)协议提供。即 HTTP + SSL(TLS) = HTTPS。

![](https://github.com/ashenone0917/image/blob/main/https.png)

SSL(Secure Socket Layer)中文称作“安全套阶层”，TLS(Transport Layer Security)，中文称作“传输层安全协议”。SSL是由网景公司（Netscape）设计的主要用于Web的安全传输协议，目的是为网络通信提供机密性、认证性及数据完整性保障。SSL最初的几个版本（SSL 1.0、SSL2.0、SSL 3.0）由网景公司设计和维护，从3.1版本开始，SSL协议由因特网工程任务小组（IETF）正式接管，并更名为TLS（Transport Layer Security），发展至今已有TLS 1.0、TLS 1.1、TLS 1.2、TLS 1.3这几个版本。

- 1994 年，网景（NetScape）公司设计了 SSL 1.0
- 1995 年，SSL 2.0，存在严重漏洞
- 1996 年，SSL 3.0，得到大规模应用
- 1999 年，IETF 对 SSL 进行标准化，发布了 TLS 1.0
- 2006 年和 2008 年，TLS 进行了两次升级，分别为 TLS 1.1 和 TLS 1.2
- 2018年，TLS 1.3作为建议标准发布于RFC 8446

由于SSL的2个版本都已经退出历史舞台了，所以后面只用TLS这个名字。读者应该明白，一般所说的SSL就是TLS。

SSL 是一个独立的协议，不只有 HTTP 可以使用，其他应用层协议也可以使用，比如 SMTP(电子邮件协议)、Telnet(远程登录协议) 等都可以使用。

## TLS的设计目标
TLS的设计目标是构建一个安全传输层（Transport Layer Security），在基于连接的传输层（如tcp）之上提供：
- 密码学安全
  - 保密，message privacy (保密通过加密encryption实现，所有信息都加密传输，第三方无法窃听)
  - 完整性，message integrity（通过MAC校验机制(Hash)，一旦被篡改，通信双方会立刻发现）
  - 认证，mutual authentication （双方认证,双方都可以配备证书，防止身份被冒充）
- 互操作，通用性 （根据公开的rfc，任何符合rfc的软件实现都可以互操作，不受限于任何专利技术）
- 可扩展性 (通过扩展机制tls_ext可以添加功能，有大量的新功能，都是通过扩展添加的)
- 高效率(通过session cache，恰当部署cache之后，tls的效率很高)

### 如何确定网站使用的TSL版本？
浏览器中，按F12调出调试窗口，选择安全(Security) TAB即可查看类似如下内容：
![](https://github.com/ashenone0917/image/blob/main/QQ%E6%88%AA%E5%9B%BE20220729001628.png)

## TLS的原理
### 密码通讯协议架构：自顶向下，分层抽象
- 构建软件的常用方式是分层，把问题域抽象为多层，每一层的概念定义为一组原语，上一层利用下一层的组件构造实现，并被上一层使用，层层叠叠即成软件。例如在编程语言领域中，汇编语言为一层，在汇编上面是C/C++等静态编译语言，C/C++之上是python/php/lua等动态类型脚本语言层，之上常常还会构造领域特定的DSL
- 在网络架构中，以太网是一层，其上是ip协议的网络层，ip之上是tcp等传输层，tcp之上是http等应用层

密码学通信协议也是分层构造得到。大致可以这么分层：
- 最底层是基础算法原语的实现，例如: aes , rsa， md5, sha256，ecdsa, ecdh 等
- 其上是选定参数后，符合密码学里标准分类的算法，包括块加密算法，签名算法，非对称加密算法，MAC算法等，例如： aes-128-cbc-pkcs7，rsaes-oaep ，rsassa-pkcs1-v1_5, hmac-sha256，ecdsa-p256，curve25519 等
- 再其上，是把多种标准算法组合而成的半成品组件，例如：对称传输组件例如 aes-128-cbc + hmac-sha256，aes-128-gcm，认证密钥协商算法: rsassa-OAEP + ecdh-secp256r1，数字信封：rsaes-oaep + aes-cbc-128 + hmac-sha256 ，文件密码加密存储组件：pbkdf2+aes-128-cbc-hmac-sha256，密钥扩展算法 PRF-sha256 等
- 再其上，是用各种组件拼装而成的各种成品密码学协议/软件，例如：tls协议，ssh协议，srp协议，gnupg文件格式，iMessage协议，bitcoin协议等等

每一层的情况：
- 第1层，一般程序员都有所了解，例如rsa，简直路人皆知; md5 被广泛使用(当然，也有广泛的误用)
- 第2层，各种莫名其妙的参数，一般很让程序员摸不着头脑，需要深入学习才能理清。
- 第3层，很多程序员自己造的轮子，往往说白了就是想重复实现第3层的某个组件而已。
- 第4层，正确地理解，使用，部署这类成熟的开放协议，并不是那么容易。很多的误用来源于不理解，需要密码学背景知识，才能搞懂是什么，为什么，怎么用。

第2层中，密码学算法，常见的有下面几类：
- 块加密算法 block cipher: AES, Serpent, 等
- 流加密算法 stream cipher: RC4，ChaCha20 等
- Hash函数 hash funtion: MD5，sha1，sha256，sha512 , ripemd 160，poly1305 等
- 消息验证码函数 message authentication code: HMAC-sha256，AEAD 等
- 密钥交换 key exchange: DH，ECDH，RSA，PFS方式的（DHE，ECDHE）等
- 公钥加密 public-key encryption: RSA，rabin-williams 等
- 数字签名算法 signature algorithm:RSA，DSA，ECDSA (secp256r1 , ed25519) 等
- 密码衍生函数 key derivation function: TLS-12-PRF(SHA-256), bcrypto，scrypto，pbkdf2 等
- 随机数生成器 random number generators: /dev/urandom 等
- 
设计一个加密通信协议的过程，就是自顶向下，逐步细化，挑选各类组件，拼装成完整协议的过程

### TLS CipherSuite(TLS加密套件)
从上述分层的角度看，TLS大致是由3个组件拼成的：
- 对称加密传输组件，协商完成后使用对称加密传输数据，例如aes-128-gcm(这几个例子都是当前2015年最主流的选择);
- 认证密钥协商组件，例如rsa-ecdhe;
- 密钥扩展组件，例如TLS-PRF-sha256

这些组件可以再拆分为5类算法，在TLS中，这5类算法组合在一起，称为一个CipherSuite：
- authentication (认证算法)
- encryption (加密算法)
- message authentication code (消息认证码算法 简称MAC)
- key exchange (密钥交换算法)
- key derivation function （密钥衍生算法)

TLS协议设计之初就考虑到了这每一类算法的演变，所以没有定死算法，而是设计了一个算法协商过程，来允许加入新的算法，协商出的一个算法组合即一个TLS CipherSuite 在 iana 集中注册，每一个CipherSuite分配有 一个2字节的数字用来标识 ，可以在[iana的注册页面](https://www.iana.org/assignments/tls-parameters/tls-parameters.xhtml#tls-parameters-4)查看

![](https://github.com/ashenone0917/image/blob/main/value.png)

服务器端支持的CipherSuite列表，如果是用的openssl，可以用 openssl ciphers -V | column -t 命令查看，输出如：

![](https://github.com/ashenone0917/image/blob/main/openssl.png)

### TLS协议的架构

![](https://github.com/ashenone0917/image/blob/main/tls-protocol.png)

大致流程如下图

![](https://github.com/ashenone0917/image/blob/main/b05863e452a0678db087e98ca16b36dd.png)

认证 -> 协商对称密钥 -> 传输加密数据

TLS（Transport Layer Security Protocol，传输层安全协议）主要目的是提供隐私和数据两个通信应用之间的完整性。该协议由两层组成：TLS 记录协议（TLS Record）和 TLS 握手协议（TLS Handshake）。

- 底层的是TLS记录协议，主要负责使用对称密码对消息进行加密。
- 上层的是TLS握手协议，主要分为握手协议，密码规格变更协议和应用数据协议4个部分。
  - 握手协议负责在客户端和服务器端商定密码算法和共享密钥，包括证书认证，是4个协议中最最复杂的部分。
  - 密码规格变更协议负责向通信对象传达变更密码方式的信号(有点冗余，在3里面已经被删掉了)
  - 警告协议负责在发生错误的时候将错误传达给对方
  - 应用数据协议负责将TLS承载的应用数据传达给通信对象的协议。

这种认证密钥协商 + 对称加密传输的结构，是绝大多数加密通信协议的通用结构。

### TLS握手协议
在用SSL进行通信之前，首先要使用SSL的Handshake协议在通信两端握手，协商数据传输中要用到的相关安全参数（如加密算法、共享密钥、产生密钥所要的材料等），并对对端的身份进行验证。

![](https://github.com/ashenone0917/image/blob/main/say-hello.png)

SSL的建立过程总共有13个包，第一次建立至少需要9个包。

![](https://github.com/ashenone0917/image/blob/main/ssl-flow.png)

**SSL建立第一阶段：客户端首先发送ClientHello消息到服务端，服务端收到ClientHello消息后，再发送ServerHello消息回应客户端。**

![](https://github.com/ashenone0917/image/blob/main/client-server.png)

**ClientHello**

握手第一步是客户端向服务端发送 Client Hello 消息，这个消息里包含了一个客户端生成的随机数 Random1、客户端支持的加密套件（Support Ciphers）和 SSL Version 等信息。

![](https://github.com/ashenone0917/image/blob/main/clienthello.png)

ClientHello中涉及到的消息具体如下：
- 客户端版本：按优先级列出客户端支持的协议版本，首选客户端希望支持的最新协议版本。
- 客户32位端随机数
- 会话ID（Session id）：如果客户端第一次连接到服务器，那么这个字段就会保持为空。上图中该字段为空，说明这是第一次连接到服务器。如果该字段不为空，说明以前是与服务器有连接的，在此期间，服务器将使用Session ID映射对称密钥，并将Session ID存储在客户端浏览器中，为映射设置一个时间限。如果浏览器将来连接到同一台服务器（在时间到期之前），它将发送Session ID，服务器将对映射的Session ID进行验证，并使用以前用过的对称密钥来恢复Session(就可以跳过一些步骤，应该包括协商密钥的步骤)，这种情况下不需要完全握手。也叫作SSL会话恢复。后面会有介绍。
- 加密套件：客户端会给服务器发送自己已经知道的密码套件列表，这是由客户按优先级排列的，但完全由服务器来决定发送与否。TLS中使用的密码套件有一种标准格式。上面的报文中，客户端发送了74套加密套件。服务端会从中选出一种来作为双方共同的加密套件。
- 压缩方法：为了减少带宽，可以进行压缩。但从成功攻击TLS的事例中来看，其中使用压缩时的攻击可以捕获到用HTTP头发送的参数，这个攻击可以劫持Cookie，这个漏洞我们称为CRIME。从TLS 1.3开始，协议就禁用了TLS压缩。
- 扩展包：其他参数（如服务器名称，填充，支持的签名算法等）可以作为扩展名使用。

这些是client hello的一部分，如果已收到client hello，接下来就是服务器的确认，服务器将发送server hello。

### ServerHello
收到客户端问候之后服务器必须发送服务器问候信息，服务器会检查指定诸如TLS版本和算法的客户端问候的条件，如果服务器接受并支持所有条件，它将发送其证书以及其他详细信息，否则，服务器将发送握手失败消息。

如果接受，第二步是服务端向客户端发送 Server Hello 消息，这个消息会从 Client Hello 传过来的 Support Ciphers 里确定一份加密套件，这个套件决定了后续加密和生成摘要时具体使用哪些算法，另外还会生成一份随机数 Random2。注意，至此客户端和服务端都拥有了两个随机数（Random1+ Random2），这两个随机数会在后续生成对称秘钥时用到。

![](https://github.com/ashenone0917/image/blob/main/serverhello.png)

ServerHello中涉及到的具体参数：
- 服务器版本Version：服务器会选择客户端支持的最新版本。
- 服务器随机数Random：服务器和客户端都会生成32字节的随机数。用来创建加密密钥。
- 加密套件：服务器会从客户端发送的加密套件列表中选出一个加密套件。
- 会话ID（Session ID）：服务器将约定的Session参数存储在TLS缓存中，并生成与其对应的Session id。它与Server Hello一起发送到客户端。客户端可以写入约定的参数到此Session id，并给定到期时间。客户端将在Client Hello中包含此id。如果客户端在此到期时间之前再次连接到服务器，则服务器可以检查与Session id对应的缓存参数，并重用它们而无需完全握手。这非常有用，因为服务器和客户端都可以节省大量的计算成本。在涉及亚马逊和谷歌等流量巨大的应用程序时，这种方法存在缺点。每天都有数百万人连接到服务器，服务器必须使用Session密钥保留所有Session参数的TLS缓存(该session之前认证的缓存)。这是一个巨大的开销。为了解决这个问题，在扩展包里加入了Session Tickets, 在这里，客户端可以在client hello中指定它是否支持Session Ticket。如果支持，服务器将创建一个新的会话票证(Session Ticket)，并使用只有服务器知道的经过私钥加密的Session参数(TLS缓存)。它将存储在客户端上，因此所有Session数据仅存储在客户端计算机上，但Ticket仍然是安全的，因为该密钥只有服务器知道。此数据可以作为名为Session Ticket的扩展包含在Client Hello中。
- 压缩算法：如果支持，服务器将同意客户端的首选压缩方法。
- 扩展包

这个阶段之后，客户端服务端知道了下列内容：
- SSL版本
- 密钥交换、信息验证和加密算法(例如RSA,MAC,AES)
- 压缩方法
- 有关密钥生成的两个随机数。

**SSL建立第二阶段：服务器向客户端发送消息。**

![](https://github.com/ashenone0917/image/blob/main/server-to-client.png)

服务器启动SSL握手第2阶段，是本阶段所有消息的唯一发送方，客户机是所有消息的唯一接收方。该阶段分为4步：
- 证书：服务器将数字证书和到根CA整个链发给客户端，使客户端能用服务器证书中的服务器公钥认证服务器。(验服务端)
- 服务器密钥交换（可选）：这里视密钥交换算法而定
- 证书请求：服务端可能会要求客户自身进行验证。(双向认证)
- 服务器握手完成：第二阶段的结束，第三阶段开始的信号

**Certificate消息（可选）—第一次建立必须要有证书(有session id可以跳过？)**

一般情况下，除了会话恢复时不需要发送该消息，在SSL握手的全流程中，都需要包含该消息。消息包含一个X.509证书(或证书链)，证书中包含公钥，发给客户端用来验证签名或在密钥交换的时候给消息加密。

这一步是服务端将自己的证书下发给客户端，让客户端验证自己的身份，客户端验证通过后取出证书中的公钥，用于接下来协商对称密钥(可用于RSA协商密钥)。

![](https://github.com/ashenone0917/image/blob/main/cert.png)

**Server Key Exchange（可选）**

根据之前在ClientHello消息中包含的CipherSuite信息，决定了密钥交换方式（例如RSA或者DH），因此在Server Key Exchange消息中便会包含完成密钥交换所需的一系列参数。

![](https://github.com/ashenone0917/image/blob/main/server-key-exchange.png)

因为这里是DH算法，所以需要发送服务器使用的DH参数。**RSA算法不需要这一步，使用的是认证时的公钥协商**。

在Diffie-Hellman中，客户端无法自行计算预主密钥; 双方都有助于计算它，因此客户端需要从服务器获取Diffie-Hellman公钥。

由上图可知，此时密钥交换也由签名保护(给密钥签名了)。

**Certificate Request（可选）——可以是单向的身份认证，也可以双向认证**

这一步是可选的，如果在对安全性要求高的常见可能用到。服务器用来验证客户端。服务器端发出Certificate Request消息(双向认证)，要求客户端发他自己的证书过来进行验证。该消息中包含服务器端支持的证书类型（RSA、DSA、ECDSA等）和服务器端所信任的所有证书发行机构的CA列表，客户端会用这些信息来筛选证书。

**Server Hello Done**

该消息表示服务器已经将所有信息发送完毕，接下来等待客户端的消息。

![](https://github.com/ashenone0917/image/blob/main/server-hello-demo.png)

**SSL建立第三阶段：客户端收到服务器发送的一系列消息并解析后，将本端相应的消息发送给服务器。**

![](https://github.com/ashenone0917/image/blob/main/client-to-server.png)

客户机启动SSL握手第3阶段，是本阶段所有消息的唯一发送方，服务器是所有消息的唯一接收方。该阶段分为3步：
- 证书（可选）：为了对服务器证明自身，客户要发送一个证书信息，这是可选的，在IIS中可以配置强制客户端证书认证(双向认证是可选的)。
- 客户机密钥交换（Pre-master-secret）：这里客户端将预备主密钥发送给服务端，注意这里会使用服务端的公钥进行加密。
- 证书验证（可选），对预备秘密和随机数进行签名，证明拥有（a）证书的公钥。

**Certificate（可选）**

如果在第二阶段服务器端要求发送客户端证书，客户端便会在该阶段将自己的证书发送过去。服务器端在之前发送的Certificate Request消息中包含了服务器端所支持的证书类型和CA列表，因此客户端会在自己的证书中选择满足这两个条件的第一个证书发送过去。若客户端没有证书，则发送一个no_certificate警告。

**Client Key exchange**

根据之前从服务器端收到的随机数，按照不同的密钥交换算法，算出一个pre-master，发送给服务器，服务器端收到pre-master算出main master。而客户端当然也能自己通过pre-master算出main master。如此以来双方就算出了对称密钥。

如果是RSA算法，会生成一个48字节的随机数(pre-master)，然后用server的公钥加密后再放入报文中。如果是DH算法，这是发送的就是客户端的DH参数，之后服务器和客户端根据DH算法，各自计算出相同的pre-master secret.

![](https://github.com/ashenone0917/image/blob/main/clent-key-exchenge.png)

本消息在给服务器发送的过程中，使用了服务器的公钥加密。服务器用自己的私钥解密后才能得到pre-master key.（向服务器证明自己的确持有客户端证书私钥。）

**Certificate verify（可选）**

只有在客户端发送了自己证书到服务器端，这个消息才需要发送。其中包含一个签名，对从第一条消息以来的所有握手消息的HMAC值（用master_secret）进行签名。


