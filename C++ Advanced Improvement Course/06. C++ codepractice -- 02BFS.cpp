#include "00. head file.hpp"

/*

深度优先遍历 -- 栈 / 递归 
广度优先遍历 -- 层层扩张的方式 -- 队列 
广度优先遍历
寻找迷宫最短路径
0 0 1 1 1 1
1 0 0 0 0 1 
1 0 1 1 0 1 
1 0 0 0 0 1 
1 0 1 1 1 1 
1 0 0 0 0 0 

* * 1 1 1 1 
1 * 0 0 0 1
1 * 1 1 0 1
1 * 0 0 0 1 
1 * 1 1 1 1 
1 * * * * * 
*/

// 四个方向的常量代表 -- 右下左上
const int RIGHT = 0;
const int DOWN = 1;
const int LEFT = 2;
const int UP = 3;

// 迷宫每一个节点的方向个数 
const int WAY_NUM = 4;

// 定义节点行走状态 
const int YES = 4;
const int NO = 5;

// 迷宫 
class Maze {
    public: 
        Maze(int row = 3 , int col = 3) 
            : _row(row) 
            , _col(col)
        {
            _pMaze = new Node*[_row];
            for(int i = 0 ; i < _row ; ++ i) {
                _pMaze[i] = new Node[_col];
            }

            _pPath.resize(_row * _col); // 给一维数组留够足够空间 , 便于二维数组的映射 
        }

        // 初始化迷宫路径节点信息 -- 节点四个方向默认的初始化 
        void initMaze(int x , int y , int val) {
            _pMaze[x][y]._x = x;
            _pMaze[x][y]._y = y;
            _pMaze[x][y]._val = val;

            // 当前节点的四个方向默认的初始化 
            for(int i = 0 ; i < WAY_NUM ; ++ i) {
                _pMaze[x][y]._state[i] = NO;
            }
        }

        // 初始化迷宫 0 节点四个方向的行走状态信息 , 判断哪里能走 , 哪里不能走
        void setNodeState() {
            for(int i = 0 ; i < _row ; ++ i) {
                for(int j = 0 ; j < _col ; ++ j) {
                    if(_pMaze[i][j]._val == 1) {
                        continue;
                    }

                    // 判断右方向能走到哪 
                    if(j < _col - 1 && _pMaze[i][j + 1]._val == 0) {
                        _pMaze[i][j]._state[RIGHT] = YES;
                    }
                    // 判断下方能走到哪
                    if(i < _row - 1 && _pMaze[i + 1][j]._val == 0) {
                        _pMaze[i][j]._state[DOWN] = YES;
                    }
                    // 判断左方能走到哪
                    if(j > 0 && _pMaze[i][j - 1]._val == 0) {
                        _pMaze[i][j]._state[LEFT] = YES;
                    }
                    // 判断上方能走到哪
                    if(i > 0 && _pMaze[i - 1][j]._val == 0) {
                        _pMaze[i][j]._state[UP] = YES;
                    }
                }
            }
        }

        
        // 广度搜索迷宫路径 
        void searchMazePath() {
            if(_pMaze[0][0]._val == 1) {  // 没入口 , 直接返回
                return; 
            } 
            _queue.push(_pMaze[0][0]);

            while(!_queue.empty()) {
                Node _top = _queue.front();
                int x = _top._x;
                int y = _top._y;

                if(_pMaze[x][y]._state[RIGHT] == YES) {
                    _pMaze[x][y]._state[RIGHT] = NO;
                    _pMaze[x][y + 1]._state[LEFT] = NO;
                    // 在辅助数组中记录一下行走信息
                    _pPath[x * _row + y + 1] = _pMaze[x][y]; // +1 的目的是 : 是把当前元素的右边的元素的当前的位置保存了下来 
                    _queue.push(_pMaze[x][y + 1]);
                    if(check(_pMaze[x][y + 1])) {
                        return;
                    }
                }

                if(_pMaze[x][y]._state[DOWN] == YES) {
                    _pMaze[x][y]._state[DOWN] = NO;
                    _pMaze[x + 1][y]._state[UP] = NO;
                    // 在辅助数组中记录一下行走信息
                    _pPath[(x + 1) * _row + y] = _pMaze[x][y]; // +1 的目的是 : 是把当前元素的右边的元素的当前的位置保存了下来 
                    _queue.push(_pMaze[x + 1][y]);
                    if(check(_pMaze[x + 1][y])) {
                        return;
                    }
                }

                if(_pMaze[x][y]._state[LEFT] == YES) {
                    _pMaze[x][y]._state[LEFT] = NO;
                    _pMaze[x][y - 1]._state[RIGHT] = NO;
                    // 在辅助数组中记录一下行走信息
                    _pPath[x * _row + y - 1] = _pMaze[x][y]; // +1 的目的是 : 是把当前元素的右边的元素的当前的位置保存了下来 
                    _queue.push(_pMaze[x][y - 1]);
                    if(check(_pMaze[x][y - 1])) {
                        return;
                    }
                }

                if(_pMaze[x][y]._state[UP] == YES) {
                    _pMaze[x][y]._state[UP] = NO;
                    _pMaze[x - 1][y]._state[DOWN] = NO;
                    // 在辅助数组中记录一下行走信息
                    _pPath[(x - 1) * _row + y] = _pMaze[x][y]; // +1 的目的是 : 是把当前元素的右边的元素的当前的位置保存了下来 
                    _queue.push(_pMaze[x - 1][y]);
                    if(check(_pMaze[x - 1][y])) {
                        return;
                    }
                }

                _queue.pop();
            }

        }

