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
