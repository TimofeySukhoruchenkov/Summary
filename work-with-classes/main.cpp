#include <iostream>


class Rectangle {
protected:
    double length;
    double width;
    char *name;
public:
    Rectangle() {
        length = 0;
        width = 0;
        name = new char[0];
    }

    Rectangle(double l, double w) {
        length = l;
        width = w;
        name = new char[0];
    }

    virtual ~Rectangle() {
        delete[] name;
    }

    Rectangle(const Rectangle &other) {
        length = other.length;
        width = other.width;
        name = new char[0];
    }

    Rectangle& operator=(const Rectangle &other) {
        if(this != &other){
            length = other.length;
            width = other.width;
        }
        return *this;
    }

    void operator()(const char* c) {
        delete[] name;
        char *new_name = new char[strlen(c) + 1];
        strcpy(new_name, c);
        name = new_name;
    }

    friend std::ostream& operator<<(std::ostream& os, const Rectangle& other) {
        os << "Rectangle: " << other.length << " x " << other.width << std::endl;
        return os;
    }

    virtual void print() const = 0;

    virtual  double square_of_figure() const = 0;

    virtual bool compare(const Rectangle& r2) const = 0;
};

class Square: public Rectangle {
public:
    Square() : Rectangle() {}
    Square(double size) : Rectangle(size, size) {}
    double square_of_figure() const override {
        return width * length;
    }
    void print() const override {

        std::cout << "Square: " << name << " " <<  length << " x " << width << std::endl;
    }
    bool compare(const Rectangle& s2) const override {
        return square_of_figure() > s2.square_of_figure();
    }
};



int main(){
    Square s1(10.123);
    Square s2(5.123);
    Square s3;
    std::cout << s1 << s2 << s3;
    std::cout << s1.compare(s2) << std::endl;
    std::cout << s3.compare(s2) << std::endl;
    s3.print();
    s3("lira");
    s3.print();
    return 0;
}