#include <iostream>
#include <cstddef>
#include <compare>
#include <iterator>
#include <utility>


template <typename T>
class contiguous_iterator {
private:
    T* ptr_ = nullptr;

public:
    using iterator_category = std::contiguous_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    contiguous_iterator() = default;
    explicit contiguous_iterator(T* ptr) : ptr_(ptr) {}

    reference operator*() const { return *ptr_; }
    pointer operator->() const { return ptr_; }
    reference get() const { return *ptr_; }

    contiguous_iterator& operator++() { 
        ++ptr_; 
        return *this; 
    }
    contiguous_iterator operator++(int) { 
        auto tmp = *this; 
        ++ptr_; 
        return tmp; 
    }

    contiguous_iterator& operator--() { 
        --ptr_; 
        return *this; 
    }
    contiguous_iterator operator--(int) { 
        auto tmp = *this; 
        --ptr_; 
        return tmp; 
    }

    contiguous_iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
    contiguous_iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }

    friend contiguous_iterator operator+(contiguous_iterator it, difference_type n) { it += n; return it; }
    friend contiguous_iterator operator+(difference_type n, contiguous_iterator it) { it += n; return it; }
    friend contiguous_iterator operator-(contiguous_iterator it, difference_type n) { it -= n; return it; }

    friend difference_type operator-(const contiguous_iterator& lhs, const contiguous_iterator& rhs) {
        return lhs.ptr_ - rhs.ptr_;
    }

    reference operator[](difference_type n) const { return *(ptr_ + n); }
    auto operator<=>(const contiguous_iterator& other) const = default;
};

template <typename T>
class Seq {
    T* d = nullptr;
    size_t n = 0;
    size_t cap = 0;

    void grow() {
        size_t nc = cap ? (cap * 3 / 2 + 1) : 2; 
        T* nd = new T[nc];
        for (size_t i = 0; i < n; i++) nd[i] = std::move(d[i]);
        delete[] d;
        d = nd;
        cap = nc;
    }

public:
    using iterator = contiguous_iterator<T>;

    Seq() = default;

    ~Seq() {
        delete[] d;
    }

    // Move-конструктор
    Seq(Seq&& o) noexcept : d(o.d), n(o.n), cap(o.cap) {
        o.d = nullptr;
        o.n = 0;
        o.cap = 0;
    }

    // Move-присваивание
    Seq& operator=(Seq&& o) noexcept {
        if (this != &o) {
            delete[] d;
            d = o.d;
            n = o.n;
            cap = o.cap;
            o.d = nullptr;
            o.n = 0;
            o.cap = 0;
        }
        return *this;
    }

    Seq(const Seq&) = delete;
    Seq& operator=(const Seq&) = delete;

    void push_back(const T& v) {
        if (n >= cap) grow();
        d[n++] = v;
    }

    void push_back(T&& v) {
        if (n >= cap) grow();
        d[n++] = std::move(v);
    }

    void insert(size_t i, const T& v) {
        if (i > n) return;
        if (n >= cap) grow();
        for (size_t k = n; k > i; k--) d[k] = std::move(d[k - 1]);
        d[i] = v;
        n++;
    }

    void insert(size_t i, T&& v) {
        if (i > n) return;
        if (n >= cap) grow();
        for (size_t k = n; k > i; k--) d[k] = std::move(d[k - 1]);
        d[i] = std::move(v);
        n++;
    }

    void erase(size_t i) {
        if (i >= n) return;
        for (size_t k = i; k + 1 < n; k++) d[k] = std::move(d[k + 1]);
        n--;
    }

    size_t size() const { return n; }
    T& operator[](size_t i) { return d[i]; }
    const T& operator[](size_t i) const { return d[i]; }

    iterator begin() { return iterator(d); }
    iterator end() { return iterator(d + n); }
};


