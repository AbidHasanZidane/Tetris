#ifndef TETRISGAME_H
#define TETRISGAME_H

#include <QObject>
#include <QVector>
#include "constants.h"
#include "tetrispiece.h"

class TetrisGame : public QObject
{
    Q_OBJECT

public:
    explicit TetrisGame(QObject *parent = nullptr);

    void start();
    void togglePause();
    bool isPaused() const { return m_paused; }
    bool isGameOver() const { return m_gameOver; }
    bool isRunning() const { return m_running; }

    // Board access
    using Board = QVector<QVector<int>>;
    const Board &board() const { return m_board; }

    // Current / next piece
    const TetrisPiece &currentPiece() const { return m_currentPiece; }
    const TetrisPiece &nextPiece() const { return m_nextPiece; }

    // Score / level / lines
    int score() const { return m_score; }
    int level() const { return m_level; }
    int linesCleared() const { return m_linesCleared; }
    int dropSpeed() const { return m_dropSpeed; }
    int ghostY() const;

    void setInitialSpeed(int ms) { m_initialSpeed = ms; }
    void setExtremeMode(bool on) { m_extremeMode = on; }
    bool isExtremeMode() const { return m_extremeMode; }

    // --- Actions (called from keyboard or UI) ---
    void moveLeft();
    void moveRight();
    void softDrop();       // one row down (with score)
    void hardDrop();       // instant drop to bottom
    void rotate();         // clockwise

    // Called by the timer tick — advance game one step (gravity)
    void tick();

signals:
    void stateChanged();             // emitted whenever the board/piece changes
    void gameOver(int finalScore);
    void scoreChanged(int score);
    void levelChanged(int level);
    void nextPieceChanged();

private:
    Board createEmptyBoard();
    TetrisPiece getRandomPiece();

    bool isValidMove(const TetrisPiece &piece) const;
    void lockPiece();
    int  clearLines();
    void updateScoreAndLevel(int linesCleared);
    void handlePostLock();

    Board m_board;
    TetrisPiece m_currentPiece;
    TetrisPiece m_nextPiece;

    int m_score = 0;
    int m_level = 1;
    int m_linesCleared = 0;
    int m_dropSpeed = INITIAL_SPEED;
    int m_initialSpeed = INITIAL_SPEED;

    bool m_running = false;
    bool m_paused = false;
    bool m_gameOver = false;
    bool m_extremeMode = false;
};

#endif // TETRISGAME_H
