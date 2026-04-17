#include "00. head file.hpp"
/*
设计模式 -- 单例模式  -- 创建性模式 
一个类不管创建多少次对象 , 永远只能得到该类型一个对象的实例 
A* p1 = new A();
A* p2 = new A();
A* p3 = new A();
正常new 3次有3个实例对象 , 但是单例模式只允许有一个对象

常用到单例模式的地方 : 日志模块 , 数据库
单例模式 : 
饿汉式单例模式 : 还没有获取实例对象 , 实例对象就已经产生了
懒汉式单例模式 : 唯一的实例对象 , 直到第一次获取它的时候 , 才产生
*/

// 不用互斥锁的懒汉单例模式 
class Singelton {
    public:
        // 需要定义一个接口 , 用户通过这个接口获取实例对象 , 为什么定义成静态 , 因为这个不通过对象调用 
        static Singelton* getInstance() { // #3 获取类的唯一实例对象的接口方法
            // 函数静态局部变量的初始化 , 在汇编指令上已经自动添加线程互斥指令了
            static Singelton instance; // 第一次运行到它才进行初始化
            return &instance;
        }
    private:
        // 限制对象的创建个数 , 就要限制构造函数 , #1 那就将构造函数私有化
        Singelton() {

        }
        // 拷贝构造函数和运算符重载赋值 delete 掉
        Singelton(const Singelton&) = delete;
        Singelton& operator=(const Singelton&) = delete;
};


#if 0
std::mutex mtx;
// 线程安全的懒汉式单例模式
class Singelton {
    public:
        // 需要定义一个接口 , 用户通过这个接口获取实例对象 , 为什么定义成静态 , 因为这个不通过对象调用 
        // 是不是可重入函数 ?  -- 如果一个函数只有在多线程环境下运行 , 而且不会发生竞态条件 , 就是重入函数
        static Singelton* getInstance() { // #3 获取类的唯一实例对象的接口方法
            //std::lock_guard<std::mutex> lck(mtx); // 进行线程间的互斥 , 锁的力度太大 , 单线程条件下还要加锁 , 应该缩小范围
            if(instance == nullptr) {
                std::lock_guard<std::mutex> lck(mtx); // 锁 + 双重判断
                if(instance == nullptr) {
                    /*
                    #1. 开辟内存 
                    #2. 构造对象  // 没关系的操作在汇编层面很有可能倒换顺序
                    #3. 给 instance 赋值
                    */
                    instance = new Singelton();
                }
            }
            return instance;
        }
    private:
        static Singelton* volatile instance; // #2 定义一个唯一的类实例对象 
        // 限制对象的创建个数 , 就要限制构造函数 , #1 那就将构造函数私有化
        Singelton() {

        }
        // 拷贝构造函数和运算符重载赋值 delete 掉
        Singelton(const Singelton&) = delete;
        Singelton& operator=(const Singelton&) = delete;
};
Singelton *volatile Singelton::instance = nullptr; // 为什么写 volatile ? 因为 static 处于数据段 , 多个线程共享这个数据 , CPU 在调用时会保存缓存 , 所以加 volatile 

int main() {
    Singelton* p1 = Singelton::getInstance();
    Singelton* p2 = Singelton::getInstance();
    Singelton* p3 = Singelton::getInstance();

     std::cout << "p1 = " << p1 << " p2 = " << p2 << " p3 = " << p3 << std::endl;
    return 0;
}
#endif