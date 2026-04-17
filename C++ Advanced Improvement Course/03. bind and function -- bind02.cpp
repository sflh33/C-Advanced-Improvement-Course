#include "00. head file.hpp"

/*
C++ bind1st 和 bind2nd 底层实现原理 

*/

template<typename Container>
void showContainer(Container &con) {
    typename Container::iterator it = con.begin();
    for( ; it != con.end() ; ++ it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}
// my_find_if 实现原理 
template<typename Iterator , typename Compare>
Iterator my_find_if(Iterator first , Iterator last , Compare comp) {
    for( ; first != last ; ++ first) { 
        if(comp(*first)) {
            return first;
        }
    }
    return last;
} 

// 绑定器 

template<typename Compare , typename T> 
class _mybind1st {  // 绑定器是函数对象的一个应用
    public:
        _mybind1st(Compare comp , T val) 
            : _comp(comp) 
            , _val(val)
        {}
        bool operator()(const T &second) {
            return _comp(_val , second);
        }
    private:
        Compare _comp;
        T _val;
};
template<typename Compare , typename T>
_mybind1st<Compare , T> mybind1st(Compare comp , const T &val) {
    return _mybind1st<Compare , T>(comp , val);  // 直接使用函数模板好处是可以直接进行类型的推演 
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
    auto it1 = my_find_if(vec.begin() , vec.end() , mybind1st(std::greater<int>() , 70));
    if(it1 != vec.end()) {
        vec.insert(it1 , 70);
    }
    showContainer(vec);

    return 0;
}