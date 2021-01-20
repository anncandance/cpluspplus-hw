#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<vector>
#include<string>
#include<tuple>


using namespace std;

class Observer{
public:
    ~Observer(){};
    virtual void set_pos(int n) = 0;
};

// позиция - номер клетки в (0,0), (5,2)
// полоса прокрутки находится в прямоугольнике 1х3 с определяющими координатами (3, 0), (5, 2),
// "кликабельные" координаты посередине полосы
class Scroll: public Observer{
    int position;
    int current_bar_pos;
public:
    vector<vector<char>> printbuf;
    Scroll(){
        current_bar_pos = 0;
        printbuf = {{' ', ' ', ' '},{' ', ' ', ' '},{' ', ' ', ' '} };
    };
    void set_pos(int n){
        position = n;
        int cur_y = position / 6;
        int cur_x = position % 6;
        current_bar_pos = cur_y;
    }
    void mouse_click(int x, int y){
        int cur_y = position / 6;
        int n = x + 6*y;
        if (x == 4)
        {
            current_bar_pos = y;
        }
        else{
            current_bar_pos += y - current_bar_pos;
        }
    }

    void print(){
        vector<char> raw1 = {'|', ' ', '|'};
        vector<char> raw2 = {'|', ' ', '|'};
        vector<char> raw3 = {'|', ' ', '|'};
        printbuf[0] = raw1;
        printbuf[1] = raw2;
        printbuf[2] = raw3;
        for(int i =0; i < 3; ++i){
            if(current_bar_pos  == i)
                printbuf[i][1] = '@';
        }
    }
};

// предполагаем вывод текста в квадрате 3х3
// с координатами левого нижнего угла (0, 0), правого верхнего (2, 2)
class Text: public Observer {
    string text;
    int current_buf;
    int position;
public:
    vector<vector<char>> printbuf;
    Text() {
        text = "";
        current_buf = 0;
        position = 0;

    };

    Text(string text_) : text(text_) {
        current_buf = 0;
        position = 0;
        printbuf = {{' ', ' ', ' '},{' ', ' ', ' '},{' ', ' ', ' '} };
    };

    void add_symbol(char ch){
        text.push_back(ch);
    }

    void set_pos(int n) {
        int old_x = position % 6;
        int old_y = position / 6;
        position = n;
        int cur_y = position / 6;
        int cur_x = position % 6;
        int k = 0;
        if (cur_x < 3)
            if (old_x == 4) {old_x = 0; old_y = 0;}
                k = cur_x + 3*cur_y - (old_x + old_y*3);
        if(cur_x == 4)
            k = 3*(cur_y - old_y);
        if(k > 0)
        {
            int i=0;
            while(i < k){
                for(int j=0; j<3; ++j){
                    if(text[current_buf] == '\n'){
                        while(j<3){
                            ++i;
                            ++j;
                        }
                        ++current_buf;
                    }
                    else{
                        ++i;
                        ++current_buf;
                    }
                    if(i == k) break;
                }
            }
        }
        if(k < 0)
        {
            int i=0;
            while(i > k && current_buf > 0){
                for(int j=0; j<3; ++j){
                    if(text[current_buf] == '\n'){
                        while(j<3){
                            --i;
                            ++j;
                        }
                        --current_buf;
                    }
                    else{
                        --i;
                        --current_buf;
                    }
                    if(i == k || current_buf == 0) break;
                }
            }
        }
    };

    void mouse_click(int x, int y) {
        int n = 6 * y + x;
        set_pos(n);

    };

    void print() {
        int print_pos = current_buf;
        int printed = 0;
        for (int i = 0; i < 3; ++i) {
            vector<char> col(3);
            for (int j = 0; j < 3; ++j) {
                if (print_pos < text.size()) {
                    if(text[print_pos] == '\n')
                    {
                        while(j<3)
                        {
                            col[j] = ' ';
                            ++j;
                        }
                        ++print_pos;
                        ++printed;
                    }
                    else
                    {
                        col[j] = text[print_pos];
                        ++print_pos;
                        ++printed;
                    }
                }
                else
                {col[j] = ' ';
                }
            }
            printbuf[i] = col;
        }
        current_buf =  print_pos - printed;
    }
};


class Subject{
public:
    virtual void notify(int i) = 0;
    virtual ~Subject(){};
};


class TextEditor : public Subject{
    int position;
public:
    vector<tuple<Text*, Scroll*>> observers;
    TextEditor(Text* text_, Scroll* text_scroll_){
        tuple<Text*, Scroll*> observe;
        position = 0;
        get<0>(observe) = text_;
        get<1>(observe) = text_scroll_;
        observers.push_back(observe);
    };
    ~TextEditor()=default;
    void add(Text* text_, Scroll* scroll_){
        tuple<Text*, Scroll*> observe;
        get<0>(observe) = text_;
        get<1>(observe) = scroll_;
        observers.push_back(observe);
    };

    void notify(int i){
        get<0>(observers[i])->set_pos(position);
        get<1>(observers[i])->set_pos(position);
    };
    // клик в прямоугольнике (0,5), (5, 2) при работе с i-м текстом
    void mouse_click(int x, int y, int i){
        position = 6*y + x;
        get<0>(observers[i])->mouse_click(x, y);
        get<1>(observers[i])->mouse_click(x, y);
    };
    // второй параметр - номер текста в таблице
    void exec_command(char c, int k){
        int cur_y = position / 6;
        int cur_x = position % 6;
        switch(c){
            case 'l':
                position = (cur_x + 1 + cur_y * 6);
                break;

            case 'h':
                position = (cur_x - 1 + cur_y * 6);
                break;

            case 'j':
                position = (cur_x + (cur_y - 1) * 6);
                break;

            case 'k':
                position = (cur_x + (cur_y + 1) * 6);
                break;
            // символ вставки вводится с клавиатуры
            case 'i':
                char ch;
                cin>>ch;
                get<0>(observers[k]) -> add_symbol(ch);
                break;
            default:
                break;
        }
        notify(k);
        if(position < 0){
            position =0;
            notify(k);
        }
    };
    void print(int k){
        get<0>(observers[k]) -> print();
        get<1>(observers[k]) -> print();
        for(int i = 0; i < 3; ++i){
            for(int j = 0; j < 3; ++j){
                cout << get<0>(observers[k]) -> printbuf[i][j];
            }
            cout << ' ';
            for(int j = 0; j < 3; ++j){
                cout << get<1>(observers[k]) -> printbuf[i][j];
            }
            cout << endl;
        }
        cout<<"______"<<endl;
    };
};


int main()
{
    Text text1("make\namerica");
    Scroll text_scroll;
    TextEditor edit1(&text1, &text_scroll);
    edit1.print(0);
    edit1.exec_command('l', 0); // вправо
    edit1.print(0);
    edit1.mouse_click(4, 1, 0); // клик на середину полосы
    edit1.print(0);
    edit1.mouse_click(0, 1, 0); // клик на "a"
    edit1.print(0);
    edit1.exec_command('l', 0); // вправо
    edit1.print(0);
    edit1.mouse_click(4, 0, 0); // начало полосы
    edit1.print(0);
    edit1.exec_command('k', 0); // вниз
    edit1.print(0);
    edit1.exec_command('h', 0); // влево
    edit1.print(0);
    edit1.exec_command('k', 0); // вниз
    edit1.print(0);
    edit1.exec_command('j', 0); // вверх
    edit1.print(0);
    edit1.mouse_click(4, 2, 0); // конец полосы
    edit1.print(0);
    edit1.exec_command('i', 0); // вставить символ
    edit1.print(0);
//    cin.ignore();

    return 0;
}

