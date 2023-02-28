#include "debug.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
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

static Cell* getCell(CellMap* cellMap, int x, int y) {
  return cellMap->cells + getCellMapIndex(cellMap, x, y);
}

inline void iterateCellMapWithContext(
    CellMap* cellMap, CellMapIteratorWithContext iterator, void* context
) {
  int y, x;
  for (y = 0; y < cellMap->height; y++) {
    for (x = 0; x < cellMap->width; x++) {
      Cell* cell = getCell(cellMap, x, y);

      (iterator)(cellMap, cell, x, y, context);
    }
  }
}

void iterateCellMap(CellMap* cellMap, CellMapIteratorWithContext iterator) {
  iterateCellMapWithContext(cellMap, iterator, NULL);
}

int sign(int val) {
  if (val == 0) {
    return 0;
  }

  return val > 0 ? 1 : -1;
}

int normalizeCoordinate(int coord, int max) {
  if (coord < 0) {
    return max + coord;
  }

  if (coord > max) {
    return coord - max - 1;
  }

  return coord;
}

int getInteractionCoord(int coord, int velocity, int max) {
  int delta = sign(velocity);

  return normalizeCoordinate(coord + delta, max);
}

Cell* getInteractionCell(CellMap* cellMap, Cell* cell, int x, int y) {
  int dx = sign(cell->vx);
  int dy = sign(cell->vy);

  int interactX = normalizeCoordinate(x + dx, cellMap->width - 1);
  int interactY = normalizeCoordinate(y + dy, cellMap->height - 1);

  return getCell(cellMap, interactX, interactY);
}

void swapCells(CellMap* cellMap, int x1, int y1, int x2, int y2) {
  Cell* cell1 = getCell(cellMap, x1, y1);
  Cell* cell2 = getCell(cellMap, x2, y2);

  Cell cell1Value = *cell1;

  memcpy(cell1, cell2, CELL_SIZE);
  memcpy(cell2, &cell1Value, CELL_SIZE);
}

typedef enum {
  Horizontal,
  Vertical,
} Direction;

void transferMomentum(Cell* cellFrom, Cell* cellTo, int signX, int signY) {
  Cell cellFromPrev, cellToPrev;
  IFDEBUG {
    cellFromPrev = *cellFrom;
    cellToPrev = *cellTo;
  }

  int velocityTransferredXTo =
      cellFrom->vx * signX * cellFrom->mass / cellTo->mass;
  int velocityTransferredXFrom =
      velocityTransferredXTo * cellTo->mass / cellFrom->mass;
  cellFrom->vx -= velocityTransferredXFrom;
  cellTo->vx += velocityTransferredXTo;

  int velocityTransferredY =
      cellFrom->vy * signY * cellFrom->mass / cellTo->mass;

  cellFrom->vy -= velocityTransferredY;
  cellTo->vy += velocityTransferredY;

  debug_print("sign %d:%d\n", signX, signY);
  debug_print(
      "from %d:%d -> %d:%d\n",
      cellFromPrev.vx,
      cellFromPrev.vy,
      cellFrom->vx,
      cellFrom->vy
  );
  debug_print(
      "to %d:%d -> %d:%d\n",
      cellToPrev.vx,
      cellToPrev.vy,
      cellTo->vx,
      cellTo->vy
  );
}

void elapseTimeForCell(
    CellMap* cellMap, Cell* cell, int x, int y, void* context
) {
  if (cell->mass == 0 || (cell->vx == 0 && cell->vy == 0)) {
    return;
  }

  int interactX = getInteractionCoord(x, cell->vx, cellMap->width - 1);
  int interactY = getInteractionCoord(y, cell->vy, cellMap->height - 1);

  debug_print(
      "-- interact between %d:%d and %d:%d\n", x, y, interactX, interactY
  );

  Cell* cellInteract = getCell(cellMap, interactX, interactY);

  debug_print("masses %d:%d\n", cell->mass, cellInteract->mass);
  if (cellInteract->mass == 0) {
    debug_print("swap\n");
    swapCells(cellMap, x, y, interactX, interactY);
  } else {
    debug_print("transfer\n");
    transferMomentum(cell, cellInteract, sign(cell->vx), sign(cell->vy));
  }
}

void elapseTime(CellMap* cellMap, int timeDelta) {
  int i;
  for (i = 0; i < timeDelta; i++) {
    iterateCellMap(cellMap, elapseTimeForCell);
  }
}

void setCell(CellMap* cellMap, int x, int y, Cell* cell) {
  memcpy(cellMap->cells + getCellMapIndex(cellMap, x, y), cell, CELL_SIZE);
}

void aggregateMass(CellMap* cellMap, Cell* cell, int x, int y, void* mass) {
  (*(int*)mass) += cell->mass;
}

int getTotalMass(CellMap* cellMap) {
  int mass = 0;
  iterateCellMapWithContext(cellMap, aggregateMass, &mass);

  return mass;
}