template <typename T>
class DList {
    struct Node {
        T data;
        Node* prev = nullptr;
        Node* next = nullptr;
    };
    Node* head = nullptr;
    Node* tail = nullptr;
    size_t n = 0;

public:
    struct iterator {
        Node* p = nullptr;
        T& operator*() const { return p->data; }
        T& get() const { return p->data; }
        iterator& operator++() { p = p->next; return *this; }
        iterator& operator--() { p = p->prev; return *this; }
        bool operator!=(const iterator& o) const { return p != o.p; }
        bool operator==(const iterator& o) const { return p == o.p; }
    };

    DList() = default;

    ~DList() {
        while (head) {
            Node* t = head;
            head = head->next;
            delete t;
        }
    }

    DList(DList&& o) noexcept : head(o.head), tail(o.tail), n(o.n) {
        o.head = o.tail = nullptr;
        o.n = 0;
    }

    DList& operator=(DList&& o) noexcept {
        if (this != &o) {
            while (head) {
                Node* t = head;
                head = head->next;
                delete t;
            }
            head = o.head;
            tail = o.tail;
            n = o.n;
            o.head = o.tail = nullptr;
            o.n = 0;
        }
        return *this;
    }

    DList(const DList&) = delete;
    DList& operator=(const DList&) = delete;

    void push_back(const T& v) {
        Node* x = new Node{v, tail, nullptr};
        if (tail) tail->next = x; else head = x;
        tail = x;
        n++;
    }

    void push_back(T&& v) {
        Node* x = new Node{std::move(v), tail, nullptr};
        if (tail) tail->next = x; else head = x;
        tail = x;
        n++;
    }

    void insert(size_t i, const T& v) {
        if (i == n) return push_back(v);
        Node* c = head;
        for (size_t k = 0; k < i; k++) c = c->next;
        Node* x = new Node{v, c->prev, c};
        if (c->prev) c->prev->next = x; else head = x;
        c->prev = x;
        n++;
    }

    void insert(size_t i, T&& v) {
        if (i == n) return push_back(std::move(v));
        Node* c = head;
        for (size_t k = 0; k < i; k++) c = c->next;
        Node* x = new Node{std::move(v), c->prev, c};
        if (c->prev) c->prev->next = x; else head = x;
        c->prev = x;
        n++;
    }

    void erase(size_t i) {
        if (i >= n) return;
        Node* c = head;
        for (size_t k = 0; k < i; k++) c = c->next;
        if (c->prev) c->prev->next = c->next; else head = c->next;
        if (c->next) c->next->prev = c->prev; else tail = c->prev;
        delete c;
        n--;
    }

    size_t size() const { return n; }
    T& operator[](size_t i) {
        Node* c = head;
        while (i--) c = c->next;
        return c->data;
    }

    iterator begin() { return {head}; }
    iterator end() { return {nullptr}; }
};


template <typename T>
class SList {
    struct Node {
        T data;
        Node* next = nullptr;
    };
    Node* head = nullptr;
    Node* tail = nullptr;
    size_t n = 0;

public:
    struct iterator {
        Node* p = nullptr;
        T& operator*() const { return p->data; }
        T& get() const { return p->data; }
        iterator& operator++() { p = p->next; return *this; }
        bool operator!=(const iterator& o) const { return p != o.p; }
        bool operator==(const iterator& o) const { return p == o.p; }
    };

    SList() = default;

    ~SList() {
        while (head) {
            Node* t = head;
            head = head->next;
            delete t;
        }
    }

    SList(SList&& o) noexcept : head(o.head), tail(o.tail), n(o.n) {
        o.head = o.tail = nullptr;
        o.n = 0;
    }

    SList& operator=(SList&& o) noexcept {
        if (this != &o) {
            while (head) {
                Node* t = head;
                head = head->next;
                delete t;
            }
            head = o.head;
            tail = o.tail;
            n = o.n;
            o.head = o.tail = nullptr;
            o.n = 0;
        }
        return *this;
    }

    SList(const SList&) = delete;
    SList& operator=(const SList&) = delete;

