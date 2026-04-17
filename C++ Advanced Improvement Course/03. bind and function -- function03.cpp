#include "00. head file.hpp"

/*
function 函数对象类型的实现原理 

*/

void hello(std::string str) {
    std::cout << str << std::endl;
}

int sum(int a , int b) {
    return a + b;
}

#if 0
template<typename Fty>
class myfunction {};

template<typename R , typename A1> 
class myfunction<R(A1)> {

    public : 
        //typedef R(*PFUNC)(A1);
        using PFUNC = R(*)(A1);
        myfunction(PFUNC pfunc) 
            :_pfunc(pfunc) 
        {}

        R operator()(A1 arg) {
            return _pfunc(arg);
        }
    private :
        PFUNC _pfunc;

};


template<typename R , typename A1 , typename A2> 
class myfunction<R(A1 , A2)> {

    public : 
        //typedef R(*PFUNC)(A1);
        using PFUNC = R(*)(A1 , A2);
        myfunction(PFUNC pfunc) 
            :_pfunc(pfunc) 
        {}

        R operator()(A1 arg1 , A2 arg2) {
            return _pfunc(arg1 , arg2);
        }
    private :
        PFUNC _pfunc;

};

#endif

template<typename Fty>  // 提供一个模板
class myfunction {};

template<typename R , typename... A> 
class myfunction<R(A...)> {

    public : 
        //typedef R(*PFUNC)(A1);
        using PFUNC = R(*)(A...);
        myfunction(PFUNC pfunc) 
            :_pfunc(pfunc) 
        {}

        R operator()(A... arg) {
            return _pfunc(arg...);
        }
    private :
        PFUNC _pfunc;

};

int main() {
    myfunction<void(std::string)> func1 = hello;
    func1("hello world!");  // func1.operator() ("hello world!")

    myfunction<int(int , int)> func2 = sum;
    std::cout << func2(20 , 30) << std::endl;
    return 0;
}