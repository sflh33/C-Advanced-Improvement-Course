#include "00. head file.hpp"

/*
海量数据的查/去重的问题 
1. 哈希表 -- 空间换时间
2. 分治 
3. Bloom Filter -- 布隆过滤器 
4. 特定字符串类型的查重 -- TrieTree 字典树(前缀树)
*/

int main() {
    /*
    a , b 两个文件 , 里面都有10亿个整数 , 内存限制 400M , 求出 a , b 两个文件当中重复的元素有哪些

    用哈希表把 a 文件全部读到内存当中 , 从 b 文件当中读数据 , 在内存哈希表中查重 , 内存太大不满足 

    把 a 和 b 两个大文件 , 划分成个数相等的一系列小文件(分治的思想)
    10亿 -> 1G * 4 = 4G * 2 = 8G / 400M = 29 个小文件 (尽量选择素数)
    把 a 全部读到内存当中 , 存储到哈希表
    把 b 的数据拿到哈希表中查询

    从 a 文件中读取数据 , 通过数据 %27 = file_index
    从 b 文件中读取数据 , 通过数据 %27 = file_index

    a 和 b 两个文件中 , 数据相同的元素 , 进行哈希映射以后 , 肯定在相同序号的小文件当中

    
    */
    return 0;
}

#if 0 // 分治思想
int main() {
    // 有一个文件 , 大量的数据 , 50亿个整数 , 内存限制 400M , 找出文件中重复的元素 
    /*
    50亿相当于 5G 内存 , 32位下一个整数 4个字节 , 需要的数据域和地址域 , 4 * 5G * 2 = 40G 
    太大了 , 分治思想 : 大文件划分成为小文件 , 使得每一个小文件能够加载到内存当中 , 求出对应的重复元素 , 
    把结果写到一个存储重复元素的文件中  

    大文件 => 小文件的个数(40G / 400M = 120个小文件)

    遍历大文件的元素 , 把每一个元素根据哈希映射函数 , 放到对应序号的小文件当中 
    */
    return 0;
}
#endif



#if 0 // 哈希表查重 
int main() {
    const int SIZE = 10000;
    int arr[SIZE] = { 0 };
    for(int i = 0 ; i < SIZE ; ++ i) {
        arr[i] = rand();
    }

    // 在 SIZE 的数据量中 , 找出谁重复了 , 并且统计重复的次数 
    std::unordered_map<int , int> map;
    for(int val : arr) {
        map[val] ++; 
    }

    for(auto pair : map) {
        if(pair.second > 1) {
            std::cout << "number : " << pair.first << " numbers : " << pair.second << std::endl;
        }
    }
    return 0;
}
#endif