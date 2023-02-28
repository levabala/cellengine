#pragma once

typedef struct {
  int mass;
  int elasticity; // divider of momentum transfer
  int vx, vy;     // velocity x/y
} Cell;

typedef struct {
  Cell* cells;
  int width;
  int height;
} CellMap;

typedef void (*CellMapIteratorWithContext
)(CellMap* cellMap, Cell* cell, int x, int y, void* context);
typedef void (*CellMapIterator)(CellMap* cellMap, Cell* cell, int x, int y);

void iterateCellMapWithContext(
    CellMap* cellMap,
    void (*iterator)(CellMap* cellMap, Cell* cell, int x, int y, void* context),
    void* context
);
void iterateCellMap(
    CellMap* cellMap,
    void (*iterator)(CellMap* cellMap, Cell* cell, int x, int y, void* context)
);

CellMap* initCellMap(int width, int height);
void setCell(CellMap* cellMap, int x, int y, Cell* cell);
void elapseTime(CellMap* cellMap, int timeDelta);
int getTotalMass(CellMap* cellMap);
