#include "00. head file.hpp"

/*
窗口卖票 int count = 100;

count ++ ; 和 count -- ; 都是不安全的操作 ,可能要减去两个但是只减去一个

lock_guard<std::mutex> guard(mtx);
count -- ;

互斥锁是比较重的 , 临界区代码做的事情稍稍复杂, 多 , 大炮打蚊子
从系统理论来说 : CAS 来保证上面 ++ -- 操作的原子特性就足够了 , 无锁操作 

*/

volatile std::atomic_bool isReady = false; // volatile 是防止多线程对共享变量进行缓存 ,访问的都是原始的值 
volatile std::atomic_int mycount = 0;

void task() {
    while(!isReady) {
        std::this_thread::yield(); // 线程出让当前的CPU时间片 , 等待下一次调度
    }

    for(int i = 0 ; i < 100 ; ++ i) {
        mycount ++;
    }
}

int main() {
    std::list<std::thread> tlist;
    for(int i = 0 ; i < 10 ; ++ i) {
        tlist.push_back(std::thread(task));
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));
    isReady = true;

    for(std::thread &t : tlist) {
        t.join();
    }

    std::cout << "count : " << mycount << std::endl;

    return 0;
}