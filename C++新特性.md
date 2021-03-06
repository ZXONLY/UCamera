# C++新特性

### explicit

* explicit只能用于类内部的构造函数
* 被explicit修饰的构造函数的类，不能发生相应的隐式类型转换

### enable_if 和 enable_if_t

### forward

完美转发

## 左值和右值

- **左值***(lvalue, left value)* 是 能被取地址、不能被移动 的值
- **右值* **(rvalue, right value)* 是 表达式中间结果/函数返回值（可能拥有变量名，也可能没有）

### **左值引用 vs 右值引用 vs 常引用**

引用类型 可以分为两种：

- **左值引用***(l-ref, lvalue reference)* 用 `&` 符号引用 左值（但不能引用右值）
- **右值引用***(r-ref, rvalue reference)* 用 `&&` 符号引用 右值（也可以移动左值）



## std::variant

C++17

std::variant类似于union

```C++
using IntFloatString = std::variant<int, float, std::string>; // 定义支持int、float、string三个类型，并取一个别名
//初始化一个variant
TEST_F(InitVariant) {

    IntFloatString i = 10;
    ASSERT_EQ(10, std::get<int>(i) );

    IntFloatString f = 20.0f;
    ASSERT_EQ(20.0f, std::get<float>(f) );

    IntFloatString s = "hello world";
    ASSERT_EQ("hello world", std::get<std::string>(s));
}
```

使用std::visit的方式访问

## STD::mt19937

`std::mt19937`是伪随机数产生器，用于产生高性能的随机数。 `C++11`引入。
返回值为`unsigned int`。

用法：

```C ++
std::mt19937 rdgen { std::random_device{}() };
std::cout << rng() << endl;
```



