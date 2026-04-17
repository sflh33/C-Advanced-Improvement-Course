#include "00. head file.hpp"

/*
适配器模式 : 让不兼容的接口可以在一起工作 
电脑    =>    投影到    =>    投影仪上   VGA , HDMI 
VGA 接口的电脑 , 投影仪也是 VGA 接口 
*/
class VGA { // VGA 接口类 
    public: 
        virtual void play() = 0;
        std::string getType() const {
            return "VGA";
        }
};
class TV01 : public VGA { // TV01 表示支持VGA接口的投影仪
    public: 
        void play() {
            std::cout << "Comuter -> VGA -> TV01" << std::endl;
        }
};
// 实现一个电脑类 (只支持 VGA 接口)
class Computer {
    public:
        void playVideo(VGA* pVGA) { // 由于电脑只支持 VGA 接口 , 所以该方法的参数也只能支持 VGA 接口的指针/引用
            pVGA->play();
        }
};

/*
方法一 : 换一个支持 HDMI接口的电脑 , 这个就叫代码重构
方法二 : 买一个转换头(适配器).把 VGA信号转成 HDMI 信号 , 这个叫添加适配器类
*/

// 进了一批新的投影仪 , 但是新的投影仪都是只支持 HDMI 接口 
class HDMI {
    public:
        virtual void play() = 0;
};

class TV02 : public HDMI {
    public: 
        void play() {
            std::cout << "Computer -> HDMI -> TV02" << std::endl;
        }
};

// 由于电脑(VGA接口) 和投影仪(HDMI接口)无法直接相连 , 所以需要添加适配器类
class VGAToHDMIAdapter : public VGA { // 对对象进行适配 
    public: 
        VGAToHDMIAdapter(HDMI* p)
            : pHdmi(p)
        {}
        void play() { // 该方法相当于就是转换头 , 做不同接口信号转换的
            pHdmi->play(); 
        }
    private:    
        HDMI* pHdmi;
};
int main() {
    Computer computer;
    //computer.playVideo(new TV01());
    computer.playVideo(new VGAToHDMIAdapter(new TV02()));
    return 0;
}