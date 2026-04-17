#include "00. head file.hpp"

/*
绑定器 和 函数对象 operator()

1. c++ STL 中的绑定器 
bind1st : operator() 的第一个形参绑定成一个默认的值 
binde2nd : operator() 的第二个形参绑定成一个默认的值 

2. c++ 从 Boost 库中引入了绑定器和 function 函数对象机制
*/
template<typename Container>
void showContainer(Container &con) {
    typename Container::iterator it = con.begin();
    for( ; it != con.end() ; ++ it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}
int main() {
    std::vector<int> vec;
    srand(time(nullptr));

    for(int i = 0 ; i < 20 ; ++ i ) {
        vec.push_back(rand() % 100 + 1);
    }

    for(int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    sort(vec.begin() , vec.end()); // 默认小到大
    showContainer(vec);

    sort(vec.begin() , vec.end() , std::greater<int>()); // 大到小排序  
    showContainer(vec);

    // 把 70 按顺序插入到 vector 容器当中  , 找第一个小于 70 的数字 
    // operator(const T &val);
    // greater a > b
    // less    a < b
    // 绑定器 + 二元函数对象 =? 一元函数对象 
    // bind1st + greater : bool operator(70 , const _Ty &val);
    // bind2nd + less : bool operator(const _Ty &val , 70);
    auto it1 = find_if(vec.begin() , vec.end() , std::bind1st(std::greater<int>() , 70));
    if(it1 != vec.end()) {
        vec.insert(it1 , 70);
    }
    showContainer(vec);


    return 0;
}