#include "00. head file.hpp"

/*
带引用计数的指针 
带引用计数 , 多个智能指针可以管理同一个资源 shared_ptr 和 weak_ptr
给每一个对象资源匹配一个引用计数

智能指针 使用资源的时候 , 引用计数 +1 , 不使用资源的时候 , 引用计数 -1 , 等于 0 的时候 , 资源释放了
*/

;// 对资源进行引用计数的类 
template<typename T>
class RefCnt {
    public: 
        RefCnt(T* ptr = nullptr)
            : mptr(ptr)
        {
            if(mptr != nullptr) {
                mcount = 1;
            }
        }
        void addRef() {
            mcount ++; // 增加资源的引用计数
        }
        int delRef() {
            return -- mcount;
        }
    private:
        T* mptr;
        int mcount;
};
template<typename T>
class CSmartPtr {

    public : 
        CSmartPtr(T* ptr = nullptr) 
            : mptr(ptr)
        {
            mpRefCnt = new RefCnt(mptr);
        }
        ~CSmartPtr() {
            if(0 == mpRefCnt->delRef()) {
                delete mptr;
                mptr = nullptr;
            }

        }

        // CSmartPtr(const CSmartPtr<T> &src) {
        //     mptr = new T(*src.mptr);
        // }
        T& operator*() const {  // 返回引用 , 改变指针指向内存
            return *mptr;
        }
        T* operator->() const {
            return mptr;
        }
        CSmartPtr(const CSmartPtr<T> &src) 
            : mptr(src.mptr) 
            , mpRefCnt(src.mpRefCnt)
        {
            if(mptr != nullptr) {
                mpRefCnt->addRef();
            }
        }
        CSmartPtr<T>& operator=(const CSmartPtr<T> &src) {
            if(this == &src) {
                return *this;
            }
            if(0 == mpRefCnt->delRef()) {
                delete mptr;
            }
            mptr = src.mptr;
            mpRefCnt = src.mpRefCnt;
            mpRefCnt->addRef();
            return *this;

        }
    private :
        T* mptr; // 指向资源的指针
        RefCnt<T>* mpRefCnt; // 指向该资源引用计数对象的指针
};

int main() {
    
    CSmartPtr<int> ptr1(new int);
    CSmartPtr<int> ptr2(ptr1);
    CSmartPtr<int> ptr3;
    ptr3 = ptr2;
    *ptr3 = 20;
    std::cout << *ptr2 << " " << *ptr3 << std::endl;
    return 0;
}