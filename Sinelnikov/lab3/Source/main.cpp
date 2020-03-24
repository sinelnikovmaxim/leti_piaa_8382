#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>
#include <stack>
#include <string>

using std::cin;
using std::cout;
using std::vector;
using std::map;
using std::sort;
using std::queue;
using std::pair;
using std::make_pair;
using std::priority_queue;
using std::stack;
using std::string;
using std::greater;

struct inputElement{
    string vertex1;
    string vertex2;
    int weight;
};

struct Edge{
    int capacity;
    int flow = 0;
    bool visited = false;
};


bool sortbyalf(const inputElement &a, const inputElement &b){//сортировака вершин для вывода в алфавитном порядке
    if(a.vertex1 != b.vertex1)
        return (a.vertex1 < b.vertex1);
    return (a.vertex2 < b.vertex2);
}

void formAdjacencList(vector<map<int,vector<Edge>>> &nodes,vector<map<int,vector<Edge>>> &reversed_nodes,map<string,int> &convert_to_int,vector<inputElement> &input_sequence){//формируем список смежности
    int vertex,dest;
    int cap;
    Edge edge;
    for(int i = 0;i < input_sequence.size();i++){//проходим по всем заданным рёбрам
        vertex = convert_to_int[input_sequence[i].vertex1];//вершину1 перевод из буквенного обозначения в числовое
        dest = convert_to_int[input_sequence[i].vertex2];//вершину2 перевод из буквенного обозначения в числовое
        cap  = input_sequence[i].weight;
        edge.capacity = cap;
        edge.visited = false;
        edge.flow = 0;
       //добавляе  вершину2 в список вершины1
        nodes[vertex][dest].push_back(edge);
        edge.flow = edge.capacity;
        edge.capacity = 0;
        edge.visited = true;
        reversed_nodes[dest][vertex].push_back(edge);
    }
}

void printCurrentWay(map<int,string> &convert_to_char,vector<pair<pair<int,int>,int>> &prev,int curr_vertex,bool flag,int source){//выводим текущий маршрут
    stack<int> way;
    int stack_size;
    int i = curr_vertex;
    if(flag)
        cout << "Найденный путь: \n";
    else
        cout << "Один из текущих маршрутов: \n";
    while (prev[i].first.first != -1){
        way.push(i);
        i = prev[i].first.first;
    }
    way.push(source);
    stack_size = way.size();
    for(int i = 0;i < stack_size;i++){//печатаем найденный путь
        cout << convert_to_char[way.top()] << " ";
        way.pop();
    }
    cout << "\n";
}

void formOutput(vector<map<int,vector<Edge>>> &nodes,map<int,string> &convert_to_string){
    cout << "Потоки по всем рёбрам: \n";
    vector<inputElement> output;
    inputElement element;
    for(int i = 0;i < nodes.size();i++){
        for (auto it = nodes[i].begin(); it != nodes[i].end(); ++it){//каждое ребро формируем как элемент структуры
            for(int j = 0;j < nodes[i][it->first].size();j++) {
                element.vertex1 = convert_to_string[i];
                element.vertex2 = convert_to_string[it->first];
                element.weight = nodes[i][it->first][j].flow;
                output.push_back(element);
            }
        }
    }
    sort(output.begin(),output.end(),sortbyalf);
    for(int i = 0;i < output.size();i++)//выводим поток через каждое ребро
        cout << output[i].vertex1 << " " << output[i].vertex2 << " " << output[i].weight << "\n";
}

unsigned int computeMaxFlow(vector<map<int,vector<Edge>>> &nodes){//вычисление значение максимального потока через какое-либо ребро
    unsigned int max = 0;
    for (int i = 0; i < nodes.size() ; ++i) {
        for (auto it = nodes[i].begin(); it != nodes[i].end(); ++it) {
            for(int j = 0;j < nodes[i][it->first].size();j++)
                if (nodes[i][it->first][j].capacity > max)
                    max = nodes[i][it->first][j].capacity;
        }
    }
    return max;
}

