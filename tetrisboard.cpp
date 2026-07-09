#include "tetrisboard.h"
#include "nextpiecewidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QFont>
#include <QRandomGenerator>
#include <cmath>

TetrisBoard::TetrisBoard(TetrisGame *game, NextPieceWidget *preview,
                         QWidget *parent)
    : QWidget(parent)
    , m_game(game)
    , m_preview(preview)
    , m_timer(new QTimer(this))
    , m_fadeTimer(new QTimer(this))
    , m_fadeAngle(0)
{
    setFixedSize(COLS * BLOCK_SIZE + 2 * BOARD_MARGIN,
                 ROWS * BLOCK_SIZE + 2 * BOARD_MARGIN);
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet("background-color: #1a1a1a;");

    // Connect game signals to local slots / forwarding
    connect(m_game, &TetrisGame::stateChanged,   this, QOverload<>::of(&QWidget::update));
    connect(m_game, &TetrisGame::scoreChanged,   this, &TetrisBoard::scoreUpdated);
    connect(m_game, &TetrisGame::gameOver,       this, &TetrisBoard::onGameOver);
    connect(m_game, &TetrisGame::nextPieceChanged, this, [this]() {
        if (m_preview) m_preview->setPiece(m_game->nextPiece());
    });

    // Timer drives the gravity ticks
    connect(m_timer, &QTimer::timeout, this, &TetrisBoard::onTick);

    // Fade timer for the "Press Start Game" text + falling background pieces
    connect(m_fadeTimer, &QTimer::timeout, this, [this]() {
        m_fadeAngle = std::fmod(m_fadeAngle + 3.6, 360.0);
        updateFallingPieces();
        update();
    });
    m_fadeTimer->start(40);  //~4 seconds

    // Seed initial falling pieces at various heights so the start screen
    // has visible activity right away.
    for (int i = 0; i < 5; ++i) {
        FallingPiece fp;
        int idx = QRandomGenerator::global()->bounded(1, 8);
        fp.shape      = SHAPES[idx];
        fp.colorIndex = idx;
        fp.x          = QRandomGenerator::global()->bounded(0, COLS - int(fp.shape[0].size()) + 1);
        fp.y          = QRandomGenerator::global()->bounded(0, ROWS);   // anywhere on the board
        fp.speed      = 0.04 + QRandomGenerator::global()->bounded(0.07);
        m_fallingPieces.append(fp);
    }
}

void TetrisBoard::startGame()
{
    m_fadeTimer->stop();
    m_fallingPieces.clear();
    m_game->start();
    if (m_preview) m_preview->setPiece(m_game->nextPiece());
    m_timer->start(m_game->dropSpeed());
    setFocus();   // grab keyboard focus
}


void TetrisBoard::onTick()
{
    m_game->tick();
    // Keep the timer interval in sync with the current drop speed
    if (m_timer->interval() != m_game->dropSpeed())
        m_timer->setInterval(m_game->dropSpeed());
}

void TetrisBoard::onGameOver(int finalScore)
{
    m_timer->stop();
    emit gameEnded(finalScore);
}

// ---------------------------------------------------------------------------
// Keyboard input
// ---------------------------------------------------------------------------

