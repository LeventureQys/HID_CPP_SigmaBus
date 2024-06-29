本文大概讲解一下HID_DeviceManager中如何运行的线程写入流程

![](https://gitee.com/Leventure/picture-bed/raw/master/LoopWrite.png)

```c++
using DataList = std::list<std::pair<int,std::pair<std::list<uint8_t>,std::pair<size_t, unsigned char*>>>>;
```

有关这个数据结构DataList的组织是如何组织的，其实也很简单

按照分层来解释：

1> std::list
第一个list是用队列结构来装整个组织，因为要保证先进先出，所以所有的数据都是从队列后方push进去

2>std::pair
这一对std::pair内容如下：
```C++
std::pair<int,std::pair<std::list<uint8_t>,std::pair<size_t, unsigned char*>>>
```
这里的key代表的是设备的index，value是发送信息的所有数据内容信息

3> std::pair
```c++
std::pair<std::list<uint8_t>,std::pair<size_t, unsigned char*>>
```

这里key代表的是发送的地址高位低位地址组，这里的消息用于处理应答信息

4> std::pair

```c++
std::pair<size_t, unsigned char*>
```
key:数据段长度
value: 数据段内容