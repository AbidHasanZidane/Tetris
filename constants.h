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
    QColor("#00BFFF"),           // 1: I — sky blue
    QColor("#FF8C00"),           // 2: L — dark orange
    QColor("#1E90FF"),           // 3: J — dodger blue
    QColor("#FFD700"),           // 4: O — gold
    QColor("#9932CC"),           // 5: T — dark orchid
    QColor("#00CC66"),           // 6: S — retro green
    QColor("#FF3333")            // 7: Z — retro red
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

// Difficulty presets
namespace Difficulty {
    constexpr int Regular   = 1000;

    constexpr int Extreme   = 500;
}

#endif // CONSTANTS_H
