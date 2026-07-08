#include "tetrispiece.h"
#include <QRandomGenerator>

TetrisPiece TetrisPiece::random()
{
    TetrisPiece piece;
    // Random index 1..7 (matching SHAPES)
    int idx = QRandomGenerator::global()->bounded(1, 8);
    piece.shape = SHAPES[idx];

    // Derive the color index from the first non-zero cell in the shape
    for (const auto &row : piece.shape) {
        for (int val : row) {
            if (val > 0) {
                piece.colorIndex = val;
                goto found;
            }
        }
    }
found:
    // Initial position: centered at the top of the board
    piece.x = COLS / 2 - int(piece.shape[0].size()) / 2;
    piece.y = 0;

    return piece;
}
