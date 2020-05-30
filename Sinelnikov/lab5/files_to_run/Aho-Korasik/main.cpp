#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <time.h>

using std::cin;
using std::cout;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;
using std::map;
using std::sort;

class Node{
public:
    char symbol;
    int index;
    bool ispattern = false;
    bool suffix_edge = false;
    int curr_branch_len = 0;
    int pos = -1;
    Node(char symbol,int index,bool ispattern,bool suffix_edge,int curr_branch_len,int pos)
    {
        this->symbol = symbol;
        this->index = index;
        this->ispattern = ispattern;
        this->suffix_edge = suffix_edge;
        this->curr_branch_len = curr_branch_len;
        this->pos = pos;
    }
    Node() = default;
};

void inputPatterns(vector<string> &patterns){
    for(int i = 0;i < patterns.size();i++)
        cin >> patterns[i];
}

string reverseString(string str){
    char temp;
    for(int i = 0;i < str.length() / 2;i++) {
        temp = str[i];
        str[i] = str[str.length() - 1 - i];
        str[str.length() - 1 - i] = temp;
    }

    return str;
}

string getName(vector<vector<Node>> &automation,vector<int> &prev_vertices,int curr_vertex){
    string name;
    int prev;
    while (curr_vertex){
        prev = prev_vertices[curr_vertex];
        for(int i = 0;i < automation[prev].size();i++){
            if(automation[prev][i].index == curr_vertex) {
                name += automation[prev][i].symbol;
                break;
            }
        }
        curr_vertex = prev;
    }
    return reverseString(name);
}

void constructTrie(vector<string> &patterns,vector<vector<Node>> &automation,vector<int> &prev_vertices){
    int branch_len = 0;
    int size = patterns.size();
    int number_of_vertices = 0;
    int curr_vertex;
    bool flag;
    for(int i = 0;i < size;i++){
        curr_vertex = 0;
        branch_len = 1;
        for(int j = 0;j < patterns[i].size();j++){
            flag = false;
            for(int k = 0;k < automation[curr_vertex].size();k++){
                if(automation[curr_vertex][k].symbol == patterns[i][j]){
                    branch_len++;
                    cout << "Переход с символом " << patterns[i][j] << " уже существует\n";
                    if(j == patterns[i].size() - 1) {
                        automation[curr_vertex][k].ispattern = true;
                        automation[curr_vertex][k].pos = i + 1;
                    }
                    curr_vertex = automation[curr_vertex][k].index;
                    flag = true;
                    break;
                }
            }
            if(flag)
                continue;
            number_of_vertices++;
            automation.emplace_back();
            if(j == patterns[i].size() - 1)
                automation[curr_vertex].emplace_back(Node(patterns[i][j],number_of_vertices,true, false,branch_len,i + 1));
            else
                automation[curr_vertex].emplace_back(Node(patterns[i][j],number_of_vertices, false, false,branch_len,-1));
            prev_vertices.emplace_back(curr_vertex);
            curr_vertex = number_of_vertices;
            branch_len++;
            cout << "Создали новую вершину с символом " << patterns[i][j] << "\n";
        }
        cout << "Шаблон " << patterns[i] << " полностью добавлен\n";
    }
}

void addSuffixEdge(vector<vector<Node>> &automation, int curr_vertex, char symbol,int main_vertex,vector<int> &prev_vertices,vector<bool> &visited) {
    if(automation[curr_vertex].size() && automation[curr_vertex][automation[curr_vertex].size() - 1].suffix_edge)
        return;
    if(!curr_vertex || !prev_vertices[curr_vertex]){
        automation[curr_vertex].emplace_back(Node('$',0, false, true,0,-1));
        if(!curr_vertex)
            cout << "Установили суффикс-ссылку от корня в корень\n";
        else{
            cout << "Установили суффикс-ссылку от " << getName(automation,prev_vertices,curr_vertex) << " в корень" << "\n";
        }
        return;
    }
    curr_vertex = prev_vertices[curr_vertex];
    char curr_symbol;
    while (curr_vertex){
        for(int i = 0;i < automation[prev_vertices[curr_vertex]].size();i++)
            if(automation[prev_vertices[curr_vertex]][i].index == curr_vertex){
                curr_symbol = automation[prev_vertices[curr_vertex]][i].symbol;
                break;
            }
        addSuffixEdge(automation,curr_vertex,curr_symbol,curr_vertex,prev_vertices,visited);
        curr_vertex = automation[curr_vertex][automation[curr_vertex].size() - 1].index;

        for(int i = 0;i < automation[curr_vertex].size();i++){
            if(automation[curr_vertex][i].symbol == symbol && !automation[curr_vertex][i].suffix_edge){
                if(automation[curr_vertex][i].index != main_vertex){
                    automation[main_vertex].emplace_back(Node(symbol,automation[curr_vertex][i].index, automation[curr_vertex][i].ispattern,
                            true,automation[curr_vertex][i].curr_branch_len, automation[curr_vertex][i].pos));

                    cout << "Установили суффикс ссылку из вершины " << getName(automation,prev_vertices,main_vertex) << " в вершину " << getName(automation,prev_vertices, automation[curr_vertex][i].index) << "\n";
                }
                else {
                    cout << "Установили суффикс ссылку из вершины " << getName(automation,prev_vertices,main_vertex) << " на корень " << "\n";
                    automation[main_vertex].emplace_back(Node('$', 0, false, true, 0, -1));
                }
                return;
            }
        }
    }
    cout << "Установили суффикс ссылку из вершины " << getName(automation,prev_vertices,main_vertex) << " на корень " << "\n";
    automation[main_vertex].emplace_back(Node('$',0, false, true,0,-1));
}

