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

/*
 *  x 5 y 2 (factor 2)
 *  1 2 3 4 5 6 7
 *  x x y x x y x
 *  1: 1 % (2(factor)+1=3) = 1 -> x (1, 0)
 *  2: 2 % (2(factor)+1=3) = 2 -> x (2, 0)
 *  3: 3 % (2(factor)+1=3) = 0 -> y (2, 1)
 *  4: 4 % (2(factor)+1=3) = 1 -> x (3, 1)
 *  5: 5 % (2(factor)+1=3) = 2 -> x (4, 1)
 *  6: 6 % (2(factor)+1=3) = 0 -> y (4, 2)
 *  7: 7 % (2(factor)+1=3) = 1 -> x (5, 2)
 */
int getClockValue(int freq1, int freq2, int clock) {
    debug_print("getClockValue %d %d %d\n", freq1, freq2, clock);
    if (freq2 > freq1) {
        return getClockValue(freq2, freq1, clock);
    }

    int factor = freq2 == 0 ? freq1 : freq1 / freq2;
    return (clock + 2) % (factor + 1);
}

void elapseTimeForCell(
    CellMap* cellMap, Cell* cell, int x, int y, void* _tick
) {
    if (cell->mass == 0 || cell->movesLeft <= 0 ||
        (cell->vx == 0 && cell->vy == 0)) {
        return;
    }

    int tick = (*(int*)_tick);

    cell->movesLeft--;

    int isVerticalInteraction = getClockValue(cell->vx, cell->vy, tick) ? 1 : 0;

    debug_print("isVerticalInteraction %d\n", isVerticalInteraction);

    int interactX = x, interactY = y;
    if (isVerticalInteraction) {
        interactY = getInteractionCoord(y, cell->vy, cellMap->height - 1);
    } else {
        interactX = getInteractionCoord(x, cell->vx, cellMap->width - 1);
    }

    if (interactX == x && interactY == y) {
        return;
    }

    debug_print(
        "interact between %d:%d and %d:%d\n", x, y, interactX, interactY
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

void resetCellMoved(CellMap* cellMap, Cell* cell, int x, int y, void* context) {
    cell->movesLeft = cell->vx + cell->vy;
}

void aggregateMaxVelocitySum(
    CellMap* cellMap, Cell* cell, int x, int y, void* _maxSum
) {
    int* maxSum = _maxSum;

    int velocitySum = cell->vx + cell->vy;
    *maxSum = *maxSum > velocitySum ? *maxSum : velocitySum;
}

void elapseTime(CellMap* cellMap, int timeDelta, ElapseTimeIterator iterator) {
    for (int i = 0; i < timeDelta; i++) {
        int maxVelocitySum = 0;
        iterateCellMapWithContext(
            cellMap, aggregateMaxVelocitySum, &maxVelocitySum
        );

        debug_print("maxVelocitySum %d\n", maxVelocitySum);

        for (int j = 0; j < maxVelocitySum; j++) {
            debug_print("-- velocity iteration %d\n", j);
            iterateCellMapWithContext(cellMap, elapseTimeForCell, &j);

            iterator(cellMap, i, j);
        }

        iterateCellMap(cellMap, resetCellMoved);
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
