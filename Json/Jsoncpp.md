# 1 Jsoncpp的下载和编译

在C++标准库中没有用于解析Json的类，但可以使用一些开源的库进行Json的解析

在C++常用的就是**Jsoncpp**，而在C语言中常用的是CJson

后续数据连接池的配置文件是用Json写的，因此要学习以下Json的开源库

## 1.1 下载

* 下载Jsoncpp源码

Jsoncpp是个跨平台的C++开源库，提供用于解析Json的类

使用前先要在github仓库下载源码，地址如下

<https://github.com/open-source-parsers/jsoncpp>


* 下载cmake工具
  
下载完jsoncpp的源码之后一般不会直接使用，而是将其编译成相应的库文件(动态库或静态库)

从GitHub上下载的源码不能直接通过VS打开，它提供的默认编译方式是`cmake`

可以使用cmake将下载的源码编译生成一个VS项目，这样就可以通过VS编译处需要的库文件了

* 通过CMake工具生成VS项目

在cmake的bin目录下提供了对应的cmake图形化界面...

由于我的cmake安装的版本相对较低，不支持vs2022，所以这里选择了GCC


# 2 jsoncpp的使用

`Jsoncpp`库中的类被定义到一个`Json`命令空间中，可使用using指令导出：
    `using namespace Json;`


使用jisoncpp库解析json格式的数据，需要用到三个类：

1. Value：将json支持的数据类型进行了包装，最终得到一个Value类型

2. FastWriter：将Value对象中的数据序列化为字符串

3. Readers：反序列化，将json字符串解析成Value类型


## 2.1 Value类

Value类可以看作是一个包装器，它可以封装所有Json的类型：


* 构造函数
Value类提供了很多构造函数，用来封装数据，得到一个统一的类型
```
Value(ValueType type = nullValue);
Value(Int value);
Value(UInt value);
Value(Int64 value);
Value(UInt64 value);
Value(double value);
Value(const char* value);
Value(const char* begin, const char* end);
Value(const Value& other);
Value(Value&& other);
```


* 检测保存的数据类型
Value提供了isType方法，检测是不是Type类型·
```
bool isNull() const;
bool isBoll() const;
bool isInt() const;
bool isInt64() const;
bool isUint() const;
bool isUint64() const;
bool isIntergral() const; // 长整型
bool isDouble() const;
bool isNumeric() const; // 数值类型
bool isString() const;
bool isArray() const;
bool isobject() const;  
```


* 将Value对象转化为实际类型
通过as方法可以将对象转化为实际的类型，函数原型如下：

```
// Type可以是支持的所有类型
Type asType() const;
```
注意:
Type == String 返回C++风格的字符串std::string
Type == CString 返回C风格的字符串const char*
这里的Type只能是**基础类型**


* 对Json数组的操作

```
// 数组元素个数
ArratIndex size() const; // 返回一个长整型


// 重载[]
Value& operator[](ArrayIndex index);
Value& operator[](Int index);
const Value& operator[](ArrayIndex index);
const Value& operator[](Int index);
// 注意得到的是Value类型的对象，需要判断其类型在做处理
// 如果是基础类型，直接调用as方法解析
// 如果是对象或数组，则需要进行下一层的解析


// 除了重载[]，Value类还提供了get方法
Value get(ArrayIndex index, const Value& default) const;
// 访问下标为index的元素，如果访问不到，则返回default
// 访问不到：指定index值无效

// 后附元素
Value& append(const Value& value);

// 此外，Value提供了迭代器操作
const_iterator begin() const;
const_iterator end() const;
iterator begin();
iterator end();
```


* 对Json对象的操作

```
// 通过key来访问value,key可以是C/C++的字符串
Value& operator[](const char* key);
const Value& operator[](const char* key) const;
Value& operator[](const JSONCPP_STRING&* key);
const Value& operator[](const JSONCPP_STRING& key) const;
Value& operator[](const StaticString& key);


// 同样也可以使用get方法，第二个参数为访问不到返回的Value
Value get(const Key key, const Value& defaultValue) const;

// 得到对象中的所有Key值
typedef std::vector<std::string> Members;
Members getMembers() const;
```


* 将对象中的数据转化为std::string

```
// 序列化格式，带有换行符
// 方便我们去阅读
std::string toStyledString() const;
```


## 2.2 FastWriter类

```
// 序列化，不带有换行符
// 用于数据的网络传输
std::string Json::FastWriter::write(const Value& root);
```


## 2.3 Reader类
这个类用于将Json格式字符串转换为Value类
* Json格式字符串的获取：
  * 别人发送
  * 读磁盘文件


```
bool Json::Reader::parse(const std::string& document, 
                        Value& root, bool collectComment = true);
```
* 函数参数:
  * - document : Json风格的字符串
  * - root : 传出参数，存储了Json字符串解析出的数据
  * - collectComment : 是否保留Json中的注释信息

* `parse`函数还有一个重载版本，即以C风格字符串存储Json字符串
  * 此时就要指定这个C风格字符串的头尾指针

* `parse`还有一个重载，即不传递字符串，而是指定一个`流对象`
`bool Json::Reader::parse(std::istream& is, Value& root, bool collectCommeents = true);`

不管调用哪一个重载parse，都会将得到的结果存到Value对象中


# 3 写代码lu

首先找到对应的头文件，位于git下来的目录下的include目录下的json目录中
`D:/code/Json/Jsoncpp_source/jsoncpp-master/jsoncpp-master/include`

这边由于只靠vccode的C/C++插件要配置链接库文件非常麻烦，这里选用CMake：

即在`CMakeLists.txt`指定库的相关信息，so esay！！！

小感悟：像是CMake、Makefile这种环境构建工具，刚开始学的时候为了用它而用它，感觉没啥屌用，还很麻烦

可在需要用的时候，这东西实在太方便了！！！


回归正题：

* 头文件的包含：
        `#include <json/json.h>`

由于前面头文件路径只指定到include目录，所以包含时要带上后面的路径

* 命名空间`Json`的使用
    前面提到的三个类都在这个命令空间中

* 准备工作做好了，开干！

测试文件路径：`D:\code_cpp\test_23_9_1\test_json`

最后的最后涉及一点点的文件操作：
```
// write -> ofstream --> wo
// read -> iftream --> ri

// 要将字符串json写入到test.json中
ofstream ofs("test.json");
ofs << json;
ofs.close();

// 从test.json中读数据
ifsteam ifs("test.json");
Reader rd;
Value root;
rd.parse(ifs, root);

// 其它操作
if(root.isArray()) {

} else {

}
```

最最最后注意：编译好的库是64位的...

* 不要试图写一个万能的读Json的函数，这种想法是不切实际的：
  * json数组和对象的嵌套是千变万化的...
  * 在项目开发中，会根据实际需求组织各种格式的json文件

* 因此，在解析json文件或数据块的时候，每个文件或数据块都对应一个特定的解析函数
  * 这样解析函数写起来简单，维护也容易。