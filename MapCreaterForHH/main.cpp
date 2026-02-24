#include "DrawWindow.h"
#include "ViewWindow.h"
#include "Contour.h"
#include <string.h>
#include "GeoCalculator.h"
#include "raylib.h"
#include <iomanip>

using namespace std;

void showPixels(string name);

int main() {

    // DrawWindow window;
    // window.run();

    ViewWindow window;
    window.run();



    // showPixels("1");  // для первого контура
    // showPixels("2");  // для второго
    // showPixels("3");  // для третьего (общего)

    return 0;



}

void showPixels(string name) {
    Contour myContour = IOContours::readContour(name);
    int c = 1;

    // Устанавливаем фиксированную точность
    cout << fixed << setprecision(6);  // ← 6 знаков после запятой

    for (Point p : myContour) {
        cout << "Point " << c
             << " | Geo: " << p.getX() << ", " << p.getY() << "" << endl;
        c++;
    }
    cout << endl;
}