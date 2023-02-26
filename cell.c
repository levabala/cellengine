#include "cell.h"

const size_t CELL_SIZE = sizeof(Cell);

CellMap* initCellMap(int width, int height) {
    Cell* cells = calloc(width * height, CELL_SIZE);
    CellMap* cellMap = (CellMap*)malloc(sizeof(CellMap));

    cellMap->cells = cells;
    cellMap->width = width;
    cellMap->height = height;

    return cellMap;
}

size_t getCellMapIndex(int x, int y, int width) {
    return y * width + x;
}

Cell getCellFromMapValue(CellMap* cellMap, int x, int y) {
    return cellMap->cells[getCellMapIndex(x, y, cellMap->width)];
}

Cell* getCellFromMapPointer(CellMap* cellMap, int x, int y) {
    return cellMap->cells + getCellMapIndex(x, y, cellMap->width);
}

void iterateCellMap(CellMap* cellMap, void (*iterator)(CellMap* cellMap, Cell* cell, int x, int y)) {
    int y, x;
    for (y = 0; y < cellMap->height; y++) {
        for (x = 0; x < cellMap->width; x++) {
            Cell* cell = getCellFromMapPointer(cellMap, x, y);

            (iterator)(cellMap, cell, x, y);
        }
    }
}

/* void elapseTime(CellMap* cellMap) { */
/*     int y, x; */
/*     for (y = 0; y < cellMap->height; y++) { */
/*         for (x = 0; x < cellMap->width; x++) { */
/*             Cell* cell = getCellFromMapPointer(cellMap, x, y); */
/*         } */
/*     } */
/* } */
