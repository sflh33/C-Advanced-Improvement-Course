#include "00. head file.hpp"

template<typename T>
class Allocator
{
    public :
        T *allocator(size_t size) { // 只负责内存开辟
            return (T*)malloc(sizeof(T) * size);
        }
        void dellocate(void *p) { // void * 可以指向任意类型
            free(p);
        }
        /*
        // 接收左值
        void construct(T *p , const T &val) {
            new (p) T(val); // 利用定位new，在指定的内存位置放上特定的值
        }
        // 接收右值
        void construct(T *p , T &&val) {
            new (p) T(std::move(val)); // 利用定位new，在指定的内存位置放上特定的值
        }
        */
        // 也可以优化 
        template<typename Ty>
        void construct(T *p , Ty &&val) { // Ty 只是给引用类型用的
            new (p) T(std::forward<Ty>(val)); // 利用定位new，在指定的内存位置放上特定的值
        }
        void destroy(T *p) {
            p -> ~T(); //T() 代表了 T 类型的析构函数
        }
};
template<typename T , typename Alloc = Allocator<T>>
class vector {
    public :
    vector(int size = 10 , const Alloc &alloc = Allocator<T>())
        	: _allocator(alloc)
        	//: _first(new T[size])
        	, _first(_allocator.allocator(size))
            , _last(_first)
            , _end(_first + size)
        {}
    ~vector() {
        // delete[] _first;
        // 要析构有效的内存
        for(T* p = _first ; p != _last ; p ++ )
        {
            _allocator.destroy(p);
        }
        _allocator.dellocate(_first);
        _first = _last = _end = nullptr;
    }
    vector(const vector<T> &rhc)  { // 拷贝函数
        int size = rhc._end - rhc._first;
        int len = rhc._last - rhc._first;
        // _first = new T[size]; 同样不能使用new ，因为初始化和开辟内存还是被同时做的
        _first = _allocator.allocator(size);
        _last = _first + len;
        _end = _first + size;
        for(int i = 0 ; i < len ; i ++ )
        {
            //_first[i] = rhc._first[i];	
            _allocator.construct(_first + i , rhc._first[i]);
        }
    }
    vector<T> &operator= (const vector<T> &rhc) {
        if(this == &rhc)
        {
            return *this;
        }
        // delete[] _first;
        for(T* p = _first ; p != _last ; p ++ )
        {
            _allocator.destroy(p);
        }
        _allocator.dellocate(_first);

        int size = rhc._end - rhc._first;
        int len = rhc._last - rhc._first;
        // _first = new T[size];
        _first = _allocator.allocator(size);
        _last = _first + len;
        _end = _first + size;
        for(int i = 0 ; i < len ; i ++ )
        {
            _first[i] = rhc._first[i]; 
        }
        return *this;
    }
    /*
    // 接收左值
    void push_back(const T &val) {
        if(full()) expand();
        // *_last ++ = val;  不对
        _allocator.construct(_last , val);
        _last ++;
    }
    // 接收右值 -- 一个右值引用变量本身是一个左值
    void push_back(T &&val) {
        if(full()) expand();
        _allocator.construct(_last , std::move(val));
        _last ++;
    }
    */
    template<typename Ty> // 函数模板的类型推演 + 引用折叠 代替上面两个方法
    void push_back(Ty &&val) { // Ty 是 CMyString&  &&val --> 引用折叠 --> 左值引用 + 右值引用 = 左值引用  右值 + 右值 = 右值引用
        if(full()) expand();
        // forward : 类型的完美转发 : 能够识别左值和右值类型
        // move : 移动语义 , 得到右值类型, 给move 里放一个左值 , 就会得到一个右值 , 是通过类型强转实现的
        _allocator.construct(_last , std::forward<Ty>(val));  // 这里的 val 本身还是个左值
        _last ++;
    }
    void pop_back() {
        if(empty()) return;
        // _last --; 不对
        -- _last;
        _allocator.destroy(_last); // 管理的是对象的析构
    }
    int back() const {
        return *(_last - 1);
    }
    bool full() const {
        return _last == _end;
    }
    bool empty() const {
        return _first == _last;
    }
    int size() const {
        return (_last - _first);
    }
    private : // 指向额外资源，浅拷贝是一定会出现问题的，要手写拷贝函数
    Alloc _allocator; // 配置器对象
    T *_first; // 指向数组的起始位置
    T *_last; // 指向数组有效元素的后继元素
    T *_end; // 指向数组的末尾位置
    
