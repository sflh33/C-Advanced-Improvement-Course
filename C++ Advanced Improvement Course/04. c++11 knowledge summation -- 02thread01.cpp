#include "00. head file.hpp"
/*
C++ 语言级别的多线程编程  => 代码可以跨平台 
thread / mutex / condition_variable 
lock_quard / unique_lock 
atomic 原子类型 基于 CAS 操作的原子类型 , 线程安全的 
sleep_for

c++ 语言层面 : 调用thread 
在 Windows 下用的还是 createThread
在 Linux 下用的还是s pthread_create 

线程内容一 : 
一. 怎么创建启动一个线程 
std::thread 定义一个线程对象,传入线程所需要的线程参数

二. 子线程如何结束 
子线程函数运行完成,线程就结束了 

三. 主线程如何处理子线程 
t1.join() : 等待 t 线程结束 , 当前线程继续往下运行 
t1.detch() : 把 t 线程设置为分离线程 , 主线程结束 , 整个子线程都自动结束了 , 如果主线程（main 函数）执行完毕退出了
，操作系统会直接强行杀死整个进程。这意味着即便子线程被 detach 了，如果它还没跑完，也会被操作系统暴力掐断。这通常会导致资源泄露或数据损坏。 

主线程一定要等待子线程结束吗 ? 
主线程运行完了, 不能够存在正在运行还未结束的其他子线程 , 就要抛异常
*/

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