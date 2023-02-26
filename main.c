#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "print.h"

int main() {
    int width = 10;
    int height = 10;

    CellMap* cellMap = initCellMap(width, height);

    printCellMap(cellMap);

    return 0;
}
