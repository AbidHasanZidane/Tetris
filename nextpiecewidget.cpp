#include "nextpiecewidget.h"
#include <QPainter>
#include <QPainterPath>

NextPieceWidget::NextPieceWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(NEXT_COLS * BLOCK_SIZE, NEXT_ROWS * BLOCK_SIZE);
    setStyleSheet("background-color: #2a2a2a; border: 2px solid #555; border-radius: 4px;");
}

void NextPieceWidget::setPiece(const TetrisPiece &piece)
{
    m_piece = piece;
    m_hasPiece = true;
    update();
}

void NextPieceWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    if (!m_hasPiece) return;

    const QVector<QVector<int>> &shape = m_piece.shape;
    int colorIdx = m_piece.colorIndex;
    QColor color = COLORS[colorIdx];

    // Centre the piece inside the preview widget
    int offsetX = (NEXT_COLS - int(shape[0].size())) / 2;
    int offsetY = (NEXT_ROWS - int(shape.size())) / 2;

    for (int y = 0; y < shape.size(); ++y) {
        for (int x = 0; x < shape[y].size(); ++x) {
            if (shape[y][x] > 0) {
                int px = (offsetX + x) * BLOCK_SIZE;
                int py = (offsetY + y) * BLOCK_SIZE;

                // Filled block
                painter.fillRect(px, py, BLOCK_SIZE, BLOCK_SIZE, color);

                // Border
                painter.setPen(QColor("#444"));
                painter.drawRect(px, py, BLOCK_SIZE - 1, BLOCK_SIZE - 1);

                // Highlight (top-left bevel)
                painter.setPen(QPen(color.lighter(150), 2));
                painter.drawLine(px, py, px + BLOCK_SIZE - 1, py);
                painter.drawLine(px, py, px, py + BLOCK_SIZE - 1);
            }
        }
    }
}
