#include "debug.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"

#define DEBUG 1

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

void iterateCellMap(CellMap* cellMap, void (*iterator)(CellMap* cellMap, Cell* cell, int x, int y, void* context), void* context) {
    int y, x;
    return;
    for (y = 0; y < cellMap->height; y++) {
        for (x = 0; x < cellMap->width; x++) {
            Cell* cell = getCell(cellMap, x, y);

            printf("oma");
            (iterator)(cellMap, cell, x, y, context);
            printf("\nomar2");
        }
    }
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

typedef enum  {
    Horizontal,
    Vertical,
} Direction;


void transferMomentum(Cell* cellFrom, Cell* cellTo, int signX, int signY) {
    Cell cellFromPrev, cellToPrev;
    IFDEBUG {
        cellFromPrev = *cellFrom;
        cellToPrev = *cellTo;
    }

    int velocityTransferredXTo = cellFrom->vx * signX * cellFrom->mass / cellTo->mass;
    int velocityTransferredXFrom = velocityTransferredXTo * cellTo->mass / cellFrom->mass;
    cellFrom->vx -= velocityTransferredXFrom;
    cellTo->vx += velocityTransferredXTo;

    int velocityTransferredY = cellFrom->vy * signY * cellFrom->mass / cellTo->mass;

    cellFrom->vy -= velocityTransferredY;
    cellTo->vy += velocityTransferredY;

    IFDEBUG printf(
       "sign %d:%d\n",
       signX,
       signY
    );
    IFDEBUG printf(
        "from %d:%d -> %d:%d\n",
        cellFromPrev.vx,
        cellFromPrev.vy,
        cellFrom->vx,
        cellFrom->vy
    );
    IFDEBUG printf(
        "to %d:%d -> %d:%d\n",
        cellToPrev.vx,
        cellToPrev.vy,
        cellTo->vx,
        cellTo->vy
    );
}

void elapseTimeForCell(CellMap* cellMap, Cell* cell, int x, int y, void* context) {
    printf("----- %d:%d\n", x, y);
    printf("-- %d\n", cell);
    printf("-- %d\n", cell->mass);
    printf("-- %d\n", cell->vx);
    printf("-- %d\n", cell->vy);
    if (cell->mass == 0 || (cell->vx == 0 && cell->vy == 0)) {
        return;
    }

    printf("lal");

    int interactX = getInteractionCoord(x, cell->vx, cellMap->width - 1);
    int interactY = getInteractionCoord(y, cell->vy, cellMap->height - 1);
    printf("lul");

    IFDEBUG printf("-- interact between %d:%d and %d:%d\n", x, y, interactX, interactY);

    Cell* cellInteract = getCell(cellMap, interactX, interactY);

    IFDEBUG printf("masses %d:%d\n", cell->mass, cellInteract->mass);
    if (cellInteract->mass == 0) {
        IFDEBUG printf("swap\n");
        swapCells(cellMap, x, y, interactX, interactY);
    } else {
        IFDEBUG printf("transfer\n");
        transferMomentum(cell, cellInteract, sign(cell->vx), sign(cell->vy));
    }
}

void elapseTime(CellMap* cellMap, int timeDelta) {
    int i;
    for (i = 0; i < timeDelta; i++) {
        iterateCellMap(cellMap, elapseTimeForCell, NULL);
        printf("elapsed");
    }
}

void setCell(CellMap* cellMap, int x, int y, Cell* cell) {
    memcpy(cellMap->cells + getCellMapIndex(cellMap, x, y), cell, CELL_SIZE);
}

void aggregateMass(CellMap* cellMap, Cell* cell, int x, int y, int* mass) {
    printf("-- %d", mass);
    printf("--- %d", cell->mass);
    (*mass) += cell->mass;
}

int getTotalMass(CellMap* cellMap) {
    printf("getTotalMass");
    int* mass = 0;
    iterateCellMap(cellMap, elapseTimeForCell, &mass);

    return *mass;
}
