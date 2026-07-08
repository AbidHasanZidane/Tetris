#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QColor>
#include <QVector>

// Board dimensions (in blocks)
constexpr int COLS = 12;
constexpr int ROWS = 24;

// Block size in pixels
constexpr int BLOCK_SIZE = 25;

// Preview area dimensions (in blocks) — largest piece is 4×4
constexpr int NEXT_COLS = 4;
constexpr int NEXT_ROWS = 4;

// Color palette (index 0 = empty, indices 1–7 = tetromino colors)
const QVector<QColor> COLORS = {
    QColor(),                    // 0: empty
    QColor("#FF0D72"),           // 1: I
    QColor("#0DC2FF"),           // 2: L
    QColor("#0DFF72"),           // 3: J
    QColor("#F538FF"),           // 4: O
    QColor("#FF8E0D"),           // 5: T
    QColor("#FFE138"),           // 6: S
    QColor("#3877FF")            // 7: Z
};

// Scoring
namespace Points {
    constexpr int SINGLE = 100;
    constexpr int DOUBLE = 300;
    constexpr int TRIPLE = 500;
    constexpr int TETRIS = 800;
    constexpr int SOFT_DROP = 1;
    constexpr int HARD_DROP = 2;
}

// Level threshold: every 1000 points = +1 level
constexpr int LEVEL_THRESHOLD = 1000;

// Drop speed (milliseconds)
constexpr int INITIAL_SPEED = 800;
constexpr int SPEED_INCREMENT = 50;   // faster by this much per level
constexpr int MIN_SPEED = 100;

#endif // CONSTANTS_H
