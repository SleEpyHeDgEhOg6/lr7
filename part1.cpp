#include <iostream>
#include <utility>

using namespace std;

template<class T>
class MyUnique{
    T*pt=nullptr;//обычный указатель на объект 
public:
    explicit MyUnique(T*p): pt(p){}//explicit используется для явного указания компилятору, что конструктор или оператор преобразования должны использоваться только в явных преобразованиях типов

    ~MyUnique(){//деструктор 
        delete pt;//освобождаем память 
    }

    //удаляем конструктор копирования,чтобы нельзя было скопировать указатели 
    MyUnique(const MyUnique&)=delete;
    MyUnique& operator=(const MyUnique&) = delete;

    MyUnique(MyUnique&&ot){
        pt=ot.pt;//забираем указатель
        ot.pt=nullptr;//теперь у ot нету объекта (то есть обнуляем указатель)
    }
    MyUnique&operator=(MyUnique&&ot){//оператор перемещения 
        if(this!=&ot){//проверяем не равен ли он сам себе 
            delete pt;
            pt=ot.pt;//забираем указатель 
            ot.pt=nullptr;
        }
        return *this;//возвращаем ссылку на текущий объект 
    }
    T*get()const{//возвращаем указатель без передачи наследования 
        return pt;
    }
    T& operator*(){//позволяет использовать умный указатель как обычный 
        return *pt;
    }
    T*operator->(){//позволяет обращаться к объектам через ->
        return pt;
    }

};

template<class T,class... Args>//создали объект с переданными данными MyUnique
MyUnique<T>Make_MyUnique(Args&&... args){
    return MyUnique<T>(new T(std::forward<Args>(args)...));
}
class P{
    int x;
    int y;
public:
    P(int x,int y):x(x),y(y){
        std::cout<<"Созданы:"<<x<<","<<y<<"\n";
    }
    ~P(){
        std::cout<<"Удалены:"<<x<<","<<y<<"\n";
    }
    void print()const{
        std::cout<<"("<<x<<","<<y<<")"<<"\n";
    }

};

int main(){
    auto p=Make_MyUnique<P>(10,20);//создаем умный указатель на Р
    p->print();

    auto t=std::move(p);//std::move()-для перемещения, а не копирования объектов; перемещаем указатель,теперь р будет пустым 
    if(!p.get()) std::cout<<"Пустой указатель\n";
    t->print();

    MyUnique<P>r(nullptr);
    r=std::move(t);
    if(!t.get()) std::cout<<"Пустой указатель\n";
    r->print();

    return 0;
}
