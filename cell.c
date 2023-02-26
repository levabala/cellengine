#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"

static const size_t CELL_SIZE = sizeof(Cell);

CellMap* initCellMap(int width, int height) {
    Cell* cells = calloc(width * height, CELL_SIZE);
    CellMap* cellMap = (CellMap*)malloc(sizeof(CellMap));

    cellMap->cells = cells;
    cellMap->width = width;
    cellMap->height = height;

    return cellMap;
}

static size_t getCellMapIndex(CellMap* cellMap, int x, int y) {
    return y * cellMap->width + x;
}

static Cell getCellFromMapValue(CellMap* cellMap, int x, int y) {
    return cellMap->cells[getCellMapIndex(cellMap, x, y)];
}

static Cell* getCellFromMapPointer(CellMap* cellMap, int x, int y) {
    return cellMap->cells + getCellMapIndex(cellMap, x, y);
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

static void setCell(CellMap* cellMap, int x, int y, Cell* cell) {
   memcpy(cellMap + getCellMapIndex(cellMap, x, y) * CELL_SIZE, cell, CELL_SIZE);
}
