#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <time.h>
#include <sstream>
#include <fstream>

using std::cin;
using std::cout;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;
using std::map;
using std::sort;
using std::istringstream;
using std::fstream;

class Node{
public:
    char symbol;
    int index;
    bool ispattern = false;
    bool suffix_edge = false;
    int curr_branch_len = 0;
    int pos = -1;
    vector<int> subpatternspos;
    Node(char symbol,int index,bool ispattern,bool suffix_edge,int curr_branch_len,int pos,int subpatternpos)
    {
        this->symbol = symbol;
        this->index = index;
        this->ispattern = ispattern;
        this->suffix_edge = suffix_edge;
        this->curr_branch_len = curr_branch_len;
        this->pos = pos;
        this->subpatternspos.push_back(subpatternpos);
    }
    Node(char symbol,int index,bool ispattern,bool suffix_edge,int curr_branch_len,int pos,vector<int> &subpatternpos)
    {
        this->symbol = symbol;
        this->index = index;
        this->ispattern = ispattern;
        this->suffix_edge = suffix_edge;
        this->curr_branch_len = curr_branch_len;
        this->pos = pos;
        this->subpatternspos = subpatternpos;
    }
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

int split(vector<pair<string,int>> &collection, string pattern,char wildcard){
    int size = 0;
    istringstream stream(pattern);
    std::string substr;
    int number = 0;
    cout << "Разбиение на построки\n";
    while (std::getline(stream, substr, wildcard)) {
        if (substr.length() > 0) {
            cout << substr << "\n";
            collection.push_back(std::make_pair(substr, number));
            number += substr.length() + 1;
            size++;
        }
        else {
            number++;
        }
    }


    return size;
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

void constructTrie(vector<pair<string,int>> collection,vector<vector<Node>> &automation,vector<int> &prev_vertices){
    int branch_len = 0;
    int size = collection.size();
    int number_of_vertices = 0;
    int curr_vertex;
    bool flag;
    for(int i = 0;i < size;i++){
        curr_vertex = 0;
        branch_len = 1;
        for(int j = 0;j < collection[i].first.size();j++){
            flag = false;
            for(int k = 0;k < automation[curr_vertex].size();k++){
                if(automation[curr_vertex][k].symbol == collection[i].first[j]){
                    branch_len++;
                    cout << "Переход с символом " << collection[i].first[j] << " уже существует\n";
                    if(j == collection[i].first.size() - 1) {
                        automation[curr_vertex][k].ispattern = true;
                        automation[curr_vertex][k].pos = i + 1;
                        automation[curr_vertex][k].subpatternspos.push_back(collection[i].second);
                    }
                    //if(automation[curr_vertex][k].ispattern)
                      //  automation[curr_vertex][k].subpatternspos.push_back(collection[i].second);
                    curr_vertex = automation[curr_vertex][k].index;
                    flag = true;
                    break;
                }
            }
            if(flag)
                continue;
            number_of_vertices++;
            automation.emplace_back();
            if(j == collection[i].first.size() - 1)
                automation[curr_vertex].emplace_back(Node(collection[i].first[j],number_of_vertices,true, false,branch_len,i + 1,collection[i].second));
            else
                automation[curr_vertex].emplace_back(Node(collection[i].first[j],number_of_vertices, false, false,branch_len,-1));
            prev_vertices.emplace_back(curr_vertex);
            curr_vertex = number_of_vertices;
            branch_len++;
            cout << "Создали новую вершину с символом " << collection[i].first[j] << "\n";
        }
        cout << "Шаблон " << collection[i].first << " полностью добавлен\n";
    }
}

void addSuffixEdge(vector<vector<Node>> &automation, int curr_vertex, char symbol,int main_vertex,vector<int> &prev_vertices,vector<bool> &visited) {
    if(automation[curr_vertex].size() && automation[curr_vertex][automation[curr_vertex].size() - 1].suffix_edge)
        return;
    if(!curr_vertex || !prev_vertices[curr_vertex]){
        automation[curr_vertex].emplace_back(Node('$',0, false, true,0,-1));
        if(!curr_vertex)
            cout << "Установили суффикс-ссылку от корня в корень\n";
        else
            cout << "Установили суффикс-ссылку от " << getName(automation,prev_vertices,curr_vertex) << " в корень" << "\n";
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
                                                              true,automation[curr_vertex][i].curr_branch_len, automation[curr_vertex][i].pos,automation[curr_vertex][i].subpatternspos));

                    cout << "Установили суффикс ссылку из вершины " << getName(automation,prev_vertices,main_vertex) << " в вершину " << getName(automation,prev_vertices, automation[curr_vertex][i].index) << "\n";
                }
                else {
                    automation[main_vertex].emplace_back(Node('$', 0, false, true, 0, -1));
                    cout << "Установили суффикс ссылку из вершины " << getName(automation,prev_vertices,main_vertex) << " на корень " << "\n";
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

void findMatching(string &text,vector<vector<Node>> &automation,vector<int> &prev_vertices,int size_of_pattern,int pattern_len){
    cout << "Начинаем обход текста\n";
    vector<int> counter;
    counter.resize(text.size());
    int curr_vertex = 0;
    int mediator;
    char symbol;
    int new_vertex;
    int prev = 0;
    for(int i = 0;i < text.length();i++){
        symbol = text[i];
        next(automation, curr_vertex,symbol,new_vertex);
        curr_vertex = new_vertex;
        cout << "Текущий символ " << symbol << "\n";
        cout << "Текущая вершина: " << getName(automation,prev_vertices,curr_vertex) << "\n";
        mediator = curr_vertex;
        prev = prev_vertices[curr_vertex];
        while (automation[mediator][automation[mediator].size() - 1].index != mediator){
            for(int j = 0;j < automation[prev].size();j++)
                if(automation[prev][j].index == mediator) {
                    if (automation[prev][j].ispattern) {
                        cout << "найден терминальный символ\n";
                        cout << "увеличиваем счётчики\n";
                        for(int k = 0;k < automation[prev][j].subpatternspos.size();k++) {
                            if (i - automation[prev][j].curr_branch_len + 1 -
                                automation[prev][j].subpatternspos[k] < 0)
                                continue;
                            counter[i - automation[prev][j].curr_branch_len + 1 -
                                    automation[prev][j].subpatternspos[k]]++;

                            if(counter[i - automation[prev][j].curr_branch_len + 1 -
                                       automation[prev][j].subpatternspos[k]] == 6){
                            }
                        }
                    }
                    break;
                }
            prev = mediator;
            mediator = automation[mediator][automation[mediator].size() - 1].index;
        }
    }
    cout << "проверяем счётчики\n";
    vector<int> output;
    for(int i = 0;i <= counter.size() - pattern_len;i++)
        if(counter[i] == size_of_pattern) {
            output.push_back(i + 1);
            cout << "Нашли вхождение на позиции " << i + 1 << "\n";
        }

    for(int i = 0;i < output.size();i++)
        cout << output[i] << "\n";
}

int main() {
    string pattern;
    string text;
    int size = 0;
    char wildcard;
    cin >> text;
    cin >> pattern;
    cin >> wildcard;
    vector<pair<string,int>> collection;
    size = split(collection, pattern,wildcard);
    vector<vector<Node>> automation(1);
    vector<pair<int,int>> output;
    vector<int> prev_vertices;
    prev_vertices.push_back(0);
    constructTrie(collection,automation,prev_vertices);
    vector<bool> visited(automation.size());
    constructAutomation(automation,0,prev_vertices,visited);
    cout << "Автомат построен\n";
    cout << "Размер автомата\n" << automation.size() << "\n";
    findMatching(text,automation,prev_vertices,size,pattern.length());
}

