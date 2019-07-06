// 老鼠走迷宫.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <stack>
#include <vector>
#include <queue>
#include <random>
#include <time.h>
#include <algorithm>
#include <cassert>
#include <string>
using namespace std;


constexpr auto ROW = 5;		    // 迷宫行数;
constexpr auto COL = 5;			// 迷宫列数;
#define N ROW*COL			    // 迷宫格数
constexpr auto DOWN = 1;
constexpr auto RIGHT = 2;
constexpr auto LEFT = 4;
constexpr auto UP = 8;
constexpr auto WALKED = 2;      // 标记已经走过的格子

vector <int> matrix_row;		// 路径的横坐标
vector <int> matrix_column;		// 路径的纵坐标
vector <int> matrix_direct;		// 路径的方向

// 定义一个点，并创建全局的迷宫入口和出口
struct point{
	int row;
	int col;
}src,des;

vector <point> path;			                    // 存储一条路径
vector <vector<point>> allpath;                     // 存储所有路径

int x_num = 1, y_num = 1;
int maze[ROW+2][COL+2];				                // 邻接矩阵，用于存储迷宫（最外层一圈是墙）
int visited[ROW + 2][COL + 2];		                // 标记访问过的点

int dir[4][2] = { {0,1},{1,0},{0,-1},{-1,0} };		// 存储四个方向

point Point(int x, int y);		                    // 构造一个点
void init();						                // 初始化邻接矩阵，设置初值为1（全部是墙，没有路）
void print_maze();					                // 根据邻接矩阵的值渲染迷宫（0为路，1为墙）
void push_into_vec(int x, int y, int direct);		// 将迷宫的行、列、方向存入向量
void wall_breaker();				                // 墙改路
void wall_builder();							    // 路改墙
int count();
void print(int m[ROW+2][COL+2]);					// 打印邻接矩阵
void create_maze();                                 // 随机建造迷宫（基于Prim算法）
bool save_maze();									// 实现迷宫存盘操作
bool get_maze();                                    // 实现迷宫读盘操作

void find_path(int x, int y);						// 深度优先搜索寻找所有路径
void print_meum();									// 打印菜单
bool cmp(vector<point> x, vector <point> y);		// 比较路径长短，从而找到最短路径

int main() {
    // 打印菜单，同时控制程序流程
	print_meum();
	return 0;
}

// 构造一个点
point Point(int x, int y)
{
	point p = {x, y};
	return p;
}

// 迷宫初始化
void init()
{
	// 将邻接矩阵的所有元素置为1，此时迷宫各点均为孤立的（没有可走的路）
	for (int i = 0; i <= ROW + 1; i++) {
		for (int j = 0; j <= COL + 1; j++) {
			maze[i][j] = 1;
		}
	}
	// 创建一个辅助数组，用于标记每个点是否被经过，初始化为0，表示各点均未被经过
	for (int i = 0; i <= ROW + 1; i++) {
		for (int j = 0; j <= COL + 1; j++) {
			visited[i][j] = 0;
		}
	}
	// 设置迷宫的入口和出口，并将入口和出口标记为3
	cout << "请输入起点：";
	cin >> src.row >> src.col;
	maze[src.row][src.col] = 3;
	cout << "\n请输入终点：";
	cin >> des.row >> des.col;
	maze[des.row][des.col] = 3;
	cout << endl;
}

// 迷宫渲染函数，将邻接矩阵渲染为迷宫（1为墙，0为路）
void print_maze()
{
	cout << "\n  0 1 2 3 4 5 6" << endl;              // 打印横向标尺
	for (int i = 0; i <= ROW + 1; i++) {
		cout << i << " ";                             // 打印纵向标尺
		for (int j = 0; j <= COL + 1; j++) {
			// 渲染迷宫入口和出口
			if ((i == src.row && j == src.col) || (i == des.row && j == des.col)) {
				printf("%c%c", 0xa7, 0xb0);
			}
			// 渲染路（不作处理）
			else if (maze[i][j] == 0) {
				printf("  ");
			}
			// 标记已经找到的路
			else if (maze[i][j] == WALKED || visited[i][j] == 1) {
				printf("%c%c", '\u002a',' ');
			}
			// 渲染墙壁
			else {
				printf("%c%c", 0xa8, 0x80);
			}
		}
		printf("\n");
	}
}

