# C++ Advanced Improvement Course

**章节目录**
- [1.1 对象的优化](1.1-对象的优化)
- [1.2 智能指针](1.2-智能指针)
- [1.3 bind 绑定器和 function 函数对象](1.3-bind-绑定器和-function-函数对象)
- [1.4 C++11 知识点总结以及多线程](1.4-C++11-知识点总结以及多线程)
- [1.5 设计模式](1.5-设计模式)
- [1.6 代码应用实践](1.6-代码应用实践)
- [1.7 C++ 面经讲解](1.7-C++-面经讲解)
- [1.8 C++11 STL 容器中 emplace 剖析](1.8-C++11-stl-容器中-emplace-剖析)
## 1.1 对象的优化 

### 1.1.1 临时对象的优化 

#### 1.1.1.1 概念 

**临时对象**（也称为右值）是在表达式求值过程中创建的对象，它们没有名字。临时对象通常在以下几种情况下产生：

- 拷贝构造：当一个对象通过拷贝构造函数初始化另一个对象时。
- 赋值操作：当一个对象通过赋值操作符赋值给另一个对象时。
- 函数返回值：当一个函数返回一个局部对象时。
- 类型转换：当进行隐式或显式类型转换时。

**临时对象的生命周期**

- **生命周期**：临时对象的生命周期非常短暂，通常只存在于创建它的那条语句中。一旦该语句执行完毕，临时对象就会被销毁。
- **引用绑定**：可以通过引用（特别是常量引用）绑定临时对象，这样临时对象的生命周期会被延长到引用的作用域结束。

#### 1.1.1.2 易错点 

**临时对象最好不要使用指针去指向它 , 由于临时对象没有持久的内存地址，并且在表达式结束后会被立即销毁，因此对临时对象取地址会导致悬空指针。编译器会阻止这种操作，以避免未定义行为 , 同时想对它进行解引用会引发不必要的错误**	

#### 1.1.1.3 简单函数说明 

```c++
class Test {
    public: 
        Test(int a = 10)
            :ma(a)
        {
            std::cout << "Test()" << std::endl;
        }
        ~Test() {
            std::cout << "~Test()" << std::endl;
        }

        Test(const Test &t) 
            : ma(t.ma)
        {
            std::cout << "Test(const Test &t)" << std::endl;
        }
        Test& operator = (const Test &t) {
            std::cout << "operator = " << std::endl;
            ma = t.ma;
            return *this;
        }

    private: 
        int ma;
};
int main() {
    Test t1;
    Test t2(t1);
    Test t3 = t1; // 拷贝构造 t3 还未被创建
    // Test(20) : 显式生成临时对象 -- 临时对象没有名字, 别的地方也没有办法使用这个临时对象 , 生存周期就是所在的语句
    // C++ 编译器对于构造的优化 : 临时对象生成新对象的时候 , 临时对象就不产生了 , 直接构造新对象就可以了
    Test t4 = Test(20);  // Test t4(20);  这两种方式是完全没有区别的 , 这里是要新构造一个对象, 临时对象就不生成了

    std::cout << "------------------" << std::endl;

    t4 = t2; // 赋值运算符重载函数 , t4 已经存在 
    // t4.operator=(const Test &t)
    // 显式生成临时对象
    t4 = Test(30); // 这里的临时对象一定是生成的
    t4 = (Test)30; // int -> Test(int) , 能不能转呢, 编译器看编译器有没有一个带整型的参数额构造函数 , 也是生成临时对象,调用赋值运算符重载

    // 隐式生成临时对象
    t4 = 30; // Test(30) int -> Test(int)
    std::cout << "------------------" << std::endl;

    // Test* p = &Test(40); // 生成临时对象 , 要不然指针的地址放在哪里不知道 
    //除了语句之后, p 指向的是一个已经析构的临时对象 , 所以不应该用一个指针指向临时对象
    const Test &ref = Test(50); // 也会生成临时对象 , 引用时起别名, 可以和这个临时对象绑定在一起

    std::cout << "------------------" << std::endl;
    
    return 0;
}
```

* **运行结果图 : **

  [对象优化临时对象的优化问题](d:/截图/对象优化临时对象.png)

#### 1.1.1.4 类全面综合 

```c++
#include "00. head file.hpp"

/*
对象使用过程中背后调用了哪些方法
*/

#if 1
class Test {
    public: 
        Test(int a = 10 , int b = 20)
            :ma(a)
            , mb(b)
        {
            std::cout << "Test(int , int)" << std::endl;
        }
        ~Test() {
            std::cout << "~Test()" << std::endl;
        }

        Test(const Test &t) 
            : ma(t.ma)
            , mb(t.mb)
        {
            std::cout << "Test(const Test &t)" << std::endl;
        }
        void operator = (const Test &t) {
            std::cout << "operator = " << std::endl;
            ma = t.ma;
            mb = t.mb;
        }

    private: 
        int ma;
        int mb;
};
Test t1(10 , 10); // 第一个构造
int main() {
    Test t2(20 , 20);  // 第三个构造
    Test T3 = t2; // 第四个构造(拷贝构造)
    static Test t4 = Test(30 , 30); // 第五个构造 
    t2 = Test(40 , 40); // 第六个构造 , 显式生成临时对象 , 调用赋值函数 , 出了语句还要析构临时对象 
    // (50 , 50) == 50 , 逗号表达式 , 值时最后一个值
    t2 = (Test)(50 , 50); // 第七个构造 , 编译器先看类中有没有带一个整形参数的构造函数 , 有的话 , 生成临时对象 , 给 a 传 50 , b 用默认值 , 然后赋值函数 , 出了语句析构临时对象
    t2 = 60; //  第八个构造 , 隐式生成临时对象 构造 , 赋值 , 析构
    Test* p1 = new Test(70 , 70); // 第 9 个构造  Test(int , int) , delete 析构
    Test* p2 = new Test[2]; // 第十个构造 , 对象数组 , 调用两次带整型参数的构造函数 , delete 析构
    //Test* p3 = &Test(80 , 80); // 第十一个 : 不要用指针指向临时变量(因为临时对象没有持久的内存地址。尝试对临时对象取地址会导致编译错误) , 最坏情况 : 构造完出语句立即析构
    const Test &p4 = Test(90 , 90); // 第十二个 : Test(int , int) => 引用什么时候出作用域,临时对象什么时候析构 
    delete p1; // 第十三个: ~Test()
    delete[] p2; // 第十四个 : 调用两次析构函数 

    return 0;
}
Test t5(100 , 100); // 第二个构造

```

* **运行结果图 **

  [对象优化临时对象全面综合](d:/截图/对象优化临时对象全面综合.png)

#### 1.1.1.5 函数调用过程中的细节

##### 1.1.1.5.1 内容

```c++
#include "00. head file.hpp"

class Test {
    public : 
        Test(int data = 10) 
            : ma(data)
        {
            std::cout << "Test(int)" << std::endl;
        }
        ~Test() {
            std::cout << "~Test()" << std::endl;
        }
        Test(const Test &t) 
            : ma(t.ma) 
        {
            std::cout << "Test(const Test &t)" << std::endl;
        }
        void operator = (const Test &t) {
            std::cout << "operator = " << std::endl;
            ma = t.ma;
        }
        int getData() const {
            return ma;
        }
    private : 
        int ma;
};

Test GetObject(Test t) {  // 不能返回局部的或者临时对象的指针或者引用
    int val = t.getData();
    Test tmp(val); // 调用整形参数的构造
    //return tmp; // tmp 本身是出不来的 , 需要在 main 函数栈帧上构造一个临时对象 , 由 tmp 拷贝构造临时对象
    Test result(tmp);
    return result;
} // 这个函数结束有两个对象需要析构 : tmp , 形参 t
int main() {
    Test t1; // 1. 调用带整型参数的构造函数 
    Test t2; // 2. 调用带整型参数的构造函数 
    t2 = GetObject(t1); // 函数调用 , 实参传递给形参是初始化 , t 是一个正在定义的 Test 的对象 , 由 t1 拷贝构造 t
    return 0;
}
```

##### 1.1.1.5.2 问题分析 

* **首先 , 为什么 GetObject 函数不能返回指针或者引用类型?**

  **ans : `在 C++ 中，局部对象（即在函数内部定义的对象）会在函数结束时被销毁。如果你尝试返回一个指向局部对象的指针或引用，那么在函数返回后，这个局部对象已经被销毁，指针或引用将指向已经释放的内存，这会导致未定义行为。`**

  * ```c++
    Test* GetObject(Test t) {
        int val = t.getData();
        Test tmp(val);
        return &tmp;
    }
    // 这个示例 : tmp 是一个局部变量 , 当函数返回 &tmp 的时候,就会销毁 tmp , 返回的指针 &tmp 指向了一块被销毁的内存 , 会产生未定义的行为(悬空指针)
    ```

  * ```c++
    Test* GetObject(Test t) {
        int val = t.getData();
        static Test tmp(val);  // 加入 static 就不会被销毁了 ,因为这里已经初始化的static 定义的对象是在 .data 段 , 不会随着栈帧的开辟和回退被释放掉
        return &tmp;
    }
    ```

* **再一个, 这里的输出只有一个拷贝构造函数 , 是实参到形参的拷贝构造函数 , 那 tmp 是要在 main 的栈帧上通过拷贝构造函数生成一个临时对象的 , 这个 拷贝构造函数哪里去了 ?**

  **ans : `这是编译器进行了返回值优化或者说是命名返回值优化.从而避免了额外的拷贝构造,可以通过强制进行拷贝构造解决这个问题`**

  * ```c++
    Test GetObject(Test t) {
        int val = t.getData();
        Test tmp(val);
        Test result(tmp);
        return result; // 写成这样就可以看到拷贝构造函数了
    }
    ```

* **第三个问题 : 实参到形参的传递到底是 初始化 还是 赋值?**

  **ans : `是初始化`**

### 1.1.2 总结三条对象优化的规则

* **在进行函数参数传递的时候 , 尽量选择引用传递 , 不要使用值传递 , 因为引用传递不涉及新对象的产生 **
* **在函数返回一个对象的时候, 尽量返回一个对象 , 不要返回一个已经初始化的对象**
* **在接受函数返回值的时候 , 尽量用初始化的方式 , 不要用赋值重载**

#### 1.1.2.1 对象优化完整代码 

```c++
#include "00. head file.hpp"

class Test {
    public : 
        Test(int data = 10) 
            : ma(data)
        {
            std::cout << "Test(int)" << std::endl;
        }
        ~Test() {
            std::cout << "~Test()" << std::endl;
        }
        Test(const Test &t) 
            : ma(t.ma) 
        {
            std::cout << "Test(const Test &t)" << std::endl;
        }
        void operator = (const Test &t) {
            std::cout << "operator = " << std::endl;
            ma = t.ma;
        }
        int getData() const {
            return ma;
        }
    private : 
        int ma;
};

// 参数传递过程中,尽量使用引用传递,不要使用值传递
Test GetObject(Test &t) { 
    int val = t.getData();
    return Test(val); // 用临时对象拷贝构造一个新对象 , C++ 的优化是此时不产生临时对象 . 直接构造新对象
}

int main() {
    Test t1; 
    Test t2 = GetObject(t1); // 同样的用临时对象拷贝构造一个新对象 , 临时对象被优化 

    return 0;
}
/*

总结 : 三条对象优化规则 
1. 函数参数传递过程中,优先按引用传递,不用值传递
2. 在函数返回一个对象的时候 , 尽量直接返回一个临时对象, 而不要返回一个定义过的对象 , C++ 的优化在这时是会把临时对象优化掉 , 直接构造新对象
3. 接收返回值是对象的调用的时候,优先按初始化的方式接收,不要用赋值的方式接收
*/
```

* [总结三条对象优化规则](d:/截图/总结三条对象优化规则.png)

### 1.1.3 添加带右值引用的拷贝构造和赋值函数 

#### 1.1.3.1 回顾右值引用 

```c++
#include <isotream>

int main() {
    int a = 10;
    int &b = a; // 左值引用 
    //int &&c = a; // 右值引用 , 错误 
    // 右值没有名字(临时量)和内存
    //int &c = 20; // 不能用一个左值引用绑定到右值 
    // 解决办法 : const
    const int &c = 20;
    int &&d = 20; // 将右值绑定到一个右值引用上
    // int &&f = d; error
    int &f = d; // 一个右值引用变量本身是一个左值
    return 0;
}
```

#### 1.1.3.2 CMyString 问题 

**代码如下 : **

```c++
#include "00. head file.hpp"

class CMyString {
    public : 
        //CMyString() = default;
        CMyString(const char* str = nullptr) {
            std::cout << "CMyString(const char* str)" << std::endl;
            if(str != nullptr) {
                mptr = new char[strlen(str) + 1];
                strcpy(mptr , str); 
            }
            else {
                mptr = new char[1];
                *mptr = '\0';
            }
        }

        ~CMyString() {
            std::cout << "~CMyString()" << std::endl;
            delete[] mptr;
            mptr = nullptr;
        }

        // 带左值引用的拷贝构造
        CMyString(const CMyString &cms) {
            std::cout << "CMyString(const CMyString &cms)" << std::endl;
            mptr = new char[strlen(cms.mptr) + 1];
            strcpy(mptr , cms.mptr);
        }
        // 带左值引用的赋值重载函数
        CMyString& operator = (const CMyString &cms) {
            std::cout << "CMyString& operator = (const CMyString &cms)" << std::endl;
            if(this == &cms) {
                return *this;
            }
            delete[] mptr;
            mptr = new char[strlen(cms.mptr) + 1];
            strcpy(mptr , cms.mptr);
            return *this;
        }

        const char* c_str() const {
            return mptr;
        }
    private :
        char* mptr;
};

CMyString GetString(CMyString &str) {
    const char* pstr = str.c_str();
    CMyString tmpStr(pstr);
    CMyString result(tmpStr);
    return result;
}
int main() {

    CMyString str1("aaaaaaaaaaaaaaaa");
    CMyString str2;
    str2 = GetString(str1);
    std::cout << str2.c_str() << std::endl;
    return 0;
}
```

* 运行结果图 : 

  ![image-20260402164532103](C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20260402164532103.png)

* **产生的问题**

  [CMyString产生的问题](d:/截图/CMyString产生的问题.png)

#### 1.1.3.3 解决方案 : 添加带右值引用的构造函数和赋值函数 

```c++
// 带左值引用的拷贝构造
CMyString(const CMyString &cms) {
    std::cout << "CMyString(const CMyString &cms)" << std::endl;
    mptr = new char[strlen(cms.mptr) + 1];
    strcpy(mptr , cms.mptr);
}
// 带左值引用的赋值重载函数
CMyString& operator = (const CMyString &cms) {
    std::cout << "CMyString& operator = (const CMyString &cms)" << std::endl;
    if(this == &cms) {
        return *this;
    }
    delete[] mptr;
    mptr = new char[strlen(cms.mptr) + 1];
    strcpy(mptr , cms.mptr);
    return *this;
}

// 带右值引用的拷贝构造
CMyString(CMyString &&cms) {
    std::cout << "CMyString(const CMyString &cms)" << std::endl;
    mptr = cms.mptr;
    cms.mptr = nullptr; // 这句话一定要有 , 否则是浅拷贝, 两块指针指向同一块内存 , 释放会出问题
}
// 带右值引用的赋值重载函数
CMyString& operator = (CMyString &&cms) { 
    std::cout << "CMyString& operator = (const CMyString &cms)" << std::endl;
    if(this == &cms) {
        return *this;
    }
    delete[] mptr;
    mptr = cms.mptr;
    cms.mptr = nullptr;
    return *this;
}
```

* **这里 &&cms 本身引用的就是一个临时对象 , 也就是说临时对象是匹配到右值引用这里 , 直接转换资源的分配 , 不需要开辟内存然后再拷贝**
* ![image-20260402171425125](C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20260402171425125.png)

#### 1.1.3.4 CMyString 在 vector 上的应用

##### 1.1.3.4.1 原码

```c++
#include "00. head file.hpp"

class CMyString {
    public : 
        //CMyString() = default;
        CMyString(const char* str = nullptr) {
            std::cout << "CMyString(const char* str)" << std::endl;
            if(str != nullptr) {
                mptr = new char[strlen(str) + 1];
                strcpy(mptr , str); 
            }
            else {
                mptr = new char[1];
                *mptr = '\0';
            }
        }

        ~CMyString() {
            std::cout << "~CMyString()" << std::endl;
            delete[] mptr;
            mptr = nullptr;
        }

        // 带左值引用的拷贝构造
        CMyString(const CMyString &cms) { // 普通对象就是匹配到左值引用这里
            std::cout << "CMyString(const CMyString &cms)" << std::endl;
            mptr = new char[strlen(cms.mptr) + 1];
            strcpy(mptr , cms.mptr);
        }
        // 带右值引用的拷贝构造
        CMyString(CMyString &&cms) {  // cms 引用的就是一个临时对象 , 因为临时对象没名字,右值
            std::cout << "CMyString(CMyString &&cms)" << std::endl;
            mptr = cms.mptr;
            cms.mptr = nullptr;
        }

        // 带左值引用的赋值重载函数
        CMyString& operator = (const CMyString &cms) {
            std::cout << "CMyString& operator = (const CMyString &cms)" << std::endl;
            if(this == &cms) {
                return *this;
            }
            delete[] mptr;
            mptr = new char[strlen(cms.mptr) + 1];
            strcpy(mptr , cms.mptr);
            return *this;
        }
        // 带右值引用的赋值重载函数
        CMyString& operator = (CMyString &&cms) {
            std::cout << "CMyString& operator = (const CMyString &&cms)" << std::endl;
            if(this == &cms) {
                return *this;
            }
            delete[] mptr;
            mptr = cms.mptr;
            cms.mptr = nullptr;
            return *this;
        }

        const char* c_str() const {
            return mptr;
        }
    private :
        char* mptr;
        friend CMyString operator+(const CMyString &lhs , const CMyString &rhs);
        friend std::ostream& operator<<(std::ostream& out , const CMyString &rhs);
};      

CMyString operator+(const CMyString &lhs , const CMyString &rhs) {
    // char* ptmp = new char[strlen(lhs.mptr) + strlen(rhs.mptr) + 1];
    // strcpy(ptmp , lhs.mptr);
    // strcat(ptmp , rhs.mptr);
    // // return CMyString(ptmp); 每 new 一块内存 无法delete 造成内存泄漏 , 只能构造新对象再释放它
    // CMyString tmpStr(ptmp);
    // delete[] ptmp;
    // return tmpStr;  // 临时对象的问题
    CMyString tmpStr;
    tmpStr = new char[strlen(lhs.mptr) + strlen(rhs.mptr) + 1];
    strcpy(tmpStr.mptr , lhs.mptr);
    strcat(tmpStr.mptr , rhs.mptr);
    return tmpStr; 
}

std::ostream& operator<<(std::ostream &out , const CMyString &rhs) {
    out << rhs.mptr;
    return out;
}
int main() {

    CMyString str1 = "aaa";

    std::vector<CMyString> vec;
    vec.reserve(10);
    std::cout << "-------------------------" << std::endl;
    vec.push_back(str1);
    vec.push_back(CMyString("bbb"));

    std::cout << "-------------------------" << std::endl;


    /*
    CMyString str1 = "hello ";
    CMyString str2 = "world!";

    std::cout << "-------------------------" << std::endl;
    CMyString str3 = str1 + str2;

    std::cout << "-------------------------" << std::endl;

    std::cout << str3 << std::endl;

    */
    return 0;
}
```

##### 1.1.3.4.2 问题 

* **两个类相加的普通重载函数效率低下的原因 ?**

  **ans : `每一次 new 出来的对象无从释放导致效率低下`**

  * **解决方法 : 利用构造新对象 delete 析构它**

  * **这时代码变为了 : **

  * ```c++
    CMyString operator+(const CMyString &lhs , const CMyString &rhs) {
         char* ptmp = new char[strlen(lhs.mptr) + strlen(rhs.mptr) + 1];
         strcpy(ptmp , lhs.mptr);
         strcat(ptmp , rhs.mptr);
         CMyString tmpStr(ptmp);
         delete[] ptmp;
         return tmpStr;  // 临时对象的问题
    }
    ```

* **这时又回到了临时对象的内存利用率不好的问题上 , 直接创建一个新对象 , 在这个新对象上面 new  出来内存 , 直接在这个新对象上面进行操作避免了 delete 释放 , 又有效提高了内存利用率 **

  * ```c++
    CMyString operator+(const CMyString &lhs , const CMyString &rhs) {
        CMyString tmpStr;
        tmpStr = new char[strlen(lhs.mptr) + strlen(rhs.mptr) + 1];
        strcpy(tmpStr.mptr , lhs.mptr);
        strcat(tmpStr.mptr , rhs.mptr);
        return tmpStr; 
    }
    ```

### 1.1.4 std::move 和 std::forward

**代码**

```c++
#include "00. head file.hpp"

template<typename T>
class Allocator
{
    public :
        T *allocator(size_t size) { // 只负责内存开辟
            return (T*)malloc(sizeof(T) * size);
        }
        void dellocate(void *p) { // void * 可以指向任意类型
            free(p);
        }
        /*
        // 接收左值
        void construct(T *p , const T &val) {
            new (p) T(val); // 利用定位new，在指定的内存位置放上特定的值
        }
        // 接收右值
        void construct(T *p , T &&val) {
            new (p) T(std::move(val)); // 利用定位new，在指定的内存位置放上特定的值
        }
        */
        // 也可以优化 
        template<typename Ty>
        void construct(T *p , Ty &&val) { // Ty 只是给引用类型用的
            new (p) T(std::forward<Ty>(val)); // 利用定位new，在指定的内存位置放上特定的值
        }
        void destroy(T *p) {
            p -> ~T(); //T() 代表了 T 类型的析构函数
        }
};
template<typename T , typename Alloc = Allocator<T>>
class vector {
    public :
    vector(int size = 10 , const Alloc &alloc = Allocator<T>())
        	: _allocator(alloc)
        	//: _first(new T[size])
        	, _first(_allocator.allocator(size))
            , _last(_first)
            , _end(_first + size)
        {}
    ~vector() {
        // delete[] _first;
        // 要析构有效的内存
        for(T* p = _first ; p != _last ; p ++ )
        {
            _allocator.destroy(p);
        }
        _allocator.dellocate(_first);
        _first = _last = _end = nullptr;
    }
    vector(const vector<T> &rhc)  { // 拷贝函数
        int size = rhc._end - rhc._first;
        int len = rhc._last - rhc._first;
        // _first = new T[size]; 同样不能使用new ，因为初始化和开辟内存还是被同时做的
        _first = _allocator.allocator(size);
        _last = _first + len;
        _end = _first + size;
        for(int i = 0 ; i < len ; i ++ )
        {
            //_first[i] = rhc._first[i];	
            _allocator.construct(_first + i , rhc._first[i]);
        }
    }
    vector<T> &operator= (const vector<T> &rhc) {
        if(this == &rhc)
        {
            return *this;
        }
        // delete[] _first;
        for(T* p = _first ; p != _last ; p ++ )
        {
            _allocator.destroy(p);
        }
        _allocator.dellocate(_first);

        int size = rhc._end - rhc._first;
        int len = rhc._last - rhc._first;
        // _first = new T[size];
        _first = _allocator.allocator(size);
        _last = _first + len;
        _end = _first + size;
        for(int i = 0 ; i < len ; i ++ )
        {
            _first[i] = rhc._first[i]; 
        }
        return *this;
    }
    /*
    // 接收左值
    void push_back(const T &val) {
        if(full()) expand();
        // *_last ++ = val;  不对
        _allocator.construct(_last , val);
        _last ++;
    }
    // 接收右值 -- 一个右值引用变量本身是一个左值
    void push_back(T &&val) {
        if(full()) expand();
        _allocator.construct(_last , std::move(val));
        _last ++;
    }
    */
    template<typename Ty> // 函数模板的类型推演 + 引用折叠 代替上面两个方法
    void push_back(Ty &&val) { // Ty 是 CMyString&  &&val --> 引用折叠 --> 左值引用 + 右值引用 = 左值引用  右值 + 右值 = 右值引用
        if(full()) expand();
        // forward : 类型的完美转发 : 能够识别左值和右值类型
        // move : 移动语义 , 得到右值类型, 给move 里放一个左值 , 就会得到一个右值 , 是通过类型强转实现的
        _allocator.construct(_last , std::forward<Ty>(val));  // 这里的 val 本身还是个左值
        _last ++;
    }
    void pop_back() {
        if(empty()) return;
        // _last --; 不对
        -- _last;
        _allocator.destroy(_last); // 管理的是对象的析构
    }
    int back() const {
        return *(_last - 1);
    }
    bool full() const {
        return _last == _end;
    }
    bool empty() const {
        return _first == _last;
    }
    int size() const {
        return (_last - _first);
    }
    private : // 指向额外资源，浅拷贝是一定会出现问题的，要手写拷贝函数
    Alloc _allocator; // 配置器对象
    T *_first; // 指向数组的起始位置
    T *_last; // 指向数组有效元素的后继元素
    T *_end; // 指向数组的末尾位置
    
    void expand()
    {
        int size = _last - _first;
        // T *ptemp = new T[2 * size]; 不对
        T* ptemp = _allocator.allocator(2 * size);
        for(int i = 0 ; i < size ; i ++ )
        {
            // ptemp[i] = _first[i]; 不对
            _allocator.construct(ptemp + i , _first[i]);
        }
        // delete[] _first; 不对
        for(T * p = _first ; p != _last ; ++ p)
        {
            _allocator.destroy(p);
        }
        _allocator.dellocate(_first);
        _first = ptemp;
        _last = _first + size;
        _end = _first + 2 * size;
    }
};


class CMyString {
    public : 
        //CMyString() = default;
        CMyString(const char* str = nullptr) {
            std::cout << "CMyString(const char* str)" << std::endl;
            if(str != nullptr) {
                mptr = new char[strlen(str) + 1];
                strcpy(mptr , str); 
            }
            else {
                mptr = new char[1];
                *mptr = '\0';
            }
        }

        ~CMyString() {
            std::cout << "~CMyString()" << std::endl;
            delete[] mptr;
            mptr = nullptr;
        }

        // 带左值引用的拷贝构造
        CMyString(const CMyString &cms) { // 普通对象就是匹配到左值引用这里
            std::cout << "CMyString(const CMyString &cms)" << std::endl;
            mptr = new char[strlen(cms.mptr) + 1];
            strcpy(mptr , cms.mptr);
        }
        // 带右值引用的拷贝构造
        CMyString(CMyString &&cms) {  // cms 引用的就是一个临时对象 , 因为临时对象没名字,右值
            std::cout << "CMyString(CMyString &&cms)" << std::endl;
            mptr = cms.mptr;
            cms.mptr = nullptr;
        }

        // 带左值引用的赋值重载函数
        CMyString& operator = (const CMyString &cms) {
            std::cout << "CMyString& operator = (const CMyString &cms)" << std::endl;
            if(this == &cms) {
                return *this;
            }
            delete[] mptr;
            mptr = new char[strlen(cms.mptr) + 1];
            strcpy(mptr , cms.mptr);
            return *this;
        }
        // 带右值引用的赋值重载函数
        CMyString& operator = (CMyString &&cms) {
            std::cout << "CMyString& operator = (const CMyString &&cms)" << std::endl;
            if(this == &cms) {
                return *this;
            }
            delete[] mptr;
            mptr = cms.mptr;
            cms.mptr = nullptr;
            return *this;
        }

        const char* c_str() const {
            return mptr;
        }
    private :
        char* mptr;
        friend CMyString operator+(const CMyString &lhs , const CMyString &rhs);
        friend std::ostream& operator<<(std::ostream& out , const CMyString &rhs);
};      

CMyString operator+(const CMyString &lhs , const CMyString &rhs) {
    // char* ptmp = new char[strlen(lhs.mptr) + strlen(rhs.mptr) + 1];
    // strcpy(ptmp , lhs.mptr);
    // strcat(ptmp , rhs.mptr);
    // // return CMyString(ptmp); 每 new 一块内存 无法delete 造成内存泄漏 , 只能构造新对象再释放它
    // CMyString tmpStr(ptmp);
    // delete[] ptmp;
    // return tmpStr;  // 临时对象的问题
    CMyString tmpStr;
    tmpStr = new char[strlen(lhs.mptr) + strlen(rhs.mptr) + 1];
    strcpy(tmpStr.mptr , lhs.mptr);
    strcat(tmpStr.mptr , rhs.mptr);
    return tmpStr; 
}

std::ostream& operator<<(std::ostream &out , const CMyString &rhs) {
    out << rhs.mptr;
    return out;
}
int main()
{
    CMyString str1 = "aaa";

    vector<CMyString> vec;

    std::cout << "-------------------------" << std::endl;
    vec.push_back(str1); // CMyString&
    vec.push_back(CMyString("bbb")); // CMyString&& 

    std::cout << "-------------------------" << std::endl;

}
```



#### 1.1.4.1 std::move 

1. **用途**：
   - `std::move` 用于将左值转换为右值引用，从而允许资源的转移（移动）而不是复制。这通常用于提高性能，特别是在处理大型对象或资源密集型对象时。
2. **工作原理**：
   - `std::move` 本质上是将一个左值强制转换为右值引用，这样可以调用移动构造函数或移动赋值操作符。
   - 例如，`std::move(x)` 将 `x` 转换为 `T&&` 类型，其中 `T` 是 `x` 的类型。
3. **应用场景**：
   - 在需要临时对象的地方，使用 `std::move` 可以避免不必要的深拷贝。
   - 例如，在 `vector` 的 `push_back` 方法中，如果传递的是一个临时对象，可以通过 `std::move` 将其资源转移给 `vector` 中的新元素。

#### 1.1.4.2 std::forward 

1. **用途**：
   - `std::forward` 用于在模板函数中保持参数的原始类型（左值或右值），从而实现完美转发。这在编写通用代码时非常有用，可以确保参数的类型和值类别在传递过程中不被改变。
2. **工作原理**：
   - `std::forward` 接受一个万能引用（`T&&`）类型的参数，并根据参数的实际类型（左值或右值）进行转发。
   - 例如，`std::forward<Ty>(val)` 会根据 `Ty` 是否是左值引用或右值引用来决定 `val` 是左值还是右值。
3. **应用场景**：
   - 在模板函数中，特别是当需要将参数传递给其他函数时，使用 `std::forward` 可以确保参数的类型和值类别保持不变。
   - 例如，在 `Allocator` 的 `construct` 方法中，`std::forward<Ty>(val)` 确保 `val` 的类型和值类别在传递给 `new (p) T(std::forward<Ty>(val))` 时保持不变。

#### 1.1.4.3 代码中的应用 

##### 1.1.4.3.1 `std::move` 的应用

在 `vector` 的 `push_back` 方法中：

```
template<typename Ty>
void push_back(Ty &&val) {
    if(full()) expand();
    _allocator.construct(_last, std::forward<Ty>(val));
    _last++;
}
```

- 如果 `val` 是一个临时对象（右值），`std::forward<Ty>(val)` 会将其作为右值传递给 `construct` 方法，从而调用 `T` 的移动构造函数。
- 如果 `val` 是一个已命名的对象（左值），`std::forward<Ty>(val)` 会将其作为左值传递给 `construct` 方法，从而调用 `T` 的拷贝构造函数。

##### 1.1.4.3.2 `std::forward` 的应用

在 `Allocator` 的 `construct` 方法中：

```
template<typename Ty>
void construct(T *p, Ty &&val) {
    new (p) T(std::forward<Ty>(val)); // 利用定位 new，在指定的内存位置放上特定的值
}
```

- `std::forward<Ty>(val)` 确保 `val` 的类型和值类别在传递给 `new (p) T(std::forward<Ty>(val))` 时保持不变。
- 这样，如果 `val` 是一个右值引用，`T` 的移动构造函数会被调用；如果 `val` 是一个左值引用，`T` 的拷贝构造函数会被调用。

#### 1.1.4.4 总结

- **`std::move`**：用于将左值转换为右值引用，从而允许资源的转移（移动）而不是复制，提高性能。
- **`std::forward`**：用于在模板函数中保持参数的原始类型（左值或右值），实现完美转发，确保参数的类型和值类别在传递过程中不被改变。

通过合理使用 `std::move` 和 `std::forward`，可以编写出高效且通用的代码，特别是在处理资源管理类和容器类时。

## 1.2 智能指针

### 1.2.1 智能指针基础知识  

#### 1.2.1.1 裸指针 

**普通的指向堆内存的指针是需要手动释放管理内存的 **

```c++
#include <iostream>
    
int main() {
    int* p = new int(10);
    *p = 20;
    delete p; // 需要手动释放内存
    return 0;
}
```

#### 1.2.1.2 智能指针的逻辑 

**智能指针是通过模板来实现的 , 也是封装一个裸指针来进行操作 **

```c++
#include "00. head file.hpp" 

template<typename T> 
class CSmartPtr {
  	public : 
    	CSmartPtr(T* ptr = nullptr) 
            : mptr(ptr)
        {}
    	~CSmartPtr() {
            delete mptr;
        }
    	CSmartPrt& operator*() const { // 返回的是引用是因为要对内存进行修改
            return *mptr; 
        }
    	CSmartPtr* operator->() const { // 这里返回的是这个指针 
            return mptr;
        }
    
    private :
    	T* mptr;
};

int main() {
    CSmartPtr<int> ptr1(new int);
    CSmartPtr<int> ptr2(ptr1);
    *ptr2 = 20; // 需要重载 *
    
    class Test {
      	public :
        	void test() {
                std::cout << "call test()" << std::endl;
            }
    };
    CSmartPtr<Test> ptr3;
    ptr3->test(); // 需要重载 -> 
    return 0;
}
```

### 1.2.2 不带计数功能的智能指针 

**智能指针的浅拷贝问题 : 智能指针的浅拷贝问题是指当一个智能指针对象被复制时，如果只是简单地复制指针本身而不处理引用计数或所有权转移，会导致多个智能指针对象指向同一个内存地址。这会引起以下问题：**

* **双重删除**：当多个智能指针对象销毁时，它们会多次删除同一个内存地址，导致未定义行为。
* **悬挂指针**：如果其中一个智能指针对象先销毁并释放了内存，其他智能指针对象将变成悬挂指针，指向已经被释放的内存。

#### 1.2.2.1 auto_ptr 

**auto_ptr 是之前 c++ 库中的一个智能指针 , 能够自动管理资源**

**缺点 : **

*  **这个智能指针处理浅拷贝的方法只是通过最后一个这个类型的智能指针来管理内存,前面的智能指针都会被置为 nullptr**
* **不建议对容器使用这个智能指针 , 以 vector 为例 : std::vector<std::auto_ptr<int>> vec1; 如果用一个同样的类型来用vec1 对自己赋值, 如果涉及到了拷贝构造扩容赋值等等一系列的问题 , vec2 前面的指针都会被置为 nullptr , 访问空地址会出错**

```c++
#include "00. head file.hpp"

/*
不带引用计数的智能指针 
*/

template<typename T>
class CSmartPtr {

    public : 
        CSmartPtr(T* ptr = nullptr) 
            : mptr(ptr)
        {}
        ~CSmartPtr() {
            delete mptr;
        }

        CSmartPtr(const CSmartPtr<T> &src) {
            mptr = new T(*src.mptr);
        }
        T& operator*() const {  // 返回引用 , 改变指针指向内存
            return *mptr;
        }
        T* operator->() const {
            return mptr;
        }
    private :
        T* mptr;
};
int main() {
    // 不带引用计数的智能指针  : auto_ptr(c++ 库) ,  scoped_ptr(c++11) , unique_ptr(c++11)
    // 怎么解决浅拷贝问题
#if 0
    // CSmartPtr<int> p1(new int);
    // CSmartPtr<int> p2(p1);
    std::auto_ptr<int> ptr1(new int);
    std::auto_ptr<int> ptr2(ptr1);  // 处理浅拷贝的方法是永远让最后一个智能指针管理资源 , 前面的智能指针全部置为 nullptr
    *ptr2 = 20;
    std::cout << *ptr1 << std::endl;

    // 不推荐使用 auto_ptr , std::vector<std::auto_ptr<int>> vec1; vec2(vec1);  不建议 , 容器涉及拷贝构造和赋值操作 , 会将最后一个指针前面的裸指针置为 nullptr , 访问空内存报错
    	
    return 0;
}
```

#### 1.2.2.2 scoped_ptr 

**这个指针是存在于 c++11 新标准库里面的一个智能指针**

**问题 : **

* **这个智能指针处理浅拷贝的方法是对于它的拷贝构造函数和赋值操作全部置为 delete**

  ```c++
  std::scoped_ptr(const std::scoped_ptr<T> &src) = delete;
  std::scoped_ptr& operator=(const std::scoped_ptr<T> &src) = delete;
  ```

* **因此, 这个智能指针只适合用来管理一个对象,不适于管理多个对象**

#### 1.2.2.3 unique_ptr 

**这个指针是存在于 c++11 新标准库中的一个智能指针 **

**问题 : **

* **这个智能指针处理浅拷贝的方法也是对于它的拷贝构造函数和赋值操作全部置为 delete**

  ```c++
  std::unique_ptr(const std::unique_ptr<T> &src) = delete;
  std::unique_ptr& operator=(const std::unique_ptr<T> &src) = delete;
  ```

**优点 : **

* **但是它提供了右值引用的方法,对于管理内存更有优势 **

  ```c++
  std::unique_ptr(std::unique_ptr<T> &&src);
  std::unique_ptr& operator=(std::unique_ptr<T> &&src);
  ```

* ```c++
  //getSmartPtr 函数是一个返回 std::unique_ptr 的函数。这个函数的作用是创建一个新的 std::unique_ptr 对象，并将其返回给调用者。由于 std::unique_ptr 支持移动语义，返回的 std::unique_ptr 可以安全地转移其所有权给调用者。
  unique_ptr<T> getSmartPtr() {
       unique_ptr<T> ptr(new T());
       return ptr;
  }
  
  int main() {
      // 调用 getSmartPtr 并接收返回的 unique_ptr<int>
      std::unique_ptr<int> p = getSmartPtr<int>();
      *p = 42; // 修改指针指向的值
      std::cout << "p: " << *p << std::endl;
  
      // 调用 getSmartPtr 并接收返回的 unique_ptr<double>
      std::unique_ptr<double> q = getSmartPtr<double>();
      *q = 3.14; // 修改指针指向的值
      std::cout << "q: " << *q << std::endl;
  
      return 0;
  }
  ```

  

```c++
#include "00. head file.hpp"

/*
不带引用计数的智能指针 
*/

template<typename T>
class CSmartPtr {

    public : 
        CSmartPtr(T* ptr = nullptr) 
            : mptr(ptr)
        {}
        ~CSmartPtr() {
            delete mptr;
        }

        CSmartPtr(const CSmartPtr<T> &src) {
            mptr = new T(*src.mptr);
        }
        T& operator*() const {  // 返回引用 , 改变指针指向内存
            return *mptr;
        }
        T* operator->() const {
            return mptr;
        }
    private :
        T* mptr;
};
int main() {
    // 推荐 unique_ptr
    // unique_ptr(const unique_ptr<T>&) = delete;
    // unique_ptr<T>& operator = (const unique_ptr<T>&) = delete;
    // std::move => 右值引用 得到当前变量的右值类型
    std::unique_ptr<int> p1(new int);
    std::unique_ptr<int> p2(std::move(p1)); // 这个之所以能实现是因为提供了这样的函数 
    // unique_ptr(unique_ptr<T>&&)
    // unique_ptr<T>& operator = (unique_ptr<T>&&)
    
    return 0;
}
```

### 1.2.3 带计数功能的智能指针 

#### 1.2.3.1 计数功能 

**计数功能 : `智能指针通过引用计数机制来跟踪有多少个智能指针实例正在指向同一个对象。当最后一个指向该对象的智能指针被销毁或不再指向该对象时（即引用计数降为0），智能指针会自动释放所指向的对象，从而避免内存泄漏。`**

* 在C++中常见的几种智能指针类型包括`std::shared_ptr`、`std::unique_ptr`和`std::weak_ptr`。其中，`std::shared_ptr`就是利用了引用计数机制的一个例子。每当一个新的`std::shared_ptr`指向同一个对象时，这个对象的引用计数就会增加；相反地，当一个`std::shared_ptr`不再指向该对象时，引用计数就会减少。如果引用计数降到0，那么就表示没有`std::shared_ptr`再使用该对象了，此时系统会自动删除该对象以回收内存。

* 需要注意的是，并非所有的智能指针都使用引用计数机制。例如，`std::unique_ptr`并不维护引用计数，而是保证任何时候只有一个`std::unique_ptr`拥有对某个对象的所有权。这意味着`std::unique_ptr`不能被复制，但可以被移动（转移所有权）。

* 总结来说，智能指针中的“计数”主要指的是引用计数，在支持共享所有权的智能指针如`std::shared_ptr`中尤为重要，用来确保资源只有在没有任何智能指针引用它的时候才会被正确清理。

#### 1.2.3.2 shared_ptr 

**shared_ptr 是c++ 11 标准下带有计数功能的强指针, 它可以改变计数器的引用计数 , 因此它的用途在于 : **

* **自动析构 , 避免内存泄露 : 当计数器的值为 0 时 , 说明指针不再持有这个对象 , 会自动 delete 这个对象**
* **安全共享 : 多个`shared_ptr`可以指向同一个对象，通过计数明确记录当前有多少所有者，只有所有所有者都放弃所有权，对象才会被销毁，完美支持对象的共享语义**

```c++
#include "00. head file.hpp" 

class B;
class A {
    public:
        A() {
            std::cout << "A()" << std::endl;
        }
        ~A() {
            std::cout << "~A()" << std::endl;
        }

        void testA() {
            std::cout << "非常好用的方法" << std::endl;
        }
};

class B {
    public:
        B() {
            std::cout << "B()" << std::endl;
        }
        ~B() {
            std::cout << "~B()" << std::endl;
        }
        
};

int main() {
    std::shared_ptr<A> pa(new A());
    std::shared_ptr<B> pb(new B());
    
    return 0;
}
```

#### 1.2.3.3 weak_ptr 

**weak_ptr 也是 C++ 11 标准下带有计数器的弱智能指针 , 它与 shared_ptr 的区别在于 : weak_ptr 不会影响计数器的引用计数 , 因此它有以下作用 : **

* **解决因 shared_ptr 引发的循环引用问题 : `shared_ptr` 靠引用计数管理对象生命周期，只有计数归0才会释放内存，但如果两个堆上的类对象互相用 `shared_ptr` 成员指向对方，就会形成 **循环引用**：初始时双方计数因互指加到2，外部`shared_ptr`析构后计数只会降到1，永远无法归零，最终导致对象永远无法释放，造成内存泄漏。把任意一方的成员改为 `weak_ptr` 就能解决问题：`weak_ptr` 不会增加对象的引用计数，互指不会改变原计数，外部`shared_ptr`析构后计数可以正常归0，对象就能被正确释放，从根源上破解了循环引用的死锁。 **

* **安全观测对象，避免悬空指针 : 在很多场景下，我们需要持有对象的引用，但**不希望影响对象的生命周期**：** 

  - 比如缓存系统、观察者模式中，我们只需要"观察"对象是否还存活：对象该销毁时就让它销毁，我们不抢所有权。
  - `weak_ptr`可以通过`lock()`方法安全地获取有效的`shared_ptr`：如果原对象已经被释放，`lock()`会返回空指针，我们可以直接感知到对象不存在，避免访问已经销毁的野内存。

* **弱智能指针只能观测对象的示例 : **

  * ```c++
    #include "00. head file.hpp"
    
    /*
    shared_ptr : 强智能指针  : 可以改变资源的引用计数 
    weak_ptr : 弱智能指针 : 不会改变资源的引用计数 , 智能观察资源 , 不能使用资源
    weak_ptr => shared_ptr => 资源(内存)
    
    强智能指针循环引用(交叉引用)是什么问题? 造成什么结果? 怎么解决 ?
    造成 new 出来的资源无法释放 , 资源泄露问题 
    定义对象的时候 , 用强智能指针, 引用对象的地方, 用弱智能指针
    */
    
    class B;
    class A {
        public:
            A() {
                std::cout << "A()" << std::endl;
            }
            ~A() {
                std::cout << "~A()" << std::endl;
            }
            
            void testA() {
                std::cout << "非常好用的方法" << std::endl;
            }
            std::weak_ptr<B> _ptrb;
    };
    
    class B {
        public:
            B() {
                std::cout << "B()" << std::endl;
            }
            ~B() {
                std::cout << "~B()" << std::endl;
            }
            void func() {
                //_ptra->testA(); // 弱指针没有提供 * 和 -> 重载
                std::shared_ptr<A> ps = _ptra.lock(); // 提升方法 , 将弱指针提升到强指针
                if(ps != nullptr) { // 提升成功
                    ps->testA();
                }
            }
            std::weak_ptr<A> _ptra;
    };
    int main() {
        std::shared_ptr<A> pa(new A());
        std::shared_ptr<B> pb(new B());
    
        pa->_ptrb = pb;
        pb->_ptra = pa;
    
        std::cout << pa.use_count() << std::endl;
        std::cout << pb.use_count() << std::endl;
    
        pb->func();
    
        return 0;
    }
    ```

* **弱智能指针强转为强智能指针的示例 : – .lock()**

  * ```c++
    void func() {
        std::weak_ptr<int> wptr;
        std::shared_ptr<int> sptr = wptr.lock();
        if(sptr != nullptr) { // 这个原理是由于shared_ptr能够影响引用计数器的规则
            std::cout << "转换成功" << std::endl;
        }
    }
    ```

    

#### 1.2.3.4 提到的智能指针循环引用情况 

##### 1.2.3.4.1 什么是智能指针的循环引用情况

智能指针的循环引用，是 `shared_ptr` 使用中常见的内存泄漏场景，具体定义和发生过程如下：

当**两个或多个 `shared_ptr` 通过互相持有对方管理的堆对象**，形成一个闭合的引用环时，就会造成引用计数永远无法归零，最终导致对象永远无法释放，这就是智能指针的循环引用情况。

以两个类对象的场景为例，具体过程是：

1. 在堆上创建类A、类B对象，分别由外部的 `shared_ptr<A> ap` 和 `shared_ptr<B> bp` 管理，此时A、B的引用计数都为1
2. 让A的成员 `shared_ptr<B>` 指向B，B的成员 `shared_ptr<A>` 指向A，双方引用计数都会+1变为2
3. 函数结束后，外部的 `ap` 和 `bp` 析构，各自引用计数只-1变为1，永远无法降到0
4. 最终两个对象都不会触发析构，内存永久泄漏。

这种问题常出现在双向链表节点、树形结构的父子节点、观察者模式等需要双向引用的场景中。

##### 1.2.3.4.2 造成的后果 

**引用计数永远无法归零，最终导致对象永远无法释放**

##### 1.2.3.4.3 解决的方法 

**定义对象的地方 , 用强智能指针 , 引用对象的地方 , 用弱智能指针  **

```c++
#include "00. head file.hpp"

/*
shared_ptr : 强智能指针  : 可以改变资源的引用计数 
weak_ptr : 弱智能指针 : 不会改变资源的引用计数 , 智能观察资源 , 不能使用资源
weak_ptr => shared_ptr => 资源(内存)

强智能指针循环引用(交叉引用)是什么问题? 造成什么结果? 怎么解决 ?
造成 new 出来的资源无法释放 , 资源泄露问题 
定义对象的时候 , 用强智能指针, 引用对象的地方, 用弱智能指针
*/

class B;
class A {
    public:
        A() {
            std::cout << "A()" << std::endl;
        }
        ~A() {
            std::cout << "~A()" << std::endl;
        }
        std::weak_ptr<B> _ptrb;
};

class B {
    public:
        B() {
            std::cout << "B()" << std::endl;
        }
        ~B() {
            std::cout << "~B()" << std::endl;
        }
        std::weak_ptr<A> _ptra;
};
int main() {
    std::shared_ptr<A> pa(new A());
    std::shared_ptr<B> pb(new B());

    pa->_ptrb = pb;
    pb->_ptra = pa;

    std::cout << pa.use_count() << std::endl;
    std::cout << pb.use_count() << std::endl;

    pb->func();

    return 0;
}
```

### 1.2.4 多线程访问共享对象安全问题 

#### 1.2.4.1 多线程问题所在 

**把子线程 detached（分离）出去，主线程可能提前销毁共享对象，子线程如果再访问对象就会造成野指针崩溃**

#### 1.2.4.2 用到的特性 

1. **`weak_ptr`不增加引用计数，不会干扰原对象的生命周期**
2. **可以通过`lock()`方法安全探测对象是否还存活，如果存活就得到一个可用的`shared_ptr`访问，如果已经销毁则直接返回空指针。**

#### 1.2.4.3 代码核心重点 

1. **类A与类B的设计（解决循环引用的基础写法）**

   ```c++
   std::weak_ptr<B> _ptrb; // A类
   std::weak_ptr<A> _ptra; // B类
   ```

   如果两个类需要互相引用，用`shared_ptr`会造成循环引用内存泄漏，这里都改用`weak_ptr`是标准写法；同时`B::func()`中也演示了`weak_ptr`的标准用法：必须先调用`lock()`提升为`shared_ptr`，判断非空后才能安全访问对象。

2. **子线程函数的核心设计**

   ```c++
   void handler01(std::weak_ptr<A> q) {
       std::this_thread::sleep_for(std::chrono::seconds(2));
       std::shared_ptr<A> sq = q.lock();
       if(sq != nullptr) {
           sq->testA();
       } else {
           std::cout << "object is destructed , can't be accessed " << std::endl;
       }
   }
   ```

   - 参数传递用`std::weak_ptr<A>`而不是`shared_ptr<A>`：不会增加原对象的引用计数，不影响主线程中对象的销毁。
   - 必须用`q.lock()`判断对象存活：这就是这段设计的核心意义——**实现了"对象存活我才访问，对象已经销毁我就不操作"的安全逻辑**，彻底避免了野指针访问。

3. **main函数的场景模拟**

   ```c++
   std::shared_ptr<A> p(new A());
   std::thread t1(handler01 , std::weak_ptr<A>(p));
   t1.detach();
   std::this_thread::sleep_for(std::chrono::seconds(2));
   ```

   这里模拟了最危险的场景：主线程让`p`在局部作用域创建，休眠2秒等待子线程睡够，然后`p`出作用域被销毁，而子线程是分离的，还会继续执行。最终你会看到输出：

   > `~A()`
   > `object is destructed , can't be accessed`
   > 完美验证了`weak_ptr`的安全探测能力，不会访问已经销毁的对象。

------

##### 💡 整体意义

这个写法是C++多线程编程中非常优雅的 idiom（惯用法），它把对象生命周期的检查交给智能指针本身自动完成，不需要你自己手动写锁+标记来判断对象状态，既安全又简洁，非常适合处理这类共享对象的跨线程访问问题。

### 1.2.5 自定义删除器

**代码 : **

```c++
#include "00. head file.hpp"

/*
智能指针的自定义删除器
智能指针最大的用处 : 能够保证资源的绝对释放
*/
// unique_ptr , shared_ptr  都可以提供删除器

template<typename T>
class MyDeletor{

    public:
        void operator()(T* ptr) const {
            std::cout << "call MyDeletor.operator()" << std::endl;
            delete[] ptr;
        }
};
template<typename T>
class MyFileDeletor{

    public:
        void operator()(T* ptr) const {
            std::cout << "call MyFileDeletor.operator()" << std::endl;
            fclose(ptr);
        }
};
int main() {
    // std::unique_ptr<int , MyDeletor<int>> ptr1(new int[100]);
    // std::unique_ptr<FILE , MyFileDeletor<FILE>> ptr2(fopen("data.txt", "w"));

    // lambda 表达式  => 函数对象 function 

    std::unique_ptr<int , std::function<void (int *)>> ptr1(new int[100] , 
        [](int* p) -> void {
            std::cout << "call lambda release new int[100]" << std::endl;
            delete[] p;
    });

    std::unique_ptr<FILE , std::function<void (FILE*)>> ptr2(fopen("data.txt", "w"), 
        [](FILE* p) -> void {
            std::cout << "call lambda release fopen(\"data.txt\", \"w\")" << std::endl;
            fclose(p);
    });
    return 0;
}
```

#### 1.2.5.1 为什么要自定义删除器 

默认情况下，`std::unique_ptr`和`std::shared_ptr`默认调用`delete`来释放资源，这只能处理普通的`new`出来的单个对象。但当遇到以下场景时，默认释放逻辑就不适用了：

- 用`new[]`分配的数组（需要调用`delete[]`释放）
- 通过`fopen`打开的文件句柄（需要调用`fclose`释放）
- 申请的堆内存来自`malloc`（需要调用`free`释放）
- 自定义的内存池/共享资源，需要特殊的释放逻辑

自定义删除器就是让智能指针，在释放资源时调用你指定的释放逻辑，保证资源一定被正确回收，不发生泄漏。

#### 1.2.5.2 代码思路 

代码分别展示了两种常见的自定义删除器写法：

- 函数对象（仿函数）写法

  ```c++
  template<typename T>
  class MyDeletor{
      void operator()(T* ptr) const { delete[] ptr; }
  };
  ```

  将释放逻辑封装在类的 operator() 中，是最基础的写法，优点是模板化后可以适配不同类型。

- Lambda表达式写法（更常用）

  ```c++
  std::unique_ptr<int , std::function<void (int *)>> ptr1(new int[100] , 
      [](int* p) { delete[] p; });
  ```

  写法更简洁灵活，不需要额外定义一个类，适合一次性的自定义释放逻辑，实际开发中更常用。


#### 1.2.5.3 删除器意义

自定义删除器是智能指针"资源自动管理"能力的延伸，它把原本需要手动处理的特殊资源释放，交给智能指针自动完成，真正做到了**"资源申请即初始化，离开作用域自动释放"**，不管是正常流程还是异常退出，都能保证资源100%正确释放，从根本上避免了资源泄漏。

## 1.3bind 绑定器和 function 函数对象 

### 1.3.1 bind 绑定器 

#### 1.3.1.1 什么是绑定器  

**绑定器在底层也就是函数对象的引用 , 它将传入的参数第一个或者第二个形参绑定成确定的值, 便于操作例如 : find_if 按条件查找第一个小于 70 的值, 效率方便一些 **

#### 1.3.1.2 bind1st 和 bind2nd 

* **bind1st : 将 operator() , 第一个参数绑定成确定的值**
* **bind2nd : 将 operator() , 第二个参数绑定成确定的值**

#### 1.3.1.3 bind绑定器应用 

``` c++
#include "00. head file.hpp"

template<typename Container> 
void showContainer(Container &con) {
    typename Container::iterator it = con.begin();
    for( ; it != con.end() ; ++ it ) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}
int main() {
    
    std::vector<int> vec;
    srand(time(nullptr));
    
    for(int i = 0 ; i < 20 ; ++ i) {
        vec.push_back(rand() % 100 + 1);
    }
    
    showContainer(vec);
    
    sort(vec.begin() , vec.end()); // 默认从小到大排序 
    showContainer(vec);
    
    sort(vec.begin() , vec.end() , std::greater<int>()); // 改变为从大到小排序
    showContainer(vec);
    auto it = find_if(vec.begin() , vec.end() , std::bind1st(std::greater<int>() , 70));
    if(it != vec.end()) {
        vec.insert(it , 70);
    }
    showContainer(vec);
    
    return 0;
}
```

* **代码问题 : 为什么  typename Container::iterator it = con.begin(); 要加 typename**

  **ans : `因为编译器是从上到下进行编译的 , 编译到 Container::iterator it = con.begn(); 的时候 , 编译器不知道 Container:: 后面跟的对象是个变量还是个类型 , 如果是类型 , 那就好说 , 如果是变量, 变量怎么可能定义变量 ? 所以加个 typename告诉编译器这里是个类型 `**

#### 1.3.1.4 bind 绑定器的底层实现 

**在介绍 bind 绑定器的实现的时候先对 find_if 底层原理介绍 **

* **find_if 是按限制的查找 , 所以会传入三个参数 , 两个迭代器 , 一个比较函数 **

* ``` c++
  template<typename Iterator , typename Compare> // 编译器并不是在 定义 模板时知道它是函数的，而是在 调用（实例化） 时才去“对账”的。!!!!
  Iterator my_find_if(Iterator first , Iterator last , Compare comp) { 
      for( ; first != last ; ++ first ){
          if(comp(*first)) {
              return first;
          }
      }
      return last;
  }
  ```

**根据 find_if 的实现 , 来实现 bind1st 以及 bind2nd**

* **bind1st :**

  ``` c++
  template<typename Compare , typename T>
  class _mybind1st {
    	public : 
      	_mybind1st(Compare comp , T val) 
              : _comp(comp)
              , _val(val)
      	{}
      	bool operator()(Compare comp , const T &second) {
              return _comp(_val , second); // 绑定第一个 , 第二个传传入的参数 
          }
      
      private ; 
      	Compare _comp;
      	T _val;
  };
  
  template<typename Compare , typename T>
  _mybind1st<Compare , T> mybind1st(Compare comp , const T &val) {
      return _mybind1st<comp , T>(comp , val);
  }
  ```

* **bind2nd : **

  ``` c++
  template<typename Compare , typename T>
  class _mybind1st {
    	public : 
      	_mybind1st(Compare comp , T val) 
              : _comp(comp)
              , _val(val)
      	{}
      	bool operator()(Compare comp , const T &first) {
              return _comp(first , _val); // 绑定第二个 , 第一个传传入的参数 
          }
      
      private ; 
      	Compare _comp;
      	T _val;
  };
  
  template<typename Compare , typename T>
  _mybind1st<Compare , T> mybind1st(Compare comp , const T &val) {
      return _mybind1st<comp , T>(comp , val);
  }
  ```

#### 1.3.1.5 完整代码实现 

``` c++
#include "00. head file.hpp"

/*
C++ bind1st 和 bind2nd 底层实现原理 

*/

template<typename Container>
void showContainer(Container &con) {
    typename Container::iterator it = con.begin();
    for( ; it != con.end() ; ++ it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}
// my_find_if 实现原理 
template<typename Iterator , typename Compare>
Iterator my_find_if(Iterator first , Iterator last , Compare comp) {
    for( ; first != last ; ++ first) { 
        if(comp(*first)) {
            return first;
        }
    }
    return last;
} 

// 绑定器 

template<typename Compare , typename T> 
class _mybind1st {  // 绑定器是函数对象的一个应用
    public:
        _mybind1st(Compare comp , T val) 
            : _comp(comp) 
            , _val(val)
        {}
        bool operator()(const T &second) {
            return _comp(_val , second);
        }
    private:
        Compare _comp;
        T _val;
};
template<typename Compare , typename T>
_mybind1st<Compare , T> mybind1st(Compare comp , const T &val) {
    return _mybind1st<Compare , T>(comp , val);  // 直接使用函数模板好处是可以直接进行类型的推演 
}
int main() {

    std::vector<int> vec;
    srand(time(nullptr));

    for(int i = 0 ; i < 20 ; ++ i ) {
        vec.push_back(rand() % 100 + 1);
    }

    for(int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    sort(vec.begin() , vec.end()); // 默认小到大
    showContainer(vec);

    sort(vec.begin() , vec.end() , std::greater<int>()); // 大到小排序  
    showContainer(vec);

    // 把 70 按顺序插入到 vector 容器当中  , 找第一个小于 70 的数字 
    // operator(const T &val);
    // greater a > b
    // less    a < b
    // 绑定器 + 二元函数对象 =? 一元函数对象 
    // bind1st + greater : bool operator(70 , const _Ty &val);
    // bind2nd + less : bool operator(const _Ty &val , 70);
    auto it1 = my_find_if(vec.begin() , vec.end() , mybind1st(std::greater<int>() , 70));
    if(it1 != vec.end()) {
        vec.insert(it1 , 70);
    }
    showContainer(vec);

    return 0;
}
```

#### 1.3.1.6 总结 

##### 1 . `my_find_if`：行为的抽象化

它不关心容器的具体类型，也不关心具体的比较逻辑。它只定义了一个**动作流程**：

- 遍历区间 `[first, last)`。
- 对每个元素执行 `comp(*first)`。
- **契约**：只要你传进来的 `comp` 能接受一个参数并返回能转为 `bool` 的值，它就能跑。

##### 2. 函数对象（Functor）：逻辑的实体化

`std::greater<int>()` 本质上是一个**携带了运算逻辑的对象**。

- 在 C++ 中，逻辑不再仅仅是“代码段”（函数指针），而是可以作为参数传递的“对象”。
- 它通过重载 `operator()`，让对象表现得像函数一样。

##### 3. 绑定器（Binder）：维度的转换（核心难点）

这是你代码中最精彩的部分。绑定器的本质是**“降维打击”**：

- **矛盾点**：`std::greater` 需要两个参数（$a > b$），但 `my_find_if` 的循环逻辑只给一个参数（`*first`）。
- **解决方案**：`_mybind1st` 类作为一个“中转站”，通过构造函数**提前存下**一个参数（比如 `70`）。
- **转换结果**：它把一个“二元函数对象”包装成了一个“一元函数对象”，从而适配了 `my_find_if` 的接口。

------

##### 4. 关键技术点总结

| **知识点**     | **作用**                                   | **你的代码体现**                     |
| -------------- | ------------------------------------------ | ------------------------------------ |
| **`typename`** | 消除歧义，告诉编译器这是一个嵌套类型。     | `typename Container::iterator it`    |
| **模板推演**   | 让调用者不需要手动写 `<int, double>`。     | `mybind1st(std::greater<int>(), 70)` |
| **隐式接口**   | 只要行为符合契约（能调用），编译器就允许。 | `if(comp(*first))`                   |
| **匿名对象**   | 快速创建并传递逻辑实体。                   | `std::greater<int>()`                |

------

##### 5. 最终执行链路

当你运行最后那段代码时，底层的调用链是这样的：

1. **`my_find_if`** 拿出容器里的一个数（假设是 `60`）。
2. 调用 **`_mybind1st`** 的 `operator()(60)`。
3. **`_mybind1st`** 内部取出存好的 `70`，调用 **`std::greater<int>`** 的 `operator()(70, 60)`。
4. 结果为 `true` (因为 $70 > 60$)，`my_find_if` 成功找到位置并返回迭代器。

### 1.3.2 function 函数对象 

#### 1.3.2.1 函数对象的概念及使用场景

* **函数对象通常也被称为 `仿函数` , 核心 : `任何定义了 `operator()`（括号运算符）的类对象，都可以像函数一样被调用。`**

 **一、 基本概念**

普通的函数只是代码块，而函数对象是**类的一个实例**。

C++

```c++
#include <iostream>

class Sum {
public:
    // 重载括号运算符
    int operator()(int a, int b) const {
        return a + b;
    }
};

int main() {
    Sum add;             // 创建一个对象
    int result = add(3, 4); // 像调用函数一样使用对象
    std::cout << result;    // 输出 7
    return 0;
}
```

------

**二、 为什么需要函数对象？（核心优势）**

为什么不直接用普通函数呢？函数对象有三个普通函数无法企及的优势：

**1. 带有“记忆”（状态保存）**

普通函数要保存状态只能靠全局变量或 `static` 变量，这不安全且不可复用。函数对象可以用**成员变量**存储信息。

> **场景：** 实现一个累加器，记录它被调用了多少次。

**2. 类型安全**

每个函数对象都有自己的类类型。在模板编程中，编译器可以根据不同的函数对象类型生成不同的优化代码。

**3. 性能更高（内联优化）**

编译器在处理函数指针时，通常无法在编译期确定调用的具体函数，因此难以内联。而函数对象的调用在编译期是确定的，编译器可以直接把 `operator()` 内部的代码“嵌入”到调用处。

------

**三、 常见使用场景**

**1. STL 算法的策略（比较准则）**

这是函数对象最广泛的用途。例如 `std::sort` 或 `std::priority_queue` 需要知道如何比较大小。

C++

```c++
#include <vector>
#include <algorithm>
#include <functional> // 包含 std::greater 等内置仿函数

std::vector<int> vec = {1, 5, 2, 8};
// 使用内置的函数对象进行从大到小排序
std::sort(vec.begin(), vec.end(), std::greater<int>()); 
```

**2. 数据过滤与查找**

在 `std::find_if` 或 `std::remove_if` 中，我们需要一个“谓词”（Predicate）来判断条件。

C++

```c++
struct IsEven {
    bool operator()(int n) const { return n % 2 == 0; }
};
// 查找第一个偶数
auto it = std::find_if(vec.begin(), vec.end(), IsEven());
```

**3. 绑定器与回调函数**

正如之前写的 `mybind1st`，函数对象常用于将多参数函数“降维”封装，或者作为异步任务的回调。

------

**四、 现代 C++ 的演变：从仿函数到 Lambda**

在 C++11 之后，虽然底层的函数对象依然重要，但我们很少手动写一个 `class` 再写 `operator()` 了，取而代之的是 **Lambda 表达式**。

**本质上：Lambda 表达式就是编译器自动为你生成的临时函数对象类。**

| **特性**     | **普通函数**        | **函数对象 (仿函数)** | **Lambda 表达式**      |
| ------------ | ------------------- | --------------------- | ---------------------- |
| **状态**     | 无（除非全局/静态） | 成员变量存储          | 捕获列表 `[]` 存储     |
| **定义位置** | 全局或命名空间      | 类定义内              | 代码逻辑处（随用随写） |
| **性能**     | 一般                | 极高（易内联）        | 极高（易内联）         |

**总结**

**函数对象是“带数据的函数”**。它不仅能执行逻辑，还能随身携带执行逻辑所需的“上下文数据”。在 STL 算法、自定义排序、状态机实现等场景中，它是 C++ 高效且灵活的基石。

#### 1.3.2.2 函数对象的应用 

```c++
#include "00. head file.hpp"

void hello1() {
    std::cout << "hello world1!" << std::endl;
}
void hello2(std::string str) {
    std::cout << str << std::endl;
}
int sum(int a , int b) {
    return a + b;
}

class Test {
    public :
    	void hello(std::string str) {
            std::cout << str << std::endl;
        }
}
int main() {
    std::function<void()> func1 = hello1;
    func1();
    
    std::function<void(std::string)> func2 = hello2;
    func2("hello world2!");
    
    std::function<int(int , int)> func3 = sum;
    std::cout << func3(20 , 30) << std::endl;
    // 也可以写成这样 
    std::function<int(int , int)> func4 = [](int a , int b)->int { return a + b; }; // 函数对象的变式
    
    std::function<void(Test* , std::string)> func5 = &Test::hello;
    func5(&Test() , "call Test::hello()");
    return 0;
}
```

##### 1.3.2.2.1 代码中需要注意的问题

* **想要调用类的成员方法与调用普通的函数的方法可不一样 , 想要调用类的需要加上作用域 **
* **函数对象想对类的成员方法进行调用, 参数可不止有一个 , 每次在进行编译的时候 ,成员方法的参数列表第一位都会加上一个 this 指针 , 因此在写代码时一定要注意 **

#### 1.3.2.3 函数对象的综合应用 – 类似图书管理系统 

```c++
#include "00. head file.hpp"

void doShowAllBooks() {
    std::cout << "查看所有书籍信息" << std::endl;
}

void doBorrow() {
    std::cout << "借书" << std::endl;
}

void doBack() {
    std::cout << "还书" << std::endl;
}

void doQueryBooks() {
    std::cout << "查书" << std::endl;
}

void doLoginOut() {
    std::cout << "注销" << std::endl;
}
int main() {
    int choice = 0;
    std::map<int , std::function<void()>> actionMap;
    actionMap.insert({1 , doShowAllBooks});
    actionMap.insert({2 , doBorrow});
    actionMap.insert({3 , doBack});
    actionMap.insert({4 , doQueryBooks});
    actionMap.insert({5 , doLoginOut});

    for( ; ; ) { 
        std::cout << "-----------------" << std::endl;
        std::cout << "1. 查看所有书籍信息"  << std::endl;
        std::cout << "2. 借书"  << std::endl;
        std::cout << "3. 还书"  << std::endl;
        std::cout << "4. 查询书籍"  << std::endl;
        std::cout << "5. 注销"  << std::endl;
        std::cout << "-----------------"  << std::endl;
        std::cout << "请选择 : " << std::endl;

        

        auto it = actionMap.find(choice);
        if(it == actionMap.end()) {
            std::cout << "数字无效 , 重新选择" << std::endl;
        }
        else {
            it->second();
        }
    }

    
#if 0
        switch(choice) {  // 代码不好, 封装不好 , 不好维护 , 不满足软件设计的 "闭合原则"
            case 1: 
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            default:
                break;
        }
    
#endif

    return 0;
}
```

##### 1.3.2.3.1 问题

* **很多时候,想进行多次有目的性的选择 , 可能会直接想到 switch , 但是 , 有时逻辑混乱 , 丢掉break 等等问题 , 最重要的一点 : 这个设计是不满足软件设计的 `闭合原则` 的 , 也就是说 , 每次增加一个选项 , 都会改变一个甚至多个调用的顺序 , 不太好维护 , 因此可以利用 map + 函数对象的格式来替代 , 这样无论增加多少设计 , 都只需要调用 it->second() 即可**

#### 1.3.2.4 模板的完全实例化与部分实例化(特例化)

##### 1.3.2.4.1 概念 

* **模板的完全实例化就是专门为某一个类型用的模板就是完全实例化**

  ```c++
  template<>
  class myfunction<char*> {};
  // 就比如这个类就是给 char* 用的 , 模板进行实例化的时候调用的就是这个模板 
  ```

* **模板的部分实例化(特例化)就是说类型我只知道部分 : 就比如说我只知道我将要实例化的类型是个指针 , 但我不知道是什么指针这样的 **

  ```c++
  template<typename Ty>
  class myfunction<Ty*> {}; 
  // 这个就是可以给 int* , double* 等等用的 , 除非完全实例化了, 就比如 char* 在前面已经有了完全实例化了, 就不会用这个了 
  ```

##### 1.3.2.4.2 模板的拆分 

* **写一个模板  template<typename T> 这个 T 包含了很多 , 返回类型 , 参数列表类型 , 那要拆分出来该怎么做 ? **

  **ans : `多定义几个模板,分别表示对应的对象, 比如 : R 对应返回类型 , A1 对应第一个参数的类型等等 , 然后要在类的后面紧跟写上这个类的类型 `**

  ```c++
  template<typename R , typename A1 , typename A2>
  class Test<R(A1 , A2)> {};
  ```

##### 1.3.2.4.3 针对函数指针类型和函数类型的特例化的区别 

* **函数指针类型**

  ```c++
  template<typename R , typename A1 , typename A2>
  class Test<R(*)(A1 , A2)> {};
  ```

* **函数类型**

  ```c++
  template<typename R , typename A1 , typename A2>
  class Test<R(A1 , A2)> {};
  ```

在 C++ 模板编程中，理解**函数类型 (Function Type)** 和 **函数指针类型 (Function Pointer Type)** 的区别，是掌握高级模板元编程（如 `std::function` 或 `std::result_of` 的实现）的关键。

简单来说，它们的区别在于“实体”与“指向实体的地址”。

------

**1. 语法定义的区别**

首先看两者在类型定义上的直观不同：

| **类型**         | **示例语法**   | **描述**                                                     |
| ---------------- | -------------- | ------------------------------------------------------------ |
| **函数类型**     | `R(A1, A2)`    | 描述了函数的“签名”（返回值和参数列表）。它像是一个抽象的接口。 |
| **函数指针类型** | `R(*)(A1, A2)` | 描述了一个指向上述“签名”函数的**指针变量**。                 |

**代码示例：**

```c++
using FuncType = int(int, int);      // 函数类型
using FuncPtr  = int(*)(int, int);   // 函数指针类型

FuncType* p1 = sum; // 正确：函数指针指向函数类型
FuncPtr p2 = sum;   // 正确：直接定义指针
// FuncType f = sum; // 错误：不能直接声明一个函数类型的变量实体
```

------

**2. 模板特例化中的匹配逻辑**

当你定义 `Vector<T>` 并进行特例化时，编译器会进行**模式匹配**。

**A. 函数指针特例化：`Vector<R(*)(A1, A2)>`**

这个特例化专门匹配“指针”。

- **匹配目标**：`int(*)(int, int)`。
- **应用场景**：当你传递 `&sum` 或者直接传递函数名（在大多数上下文中自动隐式转换为指针）给一个需要指针的模板时。
- **特征**：匹配项中带有一个显式的 `*`。

**B. 函数类型特例化：`Vector<R(A1, A2)>`**

这个特例化匹配的是“签名本身”。

- **匹配目标**：`int(int, int)`。
- **应用场景**：这在模板元编程中极其实用（例如 `std::function<int(int, int)>`）。
- **注意点**：在 C++ 中，你**不能**创建函数类型的实例，但你**可以**传递函数类型作为模板的类型参数。

------

**3. 为什么要区分它们？（核心痛点）**

在实际开发中，区分这两者主要是为了**语义的优雅性**和**处理方式的不同**。

**场景一：仿 `std::function` 的设计**

如果你想设计一个容器，用户希望像这样使用：

```c++
Vector<int(int, int)> v;
```

此时，你必须提供 **函数类型** 的部分特例化。如果你只提供了函数指针的特例化，编译器会报错，因为它找不到匹配 `int(int, int)` 的版本。

**场景二：处理原始函数名**

当你把一个函数名 `sum` 传给 `template<typename T> void func(T a)` 时：

- 如果形参是 `T a`，`T` 会推导为函数指针 `int(*)(int, int)`。
- 如果形参是 `T& a`，`T` 会推导为函数类型 `int(int, int)`。

------

**4. 总结对比**

| **特性**         | **函数类型 R(A1, A2)**               | **函数指针类型 R(\*)(A1, A2)** |
| ---------------- | ------------------------------------ | ------------------------------ |
| **能否定义变量** | 不行                                 | 可以                           |
| **模板匹配对象** | `Vector<int(int, int)>`              | `Vector<int(*)(int, int)>`     |
| **常见用途**     | 模板元编程、定义接口签名             | 回调函数存储、动态绑定         |
| **编译器推导**   | 通常出现在引用 `T&` 或模板显式指定中 | 默认的退化 (Decay) 结果        |

##### 1.3.2.4.4 模板的实参推演

**编译器会自动根据传入的类型进行推导 , 这里着重介绍的是函数指针和函数类型的推导**

****

**代码示例 :**

```c++
#include "00. head file.hpp"

/*
模板的完全特例化和非完全(部分)特例化 
模板的实参推演 => 基本概念很简单 
*/

template<typename T> // T 包含了所有大的类型 , 返回值 , 想把所有形参类型都取出来怎么定义 ?
void func1(T a) {
    std::cout << typeid(T).name() << std::endl;
}
int sum(int a , int b) {
    return a + b;
}

template<typename R , typename A1 , typename A2> // T 包含了所有大的类型 , 返回值 , 想把所有形参类型都取出来怎么定义 ?
void func2(R(*a)(A1 , A2)) {
    std::cout << typeid(R).name() << std::endl;
    std::cout << typeid(A1).name() << std::endl;
    std::cout << typeid(A2).name() << std::endl;
}

class Test {
    public:
        int sum(int a , int b) {
            return a + b;
        }
};

template<typename R , typename T , typename A1 , typename A2>
void func3(R(T::*a)(A1 , A2)) {
    std::cout << typeid(R).name() << std::endl;
    std::cout << typeid(T).name() << std::endl;
    std::cout << typeid(A1).name() << std::endl;
    std::cout << typeid(A2).name() << std::endl;
}
int main() {
    //func1(10);
    //func1("aaa");
    func1(sum); // 不写成 T* 就是函数指针类型 , 写成 T* 推出来的就是函数类型 
    func2(sum);
    func1(&Test::sum);
    func3(&Test::sum);
    return 0;
}
```

**1. 整体匹配：隐式退化 (Decay)**

在 `func1(T a)` 中，编译器执行的是最宽松的推演：

- **传入：** `sum`（函数名）。
- **推演过程：** 在 C++ 中，函数名在作为值传递时，会自动退化（Decay）为**函数指针**。
- **结果：** `T` 被整体推导为 `int(*)(int, int)`。
- **局限：** 编译器把函数看作一个“黑盒”，它只知道 `T` 是一个指针，但不会主动帮你拆分出返回值和参数。

------

**2. 精确模式匹配：剥离内部结构**

当你定义 `func2(R(*a)(A1, A2))` 时，你是在给编译器下达一份“拆解指南”。

**推演逻辑：**

1. **匹配指针符号：** 编译器发现模板形参中有一个 `(*a)`，这告诉它：传入的必须是个指针。
2. **匹配返回类型：** 编译器查看指针指向函数的返回值，并将其与 `R` 绑定。
3. **匹配参数列表：** 编译器按顺序对比参数。第一个参数类型对应 `A1`，第二个对应 `A2`。

> **重点：** 如果你传入一个有 3 个参数的函数给 `func2`，推演就会**失败**。因为模式不匹配（篮子不够装或者装不下），这正是模板偏特化和精确匹配的严谨之处。

------

**3. 成员函数指针的特殊推演**

对于 `func3(R(T::*a)(A1, A2))`，推演难度增加了一个维度，因为**成员函数指针**不是简单的地址，它还包含所属类的信息。

- **`T` 的推演：** 编译器通过 `&Test::sum` 发现这个函数属于 `Test` 类，因此推导出 `T = Test`。
- **寻址方式：** 成员函数指针必须加上类域限定符 `T::*`。编译器必须在推演阶段确认这个类类型，否则无法确定该指针的位宽和调用约定。

------

**4. 函数类型 vs 函数指针的推演差异**

这是一个非常微妙的点，通过对比以下两个模板可以看到推演的威力：

| **模板定义**                        | **调用** | **T 的推演结果**   | **说明**                                                     |
| ----------------------------------- | -------- | ------------------ | ------------------------------------------------------------ |
| `template<typename T> void f(T a)`  | `f(sum)` | `int(*)(int, int)` | **函数指针**：函数名自动退化。                               |
| `template<typename T> void f(T* a)` | `f(sum)` | `int(int, int)`    | **函数类型**：因为形参已有 `*`，`T` 只匹配剩下的函数签名部分。 |

------

**5. 总结：推演的本质**

模板实参推演的本质是**逆向推导**：

1. 编译器观察你传入的具体实参类型（如 `int(*)(int, int)`）。
2. 编译器尝试寻找一组模板参数（`R, A1, A2`），使得将这些参数代入模板形参后，能完美还原出实参类型。
3. 如果能找到唯一解，推演成功；如果匹配不上（比如参数个数不对）或有歧义，则报错。

#### 1.3.2.5 function 的底层实现原理 

**依旧 , 通过类模板来实现底层原理 , 观看 STL 类库里的 function 模板 , 它们希望的是以函数的类型进行传入 **

```c++
// 参数列表只有一个的情况
template<typename Fty>
class myfunction {};

template<typename R , typename A1> // R 是 函数返回类型的模板 , A1 是传入参数类型的模板 
template myfunction<R(A1)> { // std::function<返回类型(参数列表类型)> 对照 myfunction<R(A1)>
    public : 
    	//typedef R(*PFUNC)(A1);
    	using PFUNC = R(*)(A1);  // 这个或者是 typedef 格式都可以 
    	myfunction(PFUNC pfunc)
            : _pfunc(pfunc)
       	{}
    	R opeartor()(A1 arg) {
            return _pfunc(arg);
        }
    
    private : 	
    	PFUNC _pfunc;
    	
};
// 参数列表只有两个的情况
template<typename Fty>
class myfunction {};

template<typename R , typename A1 , typename A2> 
class myfunction<R(A1 , A2)> {

    public : 
        //typedef R(*PFUNC)(A1);
        using PFUNC = R(*)(A1 , A2);
        myfunction(PFUNC pfunc) 
            :_pfunc(pfunc) 
        {}

        R operator()(A1 arg1 , A2 arg2) {
            return _pfunc(arg1 , arg2);
        }
    private :
        PFUNC _pfunc;

};
```

##### 1.3.2.4.1 主模板 + 偏特化的问题 

**代码 : **

```c++
template<typename Fty>
class myfunction() {};
```

**一、 核心意义：为什么要写那行“空”的主模板？**

如果你直接写 `template<typename R, typename A1> class myfunction`，你其实是在定义一个**接收两个独立参数**的模板。 而写成“主模板 + 偏特化”，意义在于以下三点：

1. **语法层面的“接口定义”**

主模板 `template<typename Fty> class myfunction {};` 规定了：

- **名称**：这个类叫 `myfunction`。
- **参数个数**：它只接收 **一个** 模板参数（`Fty`）。
- **它是“存根”**：它不负责具体实现，只负责在编译器里“挂个名”。

2. **实现模式匹配（类型解构）的核心**

这是最关键的。C++ 模板有一种神奇的能力叫 **模式匹配（Pattern Matching）**。 当你写 `class myfunction<R(A1)>` 时，你是在告诉编译器：

> “如果用户传进来的那个唯一参数 `Fty`，长得正好符合 `返回类型(参数类型)` 这种形状，就请执行这段代码，并顺便帮我把 `R` 和 `A1` 拆出来。”

**如果没有主模板，编译器根本不知道去哪里匹配这个模式。**

3. **强制类型检查（安全阀）**

主模板通常是空的。这意味着：

- 如果用户传了 `myfunction<int(double)>` → 匹配成功，执行特化代码。
- 如果用户传了 `myfunction<int>` → 匹配特化失败 → 退回到主模板 → **主模板是空的，没有构造函数，没有 `operator()`** → 编译立即报错。 这种设计能防止用户把非函数类型塞进你的 `myfunction` 里。

------

**二、 判定准则：什么时候【必须】写？**

**一句话法则：当你需要从“一个整体”里“抠出局部”的时候。**

1. **解构复合类型**

当你希望你的模板能自动识别并拆解函数、指针、数组等复合结构时。

- **例子**：你想知道一个函数指针的返回类型是什么。
- **做法**：
  1. 主模板：`template<typename T> struct Traits;`
  2. 偏特化：`template<typename R, typename A> struct Traits<R(*)(A)> { using ReturnType = R; };` 这样你传 `int(*)(double)` 进去，它就能自动把 `int` 抠出来。

2. **实现“语法糖”（漂亮写法）**

你想让用户像声明函数一样声明对象。

- **不写套娃**：`myfunction<int, double> f;` （看起来像个坐标对，不像函数）
- **写套娃**：`myfunction<int(double)> f;` （非常直观，一眼看出输入输出）

3. **类型萃取（Type Traits）**

当你需要根据类型的“属性”来执行不同逻辑时。比如 `std::is_pointer<T>`。

- **主模板**：默认所有类型都不是指针（`value = false`）。
- **偏特化**：针对 `T*` 这种模式，定义 `value = true`。

------

**三、 判定准则：什么时候【不需要】写？**

**一句话法则：当你把类型当成“一块砖”，搬到哪都一样的时候。**

1. **泛型容器**

比如 `std::vector<T>`。

- 无论 `T` 是 `int`、`string` 还是 `MyClass`，`vector` 的逻辑都是：申请内存 -> 拷贝进去 -> 销毁。
- 它不需要拆开 `T` 看里面有没有参数，也不需要管 `T` 是不是函数。

2. **简单的算法包装**

比如 `std::less<T>`。

- 它的逻辑永远是 `return a < b;`。它不需要知道 `T` 内部的细节。

3. **多参数但职责明确的模板**

比如 `std::map<K, V>`。

- 你已经明确要求用户传 Key 和 Value 了，两个参数各司其职，没有必要把它们揉成一个 `Pair<K, V>` 再去拆解。

------

**四、 深度总结表**

| 维度         | **“套娃”写法（主模板 + 偏特化）**                 | **常规写法（直接写实现）**     |
| ------------ | ------------------------------------------------- | ------------------------------ |
| **本质要求** | 需要**拆解**类型（解构）                          | 只需要**引用**类型（直用）     |
| **用户语法** | `ClassName<R(A)>` (漂亮、直观)                    | `ClassName<R, A>` (传统、直白) |
| **逻辑差异** | 不同模式走完全不同的代码路径                      | 所有类型共用一套逻辑（泛化）   |
| **典型代表** | `std::function`, `std::is_array`                  | `std::vector`, `std::list`     |
| **你的代码** | **必须写**（因为你要从 `Fty` 里抠出 `R` 和 `A1`） | **不适用**                     |

**最终结论**

之所以写那行空的主模板，是因为正在写一个 **“类型解码器”**。

- **主模板**是解码器的**外壳**，它定义了“插口”的大小。
- **偏特化**是里面的**电路**，它负责把插进来的复杂信号（函数类型）拆解成二进制码（R 和 A1）。

如果没有外壳，电路就没法工作；如果不拆解信号，你就拿不到 `R` 和 `A1` 去定义你的 `PFUNC` 指针。

****

##### 1.3.2.4.2 定义函数指针问题

```c++
typedef R(*PFUNC)(A1);
using PFUNC = R(*)(A1);
```

 **这两行代码的作用是相同的：它们都在`定义一个“函数指针”的类型别名。`**

简单来说，它们是给“指向某种特定函数的指针”起一个好记的名字。在 `myfunction` 实现中，这是为了保存那个真正执行任务的函数地址。

------

**1. 语法拆解：它们在定义什么？**

无论是 `typedef` 还是 `using`，它们都在定义一个名为 `PFUNC` 的类型。这个类型代表：**一个指向“返回值为 R，参数为 A1, A2”的函数的指针**。

**方式 A：传统的 `typedef` (C 语言风格)**

C++

```cpp
typedef R (*PFUNC)(A1, A2);
```

- **语法槽点**：它的定义方式比较反人类，要把新名字 `PFUNC` 埋在中间。
- **理解方法**：去掉 `typedef`，它就是一个函数指针变量的声明；加上 `typedef`，`PFUNC` 就变成了一个类型名。

**方式 B：现代的 `using` (C++11 别名语法)**

C++

```cpp
using PFUNC = R(*)(A1, A2);
```

- **语法优势**：非常直观，遵循 `名字 = 定义` 的逻辑，读起来像“PFUNC 是一个指向返回 R、参数 A1, A2 函数的指针”。
- **推荐理由**：它是类型别名（Type Alias），支持模板别名，比 `typedef` 更强大，语义更清晰。

------

**2. 为什么要写这两玩意？（核心意义）**

在 `myfunction` 模板类里，它们的意义在于：**抽象化与存储**。

**A. 为了定义成员变量**

需要在类里开辟一块空间，用来记住用户传进来的那个函数地址。有了这个别名，才能定义私有成员：

C++

```cpp
private:
    PFUNC _pfunc; // 这里 _pfunc 就像一个“遥控器”，指向真正的函数内存
```

**B. 为了提高可读性**

如果没有这两行，构造函数和成员变量会写成这样（极其难看且易出错）：

C++

```cpp
// 不用别名的恐怖写法：
myfunction(R(*pfunc)(A1, A2)) : _pfunc(pfunc) {}
R(*_pfunc)(A1, A2); 
```

------

**3. 它们在 `myfunction` 里的完整链路**

1. **定义别名**：通过 `using PFUNC = ...` 确定了要存什么样的函数。
2. **构造函数接收**：`myfunction(PFUNC pfunc)` 接收一个真实的函数地址。
3. **内部存储**：把地址交给 `_pfunc` 存起来。
4. **最终调用**：当执行 `f(arg1, arg2)` 时，实际上是执行了 `return _pfunc(arg1, arg2);`。

------

**总结：区别与选择**

| **特性**     | **typedef**            | **using (别名)**     |
| ------------ | ---------------------- | -------------------- |
| **可读性**   | 名字在中间，像“迷宫”   | 名字在左边，像“等式” |
| **C++ 标准** | C 语言时代就有的老古董 | C++11 引入的现代语法 |
| **功能性**   | 只能定义基本别名       | 支持模板别名（更强） |

**结论：** 它们就是为了给**函数指针**穿件漂亮、简洁的“马甲”，方便在类里面反复使用。在现代 C++ 开发中，**强烈建议使用 `using`**。

##### 1.3.2.4.3 解决参数列表过多的问题 

**c++ 为了解决这种情况 , 在里面模板里提供了一个非常强大的魔法 : 可变参 `XXX . . .`**

```cpp
template<typename Fty>
class myfunction {};

template<typename R , typename... A>
class myfunction<R(A ...)> {
    public:
    	using PFUNC = R(*)(A...);
    	myfunction(PFUNC pfunc) 
			: _pfunc(pfunc)
        {}
    	R operator()(A... arg) {
            return _pfunc(arg...);
        }
    private:
    	PFUNC _pfunc;
};
```

在 C++ 变长参数模板（Variadic Templates）中，省略号 `...` 的位置极其关键。放错位置不仅会导致编译失败，还会改变代码的语义。

你可以通过 **“打包”** 和 **“解包”** 这两个概念来记忆它的位置：

------

**1. 省略号位置的黄金法则**

| **语法位置**        | **作用**            | **记忆口诀**                                              | **你的代码示例**            |
| ------------------- | ------------------- | --------------------------------------------------------- | --------------------------- |
| **`typename... A`** | **声明**参数包      | **`...` 在名字左边**：把一堆类型“打包”成一个名字 A。      | `template<typename... A>`   |
| **`A...`**          | **引用/展开**参数包 | **`...` 在名字右边**：把打包好的 A “拆开”成多个独立的项。 | `class myfunction<R(A...)>` |
| **`arg...`**        | **传递**参数值      | **`...` 在变量名右边**：把收到的所有实参一个个传给函数。  | `_pfunc(arg...)`            |

------

**2. 详细位置拆解**

**A. 模板声明阶段：打包 (Packing)**

在定义模板时，`...` 紧跟在 `typename` 或 `class` 后面。

C++

```c++
template<typename... A> // A 现在是一个“类型包”，包含了 0 到任意多个类型
```

**B. 偏特化阶段：限定模式**

在 `myfunction<R(A...)>` 中，`...` 放在 `A` 的后面。

- 这表示我们要匹配的是一种**函数签名格式**。
- 注意：`R(A...)` 整体代表一个函数类型，`...` 告诉编译器这里可以有多个参数类型。

**C. 函数参数列表：声明变量包**

在构造函数或 `operator()` 的参数定义中：

C++

```c++
R operator()(A... arg) // 正确：A 是类型包，arg 是对应的变量包
```

- **注意顺序**：必须是 `类型名... 变量名`。这里 `A...` 表示展开类型包，`arg` 则是这些类型对应的具体参数包。

**D. 函数调用阶段：展开 (Unpacking/Expanding)**

在 `return _pfunc(arg...);` 中：

- `...` 必须放在变量名 `arg` 的**直接右边**。
- 这告诉编译器：把 `arg` 包里的数据按顺序一个个摆开，填进 `_pfunc` 的参数位里。

------

**3. 特别注意：容易写错的地方**

**错误 1：`...` 放错侧**

- ❌ `R operator()(...A arg)` —— 错误：`...` 应该在类型名后面。
- ✅ `R operator()(A... arg)` —— 正确。

**错误 2：展开位置不匹配**

- ❌ `return _pfunc(...arg);` —— 错误。
- ✅ `return _pfunc(arg...);` —— 正确。

**错误 3：空格的影响**

虽然 C++ 对空格不敏感（`A...` 和 `A ...` 是一样的），但习惯上：

- 声明时连写：`typename... Args`

- 展开时连写：`Args...`

  这样读起来更像是一个整体。

------

**4. 为什么 `...` 这么神奇？**

你可以把 `...` 想象成一个**“逗号生成器”**。

如果用户定义了 `myfunction<int(double, char)>`：

1. `A...` 就变成了 `double, char`（类型列表）。
2. `A... arg` 就变成了 `double arg1, char arg2`（形参列表）。
3. `arg...` 就变成了 `arg1, arg2`（实参列表）。

总结建议

- **声明包**（在 `template` 里）：`...` 在左。
- **用包**（在函数参数或调用里）：`...` 在右。
- **形参声明**：`类型... 变量名`。

唯一需要警惕的是在更复杂的表达式里，比如 `forward<A>(arg)...`，此时 `...` 放在整个表达式的最右边，表示对包里的每一个元素都执行 `forward` 操作。​​

#### 1.3.2.6 bind 绑定器与 function 的多线程问题 

**代码**

```c++
#include "00. head file.hpp"

/*
c++11 bind 绑定器 => 返回的结果还是一个函数对象 
*/
/*

void hello(std::string str) {
    std::cout << str << std::endl;
}
int sum(int a , int b) {
    return a + b;
}
class Test {
    public: 
        int sum(int a , int b) {
            return a + b;
        }
};
int main() {
    // bind 是函数模板, 可以自动推演模板类型参数 
    std::bind(hello , "hello bind!1")();
    std::cout << std::bind(sum , 10 , 20)() << std::endl;

    std::cout << std::bind(&Test::sum , Test() , 10 , 20)() << std::endl;

    // 参数占位符  -- 必须传入参数  绑定器出了语句 , 无法继续使用
    std::bind(hello , std::placeholders::_1)("hello bind!2");
    std::cout << std::bind(sum , std::placeholders::_1 , std::placeholders::_2)(10 , 20) << std::endl;
    
    // 此处就把 bind 返回的绑定器 binder 就复用起来了 
    std::function<void(std::string)> func1 = std::bind(hello , std::placeholders::_1);
    func1("hello China");
    func1("hello sichuan");
    func1("hello chongqing");
    return 0;
}
*/

/*
muduo 源码文件 threadpool.cc thread.cc 实现线程池  bind 和 function
*/
// 线程类
class Thread {
    public:
        Thread(std::function<void()> func) 
            : _func(func) 
        {}
        std::thread start() {
            std::thread t(_func);
            return t;
        }
    private:
        std::function<void()> _func;
};
// 线程池类 
class ThreadPool {
    public:
        ThreadPool() {}
        ~ThreadPool() {
            // 释放 Thread 对象占用的堆资源 
            for(int i = 0 ; i < _pool.size() ; ++ i) {
                delete _pool[i];
            }
        }
        // 开启线程池
        void startPool(int size) {
            for(int i = 0 ; i < size ; ++ i ) {
                _pool.push_back(new Thread(std::bind(&ThreadPool::runInThread , this , i)));
            }

            for(int i = 0 ; i < size ; ++ i) {
                _handler.push_back(_pool[i]->start());
            }
            for(std::thread &t : _handler) { // 等待所有子线程完成 , 主线程再结束
                t.join();
            }
        }
    private:
        std::vector<Thread*> _pool; 
        std::vector<std::thread> _handler;
        // 把 runInThread 这个成员方法充当线程函数 
        void runInThread(int id) {
            std::cout << "call runInThread! id : " << id << std::endl; 
        }
};

int main() {
    ThreadPool pool1;
    pool1.startPool(10);
    return 0;
}
```

##### 1.3.2.6.1 bind 

**首先 , bind 是一个函数模板 , 可以自动推导模板参数类型 , 其次这里的 bind 是 c++ 11 新引入的绑定器 , 它不同于 bind1st 和 bind2nd  , 区别如下 : **

**1. bind1st 和 bind2nd (C++98/03)**

这两个函数被称为**函数适配器**，它们的作用非常单一：将一个**二元函数**（接收两个参数）的一个参数绑定为固定值，从而转变为**一元函数**。

- **bind1st**: 绑定二元函数的**第一个**参数。
- **bind2nd**: 绑定二元函数的**第二个**参数。

**局限性：**

1. **只能处理二元函数**：无法处理一元、三元或更多参数的函数。
2. **类型要求严苛**：被绑定的函数对象必须继承自 `std::binary_function`，否则编译器通常会报错。
3. **语法晦涩**：在复杂的组合中可读性极差。

> **注意**：它们在 C++11 中被弃用（deprecated），并在 **C++17 中被正式移除**。

------

**2. std::bind (C++11 及以后)**

`std::bind` 是一个更加通用且强大的模板函数，它彻底取代了前两者。

**核心优势：**

- **参数数量不受限**：可以绑定任意数量参数的函数。

- **占位符机制 (`std::placeholders`)**：通过 `_1, _2, _3` 等占位符，可以灵活决定调用时参数的传递顺序。

  ```c++
  int sum(int a , int b) {
      return a + b;
  }
  int main() {
      std::cout << std::bind(sum , std::placeholders::_1 , std::placeholders::_2)(10 , 20) << std::endl;
      return 0;
  }
  ```

  

- **无需继承**：不需要函数对象继承任何特定基类。

------

**3. 核心对比表**

| **特性**     | **bind1st / bind2nd**         | **std::bind**                |
| ------------ | ----------------------------- | ---------------------------- |
| **标准版本** | C++98 (已移除)                | C++11 及以后                 |
| **适用函数** | 仅限二元函数                  | 任意数量参数的函数           |
| **灵活性**   | 只能固定左/右一个参数         | 可任意固定参数、交换参数顺序 |
| **类型限制** | 必须满足特定的 `typedef` 要求 | 无特殊要求                   |
| **推荐程度** | **禁止使用** (过时)           | **推荐使用** (或使用 Lambda) |

------

**4. 代码示例对比**

假设我们有一个比较函数 `less(a, b)`，我们想检查一个数字是否 `10 < x`。

**使用旧版 (bind1st):**

```c++
// 绑定第一个参数为 10，逻辑变为：10 < x
std::find_if(v.begin(), v.end(), std::bind1st(std::less<int>(), 10));
```

**使用新版 (std::bind):**

```c++
using namespace std::placeholders;
// _1 代表调用时传入的第一个参数
auto fn = std::bind(std::less<int>(), 10, _1); 
fn(20); // 相当于 less(10, 20)
```

------

💡 **现在的最佳实践**

虽然 `std::bind` 比 `bind1st/2nd` 强很多，但在现代 C++（C++11 及以后）中，**Lambda 表达式**通常是比 `std::bind` 更好的选择：

```c++
// 使用 Lambda 代替 bind，更直观，编译器优化更好
std::find_if(v.begin(), v.end(), [](int x) { return 10 < x; });
```

##### 1.3.2.6.2 多线程代码实现思路

**1. 核心思路：解耦与回调**

在 C++ 线程库中，`std::thread` 需要的是一个可调用对象（Callable）。然而，我们的业务逻辑通常写在类的**成员函数**（如 `runInThread`）里。

- **挑战**：成员函数隐含一个 `this` 指针，不能直接传给 `std::function<void()>`。
- **方案**：利用 **`std::bind`** 将成员函数的指针、所属对象的地址（`this`）以及参数（`i`）“绑定”在一起，伪装成一个不带参数的函数。

------

**2. 重点步骤标记 (核心逻辑)**

在你编写代码时，需要重点关注以下三个环节：

**A. 参数绑定 (The Binding)**

在 `startPool` 中，这行代码是灵魂： `std::bind(&ThreadPool::runInThread, this, i)`

- **`&ThreadPool::runInThread`**: 告诉 bind 你要调用哪个成员函数。
- **`this`**: 关键点。成员函数依赖对象实例，必须把当前的 `ThreadPool` 对象地址传进去。
- **`i`**: 预先绑定参数。这样 `runInThread(int id)` 在执行时，`id` 就已经被固定为循环变量 `i` 的值了。

**为什么不能写成这样呢 ?  `_pool.push_back(new Thread(std::bind(&ThreadPool::runInThread , ThreadPool() , i)))`**

**1. 致命伤：生命周期（对象瞬间消失）**

`ThreadPool()` 创建的是一个**临时对象（右值）**。

- 这行代码执行时：`std::bind` 确实拷贝了一个 `ThreadPool` 的副本存到了自己的“包裹”里。
- **但是**：`ThreadPool` 类通常包含 `vector`、内存资源或者其他复杂的成员。更重要的是，在你的设计中，`ThreadPool` 应该是一个**管理者**。
- **后果**：当你真正启动线程（调用 `start()`）时，那个由 `ThreadPool()` 产生的临时副本可能已经因为作用域结束或其他原因处于不可控状态。

**2. 逻辑伤：你在操作“另一个”线程池**

这是最核心的问题。想象一下：

- 你有一个**真正的线程池 A**（你在里面调用 `startPool` 的那个对象）。
- 当你写 `ThreadPool()` 时，你创建了一个**全新的、临时的线程池 B**。

**结果就是：**

- 你原本想让“老板 A”指挥员工干活。
- 结果你临时招了个“路人 B”，把路人 B 的联系方式给了员工，然后路人 B 转身就消失了。
- 子线程启动后，去寻找“路人 B”留下的地址（`runInThread`），此时由于路人 B 已经销毁，子线程会访问非法内存，直接报错。

------

**3. 为什么必须用 `this`？**

在你的代码语境下，`this` 代表的是**当前正在运行、已经分配好内存、且正在管理这些线程的那个唯一真实的 ThreadPool 对象**。

| **比较维度** | **使用 this**                        | **使用 ThreadPool()**                        |
| ------------ | ------------------------------------ | -------------------------------------------- |
| **身份**     | 指向当前对象（真正的管理者）         | 创建一个全新的“路人”对象                     |
| **数据共享** | 所有线程访问同一个 `_pool` 和变量    | 每个线程访问自己私有的临时副本（如果有的话） |
| **稳定性**   | 只要 `ThreadPool` 不析构，地址就有效 | 临时对象生命周期极短，极易导致**野指针**     |
| **逻辑意图** | “我想让**我**的函数在线程里跑”       | “我想让**随便一个新池子**的函数在线程里跑”   |

------

**4. 总结**

在 C++ 开发中，尤其是线程和回调函数场景：

1. **如果你想让当前对象干活**，永远使用 `this`。
2. **只有当你确实需要一个全新的、独立的副本**，且这个副本不需要和当前对象共享任何状态时，才考虑传对象（但通常也会用 `*this` 拷贝一份，而不是新造一个 `ThreadPool()`）。

**一句话警示：**

如果你写了 `ThreadPool()`，你其实是在每个线程里都塞了一个“分身”，而这些“分身”在出生的一瞬间就死掉了。

****

**B. 类型擦除 (Type Erasure)**

`Thread` 类的构造函数接收的是 `std::function<void()>`。

- 不管你绑定的函数原本有几个参数，只要经过 `std::bind` 包装后变成了“无参调用”，它就能存入 `Thread` 类中。这使得 `Thread` 类变得非常通用，它不需要知道自己运行的是哪个类的哪个函数。

**C. 生命周期管理**

- **`_pool`**: 存储 `Thread` 对象的指针（管理业务逻辑包装层）。
- **`_handler`**: 存储真正的 `std::thread` 对象（管理底层系统线程）。
- **`join()`**: 确保主线程不会在子线程干完活之前退出，防止程序崩溃（产生孤儿线程或访问已销毁的资源）。

------

**3. 代码实现流程图**

1. **初始化**：创建 `ThreadPool` 对象。
2. **创建任务**：在 `startPool` 中，循环创建 `Thread` 对象，并使用 `std::bind` 封送 `runInThread`。
3. **启动线程**：遍历 `_pool`，调用 `start()` 启动 `std::thread`，并将线程句柄存入 `_handler`。
4. **阻塞等待**：主线程通过 `join()` 等待所有线程执行完毕。
5. **销毁资源**：析构函数释放 `_pool` 中的堆内存。

------

**4. 优化建议 (进阶点)**

如果你想让代码更贴近真实的 Muduo 或者更现代：

- **使用智能指针**：建议将 `vector<Thread*>` 改为 `vector<std::unique_ptr<Thread>>`，这样就不需要手动写循环 `delete`，避免内存泄漏。

- **禁止拷贝**：线程池这种资源管理类，通常应该继承 `noncopyable` 或删除拷贝构造函数。

- **Lambda 替代 bind**：在 C++11 后，很多开发者更倾向于这样写，更简洁：

  C++

  ```c++
  _pool.push_back(new Thread([this, i](){ runInThread(i); }));
  ```

**总结你的代码重点：**

> **`std::bind` 实现了将“成员函数 + 实例对象 + 参数”打包成“统一的任务接口 (`std::function`)”，这是线程池能够管理不同类成员函数的核心技术手段。**

### 1.3.3 lambda 表达式

#### 1.3.3.1 概念 

**lambda : 匿名函数 , 返回类型是函数对象 , 同时它也是函数对象的升级版**

**lambda 表达式的优点 : **

* **普通的对象在进行泛型算法参数传递 , 自定义比较方式 , 进行自定义操作 时不够灵活 , 比如说优先级队列 , 写了一个类 , 因为优先级队列出队自动排序的特性 , 写的普通类进行自定义比较的时候如果参数过多 , 那就不够灵活的实现 , 这就体现了 lambda 的灵活特性了 **

#### 1.3.3.2 语法 

**\[捕获列表\](参数列表)->返回值类型{语句体}**

##### 1.3.3.2.1 基本语句 

```c++
#include <iostream>

int main() {
    auto it = []()->void { std::cout << "hello world!" << std::endl; };
    it();
    return 0;
}
```

**在这类情形中 , 返回值如果没有的话是可以省略 -> 返回值类型 这部分的 **

```c++
#include <iostream>

int main() {
    auto it = []() { std::cout << "hello world!" << std::endl; }
    it();
    return 0;
}
```

****

**带返回类型的**

```c++
#include <iostream>

int main() {
    auto func1 = [](int a , int b)->int { return a + b; };
    std::cout << func1(10 , 20) << std::endl;
    return 0;
}
```

##### 1.3.3.2.2 捕获列表 

| Lambda 捕获方式与底层实现对照表 |                                                  |                                                              |
| ------------------------------- | ------------------------------------------------ | ------------------------------------------------------------ |
| **捕获语法**                    | **底层类成员实现 (模拟)**                        | **特点与影响**                                               |
| **`[]`** (空捕获)               | 无成员变量                                       | **性能最优**。本质是一个普通的函数对象，不占用额外内存空间。 |
| **`[a, b]`** (值捕获)           | `int ma, mb;`                                    | **快照机制**。类内部持有变量副本。默认 `operator()` 是 `const` 的，必须加 `mutable` 才能在内部修改副本。 |
| **`[&a, &b]`** (引用捕获)       | `int &ma, &mb;`                                  | **直接映射**。内部修改即外部修改。需注意：若 Lambda 寿命长于原变量，会导致**悬空引用**。 |
| **`[this]`**                    | `ClassType* const m_this;`                       | **成员访问**。允许在 Lambda 内部通过 `this` 指针访问所属类的所有 `public/private` 成员。 |
| **`[=]`** (全值捕获)            | 根据代码中用到的变量，自动生成对应的**值成员**   | **省心模式**。编译器会自动分析 Lambda 体内用到了哪些外部变量，并全部以**值传递**方式拷贝进类。 |
| **`[&]`** (全引用捕获)          | 根据代码中用到的变量，自动生成对应的**引用成员** | **便捷模式**。自动分析用到的变量并以**引用方式**关联。最常用，但风险也最高（生命周期问题）。 |
| **`[a, &b]`** (混合捕获)        | `int ma; int &mb;`                               | **精准控制**。`a` 被拷贝（安全、隔离），`b` 被引用（同步、高效）。 |

##### 1.3.3.2.3 lambda 的常方法 

**假定现在要交换两个元素的值 , 利用lambda 表达式 **

```c++
#include <iostream>

int main() {
    int a = 10;
    int b = 20;
    
    auto func = []() {
        int temp = a;
        a = b;
        b = temp;
    };
    std::cout << "a : " << a << " b : " << b << std::endl;
    // 10 20
    return 0;
}
```

**发现并没有交换元素的值,在底层的lambda 是类似这样的实现 **

```c++
template<typename T = int>
class TestLambda03 {
    public:
        TestLambda03(int a , int b)
            : ma(a)
            , mb(b)
        {}
        void operator() () const {
            int tmp = ma;
            ma = mb;
            mb = tmp;
        }
    private:
        int ma;
        int mb;
    	//mutable int ma; // 添加 multable 修饰 可以改变 const
        //mutable int mb;
};
```

编译器在底层生成的**闭包类（Closure Class）**逻辑如下：

| **特性**     | **你的 TestLambda03**           | **编译器生成的 Lambda 类**                                |
| ------------ | ------------------------------- | --------------------------------------------------------- |
| **成员变量** | `ma`, `mb` (拷贝了外部变量的值) | 自动生成同名成员 (拷贝了 `a`, `b`)                        |
| **构造函数** | `TestLambda03(int a, int b)`    | 自动生成的构造函数，用于初始化捕获列表                    |
| **调用方式** | 重载 `operator()`               | 重载 `operator()`                                         |
| **常量性**   | `void operator()() const`       | **默认也是 `const`**                                      |
| **修改权限** | 使用 `mutable` 修饰成员变量     | 因为加了 `mutable` 关键字，`operator()` 的 `const` 被去掉 |

****

**那么该如何解除掉这个 const 限制呢 ?  —  引入关键字 mutable**

###### 1.3.3.2.3.1 mutable

在 C++ 中，`mutable` 是一个非常特殊的关键字，它的字面意思是“**可变的**”。它唯一的使命就是：**突破 `const` 的限制**。

要透彻理解 `mutable`，我们需要从**普通类成员**和 **Lambda 表达式**两个场景来看，因为它们的底层逻辑虽然相通，但表现形式略有不同。

------

**1. 在普通类（仿函数）中的 `mutable`**

在类中，如果一个成员函数被声明为 `const`（常方法），编译器会禁止你在该函数内修改任何非静态成员变量。

**但是，有些变量的修改并不影响对象的“逻辑状态”。** 比如缓存、计数器、锁（mutex）等。这时候就需要 `mutable`。

**语法表现：**

```c++
class MyCounter {
public:
    int get_count() const { // 常方法：逻辑上不应该修改对象
        m_access_count++;   // 错误！不能修改成员
        return m_data;
    }
private:
    int m_data = 0;
    mutable int m_access_count = 0; // 标记为 mutable，即使在 const 方法里也能改
};
```

------

**2. 在 Lambda 表达式中的 `mutable`**

Lambda 表达式在底层会被编译器翻译成一个**闭包类**。

**默认情况：只读快照**

当你使用**值捕获 `[a, b]`** 时，编译器生成的 `operator()` 默认带 `const`。

```c++
int x = 10;
auto f = [x]() { 
    // x++; // 编译报错：表达式必须是可修改的左值
};
```

**底层逻辑：** 编译器生成的 `operator()() const` 认为，既然你只是抓取了一份快照，就不应该在函数调用时改变这个快照的值。

**开启 `mutable`：允许修改副本**

当你加上 `mutable` 关键字，情况就变了：

C++

```c++
int x = 10;
auto f = [x]() mutable { 
    x++; // 合法！
    std::cout << "Inside: " << x << std::endl; 
};

f(); // 输出 Inside: 11
std::cout << "Outside: " << x << std::endl; // 输出 Outside: 10
```

**底层逻辑演变：**

1. **去掉 `const`**：加上 `mutable` 后，编译器生成的闭包类成员函数变成了 `void operator()()`（去掉了末尾的 `const`）。
2. **修改的是成员变量**：`x++` 修改的是闭包类内部存储的那个成员变量副本，而不是外部作用域的 `x`。

------

**3. `mutable` 的底层对比总结**

我们可以把 `mutable` 在两种场景下的作用画个等号：

| **维度**     | **Lambda 表达式中的 mutable**            | **你模拟类 TestLambda03 中的 mutable** |
| ------------ | ---------------------------------------- | -------------------------------------- |
| **作用对象** | 作用于 **`operator()` 方法**。           | 作用于 **成员变量**。                  |
| **直接效果** | 删除了函数签名末尾的 `const`。           | 允许在 `const` 函数里改写特定变量。    |
| **最终目的** | 允许在 Lambda 体内修改**值捕获**的副本。 | 允许在常方法里修改**逻辑无关**的变量。 |
| **外部影响** | 不影响外部原始变量。                     | 不影响外部原始变量（如果是传值构造）。 |

------

**4. 为什么要这么设计？（避坑指南）**

很多初学者会问：*“既然修改 Lambda 内部的 `x` 不影响外部，那为什么要费劲加 `mutable` 限制呢？”*

这就是 C++ **类型安全**和**语义严谨**的体现：

- **防止误解**：如果不加 `mutable` 就能改，程序员可能会以为 `[x]` 捕获能修改外部的 `x`（实际上只有 `[&x]` 能改外部）。
- **状态保持**：Lambda 对象是可以多次调用的。如果你在一个 Lambda 里修改了捕获的副本，这个修改会**保留到下一次调用**。

**例子：**

```c++
auto counter = [n = 0]() mutable {
    return ++n;
};

std::cout << counter(); // 1
std::cout << counter(); // 2 (状态被保留在闭包类内部成员中)
```

**核心结论：**

- 如果你想在 Lambda 内部修改**值捕获**进来的变量副本，并希望这个状态在多次调用间**延续**，请用 `mutable`。
- 如果你想修改**外部**真实的变量，请直接用 **引用捕获 `[&]`**，这种情况下不需要 `mutable`，因为引用本身就是为了修改。

###### 1.3.3.2.3.2 引用

**1. 引用捕获 `[&]` 的底层原理**

当你使用 `[&a, &b]` 时，编译器生成的闭包类内部存储的是**原始变量的引用**（指针实现）。

**模拟类实现：**

```c++
class TestLambdaRef {
public:
    TestLambdaRef(int &a, int &b) : ma(a), mb(b) {}
    
    // 注意：引用捕获不需要 mutable！
    // 因为 const 方法禁止修改成员变量的值，但“修改引用指向的对象”不算修改成员变量本身。
    void operator()() const {
        int tmp = ma;
        ma = mb;      // 修改的是外部原始变量
        mb = tmp;     // 修改的是外部原始变量
    }
private:
    int &ma; // 成员是引用
    int &mb;
};
```

------

**2. `mutable` (值捕获) vs `[&]` (引用捕获) 的区别**

我们可以通过下面这个表格清晰地对比两者的本质：

| **维度**         | **[a] mutable (值捕获 + mutable)**                        | **[&a] (引用捕获)**                                          |
| ---------------- | --------------------------------------------------------- | ------------------------------------------------------------ |
| **内存位置**     | 在 Lambda 对象的**内部副本**中。                          | 指向**外部作用域**的原始内存。                               |
| **外部影响**     | **无影响**。内部改完外部看不到。                          | **有影响**。内部改完外部同步变化。                           |
| **生命周期依赖** | **独立**。外部变量销毁后，Lambda 还能运行（因为有副本）。 | **依赖**。外部变量销毁后，调用 Lambda 会导致**非法访问（悬空引用）**。 |
| **底层成员**     | `int ma;`                                                 | `int &ma;`                                                   |
| **使用场景**     | 需要保存“那一刻”的状态，且不希望干扰外部。                | 需要与外部进行数据通信或修改外部状态。                       |

------

**3. 核心逻辑差异图解**

**场景 A：`[a] mutable` —— “影分身”**

就像你复制了一个文档副本去修改。你改得再乱，原件还是干干净净的。但是，如果你连续调用两次 Lambda，第二次看到的 `a` 是第一次修改后的结果（副本在 Lambda 对象内部是持久的）。

**场景 B：`[&a]` —— “远程桌面”**

就像你通过远程桌面操作另一台电脑。你看到的、改到的，就是那台电脑本身。如果那台电脑关机了（外部变量作用域结束），你的连接就断了，甚至会崩溃。

------

**4. 为什么引用捕获不需要 `mutable`？**

这是一个语法上的巧妙之处：

- `const` 成员函数（常方法）的要求是：**不能改变成员变量的值**。
- 对于 `int &ma` 这个成员来说，它的“值”是它绑定的那个地址。
- 在 Lambda 内部执行 `ma = 10`，改变的是**地址指向的内容**，而不是**地址本身**。
- 因此，引用捕获在底层天然支持修改外部变量，无需额外声明 `mutable`。

------

**5. 总结：该选哪一个？**

在实际开发中，你的选择标准应该是：**“我需不需要影响外面？”**

1. **选 `[&]`（引用捕获）的情况：**
   - 你要修改外部的 `vector`、`count` 等。
   - 传递的是大对象（如 `std::string` 或容器），为了性能，不想发生拷贝。
   - **前提：** 确保 Lambda 的生命周期比被捕获变量短（例如立即执行的 `std::sort`）。
2. **选 `[=] mutable`（值捕获 + mutable）的情况：**
   - 你只想在 Lambda 内部维护一个“私有状态”（比如一个内部计数器）。
   - 你需要把 Lambda 传给另一个线程或者异步回调，此时外部变量可能已经销毁了。
   - 你想实现一个“函数发生器”，每次调用产生不同的内部结果，但不干扰外界。

**一句话总结：** `mutable` 是为了在**副本**上延续状态；引用捕获是为了在**本体**上同步修改。

###### 1.3.3.2.3.3 总结代码

```c++
#include "00. head file.hpp"

template<typename T = void>
class TestLambda01 {
    public:
        void operator() () {
            std::cout << "hell0 world!" << std::endl; 
        }
};

template<typename T = int>
class TestLambda02 {
    public:
        int sum(int a , int b) const {
            return a + b;
        }
};

template<typename T = int>
class TestLambda03 {
    public:
        TestLambda03(int a , int b)
            : ma(a)
            , mb(b)
        {}
        void operator() () const {
            int tmp = ma;
            ma = mb;
            mb = tmp;
        }
    private:
        mutable int ma; // 添加 multable 修饰
        mutable int mb;
};

template<typename T>
class TestLambda04 {
    public:
        TestLambda04(int &a , int &b)
            : ma(a)
            , mb(b)
        {}
        void operator() () const {
            int tmp = ma;
            ma = mb;
            mb = tmp;
        }
    private:
        int &ma; // 添加 multable 修饰
        int &mb;
};
int main() {
    auto func1 = []()-> void { std::cout << "hello world!" << std::endl; };
    func1();

    auto func2 = [](int a , int b)->int { return a + b; };
    std::cout << func2(1 , 2) << std::endl;

    int a = 10;
    int b = 20;
    auto func3 = [a , b]() mutable {  // 这是个常方法 , 是不允许修改值的 , 添加 multable 就可以修改了
        int temp = a;
        a = b;
        b = a;
    };
    std::cout << "a : " << a << " " << "b : " << b << std::endl;

    // 可以用引用传递 
    auto func4 = [&a , &b]() {  
        int temp = a;
        a = b;
        b = a;
    };
    std::cout << "a : " << a << " " << "b : " << b << std::endl;

    std::cout << "--------------------------" << std::endl;
    TestLambda01<> t1;
    t1();

    TestLambda02<> t2;
    std::cout << t2.sum(10 , 20) << std::endl;

    TestLambda03<> t3(a , b);
    t3();
    return 0;
}

```

##### 1.3.3.2.4 lambda 与 STL 容器的联系

```c++
#include "00. head file.hpp"

int main() {
    std::vector<int> vec;
    for(int i = 0 ; i < 20 ; ++ i) {
        vec.push_back(rand() % 100 + 1);
    }

    sort(vec.begin() , vec.end() , 
        [](int a , int b)->bool {
            return a > b;
    });

    for(int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    auto it = find_if(vec.begin() , vec.end() ,
        [](int a)->bool {
            return a < 65;
        }
    );

    if(it != vec.end()) {
        vec.insert(it , 65);
    }

    for(int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    for_each(vec.begin() , vec.end() , [](int a)
    {
        if(a % 2 == 0)
            std::cout << a << " ";
    });
    std::cout << std::endl;
    return 0;
}
```

**1. 行为定制：算法的“灵魂”**

容器只负责存数据，而算法（`std::sort`, `std::find_if` 等）负责处理数据。Lambda 充当了两者之间的**粘合剂**，让你能就地定义处理规则。

- **排序准则 (`sort`)**：

  你代码中的 `[](int a, int b) { return a > b; }` 将默认的升序改成了降序。这种“就地定义”避免了为一个小逻辑去写一个完整的类或函数。

- **过滤条件 (`find_if`)**：

  算法提供遍历框架，Lambda 提供判断标准（谓词）。

------

**2. 状态捕获：超越普通函数指针**

这是 Lambda 相比传统函数指针最大的优势。在容器操作中，我们经常需要用到**外部变量**作为判断依据。

- **场景举例**：如果你想找容器中所有大于某个变量 `threshold` 的元素。

- **传统做法**：需要写一个仿函数类，通过构造函数传入并保存 `threshold`。

- **Lambda 做法**：

  C++

  ```c++
  int threshold = 65;
  auto it = find_if(vec.begin(), vec.end(), [threshold](int a) {
      return a > threshold; // 轻松捕获外部变量
  });
  ```

------

**3. 遍历与就地操作 (`for_each`)**

`std::for_each` 结合 Lambda 可以替代复杂的 `for` 循环，尤其是当你只需要对容器元素进行简单输出或轻量修改时。

- 你的代码示例：`for_each(vec.begin(), vec.end(), [](int a) { ... });`
- **优势**：代码语义更清晰，明确告诉阅读者“我要对这个区间里的每一个元素执行某种操作”。
- **注意**：如果你需要修改容器里的元素，记得参数要用引用：`[](int &a) { a += 10; }`。

------

**4. 性能与底层原理**

- **内联优化**：由于 Lambda 在底层被编译为匿名仿函数（函数对象），编译器更容易对其进行**内联（Inline）**处理。相比传统的函数指针调用，Lambda 在 STL 算法中的性能通常更高。
- **闭包对象**：当你把 Lambda 传递给算法时，你实际上是传递了一个**闭包对象**。算法内部会多次调用这个对象的 `operator()`。

------

**总结表格：Lambda 在容器算法中的角色**

| **算法类型**  | **代表函数**              | **Lambda 的作用**             | **典型捕获方式**             |
| ------------- | ------------------------- | ----------------------------- | ---------------------------- |
| **排序/比较** | `sort`, `priority_queue`  | 定义优先级或排序顺序          | `[]` (通常不需要外部变量)    |
| **搜索/计数** | `find_if`, `count_if`     | 定义匹配的“谓词”（Predicate） | `[=]` (捕获查找的目标值)     |
| **变换/修改** | `transform`, `replace_if` | 定义转换逻辑或替换条件        | `[&]` (如果涉及修改外部状态) |
| **遍历**      | `for_each`                | 定义对每个元素的操作          | `[ ]` 或 `[&]`               |

#### 1.3.3.3 跨语句使用的 lambda 

**既然 lambda 只能使用在语句中 , 如果想跨语句使用之前定义好的 lambda 表达式该怎么做 ? 用什么类型表示 lambda 表达式?**

**ans : `利用 function 函数对象 `**

```c++
#include "00. head file.hpp"

class Data {
    public:
        Data(int val1 = 10 , int val2 = 20) 
            : ma(val1)
            , mb(val2)
        {}
        // bool operator>(const Data &data) { // 成员变量很多 , 不灵活
        //     return ma > data.ma;
        // }
        // bool operator<(const Data &data) {
        //     return ma < data.ma;
        // }

        int ma;
        int mb;  
};
int main() {

    // 优先级队列 
    std::priority_queue<Data , std::vector<Data> , std::function<bool(Data& , Data&)>> MaxHeap(
        [](Data &d1 , Data &d2)->bool 
    {
        return d1.ma > d2.ma;
    });
    MaxHeap.push(Data(10 , 20)); // 优先级队列里面要进行比较 , 这个 Data 可不能比较(见上)
    MaxHeap.push(Data(15 , 25));
    MaxHeap.push(Data(20 , 25));

    // 智能指针自定义删除器 delete p;
    //std::unique_ptr<FILE> ptr1(fopen("data.txt" , "w")); // 这样写默认delete释放 , 但是正确的释放时 fclose(FILE*)
    std::unique_ptr<FILE , std::function<void(FILE*)>> ptr1(fopen("data.txt" , "w")
        , [](FILE* pf) {
            fclose(pf);
    });

#if 0
    std::map<int , std::function<int(int , int)>> caculateMap;
    caculateMap[1] = [](int a , int b)->int {
        return a + b;
    };
    caculateMap[2] = [](int a , int b)->int {
        return a - b;
    };
    caculateMap[3] = [](int a , int b)->int {
        return a * b;
    };

    caculateMap[4] = [](int a , int b)->int {
        return a / b;
    };

    std::cout << "选择:";
    int choice;
    std::cin >> choice;
    std::cout << "10 + 15 : " << caculateMap[choice](10 , 15) << std::endl;
#endif
    
    return 0;
}
```

##### 1.3.3.3.1 优先级队列的策略

```c++
std::priority_queue<Data, ..., std::function<bool(Data&, Data&)>> MaxHeap([](Data &d1, Data &d2) {
    return d1.ma > d2.ma;
});
```

- **干了什么**：给“大厨”（`priority_queue`）递了一张“排队规矩”的纸条。

- **核心痛点**：默认的堆只能比较自带 `operator<` 的类型（比如 `int`）。你的 `Data` 类很复杂，编译器不知道该按 `ma` 排还是按 `mb` 排。

- **Lambda 的功劳**：它让你**不需要改动 `Data` 类的源码**，就能在创建队列的一瞬间，决定好这个堆的“大小逻辑”。

- **为什么 std::priority_queue<Data , std::vector<Data> , std::function<bool(Data& , Data&)>> 要写 vector?**

  **在 C++ 标准库中，`std::priority_queue` 是一个**容器适配器**。它本身不负责存储数据，而是“套”在一个现成的容器（比如 `vector` 或 `deque`）外面来工作。

  ------

  **1. 查看 `priority_queue` 的定义**

  首先看编译器眼中这个类的定义：

  ```c++
  template<
      class T,                            // 1. 元素类型
      class Container = std::vector<T>,   // 2. 底层容器（默认是 vector）
      class Compare = std::less<typename Container::value_type> // 3. 比较方式
  > class priority_queue;
  ```

  这就是原因所在：**底层容器（Container）在模板参数列表中排在第二位，而比较方式（Compare）排在第三位。**

  ------

  **2. C++ 的“跳格子”规则**

  C++ 的模板参数是**按顺序填充**的。如果你想指定第三个参数（比较函数），你就不能跳过第二个参数（底层容器）。

  - **如果你只写两个：** `priority_queue<Data, MyCompare>` 编译器会认为 `MyCompare` 是你指定的**容器**，然后报错（因为比较函数显然不是容器）。
  - **为了让编译器知道你的 Lambda 是第三个参数：** 你必须显式地把第二个位置填上。既然默认就是 `vector`，那我们就手动写一次 `std::vector<Data>` 把位置占住。

  ------

  **3. 为什么设计成必须要“容器”？**

  你可能会想：为什么不直接设计成 `priority_queue<Data, Compare>` 呢？

  因为 `priority_queue` 给了开发者极大的自由。虽然 99% 的情况下我们用 `vector`，但如果你对性能有极致要求，或者有特殊内存需求，你可以把它换成 `std::deque`： `std::priority_queue<Data, std::deque<Data>, MyCompare> heap;`

  **为了保留这种更换“底层存储引擎”的能力，标准库把容器参数放在了前面。**

  ------

  **4. 一个对比示例**

  这就像去店里买奶茶：

  1. **默认款**：`奶茶<珍珠>` （默认是大杯、正常冰）
  2. **改甜度**：如果你想说“少糖”，你必须按顺序说：`奶茶<珍珠, 大杯, 少糖>`。 你不能直接说 `奶茶<珍珠, 少糖>`，因为店员会以为“少糖”是你要求的杯子大小。

  ------

  💡 **进阶技巧：如何写得更优雅？**

  在 C++17 及以后，由于有了 **CTAD (Constructor Template Argument Deduction)**，如果你直接在构造函数里传 Lambda，编译器有时可以自动推导，但在处理 `std::function` 时依然比较麻烦。

  最推荐的做法是使用 `using` 起别名，这样代码就不会显得那么乱：

  ```c++
  using DataFunc = std::function<bool(Data&, Data&)>;
  using DataHeap = std::priority_queue<Data, std::vector<Data>, DataFunc>;
  
  DataHeap MaxHeap([](Data &d1, Data &d2) {
      return d1.ma > d2.ma;
  });
  ```

  **总结**

  写 `std::vector<Data>` 纯粹是为了**给中间的模板位置“占座”**，这样编译器才能正确地把你的 Lambda 识别为第三个参数（比较器）。

-  **坑 : d1.ma > d2.ma 是大根堆还是小根堆呢 ? **

  **ans : `在 std::priority_queue 中 , 我们看到的和它的比较逻辑是相反的 , > 是小根堆 , < 是大根堆`**

-  **为什么“大于”反而变成了“小根堆”？**

  这背后的逻辑是这样的：`priority_queue` 默认是**大根堆**，它默认使用的比较器是 `std::less`。

  - **默认逻辑**：`std::priority_queue` 认为：如果 `Compare(a, b)` 返回 `true`，那么 `a` 的优先级比 `b` **低**，`a` 应该排在后面（堆的底层）。
  - **你的逻辑**：你写的是 `d1.ma > d2.ma`。
    - 当 `d1` (15) > `d2` (10) 时，返回 `true`。
    - `priority_queue` 看到 `true`，就认为 15 的优先级比 10 **低**。
    - 结果：小的数字（10）反而被认为优先级更高，排到了堆顶。

##### 1.3.3.3.2 智能指针的策略 

```c++
std::unique_ptr<FILE, std::function<void(FILE*)>> ptr1(fopen(...), [](FILE* pf) {
    fclose(pf);
});
```

- **干了什么**：给“保险箱”（`unique_ptr`）换了个“自毁装置”。
- **核心痛点**：`unique_ptr` 默认只会 `delete`。但 `FILE*` 是 C 语言的文件句柄，必须用 `fclose` 关掉。如果强行 `delete`，文件就没关严，会导致数据丢失。
- **Lambda 的功劳**：它充当了一个**“定制化的垃圾桶”**。当指针离开作用域时，它不执行 `delete`，而是执行你纸条上写的 `fclose`。

##### 1.3.3.3.3 计数器 map 的策略

```c++
std::map<int, std::function<int(int, int)>> caculateMap;
caculateMap[1] = [](int a, int b) { return a + b; };
// ...
```

- **干了什么**：做了一个“逻辑工具箱”。把不同的“动作”（加减乘除）像存零钱一样存进了 `map` 里。
- **核心痛点**：如果没有这个，你可能要写一长串恶心的 `if(choice == 1) ... else if(choice == 2) ...`。
- **Lambda 的功劳**：它把**“代码逻辑”变成了“数据”**。你想增加个“开根号”功能？直接往 `map` 里再塞一个 Lambda 就行，主逻辑一行都不用改。

##### 1.3.3.3.3 总结

**A. Lambda 不再是“一次性用品”**

以前 Lambda 只能在 `sort` 语句里用完就扔。现在配合 `std::function`，你可以把 Lambda **存起来**（存入 `map`）、**带走**（交给 `unique_ptr`）、**持久化**（作为堆的比较规则）。

**B. 实现了真正的“逻辑解耦”**

- **数据归数据**：`Data` 类只需要负责存 `ma` 和 `mb`。
- **组件归组件**：`priority_queue` 和 `unique_ptr` 只负责管理容器和生命周期。
- **逻辑归 Lambda**：怎么排、怎么删，由你这个调用者通过 Lambda 临时决定。

**C. 代码的“动态性”增强**

你的程序不再是死板的一条线跑到底，而是可以根据用户的 `choice`（选择），从 `map` 里动态地拎出一段 Lambda 逻辑来执行。这其实就是**函数式编程**的核心思想：**把逻辑当成参数传来传去。**

----

## 1.4 C++11 知识点总结以及多线程

### 1.4.1 C++ 11 知识点总结  

#### 1.4.1.1 关键字和语法 

##### 1.4.1.1.1 auto 

**auto 可以根据右值的类型 , 自动推导出右值的类型 , 然后左边的类型也就已知了**

**1. 基本原理**

当你使用 `auto` 声明变量时，编译器不再是简单地寻找一个类型名，而是在**编译阶段**通过查看等号右侧的**初始化表达式（initializer）**来推断其具体的类型。

```c++
auto x = 10;           // 10 是 int，所以 x 是 int
auto y = 3.14;         // 3.14 是 double，所以 y 是 double
auto ptr = &x;         // &x 是 int*，所以 ptr 是 int*
```

------

**2. 核心规则**

虽然 `auto` 看起来很智能，但它遵循一些严谨的推导规则（主要基于模板类型推导）：

- **必须初始化**：`auto` 变量必须在定义时初始化，否则编译器无法推导。

  - 错误：`auto a;`

- **抛弃顶层 const 和 引用**：通常情况下，`auto` 会忽略掉初始化表达式的引用属性（&）和顶层 `const`。

  ```c++
  int i = 0;
  int& r = i;
  auto a = r;  // a 的类型是 int，而不是 int&
  
  const int ci = i;
  auto b = ci; // b 的类型是 int，而不是 const int
  ```

- **保留底层 const**：如果需要 `const` 或引用，必须手动加上修饰符：

  ```c++
  const auto c = ci; // c 是 const int
  auto& d = i;       // d 是 int&
  ```

------

**3. 为什么使用 auto？（优势）**

| **优势**           | **说明**                                                     |
| ------------------ | ------------------------------------------------------------ |
| **简化复杂类型**   | 尤其是在处理迭代器（iterator）或 lambda 表达式时，可以避免书写冗长的类名。 |
| **避免类型不匹配** | 自动确保左侧变量类型与右侧函数返回值完全一致，减少隐式转换带来的性能损耗或精度丢失。 |
| **重构友好**       | 如果你更改了某个函数的返回类型，所有使用 `auto` 接收该返回值的变量会自动更新，无需手动修改。 |

**代码对比示例：**

```c++
// 不使用 auto
std::map<std::string, std::vector<int>>::iterator it = myMap.begin();

// 使用 auto
auto it = myMap.begin(); 
```

------

**4. 限制与注意事项**

- **不能用于函数参数**：在 C++11/14 中，`auto` 不能直接作为普通的函数形参（但在 C++20 中允许用于简写的泛型函数）。

- **不能用于非静态成员变量**：类成员变量不能直接声明为 `auto`。

- **推导结果可能不符合预期**：例如在处理 C 风格字符串时：

  ```c++
  auto s = "hello"; // s 的类型是 const char*，而不是 std::string
  ```

**总结：** `auto` 不是把 C++ 变成了动态类型语言，它依然是**静态类型**的。编译器只是帮你把那些显而易见、或者过于复杂的类型名给写了，从而提高开发效率。

##### 1.4.1.1.2 nullptr 

**nullptr 是指针专用的 ‘0’ , NULL 是宏定义 – #define NULL 0**

可以把 `nullptr` 看作是“强类型的空指针”。

------

**1. 核心区别：类型本质**

这是两者最根本的区别：

- **`NULL`**：本质是一个**整数宏**。在大多数编译器中，它被定义为 `0` 或 `(void*)0`。
- **`nullptr`**：本质是一个**字面值常量**，它的类型是 `std::nullptr_t`。它可以隐式转换为任何指针类型（如 `int*`, `char*` 等），但它**绝对不是**整数。

------

**2. 为什么需要 `nullptr`？（解决函数重载歧义）**

这是 `nullptr` 存在的最现实意义。假设有以下两个重载函数：

```c++
void foo(int x) { 
    std::cout << "调用了 int 版本" << std::endl; 
}

void foo(char* p) { 
    std::cout << "调用了 指针 版本" << std::endl; 
}
```

当你尝试传递“空”时，两者的表现完全不同：

- **调用 `foo(NULL)`**：因为 `NULL` 是 `0`，编译器会优先匹配 `int` 版本。这通常违背了程序员的本意（原本想传空指针，结果进了整数函数）。
- **调用 `foo(nullptr)`**：它永远不会匹配 `int`，而是精确地匹配 `char*` 版本。

------

**3. 内存与安全性比较**

| **特性**     | **NULL**             | **nullptr**            |
| ------------ | -------------------- | ---------------------- |
| **定义方式** | 宏定义 (`#define`)   | 关键字 / 字面量        |
| **类型**     | `int` (通常)         | `std::nullptr_t`       |
| **类型安全** | 弱（可能与整数混淆） | 强（仅限指针相关转换） |
| **推荐场景** | C 语言代码兼容       | 所有现代 C++ 开发      |

------

**4. 深度细节：`std::nullptr_t`**

`nullptr` 的类型是 `std::nullptr_t`。这意味着你可以用它来定义专门接收空指针的变量或函数参数：

```c++
void process(std::nullptr_t) {
    // 这个函数只接受 nullptr 
}

int main() {
    int* p = nullptr; // OK
    process(nullptr);  // OK
    // process(0);     // 错误！0 是 int 而不是 nullptr_t
}
```

**5. 总结建议**

在现代 C++（C++11 及以后）开发中：

1. **永远不要再使用 `NULL`**。
2. 使用 `nullptr` 让你的意图（“这是一个指针”）在编译阶段就清晰可见。
3. 它不仅能避免隐藏的 Bug，还能让 IDE 的静态代码分析更准确。

##### 1.4.1.1.3 for_each 

**for_each 可以遍历数组 , 容器等 **

分清它和“基于范围的 `for` 循环”的区别，可以从以下几个点深入：

------

**1. 语法结构**

`for_each` 的核心思想是将**“遍历范围”**与**“操作逻辑”**解耦。

```c++
#include <algorithm>
#include <vector>

std::vector<int> v = {1, 2, 3};

// 语法：for_each(起始迭代器, 结束迭代器, 函数或 Lambda);
std::for_each(v.begin(), v.end(), [](int n) {
    std::cout << n << " ";
});
```

------

**2. `for_each` vs 基于范围的 `for` 循环**

虽然它们都能遍历，但在实际开发中侧重点不同：

| **特性**     | **基于范围的 for (C++11)**             | **std::for_each**                                          |
| ------------ | -------------------------------------- | ---------------------------------------------------------- |
| **易读性**   | 极高，代码最精简                       | 略低，需要传递函数对象或 Lambda                            |
| **灵活性**   | 遍历整个容器                           | 可以通过迭代器只遍历**局部区间**（如前 3 个）              |
| **控制流**   | 可以使用 `break`, `continue`, `return` | **无法使用 `break` 或 `continue`**（因为它本质是函数调用） |
| **并行支持** | 不支持原生并行                         | C++17 之后支持**执行策略**（可一键开启并行）               |

------

**3. 高级进阶：并行执行 (C++17)**

这是 `for_each` 相比普通循环的一个巨大优势。如果你有几百万个数据要处理，只需改动一行：

```
#include <execution> // 需包含此头文件

// std::execution::par 表示并行执行（Parallel）
std::for_each(std::execution::par, v.begin(), v.end(), [](auto& x) {
    complex_calculation(x); // 多核并行计算
});
```

------

**4. 什么时候用 `for_each`？**

- 当你需要**局部遍历**（例如：只遍历容器的中间一段）。
- 当你的操作逻辑已经封装成了一个**现成的函数或函数对象**。
- 当你需要**利用多核性能**进行并行计算时（C++17 并行算法）。

##### 1.4.1.1.4 右值引用 

* **move 移动语义函数**
* **forward 完美转发函数**

**1. 什么是右值引用？**

在 C++11 之前，我们只有左值引用（`T&`）。

- **左值 (lvalue)**：有名字、可以取地址的变量（比如 `int a = 1;` 中的 `a`）。
- **右值 (rvalue)**：临时对象、字面量、即将销毁的值（比如 `1`，或者函数返回的临时变量）。

**右值引用 (`T&&`)** 专门用来绑定到这些即将销毁的临时对象上。

```c++
int  a = 10;
int& ref = a;          // 左值引用
// int&& rref = a;     // 错误！不能把右值引用绑定到左值
int&& rref = 10;       // 正确
```

------

**2. std::move：移动语义**

`std::move` 并不移动任何东西。它的唯一作用是：**强制将一个左值转变为右值**。

**为什么要这么做？**

为了触发**移动构造函数**。想象你有一个装满 1GB 数据的 `std::vector`。

- **拷贝**：申请新的 1GB 内存，把旧数据复制一遍。
- **移动**：直接把旧容器的指针“偷”过来，指向这 1GB 内存，原来的容器置为空

```c++
std::string s1 = "Hello";
std::string s2 = std::move(s1); // s1 的资源被转移给了 s2

// 此时 s1 变成了空字符串，s2 获得了 "Hello"
// 这比拷贝整个字符串快得多
```

------

**3. std::forward：完美转发**

完美转发通常出现在**模板函数**中。

在模板中，`T&&` 被称为**万能引用（Universal Reference）**，它既能接收左值也能接收右值。但问题是：**一旦进入函数内部，所有的参数都会变成有名字的左值**。

`std::forward` 的作用就是：如果进来的是右值，传出去还是右值；如果进来的是左值，传出去还是左值。

```c++
template<typename T>
void wrapper(T&& arg) {
    // 如果不加 forward，arg 永远被当作左值处理
    func(std::forward<T>(arg)); 
}
```

------

**4. 核心对比总结**

| **特性**            | **说明**         | **核心作用**                             |
| ------------------- | ---------------- | ---------------------------------------- |
| **右值引用 (`&&`)** | 一种新的类型声明 | 允许我们识别并操作“临时对象”             |
| **`std::move`**     | 强制类型转换     | 把左值“标”为右值，从而触发高效的移动操作 |
| **`std::forward`**  | 保持参数属性     | 在模板中原样传递参数的左/右值属性        |

------

**5. 为什么要学这个？**

- **性能提升**：对于 `vector`, `string` 等大对象，移动性能是拷贝的几百倍。
- **唯一性对象**：像 `std::unique_ptr` 这种禁止拷贝的对象，必须通过 `move` 来转移所有权。

> **警示：** 被 `std::move` 之后的对象，其状态是有效的但未定义的（通常为空）。除非你重新给它赋值，否则不要再访问它的内容。

##### 1.4.1.1.5 模板新特性  – 可变参

**template<typename… A>**

**1. 参数包 (Parameter Pack)**

- **`typename... A`**：这被称为 **模板参数包**。它代表 0 个或多个类型。
- **`A... args`**：这被称为 **函数参数包**。它代表具体的参数值。

------

**2. 如何解析参数包？（递归法）**

由于参数包不是数组，你不能通过 `args[0]` 来访问。在 C++11/14 中，最经典的方法是使用**递归**：定义一个处理“头”的函数，然后不断递归调用处理“剩下的尾巴”。

```c++
#include <iostream>

// 递归终止函数：当参数包为空时调用
void print() {
    std::cout << "遍历结束" << std::endl;
}

// 展开函数
template<typename T, typename... Args>
void print(T first, Args... args) {
    std::cout << "当前参数: " << first << std::endl;
    // 递归调用，将剩下的参数继续展开
    print(args...); 
}

int main() {
    print(1, 2.5, "Hello", 'A');
    return 0;
}
```

------

**3. 获取包的大小：`sizeof...`**

如果你只想知道传进来多少个参数，可以使用专门的运算符 `sizeof...`（注意有三个点）：

```c++
template<typename... Args>
void countArgs(Args... args) {
    std::cout << "收到参数个数: " << sizeof...(args) << std::endl;
}
```

------

**4. 实际应用场景：`emplace_back`**

你之前学过右值引用和 `move`，而可变参模板与它们结合最典型的例子就是 `vector::emplace_back`。

- **普通 `push_back`**：先构造一个临时对象，再拷贝/移动进去。
- **`emplace_back`**：利用可变参模板接收构造函数所需的参数，直接在容器内存中**就地构造**，完全省去了临时对象的开销。

------

**5. 进化：C++17 折叠表达式 (Fold Expressions)**

递归法虽然经典，但写起来比较繁琐。C++17 引入了**折叠表达式**，让解包变得极其简单：

```c++
template<typename... Args>
auto sum(Args... args) {
    return (... + args); // 一行代码实现所有参数求和
}
```

**总结对比**

| **特性**          | **说明**                 |
| ----------------- | ------------------------ |
| **`typename...`** | 声明一个类型包，数量不限 |
| **`args...`**     | 展开参数包，用于传递     |
| **`sizeof...`**   | 获取包中元素的个数       |
| **递归处理**      | C++11 处理变参的主流方式 |

#### 1.4.1.2 绑定器和函数对象一类 

##### 1.4.1.2.1 绑定器 

**bind , bind1st , bind2nd **

**1. 远古时代：`bind1st` 和 `bind2nd` (C++98)**

在 C++98 中，这两个函数用于将**二元函数对象**（接受两个参数）转换为**一元函数对象**（接受一个参数）。它们通过“绑定”其中一个参数为固定值来实现。

- **`bind1st`**：绑定第一个参数。
- **`bind2nd`**：绑定第二个参数。

**缺点：**

1. **语法极其繁琐**：必须配合 `std::ptr_fun` 或 `std::mem_fun` 使用。
2. **类型要求严苛**：只能处理特定结构的函数对象。
3. **已被废弃**：它们在 C++11 中被弃用，在 **C++17 中已正式移除**。

```c++
// C++98 风格：找到第一个大于 10 的元素
// 相当于 x > 10，绑定了 greater 的第二个参数为 10
auto it = std::find_if(v.begin(), v.end(), std::bind2nd(std::greater<int>(), 10));
```

------

**2. 现代时代：`std::bind` (C++11)**

`std::bind` 是一个更通用的绑定器，它取代了上述两个过时的函数。它来自 `<functional>` 头文件。

**核心机制：占位符 (`placeholders`)**

`std::bind` 使用 `_1`, `_2`, `_3` 等占位符来表示**生成的函数对象**在调用时接收的参数。

```c++
#include <functional>
using namespace std;
using namespace std::placeholders; // 必须使用占位符命名空间

void func(int a, int b, int c) {
    cout << a << " " << b << " " << c << endl;
}

int main() {
    // 绑定第一个参数为 100，保留后两个参数
    // _1 代表调用 f1 时的第一个参数，_2 代表第二个
    auto f1 = bind(func, 100, _1, _2);
    f1(20, 30); // 输出: 100 20 30

    // 甚至可以交换参数顺序
    auto f2 = bind(func, _2, _1, 500);
    f2(10, 20); // 输出: 20 10 500 (10对应_1, 20对应_2)
}
```

------

**3. 为什么现在很少用 `bind`？**

虽然 `bind` 很强大，但 C++11 引入的 **Lambda 表达式** 几乎在所有场景下都更优：

| **维度**   | **std::bind**                  | **Lambda 表达式**              |
| ---------- | ------------------------------ | ------------------------------ |
| **可读性** | 占位符 `_1`, `_2` 容易让人头晕 | 像普通函数一样易读             |
| **性能**   | 存在一定的额外开销             | 编译器可以更好地进行内联优化   |
| **调试**   | 报错信息极其冗长复杂           | 报错信息相对直观               |
| **灵活性** | 只能绑定参数                   | 可以写复杂的逻辑（判断、循环） |

**对比示例：**

```c++
// 使用 bind
auto f = std::bind(func, 100, _1, 200);

// 使用 Lambda (推荐方式)
auto f = [](int x) { func(100, x, 200); };
```

------

**4. 总结**

1. **`bind1st` / `bind2nd`**：历史遗迹，看到它们只需知道是旧代码，**不要在任何新代码中使用**。
2. **`std::bind`**：当你需要存储一个复杂的成员函数指针，或者在非常特殊的模板编程中，它依然有用。
3. **首选 Lambda**：在 99% 的情况下，请直接使用 Lambda 表达式。

##### 1.4.1.2.2 函数对象 

**function**

`function`（准确说是 `std::function`）是 C++11 提供的**万能函数封装器**。它是对所有“可调用对象”的一次大统一。

在 C++ 中，有很多东西是可以像函数一样加个括号 `()` 调用的，比如：

1. **普通函数**
2. **函数指针**
3. **仿函数（函数对象类）**
4. **Lambda 表达式**
5. **类成员函数**

在没有 `std::function` 之前，如果你想写一个接收“函数”作为参数的函数，你可能需要写复杂的模板或者丑陋的函数指针定义。现在，`std::function` 把它们全部整合成了一个统一的**类型**。

------

**1. 语法结构**

```c++
std::function` 的模板参数非常直观，直接模拟了函数的签名： `std::function<返回值类型(参数类型1, 参数类型2, ...)>
```

```c++
#include <functional>

// 1. 普通函数
int add(int a, int b) { return a + b; }

// 2. 仿函数（重载了 operator() 的类）
struct Divide {
    int operator()(int a, int b) { return a / b; }
};

int main() {
    // 定义一个统一的封装器：接收两个 int，返回一个 int
    std::function<int(int, int)> op;

    op = add;                    // 封装普通函数
    std::cout << op(10, 5);      // 15

    op = Divide();               // 封装仿函数对象
    std::cout << op(10, 5);      // 2

    op = [](int a, int b) { return a * b; }; // 封装 Lambda
    std::cout << op(10, 5);      // 50
}
```

------

**2. 核心价值：解耦与统一**

`std::function` 最强大的地方在于它可以作为**类成员**或**容器元素**。

想象你在写一个游戏引擎的按钮类，点击按钮时要触发一个动作。这个动作可能是一个全局函数，也可能是一个对象的成员方法，还可能是一个 Lambda。

C++

```c++
class Button {
public:
    // 使用 std::function，按钮不需要关心“动作”具体是什么类型
    std::function<void()> onClick;
    
    void press() {
        if (onClick) onClick(); // 像调用普通函数一样调用它
    }
};
```

------

**3. 注意事项与性能**

- **判空检查**：`std::function` 在调用前应该检查是否包装了对象（类似于指针判空），如果调用一个空的 `std::function` 会抛出 `std::bad_function_call` 异常。
- **开销问题**：相比于直接调用函数或使用模板，`std::function` 涉及到虚函数级别的开销和可能的动态内存分配（Type Erasure 技术）。
  - **建议**：如果是在性能极其敏感的热点循环中，优先考虑**模板**；如果在需要灵活性和回调函数的业务逻辑中，优先考虑 **`std::function`**。

------

**4. 配合 `std::bind` 包装成员函数**

虽然 Lambda 更好用，但有时你必须包装一个类的成员函数，这时 `std::function` 经常配合 `std::bind` 使用：

```c++
struct Player {
    void move(int x) { std::cout << "Moving to " << x; }
};

Player p;
// 包装成员函数需要绑定对象实例
std::function<void(int)> func = std::bind(&Player::move, &p, std::placeholders::_1);
func(100); 
```

------

**总结**

- **函数对象**：是所有能用 `()` 调用的实体的统称。
- **`std::function`**：是这些实体的“容器”，它让不同类型的函数调用拥有了**相同的类型标识**。

##### 1.4.1.2.3 lambda 表达式 

**\[捕获列表\](参数列表)->返回值 { 语句体 }**

**1. 捕获列表 `[]` (Capture Clause)**

这是 Lambda 的灵魂，决定了函数体**如何访问**外部作用域的变量。

| **捕获方式**  | **说明**                                                     |
| ------------- | ------------------------------------------------------------ |
| **`[]`**      | 不捕获任何变量。                                             |
| **`[=]`**     | **按值捕获**所有外部变量（拷贝一份，只读，除非加 `mutable`）。 |
| **`[&]`**     | **按引用捕获**所有外部变量（直接操作原变量，效率高但要注意生命周期）。 |
| **`[x, &y]`** | 混合捕获：`x` 按值捕获，`y` 按引用捕获。                     |
| **`[this]`**  | 捕获当前类的 `this` 指针，以便在 Lambda 中访问成员变量或成员函数。 |

------

**2. 参数列表 `()` (Parameters)**

与普通函数参数一致。如果不需要参数，可以省略不写（但在某些编译器下建议保留 `()`）。

> **C++14 进阶：** 支持泛型 Lambda，可以使用 `auto` 作为参数类型。
>
> ```c++
> [](auto a, auto b) { return a + b; }
> ```

------

**3. 返回值类型 `-> type` (Return Type)**

通常可以**省略**，编译器会根据 `return` 语句自动推导。

但如果函数体有多个 `return` 且类型不一致，或者逻辑非常复杂，手动指定可以增加代码清晰度。

------

**4. 语句体 `{}` (Function Body)**

这里编写具体的逻辑。需要注意一个特殊的关键字：**`mutable`**。

默认情况下，**按值捕获**的变量在 Lambda 内部是 `const` 的。如果你想修改这个副本，必须加 `mutable`：

```c++
int x = 10;
auto f = [x]() mutable { 
    x++; // 如果不加 mutable，这里会报错
    std::cout << "Inside: " << x << std::endl; 
};
f();
std::cout << "Outside: " << x << std::endl; // 依然是 10，因为是按值捕获的副本
```

------

**5. 为什么 Lambda 改变了 C++？**

在没有 Lambda 之前，我们要给 `std::sort` 传一个自定义排序规则，得大费周章写一个类或全局函数：

**以前（繁琐）：**

```c++
struct Compare {
    bool operator()(int a, int b) { return a > b; }
};
std::sort(v.begin(), v.end(), Compare());
```

**现在（清爽）：**

```c++
std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
```

------

**💡 深度避坑指南**

1. **生命周期陷阱**：使用 `[&]` 引用捕获时，要确保 Lambda 执行时，外部变量还没有被销毁。如果是异步回调，建议使用 `[=]` 拷贝。
2. **捕获 `this` 的隐患**：在类成员函数中使用 `[=]` 默认会捕获 `this` 指针。如果对象析构后 Lambda 才被调用，会导致崩溃。

#### 1.4.1.3 智能指针 

* **shared_ptr**

* **weak_ptr**

##### 1.4.1.3.1 shared_ptr（共享智能指针）

`shared_ptr` 允许多个指针指向同一个对象。它内部维护了一个**引用计数 (Reference Counting)**。

- **工作原理**：

  - 当一个新的 `shared_ptr` 指向该对象时，计数 **+1**。
  - 当一个 `shared_ptr` 析构或重指向别处时，计数 **-1**。
  - 当计数变为 **0** 时，自动调用 `delete` 释放内存。

- **核心语法**：

  ```c++
  // 推荐使用 make_shared，效率更高且更安全
  auto p1 = std::make_shared<int>(100); 
  
  {
      auto p2 = p1; // 计数变为 2
      std::cout << p1.use_count() << std::endl; 
  } // p2 离开作用域，计数变回 1
  
  // p1 离开作用域或手动 reset，计数变 0，内存释放
  ```

------

##### 1.4.1.3.2 weak_ptr（弱引用智能指针）

`weak_ptr` 是为了配合 `shared_ptr` 而引入的“观察者”。它指向由 `shared_ptr` 管理的对象，但**不占用引用计数**。

**1. 为什么要用它？（解决循环引用）**

如果两个对象互相持有对方的 `shared_ptr`，它们的引用计数永远不会降为 0，导致内存泄漏。

```c++
struct B;
struct A { std::shared_ptr<B> ptrB; };
struct B { std::shared_ptr<A> ptrA; };

// 这种情况下，A 和 B 永远无法被析构
```

**解决方案**：将其中一个改为 `weak_ptr`。

**2. 如何使用？**

由于 `weak_ptr` 不增加计数，它指向的对象可能已经销毁。所以它不能直接通过 `->` 访问，必须先“提升”为 `shared_ptr`。

```c++
std::weak_ptr<int> wp = p1;

if (auto sp = wp.lock()) { // lock() 尝试提升，成功返回 shared_ptr，失败返回 nullptr
    std::cout << *sp << std::endl;
} else {
    std::cout << "对象已销毁" << std::endl;
}
```

------

**3. 性能与对比**

| **特性**     | **shared_ptr**         | **weak_ptr**                    |
| ------------ | ---------------------- | ------------------------------- |
| **引用计数** | 增加强引用计数         | 增加弱引用计数 (不影响对象销毁) |
| **内存管理** | 负责对象的生命周期     | 仅观察，不负责生命周期          |
| **访问方式** | 直接使用 `*` 或 `->`   | 必须通过 `lock()` 提升后访问    |
| **典型场景** | 多个对象共享同一份资源 | 缓存、观察者模式、解决循环引用  |

------

**4. 补充：unique_ptr（独占智能指针）**

虽然你没列出，但它是最常用的。它**禁止拷贝**，保证同一时间只有一个指针指向该资源。

```c++
std::unique_ptr<int> u1 = std::make_unique<int>(10);
// std::unique_ptr<int> u2 = u1; // 错误！禁止拷贝
std::unique_ptr<int> u2 = std::move(u1); // 正确，转移所有权
```

**总结建议**

1. **优先使用 `unique_ptr`**：如果不需要共享，它是开销最小、安全性最高的。
2. **需要共享时用 `shared_ptr`**：比如在多个对象间传递配置信息。
3. **防止死锁用 `weak_ptr`**：父子结构或双向链表中，从属方通常设为 `weak_ptr`。

#### 1.4.1.4 容器 

##### 1.4.1.4.1 unordered_set 和 unordered_map – 哈希容器

`unordered_set` 和 `unordered_map` 的核心是**哈希表 (Hash Table)**。

- **性能**：理想情况下，插入、删除、查询都是 $O(1)$。
- **代价**：内存消耗比 `set/map` 大（需要维护桶 `buckets` 和哈希表结构）。
- **无序性**：遍历时，元素的顺序是随机的，且随着插入新元素，顺序可能会发生变化（重新哈希）。

##### 1.4.1.4.3 与 set , map 的对比 

| **特性**          | **std::map / set**                     | **std::unordered_map / set**        |
| ----------------- | -------------------------------------- | ----------------------------------- |
| **底层实现**      | **红黑树** (自平衡二叉搜索树)          | **哈希表** (开链法/桶结构)          |
| **查找复杂度**    | $O(\log n)$ (稳定)                     | 平均 $O(1)$，最坏 $O(n)$            |
| **元素顺序**      | **严格有序** (默认从小到大)            | **无序**                            |
| **对 Key 的要求** | 必须定义 `operator<` (比较大小)        | 必须定义 `hash` 函数和 `operator==` |
| **适用场景**      | 需要按顺序遍历、寻找范围 (Range-query) | 只关心快速查找，不关心顺序          |

##### 1.4.1.4.4 array 

**不能扩容, 一定要知道数组的容量在用它 , 不如 vector 灵活**

它的存在是为了**替代 C 风格的原生数组** `int arr[N]`。

- **优势**：
  1. **性能**：它分配在**栈 (Stack)** 上，而不是堆上，没有动态内存分配的开销。
  2. **安全**：支持 `.at()` 越界检查，支持迭代器，可以使用 `for_each`。
  3. **零开销**：它的体积和原生数组完全一样，不会像 `vector` 那样额外存储容量 (capacity) 信息。
- **格言**：如果你知道数组的大小且**永远不会变**，用 `std::array`；如果你不确定，永远选 `std::vector`。

##### 1.4.1.4.5 forward_list 

**forward_list 是 轻量级链表**

它是为了极致的**空间效率**而设计的。

- **特点**：每个节点只保存指向“下一个”的指针（没有 `prev` 指针）。
- **对比 `list` (双向链表)**：
  - `list` 每个节点比 `forward_list` 多出一个指针的开销。
  - 在嵌入式系统或需要管理数百万个小节点的场景下，`forward_list` 能省下大量内存。
- **限制**：只能往后走，不能往回走。因此它没有 `.size()` 方法（因为计算长度需要遍历整个链表，开销为 $O(n)$，不符合 C++ “不为不使用的东西付费”的原则）。

#### 1.4.1.5 C++ 语言级别的多线程 

**c++ 语言级别支持多线程编程 , 以前在 windows 上是调用 createThread , Linux 下调用 pthread_create , 现在 thread 库支持 std::thread t; 创建线程对象  **

### 1.4.2 多线程编程 

#### 1.4.2.1 线程内容

* **怎么创建启动一个线程**

  **ans : `std::thread t(); 利用 thread 定义一个线程对象 , 然后再传入线程参数`**

  ```c++
  void threadHandle1() {
      std::cout << "hello handle1" << std::endl;
  }
  
  int main() {
      std::thread t1(threadHandle1);
  }
  ```

* **子线程何时结束**

  **ans : `当子线程函数运行结束 , 子线程结束`**

* **主线程如何处理子线程**

  * **t1.join() –  `等待 t1 线程结束 , 当前线程继续往下运行`**
  * **t1.detach() –  `把 t1 线程设置为分离线程 , 主线程结束 , 整个子线程都自动结束了 , 如果 main 函数执行完毕退出了 , 操作系统会杀死所有进程 , 这意味着即便子线程没有运行完 , 这个运行过程也会被暴力掐断 , 这通常导致资源泄露或者数据破坏 `**

* **主线程一定要等待子线程结束吗**

  **ans : `主线程运行结束之后 , 不能够存在还在运行未结束的子线程 , 否则会抛出异常 `**

#### 1.4.2.2 线程代码 thread 分析 

**包含 join() , detach() , sleep_for()**

```c++
#include "00. head file.hpp"

void threadHandle1(int time) {
    // 让子线程睡眠两秒
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "hello thread1!" << std::endl;
}

void threadHandle2(int time) {
    // 让子线程睡眠两秒
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "hello thread2!" << std::endl;
}

int main() {
    // 定义了一个线程对象 , 传入一个线程函数 
    std::thread t1(threadHandle1 , 2); // 在构造函数里面封装了
    std::thread t2(threadHandle2 , 3);
    // 主线程等待子线程结束 , 主线程继续往下运行 
    //t1.join();
    //t2.join();
    // 把子线程设置为分离线程
    t1.detach();
    t2.detach();
    std::cout << "main thread done!" << std::endl;

    // 主线程运行完成 , 查看如果当前进程还有wei
    return 0;
}
```

****

**1. 线程的创建与执行**

- **构造即启动**：`std::thread t1(threadHandle1, 2);` 这行代码一旦执行，子线程 `t1` 就会立即开始运行（进入就绪/运行态），并不会等待主线程的后续指令。
- **参数传递**：`thread` 的构造函数利用了我们之前提到的**可变参模板**，可以透明地将参数 `2` 传递给 `threadHandle1`。

------

**2. 核心争端：`join()` vs `detach()**`

你在代码中注释掉了 `join()` 而选择了 `detach()`，这两者的行为截然不同：

| **方式**       | **行为**                                                   | **结果**                                           |
| -------------- | ---------------------------------------------------------- | -------------------------------------------------- |
| **`join()`**   | 主线程会在此**阻塞（等待）**，直到子线程运行结束。         | 保证子线程的任务全部完成，主线程才继续往下走。     |
| **`detach()`** | 主线程与子线程**分离**，子线程驻留后台运行（守护线程化）。 | 主线程不再负责子线程的死活，子线程由操作系统接管。 |

------

**3. 运行结果预测与风险分析**

根据你目前的代码逻辑，运行结果大概率如下：

Plaintext

```
main thread done!
(程序直接结束，没有输出 hello thread1/2)
```

**为什么子线程没输出？**

1. **进程生存期限制**：在 C++ 中，`main` 函数返回（`return 0`）意味着整个**进程**的结束。
2. **强制销毁**：当进程退出时，操作系统会强制回收该进程申请的所有资源，包括正在后台 `detach` 运行的子线程。
3. **时间差**：主线程运行到 `return 0` 只需要几微秒，而子线程需要 `sleep` 2~3 秒。在子线程还没来得及打印之前，它就已经随着进程的毁灭而“丧命”了。

------

**4. 代码中的潜在隐患**

- **`join` 或 `detach` 必须选其一**：`std::thread` 对象在析构时，如果既没有被 `join` 也没有被 `detach`（即处于 `joinable` 状态），程序会直接调用 `std::terminate()` 导致**崩溃**。
- **资源竞争**：如果子线程在 `detach` 后尝试访问主线程栈上的局部变量，而主线程已经退出，会产生非法访问。

------

**5. 改进建议**

如果你想看到子线程的输出，通常有两种做法：

1. **使用 `join()`（最推荐）**：确保逻辑完整。

   ```c++
   t1.join();
   t2.join();
   // 此时会依次等待 2s 和 3s，看到输出后再结束主线程
   ```

2. **让主线程“等一会儿”（临时调试用）**：

   在 `return 0` 之前加上 `std::this_thread::sleep_for(std::chrono::seconds(5));`。

**总结**

- `join` 是“负责任”的表现：等孩子干完活再回家。
- `detach` 是“放任自流”：孩子去干活了，我先回家睡觉，孩子干完没干完我不管了。但在 C++ 里，**“家”如果拆了（进程结束），孩子也就没了**。

#### 1.4.2.2 线程代码 mutex 互斥锁 和 lock_guard 分析 

##### 1.4.2.2.1 mutex 的介绍 

* **mutex 确保 `同一时刻只有一个线程能访问特定的共享资源 , 是解决竞态条件的核心`**

* **竞态条件 : 多线程执行的结果是一致的 , 不会随着 CPU 对线程不同的调用顺序 , 而产生不同的运行结果 , 指的是程序的执行结果取决于 ,线程执行的顺序或时序。如果 CPU 先调度 A 再调度 B 结果是 10，先 B 再 A 结果是 8，这就是存在竞态条件。**

* **临界区 :  mutex 的两个基本操作是 lock() 和 unlock() , 被这两个操作包围的代码块就被称为`临界区` , 保证了操作的`原子性`**

* **线程的原子性 : 线程间的步骤被打包为一个整体 , 要么都做 , 要么都没做 , mutex 就是干这个的** 

* **死锁 (Deadlock) —— Mutex 的副作用**

​	必须警惕它的负面效应。当多个线程互相等待对方手中的锁时，就会发生**死锁**。

​	**死锁产生的四个必要条件：**

1. **互斥**：资源一次只能被一个线程使用（`mutex` 本身特性）。
2. **占有且等待**：线程抓着一个锁，同时去申请另一个锁。
3. **不可剥夺**：锁只能由占有它的线程释放，别人不能强抢。
4. **循环等待**：线程 A 等 B 的锁，B 等 A 的锁。

​	**避坑指南：**

​	**加锁顺序一致**：如果所有线程都先拿 `mtx1` 再拿 `mtx2`，就不会死锁。

​	**使用 `std::lock`**：C++11 提供了 `std::lock(mtx1, mtx2)`，它可以一次性锁住多个互斥量，并内部处理好顺序，防止死锁。

##### 1.4.2.2.2 lock_guard 介绍 

* **定义**

  ```c++
  std::mutex mtx; // 全局的一个互斥锁 
  std::lock_guard<std::mutex> lck(mtx); 
  ```

* **这是一个栈上的局部对象 , 保证所有的线程都能释放锁,防止死锁问题的发生**

**1. 定义与本质**

`std::lock_guard` 是 C++11 引入的一个 **模板类**，它实现了 **RAII (资源获取即初始化)** 机制。

- **构造时**：它会调用 `mtx.lock()`，自动获取锁。
- **析构时**：当 `lck` 这个局部对象生命周期结束（比如函数返回、循环结束、或者碰到大括号 `}`），它的析构函数会自动调用 `mtx.unlock()`。

------

**2. 为什么它能防止死锁？**

在没有 `lock_guard` 的年代，我们必须手动执行 `mtx.unlock()`。这会带来巨大的安全隐患：

- **异常抛出**：如果代码在 `lock` 和 `unlock` 之间抛出了异常，`unlock` 就永远不会执行，导致锁被永久占用。
- **多分支返回**：如果函数有很多 `if...return`，你必须在每一个 `return` 之前都写一遍 `unlock()`，一旦漏掉一个，程序就死锁了。

**`lock_guard` 解决了这个问题**：因为它是栈上的局部对象，无论是因为正常运行结束、`return` 还是因为异常导致栈回退，**析构函数一定会被调用**，锁一定会被释放。

------

**3. 核心限制（与 `unique_lock` 的区别）**

`lock_guard` 是为了极致的性能和简洁设计的，所以它非常“死板”：

- **不可复制/不可移动**：它禁用了拷贝构造和移动构造。
- **生命周期固定**：一旦锁定，直到析构前你无法手动解锁。
- **功能单一**：它不支持条件变量（`std::condition_variable`）的 `wait` 操作（因为 `wait` 需要在睡眠时手动解锁，醒来时重新加锁）。

------

**4. 使用示例（作用域控制）**

有时候我们不希望锁住整个函数，只希望锁住一小段代码，可以利用**局部大括号**：

```c++
void dataProcess() {
    // 准备工作（不需要锁）
    prepareData();

    {
        std::lock_guard<std::mutex> lck(mtx); // 锁开始
        // 临界区：访问共享资源
        shared_list.push_back(data);
    } // lck 在这里析构，自动解锁

    // 后续处理（不需要锁）
    logCompletion();
}
```

------

**5. 总结对比**

| **特性**         | **std::lock_guard**          | **std::unique_lock**                   |
| ---------------- | ---------------------------- | -------------------------------------- |
| **性能**         | 极高（几乎零开销）           | 略低（内部需要维护锁状态标志）         |
| **灵活性**       | 低（构造锁，析构放）         | 高（可手动 lock/unlock，支持延迟加锁） |
| **配合条件变量** | **不支持**                   | **支持**                               |
| **推荐场景**     | 简单的、确定范围的临界区保护 | 需要中途解锁、或配合 `cv` 使用的场景   |

**💡 深度纠错**

你在笔记中提到“保证所有的**进程**都能释放锁”，这里需要微调一下术语：

- 在 C++ 这种多线程语境下，通常指的是**线程 (Threads)**。
- `std::mutex` 主要是用于同一个进程内的多个线程之间互斥。如果是跨进程（Processes）的互斥，通常需要使用操作系统提供的信号量（Semaphores）或命名互斥量。

##### 1.4.2.2.3 火车窗口卖票问题 

###### 1.4.2.2.3.1 问题引入

```c++
#include "00. head file.hpp"

int ticketcont = 100;
void sellTicket(int index) {
    while(ticketcount > 0) {
        std::cout << "window " << index << " is selling " << ticketcount << " tickets" << std::endl;
        ticketcount --;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
int main() {
    std::list<std::thread> tlist;
    for(int i = 1 ; i <= 3 ; ++ i) {
        tlist.push_back(std::thread(sellTicket , i));
    }
    for(std::thread &t : tlist) {
        t.join();
    }
    
    std::cout << "ticket is empty" << std::endl;
    return 0;
}
```

* **这个时候 , 因为while 的循环限制 , 三个线程进行售票 , 可不会管什么先后顺序 , 同一张票可以售出多次 , 很明显不可能 , 所以需要一个互斥锁 , 提到互斥锁的作用就是让同一时刻只能有一个线程来访问这个共享资源 , 这时 ticketcount - - 之后 , 就不会重复了 , 底层是这样 **

  ```assembly
  move eax , ticketcount // 先将ticketcount 的值放到寄存器里
  sub eax , 1 // 寄存器存储的值减去1 , 但是下一步可能还没到 , 另一个 ticketcount 的值就又被复用了,还是100 
  move ticketcount , eax 
  ```

###### 1.4.2.2.3.2 引入 mutex 之后 

```assembly
#include "00. head file.hpp"

int ticketcont = 100;
std::mutex mtx;
void sellTicket(int index) {
	mtx.lock();
    while(ticketcount > 0) {
        std::cout << "window " << index << " is selling " << ticketcount << " tickets" << std::endl;
        ticketcount --;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    mtx.unlock();
}
int main() {
    std::list<std::thread> tlist;
    for(int i = 1 ; i <= 3 ; ++ i) {
        tlist.push_back(std::thread(sellTicket , i));
    }
    for(std::thread &t : tlist) {
        t.join();
    }
    
    std::cout << "ticket is empty" << std::endl;
    return 0;
}
```

* **为什么 mutex 要定义在全局区域内 ?**

  **ans : `因为要让所有的线程都访问的到`**

* **加上这个锁又会出现问题– 死锁问题 ;  如果说这个 lock() 和 unlock() 区域内抛出了异常 , 那么这个锁就到不了 unlock() , 释放不了这个锁 , 这个锁就会被永久的占用 ,或者是 return 也会导致这个问题 , 所以 , 我们应该该将 mtx 放在 while 循环里 , 每次更新一下 **

###### 1.4.2.2.3.3 引入 lock_guard 

```c++
#include "00. head file.hpp"

int ticketcont = 100;
std::mutex mtx;
void sellTicket(int index) {
    while(ticketcount > 0) {
    	mtx.lock();
        std::cout << "window " << index << " is selling " << ticketcount << " tickets" << std::endl;
        ticketcount --;
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
int main() {
    std::list<std::thread> tlist;
    for(int i = 1 ; i <= 3 ; ++ i) {
        tlist.push_back(std::thread(sellTicket , i));
    }
    for(std::thread &t : tlist) {
        t.join();
    }
    
    std::cout << "ticket is empty" << std::endl;
    return 0;
}
```

* **这时 , mtx 的两个操作虽然被放到了循环里 , 依然存在问题  , 如果 A 进入线程 , 此时只剩一张票的话, A 会售票 , 此时票数为0 , 但是线程 B 要慢 A 一步 , 所以就会售出 -1 的票 , 这很明显不可能 , 所以要加上条件判断变成这样 **

```c++
#include "00. head file.hpp"

int ticketcont = 100;
std::mutex mtx;
void sellTicket(int index) {
    while(ticketcount > 0) {
    	mtx.lock();
        if(ticketcount > 0) {
            std::cout << "window " << index << " is selling " << ticketcount << " tickets" << std::endl;
        	ticketcount --;
        }
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
int main() {
    std::list<std::thread> tlist;
    for(int i = 1 ; i <= 3 ; ++ i) {
        tlist.push_back(std::thread(sellTicket , i));
    }
    for(std::thread &t : tlist) {
        t.join();
    }
    
    std::cout << "ticket is empty" << std::endl;
    return 0;
}
```

* **问题又来 , 出现了 条件判断语句 , 加入说有多个条件判断 , 有的有 return , 那么必须在每个 return 前加上 mtx.unlock() 操作 , 否则会造成死锁问题 , 不太方便灵活 , 但是若是使用 lock_guard 的话 , 类似智能指针的原理 , 自动对于线程进行管控就很方便 , 它类似于 scoped_ptr , 拷贝和赋值重载是不能的, 只能进行简单操作 **

```c++
#include "00. head file.hpp"

int ticketcont = 100;
std::mutex mtx;
void sellTicket(int index) {
    while(ticketcount > 0) {
    	std::lock_guard<std::mutex> lck(mtx);
        if(ticketcount > 0) {
            std::cout << "window " << index << " is selling " << ticketcount << " tickets" << std::endl;
        	ticketcount --;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
int main() {
    std::list<std::thread> tlist;
    for(int i = 1 ; i <= 3 ; ++ i) {
        tlist.push_back(std::thread(sellTicket , i));
    }
    for(std::thread &t : tlist) {
        t.join();
    }
    
    std::cout << "ticket is empty" << std::endl;
    return 0;
}
```

* **同时还有 std::unique_lock<std::mutex> lck(mtx);  它是类似于 unique_ptr , 虽然左值拷贝和赋值重载被限制 , 但是右值还可以 , 它的操作是 lck.lock() , lck.unlock()**

```c++
#include "00. head file.hpp"

/*
c++ thread -- 模拟车站三个窗口卖票程序
*/

/*
多线程程序
竞态条件 : 多线程执行的结果是一致的 , 不会随着 CPU 对线程不同的调用顺序 ,而产生不同的运行结果
*/

int ticketcount = 100; // 车站有100张车票 , 由三个窗口一起卖票 
std::mutex mtx; // 全局的一把互斥锁 
// 模拟卖票的线程函数  lock_guard unique_lock
void sellTicket(int index) {
    while(ticketcount > 0) { // ticketCount = 1 锁 + 双重判断
        //mtx.lock();
        {
            // std::lock_guard<std::mutex> lock(mtx); //栈上的局部对象  , 保证所有线程都能释放锁 ,防止死锁问题的发生
            std::unique_lock<std::mutex> ulock(mtx);
            ulock.lock();
            if(ticketcount > 0) {
                // 临界区代码段 => 原子操作 => 线程间互斥操作了 => mutex  
                std::cout << "window " << index << " is selling " << ticketcount << " tickets" << std::endl;
                ticketcount --; // 不是一个安全的操作 , 需要互斥
            }
            ulock.unlock();
        }
        //mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
int main() {


    std::list<std::thread> tlist;
    for(int i = 0 ; i <= 3 ; ++ i) {
        tlist.push_back(std::thread(sellTicket , i)); 
    }

    for(std::thread &t : tlist) {
        t.join();
    }

    std::cout << "tickets are empty" << std::endl;
    return 0;
}
```

#### 1.4.2.3 线程间的同步通信问题

##### 1.4.2.3.1 多线程编程的两个问题 

1. 线程间的互斥问题 –  线程运行时存在竞态条件, 临界区代码段必须保证原子操作的正常进行 , 用互斥锁 mutex , 或者轻量级的无锁实现 CAS 
2. **线程间的同步通信问题 , 以 生产者 , 消费者模型为例 , 生产商品是生产一个消费一个 , 不能透支,但是线程之间的执行可不是你来我往的 , 是几乎一起执行 , 所以这个时候就需要同步通信,告诉我什么时候消费 , 告诉你什么时候生产**

##### 1.4.2.3.2 生产者 - 消费者模型的实现(基于队列)

```c++
#include "00. head file.hpp" 

/*
c++ 所有的 STL 容器都不是安全的 
c++ 多线程编程 - 线程间的同步通讯机制  
多线程编程有两个问题 : 
1. 线程间的互斥 
线程在运行时存在竞态条件 => 临界区代码段(竞态条件发生的区域) => 必须保证原子操作 => 用互斥锁 mutex , 轻量级的无锁实现 CAS 
strace ./a.out mutex => pthread_mutex_t
2. 线程间的同步通信  
生产者 , 消费者线程模型 
*/

std::mutex mtx;
std::condition_variable cv;
class Queue {
    public:
    	void put(int val) {
            std::unique_lock<std::mutex> lck(mtx);
            while(!que.empty()) {
                // que 不为空 , 生产者应该通知消费者去消费 , 消费完了再继续生产
                // 1. 生产者线程应该进入等待状态 , 2. 并且把 mtx 互斥锁释放掉
                cv.wait(lck);
            }
            que.push(val);
            cv.notify_all(); // 通知其他的所有线程 , 我生产了一个商品, 你们消费去吧
            std::cout << "producer produces :" << val << "product" << std::endl;
        }
    	int get() {
            std::unique_lock<std::mutex> lck(mtx);
            while(que.empty()) {
                // que为空 , 消费者应该通知生产者该生产商品了
                // 1. 消费者线应该进入等待状态 , 2. 并且把 mtx 互斥锁释放掉
                cv.wait(lck); 
            }
            int val = que.front();
            cv.notify_all(); // 通知其他生产者 , 我这商品用完了 , 你们快生产
            que.pop();
            std::cout << "consumer consumes : " << val << " object" << std::endl;
            return val;
        }
    private: 
    	std::queue<int> que;
};
void producer(Queue* que) {
    for(int i = 1 ; i <= 10 ; ++ i) {
        que->put(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
void consumer(Queue* que) {
    for(int i = 1 ; i <= 10 ; ++ i) {
        que->get();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
int main() {
    Queue que;
    std::thread(producer , &que);
    std::thread(consumer , &que);
    
    t1.join();
    t2.join();
    return 0;
}
```

###### 1.4.2.3.2.1 condition_variable 是什么

互斥锁解决的是“抢资源”的问题，而条件变量解决的是“按顺序协作”的问题。

------

**1. 核心操作：等待与通知**

条件变量主要依靠两个核心动作来协调线程：

- **`wait()` (等待)**：
  1. 自动释放已经持有的 `unique_lock`。
  2. 将当前线程挂起（阻塞），进入等待队列，不消耗 CPU。
  3. 当被唤醒并重新获得锁后，`wait` 结束。
- **`notify_one() / notify_all()` (通知)**：
  - `notify_one`：唤醒等待队列中的**一个**线程。
  - `notify_all`：唤醒等待队列中的**所有**线程。

------

**2. 标准使用模版（生产者-消费者逻辑）**

条件变量必须配合 `std::unique_lock<std::mutex>` 使用。

**消费者（等待者）：**

```c++
std::unique_lock<std::mutex> lck(mtx);
// 使用 while 防止“虚假唤醒”
while (!condition_is_met) { 
    cv.wait(lck); 
}
// 此时已经拿到了锁，且条件满足，开始处理逻辑
```

**生产者（通知者）：**

```c++
{
    std::lock_guard<std::mutex> lck(mtx);
    // 改变共享资源（生产商品）
    condition_is_met = true;
} 
// 改变后发出通知
cv.notify_all(); 
```

------

**3. 三个为什么（深度解析）**

**Q1：为什么必须配合 `unique_lock`？**

因为 `wait()` 内部有一个“先解锁、再睡觉、醒了再加锁”的复杂过程。`lock_guard` 太过死板，不支持中途手动解锁，所以无法胜任。

**Q2：为什么必须用 `while` 循环检查条件？**

这就是著名的**虚假唤醒（Spurious Wakeup）\**问题。 操作系统可能会在没有任何线程调用 `notify` 的情况下意外唤醒你的线程。如果用 `if`，线程醒来后会直接执行后续逻辑，可能导致在“仓库为空”的情况下尝试消费，从而程序崩溃。使用 `while` 能确保线程醒来后\**再次验证**条件。

**Q3：`wait` 的时候锁去哪了？**

当你调用 `cv.wait(lck)` 的一瞬间，它会**原子地**释放锁。这非常关键！如果不释放锁，生产者就进不来，也就没法生产东西并通知你，结果就是全家死锁。

------

**4. 总结对比**

| **机制**                 | **解决的问题**                   | **类比**                               |
| ------------------------ | -------------------------------- | -------------------------------------- |
| **`std::mutex`**         | **互斥**：防止多个线程同时改数据 | 厕所的门锁（一次只能进一个人）         |
| **`condition_variable`** | **同步**：协调线程运行的先后顺序 | 餐厅的叫号器（没饭时等着，饭好了叫你） |

**一句话总结**：`mutex` 保证了操作的**原子性**，而 `condition_variable` 保证了逻辑的**时序性**。

#### 1.4.2.4 原子操作

##### 1.4.2.4.1 为什么使用原子操作 

**因为互斥锁是比较重的 , 临界代码区要做的事情多且复杂,有点大炮打蚊子的意思 , 我们用不了那么多的操作 , 所以就需要原子操作**

你的总结非常到位，尤其是**“大炮打蚊子”**这个比喻，精准地描述了**互斥锁（Mutex）**与**原子操作（Atomic）**在性能开销上的巨大差异。

我们可以从底层的物理实现和系统开销两个维度，把这个知识点补全：

------

**1. 为什么说互斥锁“重”？ (The Overhead of Mutex)**

当你调用 `std::mutex::lock()` 时，如果锁被占用，会发生以下一系列昂贵的动作：

1. **从用户态切换到内核态**：互斥锁是由操作系统内核管理的。
2. **线程挂起（Suspend）**：操作系统会将你的线程从 CPU 上取下来，放入等待队列。
3. **上下文切换（Context Switch）**：CPU 必须保存当前线程的所有寄存器状态，然后去跑别的线程。
4. **再次唤醒**：当锁释放时，上述过程反向再走一遍。

这一套流程下来，可能消耗数千个 CPU 周期。如果你的代码只是为了给一个 `int` 加 1（这只需要 1 个周期），那剩下的几千个周期全被浪费在“管理”上了。

------

**2. 为什么原子操作“轻”？ (The Efficiency of Atomic)**

原子操作（如 `std::atomic`）不需要操作系统的介入，它直接利用 **CPU 硬件指令**：

- **CAS 指令 (Compare And Swap)**：这是硬件层面的“原子指令”。它在修改内存前会检查：“现在的值还是我刚才看到的那个吗？” 如果是，直接改；如果不是，说明别人动过，我再试一次。
- **无需挂起**：线程永远不会进入睡眠态，也就没有内核切换和上下文切换。
- **用户态完成**：所有的操作都在 CPU 寄存器和缓存层面完成。

------

**3. 适用场景对比**

| **维度**     | **互斥锁 (Mutex)**                 | **原子操作 (Atomic)**                |
| ------------ | ---------------------------------- | ------------------------------------ |
| **底层实现** | 操作系统提供的信号量/锁机制        | CPU 硬件指令 (CAS)                   |
| **保护范围** | 可以保护**一整段逻辑**（多行代码） | 只能保护**单个变量**（自增、赋值等） |
| **开销**     | 高（涉及内核态切换）               | 极低（纯硬件指令）                   |
| **编程难度** | 容易（逻辑直观）                   | 较高（无锁编程容易出内存顺序 Bug）   |
| **形象比喻** | 整个银行柜台只为你一个人服务       | 自动取款机 (ATM) 快速自助操作        |

------

**4. 笔记补充：什么时候必须回退到 Mutex？**

虽然原子操作很快，但它不是万能的。以下场景**必须**使用互斥锁：

1. **复杂逻辑**：如果你要同时修改 `vector`、`map` 和一个 `int`，原子操作无法保证这三个动作的整体原子性。
2. **长路径保护**：如果你的临界区里有文件 IO、网络访问等耗时操作。
3. **需要等待通知**：原子操作无法实现“没货了就让线程睡觉”的逻辑（这时候需要 `condition_variable`）。

------

**💡 总结**

**原子操作是追求极致性能的武器。** 在编写高性能库（如 `muduo` 或线程池底层计数）时，凡是能用 `std::atomic` 解决的简单加减和布尔标志，都不要去碰 `std::mutex`。

##### 1.4.2.4.2 原子操作示范

```c++
#include "00. head file.hpp"

/*
窗口卖票 int count = 100;

count ++ ; 和 count -- ; 都是不安全的操作 ,可能要减去两个但是只减去一个

lock_guard<std::mutex> guard(mtx);
count -- ;

互斥锁是比较重的 , 临界区代码做的事情稍稍复杂, 多 , 大炮打蚊子
从系统理论来说 : CAS 来保证上面 ++ -- 操作的原子特性就足够了 , 无锁操作 

*/

volatile std::atomic_bool isReady = false; // volatile 是防止多线程对共享变量进行缓存 ,访问的都是原始的值 
volatile std::atomic_int mycount = 0;

void task() {
    while(!isReady) {
        std::this_thread::yield(); // 线程出让当前的CPU时间片 , 等待下一次调度
    }

    for(int i = 0 ; i < 100 ; ++ i) {
        mycount ++;
    }
}
int main() {
    std::list<std::thread> tlist;
    for(int i = 0 ; i < 10 ; ++ i) {
        tlist.push_back(std::thread(task));
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));
    isReady = true;

    for(std::thread &t : tlist) {
        t.join();
    }

    std::cout << "count : " << mycount << std::endl;

    return 0;
}
```

###### 1.4.2.4.2.1 代码分析

**1. 核心技术：`std::atomic` (原子操作)**

这是代码的灵魂。你使用了 `std::atomic_int` 替代了普通的 `int`。

- **原理**：它底层依赖 CPU 的 **CAS (Compare And Swap)** 指令。
- **大炮打蚊子**：正如你注释所写，对于简单的 `++` 或 `--`，互斥锁（`mutex`）涉及内核态切换，开销巨大。原子操作在用户态硬件层面完成，速度极快。
- **结果保证**：即使 10 个线程同时对 `mycount` 执行加法，也不会出现“覆盖”现象，最终结果一定是准确的 **1000**（10 线程 × 100 次）。

------

**2. 关键修饰符：`volatile` 的误区与真相**

你在代码中使用了 `volatile`，这是一个非常经典的深度考点：

- **你的初衷**：防止编译器将变量缓存到寄存器，确保每次都从内存读取。
- **现代 C++ 现实**：在 C++11 及更高版本中，**`std::atomic` 已经自带了内存可见性保证**（内存屏障）。
  - **结论**：在处理多线程同步时，**`std::atomic` 已经足够**，通常不需要额外加 `volatile`。`volatile` 在现代 C++ 中主要用于硬件驱动编程（防止编译器优化掉对内存映射 IO 的读写），它并不保证原子性，也不保证线程间的同步顺序。

------

**3. 线程协调机制：`yield` 与 信号通知**

代码实现了一个简单的“发令枪”模型：

- **忙等 (Busy-wait) 与 `yield()`**：
  - 子线程在 `isReady` 为 `false` 时不会直接退出，而是通过 `while` 循环不断检查。
  - `std::this_thread::yield()`：这行代码非常关键。它告诉操作系统：“我现在没事干，把 CPU 让给别人吧”。这比纯 `while(true)` 节省资源，比 `sleep` 响应更快。
- **主线程控制**：主线程 `sleep` 3 秒后将 `isReady` 置为 `true`，相当于扣动了发令枪响，10 个子线程瞬间冲出 `while` 循环开始工作。

------

**4. 重点总结（笔记建议）**

| **知识点**        | **作用**                      | **备注**                                              |
| ----------------- | ----------------------------- | ----------------------------------------------------- |
| **`std::atomic`** | 解决竞态条件 (Race Condition) | 现代 C++ 无锁编程的首选，性能极高。                   |
| **`yield()`**     | 提高系统吞吐量                | 出让时间片，避免某个线程空转死占 CPU。                |
| **信号通知**      | 线程间同步                    | 通过一个原子布尔值 `isReady` 控制多个线程的启动时机。 |
| **`join()`**      | 回收线程资源                  | 确保主线程在子线程全部干完活（加完数）后再输出打印。  |

------

**💡 深度思考：下一步进化**

虽然你的代码已经很优秀，但在工业级开发中（比如你提到的 `muduo` 库），如果需要让线程“等待”，我们通常不推荐 `while + yield`，因为：

1. **CPU 占用**：即便有 `yield`，线程依然处于就绪态，会频繁被唤醒检查条件。
2. **更优解**：使用 **`std::condition_variable`**。它可以让线程彻底进入休眠，直到主线程发出 `notify`。

****

## 1.5 设计模式 

### 1.5.1 单例模式 

* **单例模式 : `一个类不管创建多少次对象 , 永远只能得到该类型的一个对象的实例 , 也就是说 , 创建了三个对象 , 打印这三个对象的地址一样的`**
* **常用到单例模式的地方 : 日志模块 , 数据库等等**

#### 1.5.1.1 单例模式的实现思路 

1. **饿汉式单例模式要限制对象的实例 , 那就首先要对构造函数进行限制 , 不能无限制的访问 , 所以将构造函数设置为私有限制 **
2. **定义唯一一个类的实例对象**
3. **需要定义一个接口 , 用户通过这个接口获取实例对象**
4. **将拷贝构造函数以及赋值运算符重载函数 delete 掉 **

#### 1.5.1.1 饿汉式单例模式

* **饿汉式单例模式 : 还没有获取实例化对象 , 实例对象就已经产生了**

* **饿汉式单例模式的实现 **

  ```c++
  #include "00. head file.hpp" 
  
  class Singleton {
      public:
      	static Singleton* getInstance() {
              return &instance;
          }
      private:
      	static Singleton instance; // 定义唯一一个类的实例对象
     		Singleton() { // 构造函数初始化
              
          }
      	Singleton(const Singleton&) = delete;
      	Singleton& operator=(const Singleton&) = delete;
  }
  Singleton Singleton::instance;
  int main() {
      Singelton* p1 = Singelton::getInstance();
      Singelton* p2 = Singelton::getInstance();
      Singelton* p3 = Singelton::getInstance();
  
      std::cout << "p1 = " << p1 << " p2 = " << p2 << " p3 = " << p3 << std::endl;
  	return 0;
  }
  ```

* **饿汉式单例模式一定是线程安全的**

  * **ans : `因为饿汉式单例模式在还没有获取实例化对象的时候 , 实例对象就已经产生了 , 所以在调用函数的时候 , 这个资源是所有线程都共享的 , 是只读的 , 不存在竞态状态`**

* **为什么唯一的类的实例对象要定义称为静态成员**

  * **ans : `因为饿汉式单例模式是要还没有获取实例化对象的时候 , 实例对象就已经产生了 , 静态是类内声明 , 类外定义 , 位于 .data 段或者 .bss 段 , 会在没有获取实例化对象的时候 , 就产生实例对象 ; 其次 , static 属于类本身 , 被所有潜伏调用者共享 , 告诉编译器 , 我无论访问这个类多少次 , 这个成员在内存中只有那一个物理地址 , 再然后 , 静态成员函数内部只能访问静态成员。因为静态函数调用时没有 this 指针 ，它无法操作非静态成员。为了让这个“入口函数”能拿到那个唯一的实例，该实例必须也是 static 的。`**

#### 1.5.1.2 懒汉式单例模式

* **懒汉式单例模式 : 唯一的实例对象 , 在第一次获取它的时候 , 才产生**

* **饿汉式单例模式的实现**

  * **第一次**

    ```c++
    #include "00. head file.hpp" 
    
    class Singleton {
      	public: 	
        	static Singleton* getInstance() {
                if(instance == nullptr) { // 直到第一次获取它的时候才产生 
                    instance = new Singleton();
                }
                return instance;
            } 
        
        private: 
        	static Singleton* instance; // 定义为指针
        	Singleton() { // 限制私有成员构造函数 
                
            }
        	Singleton(const Singleton&) = delete;
        	Singleton& operator=(const Singleton&) = delete;
    };
    Singleton *Singleton::instance = nullptr; // 类外初始化
    int main() {
        Singelton* p1 = Singelton::getInstance();
        Singelton* p2 = Singelton::getInstance();
        Singelton* p3 = Singelton::getInstance();
    
        std::cout << "p1 = " << p1 << " p2 = " << p2 << " p3 = " << p3 << std::endl;
        return 0;
    }
    ```

    * **为什么定义为静态指针?**
      * **ans : `因为可以 new 对象 , 使其在第一次获取时候再产生`**
    * **什么是重入函数 ?**
      * **ans : `如果一个函数只有在多线程环境下运行 , 而且不会发生竞态条件 , 就是重入函数`**
    * **出现的问题**
      * **首先 , 再 instance new 对象的时候 , 做了三个步骤 , 内存开辟 , 构造对象 , 给instance 赋值 , 内存开辟永远是第一位没有问题 , 但是 第二步和第三步是没有关系的 , 在调用顺序上可能会调换顺序 , 那么假如现在有两个线程 , A线程进入 , 准备new 对象 , 但是还没有返回呢 , B线程还是空 , 准备去 new 对象 , 这样子 线程就是不安全的 , 需要用互斥锁锁一下**

  * **第二次 : 引入互斥锁**

    ```c++
    #include "00. head file.hpp" 
    
    std::mutex mtx;
    class Singleton {
      	public: 	
        	static Singleton* getInstance() {
                std::lock_guard<std::mutex> lck(mtx); 
                if(instance == nullptr) { // 直到第一次获取它的时候才产生 
                    instance = new Singleton();
                }
                return instance;
            } 
        
        private: 
        	static Singleton* instance; // 定义为指针
        	Singleton() { // 限制私有成员构造函数 
                
            }
        	Singleton(const Singleton&) = delete;
        	Singleton& operator=(const Singleton&) = delete;
    };
    Singleton *Singleton::instance = nullptr; // 类外初始化
    int main() {
        Singelton* p1 = Singelton::getInstance();
        Singelton* p2 = Singelton::getInstance();
        Singelton* p3 = Singelton::getInstance();
    
        std::cout << "p1 = " << p1 << " p2 = " << p2 << " p3 = " << p3 << std::endl;
        return 0;
    }
    ```

    * **出现的问题 , 这个锁的添加位置会造成锁的力度太大 , 就是说假如现在是单线程 , 他也要锁一下 ,完全没有必要 , 所以将锁写进判断条件里 , 然后利用 `锁 + 双重判断` 来完成这个操作**

  * **第三次**

    ```c++
    #include "00. head file.hpp" 
    
    std::mutex mtx;
    class Singleton {
      	public: 	
        	static Singleton* getInstance() {
                if(instance == nullptr) { // 直到第一次获取它的时候才产生 
                    std::lock_guard<std::mutex> lck(mtx);
                    if(instance == nullptr) {
                        instance = new Singleton();
                    }
                }
                return instance;
            } 
        
        private: 
        	static Singleton* instance; // 定义为指针
        	Singleton() { // 限制私有成员构造函数 
                
            }
        	Singleton(const Singleton&) = delete;
        	Singleton& operator=(const Singleton&) = delete;
    };
    Singleton *Singleton::instance = nullptr; // 类外初始化
    int main() {
        Singelton* p1 = Singelton::getInstance();
        Singelton* p2 = Singelton::getInstance();
        Singelton* p3 = Singelton::getInstance();
    
        std::cout << "p1 = " << p1 << " p2 = " << p2 << " p3 = " << p3 << std::endl;
        return 0;
    }
    ```

    * **每次写锁太麻烦 , 更轻量级的怎么写**

  * **第四次 – 优化 :**

    ```c++
    #include "00. head file.hpp" 
    
    class Singleton {
      	public: 	
        	static Singleton* getInstance() {
                static Singleton instance;
                return &instance;
            } 
        
        private: 
        	Singleton() { // 限制私有成员构造函数 
                
            }
        	Singleton(const Singleton&) = delete;
        	Singleton& operator=(const Singleton&) = delete;
    };
    int main() {
        Singelton* p1 = Singelton::getInstance();
        Singelton* p2 = Singelton::getInstance();
        Singelton* p3 = Singelton::getInstance();
    
        std::cout << "p1 = " << p1 << " p2 = " << p2 << " p3 = " << p3 << std::endl;
        return 0;
    }
    ```

    * **将唯一定义的类实例化对象定义成函数局部变量初始化 , 在汇编指令上已经自动添加线程互斥指令了**

### 1.5.2 工厂模式 

* **为什么需要工厂模式 ?**

  **ans : `主要是封装了对象的创建`**

#### 1.5.2.1 简单工厂模式 

* **把创建的对象封装到一个函数接口里面 , 通过调用不同的标示 , 来返回不同的对象 , 客户不用自己 new 对象 , 不用详细了解对象的创建过程 **

* ```c++
  #include "00. head file.hpp" 
  
  class Car {
    	public:
      	Car(std::string name) 
  			: _name(name) 
          {}
      	virtual void show() = 0; // 提供纯虚函数 , 便于派生类的重写
      protected:
      	std::string _name;
  };
  
  class Bmw : public Car {
    	public: 
      	Bmw(std::string name) 
              :Car(name) 
          {}
      	void show() {
              std::cout << "BMW Car : " << name << std::endl;
          } 
  };
  
  class Audi : public Car {
    	public: 
      	Audi(std::string name) 
              :Car(name) 
          {}
      	void show() {
              std::cout << "BMW Car : " << name << std::endl;
          } 
  };
  
  
  int main() {
     	Car* p1 = new Bmw("BMW X1"); // 在常规派生类中 , 需要记忆很多派生类的名字 , 自己 new 对象 , 不方便
      Car* p2 = new Audi("AUDI a6");
      return 0;
  }
  ```

* **将 audi 和 bmw 封装到一个 SimpleFactory 中 , 不用自己 new 对象 , 方便很多**

  ```c++
  #include "00. head file.hpp" 
  
  class Car {
    	public:
      	Car(std::string name) 
  			: _name(name) 
          {}
      	virtual void show() = 0; // 提供纯虚函数 , 便于派生类的重写
      protected:
      	std::string _name;
  };
  
  class Bmw : public Car {
    	public: 
      	Bmw(std::string name) 
              :Car(name) 
          {}
      	void show() {
              std::cout << "BMW Car : " << name << std::endl;
          } 
  };
  
  class Audi : public Car {
    	public: 
      	Audi(std::string name) 
              :Car(name) 
          {}
      	void show() {
              std::cout << "BMW Car : " << name << std::endl;
          } 
  };
  
  enum CarType {
  	BMW , AUDI  
  };
  
  class SimpleFactory {
    	public: 
      	Car* createCar(CarType ct) {
              switch(ct) {
                  case BMW :
                      return new Bmw();
                  case AUDI :
                      return new Audi();
                  default :
                      std::cerr << "createCar is wrong" << std::endl;
                      break;
              }
              return nullptr; 
          }
  };
  ```

  * **里面包含了两种调用方式 , 一种通过裸指针调用,  另一种通过智能指针调用 **

    * **裸指针 **

      ```c++
      int main() {
          SimpleFactory* factory = new SimpleFactory(); // 纯虚函数不能实例化 , 所以不能 new Car();
          Car* p1 = factory->createCar("x1");
          Car* p2 = factory->createCar("a6");
          p1->show();
          p2->show();
          
          delete p1;
          delete p2;
          delete factory;
          
          return 0;
      }
      ```

      * **为什么 p1 不能这样调用 : Car* p1 = new Bmw(factory->createCar(“x1”);**
      * **Bmw 的构造函数是期待接收一个字符串 , factory->createCar 返回值是 Car* 类型, 很明显不可以**

    * **智能指针 **

      ```c++
      int main() {
          std::unique_ptr<SimpleFactory> factory(new SimpleFactory());
          std::unique_ptr<Car> p1(factory->createCar("x1"));
          std::unique_ptr<Car> p2(factory->createCar("a6"));
          p1->show();
          p2->show();
          return 0;
      }
      ```

* **当创建的类比较多的时候 , 用工厂模式设计 , 但是不可能一个工厂做所有的行为 , 生产所有的商品 , 所以要将它分开 , 并且这个方法对修改不关闭 ,  引入工厂行为 **

#### 1.5.2.2 工厂行为 

**基类提供了一个纯虚函数(创建产品) , 定义派生类(具体的产品生产工厂)负责创建相对应的产品 , 可以做到不同的产品在不同的工厂里面 创建 , 模块化非常清晰 , 产品之间没有任何耦合 , 能够对现有的产品和工厂修改关闭**

* **创建工厂行为**

  ```c++
  #include "00. head file.hpp" 
  
  class Car {
    	public:
      	Car(std::string name) 
  			: _name(name) 
          {}
      	virtual void show() = 0; // 提供纯虚函数 , 便于派生类的重写
      protected:
      	std::string _name;
  };
  
  class Bmw : public Car {
    	public: 
      	Bmw(std::string name) 
              :Car(name) 
          {}
      	void show() {
              std::cout << "BMW Car : " << name << std::endl;
          } 
  };
  
  class Audi : public Car {
    	public: 
      	Audi(std::string name) 
              :Car(name) 
          {}
      	void show() {
              std::cout << "BMW Car : " << name << std::endl;
          } 
  };
  
  class Factory {
      public: 
      	virtual Car* createCar() = 0;
  };
  
  class BwmFactory : public Factory {
    	public: 
      	Car* createCar(std::string name) {
              return new Bmw(name);
          }
  };
  
  class AudiFactory : public Factory {
      public:
      	Car* createCar(std::string name) {
              return new Audi(name);
          }
  }
  
  int main() {
      std::unique_ptr<Factory> bmwfty(new BMWFactory());
      std::unique_ptr<Factory> audifty(new AUDIFactory());
      std::unique_ptr<Car> p1(bmwfty->createCar("x6"));
      std::unique_ptr<Car> p2(audifty->createCar("a8"));
      p1->show();
      p2->show();
      
      return 0;
  }
  ```

  * **但是此时又存在问题 , 那就是有关联关系的产品不可能都是单独生产的 , 应该是一起生产 , 就比如 BMW 造车厂肯定不可能只造车, 肯定还有灯什么东西的生产 , 所以引入抽象工厂 **

#### 1.5.2.3 抽象工厂 

**把所有有关联关系的 , 属于一个产品簇的所有产品创建的接口函数 ,放在一个抽象工厂里 , 派生类(具体的加工工厂)应该负责创建该产品簇里面的所有产品**

* ```c++
  #include "00. head file.hpp" 
  
  class Car {
    	public:
      	Car(std::string name) 
  			: _name(name) 
          {}
      	virtual void show() = 0; // 提供纯虚函数 , 便于派生类的重写
      protected:
      	std::string _name;
  };
  
  class Bmw : public Car {
    	public: 
      	Bmw(std::string name) 
              :Car(name) 
          {}
      	void show() {
              std::cout << "BMW Car : " << name << std::endl;
          } 
  };
  
  class Audi : public Car {
    	public: 
      	Audi(std::string name) 
              :Car(name) 
          {}
      	void show() {
              std::cout << "BMW Car : " << name << std::endl;
          } 
  };
  
  class CarLight {
    	public:
      	virtual void show() = 0;
  };
  class BmwLight : public CarLight {
    	public: 
      	void show() {
              std::cout << "BMW Light" << std::endl;
          }
  };
  
  class AudiLight : public CarLight {
    	public:  
      	void show() {
              std::cout << "AUDI Light" << std::endl;
          }
  };
  class AbstractFactory {
      public: 
      	virtual Car* createCar() = 0;
      	virtual CarLight* createCarLight() = 0;
  };
  
  class BwmFactory : public AbstractFactory {
    	public: 
      	Car* createCar(std::string name) {
              return new Bmw(name);
          }
      	CarLight* createCarLight() {
              return new BmwLight();
          }
  };
  
  class AudiFactory : public AbstractFactory {
      public:
      	Car* createCar(std::string name) {
              return new Audi(name);
          }
      	CarLight* createCarLight() {
              return new AudiLight();
          }
  }
  
  int main() {
      // 现在考虑产品 一类产品(有关联关系的系列产品) -- 不可能每一件产品都要建一个工厂来生产 , 所以有关联的要在一起生产
      std::unique_ptr<AbstractFactory> bmwfty(new BMWFactory());
      std::unique_ptr<AbstractFactory> audifty(new AUDIFactory());
      std::unique_ptr<Car> p1(bmwfty->createCar("x6"));
      std::unique_ptr<Car> p2(audifty->createCar("a8"));
      std::unique_ptr<CarLight> l1(bmwfty->createCarLight());
      std::unique_ptr<CarLight> l2(audifty->createCarLight());
      p1->show();
      p2->show();
      l1->show();
      l2->show();
      return 0;
  }
  ```
  
  

### 1.5.3 代理模式

**代理模式 : 通过代理类来控制实际对象的访问权限**

**结构类似于 : 客户  ——>   助理 ——> 老板 , 客户想访问老板 , 不能直接访问老板 , 而是通过助理来访问老板 , 助理就相当于 proxy , 代理 , 老板就是委托**

* **第一步实现 **

  ```c++
  #include "00. head file.hpp"
  
  class VideoSite { // 抽象类
    	public: 
      	virtual void freeVideo() = 0;
      	virtual void vipVideo() = 0;
      	virtual void ticketVideo() = 0;
  };
  
  class FixBugVideoSite : public VideoSite { // 委托类 
      public: 
      	virtual void freeVideo() {
              std::cout << "watch free movie" << std::endl;
          }
      	virtual void vipVideo() {
              std::cout << "watch vip movie" << std::endl;
          }
      	virtual void ticketVideo() {
              std::cout << "watch ticket movie" << std::endl;
          }
  };
  
  int main() {
      VideoSite* p1 = new FixBugVideoSite();
      p1->freeVideo();
      p2->vipVideo();
      p3->ticketVideo();
      return 0;
  }
  ```

  * **像这个视频的访问一样 , 普通用户无法访问 vip 视频 , vip用户还需购买票才能访问用券视频 , 所以需要再访问的时候加入条件判断当这些开始进行访问是不被允许的, 所以这个方式不够灵活 , 引入代理模式 **

* **第二次实现 **

  ```c++
  #include "00. head file.hpp"
  
  class VideoSite { // 抽象类
    	public: 
      	virtual void freeVideo() = 0;
      	virtual void vipVideo() = 0;
      	virtual void ticketVideo() = 0;
  };
  
  class FixBugVideoSite : public VideoSite { // 委托类 
      public: 
      	virtual void freeVideo() {
              std::cout << "watch free movie" << std::endl;
          }
      	virtual void vipVideo() {
              std::cout << "watch vip movie" << std::endl;
          }
      	virtual void ticketVideo() {
              std::cout << "watch ticket movie" << std::endl;
          }
  };
  
  class FreeVideoSite : public FixBugVideoSite {
    	public: 
      	FreeVideoSite() {
              pVideo = new FixBugVideoSite();
          }
      	~FreeVideoSite() {
              delete pVideo;
          }
      	virtual void freeVideo() {
              pVideo->freeVideo();
          }
      	virtual void vipVideo() {
              std::cout << "free visitor can't access vip video" << std::endl;
          }
      	virtual void ticketVideo() {
              std::cout << "free visitor can't access ticket video" << std::endl;
          }
      	
      private:
      	VideoSite* pVideo;
  };
  
  class VipVideoSite : public FixBugVideoSite {
    	public: 
      	VipVideoSite() {
              pVideo = new FixBugVideoSite();
          }
      	~VipVideoSite() {
              delete pVideo;
          }
      	virtual void freeVideo() {
              pVideo->freeVideo();
          }
      	virtual void vipVideo() {
              pVideo->freeVideo();
          }
      	virtual void ticketVideo() {
              std::cout << "free visitor can't access ticket video" << std::endl;
          }
      	
      private:
      	VideoSite* pVideo;
  };
  
  void watchVideo(std::unique_ptr<VideoSite> &ptr) { // unqiue_ptr 取消拷贝构造 , 所以这里直接用引用 , 或者这里不用引用 , 在调用点用移动语义
      ptr->freeVideo();
      ptr->vipVideo();
      ptr->ticketVideo();
  }
  int main() {
      std::unique_ptr<VideoSite> p1(new FreeVideoSite()); // 直接用基类指针指向代理类 进行访问 
      std::unique_ptr<VideoSite> p1(new VipVideoSite()); // 直接用基类指针指向代理类 进行访问
      
      watchVideo(p1);
      watchVideo(p2);
      
      watchVideo(std::move(p1));
      watchVideo(std::move(p2));
          
      return 0;
  }
  ```

**总结一下代理模式的结构 : **

* **抽象类**
* **委托类**
* **代理类**
* **公共API接口**
* **主函数**

### 1.5.4 装饰器模式 

**装饰器模式 : 主要是通过增加类的功能实现装饰 , 但是实现装饰还有另一方法 , 就是增加子类 , 但是功能一多 , 子类增加的就有点多了 , 代码冗余 , 这时需要装饰器模式**

**装饰器模式与代理模式非常像 ,但是代理模式在调用的时候是不需要传参数的 , 它知道即将调用哪个函数 , 但是装饰器需要 , 因为功能是自己加的 , 想要什么功能只能自己加**

#### 1.5.4.1 装饰器模式与代理模式的对比

1. **装饰器 vs 代理模式：本质区别在于“控制权”**

你观察到“代理模式不需要传参数（指构造时通常已知目标），而装饰器需要传参数（包装谁由用户决定）”，这触及了两者最本质的区别：**意图（Intent）**。

**代理模式 (Proxy Pattern)**

- **核心目的：** **控制**对对象的访问。
- **关系：** 代理类和真实类之间的关系通常在**编译时**就确定了。代理类内部通常直接持有真实对象的引用，用户甚至可能不知道代理的存在。
- **例子：** 经纪人代理明星。你想找明星办事，只能通过经纪人。经纪人并不改变明星的功能，只是决定要不要让你见明星，或者在见明星前后做些杂事（收钱、签合同）。

**装饰器模式 (Decorator Pattern)**

- **核心目的：** **增强**对象的功能。
- **关系：** 装饰器和被装饰对象的关系是**动态**的、外部决定的。你（调用者）手里有一个对象，你觉得它不够强，于是你亲手把它塞进一个装饰器里。
- **例子：** 给手机套壳。手机是你买的（参数传进去的），你想套防摔壳还是发光壳，由你决定。每套一个壳，功能就增强一分。

------

2. **深度对比表**

| **维度**     | **装饰器模式 (Decorator)**               | **代理模式 (Proxy)**                       |
| ------------ | ---------------------------------------- | ------------------------------------------ |
| **关系确立** | **动态**。运行期间通过构造函数传入对象。 | **静态**。通常在代理类内部直接创建或持有。 |
| **关注点**   | 增加新功能、改变行为。                   | 控制访问、隐藏实现细节。                   |
| **透明度**   | 用户知道自己在“装饰”一个对象。           | 用户往往觉得自己就在直接调用真实对象。     |
| **层级**     | 鼓励多层嵌套（套娃）。                   | 通常只有一层代理。                         |

#### 1.5.4.2 装饰器模式的实现  

```c++
#include "00. head file.hpp"

class Car {
  	public : 
    	virtual void show() = 0;
};
class Bmw : public Car {
  	public :
    	void show() {
            std::cout << " this is BMW Car , it has : ";
        }
};
class Audi : public Car {
  	public :
    	void show() {
            std::cout << " this is Audi Car , it has : ";
        }
};
class Bnze : public Car {
  	public :
    	void show() {
            std::cout << " this is Bnze Car , it has : ";
        }
};

class Decorator : public Car { // 装饰器基类 
    public :
		Decorator(Car* p) 
			: pCar(p)
        {}
    protected: 
    	Car* pCar;
};
class CouncreteDecorator01 : public Decorator {
  	public :
    	CouncreteDecorator01(Car* p) 
            : Decorator(p)
        {}
    	void show() {
            pCar->show();
            std::cout << "cruise control funcion ";
        }
};
class CouncreteDecorator02 : public Decorator {
  	public :
    	CouncreteDecorator02(Car* p) 
            : Decorator(p)
        {}
    	void show() {
            pCar->show();
            std::cout << "automatic eergency braking ";
        }
};
class CouncreteDecorator03 : public Decorator {
  	public :
    	CouncreteDecorator03(Car* p) 
            : Decorator(p)
        {}
    	void show() {
            pCar->show();
            std::cout << "lane departure ";
        }
};
int main() {
    Car* p1 = new CouncreteDecorator01(new Bmw());
    p1 = new CouncreteDecorator02(p1);
    p1->show();
    Car* p2 = new CouncreteDecorator02(new Audi());
    p2->show();
    Car* p3 = new CouncreteDecorator03(new Bnze());
    p3->show();
    return 0;
}
```

* **第二种方式的实现 , 每一个装饰器直接继承于抽象类 **

  ```c++
  #include "00. head file.hpp"
  
  class Car {
    	public: 
      	virtual void show() = 0;
  };
  class Bmw : public Car {
    	public: 
      	void show() {
              std::cout << "this is Bmw Car , it has : ";
          }
  };
  class Audi : public Car {
    	public: 
      	void show() {
              std::cout << "this is Audi Car , it has : ";
          }
  };
  class Bnze : public Car {
    	public: 
      	void show() {
              std::cout << "this is Bnze Car , it has : ";
          }
  };
  
  class Decorator {
    	public: 
      	Decorator(Car* p)
              : pCar(p)
          {}
      protected: 
      	Car* pCar;
  };
  
  class ConcreteDecorator01 : class Car {
    	public:
      	ConcreteDecorator01(Car* p)
              : pCar(p)
          {}
      	void show() {
              pCar->show();
              std::cout << "cruise control function ";
          }
      
      private: 
      	Car* pCar;
  };
  class ConcreteDecorator012 : class Car {
    	public:
      	ConcreteDecorator02(Car* p)
              : pCar(p)
          {}
      	void show() {
              pCar->show();
              std::cout << "automatic emergency braking ";
          }
      
      private: 
      	Car* pCar;
  };
  class ConcreteDecorator03 : class Car {
    	public:
      	ConcreteDecorator03(Car* p)
              : pCar(p)
          {}
      	void show() {
              pCar->show();
              std::cout << "lane departure";
          }
      
      private: 
      	Car* pCar;
  };
  
  int main() {
      Car* p1 = new CouncreteDecorator01(new Bmw());
      p1 = new CouncreteDecorator02(p1);
      p1->show();
      Car* p2 = new CouncreteDecorator02(new Audi());
      p2->show();
      Car* p3 = new CouncreteDecorator03(new Bnze());
      p3->show();
      return 0;
  }
  ```

**第二种方法相较于第一种方法的优缺点 :**

你提供的两段代码在**功能执行结果**上是完全一样的，但在**代码结构、可维护性和设计规范**上有显著区别。

------

**核心区别：是否使用了“装饰器抽象基类” (`CarDecorator`)**

**1. 第一段代码（标准模式）：继承自 `CarDecorator`**

在这段代码中，`CouncreteDecorator01` 继承自 `CarDecorator`，而 `CarDecorator` 继承自 `Car`。

- **统一管理：** `CarDecorator` 把所有装饰器共有的成员（如 `Car* pCar` 指针）提取了出来。
- **语义清晰：** 从类关系图（UML）一眼就能看出哪些类是“真实的业务类”（Bmw, Audi），哪些是“为了增强功能的包装类”（Decorators）。
- **符合开闭原则：** 如果将来所有的装饰器都需要增加一个公共操作（比如记录日志），你只需要在 `CarDecorator` 基类中修改，而不需要去改动每一个具体的装饰器类。

**2. 第二段代码（简化实现）：直接继承自 `Car`**

在这段代码中，你跳过了 `CarDecorator` 基类，让所有具体装饰器直接继承自最顶层的 `Car`。

- **代码冗余：** 每个具体装饰器（01, 02, 03）都必须自己定义一遍 `Car* pCar` 成员变量，并自己写一遍构造函数。
- **结构扁平化：** 在这个结构中，`Bmw` 和 `CouncreteDecorator01` 在继承层级上是平级的。这在逻辑上会让人困惑：装饰器其实并不是一种“车”，而是一种“对车的包装”。

------

**为什么要坚持使用 `CarDecorator` 基类？**

在实际的大型工程中，引入 `CarDecorator` 有以下三个不可替代的好处：

**1. 减少重复代码 (DRY 原则)**

在你的第二段代码中，每个装饰器都要写：

```c++
private:
    Car* pCar; // 每个类都要重复写
```

如果装饰器有 100 个，你就得维护 100 份这个指针。有了 `CarDecorator`，这个指针只存在于基类中，子类直接继承使用即可。

**2. 默认行为的封装**

通常标准的 `CarDecorator` 会重写 `show()` 方法，使其默认调用被包装对象的方法：

```c++
class CarDecorator : public Car {
protected:
    Car* pCar;
public:
    CarDecorator(Car* p) : pCar(p) {}
    void show() override { 
        if(pCar) pCar->show(); // 默认行为：透传给被装饰对象
    }
};
```

这样，具体的装饰器只需要关注自己要**额外增加**的那部分功能，不需要强制重写整个逻辑。

**3. 类型识别与过滤**

在某些复杂的逻辑中，你可能需要判断一个对象是“原始车辆”还是“被改装过的车辆”。

- 如果使用第一种结构：你可以通过 `dynamic_cast<CarDecorator*>(p1)` 快速判断。
- 如果使用第二种结构：你很难将装饰器与真实的物理类区分开，因为它们都直接继承自 `Car`。

------

**总结**

| 特性         | 第一段代码 (有基类)                 | 第二段代码 (无基类)       |
| ------------ | ----------------------------------- | ------------------------- |
| **设计规范** | 标准装饰器模式                      | 变种/简化的装饰器         |
| **代码复用** | 高 (成员变量和构造逻辑在基类)       | 低 (每个子类都要重写)     |
| **层级关系** | 逻辑清晰 (车 vs 装饰插件)           | 逻辑扁平 (分不清谁是本体) |
| **扩展性**   | 极佳 (方便给所有装饰器增加公共逻辑) | 一般                      |

### 1.5.5 适配器模式 

**适配器模式的作用就是让不兼容的接口变得兼容**

**以电脑到多媒体的接口为例 , 有 VGA , HDMI 两种接口 , 一种接口肯定是不能往另一个接口放的 , 所以可以利用 适配器模式让接口变得兼容 **

* **实现**

  ```c++
  #include "00. head file.hpp" 
  
  class VGA {
      public:
      	virtual void play() = 0;
  };
  
  class TV01 : public VGA {
  	public: 
      	void play() {
              std::cout << "Computer -> VGA -> TV01" << std::endl;
          }
  };
  // 实现一个电脑类 
  class Computer {
    	public : 	
      	void playVideo(VGA* pVGA) {
              pVGA->play();
          }
  };
  
  // 有两种方式可以实现换接口 
  //1. 换电脑 , 不太现实 , 类实现的多了以后 , 很明显的麻烦
  //2. 买个转换器 , 这里就用的是这种方法
  
  class HDMI {
  	public:
      	virtual void play() = 0;    
  };
  
  class TV02 : public HDMI {
  	public: 
      	void play() {
              std::cout << "Computer -> HDMI -> TV02" << std::endl;
          }
  };
  
  class VGAToHDMIAdapter : public VGA {
    	public: 
      	VGAToHDMIAdapter(HDMI* p)
              : pHdmi(p) 
          {}
      	void play() { // 该方法就相当于是转换器
              pHdmi->play();
          }
      private:
      	HDMI* pHdmi;
  };
  int main() {
      Computer computer;
      //computer.playVideo(new TV01());
      computer.playVideo(new VGAToHDMIAdapter(new TV02()));
  	return 0;
  ```

* **为什么继承的是 VGA 但是却可以用 HDMI 来创建对象 ?**

  **ans : `首先,组合和继承是两回事 , HDMI 的类的定义在 VGAToHDMIAdapter 之前 , 编译器肯定是认识 HDMI类型的 , 根据类的创建可以发现 computer.playVideo 是认识 VGR 的类型 , 现在我要通过新创建 VGAToHDMIAdapter的对象把里面的 HDMI 接口返回回去 , 让它包装成一个 VGA 对象 , 这样就实现了适配 `**

### 1.5.6 观察者模式 

**观察者模式又被称为 : 观察者-监视者模式 / 发布-订阅模式 , 主要关注的是对象的一对多的关系 , 也就是多个对象都依赖一个对象 , 当该对象的状态发生改变的时候 , 其他对象都能接收到相应的通知**

* **实现 **

  ```c++
  #include "00. head file.hpp" 
  
  class Observer { // 观察者模式的基类 
    	public: 
      	virtual void handle() = 0;
  };
  
  class Observer1 : public Observer {
    	public:
       	void handle(int msgid) {
              switch(msgid) {
                  case 2 :
                      std::cout << "observer1 recv2 msg!" << std::endl;
                      break;
                  case 3 :
                      std::cout << "observer1 recv3 msg!" << std::endl;
                      break;
                  default :
                      std::cout << "observer1 recv unknow msg!" << std::endl;
              }
          }
  };
  
  class Observer2 : public Observer {
      public: 
          void handle(int msgid) {
              switch(msgid) {
                  case 1 :
                      std::cout << "observer2 recv1 msg!" << std::endl;
                      break;
                  case 3 :
                      std::cout << "observer2 recv3 msg!" << std::endl;
                      break;
                  default :
                      std::cout << "observer2 recv unknow msg!" << std::endl;
              }
  
          }
  };
  
  class Observer3 : public Observer {
      public: 
          void handle(int msgid) {
              switch(msgid) {
                  case 1 :
                      std::cout << "observer3 recv1 msg!" << std::endl;
                      break;
                  case 2 :
                      std::cout << "observer3 recv2 msg!" << std::endl;
                      break;
                  default :
                      std::cout << "observer3 recv unknow msg!" << std::endl;
              }
  
          }
  };
  
  // 主题类 
  class Subjet {
     	public: 
      	// 给观察者增加新的对象 
      	void addObserver(Observer* obser , int msgid) {
              _subMap[msgid].push_back(obser);
          }
      	// 主题检测发生改变 , 通知相应的观察者对象处理事件
      	void dispatch(int msgid) {
              auto it = _subMap.find(msgid);
              if(it != _subMap.end()) {
                  for(Observer* pObser : it->second) {
                      pObser->handle(msgid);
                  }
              }
          }
      	
      private: 
      	std::unordered_map<int , std::list<Observer*>> _subMap;
              
  };
  int main() {
      Subject Subject;
  
      Observer* p1 = new Observer1();
      Observer* p2 = new Observer2();
      Observer* p3 = new Observer3();
  
      Subject.addObserver(p1 , 1);
      Subject.addObserver(p1 , 2);
      Subject.addObserver(p2 , 2);
      Subject.addObserver(p3 , 1);
      Subject.addObserver(p3 , 3);
  
      int msgid = 0;
      for( ; ; ) {
          std::cout << "insert id : ";
          std::cin >> msgid;
          if(msgid == -1) 
              break;
          Subject.dispatch(msgid);
      }
  
      return 0;
  }
  ```

## 1.6 代码应用实践 

### 1.6.1 深度优先遍历搜索迷宫路径

#### 1.6.1.1 深度优先遍历搜索要求

```c++
请输入迷宫的行列数(例如 : 10  10) : 5 5
请输入迷宫的路径信息(0 表示可以走 , 1 表示不能走) :
0 0 0 1 1 
1 0 0 0 1
1 1 1 1 1 
1 1 0 0 1
1 1 1 0 0

迷宫路径搜索中...
>>>>如果没有路径 , 请直接输出<<<<
不存在一条迷宫路径!
>>>>如果有路径 , 直接输出<<<<
* * * 1 1 
1 0 * 0 1
1 1 * 1 1
1 1 * * 1
1 1 1 * * 
```

#### 1.6.1.2 深度优先搜索实现框架  

```c++
#include "00. head file.hpp"

// 定义四个方向的常量代表 

// 迷宫每一个节点的方向个数 

// 定义节点行走状态 

// 迷宫的类 
class Maze {
    public:
    	// 构造函数 , 初始化迷宫 , 根据传入的行列数生成二维动态数组 
    	Maze() {}
    	// 初始化迷宫路径节点信息 , 节点四个方向的默认初始化 
    	void initMaze(int x , int y , int val) {}
    	// 初始化迷宫 0 节点四个方向的行走状态信息 , 判断哪里能走 , 哪里不能走/ 最终能走到哪 
    	void setNodeState() {}
    	// 深度搜索迷宫路径 
    	void searchMazePath() {}
    	// 打印输出信息 
    	void showMazePath() {}
    private:
    	// 定义迷宫节点的信息 
    	struct Node {};
    	// 动态生成迷宫路径 
    	
    	// 行列数 
    
    	// 栈结构 
};
int main() {
    // 提示输入迷宫路径信息 
	std::cout << "please insert maze's rows and cols(eg : 10  10) = 5 5 : ";
    
    
    // 创建迷宫对象 
    
    //输入迷宫具体路径信息 , 初始化迷宫节点的基本信息 
    std::cout << "please insert maze's basic information(0 : can pass , 1 : can't pass) : ";
    
    
    // 开始设置所有节点四个方向的状态 
    maze.setDataState();
    
    // 开始进行深度搜索迷宫遍历
    maze.searchMazePath();
    
    // 打印输出信息
    
    return 0;
}
```

#### 1.6.1.3 深度搜索迷宫路径的实现 

```c++
#include "00. head file.hpp"

// 定义四个方向的常量代表 
const int RIGHT = 0;
const int DOWN = 1;
const int LEFT = 2;
const int UP = 3;

// 迷宫每一个节点的方向个数 
const int WAY_NUM = 4;

// 定义节点行走状态 
const int YES = 4;
const int NO = 5;

// 迷宫的类 
class Maze {
    public:
    	// 构造函数 , 初始化迷宫 , 根据传入的行列数生成二维动态数组 
    	Maze(int row = 3 , int col = 3) 
        	: _row(row)
            , _col(col)
    	{
        	_pMaze = new Node*[_row];
            for(int i = 0 ; i < _row ; ++ i) {
                _pMaze = new Node[_col];
            }
        }
    	// 初始化迷宫路径节点信息 , 节点四个方向的默认初始化 
    	void initMaze(int x , int y , int val) {
            _pMaze[x][y]._x = x;
            _pMaze[x][y]._y = y;
            _pMaze[x][y]._val = val;
            
            for(int i = 0 ; i < WAY_NUM ; ++ i) {
                _pMaze[x][y]._state[i] = NO;
            }
        }
    	// 初始化迷宫 0 节点四个方向的行走状态信息 , 判断哪里能走 , 哪里不能走/ 最终能走到哪 
    	void setNodeState() {
            for(int i = 0 ; i < _row ; ++ i) {
                for(int j = 0 ; j < _col ; ++ j) {
                    if(_pMaze[i][j]._val == 1) {
                        continue;
                    }
                    
                    // 向右走最多走到哪 -- 什么情况下还能继续往右走 
                    if(j < _col - 1 && _pMaze[i][j + 1] == 0) {
                        _pMaze[i][j]._state[RIGHT] = YES;
                    }
                    if(i < _row - 1 && _pMaze[i + 1][j] == 0) {
                        _pMaze[i][j]._state[DOWN] = YES;
                    }
                    if(j > 0 && _pMaze[i][j - 1] == 0) {
                        _pMaze[i][j]._state[LEFT] = YES;
                    }
                    if(i > 0 && _pMaze[i - 1][j] == 0) {
                        _pMaze[i][j]._state[UP] = YES;
                    } 
                }
            }
        }
    	// 深度搜索迷宫路径 -- 步骤 : #1 判断[0][0]节点 == 1 #2 压栈 , 取栈顶元素 , 重复右下左上顺序 , 依次探寻 , 如果进入死路 , 弹出这个节点 , 因为前面的条件判断已经生效 , 这就是半个回溯  
    	void searchMazePath() {
            if(_pMaze[0][0]._val == 1) {
                return;
            }
            _stack.push(_pMaze[0][0]);
            
            while(!_stack.empty()) {
                Node _top = _stack.top();
               	int x = _top._x;
                int y = _top._y;
                
                if(x == _row - 1 && y == _col - 1) { // 到右底角了, 结束
                    return;
                }
                
                if(_pMaze[x][y].state[RIGHT] == YES) { // 可以往右走
                    _pMaze[x][y].state[RIGHT] = NO; // 这个操作是防止反复横跳 , 避免右边节点是死胡同的时候回退 , 然后再回到这个节点
                    _pMaze[x][y + 1].state[LEFT] = NO;
                    continue;
                }
                
                if(_pMaze[x][y].state[DOWN] == YES) { 
                    _pMaze[x][y].state[DOWN] = NO; 
                    _pMaze[x + 1][y].state[UP] = NO;
                    continue;
                }
                
                if(_pMaze[x][y].state[LEFT] == YES) { 
                    _pMaze[x][y].state[LEFT] = NO; 
                    _pMaze[x][y - 1].state[RIGHT] = NO;
                    continue;
                }
                if(_pMaze[x][y].state[UP] == YES) { 
                    _pMaze[x][y].state[UP] = NO;
                    _pMaze[x - 1][y].state[DOWN] = NO;
                    continue;
                }
                _stack.pop();
            } 
        }
    	// 打印输出信息 
    	void showMazePath() {
            if(_stack.empty()) { // 如果当前栈为空 , 没有节点也就说明没有路径 
                std::cout << "there is no maze path here" << std::endl;
            }
            
            while(!_stack.empty()) { // 如果右节点 , 有路径 , 依次弹出每个节点 , 将每个节点的 val 值置为
                Node _top = _stack.top();
                _pMaze[_top._x][_top._y]._val = '*';
                _stack.pop();
            }
            
            // 按序打印
			for(int i = 0 ; i < _row ; ++ i) {
                for(int j = 0 ; j < _col ; ++ j) {
                    if(_pMaze[i][j]._val == '*') {
                        std::cout << "* ";
                    }
                    else {
                        std::cout << _pMaze[i][j]._val;
                    }
                }
                std::cout << std::endl;
            }
        }
    private:
    	// 定义迷宫节点的信息 
    	struct Node {
            int _x;
            int _y;
            int _val;
            int _state[WAY_NUM];
        };
    	// 动态生成迷宫路径 
    	Node** _pMaze;
    	// 行列数 
    	int _row;
   		int _col;
    	// 栈结构 
    	std::stack<Node> _stack;
};
int main() {
    // 提示输入迷宫路径信息 
	std::cout << "please insert maze's rows and cols(eg : 10  10) = 5 5 : ";
    int row , col;
    std::cin >> row >> col;

    // 创建迷宫对象 
    Maze maze(row , col);
    
    //输入迷宫具体路径信息 , 初始化迷宫节点的基本信息 
    std::cout << "please insert maze's basic information(0 : can pass , 1 : can't pass) : ";
    int data;
    for(int i = 0 ; i < row ; ++ i) {
        for(int j = 0 ; j < col ; ++ j) {
            std::cin >> data;
            maze.initMaze(i , j , data);
        }
    }
    
    // 开始设置所有节点四个方向的状态 
    maze.setDataState();
    
    // 开始进行深度搜索迷宫遍历
    maze.searchMazePath();
    
    // 打印输出信息
    maze.showMazePath();
    return 0;
}
```

### 1.6.2 广度优先遍历搜索迷宫路径 

#### 1.6.2.1 广度优先遍历搜索要求 

```c++
深度优先遍历 -- 栈 / 递归 
广度优先遍历 -- 层层扩张的方式 -- 队列 
广度优先遍历
寻找迷宫最短路径
0 0 1 1 1 1
1 0 0 0 0 1 
1 0 1 1 0 1 
1 0 0 0 0 1 
1 0 1 1 1 1 
1 0 0 0 0 0 

* * 1 1 1 1 
1 * 0 0 0 1
1 * 1 1 0 1
1 * 0 0 0 1 
1 * 1 1 1 1 
1 * * * * * 
```

**广度优先遍历搜索相较于深度优先遍历搜索是寻找最短路径,  它依赖`队列结构` , 上下左右都能走的 , 都要入队 , 找到新的节点 , 将旧节点出队 . 直到队尾元素是右下角的出口为止  , 而 深度优先遍历搜索是依赖于 `栈或者递归结构`, 一次性只压入一个 , 严格按照 右下左上的顺序 , `只要右边能一直走 , 就一直往右走`**

**队列结构的缺点 : `队列结构入队的节点不会存储路径信息 ; 在栈结构中 , 新节点是紧跟着旧节点入栈的 , 队列不会 ; 利用二维数组到一维数组的映射 , _pPath[x * _row + y] = _pMaze[x][y] , 每一个一维数组的节点存储是从哪个节点找到当前这个节点的` **

#### 1.6.2.2 广度优先遍历搜索实现 框架 

```c++
#include "00. head file.hpp"

// 定义四个方向的常量代表 

// 迷宫每一个节点的方向个数 

// 定义节点行走状态 

// 迷宫的类 
class Maze {
    public:
    	// 构造函数 , 初始化迷宫 , 根据传入的行列数生成二维动态数组 
    	Maze() {}
    	// 初始化迷宫路径节点信息 , 节点四个方向的默认初始化 
    	void initMaze(int x , int y , int val) {}
    	// 初始化迷宫 0 节点四个方向的行走状态信息 , 判断哪里能走 , 哪里不能走/ 最终能走到哪 
    	void setNodeState() {}
    	// 深度搜索迷宫路径 
    	void searchMazePath() {}
    	// 打印输出信息 
    	void showMazePath() {}
    private:
    	// 定义迷宫节点的信息 
    	struct Node {};
    	// 动态生成迷宫路径 
    	
    	// 行列数 
    
    	// 栈结构 
};
int main() {
    // 提示输入迷宫路径信息 
	std::cout << "please insert maze's rows and cols(eg : 10  10) = 5 5 : ";
    
    
    // 创建迷宫对象 
    
    //输入迷宫具体路径信息 , 初始化迷宫节点的基本信息 
    std::cout << "please insert maze's basic information(0 : can pass , 1 : can't pass) : ";
    
    
    // 开始设置所有节点四个方向的状态 
    maze.setDataState();
    
    // 开始进行深度搜索迷宫遍历
    maze.searchMazePath();
    
    // 打印输出信息
    
    return 0;
}
```

#### 1.6.2.3 广度搜索迷宫路径的实现 

```c++
#include "00. head file.hpp"

// 四个方向的常量代表 -- 右下左上
const int RIGHT = 0;
const int DOWN = 1;
const int LEFT = 2;
const int UP = 3;

// 迷宫每一个节点的方向个数 
const int WAY_NUM = 4;

// 定义节点行走状态 
const int YES = 4;
const int NO = 5;

// 迷宫的类 
class Maze {
    public:
    	// 构造函数 , 初始化迷宫 , 根据传入的行列数生成二维动态数组 
    	Maze(int row = 3 , int col = 3)
            : _row(row)
            , _col(col)
        {
            _pMaze = new Node*[_row];
            for(int i = 0 ; i < _row ; ++ i) {
                _pMaze[i] = new Node[_col];
            }
            _pPath.resize(_row * _col);
        }
    	// 初始化迷宫路径节点信息 , 节点四个方向的默认初始化 
    	void initMaze(int x , int y , int val) {
            _pMaze[x][y]._x = x;
            _pMaze[x][y]._y = y;
            _pMaze[x][y]._val = val;
            
            for(int i = 0 ; i < WAY_NUM ; ++ i) {
                _pMaze[x][y]._state[i] = NO;
            }
        }
    	// 初始化迷宫 0 节点四个方向的行走状态信息 , 判断哪里能走 , 哪里不能走/ 最终能走到哪 
    	void setNodeState() {
            for(int i = 0 ; i < _row ; ++ i) {
                for(int j = 0 ; j < _col ; ++ j) {
                    if(_pMaze[i][j]._val == 1) {
                        continue;
                    }

                    // 判断右方向能走到哪 
                    if(j < _col - 1 && _pMaze[i][j + 1]._val == 0) {
                        _pMaze[i][j]._state[RIGHT] = YES;
                    }
                    // 判断下方能走到哪
                    if(i < _row - 1 && _pMaze[i + 1][j]._val == 0) {
                        _pMaze[i][j]._state[DOWN] = YES;
                    }
                    // 判断左方能走到哪
                    if(j > 0 && _pMaze[i][j - 1]._val == 0) {
                        _pMaze[i][j]._state[LEFT] = YES;
                    }
                    // 判断上方能走到哪
                    if(i > 0 && _pMaze[i - 1][j]._val == 0) {
                        _pMaze[i][j]._state[UP] = YES;
                    }
                }
            }
        }
    	// 深度搜索迷宫路径 
    	void searchMazePath() {
            if(_pMaze[0][0] == 1) {
                return;
            }
            _queue.push(_pMaze[0][0]);
            
            while(!_queue.empty()) {
                Node _top = _queue.front();
                int x = _top._x;
                int y = _top._y;
                
                if(_pMaze[x][y]._state[RIGHT] == YES) {
                    _pMaze[x][y]._state[RIGHT] = NO;
                    _pMaze[x][y + 1]._state[Left] = NO;
                    // 通过二维数组映射到一维数组存储路径信息 
                    _pPath[x * _row + y + 1] = _pMaze[x][y]; // 想要存的出路径信息 , 就存的是谁是被谁找到的 , 前者很明显是当前节点的右边的节点 , 后者很明显是当前节点 , 当前节点的映射是 x * _row + y , 那后一个节点的映射就是 x * _row + y + 1
                    if(check(_pMaze[x][y + 1])) { // 检测是不是右下角节点 
                        return;
                    }
                    _queue.push(_pMaze[x][y + 1]);
                }
                if(_pMaze[x][y]._state[DOWN] == YES) {
                    _pMaze[x][y]._state[DOWN] = NO;
                    _pMaze[x + 1][y]._state[UP] = NO;
                    // 在辅助数组中记录一下行走信息
                    _pPath[(x + 1) * _row + y] = _pMaze[x][y]; 
                    _queue.push(_pMaze[x + 1][y]);
                    if(check(_pMaze[x + 1][y])) {
                        return;
                    }
                }

                if(_pMaze[x][y]._state[LEFT] == YES) {
                    _pMaze[x][y]._state[LEFT] = NO;
                    _pMaze[x][y - 1]._state[RIGHT] = NO;
                    // 在辅助数组中记录一下行走信息
                    _pPath[x * _row + y - 1] = _pMaze[x][y]; 
                    _queue.push(_pMaze[x][y - 1]);
                    if(check(_pMaze[x][y - 1])) {
                        return;
                    }
                }

                if(_pMaze[x][y]._state[UP] == YES) {
                    _pMaze[x][y]._state[UP] = NO;
                    _pMaze[x - 1][y]._state[DOWN] = NO;
                    // 在辅助数组中记录一下行走信息
                    _pPath[(x - 1) * _row + y] = _pMaze[x][y]; 
                    _queue.push(_pMaze[x - 1][y]);
                    if(check(_pMaze[x - 1][y])) {
                        return;
                    }
                }

                _queue.pop();
            }
        }
    	// 打印输出信息 
    	void showMazePath() {
            if(_queue.empty()) {
                std:;cout << "there is no maze path here" << std::endl;
            }
            else {
                int x = _row - 1;
                int y = _col - 1;
                for( ; ; ) { // 开始利用一维数组存储的路径回溯找首节点 
                    _path[x * _row + y] = '*';
                    Node node = _path[x * _row + y];
                    if(x == 0 && y == 0) { // 找到首节点了
                        break; 
                    }
                    x = node._x;
                    y = node._y;   
                }
                for(int i = 0 ; i < _row ; ++ i) {
                    for(int j = 0 ; j < _col ; ++ j) {
                        if(_pMaze[i][j]._val == '*') {
                            std::cout << "* ";
                        }
                        else {
                            std::cout << _pMaze[i][j]._val << " ";
                        }
                    }
                    std::cout << std::endl;
                }
            }
        }
    private:
    	// 定义迷宫节点的信息 
    	struct Node {
            int _x;
            int _y;
            int _val;
            int _state[WAY_NUM];
        };
    	// 动态生成迷宫路径 
    	Node** _pMaze;
    	// 行列数 
    	int _row;
    	int _col;
    	// 队列结构 以及 一维数组结构 
    	std::queue<Node> _queue;
    	std::vector<Node> _pPath;
    
};
int main() {
    // 提示输入迷宫路径信息 
	std::cout << "please insert maze's rows and cols(eg : 10  10) = 5 5 : ";
    int row , col;
    std::cin >> row >> col;
   
    // 创建迷宫对象 
    Maze maze(row , col);
    //输入迷宫具体路径信息 , 初始化迷宫节点的基本信息 
    std::cout << "please insert maze's basic information(0 : can pass , 1 : can't pass) : ";
    int data;
    for(int i = 0 ; i < row ; ++ i) {
        for(int j = 0 ; j < col ; ++ j) {
            std::cin >> data;
            maze.initMaze(i , j , data);
        }
    }
    
    // 开始设置所有节点四个方向的状态 
    maze.setDataState();
    
    // 开始进行深度搜索迷宫遍历
    maze.searchMazePath();
    
    // 打印输出信息
    maze.showMazePath();
    
    return 0;
}
```

### 1.6.3 大数的加减法 

#### 1.6.3.1 前提 

**因为内置数据类型有范围限制 , 所以在进行大数的加减法的时候无法进行,利用字符串处理可以解决这类问题 **

#### 1.6.3.1 大数的加法 

```c++
#include "00. head file.hpp"

class BigInt {
  	public:
    	BigInt(std::string str)
            : _strDigit(str) 
        {}
    private:
    	std::string _strDigit;
};
std::ostream& operator<<(std::ostream &out , const BigInt &rhs) {
    out << rhs._strDigit;
    return out;
}

BigInt operator+(const BigInt &lhs , const BigInt &rhs) {
    std::string result;
    bool flag = false; // 表示是否需要进位 
    int i = lhs._strDigit.length() - 1;
    int j = rhs._strDigit.length() - 1;
    
    for( ; i >= 0 && j >= 0 ; -- i , -- j) {
        int ret = lhs._strDigit[i] - '0' + rhs._strDigit[j] - '0';
        if(flag) {
            ret += 1;
            flag = false;
        }
        
        if(ret >= 10) {
            ret %= 10;
            flag = true;
        }
        
        result.push_back(ret + '0');
    }
    
    // 可能出现一个数有剩余的情况
    if(i >= 0) {
        while(i >= 0) {
            int ret = lhs._strDigit[i] - '0';
            if(flag) {
                ret += 1;
                flag = false;
            }
            
            if(ret >= 10) {
                ret %= 10;
                flag = true;
            }
            
            result.push_back(ret + '0');
        }
    }
    
    if(j >= 0) {
        while(j >= 0) {
            int ret = rhs._strDigit[j] - '0';
            if(flag) {
                ret += 1;
                flag = false;
            }
            
            if(ret >= 10) {
                ret %= 10;
                flag = true;
            }
            
            result.push_back(ret += '0');
        }
    }
    
    // 在进行倒数第二高位的加法产生进位的情况 
    if(flag) {
        result.push_back('1');
    }
    
    reverse(result.begin() , result.end());
    return result; // 写成这样的话 , 编译器会自动寻找在 BigInt 类中有没有这样的构造函数 : BigInt(std::string)
}
int main() {
    BigInt big1("7754549495242422897857557952792792");
    BigInt big2("446546498646546115616516984616");

  	std::cout << big1 + big2 << std::endl;
    return 0; 
}
```

* **一个问题: 为什么表示进位的判断要写在 ret >= 10 之前 ?**

  **ans : `因为如果先进行 ret >= 10 的判断的话 , 进位是会给当前位置进1的 , 这是万万不可的 , 进位是要给下一位进`**

#### 1.6.3.1 大数的减法 

```c++
#include "00. head file.hpp" 

class BigInt {
  	public:
    	BigInt(std::string str)
            : _strDigit(str) 
        {}
    private:
    	std::string _strDigit;
};
std::ostream& operator<<(std::ostream &out , const BigInt &rhs) {
    out << rhs._strDigit;
    return out;
}

BigInt operator-(const BigInt &lhs , const BigInt &rhs) {
    std::string result;
    bool flag = false; // 判断产生借位
    bool minor = false; // minor 是判断是否需要添加负号  
    std::string maxStr = lhs._strDigit;
    std::string minStr = rhs._strDigit;
    
    if(maxStr.length() < minStr.length()) { // 先比较一下字符串的长度看需要加负号吗
        maxStr = rhs._strDigit;
        minStr = lhs._strDigit;
        minor = true;
    }
    else if(maxStr.length() == minStr.length()) { // 直接比较字符串的大小 
        if(maxStr < minStr) {
            maxStr = rhs._strDigit;
            minStr = rhs._strDigit;
            minor = true;
        }
        else if(maxStr == minStr) {
            result.push_back('0');
        }
    }
    else {
        ;
    } 
    
    int i = maxStr.length() - 1;
    int j = minStr.length() - 1;
    
    for( ; i >= 0 && j >= 0 ; -- i , -- j) {
        int ret = maxStr[i] - '0' - minStr[j] - '0';
        if(flag) {
            ret -= 1;
            flag = false;
        }
        
        if(ret <= 0) {
            ret %= 10;
            flag = true;
        }
        
        result.push_back(ret + '0');
    }
    
    // 只有大数没有处理完的情况 
    
    if(i >= 0) {
        while(i >= 0) {
            int ret = maxStr[i] - '0';
            if(flag) {
                ret -= 1;
                flag = false;
            }
            
            if(ret <= 0) {
                ret += 10;
                flag = true;
            }
            
            result.push_back(ret + '0');
        }
    }
    
    if(minor) {
        result.push_back('-');
    }
    reverse(result.begin() , result.end());
    return result;
}
int main() {
    BigInt big3("7778889");
    BigInt big4("7778888");
    
    std::cout << big4 - big3 << std::endl;
    return 0;
}
```

* **减过之后发现 , 结果怎么是 -1000000 ? 所以要进行优化, 将不需要的 0 去掉 **

```c++
#include "00. head file.hpp" 

class BigInt {
  	public:
    	BigInt(std::string str)
            : _strDigit(str) 
        {}
    private:
    	std::string _strDigit;
};
std::ostream& operator<<(std::ostream &out , const BigInt &rhs) {
    out << rhs._strDigit;
    return out;
}

BigInt operator-(const BigInt &lhs , const BigInt &rhs) {
    std::string result;
    bool flag = false; // 判断产生借位
    bool minor = false; // minor 是判断是否需要添加负号  
    std::string maxStr = lhs._strDigit;
    std::string minStr = rhs._strDigit;
    
    if(maxStr.length() < minStr.length()) { // 先比较一下字符串的长度看需要加负号吗
        maxStr = rhs._strDigit;
        minStr = lhs._strDigit;
        minor = true;
    }
    else if(maxStr.length() == minStr.length()) { // 直接比较字符串的大小 
        if(maxStr < minStr) {
            maxStr = rhs._strDigit;
            minStr = rhs._strDigit;
            minor = true;
        }
        else if(maxStr == minStr) {
            result.push_back('0');
        }
    }
    else {
        ;
    } 
    
    int i = maxStr.length() - 1;
    int j = minStr.length() - 1;
    
    for( ; i >= 0 && j >= 0 ; -- i , -- j) {
        int ret = maxStr[i] - '0' - minStr[j] - '0';
        if(flag) {
            ret -= 1;
            flag = false;
        }
        
        if(ret <= 0) {
            ret %= 10;
            flag = true;
        }
        
        result.push_back(ret + '0');
    }
    
    // 只有大数没有处理完的情况 
    
    if(i >= 0) {
        while(i >= 0) {
            int ret = maxStr[i] - '0';
            if(flag) {
                ret -= 1;
                flag = false;
            }
            
            if(ret <= 0) {
                ret += 10;
                flag = true;
            }
            
            result.push_back(ret + '0');
        }
    }
    // ----------------------------------------- Optimization -------------------------------------------
    std::string retStr;
    auto it = result.rbegin(); // 反向进行遍历当前结果
    for( ; it != result.rend() ; ++ it) { // 如果当前元素不等于 0 , 跳过不进行处理 , 此时一定位于有效位上 
        if(*it != '0') {
            break;
        }
    }
    
    for( ; it != result.rend() ; ++ it) {
        retStr.push_back(*it); // 将有效值位直接压入
    }
    
    if(minor) {
        retStr.insert(retStr.begin() , '-');
    }
    
    return retStr;
	// ----------------------------------------- Optimization -------------------------------------------  
}
int main() {
    BigInt big3("7778889");
    BigInt big4("7778888");
    
    std::cout << big4 - big3 << std::endl;
    return 0;
}
```

### 1.6.4 大数据问题 

#### 1.6.4.1 大数据查重问题

* **哈希 – 空间换时间 : `假定现在有 5G 的数据 , 一个整数 4 个字节 , 总共有 20G 的数据域内存 , 链式哈希模式还有地址域,每个4字节 , 又有 20G 地址域内存 , 所以要想存储下这 5G整形数据 , 需要 40G 哈希内存`**
* **分治思想 : 将大文件哈希映射到小文件 , 然后再进行查重**
* **Bloom Filter : 布隆过滤器**
* **特定字符串的查重 : 需要利用 TrieTree 字典树(前缀树)**

#### 1.6.4.2 大数据 TopK 问题

* **大/小根堆 , 想找到前 K 大的元素 (小根堆) , 前 K 小的元素(大根堆)**
* **快排分割 : 利用基准值来递归寻找**

##### 1.6.4.2.1 大小根堆解决 

```c++
#include "00. head file.hpp"

int main() {
    
    std::vector<int> vec;
    for(int i = 0 ; i < 10000; ++ i) {
        vec.push_back(rand() + i);
    }
    
    std::priority_queue<int , std::vector<int> , std::greater<int>> minHeap; // 定义小根堆 
    int i = 0;
    for( ; i < 10 ; ++ i) {
        minHeap.push(vec[i]); // 先压入 10 个数字
    }
    
    for( ; i < 10000 ; ++ i) {
        if(vec[i] < minHeap.top()) {
            minHeap.pop();
            minHeap.push(vec[i]);
        }
    }
    
    return 0;
}
```

##### 1.6.4.2.2 快排分割解决 

```c++
#include "00. head file.hpp" 
int paration(std::vector<int> &arr , int i , int j) {
    int k = arr[i];
    while(i < j) {
        while(i < j && arr[j] > k) {
            j --;
        }
        if(i < j) {
            arr[i ++ ] = arr[j];
        }
        while(i < j && arr[i] < k) {
            i ++;
        }
        if(i < j) {
            arr[j --] = arr[i];
        }
    }
    arr[i] = k;
    return i;
}

int selectNok(std::vector<int> &arr , int i , int j , int k) {
   	int pos = paration(arr , i , j);
    if(pos == k - 1) {
        return pos;
    }
    else if(pos < k - 1) {
        return selectNok(arr , pos + 1 , j , k);
    }
    else {
        return selectNok(arr , i , pos - 1 , k);
    }
}
int main() {
    std::vector<int> vec;
    for(int i = 0 ; i < 10000 ; ++ i) {
        vec.push_back(rand() + i);
    }
    
    std::cout << vec[selectNok(vec , 0 , vec.size() - 1 , 10)] << std::endl;
    
    return 0;
}
```

#### 1.6.4.3 大数据查重及TopK综合问题 

##### 1.6.4.3.1 问题一 : *数据的重复次数最大的前 K 个*

```c++
#include "00. head file.hpp"

int main() {
    // 用 vector 存储数据 
    std::vector<int> vec;
    for(int i = 0 ; i < 100000 ; ++ i) {
        vec.push_back(rand() + i);
    }
    
    // 利用哈希表统计一下数字出现次数 
    std::unordered_map<int , int> map;
    for(int val : vec) {
        map[val] ++;
    }
    
    // 定义一个小根堆
    using PII = std::pair<int , int>;
    using FUNC = std::function<bool(PII& , PII&)>;
    using MinHeap = std::priority_queue<PII , std::vector<PII> , PFUNC>;
    MinHeap minheap([](auto &a , auto &b)->bool {
        return a.second > b.second;
    });
    
    int i = 0;
    auto it = map.begin();
    
    // 先将10个数据压入小根堆
    for( ; it != map.end() && i < 10 ; ++ it , ++ i) {
        minheap.push(*it);
    }
    
    // 处理剩下的数据 
    for( ; i < 100000 && it != map.end() ; ++ it , ++ i) {
        if(it->second < minheap.top().second) {
            minheap.pop();
            minheap.push(*it);
        }
    }
    
    // 打印输出这十个数据
    while(!minheap.empty()) {
        std::cout << "number : " << minheap.top().first << " appears : " << minheap.top().second << std::endl;
        minheap.pop();
    }
    
    return 0;
}
```

##### 1.6.4.3.2 问题二 : *大文件划分小文件(哈希映射) + 哈希统计 + 小根堆(快排分割)*

```c++
#include "00. head file.hpp" 


int main() {
    // 定义大文件的文件名
    const std::string bigFile = "big_data.dat";
    
    // 分区数量，使用素数53以减少数据倾斜
    const int M = 53;                          
    
    // 测试时生成的文件中包含的数字总数量（实际使用时可删除这行和生成文件的代码）
    const long long totalNumbers = 10000000LL; 

    // ==================== 生成测试大文件（实际使用时应该删除这部分） ====================
    std::cout << "Generating large test file...\n";
    {
        // 以二进制写入模式打开输出文件
        std::ofstream out(bigFile, std::ios::binary);
        
        // 初始化随机数种子，保证每次运行生成的随机数不同
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        
        // 循环生成指定数量的随机数字并写入文件
        for (long long i = 0; i < totalNumbers; ++i) {
            // 生成一个随机数，并加入少量偏移制造重复数据
            long long num = std::rand() % 1000000LL + (i % 100000LL);
            
            // 将数字以二进制形式写入文件（比文本方式更快）
            out.write(reinterpret_cast<const char*>(&num), sizeof(num));
        }
        // out 会在作用域结束时自动关闭
    }

    // ==================== 阶段1：哈希分区（把大文件拆分成多个小文件） ====================
    std::cout << "Phase 1: Hash partitioning into " << M << " small files (prime number)...\n";
    {
        // 创建 M 个输出文件流，用于写入各个小文件
        std::vector<std::ofstream> outs(M);
        
        // 打开 M 个小文件，文件名分别为 part_0.dat ~ part_52.dat
        for (int i = 0; i < M; ++i) {
            outs[i].open("part_" + std::to_string(i) + ".dat", std::ios::binary);
        }

        // 以二进制读取模式打开大文件
        std::ifstream in(bigFile, std::ios::binary);
        
        long long num;   // 用于存储从文件中读取的每个数字
        
        // 循环读取大文件中的每一个数字，直到文件结束
        while (in.read(reinterpret_cast<char*>(&num), sizeof(num))) {
            // 计算当前数字应该被分配到哪个小文件（哈希映射）
            // 使用 num % M 作为分区索引，素数M能让分布更均匀
            int idx = std::abs(static_cast<int>(num % M));
            if (idx < 0) idx = 0;   // 防止负数索引（理论上不会发生，但保险起见）
            
            // 将当前数字写入对应的小文件
            outs[idx].write(reinterpret_cast<const char*>(&num), sizeof(num));
        }

        // 关闭所有小文件输出流
        for (auto& f : outs) f.close();
    }

    // ==================== 阶段2：处理每个小文件，统计频率并选出本地 Top10 ====================
    std::cout << "Phase 2: Processing each small file...\n";

    // 定义一个类型别名：PII 表示 pair<出现次数, 数字>
    using PII = std::pair<int, long long>;  

    // 用于保存所有小文件的本地 Top10 结果
    std::vector<std::vector<PII>> allLocalTops;

    // 依次处理每一个小文件
    for (int i = 0; i < M; ++i) {
        // 构造当前小文件的文件名
        std::string partFile = "part_" + std::to_string(i) + ".dat";
        
        // 以二进制读取模式打开当前小文件
        std::ifstream in(partFile, std::ios::binary);

        // 使用 unordered_map 统计当前小文件中每个数字出现的次数
        std::unordered_map<long long, int> freq;
        
        long long num;
        // 读取小文件中的所有数字并统计频率
        while (in.read(reinterpret_cast<char*>(&num), sizeof(num))) {
            freq[num]++;                    // 数字出现一次，计数加1
        }
        in.close();                         // 关闭当前小文件

        // 定义小根堆的比较函数：按出现次数从小到大（堆顶是出现次数最少的）
        auto cmp = [](const PII& a, const PII& b) {
            return a.first > b.first;
        };
        
        // 创建小根堆，用于维护出现次数最多的10个数字
        std::priority_queue<PII, std::vector<PII>, decltype(cmp)> minHeap(cmp);

        // 遍历当前小文件的所有频率统计结果
        for (const auto& p : freq) {
            // 把 {出现次数, 数字} 放入堆中
            minHeap.push({p.second, p.first});
            
            // 如果堆的大小超过10，弹出堆顶（即出现次数最少的那个）
            if (minHeap.size() > 10) {
                minHeap.pop();
            }
        }

        // 用于临时保存当前小文件的 Top10 结果
        std::vector<PII> localTop;
        
        // 把堆中的所有元素取出并保存到 localTop 中
        while (!minHeap.empty()) {
            localTop.push_back(minHeap.top());
            minHeap.pop();
        }

        // 如果有有效结果，则把当前小文件的 Top10 保存到总集合中
        if (!localTop.empty()) {
            allLocalTops.push_back(std::move(localTop));   // 使用 move 避免拷贝，提高效率
        }
    }

    // ==================== 阶段3：合并所有本地 Top10，得到全局 Top10 ====================
    std::cout << "Phase 3: Merging global Top 10...\n";

    // 定义全局小根堆的比较函数（同样按出现次数从小到大）
    auto cmpGlobal = [](const PII& a, const PII& b) {
        return a.first > b.first;
    };
    
    // 创建全局小根堆，用于从所有候选结果中选出最终的 Top10
    std::priority_queue<PII, std::vector<PII>, decltype(cmpGlobal)> globalHeap(cmpGlobal);

    // 遍历所有小文件的本地 Top10
    for (const auto& local : allLocalTops) {
        for (const auto& p : local) {
            // 把每个候选结果放入全局堆
            globalHeap.push(p);
            // 保持堆的大小不超过10，自动淘汰出现次数较少的
            if (globalHeap.size() > 10) {
                globalHeap.pop();
            }
        }
    }

    // 用于保存最终的全局 Top10 结果
    std::vector<PII> globalTop10;
    
    // 把全局堆中的所有元素取出
    while (!globalHeap.empty()) {
        globalTop10.push_back(globalHeap.top());
        globalHeap.pop();
    }

    // 对最终结果按出现次数从高到低排序（方便阅读）
    std::sort(globalTop10.rbegin(), globalTop10.rend());

    // ==================== 输出最终结果 ====================
    std::cout << "\n=== Top 10 Most Frequent Numbers ===\n";
    
    // 遍历并打印全局 Top10
    for (const auto& p : globalTop10) {
        std::cout << "Number: " << p.second 
                  << "  appears: " << p.first << " times\n";
    }

    return 0;   // 程序正常结束
}
```

## 1.7 C++ 面经讲解

**技术面试过程中回答问题应该注意的问题**

1. **当面试官提问问题时 , 不要着急作答 , 应该适当停一下 , 整理一下逻辑思路**
2. **对于简单问题的回答 , 尽量不要照本宣科 , 找准问题回答的角度和层次 , 争取简单的问题的时候有亮点**
3. **针对复杂的问题 , 比较难以阐述的问题 , 思考要花一些时间 , 整理好逻辑思路 , 以及问题大致描述的顺序 **
4. **对于面试中 , 被提问的自己不知道的内容 , 真实一点**
5. **你还有什么问题? 更能关注一些公司内容 , 用到的技术 , 请技术官对面试做一下点评 , 给予一些经验**

### 1.7.1 **C++ this 指针是干什么用的 ?**

**ans : `1. this 指针是编译器在编译非静态成员函数的时候自动添加的隐式形参 `**

**`2. 类可以实例化很多对象 , 但是这些对象都共享一套成员方法 , 在进行调用的时候 , 会将当前的对象的地址传进去就是这样 obj.func(); => func(&obj); `**

**`3. 静态成员函数不需要 this 指针是因为静态成员函数属于类本身 , 即使有 this 指针 , 也会以类的作用域直接调用 , 所以它不需要 ; `**

**`4. 在普通成员函数中 , this 指针的类型是这样的 : 类名* this , 在 const 成员函数中 , 是这样的 : const 类名* this , 因为 this 指向的是 const 类的指针 , 被 cosnt 所修饰 , 所以 const成员函数中 , this 指针不可被需改 ; `**

**`5. 另外 , 在成员函数中想调用成员变量的时候 会变成这样 this->val;`**

**完美版本参考回答（你可以模仿这个风格）：**

“this 指针是 C++ 编译器为**非静态成员函数**自动插入的一个隐式参数（implicit parameter）。

它的核心作用是让成员函数知道当前正在操作的是哪一个具体的对象实例。因为一个类可以创建多个对象，但所有对象共享同一套成员函数代码，所以在调用 obj.func() 时，编译器会自动把当前对象的地址传递进去，相当于 func(&obj)，函数内部通过 this 指针来访问成员。

具体类型如下：

- 在普通成员函数中，this 的类型是 Test*；
- 在 const 成员函数中，this 的类型是 const Test*。正是因为 this 是 const 指针，所以在 const 成员函数内部无法修改普通成员变量（除非成员被 mutable 修饰）。

另外，静态成员函数没有 this 指针，因为静态成员函数属于类本身，而不是属于某个具体对象实例。

在成员函数内部，当我们直接访问成员变量时，编译器会自动将其转换为 this->成员变量 的形式。”

### 1.7.2 **C++ 的 new 和 delete , 什么时候用 new[] 申请 , 可以用 delete 释放?**

**ans : `new 和 delete 这一块坚持谁开辟 谁释放的原则 ;`**

**`1. 在自定义类类型中 ,new(new[]) 的底层原理是 :先调用operator new(operator new[]) , 然后调用 构造函数 , 返回指向对象的指针 , delete(delete[]) 的原理则是 先调用调用析构函数 , 然后调用 operator delete(operator delete[]) 释放内存 `**

**`2. 那至于为什么 delete[] 知道自己释放的是对象数组呢 , 是因为 new[] 的时候 , 会记录数组里元素的个数 , 那混用的情况呢 , 对于 new , delete[] -> 这是未定义的行为 , 危险 , new[] , delete 呢, 对于自定义类型 , new[] 开辟的是对象数组 , delete 只会调用一次析构函数 , 那么 n-1 的析构函数就无处调用 , 导致内存泄漏 , 而在内置类型中 , 虽然没有构造析构函数 , 但是还是尽量避免这样的混用 , 导致产生未定义行为 , 危险`**

**推荐的完美/高分回答版本**：

“在 C++ 中，new 与 delete、new[] 与 delete[] **必须严格一一对应**，坚持谁申请谁释放的原则。

new 表达式的执行过程是：

- 先调用 operator new（单个对象）或 operator new[]（数组）分配内存，
- 然后调用构造函数（单个对象调用1次，数组对每个元素各调用1次）。

delete 表达式的执行过程是：

- 先调用析构函数（delete 调用1次，delete[] 调用 n 次），
- 再调用 operator delete（或 operator delete[]）释放内存。

delete[] 能够正确处理数组，是因为 new[] 在分配内存时会在数组头部额外记录元素的个数，delete[] 会先读取这个计数来决定要析构多少次对象。

如果混用会出现问题：

- 用 new 分配单个对象，却用 delete[] 释放 → 未定义行为，非常危险。
- 用 new[]分配数组，却用普通 delete 释放：
  - 当元素是**自定义类**且有析构函数时：只会调用**第一个元素**的析构函数，其余 n-1 个对象的析构函数不会被执行，导致资源泄漏（比如打开的文件、锁等资源没有正确释放）。
  - 当元素是**内置类型**时：虽然没有析构函数调用，看起来可能不会崩溃，但严格来说仍是未定义行为，不推荐这样做。

因此，在实际编码中一定要严格匹配使用 new/delete 和 new[]/delete[]。”

### 1.7.3 C++ static 关键字的作用

**ans : `c++ 中 static 主要有三个应用场景 : `**

**`1. 修饰普通全局变量或者普通全局函数 , 这时 , 编译器会将这两个从外部链接变为内部链接 , 只能在当前 .cpp 文件访问 , 其他文件加了 extern 也无法访问 , 这样做是为了避免其他文件中同名变量或者函数名污染 `**

**`2. 修饰普通局部变量 , 它会让这个局部变量的生命周期延长到整个程序结束才被释放 , 因为此时它不位于栈上 , 而位于 .data 段或者 .bss 段 , 它会在程序运行到这个地方才进行初始化 , 并且只被初始化一次 , 默认值被初始化为 0 `**

**`3. 在修饰成员变量的时候 , 这个成员变量属于类本身 , 共享一块内存 , 在 c++ 17 之前 , 一定要类内声明 , 类外初始化 , 在 c++ 17 之后 , 在类内可以用 inline static 初始化 , 静态成员变量不占用单个对象大小 , 在修饰成员函数的时候是不会产生 this 指针的 , 因为此时这个成员函数属于类本身,并且 static 成员函数不能访问普通成员变量 , 因为没有this 指针`**

**最终推荐高分回答**：

“C++ 中 static 关键字主要有三个应用场景：

1. **修饰全局变量和全局函数** 它会将链接属性从 external linkage 改为 internal linkage（内部链接），使该变量或函数只能在当前 .cpp 文件中使用，其他文件即使使用 extern 也无法访问。这样可以有效避免不同文件之间同名变量或函数的命名污染。
2. **修饰局部变量** 使局部变量的生命周期延长到整个程序运行结束。它不再存储在栈上，而是放在静态存储区（.data 或 .bss 段）。特点是第一次运行到该语句时才初始化，且只初始化一次，默认零初始化为 0。
3. 修饰类成员
   - **静态成员变量**：属于类本身，所有对象共享同一份内存，不占用单个对象的 sizeof 大小。在 C++17 之前需要类内声明、类外初始化；C++17 之后支持使用 inline static 在类内直接初始化。
   - **静态成员函数**：属于类本身，**没有 this 指针**，因此不能访问非静态（普通）成员变量和成员函数，但可以访问静态成员。调用时推荐使用 ClassName::staticFunc() 的形式。

总之，static 的核心作用就是控制**可见性**、**生命周期**，以及让成员**属于类而非属于对象**。”

### 1.7.4 C++ 的继承 

**ans : **

**`首先 , c++ 支持单继承和多继承 , 不同继承权限下的访问控制符如下 继承方式基类 public 成员基类 protected 成员基类 private 成员public 继承publicprotected不可访问protected 继承protectedprotected不可访问private 继承privateprivate不可访问 , `**

**`其次 , 继承中最常使用的是公有继承 , 它突出了 "is-a" 原则 , 派生类可以直接当基类使用 , 符合里氏替换原则 , `**

**`再然后 , 继承有两大好处 : 1. 代码的复用 2. 基类为派生类提供统一的纯虚函数接口 ,等待派生类重写 , 通过多态 , 基类指针可以访问不同派生类对象的同名覆盖方法`**

**推荐的高分回答版本：**

“C++ 支持单继承和多继承，继承方式主要有三种：公有继承（public）、保护继承（protected）和私有继承（private）。

在访问权限上：

- public 继承：基类的 public 成员在派生类中仍是 public，protected 成员仍是 protected，private 成员不可访问。
- protected 继承：基类的 public 和 protected 成员在派生类中都变为 protected，private 不可访问。
- private 继承：基类的 public 和 protected 成员在派生类中都变为 private，private 不可访问。

其中**公有继承是最常用的**，因为它体现了 ‘is-a’ 关系，派生类可以完全替代基类使用，符合里氏替换原则。

继承的主要好处有两个：

1. **代码复用**：派生类可以直接使用基类中已有的 public 和 protected 成员。
2. **支持多态**：基类提供统一的接口（尤其是虚函数），派生类重写后，通过基类指针或引用可以调用不同派生类的具体实现。

另外，在继承体系中，基类析构函数通常应该声明为虚函数，否则通过基类指针删除派生类对象时只会调用基类析构函数，可能导致资源泄漏。”

### 1.7.5 c++ 多态 , 空间配置器 , vetor 和 list 的区别 , map , 多重 map ?

#### 1.7.5.1 多态

**ans : `c++ 多态分为静态多态和动态多态 ; `**

**`1. 静态多态是编译器在编译期间就已经确定好调用哪个函数了, 这也叫做静态绑定, 减少了函数调用时的开销 `**

**`2. 动态绑定就是虚函数的应用了 , 调用哪个函数是在运行时期确定的 , 也就是动态绑定 `**

**`3. 每一个含有虚函数的类都会有一张 vftable(虚函数表) , 这里面存放的是 虚函数的地址 , 而每一个有虚函数的对象在其内存前都会放一个 vfptr(虚函数指针) , 这个指针指向虚函数表 , 虚函数表是共用的 `**

**`4. 当基类指针找到了派生类的 vfptr , vfptr 会找到相对应的 vftable , 然后通过偏移量找到里面存放的虚函数的地址 , 完成函数的调用 `**

**`5. 如果没有虚析构函数的时候 , 在进行析构的时候 , delete 只会调用基类的析构函数 , 而派生类的对象就没有办法析构掉 , 造成内存资源的泄漏`**

**C++ 多态完整讲解（推荐你认真记）**

**多态（Polymorphism）的定义**： 同一种操作作用于不同的对象，可以有不同的行为（“一个接口，多种实现”）。

**C++ 中多态分为两大类：**

1. **静态多态（编译期多态 / 早绑定）**

   - **实现方式**：函数重载（overload）、运算符重载、模板（template，包括函数模板和类模板）、泛型编程。

   - **特点**：在**编译时期**就确定了调用哪个具体函数（静态绑定）。

   - **优点**：执行效率高（没有运行时开销）。

   - **例子**：func(5) 和 func(3.14) 调用不同的重载函数；模板根据类型实例化不同代码。

2. **动态多态（运行期多态 / 晚绑定）**

   - **实现方式**：**虚函数（virtual function）** + 继承 + 基类指针/引用。

   - **核心**：通过基类指针或引用调用方法时，实际执行的是**派生类**的重写版本。

**动态多态的底层实现原理（必须掌握）**：

1. 虚函数表（vtable）：

   - 每个**有虚函数的类**都会在编译期生成一张**虚函数表**（vtable）。
   - vtable 是一个指针数组，里面存放着该类所有虚函数的**函数地址**。
   - 每个类只有**一份** vtable（类级别）。

2. 虚指针（vptr）：

   - 每个**含有虚函数的对象**在创建时，编译器会在对象内存布局的最前面插入一个**虚指针（vptr）**。
   - vptr 指向**本类**的虚函数表 vtable（对象级别）。

3. 虚函数调用过程

   （面试最爱问）：

   - 当通过基类指针 

     Base* p = new Derived(); 调用虚函数 p->func()时：

     1. 找到指针 p 所指向对象的 vptr。
     2. 通过 vptr 找到该对象的 vtable。
     3. 在 vtable 中查找 func() 对应的函数地址（此时找到的是 Derived::func()）。
     4. 调用该地址的函数。

**为什么需要虚析构函数？**

如果基类析构函数不是 virtual：

- 通过基类指针 delete p; 时，只会调用基类的析构函数。
- 派生类的析构函数不会被调用 → 派生类中的资源（new 的内存、文件句柄、锁等）无法释放 → **内存泄漏或资源泄漏**。

正确做法：基类析构函数应该声明为 virtual ~Base() {}。

#### 1.7.5.2 C++空间配置器 

**ans :** **`空间配置器就是将内存管理和对象生命周期管理分离开来 , 设计原因就是因为 通常容器需要分配一大块内存 ,然后再指定位置构造对象`**

**`四个接口及功能 : allocate : 内存开辟 , deallocate : 内存释放 ; construct : 构造对现象 ; destroy : 销毁对象`**

**`allocate : 是利用 malloc 进行内存开辟 , 然后将开辟的内存返回 ; deallocate : 是利用 free 销毁内存 ; construct 是利用定位new 在指定位置构造对象 ; destroy : 是调用析构函数来销毁对象`**

“空间配置器（Allocator）的主要作用是**将内存的分配/释放与对象的构造/析构完全分离**。

这是因为 STL 容器（如 vector、list 等）经常需要：

- 先分配一大块**原始内存**（raw memory）
- 然后根据需要，在特定位置**构造对象**（而不是一次性构造所有对象）
- 对象销毁后，内存可以继续复用

如果直接使用 new/delete，会把内存分配和对象构造绑定在一起，不够灵活。

**std::allocator<T>** 是 STL 默认的空间配置器，它提供了四个主要接口：

- T* allocate(size_t n)：分配足以存放 n 个 T 对象的**原始内存**（不调用构造函数）
- void deallocate(T* p, size_t n)：释放之前由 allocate 分配的内存（不调用析构函数）
- void construct(T* p, Args&&... args)：在指针 p 所指向的内存位置上构造对象（使用 placement new / 定位 new）
- void destroy(T* p)：调用 p 指向对象的析构函数（不释放内存）

在实现上，现代 std::allocator 的 allocate/deallocate 通常基于 ::operator new / ::operator delete 实现，而老版本的 SGI STL 还实现了著名的**两级空间配置器**：

- 第一级配置器：处理大块内存，直接调用 malloc/free
- 第二级配置器：处理小块内存（≤128 bytes），使用内存池 + 自由链表来减少系统调用，提高分配效率

这样设计既提高了灵活性，又在小对象频繁分配的场景下优化了性能。”

#### 1.7.5.3 vector 和 list 的区别 

**ans : **

**`vector 底层是基于每次二倍扩容的动态数组 , 而 list 的底层则是双向循环链表`**

**`vector 常见的操作及其时间复杂度 , push_back ; O(1) , 在中间位置删除元素 , O(n) , 在头部插入元素 O(n) , 它最大的优点是随机访问 O(1) ; list 常见的操作 , 在中间位置擦混入元素 O(1) , 删除元素 O(1) ,但是这通常是与遍历操作相结合的 , O(n) , list 经常用在插入和删除的操作中`**

- “vector 和 list 是 STL 中两种非常常用的顺序容器，它们的底层实现和适用场景有很大区别：

  **1. 底层数据结构**

  - vector：基于**动态数组**实现，内存连续。当元素数量超过当前容量时，会进行扩容，通常以 **2 倍**（或 1.5 倍）的方式重新分配内存并拷贝元素。
  - list：基于**双向循环链表**实现，每个节点包含数据、前后指针，内存不连续。

  **2. 时间复杂度对比**

  | 操作                  | vector               | list                 |
  | --------------------- | -------------------- | -------------------- |
  | 随机访问 (operator[]) | O(1)                 | O(n)                 |
  | 尾部插入 (push_back)  | 平均 O(1)，最坏 O(n) | O(1)                 |
  | 头部插入 (push_front) | O(n)                 | O(1)                 |
  | 中间插入/删除         | O(n)                 | O(1)（需已有迭代器） |
  | 查找元素              | O(n)                 | O(n)                 |

  **3. 其他重要区别**

  - **内存连续性**：vector 内存连续，缓存友好（Cache Locality 好），适合频繁随机访问；list 内存分散，缓存命中率低。
  - **迭代器失效**：vector 在扩容或中间插入/删除时，迭代器可能会全部失效；list 插入/删除只会使当前迭代器失效，其他迭代器不受影响。
  - **空间开销**：vector 有一定的容量浪费（预分配）；list 每个节点需要额外 2 个指针的空间开销（通常 16 字节/节点）。

  **4. 适用场景**

  - **vector**：适合**频繁随机访问**、**尾部频繁增删**、对内存连续性有要求的场景（如数值计算、存储大量数据）。
  - **list**：适合**频繁在中间或头部进行插入和删除**，但不关心随机访问的场景（如链表操作频繁的业务逻辑）。

  总结来说：**需要快速随机访问选 vector，需要频繁中间插入删除选 list**。”

#### 1.7.5.4 map , 多重map 

**ans : **

**1. 它们底层都是红黑树 , 存储的是键值对 **

**2. map 不允许 key 值重复 , multimap 允许key重复 **

**3. 在实际应用中 , 主要在进行大数据去重/查重时结合使用**

- ### map vs multimap 总结（面试标准版）

  **共同点**：

  - 底层都是 **红黑树**（自平衡二叉搜索树）
  - 存储 <key, value> 键值对
  - key 默认按升序排列
  - 插入、删除、查找时间复杂度均为 **O(log n)**

  **核心区别**：

  - **map**：**key 必须唯一**（不允许重复）
  - **multimap**：**允许 key 重复**

  **map 中插入方式的区别**：

  - m.insert({key, value})：遇到重复 key 时**插入失败**，返回 pair<iterator, false>
  - m[key] = value：遇到重复 key 时**直接覆盖** value；如果 key 不存在，则先默认构造一个 value 再赋值

  **与 unordered_map 的核心对比**：

  | 维度         | map / multimap                      | unordered_map / unordered_multimap |
  | ------------ | ----------------------------------- | ---------------------------------- |
  | 底层结构     | 红黑树                              | 哈希表                             |
  | key 是否有序 | 有序（默认升序）                    | 无序                               |
  | 时间复杂度   | O(log n)                            | 平均 O(1)，最坏 O(n)               |
  | 区间查找     | 支持（lower_bound / upper_bound）   | 不支持                             |
  | 适用场景     | 需要有序、范围查询、按 key 顺序遍历 | 纯高速查找，不关心顺序             |

  **适用场景简述**：

  - 用 map：需要 key 有序、要做范围查找、字典类需求
  - 用 multimap：一个 key 对应多个 value（如成绩单、日志分类）
  - 用 unordered_map：对查找速度要求极高，且不需要顺序

### 1.7.6 C++ 中如何防止内存泄漏 ? 详谈智能指针

**`首先说明一下内存泄漏 : 内存开辟在程序结束后也没有被正常释放 , 或者说都没机会释放 , 就会造成内存泄漏 , 比如说 : 手动开辟了一块内存 , 中间满足了什么条件 return 掉了 , 或者抛出异常直接退出了, 就没有机会释放了 智能指针主要作用 : 不需要程序员手动管理内存 , 在栈上开辟内存 , 随着栈帧的消失 , 内存自动释放 智能指针主要有这五类 : auto_ptr , scoped_ptr , unique_ptr , shared_ptr , weak_ptr auto_ptr : 它主要就管理一个资源, 如果你开辟了多个资源 , 他只会管理最后一个, 前面的资源都会被置成 nullptr scoped_ptr : 这个智能指针智能进行简单的开辟 , 它的底层是把左值的 拷贝构造方法 和 赋值运算符重载方法 delete 掉的 unique_ptr : 这个智能指针虽然说底层也是把左值的 拷贝构造方法 和 赋值运算符重载 delete 掉的 , 但是它提供了带右值引用的拷贝构造方法和赋值运算符重载 现在上面所提到的智能指针都是不带引用计数的 下面是带引用计数的智能指针 : shared_ptr( 强智能指针 ) , weak_ptr(弱智能指针) , weak_ptr 可以通过 .lock() 方法强制转换为 shared_ptr , 它们之间有一个交叉指向的问题 , 就是一块内存用 shared_ptr 开辟的内存 和 另一块 shared_ptr 开辟的内存互相指向 , 引用计数变为 2 , 但是释放的时候 只会释放一次 , 造成资源释放不干净的问题 , 这时候就需要利用 weak_ptr 和 shared_ptr 结合的方法解决 , 因为 shared_ptr 会影响引用计数 , weak_ptr 不会影响引用计数`**

回答整体框架搭起来了，但问题不少，尤其是**概念错误**和**关键细节缺失**非常明显，我来逐一拷打你：

1. **智能指针数量和分类严重错误** 你说“主要有这五类：auto_ptr, scoped_ptr, unique_ptr, shared_ptr, weak_ptr” —— **scoped_ptr 不是标准库里的**，它是 boost 中的东西。 标准库（C++11 之后）主流智能指针只有 **unique_ptr、shared_ptr、weak_ptr**（auto_ptr 已经被废弃）。 请先纠正这个分类错误。

2. **auto_ptr 的描述不准确且已过时** auto_ptr 的最大问题不是“只管理最后一个”，而是**存在严重缺陷**（拷贝时会转移所有权，导致原指针变成 nullptr，且不支持 STL 容器）。 它在 C++11 后已被**废弃**，面试中最好不要重点强调。

3. **unique_ptr 的描述错误** 你说 unique_ptr “把左值的拷贝构造和赋值运算符重载 delete 掉” —— 正确。 但你说它“提供了带右值引用的拷贝构造方法” —— 其实是**移动构造函数和移动赋值运算符**。 请用正确的术语说明。

4. **shared_ptr + weak_ptr 的循环引用描述混乱** 你说“引用计数变为 2，但是释放的时候只会释放一次” —— 完全错误。 正确情况是：循环引用会导致**引用计数永远不会降到 0**，从而**永远无法释放**内存。 请准确描述循环引用的问题和 weak_ptr 的解决方式。

5. 重大遗漏

   （面试最爱问的点）：

   - RAII 思想
   - unique_ptr 和 shared_ptr 的使用场景区别
   - shared_ptr 的引用计数实现原理（控制块）
   - 如何避免循环引用（weak_ptr 的正确用法）
   - 现代 C++ 推荐的做法（尽量用 unique_ptr，shared_ptr 只在需要共享所有权时使用）

#### 正确、完整的讲解（面试推荐版本）

**1. 什么是内存泄漏？**

内存泄漏是指程序在堆上分配的内存，在程序结束前**没有被正确释放**，导致这块内存无法被系统回收。随着程序运行时间变长，内存占用会越来越大，最终可能导致程序崩溃或系统内存不足。

经典例子：

```c++
void func() {
    int* p = new int[100];   // 在堆上分配内存
    // ... 做了很多操作
    if (some_error) return;  // 提前返回或抛出异常
    // 没有执行 delete[] p;
}
```

**2. 智能指针的核心思想 —— RAII**

**RAII**（Resource Acquisition Is Initialization）：资源获取即初始化。

智能指针将资源（这里指堆内存）的**生命周期**绑定到**栈对象**的生命周期上：

- 智能指针对象本身在栈上
- 当栈对象生命周期结束（作用域结束、函数返回、异常抛出等），其析构函数会**自动**释放堆上的内存

这样就从根本上避免了“忘记 delete”的问题。

**3. C++11 之后标准库中的智能指针（重点掌握）**

主要有三种：

**① unique_ptr**（独占所有权智能指针）—— **最推荐使用**

- 同一时刻只有一个 unique_ptr 指向某块内存
- **不支持普通拷贝构造和赋值**（左值拷贝被 delete）
- 支持**移动语义**（移动构造和移动赋值）
- 不带引用计数，性能高、开销小
- 适用场景：独占资源管理、作为函数返回值、作为类成员等

**② shared_ptr**（共享所有权智能指针）

- 允许多个 shared_ptr 指向同一块内存
- 内部维护**引用计数**（control block）
- 只有当引用计数降为 0 时，才真正释放内存
- 开销比 unique_ptr 大（需要维护引用计数 + 控制块）
- 适用场景：真正需要共享所有权的场景

**③ weak_ptr**（弱引用智能指针）

- 不影响引用计数
- 主要用来**解决 shared_ptr 的循环引用问题**
- 不能直接访问对象，需要通过 .lock() 转为 shared_ptr 后再使用

**4. 循环引用问题 + weak_ptr 的解决方式（高频考点）**

```c++
struct A {
    shared_ptr<B> pb;
};
struct B {
    shared_ptr<A> pa;
};

auto a = make_shared<A>();
auto b = make_shared<B>();
a->pb = b;
b->pa = a;   // 形成循环引用
// a 和 b 离开作用域后，引用计数永远是 1 和 1，无法释放！
```

**解决方法**：把其中一个成员改成 weak_ptr：

```c++
struct A {
    shared_ptr<B> pb;
};
struct B {
    weak_ptr<A> pa;   // 改成 weak_ptr
};
```

因为 weak_ptr 不增加引用计数，当强引用（shared_ptr）都销毁后，内存就能正常释放。

### 1.7.7 C++ 如何调用 C语言函数接口 

**C++ 如何调用 C 函数接口（正确解答）**

**1. 根本原因：C 和 C++ 的名字修饰规则（Name Mangling）不同**

- **C 语言**：函数名在编译后基本**保持原样**。例如 int sum(int a, int b) 在符号表中就是 sum。
- **C++ 语言**：支持函数重载，为了区分不同参数的同名函数，编译器会对函数名进行**复杂修饰**（name mangling）。 例如 int sum(int a, int b) 在 C++ 中可能变成 _Z3sumii 这样的名字。

如果不做任何处理，在 C++ 文件中直接 #include 一个 C 的头文件并调用，会出现**链接错误**（Linker Error），因为 C++ 找不到 _Z3sumii 这个符号，而 C 库里只有 sum。

**2. extern "C" 的作用**

extern "C" 告诉**C++ 编译器**： **请不要对这个函数（或这块代码）进行 C++ 的名字修饰，按照 C 语言的编译和链接规则来处理。**

**3. 正确的使用方式**

**方式一：单个函数声明**

```c++
extern "C" int sum(int a, int b);
```

**方式二：多函数或整个头文件（推荐写法）**

在头文件中常用条件编译宏来处理：

```c++
// sum.h
#ifdef __cplusplus
extern "C" {          // 如果是 C++ 编译器，就加上 extern "C"
#endif

int sum(int a, int b);
int max(int a, int b);

#ifdef __cplusplus
}   // extern "C" 结束
#endif
```

在 C++ 文件中直接 #include "sum.h" 即可正常调用。

**方式三：直接在 .cpp 文件中**

```c++
extern "C" {
    #include "c_header.h"   // 直接把整个 C 头文件包进来
}
```

**4. 总结要点**

- extern "C" 只影响**名字修饰**，不影响调用约定（calling convention）等其他内容（特殊情况除外）。
- 它只在 **C++ 编译器** 中起作用，在纯 C 编译器中会被忽略。
- 现代项目中，调用 C 库（如系统调用、第三方 C 库、FFmpeg 等）时，几乎都必须使用 extern "C"。

****

```c++
#ifdef __cplusplus     // 如果是 C++ 编译器
extern "C" {           // 告诉 C++ 编译器：下面的函数用 C 的链接方式
#endif

int sum(int a, int b);   // 函数声明

#ifdef __cplusplus
}                      // extern "C" 结束
#endif
```

这段代码的作用是：**让同一个头文件既能被 C 语言编译，又能被 C++ 语言编译**，并且在 C++ 中正确调用 C 函数。

**详细工作原理：**

1. 当用 C++ 编译器编译时

   （ __cplusplus 宏被定义）：

   - 执行 extern "C" { ... }
   - 告诉 C++ 编译器：**不要对里面的函数进行 name mangling（名字修饰）**。
   - 所以 sum 函数在符号表中仍然是 sum，而不是 _Z3sumii。
   - 这样 C++ 程序就能正确链接到 C 语言编译出来的库函数。

2. 当用纯 C 编译器编译时

   （ __cplusplus 宏没有定义）：

   - extern "C" 这部分代码被跳过。
   - 直接看到正常的函数声明 int sum(int a, int b);
   - 完全按照 C 语言的规则编译（符号表就是 sum）。

------

**正确的最简洁用法**（实际项目中最常见）：

```c++
extern "C" {
    #include "your_c_header.h"   // 直接把整个 C 头文件包起来
}
```

或者在 C 头文件中自己加条件编译保护（如上面标准写法）。

### 1.7.8 C++ 什么时候会出现访问越界 

1. 超出数组范围 , 访问越界 
2. vector , 当 std::vector vec; vec[2] 就会访问越界 
3. std::string 同理 

4. 字符串未添加 '\0' , 当进行字符串的遍历的时候 , 访问越界 
5.  当进行类型强转的时候 , 当一个大类型指针指向一个小类型内存 , 对大类型指针进行解引用会造成越界访问 : 比如派生类指针指向基类对象 , 对派生类指针进行解引用时造成越界访问

**C++ 访问越界完整讲解（面试高频）**

访问越界是指程序访问了**不属于自己合法内存范围**的地址，这属于**未定义行为（Undefined Behavior）**，后果非常严重：可能崩溃、数据损坏、被黑客利用等。

**1. 数组访问越界（最经典）**

```c++
int arr[10];
arr[10] = 100;     // 越界！合法索引是 0~9
arr[-1] = 100;     // 越界！
```

**特别危险**：栈上数组越界可能破坏栈帧、返回地址，导致程序莫名其妙崩溃。

**2. std::vector 访问越界**

```c++
std::vector<int> vec(5);   // size = 5, capacity 可能更大

vec[5] = 100;              // 越界！使用 operator[] 不做边界检查
vec.at(5);                 // 安全，会抛出 std::out_of_range 异常
```

**重要区别**：

- operator[] ：不检查边界，速度快，**越界是未定义行为**
- at() ：会检查边界，越界抛异常（推荐在调试时使用）

**3. std::string 访问越界**

```c++
std::string s = "hello";   // length = 5

s[5] = '!';                // 越界！合法范围是 0~4
s.at(5);                   // 抛异常
```

注意：s[5] 虽然越界，但因为 string 内部通常会在末尾预留 '\0'，有时看起来“没事”，但仍是**未定义行为**。

**4. C风格字符串未加 '\0' 导致越界**

```c++
char buf[10];
strcpy(buf, "hello world");   // 源字符串长度 11 + '\0' = 12 > 10，缓冲区溢出
```

或者手动构造字符串忘记加 '\0'，用 strlen、printf 等函数遍历时会一直读到内存中下一个 '\0'，造成严重越界。

**5. 类型强转 + 指针解引用导致的越界（你提到的点）**

```c++
struct Base { int a; };
struct Derived : Base { int b; int c; };

Base b;
Derived* d = reinterpret_cast<Derived*>(&b);   // 危险的类型转换

d->c = 100;   // 越界访问！因为实际只有 Base 的大小，c 的位置已经超出对象内存
```

这种属于**对象切片 + 错误类型转换**，非常危险。

**6. 其他常见访问越界场景**

- 使用未初始化的指针：int* p; *p = 10;
- delete 后继续使用指针（悬挂指针 / dangling pointer）
- 迭代器失效后继续使用（vector 扩容后旧迭代器失效）
- 多线程下没有同步保护的共享内存访问
- 栈缓冲区溢出（经典安全漏洞）

------

**预防访问越界的建议（面试加分点）**

1. 优先使用 STL 容器（vector、string、array）而不是裸数组
2. 使用 at() 而不是 operator[] 做边界检查（调试阶段）
3. 使用智能指针（unique_ptr、shared_ptr）管理内存
4. 使用 std::array 替代固定大小数组
5. 开启编译器警告和 AddressSanitizer（ASan）等工具检测
6. 严格进行类型转换时使用 static_cast、dynamic_cast，避免 reinterpret_cast

### 1.7.9 C++ 中类的初始化列表 

**C++ 类初始化列表（Member Initializer List）—— 面试高频重点解析**

**1. 初始化列表 vs 构造函数体内赋值 的本质区别**

```c++
class Test {
    int a;
    std::string s;
public:
    Test(int x, std::string str) 
        : a(x), s(str)          // 初始化列表
    {
        // 函数体
    }
};
```

**初始化列表**是在**构造函数函数体执行之前**就完成的。 构造函数体内赋值是在**函数体开始执行之后**才做的。

**2. 为什么推荐使用初始化列表？（效率 + 必须使用的场景）**

**主要原因有 4 个**（面试最爱考）：

1. 效率更高

   对于类类型成员（std::string、std::vector 等），如果在函数体内赋值，会发生两次构造：

   - 先默认构造一次（初始化列表阶段）
   - 再调用 operator= 赋值一次 而使用初始化列表可以**直接调用带参构造函数**，避免一次多余的默认构造。

2. 必须使用初始化列表的情况（这几个是重中之重）：

   - **const 成员变量**（常量，必须初始化，不能赋值）
   - **引用成员变量**（引用必须在初始化时绑定，不能重新赋值）
   - **没有默认构造函数的成员对象**（无法进行默认构造）
   - **基类需要带参数的构造函数**（派生类构造基类时）

3. **初始化顺序问题** 成员变量的**初始化顺序**是按照**它们在类中声明的顺序**进行的，**和初始化列表中写的顺序无关**。 这是一个非常常见的坑。

4. 只能在初始化列表中初始化的特殊情况：

   - const 成员
   - 引用成员
   - 没有默认构造函数的类类型成员

### 1.8.1 C 和 C++ 的区别 ? C 和 C++ 的内存分布有什么区别 ?

**C语言是面向过程的语言 , 偏向更底层 , 性能稍高一些**

**C++ 是多范式语言 : 在面向过程中兼容 C , 还有面向对象 , 泛型编程 , 函数式编程**

**另外 , C++ 有引用 , 有重载 , 类和对象 , 三大特性 : 封装继承多态 , 有泛型算法, 有模板 , 有异常处理 , 命名空间 , 智能指针等c++11/14/17/20/23 一系列新特性**

**内存一样的部分 : 是这么划分的 : user space , .text .rodata , .data , .bss , heap , stack , 环境变量 , karnel space(ZONE_DMA , ZONE_NORMAL , ZONE_HIGHMEM)**

**不一样的部分 : c++ 中类可能包含隐藏数据 : 例如继承 有的 vfptr , vftable ; 虚继承有的 : vbptr , vbtable**

****

**推荐的清晰 + 深入版回答（学习阶段建议你掌握这个水平）：**

**C 和 C++ 的主要区别：**

C 是一门**面向过程**的编程语言，设计理念更接近底层硬件，强调代码的执行效率和简洁性。

C++ 则是一门**多范式**编程语言，它在完全兼容 C 的基础上，新增了：

- **面向对象编程**（类、封装、继承、多态）
- **泛型编程**（模板机制 + STL 标准模板库）
- **现代编程特性**（引用、异常处理、RAII、智能指针、lambda、constexpr 等 C++11/14/17/20 新特性）

因此，C++ 的抽象能力和代码复用性远强于 C，但也带来了更复杂的语言机制（如名字修饰、虚函数表等）。

**内存分布上的区别：**

两者**整体内存模型基本相同**，都分为：

- **代码段（.text）**：存放可执行指令
- **只读数据段（.rodata）**：常量、字符串字面量、虚函数表
- **数据段（.data）**：已初始化的全局/静态变量
- **BSS 段**：未初始化的全局/静态变量
- **堆（heap）**：动态内存分配（new/malloc）
- **栈（stack）**：局部变量、函数调用信息

**不同点主要体现在 C++ 的对象布局上**：

- C 的 struct 是简单的数据聚合，布局简单。
- C++ 的 class 对象在以下情况会增加额外内存：
  - 含有虚函数时，对象最前面会多一个**虚指针 vptr**，指向虚函数表 vtable（vtable 存放在 .rodata 段）
  - 多继承时，对象中可能存在**多个 vptr**
  - 虚继承（解决菱形继承）时，还会引入**虚基类指针 vbptr** 和对应的虚基类表

总结来说：C 的内存布局更简单、可预测；C++ 的内存布局在支持面向对象和多态特性时会引入一定的额外开销和复杂性。

### 1.8.2 int* const p 和 const int* p 的区别 

**int* const 修饰的是指针本身 , p 不能被重新指向别的地址 , 但是*p 可以被修改 ***

***const int* p 修饰的是指针指向的内容 , *p 不能被修改 , 但是 p 能被重新指向别的地址**

**完美/高分回答版本（推荐你记住这个）：**

int* const p 和 const int* p 的区别在于 **const 修饰的位置不同**，这决定了 const 到底修饰的是**指针本身**还是**指针指向的内容**。

1. int* const p（const 修饰指针本身）：

   - 指针 **p** 是常量，不能被重新指向其他地址。

   - 但指针指向的内容 **(\*p)** 可以被修改。

   - 例子：

     ```c++
     int a = 10, b = 20;
     int* const p = &a;   // p 初始化后不能再改
     *p = 30;             // 正确，可以修改指向的内容
     // p = &b;           // 错误！指针本身不能重新赋值
     ```

2. const int* p（const 修饰指针指向的内容，也可写成 int const* p）：

   - 指针指向的内容 **(\*p)** 是常量，不能通过 p 修改。

   - 但指针 **p** 本身可以指向其他地址。

   - 例子：

     ```c++
     int a = 10, b = 20;
     const int* p = &a;
     // *p = 30;         // 错误！不能修改指向的内容
     p = &b;              // 正确，指针可以重新指向别的地址
     ```

**扩展**（经常一起问）：

- const int* const p：指针本身和指向的内容**都不能修改**（最严格）。

### 1.8.3 malloc 和 new 的区别 

**首先 , malloc 是通过字节数来开辟内存的 , new 是通过 operator new 来开辟内存**

**malloc 只开辟内存不进行初始化 , new 不仅开辟内存 , 还进行初始化**

**malloc 开辟内存失败返回 nullptr , new 开辟内存失败是抛出异常**

****

**malloc vs new 完整对比（推荐记忆版）**

| 对比维度              | malloc / free                          | new / delete                                  |
| --------------------- | -------------------------------------- | --------------------------------------------- |
| **所属语言**          | C 语言                                 | C++ 语言                                      |
| **内存分配方式**      | 只分配原始内存（raw memory）           | 分配内存 + 调用构造函数                       |
| **初始化行为**        | 只分配内存，**不初始化**               | 分配内存后自动调用构造函数进行初始化          |
| **类型安全**          | **不安全**（返回 void*，需要手动强转） | **安全**（返回具体类型的指针，无需强转）      |
| **失败处理**          | 返回 NULL / nullptr                    | 默认抛出 std::bad_alloc 异常                  |
| **构造函数/析构函数** | **不调用**                             | new 调用构造函数，delete 调用析构函数         |
| **数组支持**          | malloc 可以分配数组，但不调用构造函数  | 需要用 new[] / delete[] 才能正确处理数组      |
| **是否可重载**        | **不能重载**                           | operator new / operator delete **可以被重载** |
| **底层实现**          | 通常直接调用底层内存分配函数           | 调用 operator new，然后构造对象               |

**详细解释（重点内容）**

1. **最核心区别**：

   - malloc：只负责**分配内存**，什么都不干。
   - new：是表达式，做了两件事：
     1. 调用 operator new 分配内存
     2. 调用对象的**构造函数**进行初始化

   对应地：

   - free：只释放内存
   - delete：先调用**析构函数**，再调用 operator delete 释放内存

2. **类型安全**：

   - malloc 返回 void*，必须手动强转：int* p = (int*)malloc(sizeof(int));
   - new 直接返回正确类型：int* p = new int;

3. **数组的区别**（非常重要！）：

   - malloc：int* p = (int*)malloc(10 * sizeof(int)); —— 不会调用构造函数
   - new[]：int* p = new int[10]; —— 会对每个元素调用构造函数
   - 释放时必须严格匹配：malloc 用 free，new[] 必须用 delete[]

4. **失败时的行为**：

   - malloc 失败返回 nullptr

   - new 失败默认抛异常，但可以这样不抛异常：

     ```c++
     int* p = new (std::nothrow) int[100];
     ```

5. **能否重载**：

   - malloc/free 不能重载
   - new/delete 可以重载 operator new / operator delete，实现自定义内存分配器

### 1.8.4 map & set 容器的实现原理 

**map & set 容器的实现原理（标准答案）**

**1. 底层数据结构**

- **std::map** / **std::multimap** / **std::set** / **std::multiset** 的**底层实现都是红黑树**（Red-Black Tree，一种自平衡二叉搜索树）。

具体区别在于：

| 容器     | 底层结构 | 存储内容            | key 是否允许重复 |
| -------- | -------- | ------------------- | ---------------- |
| map      | 红黑树   | <key, value> 键值对 | 不允许重复       |
| multimap | 红黑树   | <key, value> 键值对 | 允许重复         |
| set      | 红黑树   | 只有 key            | 不允许重复       |
| multiset | 红黑树   | 只有 key            | 允许重复         |

**为什么使用红黑树？**

- 红黑树能保证树的高度平衡，最坏情况下时间复杂度仍是 **O(log n)**。
- 插入、删除、查找都比较稳定。
- 能保持元素**有序**（按 key 自动排序）。

**2. map vs set 的核心区别**

- **map**：存储的是**键值对** <key, value>，通过 key 来查找对应的 value。
- **set**：只存储 **key**，本身就是一个**有序集合**，常用于去重 + 排序。

**3. 与 unordered_map / unordered_set 的本质区别（高频对比）**

| 维度         | map / set (红黑树)                       | unordered_map / unordered_set (哈希表) |
| ------------ | ---------------------------------------- | -------------------------------------- |
| 底层数据结构 | 红黑树                                   | 哈希表（链地址法）                     |
| key 是否有序 | **有序**（默认按 key 升序）              | **无序**                               |
| 时间复杂度   | 插入、查找、删除均为 **O(log n)**        | 平均 **O(1)**，最坏 **O(n)**           |
| 内存开销     | 较高（每个节点有颜色 + 左右父指针）      | 一般较低                               |
| 额外能力     | 支持 lower_bound、upper_bound 等区间查找 | 不支持有序操作                         |
| 适用场景     | 需要**有序**、需要范围查找、数据量中等   | 追求**极致查找速度**，不关心顺序       |

**红黑树的优势**：

- 性能稳定（最坏情况也是 O(log n)）
- 天然有序，可直接做范围查询

**哈希表的优势**：

- 平均查找速度更快（O(1)）

**实际选择建议**：

- 如果**需要按 key 排序**或**范围查找**（如 lower_bound）→ 用 map / set
- 如果**只关心查找速度**，且不关心顺序 → 优先用 unordered_map / unordered_set

### 1.8.5 shared_ptr 引用计数存在哪里?

shared_ptr 的引用计数**并不存储在 shared_ptr 对象本身里面**，而是存储在**一块独立的内存区域**，叫做 **控制块（Control Block）**。

#### 详细说明：

当你第一次创建 shared_ptr 时（比如用 make_shared 或 new），会发生两件事：

1. **分配对象内存**（存放实际的对象数据）
2. **额外分配一块控制块（Control Block）**

这块**控制块**里主要存放：

- **强引用计数**（strong reference count）—— shared_ptr 的数量
- **弱引用计数**（weak reference count）—— weak_ptr 的数量
- 对象的指针（指向实际分配的对象）
- 析构函数 / 删除器（deleter）
- 其他辅助信息

#### 内存布局示意图（简化）：

```assembly
shared_ptr1 ─────┐
shared_ptr2 ─────┼──→ 控制块 (Control Block)
shared_ptr3 ─────┘      ├── 强引用计数 = 3
                        ├── 弱引用计数 = 1
                        └── 指向实际对象
                           ↓
                        实际对象内存 (User Data)
```

#### 重要行为：

- 每当有一个新的 shared_ptr 指向同一个对象时，**强引用计数 +1**
- 当一个 shared_ptr 销毁时，**强引用计数 -1**
- **只有当强引用计数变为 0 时**，才会调用对象的析构函数，并释放对象内存
- 弱引用计数变为 0 时，才会释放控制块本身

#### 为什么不把引用计数直接放在对象里面？

因为对象可能是用户自定义类型，C++ 不允许随便在用户对象里加成员。 所以必须把引用计数放在**独立分配的控制块**中。

------

#### 补充高频追问点：

1. make_shared 的优势：
   - make_shared<T>() 会**一次性分配**对象内存 + 控制块内存，减少一次内存分配，提高性能和缓存友好性。
2. 循环引用问题：
   - 如果两个对象互相用 shared_ptr 持有对方，强引用计数永远不会变成 0 → 内存泄漏。
   - 解决办法：使用 weak_ptr 打破循环（weak_ptr 不增加强引用计数）。

****

#### **最终推荐的高分回答**（你可以直接用这个版本）：

“shared_ptr 的引用计数并不存储在 shared_ptr 对象本身里面，而是存储在一块**额外分配的控制块（Control Block）**中。

当我们创建一个 shared_ptr 时，通常会进行两次内存分配（或 make_shared 一次分配）：

- 一块是存放实际对象的内存

- 另一块是

  控制块

  ，里面主要包含：

  - 强引用计数（strong reference count）
  - 弱引用计数（weak reference count）
  - 指向实际对象的指针
  - 自定义删除器（deleter）
  - 其他辅助信息

每当有一个新的 shared_ptr 指向同一个对象时，强引用计数就会 +1； 当一个 shared_ptr 被销毁时，强引用计数 -1。 **只有当强引用计数减为 0 时**，才会调用对象的析构函数并释放对象内存。 而控制块本身会在弱引用计数也减为 0 时才被释放。

这就是 weak_ptr 能打破循环引用的关键——它只增加弱引用计数，不影响强引用计数。”

### 1.8.6 STL , map 底层 , deque 底层 , vector 中的 empty() 和 size() 的区别 , 函数对象 ?

**STL 中有容器 : vector , list , deque , map , unoredred_map , set , unordered_set 等等 ; 有容器适配器 : stack , queue , priority_queue, 泛型算法 , 迭代器 , 空间配置器等等 ; **

**map 红黑树的优势就是在进行插入和删除节点的时候树叶的旋转次数更少, 相较于 AVL 数有更好的效率, 并且能够天然保持有序 ; deque 不是简答的二维数组 , 而是分段连续存储结构 ; **

**vector 中的 empty() 和 size() 的区别是这样: vector 有三个指标 _first , _last , _ end , empty() 就是 _first == _end ; size() 就是 _last - _first ; **

**函数对象呢就是重载了 operator() 的类对象 , 它能够使类对象像函数一样调用**

#### 参考回答：

“STL（Standard Template Library）主要由六大组件组成：容器、算法、迭代器、容器适配器、函数对象和空间配置器。

**容器**主要分为序列容器和关联容器：

- 序列容器：vector、deque、list 等
- 关联容器：map、set、multimap、multiset、unordered_map、unordered_set 等
- 容器适配器：stack、queue、priority_queue

**map / set 的底层实现**是**红黑树**（一种自平衡二叉搜索树）。 选择红黑树的原因是：

- 插入、删除、查找的时间复杂度都是 O(log n)，且最坏情况也能保持这个复杂度。
- 相比 AVL 树，红黑树在插入和删除节点时旋转次数更少，整体效率更高。
- 能天然保持元素有序，支持 lower_bound、upper_bound 等区间操作。

**deque 的底层**采用**分段连续存储结构**：

- 它维护一个中控器（map），中控器里存放多个指针，每个指针指向一块固定大小的连续内存块。
- 这样既保证了逻辑上的连续性，又能在头部和尾部实现高效的插入和删除（O(1)）。

**vector 中 empty() 和 size() 区别 : **

- **vector 内部指针的正确含义（以 libstdc++ 为例）：**

  vector 内部通常维护三个指针：

  - _M_start ：指向**数组的起始位置**（第一个元素）
  - _M_finish ：指向**当前最后一个元素的下一个位置**（即已使用空间的末尾）
  - _M_end_of_storage ：指向**已分配内存空间的末尾**（capacity 的末尾）

  正确对应关系如下：

  - size() = _M_finish - _M_start
  - capacity() = _M_end_of_storage - _M_start
  - empty() = (_M_finish == _M_start)   或   size() == 0

**函数对象（Functor）**是指重载了 operator() 的类对象。它可以像普通函数一样被调用，但可以保存状态（成员变量），在 STL 算法（如 sort、for_each）中经常作为谓词使用。

### 1.8.7 STL 中迭代器失效的问题 

**vector从插入位置到末尾位置的所有迭代器都会失效, 如果是 push_back() 引发扩容了 , 也会导致所有迭代器失效 , 内存重新分配 ,迭代器指向的地址无效 **

**list 不会失效 , 因为list 底层是双向循环链表 , 它的数据结构里有数据域和地址域, 地址域指向下一个节点**

**删除及时更新迭代器就比如这样 auto it = erase(it);**

#### 最终总结（面试推荐回答版本）

**STL 中迭代器失效问题：**

不同容器的迭代器失效规则差异很大，主要取决于底层数据结构：

1. vector（动态数组）：
   - **insert**：从**插入位置开始到末尾**的所有迭代器都会失效。
   - **erase**：从**被删除位置开始到末尾**的所有迭代器都会失效。
   - **push_back**：如果**没有触发扩容**，只有 end() 迭代器失效；如果**触发扩容**，则**所有迭代器全部失效**（因为内存重新分配，旧地址无效）。
   - **原因**：vector 内存连续，插入/删除会导致后续元素移动，或整个内存块重新分配。
2. list（双向链表）：
   - **insert** 和 **erase** 操作**只会使当前被操作的迭代器失效**，其他迭代器**全部不受影响**。
   - **原因**：链表节点通过指针连接，插入/删除只会改变前后节点的指针，不会移动已有节点的内存地址。
3. deque：
   - 在头部或尾部插入/删除时，迭代器**可能失效**（比 vector 好一些，但不是完全安全）。

**安全删除元素的正确写法**（高频考点）：

```c++
// vector 中安全删除满足条件的元素
for (auto it = v.begin(); it != v.end(); ) {
    if (*it == value) {
        it = v.erase(it);     // erase 返回下一个有效迭代器
    } else {
        ++it;
    }
}

// list 中可以这样（更安全）
for (auto it = lst.begin(); it != lst.end(); ) {
    if (*it == value) {
        it = lst.erase(it);
    } else {
        ++it;
    }
}
```

### 1.8.8 STL 中哪些底层由红黑树实现 

**具体是 map , multimap , set , multiset , map 存储的是键值对<key , value>  , set 存储的是<key>  **

**map 和 multimap 的区别 : map 不允许 key 值重复 , multimap 允许 key 值重复 , map 的不重复 , 但是要插入重复 key 有这两方面 : 第一种 : insert , 如果已经存在 key , 就会返回 {iterator , false} , 如果是 operator[] , 就会覆盖掉原有的 key , multimap 允许重复 , 红黑树会进行插入 , 一般是在重复元素的平行位置 ; **

**红黑树能很好的维持树的高度 ,而且在进行插入删除操作的时候 ,树的旋转次数降低, 效率变高, 开销变小**

#### **最终推荐的高分回答**（面试中可以直接使用的版本）：

“STL 中底层使用**红黑树**实现的容器主要有四个：map、multimap、set、multiset。

- **map**：存储的是键值对 <key, value>，**key 不允许重复**。
- **multimap**：同样存储键值对 <key, value>，但**允许 key 重复**。
- **set**：只存储 **key**，**key 不允许重复**（本质是一个有序集合）。
- **multiset**：只存储 **key**，**允许 key 重复**。

**map 和 multimap 的区别**：

- map 不允许 key 重复，当使用 insert() 插入已存在的 key 时，会插入失败，返回 pair<iterator, false>；
- 当使用 operator[] 插入已存在的 key 时，会直接覆盖原来的 value。
- multimap 允许 key 重复，插入时不会检查 key 是否已存在，而是直接按照红黑树的规则插入到合适位置（通常在相等 key 的右侧）。

**为什么选择红黑树？** 红黑树是一种自平衡二叉搜索树，它能在插入和删除节点时通过较少的旋转操作维持树的平衡，从而保证在最坏情况下插入、删除、查找的时间复杂度仍为 **O(log n)**。同时，它能天然保持元素的**有序性**，支持 lower_bound、upper_bound 等高效的区间查找操作。

相比之下，unordered_map 和 unordered_set 使用哈希表实现，平均查找速度更快（O(1)），但不保持有序，且最坏情况下可能退化为 O(n)。

### 1.8.9 struct 和 class 的区别 

#### **面试中推荐的高分回答**（结构清晰版）：

“C++ 中 struct 和 class **本质上是完全相同的**，它们唯一的区别在于**默认访问权限**和**默认继承方式**。

**具体区别如下：**

1. 默认访问权限：
   - struct 默认访问权限是 public
   - class 默认访问权限是 private
2. 默认继承方式：
   - struct 默认继承方式是 public 继承
   - class 默认继承方式是 private 继承

**实际使用习惯上的区别**（面试中最常被追问）：

- struct：通常用来表示简单的数据集合

  （Plain Old Data，POD），或者数据结构节点。它的语义更接近“数据包”，成员一般以 public 为主，较少包含复杂的成员函数和继承关系。 例如：

  ```c++
  struct Node {
      int x, y;
      int val;
      // 很少放复杂的成员函数
  };
  ```

- class

  ：通常用来表示

  具有行为和状态的完整对象，强调封装、继承和多态。成员默认是 private，适合实现复杂的业务逻辑、接口抽象等。 例如：

  ```c++
  class Maze {
  private:
      std::vector<std::vector<int>> grid;
      // ... 各种成员函数
  public:
      bool dfs(int x, int y);
      // ...
  };
  ```

总结来说，虽然 C++ 中 struct 和 class 在语法上几乎没有区别，但**使用习惯上**：

- struct 更偏向“数据结构”
- class 更偏向“面向对象的设计单元”

在实际项目中，很多人会统一使用 class 来保持风格一致，但 struct 在表示简单数据节点时仍然非常常见。

### 1.9.0 vector 和 数组的区别 , STL 的容器分类 , 各容器底层实现 

#### 1. std::vector 和 std::array 的区别

| 维度             | std::vector                 | std::array (C++11)                |
| ---------------- | --------------------------- | --------------------------------- |
| **底层结构**     | 动态数组（可变大小）        | 固定大小数组（静态数组）          |
| **内存分配**     | 堆上动态分配，可自动扩容    | 栈上（或对象内部），大小固定      |
| **大小**         | 运行时可变（size() 可变）   | 编译时固定（size() 是编译期常量） |
| **扩容**         | 支持，容量以约 2 倍方式增长 | 不支持扩容，容量固定              |
| **性能**         | 随机访问快，但扩容时有开销  | 随机访问极快，无扩容开销          |
| **内存连续性**   | 连续                        | 连续                              |
| **是否可以拷贝** | 可以                        | 可以（支持聚合初始化）            |
| **适用场景**     | 数据量不确定，需要动态增删  | 数据量固定，且对性能要求极高      |

**总结一句话**：

- vector 是**可变长**的动态数组，适合大多数需要动态增长的场景。
- array 是**固定长**的安全数组，替代传统 C 风格数组，更安全（有边界检查版本 at()），性能更高。

------

#### 2. STL 容器分类及底层实现（完整版）

STL 容器主要分为三大类：

##### **① 序列容器（Sequence Containers）** —— 元素按插入顺序排列

- vector：动态数组，底层是**连续内存**，支持随机访问。扩容时通常按 2 倍增长。
- deque：双端队列，底层是**分段连续存储**（中控器 + 多个固定大小内存块），头部和尾部插入删除都是 O(1)。
- list：双向链表，底层是**双向循环链表**，插入删除 O(1)，但随机访问 O(n)。
- array（C++11）：固定大小数组，底层是**静态数组**，大小不可变。
- forward_list（C++11）：单向链表，节省空间，但只能单向遍历。

##### **② 关联容器（Associative Containers）** —— 按 key 自动排序

- set / multiset：底层 **红黑树**，只存 key。
- map / multimap：底层 **红黑树**，存 <key, value> 键值对。
- 特点：自动有序，插入/查找/删除均为 O(log n)。

##### **③ 无序关联容器（Unordered Associative Containers）** —— 哈希实现

- unordered_set / unordered_multiset
- unordered_map / unordered_multimap
- 底层：**哈希表**（链地址法）
- 特点：平均查找 O(1)，不保持有序，最坏 O(n)。

##### **④ 容器适配器（Container Adapters）** —— 基于其他容器封装

- stack：默认基于 deque
- queue：默认基于 deque
- priority_queue：默认基于 vector + 堆

### 1.9.1 编译链接全过程 

#### C/C++ 编译链接全过程（四个主要阶段）

从 .cpp 源文件到最终可执行程序，一共经历**四个阶段**：

##### 1. **预处理阶段（Preprocess）**

- **命令**：g++ -E test.cpp -o test.i
- 主要工作：
  - 处理所有 # 开头的预处理指令（#include、#define、#ifdef 等）
  - 展开头文件（把头文件内容插入到源文件中）
  - 展开宏定义
  - 处理条件编译（#if、#else、#endif）
  - 删除注释
- **输出文件**：.i 文件（预处理后的纯文本源代码）

##### 2. **编译阶段（Compile）**

- **命令**：g++ -S test.i -o test.s
- 主要工作：
  - 将预处理后的 .i 文件翻译成**汇编语言**（.s 文件）
  - 进行词法分析、语法分析、语义分析
  - 生成中间代码并进行优化
- **输出文件**：.s 文件（汇编代码）

##### 3. **汇编阶段（Assemble）**

- **命令**：g++ -c test.s -o test.o
- 主要工作：
  - 将汇编代码翻译成**机器指令**
  - 生成**目标文件**（Object File）
- **输出文件**：.o 文件（二进制目标文件，包含机器码、符号表等）

##### 4. **链接阶段（Link）**

- **命令**：g++ test.o -o test
- 主要工作：
  - 将多个目标文件（.o）以及所依赖的库文件合并在一起
  - 解决符号引用问题（把函数调用和变量定义连接起来）
  - 生成最终的可执行文件
- **输出文件**：可执行文件（Windows 下是 .exe，Linux 下是无后缀可执行文件）

------

#### 完整流程图（记忆版）

.cpp → **[预处理]** → .i → **[编译]** → .s → **[汇编]** → .o → **[链接]** → **可执行文件**

#### 额外高频补充点：

- 静态链接 vs 动态链接：
  - 静态链接：把库文件直接打包进可执行文件（体积大，但运行时不需要库文件）
  - 动态链接：运行时再加载 .so/.dll（体积小，但需要库文件存在）
- 常见的编译选项：
  - -E：只预处理
  - -S：只编译到汇编
  - -c：只编译不链接
  - -o：指定输出文件名

### 1.9.2 初始化全局变量和未初始化全局变量有什么区别?

“初始化全局变量和未初始化全局变量在内存中的存放位置不同：

- **已初始化且值不为 0** 的全局变量和静态变量存放在 **.data 段**（已初始化数据段），其初始值会保存在可执行文件中。
- **未初始化** 的全局变量，以及**初始化为 0** 的全局变量存放在 **.bss 段**（未初始化数据段）。 .bss 段不占用可执行文件的空间，程序启动时由操作系统自动将其清零。

这样设计的主要目的是节省可执行文件的大小，同时加快程序加载速度。”

#### 标准、完整答案（面试推荐版）

在 C/C++ 中，全局变量和静态变量（static 全局/局部变量）在内存中的存储位置取决于**是否初始化以及初始化值是否为 0**：

| 变量类型                                       | 存储段    | 说明                                                         |
| ---------------------------------------------- | --------- | ------------------------------------------------------------ |
| **已初始化且值不为 0** 的全局/静态变量         | **.data** | 存放初始化数据，程序启动时会从可执行文件中加载具体值         |
| **未初始化** 或 **初始化为 0** 的全局/静态变量 | **.bss**  | 只记录大小，不占用可执行文件空间，程序启动时由操作系统自动清零 |

##### 详细解释：

1. .data 段（已初始化数据段）
   - 存放**显式初始化且不为 0** 的全局变量和静态变量。
   - 这些变量的初始值会直接保存在可执行文件中。
   - 程序加载时，操作系统会把 .data 段的内容从文件拷贝到内存。
2. .bss 段（未初始化数据段）
   - 存放**未初始化**的全局/静态变量，以及**初始化为 0** 的全局/静态变量。
   - **不占用可执行文件的大小**（只记录段的大小和位置）。
   - 程序启动时，操作系统会自动把整个 .bss 段的内容初始化为 0（这是操作系统提供的特性）。

##### 为什么这样设计？

- **节省磁盘空间**：如果把大量初始化为 0 的变量都放在 .data 段，可执行文件会变得很大。而 .bss 段只需要记录“这里有 XX 字节需要清零”，非常节省空间。
- **启动速度**：.bss 段由操作系统一次性清零，比从文件中加载快。

### 1.9.3 堆和栈的区别 ?

#### 核心总结（面试时可以说）：

- **栈**：由系统自动管理，速度快、空间小、安全性高，适合存放**生命周期短**的小数据。
- **堆**：由程序员手动管理，空间大、灵活，但速度慢、容易出错（内存泄漏、碎片），适合存放**大对象**或**生命周期不确定**的数据。

#### 堆（Heap） 和 栈（Stack） 的区别

| 对比维度         | **栈（Stack）**                                              | **堆（Heap）**                                            |
| ---------------- | ------------------------------------------------------------ | --------------------------------------------------------- |
| **管理方式**     | 由**编译器/系统自动管理**（函数调用时自动分配，返回时自动释放） | 由**程序员手动管理**（new/malloc 分配，delete/free 释放） |
| **分配速度**     | **非常快**（只需移动栈顶指针）                               | **较慢**（需要查找合适空闲块，可能涉及系统调用）          |
| **内存大小**     | **较小**且固定（通常几 MB 到几十 MB，由系统决定）            | **很大**（可达系统可用虚拟内存上限）                      |
| **生命周期**     | **自动**：随函数作用域结束而自动销毁                         | **手动**：程序员不释放就会造成内存泄漏                    |
| **内存碎片**     | **几乎没有**（连续分配，先进后出）                           | **容易产生**（频繁分配释放导致内存碎片）                  |
| **访问方式**     | 连续访问，**缓存友好**（局部性好）                           | 随机访问，缓存命中率较低                                  |
| **分配失败后果** | 栈溢出（Stack Overflow），程序通常直接崩溃                   | 返回 NULL 或抛出异常，可通过检查处理                      |
| **适用场景**     | 局部变量、函数参数、临时对象                                 | 大对象、动态大小的数据、需要长时间存在的数据              |

**额外高频点**：

- 栈是**线程私有**的，每个线程都有自己的栈。
- 堆是**进程共享**的，多个线程可以访问同一块堆内存（需要同步）。
- 递归过深容易导致**栈溢出**，而堆如果不释放会慢慢耗尽内存。

### 1.9.4 构造函数和析构函数可不可以是虚函数? 为什么?

#### **最终高分回答版本**（推荐你记住这个）：

**构造函数不能是虚函数**，核心原因有两点：

1. 对象在构造函数执行期间**还没有完全构造好**，虚函数表指针（vptr）尚未被正确初始化。
2. 虚函数依赖于**动态绑定**（通过 vptr 查找 vtable），而构造函数执行时，对象的类型信息还是“正在构造的当前类”，而不是最终的派生类类型。因此即使把构造函数声明为虚函数，也无法实现预期的多态行为。

**析构函数可以并且推荐声明为虚函数**。

原因如下： 当我们通过**基类指针**指向派生类对象时：

```c++
Base* p = new Derived();
delete p;     // 如果基类析构函数不是 virtual
```

- 如果基类析构函数**不是虚函数**，则只会调用 Base 的析构函数，Derived 的析构函数不会被调用，导致派生类中的资源无法释放，造成内存泄漏或资源泄漏。
- 如果基类析构函数**是虚函数**，则会通过虚函数机制正确调用 Derived 的析构函数，再自动调用 Base 的析构函数，实现正确的清理顺序。

**总结**：

- 构造函数**不能**是虚函数（语言层面也不允许）。
- 析构函数**应该**是虚函数，尤其是当类作为基类被继承时（基类析构函数通常要声明为 virtual）。

### 1.9.5 构造函数和析构函数能不能抛出异常 , 为什么 ?

**总结（面试时可以说的一句话版本）**：

“构造函数**可以**抛出异常，这是处理构造失败的常用方式，但需要注意已构造成员的清理； 析构函数**不应该**抛出异常，因为在栈展开过程中如果析构函数抛异常，会导致 std::terminate() 被调用，程序直接终止。”

**正确、完整的答案**（面试标准版）：

#### 1. **构造函数是否可以抛出异常？**

**可以抛出异常**，但**需要非常谨慎**。

**原因和注意事项**：

- 构造函数抛出异常是**合法**的，也是处理构造失败的推荐方式（例如 new 失败、资源分配失败时）。
- 重要规则 : 如果构造函数抛出异常：
  - **已经构造完成**的成员对象，其析构函数**会被自动调用**（保证已构造的部分被清理）。
  - **未完全构造**的对象，其析构函数**不会被调用**（因为对象根本没构造成功）。
- 这也是为什么推荐在构造函数中使用**初始化列表** + **智能指针**的原因：可以最大程度保证资源安全。

**示例**：

```c++
class Test {
    std::string s;
    int* p;
public:
    Test() : s("hello"), p(new int[100]) {
        // 如果这里抛异常，s 的析构函数会被调用，但 p 不会（除非用智能指针）
        throw std::runtime_error("fail");
    }
};
```

#### 2. **析构函数是否可以抛出异常？**

**强烈不推荐**，**几乎永远不应该**在析构函数中抛出异常。

**核心原因**：

- 析构函数通常在**栈展开**（stack unwinding）过程中被调用（例如异常传播时）。
- 如果析构函数再抛出异常，就会出现**两个异常同时存在**的情况，C++ 标准规定此时会直接调用 std::terminate()，导致程序**立即终止**，且不会进行任何进一步的清理。
- 这属于**未定义行为**（Undefined Behavior），非常危险。

**正确做法**：

- 析构函数中**绝对不要抛出异常**。
- 如果析构函数中可能发生错误，应该记录日志或用其他方式处理，而不能抛异常。

### 1.9.6 宏 和 内联的区别 ?

#### **最终高分回答版本**（推荐你记住）：

**宏（Macro）和内联函数（Inline Function）的区别**主要有以下几点：

1. 处理阶段不同：
   - **宏**：由**预处理器**（Preprocessor）在**预编译阶段**进行纯**字符串替换**，不进行任何类型检查。
   - **内联函数**：由**编译器**在**编译阶段**处理。如果编译器接受 inline 建议，会在函数调用点直接展开函数体。
2. 类型安全：
   - **宏**：没有类型检查，容易出现意外错误（如参数带副作用时）。
   - **内联函数**：有完整的类型检查，像普通函数一样安全。
3. 调试难度：
   - **宏**：调试困难，因为宏展开后看不到原来的宏定义，错误定位难。
   - **内联函数**：可以正常调试（虽然展开后调试信息可能复杂一些）。
4. 功能与限制：
   - **宏**：可以定义常量、函数-like 宏，甚至实现条件编译，但容易出错（经典问题如 MAX(a++, b++) 的副作用）。
   - **内联函数**：是真正的函数，支持重载、有命名空间、可以有返回值类型检查等。但是否真的内联**由编译器决定**，inline 只是一个建议。编译器可能会忽略 inline（比如函数体太复杂、递归等）。
5. 代码膨胀与性能：
   - 宏和内联函数都可能导致**代码膨胀**（代码体积变大）。
   - 内联函数的膨胀通常更可控，且编译器会进行优化。

**总结建议**：

- 现代 C++ 中，**强烈推荐使用 inline 函数或 constexpr** 来替代大部分宏。
- 宏只适合做一些简单的常量定义或平台相关的条件编译。
- 经典原则：**能用 const/constexpr/inline 的地方，就不要用宏**。

### 1.9.7 局部变量存在哪里 ?

#### 总结（面试时推荐的简洁回答）：

“普通的局部变量存储在**栈区**，由系统自动管理，生命周期与函数作用域一致。

而用 static 修饰的**静态局部变量**存储在**静态存储区**（.data 或 .bss 段），生命周期是整个程序，只初始化一次。

另外，如果局部变量内部包含动态分配的内存（如 vector 的数据），那部分数据实际存放在堆上。”


#### C++ 中局部变量的存储位置（正确答案）

**不是所有局部变量都在栈区**，要分情况：

#### 1. **普通局部变量**（最常见）

- **存储位置**：**栈区（Stack）**
- 原因：
  - 栈由系统自动管理，函数进入时自动分配，函数退出时自动释放。
  - 生命周期严格与函数作用域绑定（进入作用域时构造，离开作用域时析构）。
- **特点**：速度快、内存连续、缓存友好，但空间有限。

示例：

```c++
void func() {
    int a = 10;        // 普通局部变量 → 在栈上
    std::string s;     // 普通局部变量 → 在栈上（对象本身），内部数据可能在堆上
}
```

#### 2. **静态局部变量**（static 修饰）

- **存储位置**：**静态存储区**（.data 或 .bss 段）
- 原因：
  - static 改变了它的生命周期，使其从函数退出后依然存在（程序结束才释放）。
  - 只在第一次进入函数时初始化，之后一直存在。
- **特点**：生命周期是整个程序，初始化只进行一次，默认初始化为 0。

示例：

```c++
void func() {
    static int count = 0;   // 静态局部变量 → 在 .data/.bss 段
    count++;
}
```

#### 3. **特殊情况**

- 局部变量如果是**大对象**（如很大的数组或 std::vector），其**对象本身**仍在栈上，但内部动态分配的数据在**堆**上。
- 使用 new/malloc 分配的内存一定在**堆**上，与是否是局部变量无关。

### 1.9.8 拷贝构造函数 , 为什么传引用而不传值 ?

#### 总结（面试时可以说的一句话版本）：

“拷贝构造函数**必须传引用**，不能传值。因为如果传值，在调用拷贝构造函数时需要先把实参拷贝一份传给形参，这又会再次调用拷贝构造函数，导致无限递归调用，最终栈溢出。

实际中我们通常写成 Test(const Test& t)，既能避免拷贝，又能保证语义正确。”


#### **标准、高分回答**（推荐你记住这个版本）：

**拷贝构造函数必须使用引用传递（而不是值传递），核心原因有两点：**

1. **避免无限递归调用**（最致命的问题）

如果你把拷贝构造函数写成值传递：

```c++
class Test {
public:
    Test(const Test t);     // 错误写法：传值
};
```

当执行 Test t2(t1); 时：

- 要调用 Test(const Test t)，需要把 t1 **拷贝**一份传给参数 t
- 而拷贝 t1 又会调用拷贝构造函数 Test(const Test t)
- 于是陷入**无限递归**，最终导致栈溢出（Stack Overflow）

2. **性能和语义正确性**

即使不考虑递归：

- 值传递会在函数调用时创建**临时对象**，带来不必要的拷贝开销。
- 使用常量引用const Test& t 可以：
  - 避免任何多余的拷贝
  - 保证原始对象不会被修改（const）
  - 同时支持临时对象（右值）作为参数

**正确写法**：

```c++
class Test {
public:
    Test(const Test& t);     // 推荐：常量引用
    // 或者 Test(Test&& t) noexcept;   // 移动构造函数（C++11）
};
```

### 1.9.9 内联函数和普通函数的区别? (从反汇编角度来回答)

**内联函数的主要目的是减少函数调用开销，通过在调用点直接展开函数体来实现。但它只是一个建议，最终是否内联由编译器决定。在现代 C++ 中，我们更多使用 inline 函数来替代宏定义，以获得更好的类型安全和调试体验。**

#### **最终标准答案**（面试推荐版本）

**内联函数（inline function）和普通函数的主要区别**如下：

1. 处理时机和机制不同
   - **普通函数**：编译器将其编译成独立的函数体，函数调用时会发生**真正的函数调用**（call 指令）。
   - **内联函数**：如果编译器接受 inline 建议，会在**每个调用点**将函数体**直接展开**，替换成函数体中的代码，**没有函数调用开销**。
2. 函数调用开销
   - 普通函数调用时需要：
     - 将实参压栈
     - 保存返回地址
     - 跳转到函数入口地址
     - 执行函数体
     - 返回时恢复栈帧、弹出返回地址
   - **内联函数**：没有上述调用开销，直接执行展开后的代码，速度更快。
3. 编译器行为
   - inline 只是一个**建议**，编译器可以接受，也可以忽略（比如函数体太大、包含递归、虚拟函数等情况，编译器通常不会内联）。
   - 普通函数没有这个建议机制。
4. 其他重要区别
   - **代码大小**：内联函数可能导致**代码膨胀**（同一个函数在多个调用点被展开多次）。
   - **调试难度**：内联函数展开后调试相对麻烦（虽然现代调试器支持）。
   - **类型安全与重载**：两者都支持，但内联函数仍然是真正的函数，支持重载和类型检查。
   - **递归**：普通函数可以递归，内联函数一般不能有效递归（编译器通常不会内联递归函数）。

### 2.0.1 如何实现一个不可以被继承的类?

“在 C++ 中，实现一个不可以被继承的类（Final Class）有几种方式：

**1. 最推荐的方法（C++11 及以后）**： 使用 final 关键字直接修饰类：

```c++
class Base final { ... };
```

这样任何尝试继承 Base 的类都会在编译期报错，语义清晰且实现简单。

**2. 传统方法（C++98 时代）**： 将构造函数声明为 private，并提供静态工厂函数来创建对象。这样派生类因为无法调用基类构造函数而无法继承。

**3. 其他方法**： 还可以利用虚继承 + 友元类等方式实现，但实现复杂，可读性差，现在已经很少使用。

总体来说，**现代 C++ 中推荐直接使用 final 关键字**，既简洁又清晰。”

#### 如何实现一个**不可以被继承**的类？（Final Class）

##### 1. 你说的方法（传统方法）

```c++
class Base {
private:
    Base() {}                    // 构造函数私有化
public:
    static Base* create() {      // 提供工厂函数
        return new Base();
    }
};

class Derived : public Base { };   // 编译错误！无法调用 Base 的构造函数
```

**缺点**：

- 使用起来不方便（必须通过工厂函数创建对象）
- 派生类无法正常构造
- 代码风格比较老旧

##### 2. **现代推荐方法**（C++11 及以后）

使用 **final 关键字**（最简单、最清晰）：

```c++
class Base final {     // 加 final 即可禁止继承
public:
    Base() = default;
};

// class Derived : public Base { };   // 编译错误！不能继承 final 类
```

这是目前**最推荐**的做法，语义清晰，编译器直接报错。

##### 3. 其他经典方法（了解即可）

**方法二：利用虚继承 + 友元（老方法）**

```c++
class MakeFinal {
    friend class FinalClass;
private:
    MakeFinal() {}
};

class FinalClass : virtual public MakeFinal {
public:
    FinalClass() {}
};
```

**方法三：删除拷贝/移动构造 + 私有继承**（不常用）

### 2.0.2 什么是纯虚函数 ? 为什么要有纯虚函数 ? 虚函数表放在哪里的 ? 

**纯虚函数的实现类似于这样 : virtual void func() = 0; 一个拥有纯虚函数的类叫做抽象类 , 无法实例化对象**

**纯虚函数为派生类提供了统一的虚函数接口 , 能够让派生类重写同名覆盖方法 , 便于实现多态**

**虚函数表是存放在 .rodata 只读数据段的**

#### **最终推荐的高分回答**（面试标准版）：

**1. 什么是纯虚函数？**

纯虚函数是指在基类中声明，但**没有给出具体实现**的虚函数，其声明形式为：

```c++
virtual void func() = 0;   // =0 表示纯虚函数
```

含有纯虚函数的类称为**抽象类**（Abstract Class），抽象类**不能被实例化**（无法创建对象），只能作为基类被继承。

**2. 为什么要有纯虚函数？**

纯虚函数的主要作用是**定义接口**，为派生类提供一个**统一的函数签名**。它有以下重要意义：

- 强制派生类必须重写（override）这个函数，否则派生类也会变成抽象类，无法实例化。
- 实现**运行时多态**：通过基类指针或引用调用纯虚函数时，会动态绑定到派生类具体的实现。
- 这是很多设计模式（如模板方法模式、策略模式）的核心基础，常用于定义“必须实现的接口”。

**3. 虚函数表（vtable）存放在哪里？**

虚函数表存放在 **.rodata 段**（只读数据段 / 常量数据段）。

- 每个**含有虚函数的类**只有**一份**虚函数表。
- 每个对象在内存最前面有一个**虚指针 vptr**，指向该类对应的虚函数表。
- 因为虚函数表的内容在编译期就确定了，不会运行时修改，所以放在只读段更安全且节省内存。

------

**总结（一句话版本）**：

“纯虚函数是用 virtual void func() = 0; 声明的没有实现的虚函数，含有纯虚函数的类是抽象类。它的作用是定义接口、强制派生类实现，并支持多态。虚函数表存放在 .rodata 只读数据段，每个类一份。”

### 2.0.3 手写单例模式 ? 

[单例模式](#1.5.1 单例模式)

### 2.0.4 C++ 中 const , const 和 static 的区别 ?

1. **面向过程 : c++ const 可以修饰 全局变量 , 局部变量 , static 可以修饰 全局变量 , 局部变量 , 普通函数 , 并且修饰过后的函数只能在当前 .cpp文件中被使用 , 因为它有一个性质是 文件限制 , 防止别的文件同名函数的污染**
2. **面向对象 : c++ const 可以修饰成员变量 , 成员方法 , 修饰的成员方法变为常方法 , 只读 , this 指针也会变为 const Test *this , static 可以修饰成员变量 , 但是在类内声明 , 类外初始化 , 但是 c++ 也引入了在类内就可以初始化的方法 , 可以修饰成员方法 , 但是是不会产生 this 指针的 , 因为此时这个方法只属于类本身, 是通过类的作用域调用的**

### C++ 中 const 和 static 的区别

#### **1. const 的核心作用：只读（Read-Only）**

- 面向过程：
  - 修饰普通变量：使变量变为常量，不可修改。
  - 修饰指针：const int* p（指向的内容不可改），int* const p（指针本身不可改），const int* const p（两者都不可改）。
- 面向对象：
  - 修饰成员变量：该成员变为常量，必须在初始化列表中初始化。
  - 修饰成员函数：该函数变为**常成员函数**，不能修改非 mutable 的成员变量，this 指针类型变为 const Test* this。

#### **2. static 的核心作用：改变生命周期 / 可见性 / 所属关系**

- 面向过程：
  - 修饰全局变量 / 函数：变为**内部链接**（internal linkage），只能在当前 .cpp 文件中使用，防止命名污染。
  - 修饰局部变量：生命周期延长到整个程序，只初始化一次，存储在静态存储区（.data/.bss）。
- 面向对象：
  - 修饰成员变量：属于**类**，而非对象，所有对象共享一份，不占用对象 sizeof，大部分情况需类外初始化（C++17 后支持 inline static 在类内初始化）。
  - 修饰成员函数：属于**类**，**没有 this 指针**，不能访问普通成员变量，只能访问 static 成员。

### 2.0.5 C++ 四种强制类型转换

1. **const_cast 是用来去除const/volatile 限制的 , 一般用于指针 , 引用, 对象本身(很少用)** 

2. **static_cast : 最常用 , 是在编译期中的转换 , 进行编译器认为'安全' 的转换 , 比如 : 派生类指针转换为基类指针这类不安全的行为需要自己负责** 
3. **reinterpret_cast : 是最危险的转换 , 重新解释内存中的位模式 , 几乎不做任何检查** 
4. **dynamic_cast : 唯一一个在运行时期的检查 , 一般用于有虚函数的类的情况中 , 安全的向下转换 基类指针 -> 派生类指针 ,依赖 RTTI 信息 , 如果是指针类型 , 转换失败返回 nullptr , 如果是引用类型 , 抛出 bad_cast 异常**

#### **最终推荐的高分回答**（面试时可以直接使用）：

“C++ 提供了四种显式类型转换操作符，比 C 风格的 (type) 转换更安全和清晰：

1. **const_cast** 用于**添加或去除** const / volatile 属性。一般用于指针或引用，很少用于对象本身。 例如：把 const int* 转换为 int* 以去除只读限制。
2. static_cast最常用的转换，是编译期进行的类型转换。 适用于编译器认为合理的转换，例如：
   - 基本数据类型转换（int → double）
   - 类层次结构中的向上转换（派生类 → 基类）—— 安全
   - 向下转换（基类 → 派生类）—— 不安全，需要程序员自己保证正确性
3. reinterpret_cast最危险的转换，几乎不做任何类型检查。 它会重新解释内存中的位模式，常用于：
   - 不同指针类型之间的强制转换
   - 指针与整数之间的转换 使用时要非常小心，容易出现未定义行为。
4. dynamic_cast唯一在运行时进行类型检查的转换，只适用于含有虚函数的多态类。 主要用于安全的向下转换（基类指针/引用 → 派生类指针/引用）。
   - 如果转换成功，返回正确的指针/引用；
   - 如果转换失败：指针形式返回 nullptr，引用形式抛出 std::bad_cast 异常。 它依赖 RTTI（运行时类型信息），有一定性能开销。

**安全性排序**（从安全到危险）： dynamic_cast > static_cast > const_cast > reinterpret_cast

### 2.0.6 详细解释 deque 的底层原理 

**总结一句话（面试时可以说）**：

“std::deque 的底层是**分段连续存储结构**：它通过一个中控器（动态指针数组）管理多个固定大小的内存块。这些内存块在逻辑上是连续的，从而实现了头部和尾部的高效操作（O(1)），同时保持了较好的随机访问性能。”

#### **deque 的正确原理（面试标准版）**

std::deque（双端队列）的底层采用**分段连续存储结构**（segmented contiguous storage），目的是在**头部和尾部**都能实现高效的 O(1) 插入和删除，同时保持较好的随机访问性能。

#### 核心结构：

1. 中控器（Map / Central Array）：
   - 一个动态数组，里面存放的是**指针**。
   - 每个指针指向一块**固定大小**的连续内存块（称为 Buffer 或 Node）。
   - 中控器本身会根据需要动态扩容（通常按倍数增长）。
2. 数据块（Buffer）：
   - 每个数据块是固定大小的连续内存（常见大小为 512 字节或根据元素类型调整）。
   - 所有数据块在**逻辑上**是连续的，形成一个完整的 deque。
3. 迭代器：
   - deque 的迭代器比较复杂，需要记录：
     - 当前所在数据块的索引
     - 在当前数据块中的偏移量
   - 这使得随机访问接近 O(1)，但比 vector 稍慢。

#### 为什么这样设计？

- **双端高效操作**：在头部（push_front / pop_front）和尾部（push_back / pop_back）插入删除都是 **O(1)**。
- **随机访问**：虽然不是完全连续，但通过中控器可以快速定位，随机访问复杂度接近 O(1)。
- **内存利用率**：避免了 vector 在头部插入时需要大量移动元素的开销。

#### 与 vector 的对比（高频）：

- vector：单块连续内存，尾部操作高效，头部操作低效，扩容时所有迭代器可能失效。
- deque：分段内存，双端操作都高效，但随机访问和缓存局部性比 vector 差一些，迭代器失效规则比 vector 温和。

### 2.0.7 虚函数 , 多态

**虚函数 : 虚函数有普通虚函数 , 纯虚函数 , 普通虚函数的实现就是在函数返回类型前加 virtual 关键字 , 通常在基类中使用 , 因为这样做在派生类继承基类的时候 , 基类为派生类提供了统一的虚函数接口 , 使得派生类能够重写基类的同名覆盖方法 , 形成多态 , 纯虚函数格式类似这样: virtual void func() = 0; , 含有纯虚函数的类是抽象类 , 这样的类不能实例化对象 , 并且它同样在继承体系中 , 为派生类提供了统一的重虚函数接口 , 使得派生类能够重写基类的同名覆盖方法 , 实现动态多态**

**多态分为静态多态 , 动态多态**

**静态多态就是在编译时期进行 , 有重载(函数重载 , 运算符重载) , 模板(函数模板 , 类模板 ) , 这样的多态编译器在编译时期就通过用户的传入知道调用哪个函数**

**动态多态 : 虚函数 , 拥有虚函数的类在运行时会生成一张 vftable 虚函数表 , 里面存储的有虚函数地址 , 存储在 .rodata 段, 当基类指针指向派生类对象的时候 , 指针内存会多出 vfptr 的内存 , 编译器在运行时 , 会根据 vfptr 找到 vftable , 从而找到虚函数 , 调用它**

#### **最终推荐的高分回答**（面试标准版）：

**虚函数和多态**

**1. 虚函数**

虚函数是在基类中声明的、可以在派生类中被重写的函数。

- **普通虚函数**：在函数声明前加上 virtual 关键字。 作用是为派生类提供一个**统一的接口**，允许派生类重写（override）该函数，从而实现运行时多态。
- **纯虚函数**：声明形式为 virtual void func() = 0; 含有纯虚函数的类称为**抽象类**，抽象类**不能被实例化**。纯虚函数的主要作用是**强制派生类必须实现该接口**，同时为多态提供统一调用入口。

**2. 多态**

多态分为**静态多态**和**动态多态**：

- **静态多态**（编译期多态 / 早绑定）： 在编译时期就确定要调用哪个函数。主要通过**函数重载**、**运算符重载**和**模板**（函数模板、类模板）实现。特点是效率高，没有运行时开销。

- **动态多态**（运行期多态 / 晚绑定）： 通过**虚函数**实现。核心机制是**虚函数表（vtable）**和**虚指针（vptr）**。

  **动态多态的实现原理**：

  - 每个含有虚函数的类在编译期会生成一张**虚函数表（vtable）**，存放在 **.rodata**（只读数据段），表中按顺序存放该类虚函数的地址。
  - 每个对象在内存布局的最前面有一个**虚指针 vptr**，指向本类的虚函数表。
  - 当使用基类指针 Base* p = new Derived();调用虚函数 p->func()时：
    1. 通过 p 找到对象中的 vptr；
    2. 通过 vptr 找到对应的 vtable；
    3. 根据虚函数在表中的偏移量找到实际函数地址（此时找到的是 Derived::func()）；
    4. 执行该函数，实现运行时多态。

**总结**： 虚函数是实现动态多态的基础，纯虚函数用于定义接口和抽象类，而动态多态让基类指针可以调用不同派生类的具体实现，是面向对象编程中非常重要的特性。

### 2.0.8 虚析构函数 , 智能指针

**析构函数可以并且推荐声明为虚函数**。

原因如下： 当我们通过**基类指针**指向派生类对象时：

```c++
Base* p = new Derived();
delete p;     // 如果基类析构函数不是 virtual
```

- 如果基类析构函数**不是虚函数**，则只会调用 Base 的析构函数，Derived 的析构函数不会被调用，导致派生类中的资源无法释放，造成内存泄漏或资源泄漏。
- 如果基类析构函数**是虚函数**，则会通过虚函数机制正确调用 Derived 的析构函数，再自动调用 Base 的析构函数，实现正确的清理顺序。

**总结**：

- 构造函数**不能**是虚函数（语言层面也不允许）。
- 析构函数**应该**是虚函数，尤其是当类作为基类被继承时（基类析构函数通常要声明为 virtual）。

****

**智能指针的核心思想 —— RAII**

**RAII**（Resource Acquisition Is Initialization）：资源获取即初始化。

智能指针将资源（这里指堆内存）的**生命周期**绑定到**栈对象**的生命周期上：

- 智能指针对象本身在栈上
- 当栈对象生命周期结束（作用域结束、函数返回、异常抛出等），其析构函数会**自动**释放堆上的内存

这样就从根本上避免了“忘记 delete”的问题。

**3. C++11 之后标准库中的智能指针（重点掌握）**

主要有三种：

**① unique_ptr**（独占所有权智能指针）—— **最推荐使用**

- 同一时刻只有一个 unique_ptr 指向某块内存
- **不支持普通拷贝构造和赋值**（左值拷贝被 delete）
- 支持**移动语义**（移动构造和移动赋值）
- 不带引用计数，性能高、开销小
- 适用场景：独占资源管理、作为函数返回值、作为类成员等

**② shared_ptr**（共享所有权智能指针）

- 允许多个 shared_ptr 指向同一块内存
- 内部维护**引用计数**（control block）
- 只有当引用计数降为 0 时，才真正释放内存
- 开销比 unique_ptr 大（需要维护引用计数 + 控制块）
- 适用场景：真正需要共享所有权的场景

**③ weak_ptr**（弱引用智能指针）

- 不影响引用计数
- 主要用来**解决 shared_ptr 的循环引用问题**
- 不能直接访问对象，需要通过 .lock() 转为 shared_ptr 后再使用

**4. 循环引用问题 + weak_ptr 的解决方式（高频考点）**

```c++
struct A {
    shared_ptr<B> pb;
};
struct B {
    shared_ptr<A> pa;
};

auto a = make_shared<A>();
auto b = make_shared<B>();
a->pb = b;
b->pa = a;   // 形成循环引用
// a 和 b 离开作用域后，引用计数永远是 1 和 1，无法释放！
```

**解决方法**：把其中一个成员改成 weak_ptr：

```c++
struct A {
    shared_ptr<B> pb;
};
struct B {
    weak_ptr<A> pa;   // 改成 weak_ptr
};
```

因为 weak_ptr 不增加引用计数，当强引用（shared_ptr）都销毁后，内存就能正常释放。

### 2.0.9 一个类 , 写了一个构造函数 , 还写了一个虚构造函数 , 可不可以 ? 会发生什么  ?

“构造函数**不能**是虚函数。

主要原因有两点：

1. 对象在构造函数执行期间还没有完全构造好，虚指针（vptr）和虚函数表（vtable）尚未被正确初始化，无法实现动态绑定。
2. 构造函数的调用是静态确定的，对象正在构造的过程中，其类型信息还不完整。

如果构造函数是虚函数，不仅无法达到多态的目的，还会导致构造过程出错。因此 C++ 语法明确禁止构造函数为虚函数。”

**不可以。构造函数不能是虚函数。**

#### 为什么构造函数不能是虚函数？（核心原因）

1. **对象还没有构造完成，虚函数机制无法工作** 虚函数依赖于**虚函数表（vtable）**和**虚指针（vptr）**来实现动态绑定。 而 vptr 是在构造函数的执行过程中才被初始化的（通常在构造函数的初始化列表阶段或函数体开始时）。 在构造函数执行的时候，对象的 vptr 还没有指向最终的虚函数表，因此无法进行正确的动态绑定。
2. **类型信息不完整** 当构造函数被调用时，对象的**静态类型**和**动态类型**还不一致。 对象正在被构造，它的类型是“正在构造的当前类”，而不是最终的派生类类型。 即使把构造函数声明为虚函数，编译器也无法按照多态的方式去调用派生类的构造函数。
3. **语言层面的限制** C++ 语法明确规定**构造函数不能是虚函数**。如果尝试声明，编译器会直接报错。

#### 补充说明（析构函数对比）：

- **析构函数可以并且推荐声明为虚函数**（尤其是基类）。
- 原因：当通过基类指针 delete 一个派生类对象时，如果基类析构函数不是虚函数，只会调用基类的析构函数，导致派生类资源无法正确释放。

### 2.1.0 异常是怎么回事 ?

#### 正确、完整的讲解（面试标准版）

**C++ 中的异常处理机制**：

C++ 使用 **try-catch** 机制来处理运行时异常。

#### 基本语法：

```c++
try {
    // 可能抛出异常的代码
    throw std::runtime_error("error message");   // 主动抛出异常
} 
catch (const std::exception& e) {      // 按引用捕获（推荐）
    // 处理异常
    std::cout << e.what() << std::endl;
} 
catch (...) {                          // 捕获所有未知异常
    // 处理未知异常
}
```

#### 异常传播机制（Stack Unwinding）：

- 如果 try 块中的代码抛出异常，会立即停止当前代码的执行。
- 程序会沿着**调用栈**向上查找匹配的 catch 块（栈展开过程）。
- 在栈展开过程中，已经构造的局部对象会自动调用析构函数（这就是 RAII 的重要性）。
- 如果一直找到 main() 函数都没有找到匹配的 catch，程序会调用 std::terminate()，直接终止运行（不会继续执行任何代码）。

#### 重要概念：

- **throw**：用于抛出异常。
- **noexcept**：声明函数不会抛出异常（C++11），如果标记为 noexcept 的函数抛出异常，程序会直接 terminate。
- **异常安全**：异常发生时，程序资源不会泄漏、状态不会破坏（强烈推荐使用 RAII，如智能指针）。

### 2.1.1 早绑定 和 晚绑定 ? 

#### **标准、高分回答**（面试推荐版）：

**早绑定（Early Binding）和晚绑定（Late Binding）**是 C++ 中多态的两种实现方式：

#### 1. **早绑定（静态绑定 / Static Binding）**

- **绑定时机**：**编译阶段**（编译器完成绑定）。

- **实现方式**：函数重载（overload）、运算符重载、模板（函数模板、类模板）。

- 特点：

  - 编译器根据函数名 + 参数类型（或模板参数）在编译时就确定要调用哪个具体函数。
  - 没有运行时开销，执行效率高。

- 例子：

  ```c++
  void print(int x);
  void print(double x);   // 重载 → 早绑定
  
  template<typename T>
  void print(T x);        // 模板 → 早绑定
  ```

#### 2. **晚绑定（动态绑定 / Dynamic Binding）**

- **绑定时机**：**运行阶段**（程序运行时才确定）。

- **实现方式**：**虚函数**（virtual function） + 继承 + 基类指针/引用。

- 特点：

  - 编译器无法在编译时确定最终调用哪个函数，必须在运行时通过虚函数机制（vptr + vtable）来查找。
  - 有一定的运行时开销，但提供了强大的多态能力。

- 例子

  ```c++
  class Base {
  public:
      virtual void func() { std::cout << "Base"; }
  };
  
  class Derived : public Base {
  public:
      void func() override { std::cout << "Derived"; }
  };
  
  Base* p = new Derived();
  p->func();   // 运行时才确定调用 Derived::func() → 晚绑定
  ```

**总结对比**：

- **早绑定**：编译期完成，速度快，灵活性较低（静态多态）。
- **晚绑定**：运行期完成，速度稍慢，但灵活性高，支持真正的面向对象多态（动态多态）。

在实际开发中，能用静态多态（模板、重载）解决的问题，优先使用静态多态；需要运行时多态时，才使用虚函数实现晚绑定。

### 2.1.2 指针和引用的区别 ? 

#### **标准、高分回答**（面试推荐版）

**指针（Pointer）和引用（Reference）的区别**如下：

##### 1. **本质区别**

- **指针**是一个**变量**，存放的是另一个变量的**内存地址**。
- **引用**是一个**别名**（alias），它是某个已有变量的另一个名字，本质上是对变量的**封装**。

##### 2. **关键区别对比**

| 维度             | 指针 (Pointer)                    | 引用 (Reference)                       |
| ---------------- | --------------------------------- | -------------------------------------- |
| 是否必须初始化   | 不必须（可以是 nullptr）          | **必须初始化**，且初始化后不能重新绑定 |
| 是否可以重新指向 | 可以（p = &another）              | **不可以**，一旦绑定就固定             |
| 是否存在多级     | 支持（int** pp）                  | **不支持**多级引用                     |
| 是否可以为空     | 可以（nullptr）                   | **不可以**（空引用是未定义行为）       |
| sizeof           | 返回指针本身的大小（通常 8 字节） | 返回被引用对象的大小                   |
| 使用方式         | 需要解引用 *p                     | 直接使用，像普通变量一样               |
| 底层实现         | 通常是地址                        | 通常也是地址（编译器实现），但语义不同 |

##### 3. **右值引用（&&）的作用**

- 左值引用（T&）：绑定左值
- 右值引用（T&&）：绑定右值，主要用于**移动语义**（移动构造函数、移动赋值）和**完美转发**（std::forward）

##### 4. **实际使用建议**

- 需要“空”状态或需要重新指向时 → 用**指针**
- 需要更安全、不想处理空指针、语义上是“别名”时 → 用**引用**
- 现代 C++ 中，**函数参数尽量使用引用**（const 左值引用或右值引用），减少拷贝开销

### 2.1.3 智能指针交叉引用问题怎么解决 ?

**总结回答（面试时推荐的版本）**：

“交叉引用问题是指两个或多个对象通过 shared_ptr 互相持有对方，导致强引用计数永远无法降到 0，从而产生内存泄漏。

**解决方法**是将循环中的一个方向改为 std::weak_ptr。 weak_ptr 不增加强引用计数，只起到观察作用。当强引用计数归零时，对象正常析构，weak_ptr 的 .lock() 会返回空指针，从而打破循环。”

### 2.1.4 重载的底层实现 , 虚函数的底层实现 ?

#### 正确答案（面试标准版）

##### 1. **重载（Overloading）的底层实现**

重载分为**函数重载**和**运算符重载**，底层核心机制是 **名字修饰（Name Mangling）**。

- 函数重载：
  - C++ 编译器会对函数名进行**修饰**，在函数名后面加上参数类型信息。
  - 例如：
    - void func(int) 可能被修饰成 _Z4funci
    - void func(double) 可能被修饰成 _Z4funcd
  - 链接时根据修饰后的名字找到对应函数实现。
  - 这就是**早绑定 / 静态绑定**。
- 运算符重载：
  - 本质上是函数重载（成员函数或全局函数）。
  - 例如 operator+ 会被编译器转换成对应的函数名。

##### 2. **虚函数（Virtual Function）的底层实现**

虚函数是**动态多态**的基础，底层依赖 **虚函数表（vtable）** 和 **虚指针（vptr）**。

- 虚函数表（vtable）：
  - 每个**含有虚函数的类**在编译期生成**一张虚函数表**。
  - vtable 是一个指针数组，存放该类所有虚函数的地址。
  - 存放在 **.rodata**（只读数据段）。
- 虚指针（vptr）：
  - 每个含有虚函数的**对象**在内存布局的**最前面**有一个虚指针 vptr。
  - vptr 指向本类的虚函数表。
- 调用过程（动态绑定）：
  1. 通过基类指针找到对象。
  2. 取出对象开头的 vptr。
  3. 通过 vptr 找到对应的 vtable。
  4. 根据虚函数在 vtable 中的**偏移量**找到函数地址。
  5. 调用该地址的函数（此时调用的是派生类重写的版本）。

纯虚函数（= 0）的类是抽象类，不能实例化，但同样会生成 vtable（通常 vtable 中对应位置放空指针或特殊值）。

### 2.1.5 讲一下 map 的底层实现 , avl 和 rbtree 有什么区别 ?

#### map 的底层原理 + AVL 树 vs 红黑树

**1. std::map 的底层实现**

std::map（以及 multimap、set、multiset）的底层实现是**红黑树**（Red-Black Tree），一种自平衡二叉搜索树。

- 所有操作（插入、删除、查找）的时间复杂度都是 **O(log n)**。
- 红黑树能**天然保持 key 的有序性**，支持 lower_bound、upper_bound、equal_range 等高效区间操作。

**2. AVL 树 vs 红黑树（核心对比）**

两者都是自平衡二叉搜索树，但平衡策略不同：

| 维度          | AVL 树                              | 红黑树（Red-Black Tree）            |
| ------------- | ----------------------------------- | ----------------------------------- |
| 平衡要求      | **严格平衡**：左右子树高度差 ≤ 1    | **大致平衡**：通过颜色约束维持平衡  |
| 旋转次数      | **较多**（插入/删除时需要频繁旋转） | **较少**（最多旋转 2~3 次）         |
| 插入/删除性能 | 较慢                                | **更快**（适合频繁插入删除）        |
| 查询性能      | 略快（树更平衡）                    | 接近 AVL，但整体综合性能更好        |
| 实现复杂度    | 较高                                | 较低                                |
| 实际使用      | 较少                                | **广泛使用**（std::map 就是红黑树） |

**红黑树的核心性质（五个性质）**：

1. 每个节点是红色或黑色。
2. 根节点是黑色。
3. 所有叶子节点（NIL）都是黑色。
4. 红色节点的子节点必须是黑色（不能有连续红色节点）。
5. 从任一节点到其每个叶子的所有路径都包含相同数量的黑色节点（黑色高度相同）。

这些性质保证了红黑树的最长路径不超过最短路径的 2 倍，从而维持了“大致平衡”。

**为什么 map 选择红黑树而不是 AVL 树？**

- AVL 树为了严格平衡，需要进行大量旋转，**插入和删除效率较低**。
- 红黑树通过颜色约束，用较少的旋转就能维持足够好的平衡，**插入和删除性能更好**。
- 在实际应用中，**查询、插入、删除操作混合出现**，红黑树的综合性能优于 AVL 树。

### 2.1.6 常用哪些 STL 容器 ?

**vector , list , deque , queue , priority_queue , stack , map , set , unordered_map , unordered_set , multimap , multiset **

### 2.1.7 假如 map 的键是类类型 , 那么 map 的底层是如何调整的 ?

“当 map 的 key 是自定义类类型时，底层红黑树需要知道如何比较两个 key 的大小。

默认情况下，map 使用 std::less<Key>，要求类必须重载 operator<。

更推荐的方式是提供**自定义比较器**（可以是函数对象或 lambda），在构造 map 时传入

```c++
std::map<MyClass, int, MyComparator> m;
```

红黑树在所有操作中都会调用这个比较器来决定节点的插入位置和查找路径，从而维护 key 的有序性。”

****

**核心答案**：

当 map 的 key 是**自定义类类型**时，std::map **必须知道如何比较两个 key 的大小**，因为红黑树需要根据 key 的大小来维护有序性。

#### 1. 默认情况（最常见）

如果你直接写：

```c++
std::map<MyClass, int> m;
```

编译器会要求 MyClass 必须支持**小于运算符** operator<：

```c++
struct MyClass {
    int id;
    std::string name;

    bool operator<(const MyClass& other) const {
        return id < other.id;        // 或者按照你需要的比较逻辑
    }
};
```

- map 默认使用 std::less<Key> 作为比较器。
- std::less<Key> 内部就是调用 operator<。
- 所以**必须为你的类重载 operator<**，否则编译不通过。

#### 2. 使用自定义比较器（推荐方式，尤其当你不想修改类时）

```c++
struct MyClass {
    int id;
    std::string name;
};

// 自定义比较器（可以是函数对象 / lambda / 函数指针）
struct MyComparator {
    bool operator()(const MyClass& a, const MyClass& b) const {
        return a.id < b.id;        // 或者更复杂的比较逻辑
    }
};

std::map<MyClass, int, MyComparator> m;
```

或者使用 lambda（C++11 及以后）：

```c++
auto cmp = [](const MyClass& a, const MyClass& b) {
    return a.id < b.id;
};

std::map<MyClass, int, decltype(cmp)> m(cmp);
```

#### 3. 底层红黑树如何处理？

- 红黑树在插入、查找、删除时，会**不断调用比较器**来决定往左子树还是右子树走。

- 比较器必须满足

  严格弱序（Strict Weak Ordering）：

  - 自反性：comp(a,a) 必须返回 false
  - 反对称性：如果 comp(a,b) 为 true，则 comp(b,a) 必须为 false
  - 传递性：如果 comp(a,b) 和 comp(b,c) 为 true，则 comp(a,c) 也为 true

如果比较器不满足严格弱序，会导致红黑树行为未定义（可能崩溃或逻辑错误）。

### 2.1.8 内存泄漏怎么处理 ?

**1. 智能指针的核心思想 —— RAII**

**RAII**（Resource Acquisition Is Initialization）：资源获取即初始化。

智能指针将资源（这里指堆内存）的**生命周期**绑定到**栈对象**的生命周期上：

- 智能指针对象本身在栈上
- 当栈对象生命周期结束（作用域结束、函数返回、异常抛出等），其析构函数会**自动**释放堆上的内存

这样就从根本上避免了“忘记 delete”的问题。

**2. C++11 之后标准库中的智能指针（重点掌握）**

主要有三种：

**① unique_ptr**（独占所有权智能指针）—— **最推荐使用**

- 同一时刻只有一个 unique_ptr 指向某块内存
- **不支持普通拷贝构造和赋值**（左值拷贝被 delete）
- 支持**移动语义**（移动构造和移动赋值）
- 不带引用计数，性能高、开销小
- 适用场景：独占资源管理、作为函数返回值、作为类成员等

**② shared_ptr**（共享所有权智能指针）

- 允许多个 shared_ptr 指向同一块内存
- 内部维护**引用计数**（control block）
- 只有当引用计数降为 0 时，才真正释放内存
- 开销比 unique_ptr 大（需要维护引用计数 + 控制块）
- 适用场景：真正需要共享所有权的场景

**③ weak_ptr**（弱引用智能指针）

- 不影响引用计数
- 主要用来**解决 shared_ptr 的循环引用问题**
- 不能直接访问对象，需要通过 .lock() 转为 shared_ptr 后再使用

**4. 循环引用问题 + weak_ptr 的解决方式（高频考点）**

```c++
struct A {
    shared_ptr<B> pb;
};
struct B {
    shared_ptr<A> pa;
};

auto a = make_shared<A>();
auto b = make_shared<B>();
a->pb = b;
b->pa = a;   // 形成循环引用
// a 和 b 离开作用域后，引用计数永远是 1 和 1，无法释放！
```

**解决方法**：把其中一个成员改成 weak_ptr：

```c++
struct A {
    shared_ptr<B> pb;
};
struct B {
    weak_ptr<A> pa;   // 改成 weak_ptr
};
```

因为 weak_ptr 不增加引用计数，当强引用（shared_ptr）都销毁后，内存就能正常释放。

### 2.1.9 如果让你实现一个内存池 , 要求获取资源和插入资源的时间花费为 O(1) , 怎么设计 ?

“如果要实现一个 O(1) 获取和归还资源的内存池，我会采用 **SGI STL 二级空间配置器的设计思想**：

- 使用 **16 个自由链表**，分别管理 8、16、24、...、128 字节的内存块。
- **分配时**：根据请求大小找到对应链表，直接从链表头部取一块（O(1)）。
- **释放时**：把内存块挂回对应链表头部（O(1)）。
- 当链表为空时，从**内存池**中一次性申请一大块内存，切分成多个小块挂到链表上（amortized O(1)）。

大块内存（> 128 字节）直接走一级配置器（malloc/free）。

这种设计既能做到 O(1) 的分配和释放，又能有效减少系统调用次数，提高内存利用率。”



#### 推荐设计方案（基于 SGI STL 二级空间配置器）

##### 1. 整体架构

- **一级空间配置器**：直接调用 malloc / free，用于处理**大块内存**（通常 > 128 bytes）。
- **二级空间配置器**（内存池核心）：专门管理**小块内存**（≤ 128 bytes），实现 O(1) 分配和释放。

##### 2. 二级空间配置器的核心数据结构

```c++
// 自由链表节点
union FreeListNode {
    FreeListNode* next;   // 指向下一个空闲块
    char data[1];         // 实际数据开始位置（柔性数组）
};

// 16 个自由链表（管理 8,16,24,...,128 字节的内存块）
FreeListNode* free_list[16];   // index 0 对应 8 字节块，index 1 对应 16 字节块...
```

##### 3. 内存池结构

- 维护一个**大内存池**（chunk），初始为空。
- 当某个自由链表为空时，从内存池中切出一大块内存，拆分成多个小块，挂到对应链表上。

##### 4. 关键实现（O(1) 分配和释放）

**分配（allocate）**：

```c++
void* allocate(size_t bytes) {
    if (bytes > 128) {
        return malloc(bytes);        // 大块直接 malloc
    }
    
    size_t index = (bytes + 7) / 8 - 1;   // 计算对应链表下标（8字节对齐）
    
    if (free_list[index] != nullptr) {
        // O(1) 从链表头部取一块
        FreeListNode* node = free_list[index];
        free_list[index] = node->next;
        return node;
    } else {
        // 链表为空，从内存池补充一大块
        refill(index);
        return allocate(bytes);   // 递归再取一次
    }
}
```

**释放（deallocate）**：

```c++
void deallocate(void* ptr, size_t bytes) {
    if (bytes > 128) {
        free(ptr);
        return;
    }
    
    size_t index = (bytes + 7) / 8 - 1;
    
    // O(1) 归还到链表头部
    FreeListNode* node = static_cast<FreeListNode*>(ptr);
    node->next = free_list[index];
    free_list[index] = node;
}
```

##### 5. refill 函数（内存池补充）

当链表为空时，从大内存池中切分一块内存，拆成多个小块挂到链表上。

------

#### 为什么能做到 O(1)？

- **分配**：直接从对应大小的自由链表头部取一块（链表操作 O(1)）。
- **释放**：直接把内存块挂回到对应链表头部（链表操作 O(1)）。
- 只有当链表为空时，才会从内存池补充（这个操作 amortized 是 O(1)）。

### 2.2.0 编写一个 C/C++ 程序个人认为要注意什么 ? 

**“编写 C/C++ 程序时，我最注重以下几点：首先是内存安全，优先使用智能指针和 RAII 思想，避免裸指针和内存泄漏；其次是性能，通过移动语义、合适容器选择、减少临时对象来优化；同时注重代码的可维护性和架构清晰度，并积极使用现代 C++ 特性来提升代码质量。”**

#### 编写 C/C++ 程序时需要注意的核心点（面试高分版）

编写高质量的 C/C++ 程序，需要从以下几个维度系统考虑：

##### 1. **内存管理（最重要、最容易出错）**

- **强烈推荐使用 RAII 思想**：资源的获取即初始化，资源的释放与对象生命周期绑定。
- 优先使用智能指针：
  - std::unique_ptr：独占所有权，性能最好，推荐作为默认选择。
  - std::shared_ptr：需要共享所有权时使用，优先使用 std::make_shared（一次性分配对象内存 + 控制块，减少一次内存分配，提高缓存友好性）。
  - std::weak_ptr：专门用来解决 shared_ptr 的循环引用问题。
- **避免裸指针**（raw pointer）：除非在极低层或性能极致场景，否则不要手动 new/delete。
- 注意**对象生命周期**：不要返回局部对象的指针或引用。

##### 2. **性能优化**

- **避免不必要的拷贝**：使用移动语义（std::move）、右值引用、完美转发。
- **减少临时对象**：尤其是大对象在函数参数传递、返回时。
- **选择合适的容器**：数据量不确定用 vector，双端操作频繁用 deque，频繁中间插入删除用 list，需要有序用 map/set，追求速度用 unordered_*。
- **内存池**：对于频繁小对象分配的场景，考虑实现或使用内存池，减少 malloc 调用。
- **编译期计算**：尽量使用 constexpr 把计算提前到编译期。

##### 3. **代码规范与可维护性**

- **清晰的架构设计**：合理分层（业务逻辑层、数据层、工具层等），遵循单一职责原则。
- **代码风格一致**：命名规范（驼峰或下划线）、缩进、注释清晰。
- **模块化**：头文件与实现文件分离，使用命名空间避免污染。
- **错误处理**：合理使用异常或错误码，返回值检查要完备。
- **可读性优先**：写代码时多考虑“半年后的自己是否能看懂”。

##### 4. **安全性**

- **避免缓冲区溢出**：使用 std::string、std::vector 代替裸数组和 strcpy。
- **类型安全**：尽量使用 C++ 的类型转换操作符（static_cast、dynamic_cast 等），少用 C 风格强制转换。
- **边界检查**：使用 at() 而不是 operator[] 在调试阶段。
- **线程安全**：多线程环境下注意共享数据的同步。

##### 5. **利用现代 C++ 特性**

- C++11/14/17/20/23 的新特性（如 auto、lambda、constexpr、std::move、std::unique_ptr、std::variant 等）能显著提升代码质量和性能。
- 但不要盲目使用，要理解其背后的原理和开销。

### 2.2.1 讲一下红黑树和它的特性

**红黑树的核心性质（五个性质）**：

1. 每个节点是红色或黑色。
2. 根节点是黑色。
3. 所有叶子节点（NIL）都是黑色。
4. 红色节点的子节点必须是黑色（不能有连续红色节点）。
5. 从任一节点到其每个叶子的所有路径都包含相同数量的黑色节点（黑色高度相同）。

### 2.2.2 设计模式知道哪些 ? 具体讲一下

[设计模式](#1.5 设计模式)

#### 编写 C/C++ 程序时需要注意的核心点（面试高分版）

编写高质量的 C/C++ 程序，需要从以下几个维度系统考虑：

##### 1. **内存管理（最重要、最容易出错）**

- **强烈推荐使用 RAII 思想**：资源的获取即初始化，资源的释放与对象生命周期绑定。
- 优先使用智能指针：
  - std::unique_ptr：独占所有权，性能最好，推荐作为默认选择。
  - std::shared_ptr：需要共享所有权时使用，优先使用 std::make_shared（一次性分配对象内存 + 控制块，减少一次内存分配，提高缓存友好性）。
  - std::weak_ptr：专门用来解决 shared_ptr 的循环引用问题。
- **避免裸指针**（raw pointer）：除非在极低层或性能极致场景，否则不要手动 new/delete。
- 注意**对象生命周期**：不要返回局部对象的指针或引用。

##### 2. **性能优化**

- **避免不必要的拷贝**：使用移动语义（std::move）、右值引用、完美转发。
- **减少临时对象**：尤其是大对象在函数参数传递、返回时。
- **选择合适的容器**：数据量不确定用 vector，双端操作频繁用 deque，频繁中间插入删除用 list，需要有序用 map/set，追求速度用 unordered_*。
- **内存池**：对于频繁小对象分配的场景，考虑实现或使用内存池，减少 malloc 调用。
- **编译期计算**：尽量使用 constexpr 把计算提前到编译期。

##### 3. **代码规范与可维护性**

- **清晰的架构设计**：合理分层（业务逻辑层、数据层、工具层等），遵循单一职责原则。
- **代码风格一致**：命名规范（驼峰或下划线）、缩进、注释清晰。
- **模块化**：头文件与实现文件分离，使用命名空间避免污染。
- **错误处理**：合理使用异常或错误码，返回值检查要完备。
- **可读性优先**：写代码时多考虑“半年后的自己是否能看懂”。

##### 4. **安全性**

- **避免缓冲区溢出**：使用 std::string、std::vector 代替裸数组和 strcpy。
- **类型安全**：尽量使用 C++ 的类型转换操作符（static_cast、dynamic_cast 等），少用 C 风格强制转换。
- **边界检查**：使用 at() 而不是 operator[] 在调试阶段。
- **线程安全**：多线程环境下注意共享数据的同步。

##### 5. **利用现代 C++ 特性**

- C++11/14/17/20/23 的新特性（如 auto、lambda、constexpr、std::move、std::unique_ptr、std::variant 等）能显著提升代码质量和性能。
- 但不要盲目使用，要理解其背后的原理和开销。

### 2.2.3 C++ 设计模式 - 工厂模式

[工厂模式](#1.5.2 工厂模式)

### 2.2.4 解释动多态

##### **虚函数（Virtual Function）的底层实现**

虚函数是**动态多态**的基础，底层依赖 **虚函数表（vtable）** 和 **虚指针（vptr）**。

- 虚函数表（vtable）：
  - 每个**含有虚函数的类**在编译期生成**一张虚函数表**。
  - vtable 是一个指针数组，存放该类所有虚函数的地址。
  - 存放在 **.rodata**（只读数据段）。
- 虚指针（vptr）：
  - 每个含有虚函数的**对象**在内存布局的**最前面**有一个虚指针 vptr。
  - vptr 指向本类的虚函数表。
- 调用过程（动态绑定）：
  1. 通过基类指针找到对象。
  2. 取出对象开头的 vptr。
  3. 通过 vptr 找到对应的 vtable。
  4. 根据虚函数在 vtable 中的**偏移量**找到函数地址。
  5. 调用该地址的函数（此时调用的是派生类重写的版本）。

纯虚函数（= 0）的类是抽象类，不能实例化，但同样会生成 vtable（通常 vtable 中对应位置放空指针或特殊值）。

### 2.2.5 为什么析构函数要实现成虚析构函数

**析构函数可以并且推荐声明为虚函数**。

原因如下： 当我们通过**基类指针**指向派生类对象时：

```c++
Base* p = new Derived();
delete p;     // 如果基类析构函数不是 virtual
```

- 如果基类析构函数**不是虚函数**，则只会调用 Base 的析构函数，Derived 的析构函数不会被调用，导致派生类中的资源无法释放，造成内存泄漏或资源泄漏。
- 如果基类析构函数**是虚函数**，则会通过虚函数机制正确调用 Derived 的析构函数，再自动调用 Base 的析构函数，实现正确的清理顺序。

**总结**：

- 构造函数**不能**是虚函数（语言层面也不允许）。
- 析构函数**应该**是虚函数，尤其是当类作为基类被继承时（基类析构函数通常要声明为 virtual）。

### 2.2.6 如果构造函数里面抛出异常会发生什么 ? 内存泄漏 ? 怎么解决 ?

**总结（面试时推荐的简洁回答）**：

“构造函数**可以**抛出异常，这是处理构造失败的常用手段。 当构造函数抛出异常时：

- 当前对象的析构函数**不会被调用**（对象构造未完成）。
- 已经成功构造的成员子对象和基类子对象的析构函数**会被自动调用**。

因此，在构造函数中分配资源时，**强烈推荐使用 RAII**（如 std::unique_ptr、std::make_unique），以保证即使抛出异常，资源也能被正确释放。”



#### **正确、完整的答案**

**构造函数中可以抛出异常**，这也是处理构造失败的**推荐方式**（比如资源分配失败时）。

##### 构造函数抛出异常时会发生什么？

1. 对象构造失败：
   - 当前对象的**析构函数不会被调用**（因为对象没有构造成功）。
   - 这意味着如果你在构造函数中用 new 分配了资源，而没有用 RAII 管理，资源就会泄漏。
2. 已构造的成员子对象会被清理：
   - C++ 会自动调用**已经成功构造的成员对象**的析构函数（包括基类子对象）。
   - 这就是为什么推荐在构造函数中使用**初始化列表** + **智能指针**的原因。

**示例**：

```c++
class Test {
    std::string s;           // 会自动清理
    int* p;                  // 裸指针，如果 new 失败前抛异常，会泄漏
public:
    Test() : s("hello"), p(new int[1000]) {
        throw std::runtime_error("fail");   // p 指向的内存会泄漏！
    }
};
```

**正确做法**（使用 RAII）：

```c++
class Test {
    std::string s;
    std::unique_ptr<int[]> p;   // 推荐
public:
    Test() : s("hello"), p(std::make_unique<int[]>(1000)) {
        // 如果这里抛异常，unique_ptr 会自动释放内存
        throw std::runtime_error("fail");
    }
};
```

##### 与析构函数对比（高频对比）：

- **构造函数**：**可以**抛异常（推荐用于构造失败处理）。
- **析构函数**：**强烈不推荐**抛异常。如果析构函数抛异常，且当前正处于栈展开过程中，会导致 std::terminate() 被调用，程序直接终止。

### 2.2.7 为什么推荐用 make_shared 代替普通的 shared_ptr 

**先来一张图 **

[推荐用make_shared代替shared_ptr](d:/截图/建议用make_shared代替shared_ptr.png)

#### 代码 

```c++
#include "00. head file.hpp"

/*
建议用make_shared 代替 shared_ptr 
// 优点 : 内存分配效率高了 , 防止资源泄漏的风险 
// 缺点 : make_shared 无法自定义删除器  , 导致托管的资源延迟释放即就是现在没有 shared_ptr 指向资源了 , 资源也无法释放

*/
class Test {
    public:
        void func(int a) {
            std::cout << "a = " << a << std::endl;
        }
        void func(int a , int b) {
            std::cout << "a = " << a << " b = " << b << std::endl;
        }

};
int main() {
    std::shared_ptr<int> sp1(new int(10));
    std::shared_ptr<Test> sp2(new Test());
    sp2->func(10);
    sp2->func(10 , 20);


    std::shared_ptr<int> sp3 = std::make_shared<int>(10);
    *sp3 = 20;
    std::cout << "*sp3 = " << *sp3 << std::endl;
    std::shared_ptr<Test> sp4(std::make_shared<Test>());
    sp4->func(15);
    sp4->func(15 , 25);

    
    return 0;
}
```

#### 1. 代码核心内容总结

你的代码演示了两种创建 std::shared_ptr 的方式：

- 方式1：手动 new + shared_ptr 构造（传统写法）

  ```c++
  std::shared_ptr<int> sp1(new int(10));
  std::shared_ptr<Test> sp2(new Test());
  ```

- 方式2：使用 std::make_shared

  ```c++
  std::shared_ptr<int> sp3 = std::make_shared<int>(10);
  std::shared_ptr<Test> sp4 = std::make_shared<Test>();   // 更推荐写法
  ```

两者最终效果类似（都是智能指针管理资源），但底层实现和安全性有很大区别。

#### 2. make_shared vs new + shared_ptr 的对比

| std::make_shared                              | 对比项               | new + shared_ptr                                | 推荐程度       |
| --------------------------------------------- | -------------------- | ----------------------------------------------- | -------------- |
| **1次**（对象和控制块一起分配）               | **内存分配次数**     | 2次（一次new对象，一次分配控制块）              | make_shared 胜 |
| **更高**（一次分配，缓存友好）                | **内存分配效率**     | 较低                                            | make_shared 胜 |
| **更安全**（原子性分配）                      | **异常安全性**       | 有风险（new成功后，shared_ptr构造前可能抛异常） | make_shared 胜 |
| **不可以**                                    | **能否自定义删除器** | 可以                                            | new 胜         |
| **连续存储**（对象紧跟控制块之后）            | **控制块与对象布局** | 分开存储                                        | make_shared 胜 |
| 引用计数为0时立即释放（但控制块可能稍晚销毁） | **资源释放时机**     | 引用计数为0时立即释放                           | 基本相同       |

**结论**： **日常开发中强烈推荐使用 std::make_shared**，除非你有特殊需求（需要自定义删除器、或者对象非常大需要单独控制分配）。

#### 3. make_shared 的优点（你注释中提到的）

- **内存分配效率更高**：只进行一次内存分配，而不是两次。
- **异常更安全**：避免了 “new 成功，但 shared_ptr 构造函数抛异常” 导致的内存泄漏。
- **代码更简洁**：不需要显式写 new。

#### 4. make_shared 的缺点（你注释中提到的）

- 无法自定义删除器（Deleter）：

  ```
  // 这种写法 make_shared 不支持
  std::shared_ptr<Test> sp(new Test(), custom_deleter);
  ```

- 控制块与对象一起分配：

  - 当最后一个 shared_ptr 销毁后，**对象立即析构**。
  - 但**控制块**（引用计数等信息）可能会因为 weak_ptr 的存在而延迟释放。
  - 这会导致**内存占用延迟释放**（不是资源泄漏，只是释放稍晚）。

#### 5. 最佳实践建议

```c++
// 推荐写法（C++11 起）
auto sp1 = std::make_shared<int>(10);
auto sp2 = std::make_shared<Test>();

// 如果需要自定义删除器，必须使用 new
std::shared_ptr<Test> sp3(new Test(), [](Test* p) {
    std::cout << "自定义删除器\n";
    delete p;
});
```

#### 6. 额外小建议（代码改进点）

1. std::shared_ptr<Test> sp4(std::make_shared<Test>());可以简化为：

   ```
   auto sp4 = std::make_shared<Test>();
   ```

2. Test

    类最好加上虚析构函数（如果以后可能被继承）：

   ```c++
   class Test {
   public:
       virtual ~Test() = default;
       // ...
   };
   ```

------

**一句话总结**：

> **优先使用 std::make_shared**，它更高效、更安全、代码更简洁。只有在需要自定义删除器时，才使用 new + shared_ptr 的方式。

## 1.8 C++11 STL 容器中 emplace 剖析 

### 1.8.1 前提引入

```c++
#include "00. head file.hpp"

/*
容器的 emplace 方法 

emplace_back 等等方法 : 有一个重要的性质 : 当有一个自定义的类类型 , 可以直接传入右值 , 它会在底层自动调用类的构造函数 

*/

class Test {
    public:

        Test(int a) { std::cout << "Test(int)" << std::endl; }
        Test(int a , int b) { std::cout << "Test(int , int)" << std::endl; }
        ~Test() { std::cout << "~Test()" << std::endl; }
        Test(const Test&) { std::cout << "Test(const Test&)" << std::endl; }
        Test(Test &&) { std::cout << "Test(Test &&)" << std::endl; }
};
int main() {
    std::vector<Test> vec;

    Test t(10);
    vec.reserve(100);

    std::cout << "=================" << std::endl;
    // 直接插入对象 , 两种方式是没有区别的 
    vec.push_back(t);
    vec.emplace_back(t);

    std::cout << "=================" << std::endl;
    // 直接插入临时对象 , 两个是没有区别的 
    vec.push_back(Test(10));
    vec.emplace_back(Test(20));

    std::cout << "=================" << std::endl;
    // 给 emplace 传入 Test 对象构造所需要的参数 , 直接在容器底层构造对象即可
    vec.emplace_back(20);
    vec.emplace_back(20 , 30);

    std::cout << "=================" << std::endl;

    /*
    std::map<int , std::string> m;
    m.insert(std::make_pair(10 , "zhang san"));
    m.emplace(10 , "zhang san"); // 在map底层直接调用普通构造函数 , 生成一个 pair 对象即可
    */
    return 0;
}
```

**从代码中可以发现, emplace_back 方法要比 push_back 方法更加高效 , 它不仅一次可以入列多个数据 , 还能再自定义类类型时 , 直接传入类对象构造所需要的参数 , 在底层构造对象即可 , 非常高效**

### 1.8.2 emplace 底层剖析

**用到技术 : 模板可变参列表 , 移动语义 , 类型的完美转发 **

```c++
#include "00. head file.hpp"

template<typename T> 
struct MyAllocator {
    // allocate 
    T* allocate(size_t size) { // 开辟内存按照字节数 
        return (T*)malloc(size * sizeof(T));
    }
    // deallocate // 释放内存 , 直接 free
    void deallocate(void* p) { // void* 可以代表任意类型 
        free(p);
    }
    // construct 
    template<typename... Types> 
    void construct(T* p , Types&&... args) {
        new (p) T(std::forward<Types>(args)...);
    }
    // destroy
    void destroy(T* p) { // 调用对象的析构函数 
        p->~T();
    }
};

template<typename T , typename Alloc = MyAllocator<T>> 
class vector {
    public:		
    	vector() 
            : _vec(nullptr) 
            , _size(0)
            , _idx(0)
        {}
    	~vector() {
            for(int i = 0 ; i < _idx ; ++ i) {
                _allocator.destroy(_vec + i);
            }
            _allocator.deallocate(_vec);
        }
    	
    	void reserve(size_t size) {
            if(_vec) {
                _allocator.deallocate(_vec);
            }
            _vec = _allocator.allocate(size);
            _size = size;
		}
    
		// ----------------------------------------- Optimization ----------------------------------------
    	
    	// void push_back(const T &args) { 
        //     _allocator.construct(_vec + _idx , args);
        //     ++_idx;
        // }
        
        // void push_back(T &&args) { 
        //     _allocator.construct(_vec + _idx , std::move(args));
        //     ++_idx;
        // }
    	template<typename Types> 
        void push_back(Types&& args) { // push_back 一次进一个数据 
            _allocator.construct(_vec + _idx , std::forward<Types>(args));
            ++ _idx; 
        }
        // ----------------------------------------- Optimization ----------------------------------------
    	
    	template<typename... Types> 
    	void emplace_back(Types&&... args) {
            _allocator.construct(_vec + _idx , std::forward<Types>(args)...);
            ++ _idx;
        }
    
    private: 
    	T* _vec; // 指向一个数组 
    	int _size; // 代表数组大小 
    	int _idx; // 代表当前已经构造好的对象的下一个位置
    	Alloc _allocator; // 空间配置器 
};
class Test {
    public:

        Test(int a) { std::cout << "Test(int)" << std::endl; }
        Test(int a , int b) { std::cout << "Test(int , int)" << std::endl; }
        ~Test() { std::cout << "~Test()" << std::endl; }
        Test(const Test&) { std::cout << "Test(const Test&)" << std::endl; }
        Test(Test &&) { std::cout << "Test(Test &&)" << std::endl; }
};
int main() {
    vector<Test> vec;

    Test t(10);
    vec.reserve(100);

    std::cout << "=================" << std::endl;
    // 直接插入对象 , 两种方式是没有区别的 
    vec.push_back(t);
    vec.emplace_back(t);

    std::cout << "=================" << std::endl;
    // 直接插入临时对象 , 两个是没有区别的 
    vec.push_back(Test(10));
    vec.emplace_back(Test(20));

    std::cout << "=================" << std::endl;
    // 给 emplace 传入 Test 对象构造所需要的参数 , 直接在容器底层构造对象即可
    vec.emplace_back(20);
    vec.emplace_back(20 , 30);

    std::cout << "=================" << std::endl;
    return 0;
}
```

![image-20260416220147842](C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20260416220147842.png)

### 1.8.3 里面的重点 

以下是对你代码中**最核心知识点**的清晰总结，重点围绕 std::move、std::forward 以及为什么要这样写：

#### 1. std::move 的作用和本质

**作用**： 把一个**左值**强制转换成**右值引用**（T&&），告诉编译器：“这个对象可以被移动了”。

```c++
void push_back(T&& value) {
    _allocator.construct(_vec + _idx, std::move(value));  // 关键
}
```

**为什么要用 std::move**？

- 普通左值（如 Test t(10); vec.push_back(t);）默认只能调用拷贝构造函数。
- 当我们明确知道这个对象**之后不会再被使用**（比如临时对象或即将销毁的局部变量）时，用 std::move 可以让它走**移动构造函数**，性能更高（尤其是对象里有指针、vector、string等资源时）。

**注意**：

- std::move 只是做**类型转换**，本身不移动任何东西。
- 移动后，原对象处于**有效但未指定状态**（通常是空的或可析构状态）。

------

#### 2. std::forward 的作用和本质（完美转发）

**作用**： 在模板中**保持参数的原始值类别**（左值还是右值），不让它变成左值。

这是 emplace_back 的灵魂：

```c++
template<typename... Args>
void emplace_back(Args&&... args) {
    _allocator.construct(_vec + _idx, std::forward<Args>(args)...);
}
```

**为什么要这样写？**

假设我们调用：

```c++
vec.emplace_back(20);           // 20 是右值（字面量）
vec.emplace_back(t);            // t 是左值
```

如果不写 std::forward，直接这样写会出问题：

```c++
_allocator.construct(_vec + _idx, args...);   // 错误写法！
```

- args 在函数内部永远是**左值**（即使外面传进来的是右值）。
- 这样会导致 Test(int) 构造函数永远匹配失败，或者错误地调用拷贝构造函数。

**完美转发的工作原理（引用折叠 + 类型推导）**：

Args&& 是**万能引用**（forwarding reference）：

- 传入**左值**时 → Args 被推导为 Test&，Args&& 折叠成 Test&
- 传入**右值**时 → Args 被推导为 Test，Args&& 就是 Test&&

然后 std::forward<Args>(args) 会：

- 如果 Args 是左值类型 → 转发成左值引用
- 如果 Args 是右值类型 → 转发成右值引用

这样就能把调用者原本的“左值/右值”属性**完美保留**下来，传递给底层的构造函数。

------

#### 3. push_back vs emplace_back 的本质区别

| 操作                | 构造过程                                           | 是否产生临时对象 | 效率 |
| ------------------- | -------------------------------------------------- | ---------------- | ---- |
| push_back(t)        | 先拷贝/移动构造一个临时对象，再拷贝/移动到vector里 | 可能有           | 较低 |
| push_back(Test(10)) | 临时对象 → 移动构造到vector                        | 有               | 中等 |
| emplace_back(10)    | **直接**在vector内存里调用 Test(int)               | **无**           | 最高 |

**核心结论**：

- push_back 需要先有一个 T 类型的对象（可能是临时对象），再把它拷贝或移动进去。
- emplace_back **直接把参数转发**到 T 的构造函数，在容器预分配的内存位置上原地构造（placement new），避免了临时对象的创建。

这就是为什么现代 C++ 强烈推荐优先使用 emplace_back 而不是 push_back（除非你已经有一个现成的对象）。

------

#### 4. 为什么 emplace_back 要写成这样？

```c++
template<typename... Args>
void emplace_back(Args&&... args)
{
    _allocator.construct(_vec + _idx, std::forward<Args>(args)...);
}
```

- Args&&... args：万能引用 + 参数包，接收任意数量、任意类型的参数
- std::forward<Args>(args)...：完美转发参数包，保留每个参数的左值/右值属性
- new (p) T(std::forward<Args>(args)...)：在指定位置原地构造对象

少了任何一步，都无法实现“直接用参数构造对象而不产生临时对象”的目标。

------

#### 总结一句话：

- **std::move**：把左值“偷”成右值，让移动构造函数有机会被调用。
- **std::forward**：在模板中**忠实保留**参数的原始值类别（左值还是右值），实现完美转发。
- **emplace_back**：利用完美转发 + 原地构造，避免临时对象，性能最高。

### 1.8.4 类型完美转发 std::forward 的 补充 / 详解 

#### 1. 核心概念：万能引用（Forwarding Reference）

```c++
template<typename Args>
void emplace_back(Args&& args);   // 注意：Args&& 是万能引用（不是普通右值引用）
```

只有当参数形式是 T&&（**模板参数 T** + &&）时，才是万能引用。

#### 2. 类型推导与引用折叠规则（最重要的一张“表”）

当你调用 emplace_back(x) 时，编译器会这样推导：

| 调用方式                   | 传入参数的类型     | Args 被推导成 | Args&& 折叠后实际类型 | std::forward<Args>(args) 最终效果                   |
| -------------------------- | ------------------ | ------------- | --------------------- | --------------------------------------------------- |
| emplace_back(10)           | 右值（int 字面量） | int           | int&&（右值引用）     | 转发为右值引用 → 调用 Test(int&&) 或普通构造函数    |
| emplace_back(t)            | 左值（Test 对象）  | Test&         | Test&（左值引用）     | 转发为左值引用 → 调用拷贝构造函数 Test(const Test&) |
| emplace_back(std::move(t)) | 右值（move后的）   | Test          | Test&&                | 转发为右值引用 → 调用移动构造函数                   |

**引用折叠规则**（C++ 自动应用）：

- T& &&  → T&      （左值引用胜出）
- T&& &  → T&
- T& &   → T&
- T&& && → T&&     （右值引用保留）

这就是为什么 Args&& 能“既能接左值，又能接右值”。

#### 3. std::forward 的实际工作原理（流程图文字版）

```c++
调用者传入参数
       ↓
推导 Args 的类型（上面表格）
       ↓
函数内部：args 永远表现为【左值】（即使外面是右值）
       ↓
std::forward<Args>(args)
       ↓
根据推导出的 Args 类型决定：
   ├── 如果 Args 是左值类型（如 Test&）   → 返回左值引用（保持左值特性）
   └── 如果 Args 是非左值类型（如 int 或 Test）→ 返回右值引用（T&&）
       ↓
最终传递给构造函数 new (p) T( ... )
```

**一句话总结 std::forward**：

> 它不是“转换”，而是**根据模板参数类型信息，恢复参数原本的左值/右值属性**。

#### 4. 直观对比图（推荐你记住这张）

想象一个快递流程：

- **左值**（已有对象 t） → 必须小心搬运（拷贝）→ 用左值引用
- **右值**（临时对象 Test(10) 或 std::move(t)）→ 可以直接“偷”走资源（移动）→ 用右值引用

std::forward 的作用就是：**把快递单上的“可移动”标记忠实保留下来**，不让它在转发过程中被抹掉变成普通左值。

#### 5. 实际代码中的推导例子

```c++
Test t(10);

vec.emplace_back(20);           // 1. Args = int          → forward → int&&
vec.emplace_back(t);            // 2. Args = Test&        → forward → Test&
vec.emplace_back(std::move(t)); // 3. Args = Test         → forward → Test&&
```

如果你想看更形象的图片，我推荐下面这张经典图（完美转发整体流程）：



![Rvalue References and Perfect Forwarding in C++: A Complete Guide](https://b3252691.smushcdn.com/3252691/wp-content/uploads/2026/02/cpp_57-1536x864.png?lossy=2&strip=1&webp=1)

[techietory.com](https://techietory.com/programming/c/rvalue-references-perfect-forwarding-cpp/)

Rvalue References and Perfect Forwarding in C++: A Complete Guide



还有一张专门展示 Forwarding Reference 的图：



![C++ Forwarding References and Perfect Forwarding | by Sagar | Medium](https://miro.medium.com/v2/resize:fit:1400/1*aYj5GI_1VdazBalYdeKQnA.png)

[medium.com](https://medium.com/@sagar.necindia/c-forwarding-references-and-perfect-forwarding-ec7a9285d8b5)

C++ Forwarding References and Perfect Forwarding | by Sagar | Medium

------

**记忆口诀**（非常好用）：

- T&& 在模板里 = 万能引用（能接左值也能接右值）
- 函数体内参数永远是左值
- std::forward<T>(param) = **按原来类型重新转发**
- 左值 → 转发成左值引用（拷贝）
- 右值 → 转发成右值引用（移动/直接构造）
