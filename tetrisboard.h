#ifndef TETRISBOARD_H
#define TETRISBOARD_H

#include <QWidget>
#include <QTimer>
#include "tetrisgame.h"

class NextPieceWidget;

struct FallingPiece {
    QVector<QVector<int>> shape;
    qreal x = 0;
    qreal y = 0;
    int colorIndex = 0;
    qreal speed = 0.06;   // blocks per tick
};

class TetrisBoard : public QWidget
{
    Q_OBJECT

public:
    explicit TetrisBoard(TetrisGame *game, NextPieceWidget *preview,
                         QWidget *parent = nullptr);

    void startGame();
    int  score() const { return m_game->score(); }
    int  linesCleared() const { return m_game->linesCleared(); }

signals:
    void scoreUpdated(int score);
    void gameEnded(int finalScore);
    void pauseToggled(bool paused);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onTick();
    void onGameOver(int finalScore);

private:
    void drawBlock(QPainter &painter, int col, int row, const QColor &color);
    void drawBoard(QPainter &painter);
    void drawPiece(QPainter &painter);
    void drawGhostPiece(QPainter &painter);
    void drawBorder(QPainter &painter);
    void drawGrid(QPainter &painter);
    void drawPauseOverlay(QPainter &painter);
    void drawStartOverlay(QPainter &painter);
    void drawGameOverOverlay(QPainter &painter);
    void updateFallingPieces();
    void drawFallingPieces(QPainter &painter);

    TetrisGame *m_game;
    NextPieceWidget *m_preview;
    QTimer *m_timer;
    QTimer *m_fadeTimer;
    qreal m_fadeAngle = 0;
    QVector<FallingPiece> m_fallingPieces;
    int m_spawnTimer = 0;
};

#endif // TETRISBOARD_H
