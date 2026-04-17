#include "00. head file.hpp"

/*
智能指针的自定义删除器
智能指针最大的用处 : 能够保证资源的绝对释放
*/
// unique_ptr , shared_ptr  都可以提供删除器

template<typename T>
class MyDeletor{

    public:
        void operator()(T* ptr) const {
            std::cout << "call MyDeletor.operator()" << std::endl;
            delete[] ptr;
        }
};
template<typename T>
class MyFileDeletor{

    public:
        void operator()(T* ptr) const {
            std::cout << "call MyFileDeletor.operator()" << std::endl;
            fclose(ptr);
        }
};
int main() {
    // std::unique_ptr<int , MyDeletor<int>> ptr1(new int[100]);
    // std::unique_ptr<FILE , MyFileDeletor<FILE>> ptr2(fopen("data.txt", "w"));

    // lambda 表达式  => 函数对象 function 

    std::unique_ptr<int , std::function<void (int *)>> ptr1(new int[100] , 
        [](int* p) -> void {
            std::cout << "call lambda release new int[100]" << std::endl;
            delete[] p;
    });

    std::unique_ptr<FILE , std::function<void (FILE*)>> ptr2(fopen("data.txt", "w"), 
        [](FILE* p) -> void {
            std::cout << "call lambda release fopen(\"data.txt\", \"w\")" << std::endl;
            fclose(p);
    });
    return 0;
}