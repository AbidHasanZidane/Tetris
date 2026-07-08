#include "tetrisgame.h"
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TetrisGame::TetrisGame(QObject *parent)
    : QObject(parent)
{
    m_board = createEmptyBoard();
}

// ---------------------------------------------------------------------------
// Board helpers
// ---------------------------------------------------------------------------

TetrisGame::Board TetrisGame::createEmptyBoard()
{
    return Board(ROWS, QVector<int>(COLS, 0));
}

// ---------------------------------------------------------------------------
// Piece factory
// ---------------------------------------------------------------------------

TetrisPiece TetrisGame::getRandomPiece()
{
    TetrisPiece piece = TetrisPiece::random();
    // Adjust x so the piece is horizontally centred on the board
    piece.x = COLS / 2 - int(piece.shape[0].size()) / 2;
    piece.y = 0;
    return piece;
}

// ---------------------------------------------------------------------------
// Collision detection
// ---------------------------------------------------------------------------

bool TetrisGame::isValidMove(const TetrisPiece &piece) const
{
    for (int y = 0; y < piece.shape.size(); ++y) {
        for (int x = 0; x < piece.shape[y].size(); ++x) {
            if (piece.shape[y][x] > 0) {
                int bx = piece.x + x;
                int by = piece.y + y;

                // Wall / floor check
                if (bx < 0 || bx >= COLS || by >= ROWS)
                    return false;

                // Collision with locked blocks (skip cells above the board)
                if (by >= 0 && m_board[by][bx] > 0)
                    return false;
            }
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// Rotation (clockwise) with basic wall-kick
// ---------------------------------------------------------------------------

void TetrisGame::rotate()
{
    if (m_gameOver || m_paused) return;

    const QVector<QVector<int>> &oldShape = m_currentPiece.shape;
    int N = oldShape.size();

    // Build a clockwise-rotated matrix
    QVector<QVector<int>> newShape(N, QVector<int>(N, 0));
    for (int y = 0; y < N; ++y)
        for (int x = 0; x < N; ++x)
            newShape[x][N - 1 - y] = oldShape[y][x];

    TetrisPiece testPiece = m_currentPiece;
    testPiece.shape = newShape;

    // Try the naive rotation first
    if (isValidMove(testPiece)) {
        m_currentPiece.shape = newShape;
        emit stateChanged();
        return;
    }

    // Wall-kick: try shifting right by 1
    testPiece.x++;
    if (isValidMove(testPiece)) {
        m_currentPiece.shape = newShape;
        m_currentPiece.x++;
        emit stateChanged();
        return;
    }

    // Wall-kick: try shifting left by 1 (from original x)
    testPiece.x -= 2;
    if (isValidMove(testPiece)) {
        m_currentPiece.shape = newShape;
        m_currentPiece.x--;
        emit stateChanged();
        return;
    }

    // For the I-piece (4×4) try shifting farther
    if (oldShape.size() > 2) {
        testPiece.x++;          // back to original x
        testPiece.x -= 2;       // try left 2
        if (isValidMove(testPiece)) {
            m_currentPiece.shape = newShape;
            m_currentPiece.x -= 2;
            emit stateChanged();
            return;
        }
        testPiece.x += 4;       // try right 2
        if (isValidMove(testPiece)) {
            m_currentPiece.shape = newShape;
            m_currentPiece.x += 2;
            emit stateChanged();
            return;
        }
    }

    // Rotation failed — piece stays as-is; no stateChanged needed
}

// ---------------------------------------------------------------------------
// Movement
// ---------------------------------------------------------------------------

void TetrisGame::moveLeft()
{
    if (m_gameOver || m_paused) return;
    TetrisPiece test = m_currentPiece;
    test.x--;
    if (isValidMove(test)) {
        m_currentPiece.x--;
        emit stateChanged();
    }
}

void TetrisGame::moveRight()
{
    if (m_gameOver || m_paused) return;
    TetrisPiece test = m_currentPiece;
    test.x++;
    if (isValidMove(test)) {
        m_currentPiece.x++;
        emit stateChanged();
    }
}

void TetrisGame::softDrop()
{
    if (m_gameOver || m_paused) return;
    TetrisPiece test = m_currentPiece;
    test.y++;
    if (isValidMove(test)) {
        m_currentPiece.y++;
        emit stateChanged();
    }
    // If it can't move down, we don't auto-lock here — the next tick handles it.
}

void TetrisGame::hardDrop()
{
    if (m_gameOver || m_paused) return;
    int rowsDropped = 0;
    TetrisPiece test = m_currentPiece;
    while (true) {
        test.y++;
        if (isValidMove(test)) {
            rowsDropped++;
            m_currentPiece.y++;
        } else {
            break;
        }
    }
    lockPiece();
    handlePostLock();
    emit stateChanged();
}

int TetrisGame::ghostY() const
{
    TetrisPiece ghost = m_currentPiece;
    while (true) {
        ghost.y++;
        if (!isValidMove(ghost)) {
            return ghost.y - 1;
        }
    }
}

// ---------------------------------------------------------------------------
// Locking
// ---------------------------------------------------------------------------

void TetrisGame::lockPiece()
{
    for (int y = 0; y < m_currentPiece.shape.size(); ++y) {
        for (int x = 0; x < m_currentPiece.shape[y].size(); ++x) {
            int val = m_currentPiece.shape[y][x];
            if (val > 0) {
                int bx = m_currentPiece.x + x;
                int by = m_currentPiece.y + y;
                if (by >= 0 && by < ROWS && bx >= 0 && bx < COLS) {
                    m_board[by][bx] = m_currentPiece.colorIndex;
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Line clearing
// ---------------------------------------------------------------------------

int TetrisGame::clearLines()
{
    int cleared = 0;
    for (int y = ROWS - 1; y >= 0; --y) {
        bool full = std::all_of(m_board[y].begin(), m_board[y].end(),
                                [](int c) { return c > 0; });
        if (full) {
            ++cleared;
            // Shift all rows above this one down by one
            for (int row = y; row > 0; --row)
                m_board[row] = m_board[row - 1];
            // New empty row at the top
            m_board[0] = QVector<int>(COLS, 0);
            // Re-check the same y index because a new row slid into it
            ++y;
        }
    }
    return cleared;
}

void TetrisGame::updateScoreAndSpeed(int cleared)
{
    if (cleared <= 0) return;

    int lineScore = 0;
    switch (cleared) {
    case 1: lineScore = Points::SINGLE; break;
    case 2: lineScore = Points::DOUBLE; break;
    case 3: lineScore = Points::TRIPLE; break;
    case 4: lineScore = Points::TETRIS; break;
    }
    m_score += lineScore;
    m_linesCleared += cleared;

    // Speed increases directly with score: every SPEED_SCORE_STEP points
    // the drop interval decreases by SPEED_INCREMENT ms
    m_dropSpeed = std::max(MIN_SPEED,
                           m_initialSpeed - (m_score / SPEED_SCORE_STEP) * SPEED_INCREMENT);
    emit scoreChanged(m_score);
}

void TetrisGame::handlePostLock()
{
    int cleared = clearLines();
    updateScoreAndSpeed(cleared);

    // Advance to next piece
    m_currentPiece = m_nextPiece;
    m_nextPiece = getRandomPiece();
    emit nextPieceChanged();

    // If the new piece collides immediately → game over
    if (!isValidMove(m_currentPiece)) {
        m_gameOver = true;
        m_running = false;
        emit gameOver(m_score);
    }
}

// ---------------------------------------------------------------------------
// Game-loop tick (called by QTimer)
// ---------------------------------------------------------------------------

void TetrisGame::tick()
{
    if (m_gameOver || m_paused || !m_running) return;

    TetrisPiece test = m_currentPiece;
    test.y++;
    if (isValidMove(test)) {
        m_currentPiece.y++;
    } else {
        lockPiece();
        handlePostLock();
    }
    emit stateChanged();
}

// ---------------------------------------------------------------------------
// Start / Pause
// ---------------------------------------------------------------------------

void TetrisGame::start()
{
    m_board = createEmptyBoard();
    m_score = 0;
    m_linesCleared = 0;
    m_dropSpeed = m_initialSpeed;
    m_gameOver = false;
    m_running = true;
    m_paused = false;

    m_currentPiece = getRandomPiece();
    m_nextPiece = getRandomPiece();

    emit scoreChanged(m_score);
    emit nextPieceChanged();
    emit stateChanged();
}

void TetrisGame::togglePause()
{
    if (m_gameOver || !m_running) return;
    m_paused = !m_paused;
    emit stateChanged();   // trigger a repaint so the overlay can be shown
}