    void expand()
    {
        int size = _last - _first;
        // T *ptemp = new T[2 * size]; 不对
        T* ptemp = _allocator.allocator(2 * size);
        for(int i = 0 ; i < size ; i ++ )
        {
            // ptemp[i] = _first[i]; 不对
            _allocator.construct(ptemp + i , _first[i]);
        }
        // delete[] _first; 不对
        for(T * p = _first ; p != _last ; ++ p)
        {
            _allocator.destroy(p);
        }
        _allocator.dellocate(_first);
        _first = ptemp;
        _last = _first + size;
        _end = _first + 2 * size;
    }
};


class CMyString {
    public : 
        //CMyString() = default;
        CMyString(const char* str = nullptr) {
            std::cout << "CMyString(const char* str)" << std::endl;
            if(str != nullptr) {
                mptr = new char[strlen(str) + 1];
                strcpy(mptr , str); 
            }
            else {
                mptr = new char[1];
                *mptr = '\0';
            }
        }

        ~CMyString() {
            std::cout << "~CMyString()" << std::endl;
            delete[] mptr;
            mptr = nullptr;
        }

        // 带左值引用的拷贝构造
        CMyString(const CMyString &cms) { // 普通对象就是匹配到左值引用这里
            std::cout << "CMyString(const CMyString &cms)" << std::endl;
            mptr = new char[strlen(cms.mptr) + 1];
            strcpy(mptr , cms.mptr);
        }
        // 带右值引用的拷贝构造
        CMyString(CMyString &&cms) {  // cms 引用的就是一个临时对象 , 因为临时对象没名字,右值
            std::cout << "CMyString(CMyString &&cms)" << std::endl;
            mptr = cms.mptr;
            cms.mptr = nullptr;
        }

        // 带左值引用的赋值重载函数
        CMyString& operator = (const CMyString &cms) {
            std::cout << "CMyString& operator = (const CMyString &cms)" << std::endl;
            if(this == &cms) {
                return *this;
            }
            delete[] mptr;
            mptr = new char[strlen(cms.mptr) + 1];
            strcpy(mptr , cms.mptr);
            return *this;
        }
        // 带右值引用的赋值重载函数
        CMyString& operator = (CMyString &&cms) {
            std::cout << "CMyString& operator = (const CMyString &&cms)" << std::endl;
            if(this == &cms) {
                return *this;
            }
            delete[] mptr;
            mptr = cms.mptr;
            cms.mptr = nullptr;
            return *this;
        }

        const char* c_str() const {
            return mptr;
        }
    private :
        char* mptr;
        friend CMyString operator+(const CMyString &lhs , const CMyString &rhs);
        friend std::ostream& operator<<(std::ostream& out , const CMyString &rhs);
};      

CMyString operator+(const CMyString &lhs , const CMyString &rhs) {
    // char* ptmp = new char[strlen(lhs.mptr) + strlen(rhs.mptr) + 1];
    // strcpy(ptmp , lhs.mptr);
    // strcat(ptmp , rhs.mptr);
    // // return CMyString(ptmp); 每 new 一块内存 无法delete 造成内存泄漏 , 只能构造新对象再释放它
    // CMyString tmpStr(ptmp);
    // delete[] ptmp;
    // return tmpStr;  // 临时对象的问题
    CMyString tmpStr;
    tmpStr = new char[strlen(lhs.mptr) + strlen(rhs.mptr) + 1];
    strcpy(tmpStr.mptr , lhs.mptr);
    strcat(tmpStr.mptr , rhs.mptr);
    return tmpStr; 
}

std::ostream& operator<<(std::ostream &out , const CMyString &rhs) {
    out << rhs.mptr;
    return out;
}
int main()
{
    CMyString str1 = "aaa";

    vector<CMyString> vec;

    std::cout << "-------------------------" << std::endl;
    vec.push_back(str1); // CMyString&
    vec.push_back(CMyString("bbb")); // CMyString&& 

    std::cout << "-------------------------" << std::endl;

}