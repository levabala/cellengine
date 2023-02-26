#include <stdio.h>

#include "print.h"
#include "cell.h"

static void printCell(CellMap* cellMap, Cell* cell, int x, int y) {
    printf("%d", cell->mass);

    if (x != cellMap->width - 1) {
        printf(" ");
    } else {
        printf("\n");
    }
}

void printCellMap(CellMap* cellMap) {
    iterateCellMap(cellMap, printCell);
}
