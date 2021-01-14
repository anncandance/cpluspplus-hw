#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<type_traits>
#include<iostream>
#include<vector>
#include<typeinfo>
#include<cstring>
#include<climits>
#include<typeinfo>
#include<string>
#include<cstdio>



using namespace std;

class Box {
public:
    int x, y;
    int width() { return x; }   
    int height() { return y; }
    virtual void layout() = 0;
    virtual Box *copy() = 0;
    virtual void print(vector<vector<char>> &v, int a, int b) = 0;
    virtual ~Box(){};
};

class CharBox: public Box {

    char c;
public:
    CharBox(char c_) : c(c_) {x = 1; y = 1;};

    void layout() {
        x = 1; y = 1;
    };

    Box *copy() {
        return new CharBox(*this);
    };

    void print(vector<vector<char>> &v, int a, int b) {
        v[a][b] = c;
    };
    ~CharBox() = default;
};


// ячейка таблицы
struct Cell{
    Box* box;
    int x;
    int y;
    Cell():box(nullptr){x = 0, y = 0;};
    Cell(Box* box_): box(box_){x = 0; y = 0;};
    Cell(Box* box_, int x_, int y_): box(box_), x(x_), y(y_){};
};

class TableBox: public Box{
public:
    vector<vector <Cell*>> table;
    int raws1;
    int cols1;

    TableBox(int cols_, int raws_):raws1(cols_), cols1(raws_){

        vector<vector<Cell*>> table_(cols1);
        for(int i = 0; i < cols1; ++i)
        {
            vector<Cell*> tmp(raws1);
            for(int j = 0; j < raws1; ++j)
            {
                tmp[j] = new Cell;
            }
            table_[i] = tmp;
        }
        table = table_;
    }
    void add(Box* box_){
        for(int i=0; i < raws1; ++i){
            for(int j = 0; j < cols1; ++j){
                if(table[j][i] -> box == nullptr) {
                    Box* copy_box = box_->copy();
                    table[j][i] = new Cell(copy_box);
                    return;
                }
            }
        }
    }
    void layout(){
        // вычисляем размеры вложенных прямоугольников
        for(int i = 0; i < cols1; ++i){
            for(int j = 0; j < raws1; ++j){
                if(table[i][j] -> box)
                    table[i][j] -> box -> layout();
            }
        }
        // для каждого столбца вычисляем максимальную ширину прямоугольника
        for(int i = 0; i < raws1 -1; ++i)
        {
            int max_wid_col = 0;
            for(int j = 0; j < cols1; ++j)
            {
                int width_box = 0;
                if(table[j][i] -> box)
                    width_box = table[j][i] -> box -> width();
                if(max_wid_col < width_box)
                    max_wid_col = width_box;
            }
            // координата по x следующего столбца = координата предыдущего + максимальная ширина предыдущего
            for(int j = 0; j < cols1; ++j)
            {
                table[j][i + 1] -> x = table[j][i] -> x + max_wid_col;
            }
        }
        // аналогично по строкам для высоты
        for(int i = 0; i < cols1 - 1; ++i)
        {
            int max_heigth_raw = 0;
            for(int j = 0; j < raws1; ++j)
            {
                int heigth_box = 0;
                if(table[i][j] -> box)
                    heigth_box = table[i][j] -> box -> height();
                if(max_heigth_raw < heigth_box)
                    max_heigth_raw = heigth_box;
            }
            for(int j = 0; j < raws1; ++j)
            {
                table[i + 1][j] -> y = table[i][j] -> y + max_heigth_raw;
            }
        }
        // суммарный размер таблицы
        // ширина последнего столбца
        int max_last_width = 0;
        for(int j=0; j<cols1; ++j)
        {
            if(table[j][raws1-1] -> box)
            {
            if(table[j][raws1-1] -> box -> width() > max_last_width)
                max_last_width = table[j][raws1-1] -> box -> width();
            }
        }
        x = table[0][raws1-1] -> x + max_last_width;
        // высота последней строки
        int max_last_height = 0;
        for(int i=0; i<raws1; ++i)
        {
            if(table[cols1-1][i] -> box)
            {
            if(table[cols1-1][i] -> box -> height() > max_last_height)
                max_last_height = table[cols1-1][i] -> box -> height();
            }
        }
        y = table[cols1-1][0] -> y + max_last_height;


    }
    void print(vector<vector<char>> &v, int a, int b){
        for(int i = 0; i < cols1; ++i){
            for(int j = 0; j < raws1; ++j){
                if(table[i][j] -> box)
                    table[i][j] -> box -> print(v, a + table[i][j] -> x, b + table[i][j] -> y);
            }
        }
    }
    TableBox(const TableBox& tab){
        x = tab.x;
        y = tab.y;
        raws1 = tab.raws1;
        cols1 = tab.cols1;
        vector<vector<Cell*>> table_(cols1);
        for(int i=0; i< cols1; ++i)
        {
            vector<Cell*> tmp(raws1);
            for(int j=0; j<raws1; ++j)
            {
                if(tab.table[i][j]->box) {
                    Box *copy_box = tab.table[i][j]->box->copy();
                    tmp[j] = new Cell(copy_box, tab.table[i][j]->x, tab.table[i][j]->y);
                }
                else tmp[j] = new Cell;
            }
            table_[i] = tmp;
        }
        table = table_;

    }
    Box* copy(){
        return new TableBox(*this);
    }
    ~TableBox(){
        for(int i=0; i<cols1; ++i){
            for(int j=0; j < raws1; ++j){
                delete table[i][j];
            }
        }
    }
};

