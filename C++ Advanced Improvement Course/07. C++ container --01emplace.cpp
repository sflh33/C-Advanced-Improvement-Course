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