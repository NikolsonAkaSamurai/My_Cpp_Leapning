#include <windows.h>
#include <iostream>

using namespace std;

//Операторы сравнения
void conditional() {
    short a = 1, b = 2;

    bool result;

    cout.setf(ios::boolalpha);
    //больше >
    result = a > b;
    cout << "a > b = " << result << endl;
    //больше или равно >=
    result = a >= b;
    cout << "a >= b = " << result << endl;
    //меньше <
    result = a < b;
    cout << "a < b = " << result << endl;
    //меньше или равно <=
    result = a <= b;
    cout << "a <= b = " << result << endl;
    //равно ==
    result = a == b;
    cout << "a == b = " << result << endl;
    //оператор неравенства !=
    result = a != b;
    cout << "a != b = " << result << endl;
}

//Вводим данные и программа отвечает что больше
void logic() {
    cout << "Введите число А" << endl;
    int a;
    cin >> a;

    cout << "Введите число Б" << endl;
    int b;
    cin >> b;

    if (a < b) {
        cout << "a < b" << endl;
    }
    else if (a == b) {
        cout << "a == b" << endl;
    }
    else {
        cout << " a > b!" << endl;
    }
}

int main() {

    SetConsoleOutputCP(CP_UTF8);

    // conditional();
    logic();

    return 0;
}