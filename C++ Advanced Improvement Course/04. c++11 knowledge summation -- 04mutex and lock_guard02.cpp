#include "00. head file.hpp"

/*
c++ 所有的 STL 容器都不是安全的 
c++ 多线程编程 - 线程间的同步通讯机制  
多线程编程有两个问题 : 
1. 线程间的互斥 
线程在运行时存在竞态条件 => 临界区代码段(竞态条件发生的区域) => 必须保证原子操作 => 用互斥锁 mutex , 轻量级的无锁实现 CAS 
strace ./a.out mutex => pthread_mutex_t
2. 线程间的同步通信  
生产者 , 消费者线程模型 
*/

std::mutex mtx; // 定义互斥锁 , 做线程之间的操作
std::condition_variable cv; // 定义条件变量 , 做线程间的同步通信操作
// 生产者生产一个物品, 通知消费者消费一个;消费者消费完了, 消费者再通知消费者继续生产物品了
class Queue {
    public:
        void put(int val) {
            //std::lock_guard<std::mutex> gard(mtx);
            std::unique_lock<std::mutex> lck(mtx);
            while(!que.empty()) {
                // que 不为空 , 生产者应该通知消费者去消费 , 消费完了再继续生产
                // 1.生产者线程应该进入等待状态 , 2.并且把 mtx 互斥锁释放掉
                cv.wait(lck);
            }
            que.push(val);
            cv.notify_all(); // 通知其它所有的线程 , 我生产了一个物品, 你们赶紧消费吧
            /*
            notift_all : 通知其它所有线程  , 其他线程得到该通知就会从等待状态变成阻塞状态 , 获取互斥锁才能继续执行
            notify_one : 通知其它一个线程
            */
            std::cout << "producer produces :" << val << " object" << std::endl;
        }
        int get() { // 消费物品
            //std::lock_guard<std::mutex> gard(mtx); // scoped_ptr 把拷贝构造去了
            std::unique_lock<std::mutex> lck(mtx);
            while(que.empty()) {
                // 消费者线程发现que是空的 , 通知生产者线程先生产物品
                // 1. 进入等待状态 , 2. 把互斥锁释放 
                std::unique_lock<std::mutex> lck(mtx); // 智能指针的思想 
                cv.wait(lck);
            }
            int val = que.front();
            que.pop();
            cv.notify_all(); // 通知其他线程我消费完了 , 赶紧生产吧
            std::cout << "consumer consumes : " << val << " object" << std::endl;
            return val;
        }
 
    private:
        std::queue<int> que;
};
void producer(Queue* que) { // 生产者线程 
    for(int i = 1 ; i <= 10 ; ++ i) {
        que->put(i);
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void consumer(Queue* que) { // 消费者线程 
    for(int i = 1 ; i <= 10 ; ++ i) {
        que->get();
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
int main() {
    Queue que; // 两个线程共享的队列 
    std::thread t1(producer , &que);
    std::thread t2(consumer , &que);

    t1.join();
    t2.join();
    return 0;
}