void input(vector<map<int,vector<Edge>>> &nodes,vector<map<int,vector<Edge>>> &reversed_nodes,map<int,string> &convert_to_string,map<string,int> &convert_to_int,int number_of_edges){//ввод входных данных
    vector<inputElement> input_sequnece;//вектор рёбер графа
    inputElement elem;
    string vertex1,vertex2;
    vertex1 = ' ';
    int capacity;
    int counter = 0;
    int i = 0;
    while (i < number_of_edges){//цикл для ввода рёбер графа
        cin >> vertex1;
        cin >> vertex2;
        cin >> capacity;
        elem.vertex1 = vertex1;
        elem.vertex2 = vertex2;
        elem.weight = capacity;
        input_sequnece.push_back(elem);
        if(convert_to_int.find(vertex1) == convert_to_int.end()) {//если вершина ещё не встречалась, записываем её в map convert_to_int.find
            convert_to_int[vertex1] = counter;
            convert_to_string[counter] = vertex1;
            counter++;
        }
        if(convert_to_int.find(vertex2) == convert_to_int.end()) {
            convert_to_int[vertex2] = counter;
            convert_to_string[counter] = vertex2;
            counter++;
        }
        i++;
    }
    nodes.resize(counter);//задаеём размер векттора
    reversed_nodes.resize(counter);//задаём размер вектора
    formAdjacencList(nodes,reversed_nodes,convert_to_int,input_sequnece);//вызываем функцию для формирования списка смежности
}

void zeroInitialization(vector<bool> &visited){//инициализируем вершины как ещё не посещённые
    for(int i = 0;i < visited.size();i++)
        visited[i] = false;
}

void emptyQueue(priority_queue<string,vector<string>,greater<string>> &neigbours){//опустошаем кучу
    while (!neigbours.empty())
        neigbours.pop();
}

int computeMinCapacity(vector<map<int,vector<Edge>>> &nodes,vector<map<int,vector<Edge>>> &reversed_nodes,vector<pair<pair<int,int>,int>> &prev,int sink, unsigned int max_possible_flow){//вычисляем минималбную пропускную способность
    int min,i;
    min = max_possible_flow;
    i = sink;
    while (1){//проходим найденный путь от стока к истоку
        if(prev[i].first.first == -1)//выходим, когда дошли до стока
            break;
        if((prev[i].second && nodes[prev[i].first.first][i][prev[i].first.second].capacity < min) || (!prev[i].second && reversed_nodes[prev[i].first.first][i][prev[i].first.second].capacity < min))
            if(prev[i].second)
                min = nodes[prev[i].first.first][i][prev[i].first.second].capacity;
            else
                min = reversed_nodes[prev[i].first.first][i][prev[i].first.second].capacity;
        i = prev[i].first.first;//переходим к следующей вершине на пути
    }
    return min;//возвращаем мин пропускную способность
}

void changeFlow(vector<map<int,vector<Edge>>> &nodes,vector<map<int,vector<Edge>>> &reversed_nodes, vector<pair<pair<int,int>,int>> &prev,int sink,int min){//меняем поток для рёбер на найденном пути
    int i;
    i = sink;
    while (1){
        if(prev[i].first.first == -1)//выходим, когда дошли до стока
            break;
        if(prev[i].second) {// обрабатываем случай, когда прошли по обычному ребру
            nodes[prev[i].first.first][i][prev[i].first.second].flow += min;
            nodes[prev[i].first.first][i][prev[i].first.second].capacity -= min;
            reversed_nodes[i][prev[i].first.first][prev[i].first.second].flow -= min;
            reversed_nodes[i][prev[i].first.first][prev[i].first.second].capacity += min;
            if(!nodes[prev[i].first.first][i][prev[i].first.second].capacity)
                nodes[prev[i].first.first][i][prev[i].first.second].visited = true;
            if(reversed_nodes[i][prev[i].first.first][prev[i].first.second].capacity)
                reversed_nodes[i][prev[i].first.first][prev[i].first.second].visited = false;
        } else{//обрабатываем случай, когда по обратному ребру
            reversed_nodes[prev[i].first.first][i][prev[i].first.second].flow += min;
            reversed_nodes[prev[i].first.first][i][prev[i].first.second].capacity -= min;
            nodes[i][prev[i].first.first][prev[i].first.second].flow -= min;
            nodes[i][prev[i].first.first][prev[i].first.second].capacity += min;
            if(!reversed_nodes[prev[i].first.first][i][prev[i].first.second].capacity)
                reversed_nodes[prev[i].first.first][i][prev[i].first.second].visited = true;
            if(nodes[i][prev[i].first.first][prev[i].first.second].capacity)
                nodes[i][prev[i].first.first][prev[i].first.second].visited = false;
        }
        i = prev[i].first.first;//переходим к следующей вершине на пути
    }
}

