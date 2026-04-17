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
int main() {

    CMyString str1 = "aaa";

    std::vector<CMyString> vec;
    vec.reserve(10);
    std::cout << "-------------------------" << std::endl;
    vec.push_back(str1);
    vec.push_back(CMyString("bbb"));

    std::cout << "-------------------------" << std::endl;


    /*
    CMyString str1 = "hello ";
    CMyString str2 = "world!";

    std::cout << "-------------------------" << std::endl;
    CMyString str3 = str1 + str2;

    std::cout << "-------------------------" << std::endl;

    std::cout << str3 << std::endl;

    */
    return 0;
}