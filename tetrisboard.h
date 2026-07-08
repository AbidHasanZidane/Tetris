#ifndef TETRISBOARD_H
#define TETRISBOARD_H

#include <QWidget>
#include <QTimer>
#include "tetrisgame.h"

class NextPieceWidget;

class TetrisBoard : public QWidget
{
    Q_OBJECT

public:
    explicit TetrisBoard(TetrisGame *game, NextPieceWidget *preview,
                         QWidget *parent = nullptr);

    void startGame();
    int  score() const { return m_game->score(); }
    int  level() const { return m_game->level(); }
    int  linesCleared() const { return m_game->linesCleared(); }

signals:
    void scoreUpdated(int score);
    void levelUpdated(int level);
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
    void drawGrid(QPainter &painter);
    void drawPauseOverlay(QPainter &painter);
    void drawStartOverlay(QPainter &painter);
    void drawGameOverOverlay(QPainter &painter);

    TetrisGame *m_game;
    NextPieceWidget *m_preview;
    QTimer *m_timer;
};

#endif // TETRISBOARD_H