void foldFurkenson(vector<map<int,vector<Edge>>> &nodes,vector<map<int,vector<Edge>>> &reversed_nodes, map<int,string> &convert_to_string,map<string,int> &convert_to_int,int source,int sink){
    unsigned int max_possible_flow = computeMaxFlow(nodes);
    unsigned int max_flow = 0;
    int min_capacity;
    bool check;
    priority_queue<string,vector<string>, greater<string>> neigbours;
    vector<pair<pair<int,int>,int>> prev(nodes.size());
    vector<bool> visited(nodes.size());
    unsigned long size;
    int curr_vertex;
    prev[source] = make_pair(make_pair(-1,0),0);
    while (1) {//пока можно найти путь
        cout << "Ищем очередной путь: \n";
        neigbours.push(convert_to_string[source]);
        cout << "Положили в очередь вершину " << convert_to_string[source] << " - исток\n";
        check = false;
        zeroInitialization(visited);
        visited[source] = true;
        while (!neigbours.empty()) {
            size = neigbours.size();
            for (int i = 0; i < size; ++i) {
                curr_vertex = convert_to_int[neigbours.top()];
                cout << "Достали из очереди вершину " << convert_to_string[curr_vertex] << "\n";
                neigbours.pop();
                for (auto it = nodes[curr_vertex].begin(); it != nodes[curr_vertex].end(); ++it){//обрабатываем смежные вершины с текущей по исходному графу
                    for(int j = 0;j < nodes[curr_vertex][it->first].size();j++) {
                        if (!visited[it->first] && !nodes[curr_vertex][it->first][j].visited) {//проверка на то, можно ли пройти по ребру и посещена ли уже вершина
                            neigbours.push(convert_to_string[it->first]);
                            cout << "Положили в очередь вершину " << convert_to_string[it->first] << "\n";
                            prev[it->first] = make_pair(make_pair(curr_vertex,j), 1);
                            visited[it->first] = true;
                            if (it->first == sink) {//если сток, заканчиваем поиск пути
                                cout << "Дошли до стока\n";
                                printCurrentWay(convert_to_string, prev, sink, true, source);
                                check = true;
                                break;
                            } else
                                printCurrentWay(convert_to_string, prev, it->first, false, source);
                        }
                    }
                }
                if (check)
                    break;
                for (auto it = reversed_nodes[curr_vertex].begin(); it != reversed_nodes[curr_vertex].end(); ++it){//обрабатываем смежные вершины с текущей по обратному графу
                    for(int j = 0;j < reversed_nodes[curr_vertex][it->first].size();j++) {
                        if (!visited[it->first] && !reversed_nodes[curr_vertex][it->first][j].visited) {//проверка на то, можно ли пройти по ребру и посещена ли уже вершина
                            neigbours.push(convert_to_string[it->first]);
                            cout << "Положили в очередь вершину " << convert_to_string[it->first] << "\n";
                            prev[it->first] = make_pair(make_pair(curr_vertex,j), 0);
                            visited[it->first] = true;
                            if (it->first == sink) {//если сток, заканчиваем поиск пути
                                cout << "Дошли до стока\n";
                                printCurrentWay(convert_to_string, prev, sink, true, source);
                                check = true;
                                break;
                            } else
                                printCurrentWay(convert_to_string, prev, it->first, false, source);
                        }
                    }
                }
                if (check)
                    break;
            }
            if (check)
                break;
        }
        if(neigbours.empty())
            break;
        min_capacity = computeMinCapacity(nodes,reversed_nodes,prev,sink,max_possible_flow);//вычисляем минимальную пропускную способность
        changeFlow(nodes,reversed_nodes,prev,sink,min_capacity);//меняем потоки по рёбрам в найденном пути
        max_flow += min_capacity;//увеличиваем максимальный поток
        cout << "Текущий возможный поток через сеть; " << max_flow << "\n";
        emptyQueue(neigbours);
    }
    cout << "Больше путей не найти\n";
    cout << "Максимальный поток: " << max_flow << "\n";
    formOutput(nodes,convert_to_string);//формируем выход
}

int main() {
    string v1,v2;
    int number_of_edges;
    int source,sink;
    vector<map<int,vector<Edge>>> nodes;
    vector<map<int,vector<Edge>>> reversed_nodes;
    map<int,string> convert_to_string;
    map<string,int> convert_to_int;
    cout << "Введите количество рёбер: ";
    cin >> number_of_edges;
    cout << "Введите исток и сток: ";
    cin >> v1  >> v2;
    if(!number_of_edges)
        cout << "Максимальный поток: " << 0;
    else {
        cout << "Вводите рёбра: ";
        input(nodes, reversed_nodes, convert_to_string, convert_to_int, number_of_edges);
        source = convert_to_int[v1];
        sink = convert_to_int[v2];
        foldFurkenson(nodes, reversed_nodes, convert_to_string, convert_to_int, source, sink);
    }
    return 0;
}
