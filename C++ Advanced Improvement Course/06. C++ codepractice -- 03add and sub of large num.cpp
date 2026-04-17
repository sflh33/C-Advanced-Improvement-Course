#include "00. head file.hpp"

/*
大数的加减法 
// 用字符串存储数据 
*/
class BigInt {
    public:
        BigInt(std::string str) 
            :strDigit(str) 
        {}
    private: 
        std::string strDigit;
        friend std::ostream& operator<<(std::ostream &out , const BigInt &src);
        friend BigInt operator+(const BigInt &lhs , const BigInt &rhs);
        friend BigInt operator-(const BigInt &lhs , const BigInt &rhs);
};
std::ostream& operator<<(std::ostream &out , const BigInt &src) {
    out << src.strDigit; 
    return out;
}

BigInt operator+(const BigInt &lhs , const BigInt &rhs) {
    std::string result;
    bool flag = false; // 进位 
    int i = lhs.strDigit.length() - 1;
    int j = rhs.strDigit.length() - 1;

    for( ; i >= 0 && j >= 0 ; -- i , -- j) {
        int ret = lhs.strDigit[i] - '0' + rhs.strDigit[i] - '0';
        if(flag) {
            ret += 1;
            flag = false;
        }

        if(ret >= 10) {
            ret %= 10;
            flag = true;
        }
        result.push_back(ret + '0');
    }
    
    if(i >= 0) {
        while(i >= 0) {
            int ret = lhs.strDigit[i] - '0';
            if(flag) {
                ret += 1;
                flag = false;
            }

            if(ret >= 10) {
                ret %= 10;
                flag = true;
            }
            result.push_back(ret + '0');
            -- i;
        }
    }
    else if(j >= 0) {
        while(j >= 0) {
            int ret = rhs.strDigit[j] - '0';
            if(flag) {
                ret += 1;
                flag = false;
            }

            if(ret >= 10) {
                ret %= 10;
                flag = true;
            }
            result.push_back(ret + '0');
            -- j;
        }
    }

    if(flag) {
        result.push_back('1');
    }

    reverse(result.begin() , result.end());
    return BigInt(result);
}

BigInt operator-(const BigInt &lhs , const BigInt &rhs) {
    std::string result;
    bool flag = false;
    bool minor = false; // 是否需要添加负号

    std::string maxStr = lhs.strDigit;
    std::string minStr = rhs.strDigit;

    if(maxStr.length() < minStr.length()) { // 右边的数大 , 小数减大数需要添加符号
        maxStr = rhs.strDigit;
        minStr = lhs.strDigit;
        minor = true;
    }
    else if(maxStr.length() == minStr.length()) {
        if(maxStr < minStr) { // 比较字符串
            maxStr = rhs.strDigit;
            minStr = lhs.strDigit;
            minor = true;
        }
        else if(maxStr == minStr) {
            result.push_back('0');
        }
    }
    else {
        ;
    }

    int i = maxStr.length() - 1;
    int j = minStr.length() - 1;
    for( ; i >= 0 && j >= 0 ; -- i , -- j) {
        int ret = maxStr[i] - minStr[j];
        if(flag) {
            ret -= 1;
            flag = false;
        }
        
        if(ret < 0) {
            ret += 10;
            flag = true;
        }
        result.push_back(ret + '0');
    }

    while(i >= 0) {
        int ret = maxStr[i] - '0';

        if(flag) {
            ret -= 1;
            flag = false;
        }

        if(ret < 0) {
            ret += 10;
            flag = true;
        }
        result.push_back(ret + '0');
    }
    
    std::string retStr;
    auto it = result.rbegin();
    for( ; it != result.rend() ; ++ it) {
        if(*it != '0') {
            break;
        }
    }

    for( ; it != result.rend() ; ++ it) {
        retStr.push_back(*it);
    }
    
    if(minor) { 
        retStr.insert(retStr.begin() , '-');
    }
    return BigInt(retStr);
}
int main() {
    BigInt big1("7754549495242422897857557952792792");
    BigInt big2("446546498646546115616516984616");

    BigInt big3("7778889");
    BigInt big4("7778888");

    std::cout << big2 - big1 << std::endl;
    std::cout << (big4 - big3) << std::endl;
    
    return 0;             
}  

