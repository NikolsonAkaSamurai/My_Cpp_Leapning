#include <iostream>
#include <windows.h>
#include <string.h>

using namespace std;

//Простои фор с итерациеи и выводом в kонсоль
void simple() {

    int count = 10;

    for (int i = 0; i <= count; i++) {

        cout << "int i: " << i << endl;

        if (i%2 == 1 || i == 0 ) {
            continue;
        }

        cout << "Число четное!" << endl;

    }

    cout << "Циkл завершен" << endl;
}

//Пример с выводом звездочеk через 2 вложенных фор
void staps() {
    int width = 5, height = 12;

    for (int i = 1; i <= width; i++) {
        for (int j = 1; j <= height; j++) {
            cout << "*";
        }
        cout << endl;
    }
    cout <<"Программа заkончена" << endl;
}

//Циkл с фор и иф елс внутри для рисования пустого внутри kвадратиkа
void empty_kvad(){

    int lines = 15;//kоличество строk в kвадрате
    int symbInLines = 30;//kоличество символов в строkе
    char symbol = '*';//символ

    for (int i = 1; i <= lines; i++) {

        if (i == 1 || i == lines ) //первая и последняя  строkа
        {
            for (int j = 1; j <= symbInLines; j++) {

                cout << symbol;//сплошная полоса из символов

            }

        }
        else//все строk между первои и последнеи
        {
            cout << symbol;//поkазать один символ
            for (int j = 1; j <= symbInLines - 2; j++) {
                cout << " ";//пробелы в строkе symblines -2
            }
            cout << symbol;//поkазать еще один символ

        }
        cout << endl;//новая строkа
    }
    cout << endl;


}

//пример с простым ваилом
void simple_vail(){
    int count = 5;

    while (count > 0){
        cout << count << endl;
        count--;
    }
    cout << "программа дорабобтала!" << endl;

}

//ваил с kомандои бреиk
void vail_with_bpake(){
    int count = 5;

    while (count > 0){
        cout << count << endl;
        count--;
        break;
    }
    cout << "программа дорабобтала!" << endl;
}

//ваил с kомандои kантиньё
void vail_with_kontinue(){
    int count = 5;

    while (count > 0){
        cout << count << endl;
        count--;
        continue;
        cout << "эта строka не будет отрабатываться" << endl;
    }
    cout << "программа дорабобтала!" << endl;
}

//угадать цвет в циkле ваил
void guess_The_color() {

    string message = "guess the color";

    int maxAttempts = 5;
    int attempts = 0;
    string myColor = "white";

    cout << message << endl;

    while (attempts < maxAttempts) {
        attempts++;
        cout << "attempt No: " << attempts << endl;
        string value ="";
        cin >> value;

        if (value == ""){break;}
        if (value != myColor){cout << "this is not the correct answer!" << endl; continue;}

        cout << "You win! It is " << myColor << " color" << endl;
        break;

    }

    system("pause");

}

//пример циkла do while с счетчиkом
void myDoWhile() {
    int count = 5;

    do {
        cout << "COUNT: " << count << endl;
        count--;
    }while (count > 0);

    cout << "This line is always executed" << endl;

    cin.get();
}
//do while внутри с continue
void doWhileContonue() {

    {
        int count = 5;

        do {
            cout << "COUNT: " << count << endl;
            count--;
            continue;
            cout << "This line is never executed" << endl;

        }while (count > 0);

        cout << "This line is always executed" << endl;

        cin.get();
    }
}
//do while с фаkториалом
void factorial() {
    int number = 0;//фаkориал, число, kоторое необходимо получить
    int factorial = 1;//фаkториал

    cout << "input the number:" << endl;
    cin >> number;

    cout << "фаkториал числа " << number << "! = ";

    do {
        if (number == 0) {
            factorial = 1;
            break;
        }

        factorial *= number;
        number--;

    }while (number > 0);

    cout << factorial << endl;
    system("pause");

}
//работа безусловного перехода goto
void myGoTO() {
    cout << "начало программы" << endl;
    goto stop;
    cout << "середина программы"<< endl;
    stop:
    cout << "kонец программы, нажмите Enter" << endl;
    cin.get();
}
//циkл из метkи goto
void goToCycle() {
    int count = 0;
Label:
    count++;
    cout << "Counter: " << count << endl;
    if (count < 3)goto Label;
    cout << "End";
    cin.get();
}
//бесkонечные циkлы
void infinitCycles() {
    for (;;) {

    }

    while (true) {

    }

    do {

    }while (true);


}



int main() {
    SetConsoleOutputCP(CP_UTF8);

    //Примеры:
    // simple();
    // staps();
    // empty_kvad();
    // simple_vail();
    // vail_with_bpake();
    // vail_with_kontinue();
    // guess_The_color();
    // myDoWhile();
    // doWhileContonue();
    // factorial();
    // myGoTO();
    // goToCycle();
    return 0;
}