class FrameBox:public Box{
public:
    vector<Box*> boxes;
    vector<vector<int>> coords;
    FrameBox(){boxes = {};}
    FrameBox(Box *box){boxes = {}; boxes.push_back(box->copy());}
    void add(Box *box){
        boxes.push_back(box->copy());
    }
    void layout(){
        x = 0;
        y = 0;
        if(!boxes.empty()){
            for (int i = 0; i < boxes.size(); ++i){
                boxes[i]->layout();
                vector<int> tmp = {x, y};
                coords.push_back(tmp);
                x += boxes[i]->width();
                y += boxes[i]->height();
            }

        }
        x += 2;
        y += 2;
    }
    FrameBox(const FrameBox &frame){
        x = frame.x;
        y = frame.y;
        boxes = {};
        coords = frame.coords;
        if(!frame.boxes.empty())
        {
            for(int i=0; i<frame.boxes.size(); ++i)
            {
                boxes.push_back(frame.boxes[i] -> copy());
            }
        }
    }
    Box* copy(){
        return new FrameBox(*this);
    }
    ~FrameBox(){
        if(!boxes.empty()){
            for(int i=0; i<boxes.size(); ++i){
                delete boxes[i];
            }
        }
    }
    void print(vector<vector<char>> &v, int a, int b){
        v[a][b] = '+';
        v[a][b + width() - 1] = '+';
        v[a + height() - 1][b] = '+';
        v[a + height() - 1][b + width() - 1] = '+';
        for(int i = 1; i < width() - 1; ++i)
        {
            v[a][b + i] = '-';
            v[a + height() - 1][b + i] = '-';
        }
        for(int i = 1; i < height() - 1; ++i)
        {
            v[a + i][b] = '|';
            v[a + i][b + width() - 1] = '|';
        }
        for(int i=0; i< boxes.size(); ++i){
            boxes[i]->print(v, a + coords[i][0] + 1, b + coords[i][1] + 1);
        }
    }
};

class ScriptBox: public Box{
public:
    Box* upper;
    Box* lower;
    ScriptBox(Box* upper_, Box* lower_): upper(upper_->copy()), lower(lower_->copy()){};
    void layout(){
        upper->layout();
        lower->layout();
        x = max(upper->width(), lower->width());
        y = upper->height() + lower->height();
    }
    void print(vector<vector<char>> &v, int a, int b){
        upper->print(v, a, b);
        lower->print(v, a + upper->height(), b);
    }
    ScriptBox(const ScriptBox &box){
        upper = box.upper->copy();
        lower = box.lower->copy();
        x = box.x;
        y =box.y;
    }
    Box* copy(){
        return new ScriptBox(*this);
    }
    ~ScriptBox(){
        delete upper;
        delete lower;
    }

};


int main()
{
    CharBox cb1('a');
    CharBox cb2('b');
    CharBox cb3('c');
    CharBox cb4('d');
    TableBox tab(2, 2);
    tab.add(&cb1); tab.add(&cb2); tab.add(&cb3); tab.add(&cb4);
    TableBox tab2(1, 5);
    CharBox cb5('f'), cb6('g'), cb7('h'), cb8('i'), cb9('j'), cb10('e'), cb11('k'), cb12('l');
    tab2.add(&cb5); tab2.add(&cb6); tab2.add(&cb7); tab2.add(&cb8); tab2.add(&cb9);
    TableBox tab3(1, 5);
    tab3.add(&cb1); tab3.add(&cb2); tab3.add(&cb3); tab3.add(&cb4); tab3.add(&cb10);
    TableBox tab4(2, 7);
    tab4.add(&cb1); tab4.add(&cb2); tab4.add(&cb3); tab4.add(&cb4); tab4.add(&cb10);
    tab4.add(&cb5); tab4.add(&cb6); tab4.add(&cb7); tab4.add(&cb8); tab4.add(&cb9);
    tab4.add(&cb11); tab4.add(&cb12);
    TableBox tab5(2, 2);
    FrameBox frame(&tab);
    tab5.add(&frame); tab5.add(&tab2); tab5.add(&tab3); tab5.add(&tab4);

    vector<vector<char>> pr2(6);
    for(int i=0; i< 6; ++i){
        vector<char> tmp(12, ' ');
        pr2[i] = tmp;
    }
    tab5.layout();
    tab5.print(pr2, 0, 0);
    for(int i=0; i< 6; ++i){
        for(int j=0; j<12; ++j){
            cout<<pr2[i][j];
        }
        cout<<endl;
    };



//    cin.ignore();

    return 0;
}