void TetrisBoard::keyPressEvent(QKeyEvent *event)
{
    if (m_game->isGameOver() || !m_game->isRunning()) {
        QWidget::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        m_game->moveLeft();
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        m_game->moveRight();
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        m_game->softDrop();
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        m_game->rotate();
        break;
    case Qt::Key_Space:
        m_game->hardDrop();
        break;
    case Qt::Key_P:
    case Qt::Key_Escape:
        m_game->togglePause();
        if (m_game->isPaused()) {
            m_timer->stop();
        } else {
            m_timer->start(m_game->dropSpeed());
        }
        emit pauseToggled(m_game->isPaused());
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------

void TetrisBoard::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Draw the outer frame first (widget coordinates)
    drawBorder(painter);

    // Shift into board coordinates so game content sits inside the margin
    painter.save();
    painter.translate(BOARD_MARGIN, BOARD_MARGIN);

    drawGrid(painter);
    drawBoard(painter);

    if (!m_game->isRunning() && !m_game->isGameOver())
        drawFallingPieces(painter);

    if (!m_game->isGameOver() && m_game->isRunning()) {
        if (!m_game->isExtremeMode())
            drawGhostPiece(painter);
        drawPiece(painter);
    }

    painter.restore();

    // Overlays cover the full widget
    if (m_game->isPaused())
        drawPauseOverlay(painter);

    if (!m_game->isRunning() && !m_game->isGameOver())
        drawStartOverlay(painter);

    if (m_game->isGameOver())
        drawGameOverOverlay(painter);
}

void TetrisBoard::drawBlock(QPainter &painter, int col, int row, const QColor &color)
{
    int x = col * BLOCK_SIZE;
    int y = row * BLOCK_SIZE;

    // Fill
    painter.fillRect(x, y, BLOCK_SIZE, BLOCK_SIZE, color);

    // Inner border
    painter.setPen(QColor("#333"));
    painter.drawRect(x, y, BLOCK_SIZE - 1, BLOCK_SIZE - 1);

    // Highlight edge (top + left) for a slight 3-D effect
    QColor light = color.lighter(140);
    painter.setPen(QPen(light, 2));
    painter.drawLine(x, y, x + BLOCK_SIZE - 1, y);          // top
    painter.drawLine(x, y, x, y + BLOCK_SIZE - 1);          // left

    // Shadow edge (bottom + right)
    QColor dark = color.darker(140);
    painter.setPen(QPen(dark, 2));
    painter.drawLine(x, y + BLOCK_SIZE - 1, x + BLOCK_SIZE - 1, y + BLOCK_SIZE - 1); // bottom
    painter.drawLine(x + BLOCK_SIZE - 1, y, x + BLOCK_SIZE - 1, y + BLOCK_SIZE - 1); // right
}

void TetrisBoard::drawBoard(QPainter &painter)
{
    const auto &board = m_game->board();
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            int val = board[row][col];
            if (val > 0)
                drawBlock(painter, col, row, COLORS[val]);
        }
    }
}

void TetrisBoard::drawPiece(QPainter &painter)
{
    const TetrisPiece &piece = m_game->currentPiece();
    for (int y = 0; y < piece.shape.size(); ++y) {
        for (int x = 0; x < piece.shape[y].size(); ++x) {
            if (piece.shape[y][x] > 0) {
                int col = piece.x + x;
                int row = piece.y + y;
                if (row >= 0)   // don't draw rows above the visible board
                    drawBlock(painter, col, row, COLORS[piece.shape[y][x]]);
            }
        }
    }
}

void TetrisBoard::drawGhostPiece(QPainter &painter)
{
    const TetrisPiece &piece = m_game->currentPiece();
    int ghostY = m_game->ghostY();

    // Skip if ghost is at the same position as the piece
    if (ghostY == piece.y) return;

    QColor color = COLORS[piece.colorIndex];
    color.setAlpha(60);

    for (int y = 0; y < piece.shape.size(); ++y) {
        for (int x = 0; x < piece.shape[y].size(); ++x) {
            if (piece.shape[y][x] > 0) {
                int col = piece.x + x;
                int row = ghostY + y;
                if (row >= 0) {
                    int px = col * BLOCK_SIZE;
                    int py = row * BLOCK_SIZE;

                    painter.fillRect(px, py, BLOCK_SIZE, BLOCK_SIZE, color);
                    painter.setPen(QPen(color, 1));
                    painter.drawRect(px, py, BLOCK_SIZE - 1, BLOCK_SIZE - 1);
                }
            }
        }
    }
}

void TetrisBoard::drawBorder(QPainter &painter)
{
    constexpr int T = 4;   // frame thickness

    // The board area sits inside BOARD_MARGIN
    int x = BOARD_MARGIN;
    int y = BOARD_MARGIN;
    int w = COLS * BLOCK_SIZE;
    int h = ROWS * BLOCK_SIZE;

    // Draw the frame just outside the board area
    QPen pen(QColor("#555"), T);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(x - T/2, y - T/2, w + T, h + T);
}

void TetrisBoard::drawGrid(QPainter &painter)
{
    painter.setPen(QPen(QColor("#333"), 1));
    for (int col = 0; col <= COLS; ++col)
        painter.drawLine(col * BLOCK_SIZE, 0,
                         col * BLOCK_SIZE, ROWS * BLOCK_SIZE);
    for (int row = 0; row <= ROWS; ++row)
        painter.drawLine(0, row * BLOCK_SIZE,
                         COLS * BLOCK_SIZE, row * BLOCK_SIZE);
}

