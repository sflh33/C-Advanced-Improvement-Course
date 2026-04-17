#include "00. head file.hpp"

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
};

CMyString GetString(CMyString &str) {
    const char* pstr = str.c_str();
    CMyString tmpStr(pstr);
    return std::move(tmpStr);
}
int main() {

    CMyString str1("aaaaaaaaaaaaaaaa");
    CMyString str2;
    str2 = GetString(str1);
    std::cout << str2.c_str() << std::endl;
    return 0;
}