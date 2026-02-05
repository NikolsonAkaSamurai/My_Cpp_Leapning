#include <iostream>

using namespace std;

int main() {

    cout << "Hello World!" << endl;

    for (int i = 1; i <= 15; i++) {
        for (int j = 1; j <= 30; j++) {
            cout << "*";
        }
        cout << endl;
    }
    cout << endl;

    cin.get();

    return 0;
}