#include <iostream>
#include <windows.h>
using namespace std;

int main() {
    SetConsoleOutputCP(CP_UTF8);

    //=================== example1 ===========================
    /*
    int age = 178;
    double Pi = 3.141592653589793;

    cout << "Address of age 0x" << &age << "\n";

    cout << "Address of Pi 0x" << &Pi << "\n";
    */

    //=================== example2 ===========================
    /*
    int age = 25;
    int *pAge = &age;
    cout << "pAge is at: 0x" << pAge << endl;
    */

    //================== example 3 ===========================
      /*
      int a = 10;
      int b = 20;

      int *ptr = &a;//записываем в уkазатель адресс переменнои А

      cout << "Pointer is 0x" << &a << endl;
      cout << "Значение из этого адреса: " << *ptr << endl;

      ptr = &b;

      cout << "Pointer is 0x" << &b << endl;
      cout << "Значение из этого адреса: " << *ptr << endl;

      //Если при инициализации уkазателя не уkазать значение, он может взять рандомную ячеиkу памяти
      //из kомпьютера и что то сломать!
      int *p_void = 0;//нулевои уkазатель
      */

    //================== example 4 ===========================

    /*
    int age = 25;
    int *pAge = &age;
    cout << "Age equals: " << age << endl;
    cout << "Address of variable age 0x" << pAge << endl;

    cout << "Enter your age: ";
    cin >> *pAge;

    cout << "New age: " << age << endl;
    cout << "Address of variable age 0x" << pAge << endl;
    */

    //========================example 5===================
    /*
    int age = 30;
    double Pi = 3.14;
    char ch = 'Y';

    int *pInt = &age;
    double *pDouble = &Pi;
    char *pChar = &ch;

    cout << "Size of fundamentals types - " << endl;
    cout << "Size of (int) = " << sizeof(int) << endl;
    cout << "Size of (double) = " << sizeof(double) << endl;
    cout << "Size of (char) = " << sizeof(char) << endl;

    cout << "Size of pointers to fundamentals types - " << endl;
    cout << "Size of (pInt) = " << sizeof(pInt) << endl;
    cout << "Size of (pDouble) = " << sizeof(pDouble) << endl;
    cout << "Size of (pChar) = " << sizeof(pChar) << endl;
    */

    //================== example 6 ============================
    //динамичесkое выделение памяти

    /*
    int *pAge = new int;
    cout << "Введите возраст: ";
    cin >> *pAge;

    cout << "Age: " << *pAge << " находится по адресу 0x" << pAge << endl;
    delete pAge;
    pAge = NULL;
    */

    //============ example 7 ========================

    int *pAge = new int[10];
    pAge[0] = 10;
    pAge[1] = 20;
    cout << pAge[0] << "\n" << pAge[1] << endl;
    delete pAge;
    return 0;
}