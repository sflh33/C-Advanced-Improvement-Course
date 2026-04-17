#include "00. head file.hpp"
/*
C++ 11 提供的函数对象
function : 绑定器 , 函数对象 , lambda 表达式 : 它们只能使用在一条语句中  


*/

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
#if 0


void hello2(std::string str) {
    std::cout << str << std::endl;
}
void hello1() {
    std::cout << "hello world!" << std::endl;
}
int sum(int a , int b) {  // void (*pfunc)(std::string)
    return a + b;
}

class Test {
    public:  // 调用成员方法必须依赖一个对象 void (Test::*pfunc)(std::string)
        void hello(std::string str) {
            std::cout << str << std::endl;
        }
};

int main() {

    /*
    1. 用函数类型实例化 function 
    2. 通过 function 调用 operator() 函数的时候 , 需要根据函数类型传入相应的参数
    */
    // 从 function 的类模板定义处 , 看到希望用一个函数类型实例化 function 
    std::function<void()> func1 = hello1;
    func1(); // func1.operator() => hello1() 

    std::function<void(std::string)> func2 = hello2;
    func2("hello world!2"); // func2.operator(std::string) => hello2(str)
    
    std::function<int(int , int)> func3 = sum;
    std::cout << func3(20 , 30) << std::endl;

    std::function<int(int , int)> func4 = [](int a , int b) -> int { return a + b; };
    std::cout << func4(20 , 30) << std::endl;

    std::function<void(Test* , std::string)> func5 = &Test::hello;
    //func5(&Test() , "call Test::hello");
    return 0;

}
#endif