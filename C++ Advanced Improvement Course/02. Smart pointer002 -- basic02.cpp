#include "00. head file.hpp"

/*
不带引用计数的智能指针 
*/

template<typename T>
class CSmartPtr {

    public : 
        CSmartPtr(T* ptr = nullptr) 
            : mptr(ptr)
        {}
        ~CSmartPtr() {
            delete mptr;
        }

        CSmartPtr(const CSmartPtr<T> &src) {
            mptr = new T(*src.mptr);
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
    // 不带引用计数的智能指针  : auto_ptr(c++ 库) ,  scoped_ptr(c++11) , unique_ptr(c++11)
    // 怎么解决浅拷贝问题
#if 0
    // CSmartPtr<int> p1(new int);
    // CSmartPtr<int> p2(p1);
    std::auto_ptr<int> ptr1(new int);
    std::auto_ptr<int> ptr2(ptr1);  // 处理浅拷贝的方法是永远让最后一个智能指针管理资源 , 前面的智能指针全部置为 nullptr
    *ptr2 = 20;
    std::cout << *ptr1 << std::endl;

    // 不推荐使用 auto_ptr , std::vector<std::auto_ptr<int>> vec1; vec2(vec1);  不建议 , 容器涉及拷贝构造和赋值操作 , 会将最后一个指针前面的裸指针置为 nullptr , 访问空内存报错
#endif

    // scoped_ptr 的拷贝和赋值直接删除了, 想用 scoped_ptr 进行拷贝和赋值jiuhuibaocuo 
    // scoped_ptr(const scoped_ptr<T>&) = delete;
    // scoped_ptr<T>& operator = (const scoped_ptr<T>&) = delete;

    // 推荐 unique_ptr
    // unique_ptr(const unique_ptr<T>&) = delete;
    // unique_ptr<T>& operator = (const unique_ptr<T>&) = delete;
    // std::move => 右值引用 得到当前变量的右值类型
    std::unique_ptr<int> p1(new int);
    std::unique_ptr<int> p2(std::move(p1)); // 这个之所以能实现是因为提供了这样的函数 
    // unique_ptr(unique_ptr<T>&&)
    // unique_ptr<T>& operator = (unique_ptr<T>&&)
    // unique_ptr<T> getSmartPtr() {
    //    unique_ptr<T> ptr(new T());
    //    return ptr;
    //}

    return 0;
}