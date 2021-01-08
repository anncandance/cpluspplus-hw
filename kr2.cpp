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


// задача 2
class First{
public:
    double foo[600];
    First(){};
};

class Second{
public:
    double bar[600];
    Second() = default;
};

class Third{
public:
    int foo;
    Third() = default;
};

class Fourth{
public:
    Fourth() = default;
};

class Fifth{
public:
    double foo[400];
    Fifth() = default;
};

class Sixth{
public:
    char foo[600];
    Sixth() = default;
};


template<typename T, typename = void>
struct has_foo : false_type{
    enum
    {
        val=0,
        val2=0
    };
};

template<typename T>
struct has_foo<T, decltype(declval<T>().foo, void())> : true_type {
    enum
    {
        val = is_array<decltype(declval<T>().foo)>::value,
        val2 = val && (500 <= sizeof(decltype(T().foo)) / sizeof(double))
    };
};

void second_task_test(){
    cout<<has_foo<First>::val2<<endl;
    cout<<has_foo<Second>::val2<<endl;
    cout<<has_foo<Third>::val2<<endl;
    cout<<has_foo<Fourth>::val2<<endl;
    cout<<has_foo<Fifth>::val2<<endl;
    cout<<has_foo<Sixth>::val2<<endl;
}

//задача 4

template<bool x>
struct Fill{
    template<class It, class T>
    static void do_fill(It f, It l, const T& v);
};

template<> struct Fill< false >
{
    template<class It, class T >
    static void do_fill(It first, It last, const T& value)
    {
        cout << "using standard fill"<<endl;
        for (; first != last; ++first) {
            *first = value;
        }
    }
};


template<> struct Fill < true >
{
    template<class It, class T>
    static void do_fill(It first, It last, const T& value)
    {
        cout<<"using memset fill"<<endl;
        memset(first, value, last-first);
    }
};

template<class It, class T>
static void my_fill(It f, It l,  const T& v)
{
    Fill<is_trivially_copyable<T>::value && sizeof(T) == 1>::do_fill(f, l, v);
};


template<bool x>
struct Fill_n{
    template<class It, class Size, class T>
    static void do_fill_n(It it, Size s, const T& v);
};

template<> struct Fill_n< false >
{
    template<class It, class Size, class T >
    static void do_fill_n(It it, Size s, const T& value)
    {
        cout << "using standard fill_n"<<endl;
        for ( Size i = 0; i != s; ++i) {
            *it = value;
            ++it;
        }
    }
};


template<> struct Fill_n < true >
{
    template<class It, class Size, class T>
    static void do_fill_n(It it, Size s, const T& value)
    {
        cout<<"using memset fill_n"<<endl;
        memset(it, value, s);
    }
};

template<class It, class Size, class T>
static void my_fill_n(It it, Size s,  const T& v)
{
    Fill_n<is_trivially_copyable<T>::value && sizeof(T) == 1>::do_fill_n(it, s, v);
};




void fourth_task_test(){
    vector<int> v1;
    for (int i = 0; i < 10; ++i){
        v1.push_back(i);
    }
    auto it = v1.begin();
    auto en = v1.end();
    my_fill(it, en, -1);
    for (int i = 0; i < 10; ++i){
        cout << v1[i];
    }
    cout<<endl;
    char v2[] = "abcdef";
    char* en2 = v2 + 3;
    my_fill(v2, en2, 'o');
    for (int i = 0; i < 7; ++i){
        cout << v2[i];
    }
    cout<<endl;
    it = v1.begin();
    my_fill_n(it, 5, 5);
    for (int i = 0; i < 10; ++i){
        cout << v1[i];
    }
    cout<<endl;
    my_fill_n(v2, 2, 'v');
    for (int i = 0; i < 7; ++i){
        cout << v2[i];
    }

}

// задача 1


class NullType {
public:
    static inline void print(){
        cout<<"";
    }
};


// typelist
template <class T, class U>
struct Typelist
{
    typedef T Head;
    typedef U Tail;
public:
    static inline void print(){
        cout<<typeid(T).name()<< " ";
        U::print();
        cout<<endl;
    }
};



#define TYPELIST_1(T1) Typelist<T1, NullType>
#define TYPELIST_2(T1, T2) Typelist<T1, TYPELIST_1(T2) >
#define TYPELIST_3(T1, T2, T3) Typelist<T1, TYPELIST_2(T2, T3) >
#define TYPELIST_4(T1, T2, T3, T4) Typelist<T1, TYPELIST_3(T2, T3, T4) >
#define TYPELIST_5(T1, T2, T3, T4, T5) Typelist<T1, TYPELIST_4(T2, T3, T4, T5) >



