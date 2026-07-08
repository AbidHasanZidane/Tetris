#ifndef TETRISPIECE_H
#define TETRISPIECE_H

#include <QVector>
#include "constants.h"

// Pre-defined tetromino shapes (each cell stores a color index, 0 = empty)
// Index 0 is a dummy empty shape so the JS-style 1-based indexing works.
const QVector<QVector<QVector<int>>> SHAPES = {
    {},                                                          // 0: placeholder
    {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},              // 1: I
    {{2,0,0},   {2,2,2},   {0,0,0}},                            // 2: L
    {{0,0,3},   {3,3,3},   {0,0,0}},                            // 3: J
    {{4,4},     {4,4}},                                         // 4: O
    {{0,5,0},   {5,5,5},   {0,0,0}},                            // 5: T
    {{0,6,6},   {6,6,0},   {0,0,0}},                            // 6: S
    {{7,7,0},   {0,7,7},   {0,0,0}}                             // 7: Z
};

struct TetrisPiece {
    int x = 0;                      // column of top-left corner on the board
    int y = 0;                      // row of top-left corner on the board
    QVector<QVector<int>> shape;    // 2-D grid of color indices
    int colorIndex = 0;             // cached color index for quick lookup

    TetrisPiece() = default;

    // Create a random piece (colorIndex 1..7)
    static TetrisPiece random();
};

#endif // TETRISPIECE_H
