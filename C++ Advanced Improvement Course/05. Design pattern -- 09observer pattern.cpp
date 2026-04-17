#include "00. head file.hpp"

/*
行为型模式 : 主要关注的是对象之间的通信 
观察者-监听者(发布-订阅)设计模式 : 主要关注的是对象的一对多的关系,也就是多个对象都依赖一个对象,当该对象的状态发生改变时,其它对象都能够接收到相应的通知  

一组数据(对象) => 通过这一组数据 => 曲线图(对象1)/ 柱状图(对象2) / 圆饼图 (对象3)
当数据对象改变时 , 对象1 , 对象2 , 对象3 应该及时收到相应的通知 

Observer1    Observer2    Observer3
              Subject (主题有更改 , 应该及时通知相应的观察者 , 去处理相应的事件)
*/
class Observer { // 观察者抽象类
    public:
        // 处理消息的接口
        virtual void handle(int msgid) = 0;
};
// 第一个观察者实例
class Observer1 : public Observer {
    public: 
        void handle(int msgid) {
            switch(msgid) {
                case 2 :
                    std::cout << "observer1 recv2 msg!" << std::endl;
                    break;
                case 3 :
                    std::cout << "observer1 recv3 msg!" << std::endl;
                    break;
                default :
                    std::cout << "observer1 recv unknow msg!" << std::endl;
            }

        }
};

class Observer2 : public Observer {
    public: 
        void handle(int msgid) {
            switch(msgid) {
                case 1 :
                    std::cout << "observer2 recv1 msg!" << std::endl;
                    break;
                case 3 :
                    std::cout << "observer2 recv3 msg!" << std::endl;
                    break;
                default :
                    std::cout << "observer2 recv unknow msg!" << std::endl;
            }

        }
};


class Observer3 : public Observer {
    public: 
        void handle(int msgid) {
            switch(msgid) {
                case 1 :
                    std::cout << "observer3 recv1 msg!" << std::endl;
                    break;
                case 2 :
                    std::cout << "observer3 recv2 msg!" << std::endl;
                    break;
                default :
                    std::cout << "observer3 recv unknow msg!" << std::endl;
            }

        }
};
// 主题类
class Subject {
    public:
        // 给主题增加观察者对象
        void addObserver(Observer* obsver , int msgid) {
            _subMap[msgid].push_back(obsver);
        }
        // 主题检测发生改变 , 通知相应的观察者对象处理事件
        void dispatch(int msgid) {
            auto it = _subMap.find(msgid);
            if(it != _subMap.end()) {
                for(Observer* pObser : it->second) {
                    pObser->handle(msgid);
                }
            }
        }   
    private:
        std::unordered_map<int , std::list<Observer*>> _subMap;
};
int main() {
    Subject Subject;

    Observer* p1 = new Observer1();
    Observer* p2 = new Observer2();
    Observer* p3 = new Observer3();

    Subject.addObserver(p1 , 1);
    Subject.addObserver(p1 , 2);
    Subject.addObserver(p2 , 2);
    Subject.addObserver(p3 , 1);
    Subject.addObserver(p3 , 3);

    int msgid = 0;
    for( ; ; ) {
        std::cout << "insert id : ";
        std::cin >> msgid;
        if(msgid == -1) 
            break;
        Subject.dispatch(msgid);
    }

    return 0;
}