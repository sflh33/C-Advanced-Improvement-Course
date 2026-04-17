#include "00. head file.hpp"

/*
c++ 11 函数对象的升级版 => lambda 表达式 
函数对象的缺点 : 
使用在泛型算法参数传递 , 比较性质/自定义操作 , 优先级队列

lambda 表达式的语法 :
[捕获外部变量](形参列表)->返回值{操作代码};

// 如果 lambda 的返回值是不需要的 , 那么 ->返回值 这一部分可以省去

[捕获外部变量]
[] : 表示不捕获外部变量 
[=] : 表示以传值的方式捕获外部的所有变量
[&] : 以传引用的方式捕获外部的所有变量 
[this] : 捕获外部的 this 指针
[= , &a] : 以传值的方式捕获外部的所有变量 , 但是 a 变量以传引用的方式捕获
[a , b] : 以值传递的方式捕获外部变量 a 和 b
[a , &b] : a 以值传递的方式捕获 , b 以传引用的方式捕获 
*/

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
#if 0
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

#endif