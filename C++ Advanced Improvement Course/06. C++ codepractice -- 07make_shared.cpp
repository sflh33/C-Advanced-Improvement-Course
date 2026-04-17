#include "00. head file.hpp"

/*
建议用make_shared 代替 shared_ptr 
// 优点 : 内存分配效率高了 , 防止资源泄漏的风险 
// 缺点 : make_shared 无法自定义删除器  , 导致托管的资源延迟释放即就是现在没有 shared_ptr 指向资源了 , 资源也无法释放

*/
class Test {
    public:
        void func(int a) {
            std::cout << "a = " << a << std::endl;
        }
        void func(int a , int b) {
            std::cout << "a = " << a << " b = " << b << std::endl;
        }

};
int main() {
    std::shared_ptr<int> sp1(new int(10));
    std::shared_ptr<Test> sp2(new Test());
    sp2->func(10);
    sp2->func(10 , 20);


    std::shared_ptr<int> sp3 = std::make_shared<int>(10);
    *sp3 = 20;
    std::cout << "*sp3 = " << *sp3 << std::endl;
    std::shared_ptr<Test> sp4(std::make_shared<Test>());
    sp4->func(15);
    sp4->func(15 , 25);

    
    return 0;
}