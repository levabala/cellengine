#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "print.h"

void printBlank() {
    printf("nani");
    /* printf("\n"); */
    printf("the fuck");
}

int main() {
    int width = 5;
    int height = 5;

    CellMap* cellMap = initCellMap(width, height);

    printCellMap(cellMap);

    setCell(cellMap, 1, 2, &(Cell){.mass = 3, .vx = 6});
    setCell(cellMap, 2, 2, &(Cell){.mass = 7});
    setCell(cellMap, 3, 2, &(Cell){.mass = 2});
    setCell(cellMap, 4, 3, &(Cell){.mass = 4});

    int i = 5;
    while (1 && i--) {
        /* getchar(); */

        elapseTime(cellMap, 1);

        printf("lol");

        printf("lll2");
        printBlank();
        printf("llllwtf");
        printCellMap(cellMap);

        printf("total mass: %d\n", getTotalMass(cellMap));
    }

    return 0;
}
