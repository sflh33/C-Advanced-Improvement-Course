#include "00. head file.hpp"

/*
c++ thread -- 模拟车站三个窗口卖票程序
*/

/*
多线程程序
竞态条件 : 多线程执行的结果是一致的 , 不会随着 CPU 对线程不同的调用顺序 ,而产生不同的运行结果
*/

int ticketcount = 100; // 车站有100张车票 , 由三个窗口一起卖票 
std::mutex mtx; // 全局的一把互斥锁 
// 模拟卖票的线程函数  lock_guard unique_lock
void sellTicket(int index) {
    while(ticketcount > 0) { // ticketCount = 1 锁 + 双重判断
        //mtx.lock();
        {
            // std::lock_guard<std::mutex> lock(mtx); //栈上的局部对象  , 保证所有线程都能释放锁 ,防止死锁问题的发生
            std::unique_lock<std::mutex> ulock(mtx);
            ulock.lock();
            if(ticketcount > 0) {
                // 临界区代码段 => 原子操作 => 线程间互斥操作了 => mutex  
                std::cout << "window " << index << " is selling " << ticketcount << " tickets" << std::endl;
                ticketcount --; // 不是一个安全的操作 , 需要互斥
            }
            ulock.unlock();
        }
        //mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
int main() {


    std::list<std::thread> tlist;
    for(int i = 0 ; i <= 3 ; ++ i) {
        tlist.push_back(std::thread(sellTicket , i)); 
    }

    for(std::thread &t : tlist) {
        t.join();
    }

    std::cout << "tickets are empty" << std::endl;
    return 0;
}