template <class Tlist, class T> struct Append;
template <> struct Append<NullType, NullType>
        {
        typedef NullType Result;
        };

template <class T> struct Append<NullType, T>
         {
             typedef Typelist<T,NullType> Result;
         };

template <class Head, class Tail> struct Append<NullType, Typelist<Head, Tail> >
         {
             typedef Typelist<Head, Tail> Result;
         };

template <class Head, class Tail, class T> struct Append<Typelist<Head, Tail>, T>
         {
             typedef Typelist<Head, typename Append<Tail, T>::Result>
                 Result;
         };

// tree

template <class T, class L, class R>
struct TypeTree
{
    typedef T Type;
    typedef L Left;
    typedef R Right;

};

// бахрома

template<class Tree, class List>
struct walk;

// пустое дерево - возвращаем сам список
template<class List> struct walk<NullType, List >
{
    typedef typename Append<List, NullType>::Result res;
};

// лист
template<class T, class List> struct walk<TypeTree<T, NullType, NullType>, List>
{
    typedef typename Append<List, T>::Result res;
};

// не лист
template<class T, class L, class R, class List> struct walk<TypeTree<T, L, R>, List >
{
    typedef typename walk< L, List >::res res1;
    typedef typename walk< R, res1 >::res res;
};


void first_task_test(){
    walk<TypeTree<int, NullType, NullType> , NullType>::res first_walk;
    cout<< "First walk: "; first_walk.print();

    walk<TypeTree<int*,
            TypeTree<char,
                    TypeTree<int, NullType, NullType>,
                    TypeTree<float, NullType, NullType> >,
            TypeTree<unsigned int,
                    TypeTree<short int, NullType, NullType>,
                    TypeTree<double, NullType, NullType> > >, NullType>::res second_walk;

    cout<<"Second walk: "; second_walk.print();
}


// задача 3

struct No_replace{
    string find_str = "";
    string replace_str = "";
};

struct T_to_W{
    string find_str = "t";
    string replace_str = "w";
};

struct Test_to_west
{
    string find_str = "test";
    string replace_str = "west";
};

struct ab_to_b{
    string find_str = "ab";
    string replace_str = "b";
};

template<class Replace_policy>
struct Replace_stream: public Replace_policy{
    string buff;
    FILE *outf;
    Replace_stream(){ outf = nullptr; buff = "";};

    void get_char(char ch){
        buff.push_back(ch);

    }

    void replace() {
        string old = Replace_policy::find_str;
        string new_ = Replace_policy::replace_str;
        if (!old.empty()) {
            while (true) {

                size_t index = buff.find(old);
                if (index == string::npos) break;
                buff.replace(index, new_.size(), new_);
            }
        }
    }

    void print_next(){
        replace();
        if(!buff.empty()){
            size_t written = fwrite((&buff[0]), sizeof(buff[0]), 1, outf);
            buff.erase(buff.begin());
        }
    }

    void print_buff(string filename){
        outf = fopen(filename.c_str(), "w");
        while(!buff.empty()){
            print_next();
        }
        fclose(outf);
        outf = nullptr;
    }
};

void third_task_test(){
    string fn = "C:\\Users\\belyn\\patterns\\test";
    string str = "this is the test string";
    Replace_stream<Test_to_west> stream1;
    for(int i=0; i<str.size(); ++i){
        stream1.get_char(str[i]);
    }
    stream1.print_buff(fn);
    string fn2 = "C:\\Users\\belyn\\patterns\\test2";
    string str2 = "aaaaabbb";
    Replace_stream<ab_to_b> stream2;
    for(int i=0; i<str2.size(); ++i){
        stream2.get_char(str2[i]);
    }
    stream2.print_buff(fn2);
    string fn3 = "C:\\Users\\belyn\\patterns\\test3";
    string str3 = "simple test";
    Replace_stream<No_replace> stream3;
    for(int i=0; i<str3.size(); ++i){
        stream3.get_char(str3[i]);
    }
    stream3.print_buff(fn3);
}

int main()
{
    cout<<"first task test: "<<endl;
    first_task_test();
    cout<<"second task test: "<<endl;
    second_task_test();
    cout<<"third task test: "<<endl;
    third_task_test();
    cout<<"fourth task test: (output in files) "<<endl;
    fourth_task_test();

//    cin.ignore();

    return 0;
}

