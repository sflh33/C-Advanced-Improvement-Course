#include "00. head file.hpp"

/*
抽象工厂方法  
把所有有关联关系的 , 属于一个产品簇的所有产品创建的接口函数 , 放在一个抽象工厂里面 , 派生类(具体产品的工厂)应该负责创建该产品簇里面的所有的产品
*/

// 系列产品一 
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

// 系列产品二 
class CarLight{
    public:
        virtual void show() = 0;
};

class BMWLight : public CarLight {
    public:
        void show() {
            std::cout << "BMW Light" << std::endl;
        }
};

class AUDILight : public CarLight {
    public:
        void show() {
            std::cout << "AUDI Light" << std::endl;
        }
};

// 工厂方法 => 抽象方法(对一组有关联关系的产品簇提供产品对象的统一创建)
class AbstractFactory {
    public: 
        virtual Car* createCar(std::string name) = 0;
        virtual CarLight* createCarLight() = 0; //工厂方法 创建汽车关联的产品
};
// 宝马工厂
class BMWFactory : public AbstractFactory {
    public:
        Car* createCar(std::string name) {
            return new Bmw(name);
        }
        CarLight* createCarLight() {
            return new BMWLight();
        }
};
// 奥迪工厂
class AUDIFactory : public AbstractFactory {
    public:
        Car* createCar(std::string name) {
            return new Audi(name);
        }

        CarLight* createCarLight() {
            return new AUDILight();
        }
};


int main() {
    // 现在考虑产品 一类产品(有关联关系的系列产品) -- 不可能每一件产品都要建一个工厂来生产 , 所以有关联的要在一起生产
    std::unique_ptr<AbstractFactory> bmwfty(new BMWFactory());
    std::unique_ptr<AbstractFactory> audifty(new AUDIFactory());
    std::unique_ptr<Car> p1(bmwfty->createCar("x6"));
    std::unique_ptr<Car> p2(audifty->createCar("a8"));
    std::unique_ptr<CarLight> l1(bmwfty->createCarLight());
    std::unique_ptr<CarLight> l2(audifty->createCarLight());
    p1->show();
    p2->show();
    l1->show();
    l2->show();

    return 0;
}