        // 打印输出 
        void showMazePath() {
            if(_queue.empty()) {
                std::cout << "there is no maze path here" << std::endl;
            }
            else {
                int x = _row - 1;
                int y = _col - 1;
                for( ; ; ) { // 对一维数组进行回溯寻找路径 
                    _pMaze[x][y]._val = '*';
                    if(x == 0 && y == 0) { // 找到首入口了
                        break;
                    }
                    Node node = _pPath[x * _row + y];
                    x = node._x;
                    y = node._y;
                }

                for(int i = 0 ; i < _row ; ++ i) {
                    for(int j = 0 ; j < _col ; ++ j) {
                        if(_pMaze[i][j]._val == '*') {
                            std::cout << "* ";
                        }
                        else {
                            std::cout << _pMaze[i][j]._val << " ";
                        }
                    }
                    std::cout << std::endl;
                }
            }
        }

    private: 
        // 节点信息 
        struct Node {
            int _x;
            int _y;
            int _val; // 节点的值
            int _state[WAY_NUM]; // 记录节点四个方向的状态 
        };

        Node** _pMaze;
        // 定义行列 
        int _row;
        int _col;

        // 定义队列结构 
        std::queue<Node> _queue; // 存储节点 
        std::vector<Node> _pPath; // 辅助数组 , 用来保存路径信息 

        bool check(Node tmp) {
            if(tmp._x == _row - 1 && tmp._y == _col - 1) {
                return true;
            }
            return false;
        }
};
int main() {
    // 提示输入迷宫路径 
    std::cout << "please insert maze's rows and cols(eg : 10  10) = 5 5 : ";
    int row , col;
    std::cin >> row >> col;

    // 创建迷宫对象
    Maze maze(row , col);

     // 输入迷宫具体路径信息 , 初始化迷宫节点的基本信息 
    std::cout << "please insert maze's basic information(0 : can pass , 1 : can't pass) : ";
    int data;
    for(int i = 0 ; i < row ; ++ i) {
        for(int j = 0 ; j < col ; ++ j) {
            std::cin >> data;
            maze.initMaze(i , j , data);
        }
    }

    // 开始设置所有节点的四个方向的状态 
    maze.setNodeState();

    // 开始进行深度搜索迷宫路径
    maze.searchMazePath();

    // 打印输出信息
    maze.showMazePath();

    return 0;
}