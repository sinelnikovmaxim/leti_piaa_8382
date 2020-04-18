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
    else
        return text[index - (2 * pattern.length() + 1)];
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
    for(int i = 1;i < pattern.length() + 2 * text.length() + 1;i++){//проходим по каждмоу элементу строки, склеинной из шаблона, нейтрального элемента и текста
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

void findMatching(string &text,string &pattern, vector<int> &prefix_array,int flow) {
    bool flag = false;
    int min = pattern.length() + 1;
    unsigned long start, end;
    string curr;
    for (int k = 0; k < flow; k++) {//разделяем поиск по "потокам"
        cout << "Обработка " << k + 1 << " ого потока\n";
        start = pattern.length() + 1 + k * (ceil(2 * text.length()) / flow);//выделяем начальную позицию для текущего потока
        end = pattern.length() + 1 + (k + 1) * (ceil(2 * text.length()) / flow);//выделяем конечную позицию для текущего потока
        end = end < pattern.length() + 2 * text.length() + 1 ? end : pattern.length() + 2 * text.length() + 1;//проверка, не выходим ли за границы текста
        for (int i = start; i < end; i++) {
            if((i - pattern.length() - 1 - prefix_array[i]) % text.length() + prefix_array[i] < text.length())
                curr = text.substr((i -  pattern.length()  - prefix_array[i]) % text.length(), prefix_array[i]);
            else{
                curr = text.substr(((i - pattern.length() - prefix_array[i]) % text.length()), text.length());
                curr += text.substr(0,((i - pattern.length() - 1) - text.length() + 1) % text.length());
            }
            cout << "Текущая последовательность: " << curr << "\n";
            if (prefix_array[i] == pattern.length()) {//проверка значения префикс функции с длинной шаблона
                cout << "Нашли циклический сдвиг с индексом " << pattern.length() - (i - 2 * pattern.length()) << "\n";
                if (pattern.length() - (i - 2 * (pattern.length())) < min) {
                    min = pattern.length() - (i - 2 * (pattern.length()));
                    cout << "На данный момент он минимальный\n";
                }
                if (!flag)
                    flag = true;
            }
        }
    }
    if (!flag)
        cout << -1;//если не нашли циклического сдвига, выводим -1
    else
        cout << min;//если нашли. выводим минимальный индекс
}

int main() {
    string pattern;
    string text;
    int flow;
    char symbol;
    cout << "Введите строку 1: ";
    cin >> pattern;
    cout << "Введите строку 2: ";
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
    if(pattern == text){
        cout << 0;
        return 0;
    }
    vector<int> prefix_array(pattern.length() + 2 * text.length() + 1);
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