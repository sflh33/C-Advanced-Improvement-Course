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