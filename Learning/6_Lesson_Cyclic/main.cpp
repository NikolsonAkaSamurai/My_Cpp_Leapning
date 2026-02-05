#include <iostream>
#include <windows.h>

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


int main() {
    SetConsoleOutputCP(CP_UTF8);

    //Примеры:
    // simple();
    // staps();
    // empty_kvad();
    // simple_vail();
    // vail_with_bpake();
    vail_with_kontinue();
    return 0;
}

