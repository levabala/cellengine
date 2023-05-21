#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cell.h"
#include "print.h"

void printBlank() {
    printf("\n");
}

int mssleep(long miliseconds) {
    struct timespec req = {
        (int)(miliseconds / 1000), (miliseconds % 1000) * 1000000};

    return nanosleep(&req, NULL);
}

void clearScreen() {
    printf("\e[1;1H\e[2J");
}

void printOnElapseTimeIteration(
    CellMap* cellMap, int timeTick, int velocityTick
) {
    clearScreen();

    /* getchar(); */
    mssleep(200);

    printBlank();
    printCellMap(cellMap);

    printf("total mass: %d\n", getTotalMass(cellMap));
    printf("timeTick/velocityTick: %d/%d", timeTick, velocityTick);
}

int main() {
    int width = 20;
    int height = 5;

    CellMap* cellMap = initCellMap(width, height);

    printCellMap(cellMap);

    setCell(cellMap, 1, 2, &(Cell){.mass = 1});
    setCell(cellMap, 2, 2, &(Cell){.mass = 2, .vx = 1});
    setCell(cellMap, 3, 2, &(Cell){.mass = 1});
    setCell(cellMap, 4, 3, &(Cell){.mass = 1});

    printBlank();
    printCellMap(cellMap);

    int i = 50;
    while (1 && i--) {
        elapseTime(cellMap, 1, printOnElapseTimeIteration);
    }

    return 0;
}
