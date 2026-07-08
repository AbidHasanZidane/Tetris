#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QColor>
#include <QVector>

// Board dimensions (in blocks)
constexpr int COLS = 12;
constexpr int ROWS = 24;

// Block size in pixels
constexpr int BLOCK_SIZE = 25;

// Margin around the play area for the border frame
constexpr int BOARD_MARGIN = 6;

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

// Speed increases as score grows: every SPEED_SCORE_STEP points,
// drop interval decreases by SPEED_INCREMENT ms
constexpr int SPEED_SCORE_STEP = 1000;
constexpr int INITIAL_SPEED    = 800;
constexpr int SPEED_INCREMENT  = 50;
constexpr int MIN_SPEED        = 100;

// Difficulty presets (initial drop speed in ms)
constexpr int REGULAR_INITIAL_SPEED = 800;
constexpr int EXTREME_INITIAL_SPEED = 500;

#endif // CONSTANTS_H
