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

#### SSL建立第一阶段：客户端首先发送ClientHello消息到服务端，服务端收到ClientHello消息后，再发送ServerHello消息回应客户端。

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

#### SSL建立第二阶段：服务器向客户端发送消息。

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

#### SSL建立第四阶段：完成握手协议，建立SSL连接。

![](https://github.com/ashenone0917/image/blob/main/ssl-finished.png)

客户机启动SSL握手第4阶段，使服务器结束。该阶段分为4步，前2个消息来自客户机，后2个消息来自服务器。

建立起一个安全的连接，客户端发送一个Change Cipher Spec消息，并且把协商得到的CipherSuite拷贝到当前连接的状态之中。然后，客户端用新的算法、密钥参数发送一个Finished消息，这条消息可以检查密钥交换和认证过程是否已经成功。其中包括一个校验值，对客户端整个握手过程的消息进行校验。服务器同样发送Change Cipher Spec消息和Finished消息。握手过程完成，客户端和服务器可以交换应用层数据进行通信。

**ChangeCipherSpec**

编码改变通知，表示随后的信息都将用双方商定的加密方法和密钥发送（ChangeCipherSpec是一个独立的协议，体现在数据包中就是一个字节的数据，用于告知服务端，客户端已经切换到之前协商好的加密套件（Cipher Suite）的状态，准备使用之前协商好的加密套件加密数据并传输了）。

![](https://github.com/ashenone0917/image/blob/main/ChangeCipherSpec.png)

**Clinet Finished**

客户端握手结束通知, 表示客户端的握手阶段已经结束。这一项同时也是前面发送的所有内容的hash值，用来供服务器校验（使用HMAC算法计算收到和发送的所有握手消息的摘要，然后通过RFC5246中定义的一个伪函数PRF计算出结果，加密后发送。此数据是为了在正式传输应用数据之前对刚刚握手建立起来的加解密通道进行验证。）

**Server Finished**

服务端握手结束通知。

- 使用私钥解密加密的Pre-master数据，基于之前(Client Hello 和 Server Hello)交换的两个明文随机数 random_C 和 random_S，计算得到协商密钥:enc_key=Fuc(random_C, random_S, Pre-Master);
- 计算之前所有接收信息的 hash 值，然后解密客户端发送的 encrypted_handshake_message，验证数据和密钥正确性;
- 发送一个 ChangeCipherSpec（告知客户端已经切换到协商过的加密套件状态，准备使用加密套件和 Session Secret加密数据了）
- 服务端也会使用 Session Secret 加密一段 Finish 消息发送给客户端，以验证之前通过握手建立起来的加解密通道是否成功。

### TLS记录协议
TLS记录协议主要负责消息的压缩，加密及数据的认证：

![](https://github.com/ashenone0917/image/blob/main/record.png)

消息首先将会被分段，然后压缩，再计算其消息验证码，然后使用对称密码进行加密，加密使用的是[CBC模式](https://github.com/ashenone0917/c-notes/blob/main/%E5%8A%A0%E5%AF%86%E4%B8%8E%E8%A7%A3%E5%AF%86/CBC%E6%A8%A1%E5%BC%8F.md)，CBC模式的初始向量是通过主密码来生成的。得到密文之后会附加类型，版本和长度等其他信息，最终组成最后的报文数据。

record协议做应用数据的对称加密传输，占据一个TLS连接的绝大多数流量。

Record 协议 – 从应用层接受数据，并且做:
- 分片，逆向是重组
- 生成序列号，为每个数据块生成唯一编号，防止被重放或被重排序
- 压缩，可选步骤，使用握手协议协商出的压缩算法做压缩
- 加密，使用握手协议协商出来的key做加密/解密
- 算HMAC，对数据计算HMAC，并且验证收到的数据包的HMAC正确性
- 发给tcp/ip，把数据发送给 TCP/IP 做传输(或其它ipc机制)。

**SecurityParameters**

record层的上述处理，完全依据下面这个SecurityParameters里面的参数进行：

```cpp
struct {
    ConnectionEnd          entity;
    PRFAlgorithm           prf_algorithm;
    BulkCipherAlgorithm    bulk_cipher_algorithm;
    CipherType             cipher_type;
    uint8                  enc_key_length;
    uint8                  block_length;
    uint8                  fixed_iv_length;
    uint8                  record_iv_length;
    MACAlgorithm           mac_algorithm;
    uint8                  mac_length;
    uint8                  mac_key_length;
    CompressionMethod      compression_algorithm;
    opaque                 master_secret[48];
    opaque                 client_random[32];
    opaque                 server_random[32];
} SecurityParameters;
```

record 层使用上面的SecurityParameters生成下面的6个参数（不是所有的CipherSuite都需要全部6个，如果不需要，那就是空）:

```cpp
client write MAC key //签名密钥
server write MAC key
client write encryption key //加解密密钥
server write encryption key
client write IV //CBC初始化向量
server write IV //CBC初始化向量
```

当handshake完成，上述6个参数生成完成之后，就可以建立连接状态，连接状态除了上面的SecurityParameters，还有下面几个参数，并且随着数据的发送/接收，更新下面的参数：
- compression state : 当前压缩算法的状态。
- cipher state : 加密算法的当前状态，对块加密算法比如aes，包含密码预处理生成的轮密钥(感谢温博士指出) “round key”，还有IV等；对于流加密，包含能让流加密持续进行加解密的状态信息
- sequence number : 每个连接状态都包含一个sequence number，并且读和写状态有不同的sequence number。当连接开始传输数据时，sequence number必须置为 sequence number 是uint64类型的，并且不得超过264-1。s. Sequence number不得回绕。如果一个TLS实现无法避开回绕一个sequence number，必须进行重协商。sequence number在每个record被发送时都增加1。并且传输的第1个Record必须使用0作为sequence number。

**record层分段**
如上图所示，对要发送的数据流，首先分段，分段成如下格式：
```cpp
struct {
    uint8 major;
    uint8 minor;
} ProtocolVersion;

enum {
    change_cipher_spec(20), alert(21), handshake(22),
    application_data(23), (255)
} ContentType;

struct {
    ContentType type;
    ProtocolVersion version;
    uint16 length;
    opaque fragment[TLSPlaintext.length];
} TLSPlaintext;
```
- version字段 : ，定义当前协商出来的TLS协议版本，例如 TLS 1.2 version 是 { 3, 3 }
- length字段 : 即长度，tls协议规定length必须小于214，一般我们不希望length过长，因为解密方需要收完整个- - record，才能解密，length过长会导致解密方需要等待更多的rtt，增大latency，破坏用户体验，参考 Web性能权威指南 TLS那一章。
- type字段 : ，用来标识当前record是4种协议中的哪一种，

record压缩：TLS协议定义了可选的压缩，但是，由于压缩导致了 2012 年被爆出CRIME攻击，BREACH攻击，所以在实际部署中，一定要禁用压缩。

**record层的密码学保护**

经过处理后的包格式定义如下：

```cpp
struct {
    ContentType type;
    ProtocolVersion version;
    uint16 length;
    select (SecurityParameters.cipher_type) {
        case stream: GenericStreamCipher;
        case block:  GenericBlockCipher;
        case aead:   GenericAEADCipher;
    } fragment;
} TLSCiphertext;
```

TLS协议设计目标中的保密(encryption)、完整性(authentication)和防重放就在这里实现。 实现方式有3类：

1. Block Cipher (CBC mode of operation) + HMAC：例如 aes-128-cbc+hmac-sha256
2. Stream Cipher (RC4) + HMAC
3. Authenticated-Encryption using block cipher (GCM/CCM 模式)：例如 aes-128-gcm
Block Cipher+HMAC 和 Stream Cipher + HMAC 的各类算法目前（2015年）都已经爆出各种漏洞，目前最可靠的是Authenticated-Encryption 类的算法，主要就是aes-gcm，下一代的TLS v1.3只保留了Authenticated-Encryption，把1和2直接禁止了。

GCM模式是AEAD的，所以不需要MAC算法。 GCM模式是AEAD的一种，AEAD 的 作用类似于 Encrypt-then-HMAC ，例如 Sha256 + Salt + AES + IV

此处需要介绍一个陷阱。 在密码学历史上，出现过3种加密和认证的组合方式：

1. Encrypt-and-MAC
2. MAC-then-Encrypt
3. Encrypt-then-MAC

在TLS协议初定的那个年代，人们还没意识到这3种组合方式的安全性有什么差别，所以TLS协议规定使用 2.MAC-then-Encrypt，即先计算MAC，然后把 “明文+MAC” 再加密(块加密或者流加密)的方式，做流加密+MAC，和块加密+MAC。 但是，悲剧的是，近些年，人们发现 MAC-then-Encrypt 这种结构导致了很容易构造padding oracle 相关的攻击，例如这在TLS中，间接形成被攻击者利用，这间接导致了 BEAST 攻击 , Lucky 13攻击 (CVE-2013-0169), 和 POODLE 攻击 (CVE-2014-3566).

**record层的密码学保护–MAC**

TLS record 层 MAC的计算方法：

```cpp
MAC(MAC_write_key, seq_num +
                      TLSCompressed.type +
                      TLSCompressed.version +
                      TLSCompressed.length +
                      TLSCompressed.fragment);
```

其中的seq_num是当前record的 sequence number，每条record都会++， 可以看到把 seq_num，以及record header里面的几个字段也算进来了，这样解决了防重放问题，并且保证record的任何字段都不能被篡改。

算完MAC，格式如下：

```cpp
stream-ciphered struct {
    opaque content[TLSCompressed.length];
    opaque MAC[SecurityParameters.mac_length];
} GenericStreamCipher;
```

然后根据SecurityParameters.cipher_type，选择对应的对称加密算法进行加密。

**record层的密码学保护–stream cipher**

stream cipher: 算stream cipher，stream cipher的状态在连续的record之间会复用。 stream cipher的主力是RC4，但是目前RC4已经爆出多个漏洞，所以实际中基本不使用流加密算法。

**record层的密码学保护– CBC block cipher**

CBC模式块加密 TLS目前靠得住的的块加密cipher也不多，基本就是AES（最靠谱，最主流），Camellia，SEED，（3DES，IDEA之类已经显得老旧，DES请禁用），加密完的格式如下：

```cpp
struct {
    opaque IV[SecurityParameters.record_iv_length];
    block-ciphered struct {
        opaque content[TLSCompressed.length];
        opaque MAC[SecurityParameters.mac_length];
        uint8 padding[GenericBlockCipher.padding_length];
        uint8 padding_length;
    };
} GenericBlockCipher;
```

这个值得说道说道，因为我们码农平常在业界还能看到很多用AES-CBC的地方，其中的几个参数：
- IV： 要求必须用密码学安全的伪随机数生成器(CSPRNG)生成，并且必须是不可预测的，在Linux下，就是用用/dev/urandom，或者用 openssl 库的 RAND_bytes()。注意：TLS 在1版本之前，没有这个IV字段，前一个record的最后一个block被当成下一个record的IV来用，然后粗大事了，这导致了BEAST攻击。 所以，TLS1.2改成了这样。 (还在使用CBC的各位，建议关注一下自己的IV字段是怎么生成出来的。如果要用，做好和TLS1.2的做法保持一致)。其中 SecurityParameters.record_iv_length 一定等于 SecurityParameters.block_size. 例如 AES-256-CBC的 IV 一定是16字节长的，因为AES 128/192/256 的block size都是16字节。
- padding：使用CBC常用的PKCS 7 padding（在block size=16字节这种情况下，和pkcs 5的算法是一回事，java代码里面就可以这么用这个case里，和pkcs 5的结果是一样的）。
- padding_length：就是PKCS 7 padding的最后一个字节。注意2个险恶的陷阱：
  - 实现的代码必须在收到全部明文之后才能传输密文，否则可能会有BEAST攻击
  - 实现上，根据MAC计算的时间，可能进行时间侧通道攻击，因此必须确保–运行时间和padding是否正确无关。

**record层的密码学保护– AEAD cipher**

AEAD 到了我们重点关注的AEAD，AEAD是新兴的主流加密模式，是目前最重要的模式，其中主流的AEAD模式是 aes-gcm-128/aes-gcm-256/chacha20-poly1305

AEAD加密完的格式是：

```cpp
struct {
   opaque nonce_explicit[SecurityParameters.record_iv_length];
   aead-ciphered struct {
       opaque content[TLSCompressed.length];
   };
} GenericAEADCipher;
```

AEAD ciphers的输入是: key，nonce, 明文,和 “additional data”. key是 client_write_key 或者 the server_write_key. 不需要使用 MAC key.

每一个AEAD算法都要指定不同的nonce构造算法，并指定 GenericAEADCipher.nonce_explicit 的长度. 在TLS 1.2中，规定很多情况下，可以按照rfc5116 section 3.2.1的技术来做。其中record_iv_length是nonce的显式部分的长度，nonce的隐式部分从key_block作为 client_write_iv和 and server_write_iv得出，并且把显式部分放在 GenericAEAEDCipher.nonce_explicit 里.

在TLS 1.3 draft中，做了更改:

规定 AEAD算法的 nonce的长度规定为 max(8 bytes, N_MIN)，即如果N_MIN比8大，就用N_MIN; 如果比8小，就用8。
- 规定 N_MAX小于8字节的AEAD不得用于TLS。
- 规定TLS AEAD中每条record的nonce通过下面的方法构造出来： 64bit的sequence number的右侧填充0，直到长度达到iv_length。然后把填充过的sequence number和静态的 client_write_iv或 server_write_iv （根据发送端选择）做异或(XOR)。异或完成后，得到的 iv_length 的nonce就可以做每条record的nonce用了。

AEAD输入的明文就是 TLSCompressed.fragment (记得上面的介绍吗？AEAD是MAC和encrypt的集成，所以输入数据不需要在算MAC了).

AEAD输入的additional_data 是:

```cpp
additional_data = seq_num + TLSCompressed.type +
                  TLSCompressed.version + TLSCompressed.length;
```

“+” 表示字符串拼接。 可以看到，此处类似上面的MAC计算，算入了seq_num来防重放，type,version,length等字段防止这些元数据被篡改。

```cpp
AEADEncrypted = AEAD-Encrypt(write_key, nonce, plaintext,
                             additional_data)
```

**解密+验证完整性：**

```cpp
TLSCompressed.fragment = AEAD-Decrypt(write_key, nonce,
                                            AEADEncrypted,
                                            additional_data)
```

如果解密/验证完整性失败,就回复一条 fatal bad_record_mac alert 消息.

aes-gcm的iv长度，nonce长度，nonce构成等，后续再深入探讨。

**record层的密码学保护– Key扩展**

TLS握手生成的master_secret只有48字节，2组encryption key, MAC key, IV加起来，长度一般都超过48，(例如 AES_256_CBC_SHA256 需要 128字节),所以，TLS里面用1个函数，来把48字节延长到需要的长度，称为PRF：

```cpp
key_block = PRF(SecurityParameters.master_secret,
                "key expansion",
                SecurityParameters.server_random +
                SecurityParameters.client_random);
```

然后，key_block像下面这样被分割：

```cpp
client_write_MAC_key[SecurityParameters.mac_key_length]
server_write_MAC_key[SecurityParameters.mac_key_length]
client_write_key[SecurityParameters.enc_key_length]
server_write_key[SecurityParameters.enc_key_length]
client_write_IV[SecurityParameters.fixed_iv_length]
server_write_IV[SecurityParameters.fixed_iv_length]
```

TLS使用HMAC结构，和在CipherSuite中指定的hash函数（安全等级起码是SHA256的水平） 来构造PRF，

首先定义P_hash，把(secret,seed)扩展成无限长的字节流：

```cpp
P_hash(secret, seed) = HMAC_hash(secret, A(1) + seed) +
                       HMAC_hash(secret, A(2) + seed) +
                       HMAC_hash(secret, A(3) + seed) + ...
```

其中”+“表示字符串拼接。 A() 定义为:

```cpp
A(0) = seed
A(i) = HMAC_hash(secret, A(i-1))
```

TLS的 PRF 就是把 P_hash 应用在secret上:

```cpp
PRF(secret, label, seed) = P_<hash>(secret, label + seed)
```

其中 label 是一个协议规定的，固定的 ASCII string.

要注意的是，TLS 1.3里面已经废弃了这种方式，改为使用更靠谱的 HKDF，HKDF 也是 html5的WebCryptoAPI的标准算法之一。

### 数字证书

数字证书，又称互联网上的”身份证”，用于唯一标识一个组织或一个服务器的，这就好比我们日常生活中使用的”居民身份证”，用于唯一标识一个人。

网站的证书也是同样的道理。一般来说数字证书从受信的权威证书授权机构 (Certification Authority，证书授权机构)买来的。一般浏览器在出厂时就内置了诸多知名CA。

实际应用中，HTTPS并非直接传输公钥信息，而是使用携带公钥信息的数字证书来保证公钥的安全性和完整性。用来保障HTTPS服务端发送给客户端的公钥信息不被篡改。

#### 数字证书和 CA 机构
一个数字证书通常包含了：

- 服务器公钥
- 持有者信息
- 证书认证机构（CA）的信息
- CA 对这份文件的数字签名及使用的算法
- 证书有效期
- 还有一些其他额外信息

数字证书的作用，是用来认证公钥持有者的身份，以防止第三方进行冒充。说简单些，证书就是用来告诉客户端，该服务端是否是合法的，因为只有证书合法，才代表服务端身份是可信的。

我们用证书来认证公钥持有者的身份（服务端的身份），那证书又是怎么来的？又该怎么认证证书呢？为了让服务端的公钥被大家信任，服务端的证书都是由 CA （Certificate Authority证书认证机构）签名的，CA 就是网络世界里的公安局、公证中心，具有极高的可信度，所以由它来给各个公钥签名，信任的一方签发的证书，那必然证书也是被信任的。之所以要签名，是因为签名的作用可以避免中间人在获取证书时对证书内容的篡改。

数字证书签发和验证流程

如下图所示，为数字证书签发和验证流程：

![](https://github.com/ashenone0917/image/blob/main/ca-signature.jpg)

- CA 签发证书的过程，如上图左边部分：
  - 首先 CA 会把持有者的公钥、用途、颁发者、有效时间等信息打成一个包(蓝色)，然后对这些信息进行 Hash 计算，得到一个 Hash 值
  - 然后 CA 会使用自己的私钥将该 Hash 值加密，生成 Certificate Signature，也就是 CA 对证书做了签名
  - 最后将 Certificate Signature 添加在文件证书上，形成数字证书
- 客户端校验服务端的数字证书的过程，如上图右边部分：
  - 首先客户端会使用同样的 Hash 算法计算该证书的 Hash 值 H1
  - 通常浏览器和操作系统中集成了 CA 的公钥信息，浏览器收到服务器的证书后可以使用 CA 的公钥解密 Certificate Signature 内容，得到一个 Hash 值 H2
  - 最后比较 H1 和 H2，如果值相同，则为可信赖的证书，否则则认为服务器证书不可信

#### 证书链

但事实上，证书的验证过程中还存在一个证书信任链的问题，因为我们向 CA 申请的证书一般不是根证书签发的，而是由中间证书签发的，比如百度的证书，从下图你可以看到，证书的层级有三级：

![](https://github.com/ashenone0917/image/blob/main/zhenshu.png)

对于这种三级层级关系的证书的验证过程如下：

- 客户端收到com的证书后，发现这个证书的签发者不是根证书，就无法根据本地已有的根证书中的公钥去验证baidu.com证书是否可信。于是，客户端根据baidu.com证书中的签发者，找到该证书的颁发机构是 “GlobalSign Organization Validation CA – SHA256 – G2”，然后向 CA 请求该中间证书。
- 请求到证书后发现 “GlobalSign Organization Validation CA – SHA256 – G2” 证书是由 “GlobalSign Root CA” 签发的，由于 “GlobalSign Root CA” 没有再上级签发机构，说明它是根证书，也就是自签证书。应用软件会检查此证书有否已预载于根证书清单上，如果有，则可以利用根证书中的公钥去验证 “GlobalSign Organization Validation CA – SHA256 – G2” 证书，如果发现验证通过，就认为该中间证书是可信的。
- “GlobalSign Organization Validation CA – SHA256 – G2” 证书被信任后，可以使用 “GlobalSign Organization Validation CA – SHA256 – G2” 证书中的公钥去验证com证书的可信性，如果验证通过，就可以信任baidu.com证书。

在这些步骤中，最开始客户端只信任根证书 GlobalSign Root CA 证书的，然后 “GlobalSign Root CA” 证书信任 “GlobalSign Organization Validation CA – SHA256 – G2” 证书，而 “GlobalSign Organization Validation CA – SHA256 – G2” 证书又信任baidu.com证书，于是客户端也信任baidu.com证书。

总括来说，由于用户信任 GlobalSign，所以由 GlobalSign 所担保的baidu.com可以被信任，另外由于用户信任操作系统或浏览器的软件商，所以由软件商预载了根证书的 GlobalSign 都可被信任。

![](https://github.com/ashenone0917/image/blob/main/ca-flow.jpg)

操作系统里一般都会内置一些根证书：

![](https://github.com/ashenone0917/image/blob/main/root-cert.jpg)

这样的一层层地验证就构成了一条信任链路，整个证书信任链验证流程如下图所示：

![](https://github.com/ashenone0917/image/blob/main/level.jpg)

最后一个问题，为什么需要证书链这么麻烦的流程？Root CA 为什么不直接颁发证书，而是要搞那么多中间层级呢？

这是为了确保根证书的绝对安全性，将根证书隔离地越严格越好，不然根证书如果失守了，那么整个信任链都会有问题。

### TLS 1.3 VS TLS 1.2
TLS 1.3 是时隔九年对 TLS 1.2 等之前版本的新升级，也是迄今为止改动最大的一次。针对目前已知的安全威胁，IETF（Internet Engineering Task Force，互联网工程任务组） 正在制定 TLS 1.3 的新标准，使其有望成为有史以来最安全，但也最复杂的 TLS 协议。

TLS 1.3 与之前的协议有较大差异，主要在于：
- 相比过去的的版本，引入了新的密钥协商机制 — PSK
- 支持 0-RTT 数据传输，在建立连接时节省了往返时间
- 废弃了 3DES、RC4、AES-CBC 等加密组件，废弃了 SHA1、MD5 等哈希算法
- ServerHello 之后的所有握手消息采取了加密操作，可见明文大大减少
- 不再允许对加密报文进行压缩、不再允许双方发起重协商
- DSA 证书不再允许在 TLS 1.3 中使用

对比旧协议中的不足，TLS 1.3 确实可以称得上是向前迈了一大步。既避免之前版本出现的缺陷，也减少了 TLS 握手的时间。

总结一下，TLS 1.3 与以前的版本相比具有如下两个大的优势，分别是：

#### 更快的访问速度
为了对比 TLS 1.3 在 TLS 握手阶段的变化， 这里将 TLS 1.2 和 TLS 1.3 在 TLS 握手阶段进行对比。

![](https://github.com/ashenone0917/image/blob/main/tls-1.2.png)

从上图可以看出，使用 TLS 1.2 需要两次往返（ 2-RTT ）才能完成握手，然后才能发送请求。

![](https://github.com/ashenone0917/image/blob/main/tls-1.3.png)

TLS 1.3 的握手不再支持静态的 RSA 密钥交换，这意味着必须使用带有前向安全的 Diffie-Hellman 进行全面握手。从上图可以看出，使用 TLS 1.3 协议只需要一次往返（ 1-RTT ）就可以完成握手。

![](https://github.com/ashenone0917/image/blob/main/tls12-vs-tls13.png)

上图的右边部分就是 TLS 1.3 的握手过程，可以发现 TLS 1.3 把 Hello 和公钥交换这两个消息合并成了一个消息，于是这样就减少到只需 1 RTT 就能完成 TLS 握手。

怎么合并的呢？具体的做法是，客户端在 Client Hello 消息里带上了支持的椭圆曲线，以及这些椭圆曲线对应的公钥。

服务端收到后，选定一个椭圆曲线等参数，然后返回消息时，带上服务端这边的公钥。经过这 1 个 RTT，双方手上已经有生成会话密钥的材料了，于是客户端计算出会话密钥，就可以进行应用数据的加密传输了。

相比 TLS 1.2，TLS 1.3 的握手时间减半。这意味着访问一个移动端网站，使用 TLS 1.3 协议，可能会减少将近 100ms 的时间。

#### 更强的安全性
TLS 的发展有 20 多年的历史，在之前的版本中，TLS 1.2 是高度可配置的，为了更好的兼容旧版本的浏览器，这意味着那些易受攻击的站点始终在运行着不安全的加密算法，这让互联网黑客有可乘之机。

TLS 1.3 在之前版本的基础上删除了那些不安全的加密算法，这些加密算法包括：
- RSA 密钥传输 —— 不支持前向安全性
- CBC 模式密码—— 易受 BEAST 和 Lucky 13 攻击
- RC4 流密码 —— 在 HTTPS 中使用并不安全
- SHA-1 哈希函数—— 建议以 SHA-2 取而代之
- 任意 Diffie-Hellman 组—— CVE-2016-0701 漏洞
- 输出密码 —— 易受 FREAK 和 LogJam 攻击

TLS1.3 对密码套件进行“减肥”了，对于密钥交换算法，废除了不支持前向安全性的 RSA 和 DH 算法，只支持 ECDHE 算法。对于对称加密和签名算法，只支持目前最安全的几个密码套件，比如 openssl 中仅支持下面 5 种密码套件：

- TLS_AES_256_GCM_SHA384
- TLS_CHACHA20_POLY1305_SHA256
- TLS_AES_128_GCM_SHA256
- TLS_AES_128_CCM_8_SHA256
- TLS_AES_128_CCM_SHA256

之所以 TLS1.3 仅支持这么少的密码套件，是因为 TLS1.2 由于支持各种古老且不安全的密码套件，中间人可以利用降级攻击，伪造客户端的 Client Hello 消息，替换客户端支持的密码套件为一些不安全的密码套件，使得服务器被迫使用这个密码套件进行 HTTPS 连接，从而破解密文。

总之，TLS 1.3 相比老版本的 TLS 协议将会更加安全，这也代表着互联网安全的一大进步。

### TLS 1.3具体改动
TLS 1.3 的改动 值得关注的重大改进有：

- RTT支持
- RTT握手支持
- 改为使用HKDF做密钥拓展
- 彻底禁止RC4
- 彻底禁止压缩
- 彻底禁止aead以外的其他算法
- 去除aead的显式IV
- 去除了AEAD的AD中的长度字段
- 去除ChangeCipherSpec
- 去除重协商
- 去除静态RSA和DH密钥协商
TLS 1.3 的各项改进，主要就是针对移动互联网场景的。TLS 1.3 去掉了 ChangeCipherSpec ，这样record之上有3个协议：handshake，alert，application data

#### record层的密码学保护的改动
由于只保留了aead，所以不需要MAC key了。aead的具体参数用法也有调整，前文有。KDF 换成了标准的HKDF，有2种 tls_kdf_sha256, tls_kdf_sha384

#### handshake协议的改动
鉴于session ticket如此之好用，简直人见人爱，所以 TLS 1.3 直接把session ticket内置了，并改名叫 PSK。要注意的是，此 PSK 和 tls 1.2中一个很生僻的psk(见 rfc4279 )并不是一回事。

综合考虑了 session resuming ，session ticket后， TLS 1.3 提出了3种handshake模式：

- Diffie-Hellman （ 包含 DH 和 ECDH 两种，下文说到 ECDH 的地方，请自行脑补成 “ECDH/DH”）.
- A pre-shared symmetric key (PSK) ，预先共享的对称密钥，此处用统一的模型来处理session resuming 和 rfc4279的psk
- A combination of a symmetric key and Diffie-Hellman ，前两者合体

**1-RTT 握手**

首先，TLS 1.2 的握手有2个rtt，第一个rtt是 ClientHello/ServerHello，第二个rtt是ClientKeyExchange/ServerKeyExchange， 之所以KeyExchange要放在第二个rtt，是由于tls1.2要支持多种密钥交换算法，和各种不同参数(比如 DH还是ECDH还是RSA，ECDHE用什么曲线，DH用什么群生成元，用什么模数，等等)，这些算法和参数都依赖第一个rtt去协商出来， TLS1.3大刀阔斧地砍掉了各种自定义DH群，砍掉了ECDH的自定义曲线，砍掉了RSA协商，密钥协商的算法只剩下不多几个，而且其实大家实际应用中基本都用 ECDH P-256，也没啥人用别的，所以干脆让客户端缓存服务器上一次用的是啥协商算法，把 KeyExchange直接和入第一个rtt，客户端在第一个rtt里直接就用缓存的这个算法发KeyExchange的公钥，如果服务器发现客户端发上来的算法不对，那么再告诉正确的，让客户端重试好了。 这样，就引入了 HelloRetryRequest 这个消息。

这样，基本没有副作用，就可以降到 1-RTT。 这是TLS 1.3 的完整握手。

显然，如果一个协议只有一种密钥协商算法，比如定死为 ECDH P-256，那一定可以做到 1-RTT

**有副作用的 0-RTT握手**

0-RTT应该是受Google的QUIC协议的启发， 如果服务器把自己的 ECDH 公钥长期缓存在客户端，那么客户端就可以用缓存里的ECDHE公钥，构造一个电子信封，在第一个RTT里，直接就发送应用层数据了。 这个长期缓存在客户端的ECDH公钥，称为 半静态 ECDH 公钥（ semi-static (EC)DH share ） ECDH公钥通过 ServerConfiguration 消息发送给客户端。

这个0-rtt优化是有副作用的：

- RTT发送的应用数据没有前向安全性。
- 跨连接可以重放0-RTT里的应用数据（任何服务器端无共享状态的协议，都无法做到跨连接防重放）
- 如果服务器端 半静态 ECDH公钥对应的私钥泄露了，攻击者就可以伪装成客户端随意篡改数据了。
服务器在 ServerConfiguration 消息里把半静态 ECDH 公钥发送给客户端。 ServerConfiguration 值得关注一下：

```cpp
struct {
    opaque configuration_id<1..2^16-1>;
    uint32 expiration_date;
    NamedGroup group;
    opaque server_key<1..2^16-1>;
    EarlyDataType early_data_type;
    ConfigurationExtension extensions<0..2^16-1>;
} ServerConfiguration;
```

其中的 expiration_date 是本 ServerConfiguration 最后的有效期限。 这个值绝对不允许大于7天。 客户端绝对不允许存储 ServerConfiguration 大于7天，不管服务器怎么填这个值。

0-RTT 中的应用数据，放在 EarlyDataIndication 中发送，TLS 1.3 还特意给 EarlyDataIndication 定义了一种 ContentType : early_handshake （共四种 alert(21), handshake(22), application_data(23), early_handshake(25) ）

**Resumption 和 PSK**

TLS 1.3 里面，把session resumption/session ticket 恢复出来的key，和 psk (rfc4279)， 统一在一个 handshake PSK 模式下处理。

PSK CipherSuite可以 把PSK和ECDHE结合起来用，这样是有前向安全性的。 也可以仅仅使用PSK，这样就没有前向安全性。

**Key Schedule 过程的改动**

TLS 1.3 中，综合考虑的 session ticket的各种情况后，提出了 ES，SS 两个概念，统一处理密钥协商的各种情况。 在各种handshake模式下，ES和SS的取值来源不同。

- Ephemeral Secret (ES)：每个连接新鲜的 ECDHE 协商得出的值。凡是从 ES 得出的值，都是前向安全的（当然，在 PSK only模式下，不是前向安全的）。
- Static Secret (SS)：从静态，或者半静态key得出的值。例如psk，或者服务器的半静态 ECDH 公钥。
在各种 handshake 模式下：

![](https://github.com/ashenone0917/image/blob/main/QQ截图20220730161027.png)

如上表：

- 完整的 1-RTT握手的时候， SS 和 ES 都是用的 ephemeral key ，这样是一定有前向安全性的。
- 使用 0-RTT 的握手的时候，使用客户端的 ephemeral key 和 服务器端的半静态 ECDH 公钥生成 SS，
- 纯 PSK，这种场景完全没有前向安全性，应该避免。
- PSK +　ECDHE，这种场景比较有意思，SS是用的Pre-Shared Key，没有前向安全性，ES 用的 ephemeral key，有前向安全性。
- 
可以看到，相比 TLS 1.2 的 session ticket，TLS 1.3 中 的 PSK + ECDHE，是结合了 ES 的，这样就有了前向安全性，相对更安全。

和 TLS 1.2 不同的是，TLS 1.3的 master_secret 是使用 ES和SS 两个得出的。

```cpp
HKDF-Expand-Label(Secret, Label, HashValue, Length) = HKDF-Expand(Secret, Label + '<pre class="EnlighterJSRAW" data-enlighter-language="generic">HKDF-Expand-Label(Secret, Label, HashValue, Length) = HKDF-Expand(Secret, Label + '\0' + HashValue, Length)
1. xSS = HKDF(0, SS, "extractedSS", L)
2. xES = HKDF(0, ES, "extractedES", L)
3. master_secret = HKDF(xSS, xES, "master secret", L)
4. finished_secret = HKDF-Expand-Label(xSS, "finished secret", handshake_hash, L)
</pre>' + HashValue, Length)
1. xSS = HKDF(0, SS, "extractedSS", L)
2. xES = HKDF(0, ES, "extractedES", L)
3. master_secret = HKDF(xSS, xES, "master secret", L)
4. finished_secret = HKDF-Expand-Label(xSS, "finished secret", handshake_hash, L)
```

**Traffic Key Calculation**

加密流量用的key，在 TLS 1.3 里面称为 Traffic Key，由于多引入了一种ContentType，在不同的ContentType下，Traffic Key 并不相同。 如下表：

![](https://github.com/ashenone0917/image/blob/main/QQ截图20220730161135.png)

要关注的是， Early Data 的 Traffic Key 是用 xSS 算出来的。也就是说，是用 Pre-Shared Key决定的。因此是没有前向安全性的。

在一个TLS 连接中，究竟是用哪种 handshake 模式，是由 CipherSuite 协商决定的。

参考链接:
[理解 HTTPS 原理，SSL/TLS协议详解](https://www.biaodianfu.com/https-ssl-tls.html)
