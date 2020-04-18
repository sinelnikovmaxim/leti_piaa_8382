#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <math.h>

using std::string;
using std::cout;
using std::cin;
using std::vector;
using std::map;

char getIndex(string &text,string &pattern,int index,char symbol){//функция для получения элемента "строки" по индексу
    if(index < pattern.length())//если элемента строки pattern
        return pattern[index];
    else
    if(index == pattern.length())//если нейтральный символ
        return symbol;
    else
    if(index < pattern.length() + text.length() + 1)//если элемента строки text
        return text[index - (pattern.length() + 1)];
}

void fillHashTable(map<char,bool> &hash_table,string &text){//заполнение хэш таблицы
    for(int i = 0;i < text.length();i++)
        hash_table[text[i]] = true;
}

char findSymbol(map<char,bool > &hash_table){//нахождение подходящего нейтрального символа
    int counter = 1;
    while (1){//смотрим все возможные символы
        if(!hash_table[counter]){
            return (char) counter;//когда нашли, возвращаем его
        } else
            counter++;
    }
}

void constructPrefixTable(string &text,string &pattern,vector<int> &prefix_array,char symbol){//построение таблицы со значениями префикс функции
    prefix_array[0] = 0;
    int longest_prefix;
    for(int i = 1;i < pattern.length() + text.length() + 1;i++){//проходим по каждмоу элементу строки, склеинной из шаблона, нейтрального элемента и текста
        longest_prefix = prefix_array[i - 1];
        while (1) {
            if (getIndex(text,pattern,i,symbol) == getIndex(text,pattern,longest_prefix,symbol)) {//если совпадает с элементом из префикса, увеличиваем значение на единицу
                prefix_array[i] = longest_prefix + 1;
                break;
            } else {
                if(!longest_prefix)//если не соответсвует никакому префику, переходим к следующему
                    break;
                longest_prefix = prefix_array[longest_prefix - 1];
                continue;
            }
        }
    }
}

void findMatching(string &text,string &pattern, vector<int> &prefix_array,int flow) {//функция для нахождения всех вхождений шаблона в текст
    unsigned long  start,end;
    vector<vector<int>> indeces(flow);
    bool flag = false;
    string curr = "";
    for(int k = 0;k < flow;k++) {//разделяем поиск по "потокам"
        cout << "Обработка " << k + 1 << " ого потока\n";
        start = pattern.length() + 1 + k * (ceil(text.length()) / flow);//выделяем начальную позицию для текущего потока
        end = pattern.length() + 1 + (k + 1) * (ceil(text.length()) / flow);//выделяем конечную позицию для текущего потока
        end = end < pattern.length() + text.length() + 1 ? end : pattern.length() + text.length() + 1;//проверка, не выходим ли за границы текста
        for (int i = start; i < end; i++) {
            curr = text.substr(i - pattern.length() - prefix_array[i], prefix_array[i]);
            cout << "Текущая последовательность: " << curr << "\n";
            if (prefix_array[i] == pattern.length()) {//проверяем значение префикс функциис размером шаблона
                cout << "Найденно новое вхождение с позиции " << i - 2 * pattern.length() << "\n";
                if (!flag)
                    flag = true;
                indeces[k].push_back(i - 2 * pattern.length());//добавляем в результирующий вектор
            }
        }
    }
    if(!flag)
        cout << -1;//выодим -1, если не встретили шаблон
    else{
        flag = false;
        for(int i = 0;i < flow;i++){//выводим все вхождения
            for(int j = 0;j < indeces[i].size();j++)
                if(flag)
                    cout << "," << indeces[i][j];
                else {
                    cout << indeces[i][j];
                    flag = true;
                }
        }
    }
}

int main() {
    string pattern;
    string text;
    int flow;
    char symbol;
    cout << "Введите шаблон: ";
    cin >> pattern;
    cout << "Введите текст: ";
    cin >> text;
    cout << "Введите количество потоков: ";
    cin >> flow;
    if(flow > text.size()) {//проверка на корректность количества потоков
        cout << "Потоков не может быть больше, чем длина текста!!!";
        return 0;
    }
    if(flow <= 0){
        cout << "Нельзя вводить отрицательные или нулевые потоки!!!";
        return 0;
    }
    vector<int> prefix_array(pattern.length() + text.length() + 1);
    map<char,bool> hash_table;
    fillHashTable(hash_table,text);
    symbol = findSymbol(hash_table);
    if(pattern.length() > text.length()) {//если размер шаблона больше размера текста, не обрабатываем
        cout << -1;
        return 0;
    }
    constructPrefixTable(text,pattern,prefix_array,symbol);//вызываем построение префикс таблицы
    findMatching(text,pattern,prefix_array,flow);//вызываем функцию для нахождения всех вхождений щаблона в текст
    return 0;
}