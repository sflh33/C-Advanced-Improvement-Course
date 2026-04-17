#include "00. head file.hpp"

/*
装饰器模式 -- 结构性模式 : 主要是增加现有类的功能  , 但是增加一个现有类的功能 , 还有一个方法 , 就是新增加一个子类 
通过子类实现功能增强的问题 , 增加进来的子类太多了 
*/

class Car {
    public: 
        virtual void show() = 0;
};

// 三个实体的派生类 
class Bmw : public Car {
    public: 
        void show() {
            std::cout << "this is a bmw car" << std::endl;
        }

};

class Audi : public Car {
    public: 
        void show() {
            std::cout << "this is a Audi car" << std::endl;
        }

};

class Bnze : public Car {
    public: 
        void show() {
            std::cout << "this is a Bnze car" << std::endl;
        }

};

// 装饰器的基类 
class CarDecorator : public Car {
    public: 
        CarDecorator() = default;
        CarDecorator(Car* p) 
            : pCar(p)
        { }

    private:
        Car* pCar;
};
// 装饰器1 : 定速巡航的功能
class CouncreteDecorator01 : public CarDecorator {
    public: 
        CouncreteDecorator01(Car* p) 
            : pCar(p)
        {} 
        void show() {
            pCar->show();
            std::cout << "cruise control funcion" << std::endl;
        }
    private: 
        Car* pCar;

};

class CouncreteDecorator02 : public Car {
    public: 
        CouncreteDecorator02(Car* p) 
            : pCar(p)
        {} 
        void show() {
            pCar->show();
            std::cout << "automatic eergency braking" << std::endl;
        }
    private: 
        Car* pCar;

};


class CouncreteDecorator03 : public Car {
    public: 
        CouncreteDecorator03(Car* p) 
            : pCar(p)
        {} 
        void show() {
            pCar->show();
            std::cout << "lane departure" << std::endl;
        }
    private: 
        Car* pCar;

};

int main() {
    Car* p1 = new CouncreteDecorator01(new Bmw());
    p1 = new CouncreteDecorator02(p1);
    p1->show();
    Car* p2 = new CouncreteDecorator02(new Audi());
    p2->show();
    Car* p3 = new CouncreteDecorator03(new Bnze());
    p3->show();
    return 0;
}