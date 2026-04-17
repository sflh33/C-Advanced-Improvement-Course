#include "00. head file.hpp"

/*
shared_ptr : 强智能指针  : 可以改变资源的引用计数 
weak_ptr : 弱智能指针 : 不会改变资源的引用计数 , 智能观察资源 , 不能使用资源
weak_ptr => shared_ptr => 资源(内存)

强智能指针循环引用(交叉引用)是什么问题? 造成什么结果? 怎么解决 ?
造成 new 出来的资源无法释放 , 资源泄露问题 
定义对象的时候 , 用强智能指针, 引用对象的地方, 用弱智能指针
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
            std::cout << "非常好用的方法" << std::endl;
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
int main() {
    std::shared_ptr<A> pa(new A());
    std::shared_ptr<B> pb(new B());

    pa->_ptrb = pb;
    pb->_ptra = pa;

    std::cout << pa.use_count() << std::endl;
    std::cout << pb.use_count() << std::endl;

    pb->func();

    return 0;
}