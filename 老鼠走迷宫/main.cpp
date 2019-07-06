// �������Թ�.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
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


constexpr auto ROW = 5;		    // �Թ�����;
constexpr auto COL = 5;			// �Թ�����;
#define N ROW*COL			    // �Թ�����
constexpr auto DOWN = 1;
constexpr auto RIGHT = 2;
constexpr auto LEFT = 4;
constexpr auto UP = 8;
constexpr auto WALKED = 2;      // ����Ѿ��߹��ĸ���

vector <int> matrix_row;		// ·���ĺ�����
vector <int> matrix_column;		// ·����������
vector <int> matrix_direct;		// ·���ķ���

// ����һ���㣬������ȫ�ֵ��Թ���ںͳ���
struct point{
	int row;
	int col;
}src,des;

vector <point> path;			                    // �洢һ��·��
vector <vector<point>> allpath;                     // �洢����·��

int x_num = 1, y_num = 1;
int maze[ROW+2][COL+2];				                // �ڽӾ������ڴ洢�Թ��������һȦ��ǽ��
int visited[ROW + 2][COL + 2];		                // ��Ƿ��ʹ��ĵ�

int dir[4][2] = { {0,1},{1,0},{0,-1},{-1,0} };		// �洢�ĸ�����

point Point(int x, int y);		                    // ����һ����
void init();						                // ��ʼ���ڽӾ������ó�ֵΪ1��ȫ����ǽ��û��·��
void print_maze();					                // �����ڽӾ����ֵ��Ⱦ�Թ���0Ϊ·��1Ϊǽ��
void push_into_vec(int x, int y, int direct);		// ���Թ����С��С������������
void wall_breaker();				                // ǽ��·
void wall_builder();							    // ·��ǽ
int count();
void print(int m[ROW+2][COL+2]);					// ��ӡ�ڽӾ���
void create_maze();                                 // ��������Թ�������Prim�㷨��
bool save_maze();									// ʵ���Թ����̲���
bool get_maze();                                    // ʵ���Թ����̲���

void find_path(int x, int y);						// �����������Ѱ������·��
void print_meum();									// ��ӡ�˵�
bool cmp(vector<point> x, vector <point> y);		// �Ƚ�·�����̣��Ӷ��ҵ����·��

int main() {
    // ��ӡ�˵���ͬʱ���Ƴ�������
	print_meum();
	return 0;
}

// ����һ����
point Point(int x, int y)
{
	point p = {x, y};
	return p;
}

// �Թ���ʼ��
void init()
{
	// ���ڽӾ��������Ԫ����Ϊ1����ʱ�Թ������Ϊ�����ģ�û�п��ߵ�·��
	for (int i = 0; i <= ROW + 1; i++) {
		for (int j = 0; j <= COL + 1; j++) {
			maze[i][j] = 1;
		}
	}
	// ����һ���������飬���ڱ��ÿ�����Ƿ񱻾�������ʼ��Ϊ0����ʾ�����δ������
	for (int i = 0; i <= ROW + 1; i++) {
		for (int j = 0; j <= COL + 1; j++) {
			visited[i][j] = 0;
		}
	}
	// �����Թ�����ںͳ��ڣ�������ںͳ��ڱ��Ϊ3
	cout << "��������㣺";
	cin >> src.row >> src.col;
	maze[src.row][src.col] = 3;
	cout << "\n�������յ㣺";
	cin >> des.row >> des.col;
	maze[des.row][des.col] = 3;
	cout << endl;
}

// �Թ���Ⱦ���������ڽӾ�����ȾΪ�Թ���1Ϊǽ��0Ϊ·��
void print_maze()
{
	cout << "\n  0 1 2 3 4 5 6" << endl;              // ��ӡ������
	for (int i = 0; i <= ROW + 1; i++) {
		cout << i << " ";                             // ��ӡ������
		for (int j = 0; j <= COL + 1; j++) {
			// ��Ⱦ�Թ���ںͳ���
			if ((i == src.row && j == src.col) || (i == des.row && j == des.col)) {
				printf("%c%c", 0xa7, 0xb0);
			}
			// ��Ⱦ·����������
			else if (maze[i][j] == 0) {
				printf("  ");
			}
			// ����Ѿ��ҵ���·
			else if (maze[i][j] == WALKED || visited[i][j] == 1) {
				printf("%c%c", '\u002a',' ');
			}
			// ��Ⱦǽ��
			else {
				printf("%c%c", 0xa8, 0x80);
			}
		}
		printf("\n");
	}
}

