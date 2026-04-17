#include "00. head file.hpp"

/*
多线程额访问共享对象的线程安全问题

*/

class B;
class A {
    public:
        A() {
            std::cout << "A()" << std::endl;
        }
        ~A() {
            std::cout << "~A()" << std::endl;
        }

        void testA() {
            std::cout << "this is a good idea" << std::endl;
        }
        std::weak_ptr<B> _ptrb;
};

class B {
    public:
        B() {
            std::cout << "B()" << std::endl;
        }
        ~B() {
            std::cout << "~B()" << std::endl;
        }
        void func() {
            //_ptra->testA(); // 弱指针没有提供 * 和 -> 重载
            std::shared_ptr<A> ps = _ptra.lock(); // 提升方法 , 将弱指针提升到强指针
            if(ps != nullptr) { // 提升成功
                ps->testA();
            }
        }
        std::weak_ptr<A> _ptra;
};
// 子线程
void handler01(std::weak_ptr<A> q) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // q 在访问 A 对象的时候 , 需要侦测一下 A 对象是否成活  -- 使用强弱智能指针
    std::shared_ptr<A> sq = q.lock();
    if(sq != nullptr) {
        sq->testA();
    }
    else {
        std::cout << "object is destructed , can't be accessed " << std::endl;
    }

}
int main() {
    {
        std::shared_ptr<A> p(new A());  // 出作用域才析构
        std::thread t1(handler01 , std::weak_ptr<A>(p));
        //std::this_thread::sleep_for(std::chrono::seconds(2));
        t1.detach();

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    // 阻塞等待线程结束
    //t1.join();
    std::this_thread::sleep_for(std::chrono::seconds(20));
    return 0;
}