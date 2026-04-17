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

// Test GetObject(Test t) {  // 不能返回局部的或者临时对象的指针或者引用
//     int val = t.getData();
//     Test tmp(val); // 调用整形参数的构造
//     //return tmp; // tmp 本身是出不来的 , 需要在 main 函数栈帧上构造一个临时对象 , 由 tmp 拷贝构造临时对象
//     Test result(tmp);
//     return result;
// } // 这个函数结束有两个对象需要析构 : tmp , 形参 t

// 参数传递过程中,尽量使用引用传递,不要使用值传递
Test GetObject(Test &t) { 
    int val = t.getData();
    return Test(val); // 用临时对象拷贝构造一个新对象 , C++ 的优化是此时不产生临时对象 . 直接构造新对象
}
// int main() {
//     Test t1; // 1. 调用带整型参数的构造函数 
//     Test t2; // 2. 调用带整型参数的构造函数 
//     t2 = GetObject(t1); // 函数调用 , 实参传递给形参是初始化 , t 是一个正在定义的 Test 的对象 , 由 t1 拷贝构造 t
//     return 0;
// }
int main() {
    Test t1; 
    Test t2 = GetObject(t1); 

    return 0;
}
/*

总结 : 三条对象优化规则 
1. 函数参数传递过程中,优先按引用传递,不用值传递
2. 在函数返回一个对象的时候 , 尽量直接返回一个临时对象, 而不要返回一个定义过的对象 , C++ 的优化在这时是会把临时对象优化掉 , 直接构造新对象
3. 接收返回值是对象的调用的时候,优先按初始化的方式接收,不要用赋值的方式接收
*/