void push_into_vec(int x, int y, int direct)
{
	//把一组数据存放进三个vector容器中
	matrix_row.push_back(x);
	matrix_column.push_back(y);
	matrix_direct.push_back(direct);
}

// 迷宫编辑：将墙变为路
void wall_breaker()
{
	int x = -1, y = -1;
	do{
		cout << "\n请输入要去除的墙的位置，输入'-1 -1' 退出函数：";
		cin >> x >> y;
		maze[x][y] = 0;
	}while (x != -1 && y != -1);
	cout << endl;
	print_maze();       // 重新渲染迷宫
}

// 迷宫编辑：将路变为墙
void wall_builder()
{
	int x = -1, y = -1;
	do{
		cout << "\n请输入要插入的墙的位置，输入'-1 -1' 退出函数：";
		cin >> x >> y;
		maze[x][y] = 1;
	}while (x != -1 && y != -1);
	cout << endl;
	print_maze();       // 重新渲染迷宫
}


int count()
{
	int cnt = 0;
	if (x_num + 1 <= ROW) {
		push_into_vec(x_num + 1, y_num, DOWN);
		cnt++;
	} //DOWN
	if (y_num + 1 <= COL) {
		push_into_vec(x_num, y_num + 1, RIGHT);
		cnt++;
	} //RIGHT
	if (x_num - 1 >= 1) {
		push_into_vec(x_num - 1, y_num, UP);
		cnt++;
	} //UP
	if (y_num - 1 >= 1) {
		push_into_vec(x_num, y_num - 1, LEFT);
		cnt++;
	} //LEFT
	return cnt;
}

// 打印邻接矩阵（调试用）
void print(int m[ROW+2][COL+2])
{
	cout << "\n  0  1  2  3  4  5  6" << endl;      // 打印横向标尺
	for (int i = 0; i <= ROW + 1; i++) {
		cout << i << " ";                           // 打印纵向标尺
		for (int j = 0; j < COL+2; j++) {
			cout << m[i][j] << "  ";
		}
		cout << endl;
	}
}


// 创建迷宫
void create_maze()
{
	init();                                                                     // 初始化邻接矩阵
	srand((unsigned)time(NULL));                                                // 随机数种子
	count();
	while (matrix_row.size())
	{
		//第一步压入两堵墙（起点右边和起点下面）进入循环
		size_t num = matrix_row.size();
		int randnum = rand() % num;												//生成0-num-1之间的随机数，同时也是vector里的下标
		x_num = matrix_row[randnum];
		y_num = matrix_column[randnum];
		switch (matrix_direct[randnum]) {										//选择一个方向进行后续操作，起始点 邻块 目标块 三块区域在同一直线上 随后移动到目标块的位置
            case DOWN: {
                x_num++;
                break;
            }
            case RIGHT: {
                y_num++;
                break;
            }
            case LEFT: {
                y_num--;
                break;
            }
            case UP: {
                x_num--;
                break;
            }
		}
		if (maze[x_num][y_num] == 1) {                              //目标块如果是墙
			maze[matrix_row[randnum]][matrix_column[randnum]] = 0;  //打通墙
			maze[x_num][y_num] = 0;                                 //打通目标块
			count();                                                //再次计算当前位置周围的邻墙个数并保存进vector
		}
		matrix_row.erase(matrix_row.begin() + randnum); 			//删除这堵墙(把用不了的墙删了，确保能够跳出循环)
		matrix_column.erase(matrix_column.begin() + randnum);
		matrix_direct.erase(matrix_direct.begin() + randnum);
	}
}



void find_path(int x, int y) {
    // 如果抵达出口，则将路径存入路径集中
	if (x == des.row && y == des.col) {
		point p1 = {x, y};
		path.push_back(p1);
		allpath.push_back(path);
		path.pop_back();
		return;
	}

    // 异常情况处理
	if (maze[x][y] == 1) return;            // 如果目标不可达，则终止本次操作
	if (visited[x][y] == 1) return;         // 如果目标已被访问，则终止本次操作

    // 当目标可达且未被访问时，将目标加入路径，并标记为已访问
	point p2 = { x,y };
	path.push_back(p2);
	visited[x][y] = 1;
	// 从目标出发，向右下左上四个方向进行探测
	for (int i = 0; i < 4; ++i) {
		int xx = x + dir[i][0], yy = y + dir[i][1];
		find_path(xx, yy);
	}

    // 到达此步，说明陷入困境，需要回溯
	visited[x][y] = 0;
	path.pop_back();
	return;
}

