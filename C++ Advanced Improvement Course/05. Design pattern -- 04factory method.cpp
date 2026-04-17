#include "00. head file.hpp"

/*
工厂方法
Factory 基类 : 提供了一个纯虚函数(创建产品) , 定义派生类(具体产品的工厂)负责创建对应的产品, 
可以做到不同的产品在不同的工厂里面创建 , 模块化非常清晰 , 
产品和产品之间没有任何耦合 , 能够对现有工厂以及产品的修改关闭

实际上 , 很多产品是有关联关系的, 属于一个产品簇 , 不应该放在不同的工厂里面去创建 , 这样
一是不符合实际的产品对象创建逻辑 , 二是工厂太多 , 不好维护


*/

class Car {

    public:
        Car(std::string name) 
            :_name(name) 
        {}
        virtual void show() = 0;
    protected:
        std::string _name;
};
class Bmw : public Car {
    public:
        Bmw(std::string name) 
            :Car(name) 
            ,_name(name)
        {}
        void show() {
            std::cout << "BMW CAR" << _name << std::endl;
        }  
    protected:
        std::string _name;
};
class Audi : public Car {
    public:
        Audi(std::string name) 
            :Car(name) 
            , _name(name)
        {}
        void show() {
            std::cout << "Audi CAR" << _name << std::endl;
        }  
    protected:
        std::string _name;
};

class Factory {
    public: 
        virtual Car* createCar(std::string name) = 0;
};
// 宝马工厂
class BMWFactory : public Factory {
    public:
        Car* createCar(std::string name) {
            return new Bmw(name);
        }
};
// 奥迪工厂
class AUDIFactory : public Factory {
    public:
        Car* createCar(std::string name) {
            return new Audi(name);
        }
};
int main() {
    std::unique_ptr<Factory> bmwfty(new BMWFactory());
    std::unique_ptr<Factory> audifty(new AUDIFactory());
    std::unique_ptr<Car> p1(bmwfty->createCar("x6"));
    std::unique_ptr<Car> p2(audifty->createCar("a8"));
    p1->show();
    p2->show();
    return 0;
}