#include <iostream>
#include <cstddef>

// Последовательный
class Seq {
    int *d = nullptr;
    size_t n = 0, cap = 0;
    void grow() {
        size_t nc = cap ? cap * 3 / 2 : 2;
        int *nd = new int[nc];
        for (size_t i = 0; i < n; i++) nd[i] = d[i];
        delete[] d; d = nd; cap = nc;
    }
public:
    struct It {
        int *p;
        int& operator*() { return *p; }
        int get() { return *p; }
        It& operator++() { p++; return *this; }
        bool operator!=(It o) { return p != o.p; }
    };
    Seq() = default;
    ~Seq() { delete[] d; }
    Seq(Seq &&o) : d(o.d), n(o.n), cap(o.cap) { o.d = nullptr; o.n = o.cap = 0; }
    Seq& operator=(Seq &&o) {
        if (this != &o) { delete[] d; d=o.d; n=o.n; cap=o.cap; o.d=nullptr; o.n=o.cap=0; }
        return *this;
    }
    Seq(const Seq&) = delete;
    Seq& operator=(const Seq&) = delete;

    void push_back(int v) { if (n == cap) grow(); d[n++] = v; }
    void insert(size_t i, int v) { if (n == cap) grow();
        for (size_t j = n; j > i; j--) d[j] = d[j-1]; d[i] = v; n++; }
    void erase(size_t i) { for (size_t j = i; j < n-1; j++) d[j] = d[j+1]; n--; }
    size_t size() const { return n; }
    int& operator[](size_t i) { return d[i]; }
    It begin() { return {d}; }
    It end() { return {d + n}; }
};

// Двусвязный список
class DList {
    struct N { int data; N *prev, *next; };
    N *head = nullptr, *tail = nullptr;
    size_t n = 0;
public:
    struct It {
        N *p;
        int& operator*() { return p->data; }
        int get() { return p->data; }
        It& operator++() { p = p->next; return *this; }
        bool operator!=(It o) { return p != o.p; }
    };
    DList() = default;
    ~DList() { while (head) { N *t = head; head = head->next; delete t; } }
    DList(DList &&o) : head(o.head), tail(o.tail), n(o.n) { o.head=o.tail=nullptr; o.n=0; }
    DList& operator=(DList &&o) {
        if (this != &o) {
            while (head) { N *t = head; head = head->next; delete t; }
            head=o.head; tail=o.tail; n=o.n; o.head=o.tail=nullptr; o.n=0;
        }
        return *this;
    }
    DList(const DList&) = delete;
    DList& operator=(const DList&) = delete;

    void push_back(int v) {
        N *x = new N{v, tail, nullptr};
        if (tail) tail->next = x; else head = x;
        tail = x; n++;
    }
    void insert(size_t i, int v) {
        if (i == n) return push_back(v);
        N *c = head; for (size_t k = 0; k < i; k++) c = c->next;
        N *x = new N{v, c->prev, c};
        if (c->prev) c->prev->next = x; else head = x;
        c->prev = x; n++;
    }
    void erase(size_t i) {
        N *c = head; for (size_t k = 0; k < i; k++) c = c->next;
        if (c->prev) c->prev->next = c->next; else head = c->next;
        if (c->next) c->next->prev = c->prev; else tail = c->prev;
        delete c; n--;
    }
    size_t size() const { return n; }
    int& operator[](size_t i) { N *c = head; while (i--) c = c->next; return c->data; }
    It begin() { return {head}; }
    It end() { return {nullptr}; }
};

// Односвязный список
class SList {
    struct N { int data; N *next; };
    N *head = nullptr, *tail = nullptr;
    size_t n = 0;
public:
    struct It {
        N *p;
        int& operator*() { return p->data; }
        int get() { return p->data; }
        It& operator++() { p = p->next; return *this; }
        bool operator!=(It o) { return p != o.p; }
    };
    SList() = default;
    ~SList() { while (head) { N *t = head; head = head->next; delete t; } }
    SList(SList &&o) : head(o.head), tail(o.tail), n(o.n) { o.head=o.tail=nullptr; o.n=0; }
    SList& operator=(SList &&o) {
        if (this != &o) {
            while (head) { N *t = head; head = head->next; delete t; }
            head=o.head; tail=o.tail; n=o.n; o.head=o.tail=nullptr; o.n=0;
        }
        return *this;
    }
    SList(const SList&) = delete;
    SList& operator=(const SList&) = delete;

    void push_back(int v) {
        N *x = new N{v, nullptr};
        if (tail) tail->next = x; else head = x;
        tail = x; n++;
    }
    void insert(size_t i, int v) {
        if (i == 0) { head = new N{v, head}; if (!tail) tail = head; n++; return; }
        if (i == n) return push_back(v);
        N *c = head; for (size_t k = 0; k < i-1; k++) c = c->next;
        c->next = new N{v, c->next}; n++;
    }
    void erase(size_t i) {
        if (i == 0) { N *t = head; head = head->next; if (!head) tail = nullptr; delete t; n--; return; }
        N *c = head; for (size_t k = 0; k < i-1; k++) c = c->next;
        N *t = c->next; c->next = t->next; if (!c->next) tail = c;
        delete t; n--;
    }
    size_t size() const { return n; }
    int& operator[](size_t i) { N *c = head; while (i--) c = c->next; return c->data; }
    It begin() { return {head}; }
    It end() { return {nullptr}; }
};

// Демонстрация
template <class C>
void show(const char *s, C &c) {
    std::cout << s << ": ";
    for (size_t i = 0; i < c.size(); i++) std::cout << c[i] << (i+1<c.size()?", ":"");
    std::cout << "\n";
}

template <class C>
void test(const char *name) {
    std::cout << "\n" << name << "\n";
    C c;
    for (int i = 0; i < 10; i++) c.push_back(i);
    show("Содержимое", c);
    std::cout << "Размер: " << c.size() << "\n";

    c.erase(2); c.erase(3); c.erase(4);
    show("После удаления", c);

    c.insert(0, 10);  show("+10 в начало", c);
    c.insert(4, 20);  show("+20 в середину", c);
    c.push_back(30);  show("+30 в конец", c);

    std::cout << "Итератор: ";
    for (auto it = c.begin(); it != c.end(); ++it) std::cout << it.get() << " ";
    std::cout << "\n";

    C m = static_cast<C&&>(c);
    std::cout << "После move: size=" << m.size() << ", old size=" << c.size() << "\n";
}

int main() {
    test<Seq>("Последовательный");
    test<DList>("Двусвязный");
    test<SList>("Односвязный");
}