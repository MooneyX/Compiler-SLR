#include <bits/stdc++.h>
using namespace std;

//用string表示右部
//由于需要记录右部的编号， 故将所有产生式的右部按顺序统一存储
//下标就是其编号
//则非终结符就是表示其右部下标的整数的vector

vector<string> Right;
vector<int> right2Left;//右部编号对应的非终结符

vector<int> nonT[30];

vector<char> symbol;

struct Project {
    int leftId;//左部 
    int rightId;//右部的ID 
    int pointPos;//点的位置，值为0表示的是后面一个是右部的第0位
    Project(int L, int R, int P) {
        leftId = L;
        rightId = R;
        pointPos = P;
    }
    void Display() {
        cout << char(leftId + 'A') << "->";
        for(int j = 0; j <= Right[rightId].size(); ++j) {
            if(j == pointPos) {
                cout << '#';
            }
            cout << Right[rightId][j];
        }
        cout << "\n";
    }

    bool friend operator < (Project x, Project y) {
    	if(x.leftId != y.leftId) {
    		return x.leftId < y.leftId;
		}
		if(x.rightId != y.rightId) {
			return x.rightId < y.rightId;
		}
		return x.pointPos < y.pointPos;
	}
    bool friend operator == (Project x, Project y) {
        return x.leftId == y.leftId && x.rightId == y.rightId && x.pointPos == y.pointPos;
    }

};

typedef set<Project> ProSet;

//项集需要用set<Project>表示
//一个节点包含一个项集和出边的信息
struct State {
	ProSet ps;//project set 节点里面的项集
	int nex[300];//nex['a']表示当前状态接收符号a之后到达的状态 
    vector<int> reduce;//可以归约的产生式编号

    State() {
        memset(nex, -1, sizeof(nex));
    }

	State(ProSet p) {
        memset(nex, -1, sizeof(nex));
		ps = p;
	}

    void Display() {
        for(auto i : ps) {
            i.Display();
        }
    }

    void getReduce() {
        for(auto i : ps) {
            if(i.pointPos == Right[i.rightId].length()) {
                reduce.push_back(i.rightId);
            }
        }
    }
}; 
vector<State> state;

void parse_prod(string s) {
    int n = s.length();
    if(s[0] < 'A' || s[0] > 'Z' || s[1] != '-' || s[2] != '>') {
        puts("产生式不合法!");
        return;
    }
    symbol.push_back(s[0]);
    int id = s[0] - 'A';
    Right.push_back(s.substr(3, n - 3));
    right2Left.push_back(s[0]);
    nonT[id].push_back(Right.size() - 1);
    for(int i = 3; i < s.size(); ++i) {
        symbol.push_back(s[i]);
    }
}


ProSet Closure(ProSet now) {
    queue<Project> workList;
    for(auto i : now) {
        workList.push(i);
    }
    while(!workList.empty()) {
        int x = workList.front().leftId;
        int y = workList.front().rightId;
        int z = workList.front().pointPos;
        workList.pop();
        cout << "???" << char(x + 'A') << " " << y << " " << z << " " << Right[y][z] << "???\n";
        if(Right[y][z] >= 'A' && Right[y][z] <= 'Z') {
            int id = Right[y][z] - 'A';
            for(auto j : nonT[id]) {
            	bool t = 1;
            	for(auto k : now) {
            		if(k.leftId == id && k.rightId == j && k.pointPos == 0) {
            			t = 0;
            			break;
					}
				}
				if(t) {
                    cout << char(id + 'A') << "->" << Right[j] << "!!!\n";
                	workList.push(Project(id, j, 0));
                    now.insert(Project(id, j, 0));
                }
            }
        }
    }
    return now;
}

ProSet Move(ProSet u, int w) {
    ProSet res;
    for(auto i : u) {
        if(Right[i.rightId][i.pointPos] == w) {
            res.insert(Project(i.leftId, i.rightId, i.pointPos + 1));
        }
    }
    return res;
}

bool inState(ProSet x) {
    for(auto i : state) {
        if(i.ps == x) {
            return 1;
        }
    }
    return 0;
}

int getStateId(ProSet x) {
    for(int i = 0; i < state.size(); ++i) {
        if(state[i].ps == x) {
            return i;
        }
    }
    return -1;
}

void output(bool &flag, string s) {
    if(flag) {
        cout << '/';
    }
    cout << s;
    flag = 1;
}

bool Letter(char x) {
    return x >= 'A' && x <= 'Z';
}

bool cmp(char x, char y) {
    if(x == '$') {
        return Letter(y);
    }
    if(y == '$') {
        return !Letter(x);
    }
    if(Letter(x)) {
        if(Letter(y)) {
            return x < y;
        }
        else {
            return 0;
        }
    }
    if(Letter(y)) {
        return 1;
    }
    return x < y;

}

int stateStack[510];
int stateTop;
char symbolStack[510];
int symbolTop;

