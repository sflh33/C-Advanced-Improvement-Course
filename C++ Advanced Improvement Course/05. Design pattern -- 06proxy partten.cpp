#include "00. head file.hpp"

/*
代理模式 Proxy 模式 : 通过代理类来控制实际对象的访问权限  -- 结构型模式
客户    助理     老板(委托类) , 助理就是老板的代理 , 相当于 proxy , 访问老板的权限控制  
*/
class VideoSite { // 抽象类
    public:
        virtual void freeMovie() = 0; // 免费电影 
        virtual void vipMovie() = 0; // vip 电影
        virtual void ticketMovie() = 0; // 用券电影 
};

class FixBugVideoSite : public VideoSite { // 委托类
    public:
        virtual void freeMovie() {
            std::cout << "watch free movie" << std::endl;
        }
        virtual void vipMovie() {
            std::cout << "watch vip movie" << std::endl;
        }
        virtual void ticketMovie() {
            std::cout << "watch ticket movie" << std::endl;
        }
};

class FreeVideoSiteProxy : public FixBugVideoSite { // 对委托类进行代理
    public:
        FreeVideoSiteProxy() {
            pVideo = new FixBugVideoSite();
        }
        ~FreeVideoSiteProxy() {
            delete pVideo;
        }

        virtual void freeMovie() {
            pVideo->freeMovie(); // 通过对象对象的 freeMovie , 来访问真正的委托类对象的 freeMovie
        }
        virtual void vipMovie() {
            std::cout << "free visiter can't access vip movie" << std::endl;
        }
        virtual void ticketMovie() {
            std::cout << "free visiter can't access ticket movie" << std::endl;
        }
    private:
        VideoSite* pVideo;
}; 

class VipVideoSiteProxy : public FixBugVideoSite { // 对委托类进行代理
    public:
        VipVideoSiteProxy() {
            pVideo = new FixBugVideoSite();
        }
        ~VipVideoSiteProxy() {
            delete pVideo;
        }

        virtual void freeMovie() {
            pVideo->freeMovie(); // 通过对象对象的 freeMovie , 来访问真正的委托类对象的 freeMovie
        }
        virtual void vipMovie() {
            pVideo->vipMovie();
        }
        virtual void ticketMovie() {
            std::cout << "vip visiter can't access ticket movie" << std::endl;
        }
    private:
        VideoSite* pVideo;
}; 
// 这里都是通用的 API 接口 , 使用的都是基类的指针或者引用 
void watchMovie(std::unique_ptr<VideoSite> ptr) { // 这里调用拷贝构造 , unique_ptr 取消左值引用 ,直接在这加引用或者调用用move
    ptr->freeMovie();
    ptr->vipMovie();
    ptr->ticketMovie();
}
int main() {
    // VideoSite* p1 = new FixBugVideoSite(); 
    // p1->freeMovie();
    // p1->vipMovie();
    // p1->ticketMovie();

    // 问题引入 : 因为代理类是对委托类的访问的限制 , 那么假如是游客用户,想要访问 vipVideo 就不行 , 就需要加判断 , 不灵活, 引入代理类
    std::unique_ptr<VideoSite> p1(new FreeVideoSiteProxy());
    std::unique_ptr<VideoSite> p2(new VipVideoSiteProxy());
    watchMovie(std::move(p1));
    watchMovie(std::move(p2));
    
    return 0;
}