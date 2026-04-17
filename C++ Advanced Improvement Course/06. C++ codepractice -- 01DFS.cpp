#include "00. head file.hpp"

/*
深度遍历搜索迷宫路径 软件运行要求如下 

请输入迷宫的行列数(例如 : 10  10) : 5 5
请输入迷宫的路径信息(0 表示可以走 , 1 表示不能走) :
0 0 0 1 1 
1 0 0 0 1
1 1 1 1 1 
1 1 0 0 1
1 1 1 0 0

迷宫路径搜索中...
>>>>如果没有路径 , 请直接输出<<<<
不存在一条迷宫路径!
>>>>如果有路径 , 直接输出<<<<
* * * 1 1 
1 0 * 0 1
1 1 * 1 1
1 1 * * 1
1 1 1 * * 
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

// 迷宫的类
class Maze {
    public:
        // 构造函数 , 初始化迷宫 , 根据传入的行列生成二维数组
        Maze(int row = 0 , int col = 0) 
            : _row(row)
            , _col(col)
        {
            _pMaze = new Node*[_row];
            for(int i = 0 ; i < _row ; ++ i) { // 在行的基础上动态开辟列
                _pMaze[i] = new Node[_col];
            }
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
        // 深度搜索迷宫路径 
        void searchMazePath() {
            if(_pMaze[0][0]._val == 1) {
                return;
            }
            // 入栈
            _stack.push(_pMaze[0][0]);
            // 取栈顶元素 , 判断 
            while(!_stack.empty()) {
                Node top = _stack.top();
                int x = top._x; // 取出栈顶元素的坐标 
                int y = top._y;

                // 已经找到右下角出口得迷宫路径 
                if(x == _row - 1 && y == _col - 1) {
                    return;
                }
                // 如果右方向可以走
                if(_pMaze[x][y]._state[RIGHT] == YES) {
                    _pMaze[x][y]._state[RIGHT] = NO; // 把当前节点的右方向设置为不能走 
                    _pMaze[x][y + 1]._state[LEFT] = NO; // 当当前节点的右节点的左方向设置为不能走 , 这样做的意义是防止来回跳 
                    _stack.push(_pMaze[x][y + 1]);
                    continue;
                }

                if(_pMaze[x][y]._state[DOWN] == YES) {
                    _pMaze[x][y]._state[DOWN] = NO; // 把当前节点的下方向设置为不能走 
                    _pMaze[x + 1][y]._state[UP] = NO; // 当当前节点的下节点的上方向设置为不能走 , 这样做的意义是防止来回跳 
                    _stack.push(_pMaze[x + 1][y]);
                    continue;
                }

                if(_pMaze[x][y]._state[LEFT] == YES) {
                    _pMaze[x][y]._state[LEFT] = NO; // 把当前节点的左方向设置为不能走 
                    _pMaze[x][y - 1]._state[RIGHT] = NO; // 当当前节点的左节点的右方向设置为不能走 , 这样做的意义是防止来回跳 
                    _stack.push(_pMaze[x][y - 1]);
                    continue;
                }

                if(_pMaze[x][y]._state[UP] == YES) {
                    _pMaze[x][y]._state[UP] = NO; // 把当前节点的上方向设置为不能走 
                    _pMaze[x - 1][y]._state[DOWN] = NO; // 当当前节点的上节点的下方向设置为不能走 , 这样做的意义是防止来回跳 
                    _stack.push(_pMaze[x - 1][y]);
                    continue;
                }

                _stack.pop();
            }


        }
            
        // 打印输出信息 
        void showMazePath() {
            if(_stack.empty()) {
                std::cout << "there is no maze path here" << std::endl;
            }
            
            while(!_stack.empty()) {
                Node top = _stack.top();
                _pMaze[top._x][top._y]._val = '*';
                _stack.pop();
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
    private:    
        // 定义迷宫节点的信息  -- 位置 x , y , 节点的值 , 记录节点四个方向的状态
        struct Node {
            int _x;
            int _y;
            int _val; // 节点的值
            int _state[WAY_NUM]; // 记录节点四个方向的状态 
        };
        // 动态生成迷宫路径 -- 二级指针 
        Node** _pMaze;
        // 行列数
        int _row;
        int _col;
        // 栈结构 , 辅助深度搜索迷宫路径
        std::stack<Node> _stack;
};
int main() {
    // 提示输入迷宫路径 
    std::cout << "please insert maze's rows and cols(eg : 10  10) = 5 5 : ";
    int col , row;
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