// 比较路径长度找到最短路径
bool cmp(vector<point> x, vector <point> y)
{
	return x.size() < y.size();
}


void print_meum()
{
	cout << "老鼠走迷宫小游戏 1.0" << endl;
	int ch;
	cout << "\n1..........................................................................生成迷宫" << endl;
	cout << "\n2..........................................................................显示迷宫底层（邻接矩阵）" << endl;
	cout << "\n3..........................................................................显示迷宫" << endl;
	cout << "\n4..........................................................................减少墙壁" << endl;
	cout << "\n5..........................................................................增加墙壁" << endl;
	cout << "\n6..........................................................................迷宫存盘" << endl;
	cout << "\n7..........................................................................找出所有路径（基于DFS）" << endl;
	cout << "\n8..........................................................................找出最短路径（基于7的结果）" << endl;
	cout << "\n9..........................................................................迷宫读盘" << endl;
	cout << "\n0..........................................................................退出程序" << endl;
	cout << "\n请输入你的选择： ";
	cin >> ch;
	cout << endl;
	while (ch != 0) {
		switch (ch){
		case 1:
			create_maze();
			cout << "迷宫已成功生成! " << endl;
			break;
		case 2:
			cout << "\n展示迷宫的底层：" << endl;
			print(maze);
			break;
		case 3:
			cout << "展示迷宫界面：" << endl;
			print_maze();
			break;
		case 4:
			wall_breaker();
			break;
		case 5:
			wall_builder();
			break;
		case 6:
		    cout << "执行迷宫存盘操作\n";
		    if(save_maze()){
                cout << "迷宫已经成功存盘！\n";
		    }
		    else{
                cout << "写入迷宫失败！\n";
		    }
			break;
		case 7:
			find_path(src.row, src.col);
			// 路径集为空说明无路可走
			if (allpath.size() == 0) {
				printf("\n没有可行路径\n");
			}
			else {
				sort(allpath.begin(), allpath.end(), cmp);      // 对路径集中的路径进行升序排序
				printf("输出所有路径：\n");
                // 格式化输出所有路径
				for (unsigned p = 0; p < allpath.size(); p++) {
					cout << "\n第 " << p + 1 << " 条路径：" << endl;
					for (unsigned t = 0; t < allpath[p].size(); t++) {
						cout << "( " << allpath[p][t].row << ", " << allpath[p][t].col << " )" << endl;
					}
					cout << "\n长度：" << allpath[p].size() << endl;
				}
			}
			cout << endl;
			break;
		case 8:
		    // 前半部分操作同7，只是后面只输出第一条路径（最短路径）
			find_path(src.row, src.col);
			if (allpath.size() == 0) {
				printf("没有可行路径\n");
			}
			else {
				sort(allpath.begin(), allpath.end(), cmp);
				printf("输出最短路径：\n");
				for (unsigned t = 0; t < allpath[0].size(); t++) {
					cout << "( " << allpath[0][t].row << ", " << allpath[0][t].col << " )" << endl;
				}
				cout << "\n长度：" << allpath[0].size() << endl;
			}
                cout << endl;
				break;
        case 9:
            cout << "执行迷宫读盘操作：\n";
            get_maze();
            break;
		case 0:
			cout << "Bye!";
			system("pause");
			return;
		default:
			std::cout << "您输入了不合适的选项, 请重新输入" << endl;
			break;
		}
		cout << "\n请输入你的选择： ";
		cin >> ch;
		cout << endl;
	}
}

bool save_maze()
{
    ofstream out("my_maze.txt", ios::ate);
    if(out.fail())
    {
        cerr << "打开my_maze.txt 失败！\n";
        return false;
    }
    else{
        out << "邻接矩阵" << endl;;
        for(int i = 0; i < ROW +2; i++){
            for(int j = 0; j < COL + 2; j++){
                out << maze[i][j] << "  ";
            }
            out << endl;
        }
    }
    out.close();
    return true;
}

// 从文件中读取迷宫
bool get_maze()
{
    ifstream in("my_maze.txt", ios::in);
    assert(in.is_open());
    string s;

    while(getline(in, s)){
        cout << s << endl;
    }
    in.close();
    return true;
}
