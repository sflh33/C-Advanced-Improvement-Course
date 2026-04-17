#include "00. head file.hpp"

/*
海量数据查重和topK的综合应用 
查重 : 数据是否有重复 , 以及数据重复的次数 
topK : 元素的值前K大/小, 第K大或者第K小

题目 : 数据的重复次数最大的前 K 个 , 第 K 个?

有一个大文件 , 内存限制 200M , 求文件中重复次数最多的前10个 

大文件分成小文件
大文件里面的数据 通过 哈希映射 , 把数据离散的放入小文件中
哈希统计(map) + 堆/快排
*/
// 大文件划分小文件(哈希映射) + 哈希统计 + 小根堆(快排分割)

int main() {
    // 定义大文件的文件名
    const std::string bigFile = "big_data.dat";
    
    // 分区数量，使用素数53以减少数据倾斜
    const int M = 53;                          
    
    // 测试时生成的文件中包含的数字总数量（实际使用时可删除这行和生成文件的代码）
    const long long totalNumbers = 10000000LL; 

    // ==================== 生成测试大文件（实际使用时应该删除这部分） ====================
    std::cout << "Generating large test file...\n";
    {
        // 以二进制写入模式打开输出文件
        std::ofstream out(bigFile, std::ios::binary);
        
        // 初始化随机数种子，保证每次运行生成的随机数不同
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        
        // 循环生成指定数量的随机数字并写入文件
        for (long long i = 0; i < totalNumbers; ++i) {
            // 生成一个随机数，并加入少量偏移制造重复数据
            long long num = std::rand() % 1000000LL + (i % 100000LL);
            
            // 将数字以二进制形式写入文件（比文本方式更快）
            out.write(reinterpret_cast<const char*>(&num), sizeof(num));
        }
        // out 会在作用域结束时自动关闭
    }

    // ==================== 阶段1：哈希分区（把大文件拆分成多个小文件） ====================
    std::cout << "Phase 1: Hash partitioning into " << M << " small files (prime number)...\n";
    {
        // 创建 M 个输出文件流，用于写入各个小文件
        std::vector<std::ofstream> outs(M);
        
        // 打开 M 个小文件，文件名分别为 part_0.dat ~ part_52.dat
        for (int i = 0; i < M; ++i) {
            outs[i].open("part_" + std::to_string(i) + ".dat", std::ios::binary);
        }

        // 以二进制读取模式打开大文件
        std::ifstream in(bigFile, std::ios::binary);
        
        long long num;   // 用于存储从文件中读取的每个数字
        
        // 循环读取大文件中的每一个数字，直到文件结束
        while (in.read(reinterpret_cast<char*>(&num), sizeof(num))) {
            // 计算当前数字应该被分配到哪个小文件（哈希映射）
            // 使用 num % M 作为分区索引，素数M能让分布更均匀
            int idx = std::abs(static_cast<int>(num % M));
            if (idx < 0) idx = 0;   // 防止负数索引（理论上不会发生，但保险起见）
            
            // 将当前数字写入对应的小文件
            outs[idx].write(reinterpret_cast<const char*>(&num), sizeof(num));
        }

        // 关闭所有小文件输出流
        for (auto& f : outs) f.close();
    }

    // ==================== 阶段2：处理每个小文件，统计频率并选出本地 Top10 ====================
    std::cout << "Phase 2: Processing each small file...\n";

    // 定义一个类型别名：PII 表示 pair<出现次数, 数字>
    using PII = std::pair<int, long long>;  

    // 用于保存所有小文件的本地 Top10 结果
    std::vector<std::vector<PII>> allLocalTops;

    // 依次处理每一个小文件
    for (int i = 0; i < M; ++i) {
        // 构造当前小文件的文件名
        std::string partFile = "part_" + std::to_string(i) + ".dat";
        
        // 以二进制读取模式打开当前小文件
        std::ifstream in(partFile, std::ios::binary);

        // 使用 unordered_map 统计当前小文件中每个数字出现的次数
        std::unordered_map<long long, int> freq;
        
        long long num;
        // 读取小文件中的所有数字并统计频率
        while (in.read(reinterpret_cast<char*>(&num), sizeof(num))) {
            freq[num]++;                    // 数字出现一次，计数加1
        }
        in.close();                         // 关闭当前小文件

        // 定义小根堆的比较函数：按出现次数从小到大（堆顶是出现次数最少的）
        auto cmp = [](const PII& a, const PII& b) {
            return a.first > b.first;
        };
        
        // 创建小根堆，用于维护出现次数最多的10个数字
        std::priority_queue<PII, std::vector<PII>, decltype(cmp)> minHeap(cmp);

        // 遍历当前小文件的所有频率统计结果
        for (const auto& p : freq) {
            // 把 {出现次数, 数字} 放入堆中
            minHeap.push({p.second, p.first});
            
            // 如果堆的大小超过10，弹出堆顶（即出现次数最少的那个）
            if (minHeap.size() > 10) {
                minHeap.pop();
            }
        }

        // 用于临时保存当前小文件的 Top10 结果
        std::vector<PII> localTop;
        
        // 把堆中的所有元素取出并保存到 localTop 中
        while (!minHeap.empty()) {
            localTop.push_back(minHeap.top());
            minHeap.pop();
        }

        // 如果有有效结果，则把当前小文件的 Top10 保存到总集合中
        if (!localTop.empty()) {
            allLocalTops.push_back(std::move(localTop));   // 使用 move 避免拷贝，提高效率
        }
    }

    // ==================== 阶段3：合并所有本地 Top10，得到全局 Top10 ====================
    std::cout << "Phase 3: Merging global Top 10...\n";

    // 定义全局小根堆的比较函数（同样按出现次数从小到大）
    auto cmpGlobal = [](const PII& a, const PII& b) {
        return a.first > b.first;
    };
    
    // 创建全局小根堆，用于从所有候选结果中选出最终的 Top10
    std::priority_queue<PII, std::vector<PII>, decltype(cmpGlobal)> globalHeap(cmpGlobal);

    // 遍历所有小文件的本地 Top10
    for (const auto& local : allLocalTops) {
        for (const auto& p : local) {
            // 把每个候选结果放入全局堆
            globalHeap.push(p);
            // 保持堆的大小不超过10，自动淘汰出现次数较少的
            if (globalHeap.size() > 10) {
                globalHeap.pop();
            }
        }
    }

    // 用于保存最终的全局 Top10 结果
    std::vector<PII> globalTop10;
    
    // 把全局堆中的所有元素取出
    while (!globalHeap.empty()) {
        globalTop10.push_back(globalHeap.top());
        globalHeap.pop();
    }

    // 对最终结果按出现次数从高到低排序（方便阅读）
    std::sort(globalTop10.rbegin(), globalTop10.rend());

    // ==================== 输出最终结果 ====================
    std::cout << "\n=== Top 10 Most Frequent Numbers ===\n";
    
    // 遍历并打印全局 Top10
    for (const auto& p : globalTop10) {
        std::cout << "Number: " << p.second 
                  << "  appears: " << p.first << " times\n";
    }

    return 0;   // 程序正常结束
}
#if 0
int main() {
    // 用 vec 存储数字 
    std::vector<int> vec;
    for(int i = 0 ; i < 200000 ; ++ i) {
        vec.push_back(rand() + i);
    }

    // 统计所有数字的重复次数 , key : 数字的值 , value : 数字重复的次数 
    std::unordered_map<int , int> map;
    for(int val : vec) {
        map[val] ++;
    }
    // 先定义一个小根堆 
    using PII = std::pair<int , int>;
    using FUNC = std::function<bool(PII& , PII&)>;
    using MinHeap = std::priority_queue<PII , std::vector<PII> , FUNC>;

    // 自定义小根堆元素的大小比较方式
    MinHeap minheap([] (auto &a , auto &b)->bool {
        return a.second > b.second; // 小根堆比较方式
    });

    // 先往堆里放十个数据 
    int k = 0;
    auto it = map.begin();

    for( ; it != map.end() && k < 10 ; ++ it , ++ k) {
        minheap.push(*it);
    }

    // 如果碰到有比堆顶大的 , 堆顶元素出堆
    for( ; it != map.end() ; ++ it) {
        if(it->second > minheap.top().second) {
            minheap.pop();
            minheap.push(*it);
        }
    }

    while(!minheap.empty()) {
        std::cout << "num : " << minheap.top().first << " appears : " << minheap.top().second << std::endl;
        minheap.pop(); 
    }

    return 0;
}
#endif