#ifndef NEXTPIECEWIDGET_H
#define NEXTPIECEWIDGET_H

#include <QWidget>
#include "tetrispiece.h"

class NextPieceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NextPieceWidget(QWidget *parent = nullptr);

    void setPiece(const TetrisPiece &piece);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    TetrisPiece m_piece;
    bool m_hasPiece = false;
};

#endif // NEXTPIECEWIDGET_H