void TetrisBoard::drawPauseOverlay(QPainter &painter)
{
    painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 160));

    painter.setPen(Qt::white);
    QFont font("Arial", 24, QFont::Bold);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, "Paused\n");
}

void TetrisBoard::drawStartOverlay(QPainter &painter)
{
    // Semi-transparent overlay
    painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 180));

    // Title
    painter.setPen(QColor("#4caf50"));
    QFont titleFont("Arial", 28, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(QRect(0, height() / 2 - 60, width(), 50),
                     Qt::AlignHCenter, "TETRIS");

    // Fading subtitle (sine-wave opacity)
    {
        qreal opacity = (std::sin(m_fadeAngle * M_PI / 180.0) + 1.0) / 2.0;
        painter.setOpacity(opacity);
        painter.setPen(QColor("#aaa"));
        QFont subFont("Arial", 12);
        painter.setFont(subFont);
        painter.drawText(QRect(0, height() / 2 - 10, width(), 30),
                         Qt::AlignHCenter, "Press Start Game to Play");
        painter.setOpacity(1.0);
    }
}

void TetrisBoard::drawGameOverOverlay(QPainter &painter)
{
    // Semi-transparent overlay
    painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 180));

    // Game Over title
    painter.setPen(QColor("#FF0D72"));
    QFont titleFont("Arial", 28, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(QRect(0, height() / 2 - 60, width(), 50),
                     Qt::AlignHCenter, "GAME OVER");

    // Retry hint
    painter.setPen(QColor("#aaa"));
    QFont subFont("Arial", 12);
    painter.setFont(subFont);
    painter.drawText(QRect(0, height() / 2 + 0, width(), 30),
                     Qt::AlignHCenter, "Press Retry to Play Again");
}

// ---------------------------------------------------------------------------
// Falling ghost pieces (start-screen background animation)
// ---------------------------------------------------------------------------

void TetrisBoard::updateFallingPieces()
{
    // Move existing pieces down
    for (int i = m_fallingPieces.size() - 1; i >= 0; --i) {
        m_fallingPieces[i].y += m_fallingPieces[i].speed;
        // Remove if fully off the bottom of the board
        if (m_fallingPieces[i].y > ROWS + 2)
            m_fallingPieces.removeAt(i);
    }

    // Spawn new pieces periodically
    constexpr int SPAWN_TICKS = 20;      // ~0.8 seconds at 40ms
    constexpr int MAX_PIECES   = 6;

    m_spawnTimer++;
    if (m_spawnTimer >= SPAWN_TICKS && m_fallingPieces.size() < MAX_PIECES) {
        // Randomise next interval to avoid regular cadence
        m_spawnTimer = 0;
        m_spawnTimer -= QRandomGenerator::global()->bounded(0, 8);   // jitter

        FallingPiece fp;
        int idx = QRandomGenerator::global()->bounded(1, 8);   // 1..7
        fp.shape      = SHAPES[idx];
        fp.colorIndex = idx;
        fp.x          = QRandomGenerator::global()->bounded(0, COLS - int(fp.shape[0].size()) + 1);
        // Start above the board or at a random visible row
        fp.y          = QRandomGenerator::global()->bounded(-int(fp.shape.size()), ROWS / 2);
        fp.speed      = 0.04 + QRandomGenerator::global()->bounded(0.07);   // 0.04 – 0.11

        m_fallingPieces.append(fp);
    }
}

void TetrisBoard::drawFallingPieces(QPainter &painter)
{
    painter.save();
    painter.setOpacity(0.60);   // ghostly appearance

    for (const auto &fp : m_fallingPieces) {
        const QColor color = COLORS[fp.colorIndex];
        for (int row = 0; row < fp.shape.size(); ++row) {
            for (int col = 0; col < fp.shape[row].size(); ++col) {
                if (fp.shape[row][col] > 0) {
                    int px = static_cast<int>((fp.x + col) * BLOCK_SIZE);
                    int py = static_cast<int>((fp.y + row) * BLOCK_SIZE);
                    painter.fillRect(px, py, BLOCK_SIZE, BLOCK_SIZE, color);
                    painter.setPen(QPen(color, 1));
                    painter.drawRect(px, py, BLOCK_SIZE - 1, BLOCK_SIZE - 1);
                }
            }
        }
    }

    painter.restore();
}
