#include "00. head file.hpp"

// 智能指针 : 保证能做到资源的自动释放
// 利用栈上的对象出作用域自动析构来做到资源的自动释放
// CSmartPtr<int>* p = new CSmartPtr<int>(new int);  // 这是个裸指针 , 需要 delete , 不能够直接把智能指针放在堆上
template<typename T>
class CSmartPtr {

    public : 
        CSmartPtr(T* ptr = nullptr) 
            : mptr(ptr)
        {}
        ~CSmartPtr() {
            delete mptr;
        }
        T& operator*() const {  // 返回引用 , 改变指针指向内存
            return *mptr;
        }
        T* operator->() const {
            return mptr;
        }
    private :
        T* mptr;
};
int main() {

    CSmartPtr<int> ptr1(new int);
    *ptr1 = 20;

    class Test {

        public : 
            void test() {
                std::cout << "call test()" << std::endl;
            }
    };
    CSmartPtr<Test> ptr(new Test());
    ptr->test();
    return 0;
}
int main1() {
    // 内存分为三部分 : data 段 , heap 堆 , stack 栈
    int* p = new int(10);  // 裸指针
    *p = 20;
    delete p; // 用完之后一定要释放
    return 0;
}