#include "00. head file.hpp"

/*
简单工厂 : simple factory  -- 把对象的创建封装在一个接口函数里面 , 通过传入不同的标识 , 来返回创建的对象 
客户不用自己负责 new 对象 , 不用了解对象创建的详细过程

提供创建对象实例的接口方法(函数) 不闭合 , 不能对修改关闭
为什么要工厂模式 ? 主要是封装了对象的创建 
当创建的类比较多的时候 , 用工厂模式设计
不可能一个工厂能做所有的事情 , 才有了 factory method 
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
        {}
        void show() {
            std::cout << "BMW CAR" << std::endl;
        }  
};
class Audi : public Car {
    public:
        Audi(std::string name) 
            :Car(name) 
        {}
        void show() {
            std::cout << "Audi CAR" << std::endl;
        }  
};
enum CarType {
    BMW , AUDI
};
class SimpleFactory {

    public:
        Car* createCar(CarType ct) {
            switch(ct) {
                case BMW:
                    return new Bmw("BMW X1");
                case AUDI:
                    return new Audi("Audi A6");
                default:
                    std::cerr << "the partment is wrong" << std::endl;
                    break;
            }
            return nullptr;
        }
};

int main() {
#if 0
    //Car* p1 = new BMW("BMW X1");  // 在常规方法中 , 我们需要记忆每个派生类,了解对象创建的具体内容,完全不需要
    //Car* p2 = new Audi("Audi A6");

    SimpleFactory* factory = new SimpleFactory();
    Car* p1 = factory->createCar(BMW);
    Car* p2 = factory->createCar(AUDI);
    p1->show();
    p2->show();

    delete p1;
    delete p2;
    delete factory;
#endif 
    // 智能指针写法 
    std::unique_ptr<SimpleFactory> factory(new SimpleFactory());
    std::unique_ptr<Car> p1(factory->createCar(BMW));
    std::unique_ptr<Car> p2(factory->createCar(AUDI));
    p1->show();
    p2->show();

    return 0;
}