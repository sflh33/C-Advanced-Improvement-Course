#include "00. head file.hpp"

/*
对象使用过程中背后调用了哪些方法
*/

#if 0
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

#endif 
#if 0
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
#endif 