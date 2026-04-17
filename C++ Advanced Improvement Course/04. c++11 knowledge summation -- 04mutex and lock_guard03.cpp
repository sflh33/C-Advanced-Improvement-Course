#include "00. head file.hpp"

/*
unique_lock condition_variable 
1. lock_guard 和 unique_lock
2. condition_variable 的 wait 和 notify_all 和 notify_one 方法
*/

std::mutex mtx; // pthread_mutex_t
std::condition_variable cv; // pthread_condition_t
int main() {
    /*
    通知在 cv 上等待的线程 , 条件成立了, 起来干活了
    其它在 cv 上等待的线程 , 收到通知 => 变为阻塞状态 => 获取互斥锁了 , 线程再继续进行
    */
    cv.notify_all(); 
    // 它不仅可以使用在简单的临界区代码段的互斥操作中 , 还能用在函数调用过程中 
    // std::unique_lock<std::mutex> lck(mtx); 
    // cv.wait(lck); // 1. 使线程进入等待状态  2. lck.unlock() 可以把持有的 mtx 释放掉
    // 只能用在临界区代码段的互斥操作中
    //std::lock_guard<std::mutex> lck(mtx); // 当这个函数出作用域 调用 unlock 方法 , 不可能用在函数参数传递或者返回过程中

    //mtx.lock(); // 获取互斥锁 
    //mtx.unlock(); // 释放互斥锁
    return 0;
}