int main() {
	string s; 
    while(cin >> (s)) {
        if(s == "!") {
            break;
        }
        parse_prod(s);
    }
    // cout << "input ok\n";
    sort(symbol.begin(), symbol.end(), cmp);
    symbol.erase(unique(symbol.begin(), symbol.end()), symbol.end());

    cout << "unique ok\n";
    // ProSet t;
    // t.insert(Project('S' - 'A', 0, 0));
    // t = Closure(t);
    // for(auto i : t) {
    //     i.Display();
    // }
    ProSet temp;
    temp.insert(Project('S' - 'A', 0, 0));
    State start = Closure(ProSet(temp));
    cout << "start ok\n";
    int cnt = 1;
    state.push_back(start);
    for(int i = 0; i < state.size(); ++i) {
        // cout << "u:\n";
        // state[i].Display();
        for(auto j : symbol) {
            ProSet v = Closure(Move(state[i].ps, j));
            if(v.size() == 0) {
                continue;
            }
            if(j == '$') {
                state[i].nex[j] = -2;//不将移进'$'后的状态加入， 而是做一个特殊标记
                continue;
            }
            int id = getStateId(v);
            // cout << "v:\n";
            // State(v).Display();
            // cout << id << "!!!\n";
            if(id == -1) {
                state.push_back(State(v));
                id = cnt++;
            }
            state[i].nex[j] = id;
            // cout << (char)i << u.nex[i] << "\n";
        }
    }

    for(int i = 0; i < state.size(); ++i) {
        cout << "state[" << i << "]:\n";
        state[i].Display();
        for(auto j : symbol) {
            if(state[i].nex[j] != -1) {
                cout << "\t-" << j << '>' << state[i].nex[j] << "\n";
            }
        }
    }
    cout << '\n';

    cout << '\t';
    for(auto i : symbol) {
        cout << i << '\t';
    }
    cout << '\n';
    for(int i = 0; i < state.size(); ++i) {
        state[i].getReduce();
        cout << i << '\t';
        for(auto j : symbol) {
            // cout << '?' << j << '?';
            if(!Letter(j)) {
                bool flag = 0;
                if(state[i].nex[j] == -2) {
                    cout << "accept";
                    continue;
                }
                if(state[i].nex[j] != -1) {
                    output(flag, "s" + to_string(state[i].nex[j]));
                }
                for(auto k : state[i].reduce) {
                    output(flag, "r" + to_string(k));
                }
            }
            else {
                if(state[i].nex[j] != -1) {
                    cout << state[i].nex[j];
                }
            }
            cout << '\t';
        }
        cout << '\n';
    }

    //开始分析
    stateStack[++stateTop] = 0;
    symbolStack[++symbolTop] = '$';
    cout << "请输入记号流\n";
    string tokens;
    cin >> tokens;
    tokens += '$';
    int pos = 0;
    while(1) {
        // cout << "pos = " << pos << "\n";
        char t = tokens[pos];
        // cout << "t = " << t << "\n";
        State u = state[stateStack[stateTop]];
        // for(int i = 1; i <= stateTop; ++i) {
        //     cout << stateStack[i] << " ";
        // }
        // cout << '\n';
        // for(int i = 1; i <= symbolTop; ++i) {
        //     cout << symbolStack[i] << " ";
        // }
        // cout << '\n';
        // cout << "u = status[" << stateStack[stateTop] << "], t = " << t << ", nex = " << u.nex[t] << "\n";
        if(u.reduce.size()) {
            int rightId = u.reduce[0];
            // cout << "rightId = " << rightId << " size = " << Right[rightId].size() << "???\n";
            symbolTop -= Right[rightId].size();
            stateTop -= Right[rightId].size();
            // cout << "stacks : \n";
            // for(int i = 1; i <= stateTop; ++i) {
            //     cout << stateStack[i] << " ";
            // }
            // cout << '\n';
            // for(int i = 1; i <= symbolTop; ++i) {
            //     cout << symbolStack[i] << " ";
            // }
            // cout << "\n";
            // cout << "left = " << (char)right2Left[rightId] << '\n';
            symbolStack[++symbolTop] = right2Left[rightId];
            // cout << "stateTop = " << stateTop << "stateStac"
            stateStack[++stateTop] = state[stateStack[stateTop]].nex[right2Left[rightId]];
            // cout << '\n';
        }
        else if(u.nex[t] == -2) {
            cout << "accept!\n";
            return 0;
        }
        else if(u.nex[t] != -1) {//移进
            // puts("shift");
            symbolStack[++symbolTop] = t;
            stateStack[++stateTop] = u.nex[t];
            pos++;
        }
        else  {
            cout << "error!\n";
            return 0;
        }
    }
    cout << "error!\n";
    return 0;
} 

/*
S->E$
E->E+T
E->T
T->T*F
T->F
F->n
*/
/*
S->A$
A->xxT
T->y
*/
