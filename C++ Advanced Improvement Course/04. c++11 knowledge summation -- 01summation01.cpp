#include "00. head file.hpp"

/*
c++11 标准相关内容 总结  
一. 关键字和语法 
auto : 可以根据右值 , 推导出右值的类型 , 然后左边变量的类型也就已知了
nullptr : 给指针专用(能够和整数进行区别) NULL 是一个宏定义 , #define NUll 0
for_each : 可以遍历数组,容器等
for(Type val : container) {  => 底层就是通过指针或者迭代器实现的
    std::cout << val << " ";
}
右值引用 : move 移动语义函数 , forward 类型完美转发函数 
模板的一个新特性 : typename... A 表示可变参(类型参数)

二. 绑定器和函数对象一类的
function : 函数对象
bind : 绑定器  bind1st 和 bind2nd 将二元函数对象绑定为 医院函数对象 
lambda 表达式 

三. 智能指针 
shared_ptr 和 weak_ptr : 带计数功能的智能指针 
auto_ptr 和 scoped_ptr 和 unique_ptr : 不带计数功能的指针 

四. 容器 
以前 set 和 map : 红黑树 O(log2n) [以二为底]
unordered_set 和 unordered_map : 哈希表 O(1)
array : 数组 --  不如vector灵活 , array 无法扩容, 必须知道具体容量 
forward_list : 前向链表--比 list 更轻量 , 但不如它们灵活

五 : C++ 语言级别支持的多线程 
*/ 
int main() {
    return 0;
}