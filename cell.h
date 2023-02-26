#pragma once

typedef struct {
    int mass;
    int vx, vy; // velocity x/y
} Cell;

typedef struct {
    Cell* cells;
    int width;
    int height;
} CellMap;

void iterateCellMap(CellMap* cellMap, void (*iterator)(CellMap* cellMap, Cell* cell, int x, int y));
CellMap* initCellMap(int width, int height);