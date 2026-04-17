#include "00. head file.hpp"

/*
lambda表达式 
既然 lambda 表达式只能使用在语句当中 , 如果想跨语句使用之前定义好的 lambda 表达式怎么办 , 用什么类型表示 lambda 表达式?
用 function 类型来表示函数对象的类型了 

lambda表达式 => 函数对象 
*/

class Data {
    public:
        Data(int val1 = 10 , int val2 = 20) 
            : ma(val1)
            , mb(val2)
        {}
        // bool operator>(const Data &data) { // 成员变量很多 , 不灵活
        //     return ma > data.ma;
        // }
        // bool operator<(const Data &data) {
        //     return ma < data.ma;
        // }

        int ma;
        int mb;  
};
int main() {

    // 优先级队列 
    std::priority_queue<Data , std::vector<Data> , std::function<bool(Data& , Data&)>> MaxHeap(
        [](Data &d1 , Data &d2)->bool 
    {
        return d1.ma > d2.ma;
    });
    MaxHeap.push(Data(10 , 20)); // 优先级队列里面要进行比较 , 这个 Data 可不能比较(见上)
    MaxHeap.push(Data(15 , 25));
    MaxHeap.push(Data(20 , 25));

    // 智能指针自定义删除器 delete p;
    //std::unique_ptr<FILE> ptr1(fopen("data.txt" , "w")); // 这样写默认delete释放 , 但是正确的释放时 fclose(FILE*)
    std::unique_ptr<FILE , std::function<void(FILE*)>> ptr1(fopen("data.txt" , "w")
        , [](FILE* pf) {
            fclose(pf);
    });

#if 0
    std::map<int , std::function<int(int , int)>> caculateMap;
    caculateMap[1] = [](int a , int b)->int {
        return a + b;
    };
    caculateMap[2] = [](int a , int b)->int {
        return a - b;
    };
    caculateMap[3] = [](int a , int b)->int {
        return a * b;
    };

    caculateMap[4] = [](int a , int b)->int {
        return a / b;
    };

    std::cout << "选择:";
    int choice;
    std::cin >> choice;
    std::cout << "10 + 15 : " << caculateMap[choice](10 , 15) << std::endl;
#endif
    
    return 0;
}