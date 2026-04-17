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

// 饿汉式的单例 -- 一定是线程安全的
class Singelton {
    public:
        // 需要定义一个接口 , 用户通过这个接口获取实例对象 , 为什么定义成静态 , 因为这个不通过对象调用 
        static Singelton* getInstance() { // #3 获取类的唯一实例对象的接口方法
            return &instance;
        }
    private:
        static Singelton instance; // #2 定义一个唯一的类实例对象 
        // 限制对象的创建个数 , 就要限制构造函数 , #1 那就将构造函数私有化
        Singelton() {

        }
        // 拷贝构造函数和运算符重载赋值 delete 掉
        Singelton(const Singelton&) = delete;
        Singelton& operator=(const Singelton&) = delete;
};
Singelton Singelton::instance;
int main() {
    Singelton* p1 = Singelton::getInstance();
    Singelton* p2 = Singelton::getInstance();
    Singelton* p3 = Singelton::getInstance();

    std::cout << "p1 = " << p1 << " p2 = " << p2 << " p3 = " << p3 << std::endl;
    return 0;
}
