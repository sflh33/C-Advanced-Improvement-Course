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
#if 0
template<typename T>
class Vector {
    public:
        Vector() {
            std::cout << "call Vector template init" << std::endl;
        }
    private:
};
// 下面这个是对 char* 类型提供的完全特例化版本 
template<>
class Vector<char*> {
    public:
        Vector() {
                std::cout << "call Vector<char*> template init" << std::endl;
        }
};
// 下面这个是对指针类型提供的部分特例化版本 
template<typename Ty>
class Vector<Ty*> {
    public:
       Vector() {
                std::cout << "call Vector<Ty*> template init" << std::endl;
        }

};
// 针对函数指针(有返回值 , 有两个形参变量)提供的部分特例化
template<typename R , typename A1 , typename A2>
class Vector<R(*)(A1 , A2)> {
    public:
        Vector() {
            std::cout << "call Vector<R(*)(A1 , A2)> template init" << std::endl;
        }
};
// 针对函数类型提供的部分(有一个返回值 , 两个形参变量)特例化
template<typename R , typename A1 , typename A2>
class Vector<R(A1 , A2)> {
    public: 
        Vector() {
            std::cout << "call Vector<R(A1 , A2)> template init" << std::endl;
        }
};
int sum(int a , int b) {
    return a + b;
} 
int main() {
    Vector<int> vec1;
    Vector<char*> vec2;
    Vector<int*> vec3;
    Vector<int(*)(int , int)> func4;
    Vector<int(int , int)> vec5;

    // 区分一下函数类型和函数指针类型
    using PFUNC1 = int(*)(int , int);
    PFUNC1 pfunc1 = sum;
    std::cout << pfunc1(10 , 20) << std::endl;

    using PFUNC2 = int(int , int);
    PFUNC2* pfunc2 = sum;
    std::cout << pfunc2(10 , 20) << std::endl;

    return 0;
}
#endif
#if 0
template<typename T>
bool compare(T a , T b) {
    std::cout << "template compare" << std::endl;
    return a > b;
}

template<>
bool compare<const char*>(const char* a , const char* b) {
    return strcmp(a , b) > 0;
}
int main() {
    compare(10 , 20);
    compare("aaa" , "bbb");
    return 0;
}
#endif