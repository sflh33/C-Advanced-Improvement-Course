#include "00. head file.hpp"

/*
大数据 TopK 问题 
1. 求最大的/最小的前 k 个元素
2. 求最大的/最小的第 k 个元素 

10000 个整数 , 找值前 10 大的元素
先用前10个整数创建一个小根堆(最小值就在堆顶) , 然后遍历剩下的整数 , 如果整数比堆顶元素大,
那么删除掉堆顶元素(出堆) , 然后再把整数入堆, 遍历完所有整数 , 小根堆里面放的就是值最大的前10个元素了
如果找的是第 k 小(大根堆堆顶)或者第 k 大(小根堆堆顶)的 , 只需要访问堆顶一个元素就可以了 
解法1. 大根堆/小根堆  => 优先级队列

解法2. 快排分割函数 
经过快排分割函数 , 能够在 O(log2n)时间内 , 把小于基准数的整数调整到左边 , 把大于基准数的整数调整到右边 , 基准数(index)就可以认为是第(index + 1)小的整数了 
*/
int myparation(std::vector<int>& , int , int);
int selectNok(std::vector<int> &vec , int i , int j , int k) {
    int pos = myparation(vec , i , j);
    if(k - 1 == pos) {
        return pos;
    }
    else if(pos < k - 1) {
        return selectNok(vec , pos + 1 , j , k);
    }
    else {
        return selectNok(vec , i , pos - 1 , k);
    }
    //throw "none";
}

int myparation(std::vector<int> &arr , int i , int j) {
    int k = arr[i];
    while(i < j) {
        while(i < j && arr[j] >= k) {
            j --;
        }
        if(i < j) {
            arr[i ++] = arr[j];
        }
        while(i < j && arr[i] < k) {
            i ++;
        }
        if(i < j) {
            arr[j --] = arr[i];
        }
    }
    arr[i] = k;
    return i;
}

int main() {
    // 快排分割解决 : 时间复杂度: O(log2n)
    // 求 vector 容器中第10小的
    std::vector<int> vec;
    for(int i = 0 ; i < 100 ; ++ i) {
        vec.push_back(rand());
    }

    std::cout << "10th small num is : " << vec[selectNok(vec , 0 , vec.size() - 1 , 10)] << std::endl;
    
    // 有一个大文件 , 里面放的是整数 , 内存限制200M , 求最大的前10个 
    /*
    分治的思想 
    */
    
    return 0;
}

#if 0
int main() {

    // 求 vector 前10大的数字 
    std::vector<int> vec;
    for(int i = 0 ; i < 10000 ; ++ i) {
        vec.push_back(rand());
    }
    // 定义小根堆 
    std::priority_queue<int , std::vector<int> , std::greater<int>> minHeap;
    // 先往小根堆放入10个元素
    for(int i = 0 ; i < 10 ; ++ i) {
        minHeap.push(vec[i]);
    }
    /*
    遍历剩下的元素依次和堆顶元素进行比较 , 如果比堆顶元素大 , 那么删除堆顶元素
    , 把当前元素添加到小根堆中 , 元素遍历完成 , 堆中剩下10个元素 , 就是值最大的10个元素 
    */
    // 时间复杂度 O(n)
    for(int i = 10 ; i < 10000 ; ++ i) {
        if(vec[i] > minHeap.top()) {
            minHeap.pop();
            minHeap.push(vec[i]);
        }
    }

    while(!minHeap.empty()) {
        std::cout << minHeap.top() << " ";
        minHeap.pop();
    }
    return 0;
} 
#endif