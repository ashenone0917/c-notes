对于某些类来说，我们希望这些类只被实例化一次，比如连接池、类工厂、文件系统等。这就是设计模式中的单例模式(Singleton Pattern)。

在多线程并发环境中，如何线程安全的获取（初始化）实例？本文对目前学到的几种方法进行总结。

## Meyers Singleton
Meyers Singleton的实现方式基于["static variables with block scope"](https://www.modernescpp.com/index.php/thread-safe-initialization-of-data)的自动线程安全特性，非常简单易懂。
```cpp
class MeyersSingleton{
public:
  static MySingleton& getInstance(){
    static MySingleton instance;
    // volatile int dummy{};
    return instance;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;
};
```
## std::call_once & std::once_flag
这种方式基于C++新特性，保证多线程下实例化方法只被调用一次。
```cpp
class CallOnceSingleton{
public:
  static MySingleton& getInstance(){
    std::call_once(initInstanceFlag, &MySingleton::initSingleton);
    // volatile int dummy{};
    return *instance;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;
  static MySingleton* instance;
  static std::once_flag initInstanceFlag;
  static void initSingleton(){
    instance= new MySingleton;
  }
};

MySingleton* MySingleton::instance= nullptr;
std::once_flag MySingleton::initInstanceFlag;
```

## 锁机制
```cpp
std::mutex myMutex;

class MySingleton{
public:
  static MySingleton& getInstance(){
    std::lock_guard<std::mutex> myLock(myMutex);
    if ( !instance ){
        instance= new MySingleton();
    }
    // volatile int dummy{};
    return *instance;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;

  static MySingleton* instance;
};


MySingleton* MySingleton::instance= nullptr;
```

每次getInstance方法调用，都需要申请和释放锁，开销非常大。
## 原子变量atomic variables
实际上只有当程序启动，多个线程并发初始化第一个instance时，锁的同步才有必要。当第一个instance初始化之后，每次getInstance只需要读取已经创建好的instance即可，不需要使用互斥锁。

原子变量方法首先判断是否已经初始化实例，如果已经实例化，直接返回即可；否则回退至锁方案。对于原子变量的使用，可细分为Sequential Consistency和Acquire/Release两种方式。

### Sequential Consistency
```cpp
class MySingleton{
public:
  static MySingleton* getInstance(){
    MySingleton* sin= instance.load();
    if ( !sin ){
      std::lock_guard<std::mutex> myLock(myMutex);
      sin= instance.load();
      if( !sin ){
        sin= new MySingleton();
        instance.store(sin);
      }
    }   
    // volatile int dummy{};
    return sin;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;

  static std::atomic<MySingleton*> instance;
  static std::mutex myMutex;
};


std::atomic<MySingleton*> MySingleton::instance;
std::mutex MySingleton::myMutex;
```

### Acquire/Release Semantic
```cpp
class MySingleton{
public:
  static MySingleton* getInstance(){
    MySingleton* sin= instance.load(std::memory_order_acquire);
    if ( !sin ){
      std::lock_guard<std::mutex> myLock(myMutex);
      sin= instance.load(std::memory_order_relaxed);
      if( !sin ){
        sin= new MySingleton();
        instance.store(sin, std::memory_order_release);
      }
    }   
    // volatile int dummy{};
    return sin;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;

  static std::atomic<MySingleton*> instance;
  static std::mutex myMutex;
};
std::atomic<MySingleton*> MySingleton::instance;
std::mutex MySingleton::myMutex;
```

## Singleton based on CRTP
以上的单例方法对于每个单例类目标，都要实现一次，难免有造轮子的感觉。这里介绍一种基于CRTP的单例实现：目标类通过继承模板类Singleton来实现单例。
```cpp
template <typename Derived>
class Singleton {
 public:
  static Derived* GetInstance() {
    Derived* temp = instance_.load(std::memory_order_acquire);
    if (temp == nullptr) {
      std::lock_guard<std::mutex> lock(mutex_);
      temp = instance_.load(std::memory_order_relaxed);
      if (temp == nullptr) {
        temp = new Derived;
        instance_.store(temp, std::memory_order_release);
      }
    }
    return temp;
  }

 protected:
  Singleton() = default;
  ~Singleton() = default;
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;

 private:
  static std::atomic<Derived*> instance_;
  static std::mutex mutex_;
};
template<typename Derived>
std::atomic<Derived*> Singleton<Derived>::instance_(nullptr);
template<typename Derived>
std::mutex Singleton<Derived>::mutex_;
```

## 引用
>https://www.modernescpp.com/index.php/thread-safe-initialization-of-a-singleton
>
>https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/
