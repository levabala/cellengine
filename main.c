#include <stdio.h>
#include <stdlib.h>

#include "cell.c"
#include "print.c"


int main() {
    int width = 10;
    int height = 10;

    CellMap* cellMap = initCellMap(width, height);

    printCellMap(cellMap);

    return 0;
}
