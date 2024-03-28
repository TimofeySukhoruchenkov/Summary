#include <iostream>

template<typename T>
class Set {
private:
    int *elements;
    int size;
public:
    Set() {
        elements = new T[0];
        size = 0;
    }

    Set(const Set &other) {
        size = other.size;
        elements = new T[size];
        for (int i = 0; i < size; i++) {
            elements[i] = other.elements[i];
        }
    }

    ~Set() {
        delete[] elements;
    }

    void add(T element) {
        if (!contains(element)) {
            T *new_elements = new T[size + 1];
            for (int i = 0; i < size; i++) {
                new_elements[i] = elements[i];
            }
            new_elements[size] = element;
            delete[] elements;
            elements = new_elements;
            size++;
        }
    }

    void remove(T element) {
        if (contains(element)) {
            T *new_elements = new T[size - 1];
            int j = 0;
            for (int i = 0; i < size; i++) {
                if (elements[i] != element) {
                    new_elements[j] = elements[i];
                    j++;
                }
            }
            delete[] elements;
            elements = new_elements;
            size--;
        }
    }


    Set<T> intersection(const Set &other) const {
        Set result;
        for (int i = 0; i < size; i++) {
            if (other.contains(elements[i])) {
                result.add(elements[i]);
            }
        }
        return result;
    }

    Set<T> union_set(const Set &other) const {
        Set result(*this);
        for (int i = 0; i < other.size; i++) {
            result.add(other.elements[i]);
        }
        return result;
    }

    Set<T> difference(const Set &other) const {
        Set result(*this);
        for (int i = 0; i < other.size; i++) {
            result.remove(other.elements[i]);
        }
        return result;
    }

    bool contains(T element) const {
        for (int i = 0; i < size; i++) {
            if (elements[i] == element) {
                return true;
            }
        }
        return false;
    }


    T get_size() const {
        return size;
    }

    void print() const {
        std::cout << "{";
        for (int i = 0; i < size; i++) {
            std::cout << elements[i];
            if (i != size - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "}" << std::endl;
    }
};

void menu_set(Set<int> &set1) {
    std::cout << std::endl;
    int noms = 1;
    while (noms != 0) {
        std::cout << "Choose an option:" << std::endl;
        std::cout << "\t 1 - Add the element" << std::endl;
        std::cout << "\t 2 - Remove the element" << std::endl;
        std::cout << "\t 0 - Back to main menu" << std::endl;
        std::cout << "Option: ";
        std::cin >> noms;
        switch (noms) {
            case 0: {
                break;
            }
            case 1: {
                int element;
                std::cout << "Element: ";
                std::cin >> element;
                set1.add(element);
                std::cout << std::endl;
                break;
            }
            case 2: {
                int element;
                std::cout << "Element: ";
                std::cin >> element;
                set1.remove(element);
                std::cout << std::endl;
                break;
            }
            default: {
                std::cout << "Wrong number of option" << std::endl;
            }
        }
    }
}

void menu() {
   Set<int> setm[2];
   int nom = 1;
   while(nom != 0){
       std::cout << "Choose an option:" << std::endl;
       std::cout << "\t 1 - Work with set" << std::endl;
       std::cout << "\t 2 - Print the sets" << std::endl;
       std::cout << "\t 3 - Intersection of two sets" << std::endl;
       std::cout << "\t 4 - Union of two sets" << std::endl;
       std::cout << "\t 5 - Difference of two sets" << std::endl;
       std::cout << "\t 0 - Close menu" << std::endl;
       std::cout << "Option: ";
       std::cin >> nom;
       switch(nom){
           case 0: {
               break;
           }
           case 1: {
               std::cout << "Set number(1 or 2): " << std::endl;
               int num;
               std::cin >> num;
               if((num != 1) && (num != 2)){
                   std::cout << "Wrong number" << std::endl;
               }
               else {
                   menu_set(setm[num - 1]);
               }
               break;
           }
           case 2: {
               setm[0].print();
               setm[1].print();
               break;
           }
           case 3:{
               std::cout << "Intersection:" << std::endl;
               Set<int> intersection = setm[0].intersection(setm[1]);
               intersection.print();
               break;
           }
           case 4:{
               std::cout << "Union:" << std::endl;
               Set<int> uni = setm[0].union_set(setm[1]);
               uni.print();
               break;
           }
           case 5:{
               std::cout << "Set number(1 or 2): " << std::endl;
               int num;
               std::cin >> num;
               if((num != 1) && (num != 2)){
                   std::cout << "Wrong number" << std::endl;
               }
               else {
                   std::cout << "Difference:" << std::endl;
                   Set<int> dif= setm[num - 1].difference(setm[2 - num]);
                   dif.print();
                   break;
               }
               break;
           }
           default: {
               std::cout << "Wrong number of option" << std::endl;
           }
       }
   }
}

int main() {
menu();
std::cout << "END OF TESTS" << std::endl;
return 0;
}