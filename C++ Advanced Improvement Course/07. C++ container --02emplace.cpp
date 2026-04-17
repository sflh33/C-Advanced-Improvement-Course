#include "00. head file.hpp"

/*
聚焦于 emplace 的底层实现 
*/
template<typename T>
struct MyAllocator {
    // allocate dellacote 
    T* allocate(size_t size) {
        return (T*)malloc(size * sizeof(T));
    }

    void deallocate(void* p) {
        free(p);    
    }
    // construct destroy 
    template<typename... Types>
    void construct(T* p , Types&&... args) {
        new (p) T(std::forward<Types>(args)...);
    }

    void destroy(T* p) {
        p->~T();
    }
};
template<typename T , typename Alloc = MyAllocator<T>>
class vector {
    public:
        vector() 
            : _vec(nullptr) 
            , _size(0)
            , _idx(0)
        {}
        ~vector() {
            for (int i = 0; i < _idx; ++i) {
                _allocator.destroy(_vec + i);
            }
            _allocator.deallocate(_vec);
        }
        void reserve(size_t size) {
            if (_vec) {
                for (int i = 0; i < _idx; ++ i) {
                    _allocator.destroy(_vec + i);
                }
                 _allocator.deallocate(_vec);
            }
            _vec = _allocator.allocate(size);
            _size = size;
        }
        
        
        // void push_back(const T &args) { 
        //     _allocator.construct(_vec + _idx , args);
        //     ++_idx;
        // }
        
        // void push_back(T &&args) { 
        //     _allocator.construct(_vec + _idx , std::move(args));
        //     ++_idx;
        // }

        template<typename Types> 
        void push_back(Types&& args) { // push_back 一次进一个数据 
            _allocator.construct(_vec + _idx , std::forward<Types>(args));
            ++ _idx; 
        }
        

        template<typename... Types>
        void emplace_back(Types&&... args) {  // 利用引用折叠, 因为这里两个一起实现 , 如果传入的是个实参(10) , 那么 Types 如果被识别成了 Types 类型 , 这就是个右值(&&) , 右值无法绑定到右值上 , 识别成 Types& , 右值才会被绑定到(& &&)左值上 
            // 不管是左值引用 , 右值引用变量 , 它本身是个左值 , 传递过程中要保持 args 的引用类型 , 是左值 ? 右值? 用完美转发自己判断
            _allocator.construct(_vec + _idx , std::forward<Types>(args)...);
            ++ _idx;
        }
        
    private:
        T* _vec;
        int _size;
        int _idx;
        Alloc _allocator;
};


class Test {
    public:

        Test(int a) { std::cout << "Test(int)" << std::endl; }
        Test(int a , int b) { std::cout << "Test(int , int)" << std::endl; }
        ~Test() { std::cout << "~Test()" << std::endl; }
        Test(const Test&) { std::cout << "Test(const Test&)" << std::endl; }
        Test(Test &&) { std::cout << "Test(Test &&)" << std::endl; }
};
int main() {
    vector<Test> vec;

    Test t(10);
    vec.reserve(100);

    std::cout << "=================" << std::endl;
    // 直接插入对象 , 两种方式是没有区别的 
    vec.push_back(t);
    vec.emplace_back(t);

    std::cout << "=================" << std::endl;
    // 直接插入临时对象 , 两个是没有区别的 
    vec.push_back(Test(10));
    vec.emplace_back(Test(20));

    std::cout << "=================" << std::endl;
    // 给 emplace 传入 Test 对象构造所需要的参数 , 直接在容器底层构造对象即可
    vec.emplace_back(20);
    vec.emplace_back(20 , 30);

    std::cout << "=================" << std::endl;

    /*
    std::map<int , std::string> m;
    m.insert(std::make_pair(10 , "zhang san"));
    m.emplace(10 , "zhang san"); // 在map底层直接调用普通构造函数 , 生成一个 pair 对象即可
    */
    return 0;
}