void push_into_vec(int x, int y, int direct)
{
	//��һ�����ݴ�Ž�����vector������
	matrix_row.push_back(x);
	matrix_column.push_back(y);
	matrix_direct.push_back(direct);
}

// �Թ��༭����ǽ��Ϊ·
void wall_breaker()
{
	int x = -1, y = -1;
	do{
		cout << "\n������Ҫȥ����ǽ��λ�ã�����'-1 -1' �˳�������";
		cin >> x >> y;
		maze[x][y] = 0;
	}while (x != -1 && y != -1);
	cout << endl;
	print_maze();       // ������Ⱦ�Թ�
}

// �Թ��༭����·��Ϊǽ
void wall_builder()
{
	int x = -1, y = -1;
	do{
		cout << "\n������Ҫ�����ǽ��λ�ã�����'-1 -1' �˳�������";
		cin >> x >> y;
		maze[x][y] = 1;
	}while (x != -1 && y != -1);
	cout << endl;
	print_maze();       // ������Ⱦ�Թ�
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

// ��ӡ�ڽӾ��󣨵����ã�
void print(int m[ROW+2][COL+2])
{
	cout << "\n  0  1  2  3  4  5  6" << endl;      // ��ӡ������
	for (int i = 0; i <= ROW + 1; i++) {
		cout << i << " ";                           // ��ӡ������
		for (int j = 0; j < COL+2; j++) {
			cout << m[i][j] << "  ";
		}
		cout << endl;
	}
}


// �����Թ�
void create_maze()
{
	init();                                                                     // ��ʼ���ڽӾ���
	srand((unsigned)time(NULL));                                                // ���������
	count();
	while (matrix_row.size())
	{
		//��һ��ѹ������ǽ������ұߺ�������棩����ѭ��
		size_t num = matrix_row.size();
		int randnum = rand() % num;												//����0-num-1֮����������ͬʱҲ��vector����±�
		x_num = matrix_row[randnum];
		y_num = matrix_column[randnum];
		switch (matrix_direct[randnum]) {										//ѡ��һ��������к�����������ʼ�� �ڿ� Ŀ��� ����������ͬһֱ���� ����ƶ���Ŀ����λ��
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
		if (maze[x_num][y_num] == 1) {                              //Ŀ��������ǽ
			maze[matrix_row[randnum]][matrix_column[randnum]] = 0;  //��ͨǽ
			maze[x_num][y_num] = 0;                                 //��ͨĿ���
			count();                                                //�ٴμ��㵱ǰλ����Χ����ǽ�����������vector
		}
		matrix_row.erase(matrix_row.begin() + randnum); 			//ɾ�����ǽ(���ò��˵�ǽɾ�ˣ�ȷ���ܹ�����ѭ��)
		matrix_column.erase(matrix_column.begin() + randnum);
		matrix_direct.erase(matrix_direct.begin() + randnum);
	}
}



void find_path(int x, int y) {
    // ����ִ���ڣ���·������·������
	if (x == des.row && y == des.col) {
		point p1 = {x, y};
		path.push_back(p1);
		allpath.push_back(path);
		path.pop_back();
		return;
	}

    // �쳣�������
	if (maze[x][y] == 1) return;            // ���Ŀ�겻�ɴ����ֹ���β���
	if (visited[x][y] == 1) return;         // ���Ŀ���ѱ����ʣ�����ֹ���β���

    // ��Ŀ��ɴ���δ������ʱ����Ŀ�����·���������Ϊ�ѷ���
	point p2 = { x,y };
	path.push_back(p2);
	visited[x][y] = 1;
	// ��Ŀ������������������ĸ��������̽��
	for (int i = 0; i < 4; ++i) {
		int xx = x + dir[i][0], yy = y + dir[i][1];
		find_path(xx, yy);
	}

    // ����˲���˵��������������Ҫ����
	visited[x][y] = 0;
	path.pop_back();
	return;
}

// �Ƚ�·�������ҵ����·��
bool cmp(vector<point> x, vector <point> y)
{
	return x.size() < y.size();
}


void print_meum()
{
	cout << "�������Թ�С��Ϸ 1.0" << endl;
	int ch;
	cout << "\n1..........................................................................�����Թ�" << endl;
	cout << "\n2..........................................................................��ʾ�Թ��ײ㣨�ڽӾ���" << endl;
	cout << "\n3..........................................................................��ʾ�Թ�" << endl;
	cout << "\n4..........................................................................����ǽ��" << endl;
	cout << "\n5..........................................................................����ǽ��" << endl;
	cout << "\n6..........................................................................�Թ�����" << endl;
	cout << "\n7..........................................................................�ҳ�����·��������DFS��" << endl;
	cout << "\n8..........................................................................�ҳ����·��������7�Ľ����" << endl;
	cout << "\n9..........................................................................�Թ�����" << endl;
	cout << "\n0..........................................................................�˳�����" << endl;
	cout << "\n���������ѡ�� ";
	cin >> ch;
	cout << endl;
	while (ch != 0) {
		switch (ch){
		case 1:
			create_maze();
			cout << "�Թ��ѳɹ�����! " << endl;
			break;
		case 2:
			cout << "\nչʾ�Թ��ĵײ㣺" << endl;
			print(maze);
			break;
		case 3:
			cout << "չʾ�Թ����棺" << endl;
			print_maze();
			break;
		case 4:
			wall_breaker();
			break;
		case 5:
			wall_builder();
			break;
		case 6:
		    cout << "ִ���Թ����̲���\n";
		    if(save_maze()){
                cout << "�Թ��Ѿ��ɹ����̣�\n";
		    }
		    else{
                cout << "д���Թ�ʧ�ܣ�\n";
		    }
			break;
		case 7:
			find_path(src.row, src.col);
			// ·����Ϊ��˵����·����
			if (allpath.size() == 0) {
				printf("\nû�п���·��\n");
			}
			else {
				sort(allpath.begin(), allpath.end(), cmp);      // ��·�����е�·��������������
				printf("�������·����\n");
                // ��ʽ���������·��
				for (unsigned p = 0; p < allpath.size(); p++) {
					cout << "\n�� " << p + 1 << " ��·����" << endl;
					for (unsigned t = 0; t < allpath[p].size(); t++) {
						cout << "( " << allpath[p][t].row << ", " << allpath[p][t].col << " )" << endl;
					}
					cout << "\n���ȣ�" << allpath[p].size() << endl;
				}
			}
			cout << endl;
			break;
		case 8:
		    // ǰ�벿�ֲ���ͬ7��ֻ�Ǻ���ֻ�����һ��·�������·����
			find_path(src.row, src.col);
			if (allpath.size() == 0) {
				printf("û�п���·��\n");
			}
			else {
				sort(allpath.begin(), allpath.end(), cmp);
				printf("������·����\n");
				for (unsigned t = 0; t < allpath[0].size(); t++) {
					cout << "( " << allpath[0][t].row << ", " << allpath[0][t].col << " )" << endl;
				}
				cout << "\n���ȣ�" << allpath[0].size() << endl;
			}
                cout << endl;
				break;
        case 9:
            cout << "ִ���Թ����̲�����\n";
            get_maze();
            break;
		case 0:
			cout << "Bye!";
			system("pause");
			return;
		default:
			std::cout << "�������˲����ʵ�ѡ��, ����������" << endl;
			break;
		}
		cout << "\n���������ѡ�� ";
		cin >> ch;
		cout << endl;
	}
}

bool save_maze()
{
    ofstream out("my_maze.txt", ios::ate);
    if(out.fail())
    {
        cerr << "��my_maze.txt ʧ�ܣ�\n";
        return false;
    }
    else{
        out << "�ڽӾ���" << endl;;
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

// ���ļ��ж�ȡ�Թ�
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