void constructAutomation(vector<vector<Node>> &automation,int curr_vertex,vector<int> &prev_vertices,vector<bool> &visited){
        char symbol;
        for(int i = 0;i < automation[prev_vertices[curr_vertex]].size();i++)
            if(automation[prev_vertices[curr_vertex]][i].index == curr_vertex){
                symbol = automation[prev_vertices[curr_vertex]][i].symbol;
                break;
            }
        addSuffixEdge(automation,curr_vertex, symbol,curr_vertex,prev_vertices,visited);
        for(int i = 0;i < automation[curr_vertex].size();i++)
            if(!automation[curr_vertex][i].suffix_edge)
                constructAutomation(automation,automation[curr_vertex][i].index,prev_vertices,visited);
}

void next(vector<vector<Node>> &automation, int curr_vertex,char symbol,int &new_vertex){
    for(int j = 0;j < automation[curr_vertex].size();j++)
        if(automation[curr_vertex][j].symbol == symbol && !automation[curr_vertex][j].suffix_edge){
            curr_vertex = automation[curr_vertex][j].index;
            new_vertex = curr_vertex;
            return;
        }

    if(!curr_vertex) {
        new_vertex = curr_vertex;
        return;
    }
    next(automation, automation[curr_vertex][automation[curr_vertex].size() - 1].index,symbol,new_vertex);
}

void findMatching(string &text,vector<vector<Node>> &automation,vector<pair<int,int>> &output,vector<int> &prev_vertices){
      cout << "Начинаем обход текста\n";
      int curr_vertex = 0;
      int mediator;
      char symbol;
      int new_vertex;
      int prev = 0;
      for(int i = 0;i < text.length();i++){
          symbol = text[i];
          cout << "Текущий символ " << symbol << "\n";
          next(automation, curr_vertex,symbol,new_vertex);
          curr_vertex = new_vertex;
          cout << "Текущая вершина: " << getName(automation,prev_vertices,curr_vertex) << "\n";
          mediator = curr_vertex;
          prev = prev_vertices[curr_vertex];
          while (mediator){
              for(int j = 0;j < automation[prev].size();j++)
                  if(automation[prev][j].index == mediator) {
                      if (automation[prev][j].ispattern) {
                          output.emplace_back(make_pair(i - automation[prev][j].curr_branch_len + 2,
                                  automation[prev][j].pos));

                          cout << "Нашли вхождение шаблона с номером " << automation[prev][j].pos << " " << "с индекса " << i - automation[prev][j].curr_branch_len + 2 << "\n";
                      }
                      break;
                  }
              prev = mediator;
              mediator = automation[mediator][automation[mediator].size() - 1].index;
          }
      }
}

void printOutput(vector<pair<int,int>> &output,vector<string> &patterns) {
    vector<int> mediator(output.size());
    sort(output.begin(), output.end());
    for(int i = 0;i < output.size();i++)
        cout << output[i].first << " " << output[i].second << "\n";

    cout << "Список образцов,имеющих пересечение с другими образцами:\n";
    int counter = 0;
    bool flag ;
    for (int i = 0; i < output.size(); i++) {
        flag = false;
        if (mediator[i]) {
            flag = true;
        }
        counter = i + 1;
        while (1) {
            if (counter == output.size())
                break;
            if (output[counter].first == output[i].first) {
                mediator[counter]++;
                flag = true;
            }
            else if (output[i].first + patterns[output[i].second - 1].length() >= output[counter].first) {
                mediator[counter]++;
                flag = true;
            }
            else
                break;

            counter++;
        }
        if(flag)
            cout << patterns[output[i].second - 1] << " на позиции " << output[i].first << "\n";
    }
}

int main() {
    int number_of_patterns;
    string curr_string;
    string text;
    cin >> text;
    cin >> number_of_patterns;
    vector<string> patterns(number_of_patterns);
    vector<vector<Node>> automation(1);
    vector<pair<int,int>> output;
    inputPatterns(patterns);
    vector<int> prev_vertices;
    prev_vertices.push_back(0);
    constructTrie(patterns,automation,prev_vertices);
    vector<bool> visited(automation.size());
    constructAutomation(automation,0,prev_vertices,visited);
    cout << "Автомат построен" << "\n";
    cout << "Количество вершин в автомате: " << automation.size() << "\n";
    findMatching(text,automation,output,prev_vertices);
    printOutput(output,patterns);
    return 0;
}