#include <iostream>


template<class T>
class  B {
public:
    T* ptr;         // Указатель на управляемый объект
    size_t count;   // Счетчик 

    B(T* p) : ptr(p), count(1) {}
    
    ~B() {
        delete ptr;
    }
};

// Шаблон умного указателя с подсчетом ссылок
template<class T>
class MyShared {
    B<T>* cb;  // Блок управления с счетчиком

public:
    // Конструктор, принимающий сырой указатель
    explicit MyShared(T* p = nullptr) : cb(new B<T>(p)) {}

    // Деструктор
    ~MyShared() {
        release();
    }

    // Конструктор копирования
    MyShared(const MyShared& other) : cb(other.cb) {
        if (cb) cb->count++;
    }

    // Оператор присваивания копированием
    MyShared& operator=(const MyShared& ot) {
        if (this != &ot) {
            release();
            cb = ot.cb;
            if (cb) cb->count++;
        }
        return *this;
    }

    // Конструктор перемещения
    MyShared(MyShared&& ot) noexcept : cb(ot.cb) {
        ot.cb = nullptr;
    }

    // Оператор присваивания перемещением
    MyShared& operator=(MyShared&& ot) noexcept {
        if (this != &ot) {
            release();
            cb = ot.cb;
            ot.cb = nullptr;
        }
        return *this;
    }

    // Возвращает количество владельцев
    size_t use_count() const {
        return cb ? cb->count : 0;
    }

    // Возвращает сырой указатель
    T* get() const {
        return cb ? cb->ptr : nullptr;
    }

    // Перегрузка операторов
    T& operator*() const {
        return *cb->ptr;
    }

    T* operator->() const {
        return cb->ptr;
    }

private:
    // Если счетчик достиг 0 - удаляет B, что влечет удаление управляемого объекта
    void release() {
        if (cb) {
            cb->count--;
            if (cb->count == 0) {
                delete cb;
            }
        }
    }
};

// Функция для создания MyShared
template<class T, class... Args>
MyShared<T> Make_MyShared(Args&&... args) {
    return MyShared<T>(new T(std::forward<Args>(args)...));
}


class Test {
    int value;
public:
    Test(int v) : value(v) {
        std::cout << "Test(" << value << ") created\n";
    }
    
    ~Test() {
        std::cout << "Test(" << value << ") destroyed\n";
    }
    
    void print() const {
        std::cout << "Value: " << value << "\n";
    }
};

int main() {
    // Создание shared указателя
    auto p1 = Make_MyShared<Test>(10);
    p1->print();
    std::cout << "p1 use count: " << p1.use_count() << "\n";

    // Копирование
    {
        auto p2 = p1;
        p2->print();
        std::cout << "p1 use count after copy: " << p1.use_count() << "\n";
        std::cout << "p2 use count: " << p2.use_count() << "\n";
    }
    std::cout << "p1 use count after p2 destroyed: " << p1.use_count() << "\n";

    // Перемещение
    auto p3 = std::move(p1);
    std::cout << "After move:\n";
    std::cout << "p1 use count: " << (p1.get() ? p1.use_count() : 0) << "\n";
    std::cout << "p3 use count: " << p3.use_count() << "\n";
    p3->print();

    return 0;
}
