#include <stdio.h>

#include "cell.h"
#include "print.h"

static void
printCell(CellMap* cellMap, Cell* cell, int x, int y, void* context) {
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