    void push_back(const T& v) {
        Node* x = new Node{v, nullptr};
        if (tail) tail->next = x; else head = x;
        tail = x;
        n++;
    }

    void push_back(T&& v) {
        Node* x = new Node{std::move(v), nullptr};
        if (tail) tail->next = x; else head = x;
        tail = x;
        n++;
    }

    void insert(size_t i, const T& v) {
        if (i == 0) {
            head = new Node{v, head};
            if (!tail) tail = head;
            n++;
            return;
        }
        if (i == n) return push_back(v);
        Node* c = head;
        for (size_t k = 0; k < i - 1; k++) c = c->next;
        c->next = new Node{v, c->next};
        n++;
    }

    void insert(size_t i, T&& v) {
        if (i == 0) {
            head = new Node{std::move(v), head};
            if (!tail) tail = head;
            n++;
            return;
        }
        if (i == n) return push_back(std::move(v));
        Node* c = head;
        for (size_t k = 0; k < i - 1; k++) c = c->next;
        c->next = new Node{std::move(v), c->next};
        n++;
    }

    void erase(size_t i) {
        if (i >= n) return;
        if (i == 0) {
            Node* t = head;
            head = head->next;
            if (!head) tail = nullptr;
            delete t;
            n--;
            return;
        }
        Node* c = head;
        for (size_t k = 0; k < i - 1; k++) c = c->next;
        Node* t = c->next;
        c->next = t->next;
        if (!c->next) tail = c;
        delete t;
        n--;
    }

    size_t size() const { return n; }
    T& operator[](size_t i) {
        Node* c = head;
        while (i--) c = c->next;
        return c->data;
    }

    iterator begin() { return {head}; }
    iterator end() { return {nullptr}; }
};

template <class C>
void print_elements(C& c) {
    for (size_t i = 0; i < c.size(); i++) {
        std::cout << c[i] << (i + 1 < c.size() ? ", " : "");
    }
    std::cout << "\n";
}

template <class C>
void run_scenario(const char* container_title) {
    std::cout << "Тестирование: " << container_title << "\n";

    // Создание объекта контейнера для хранения int
    C c;

    // Добавление десяти элементов 
    for (int i = 0; i < 10; ++i) {
        c.push_back(i);
    }

    // Вывод содержимого контейнера
    std::cout << "Содержимое: ";
    print_elements(c);

    // Вывод размера контейнера
    std::cout << "Размер: " << c.size() << "\n";

    // Удаление третьего, пятого и седьмого элементов
    c.erase(2); // был 3-й (число 2)
    c.erase(3); // был 5-й (число 4)
    c.erase(4); // был 7-й (число 6)

    // Вывод содержимого
    std::cout << "После удаления 3-го, 5-го, 7-го: ";
    print_elements(c);

    // Добавление элемента 10 в начало
    c.insert(0, 10);

    // Вывод содержимого
    std::cout << "После добавления 10 в начало: ";
    print_elements(c);

    // Добавление элемента 20 в середину контейнера
    c.insert(c.size() / 2, 20);

    // Вывод содержимого
    std::cout << "После добавления 20 в середину: ";
    print_elements(c);

    // Добавление элемента 30 в конец контейнера
    c.push_back(30);

    // Вывод содержимого
    std::cout << "После добавления 30 в конец: ";
    print_elements(c);

    // Проверка итератора
    std::cout << "Обход через итератор (.get()): ";
    for (auto it = c.begin(); it != c.end(); ++it) {
        std::cout << it.get() << " ";
    }
    std::cout << "\n";

    // Проверка семантики перемещения
    C moved_c = std::move(c);
    std::cout << "После std::move: новый размер = " << moved_c.size() 
              << ", старый размер = " << c.size() << "\n\n";
}

int main() {
    run_scenario<Seq<int>>("Последовательный контейнер");
    run_scenario<DList<int>>("Двунаправленный список");
    run_scenario<SList<int>>("Однонаправленный список");
    return 0;
}