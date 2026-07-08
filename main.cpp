#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>
#include <QFont>
#include <QMessageBox>

#include "tetrisgame.h"
#include "tetrisboard.h"
#include "nextpiecewidget.h"
#include "constants.h"
#include "scoredatabase.h"
#include "leaderboarddialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Tetris");
    app.setStyleSheet(R"(
        QMainWindow {
            background-color: #121212;
        }
        QGroupBox {
            color: #ccc;
            font-size: 14px;
            font-weight: bold;
            border: 2px solid #444;
            border-radius: 6px;
            margin-top: 12px;
            padding-top: 18px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 6px;
        }
        QLabel {
            color: #ccc;
        }
        QPushButton {
            background-color: #2e7d32;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: bold;
            min-width: 120px;
        }
        QPushButton:hover {
            background-color: #388e3c;
        }
        QPushButton:pressed {
            background-color: #1b5e20;
        }
        QPushButton:disabled {
            background-color: #555;
            color: #999;
        }
        QPushButton#leaderboardBtn {
            background-color: #1565c0;
        }
        QPushButton#leaderboardBtn:hover {
            background-color: #1976d2;
        }
        QPushButton#leaderboardBtn:pressed {
            background-color: #0d47a1;
        }
    )");

    // ------------------------------------------------------------------
    // Model
    // ------------------------------------------------------------------
    TetrisGame *game = new TetrisGame;

    // ------------------------------------------------------------------
    // Main window
    // ------------------------------------------------------------------
    QMainWindow window;

    // Database for high scores
    ScoreDatabase *scoreDb = new ScoreDatabase;
    if (!scoreDb->init()) {
        QMessageBox::warning(&window, "Warning",
                             "Could not open the high-score database.\n"
                             "Scores will not be saved.");
    }

    LeaderboardDialog *leaderboardDlg = new LeaderboardDialog(scoreDb, &window);
    window.setWindowTitle("Tetris Extreme");
    window.setFixedSize(520, 680);

    QWidget *centralWidget = new QWidget;
    window.setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(14);

    // -- Left side: game board ------------------------------------------
    NextPieceWidget *preview = new NextPieceWidget;
    TetrisBoard *board = new TetrisBoard(game, preview);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(board);
    mainLayout->addLayout(leftLayout);

    // -- Right side: info panel -----------------------------------------
    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->setSpacing(10);

    // Next piece group
    QGroupBox *nextGroup = new QGroupBox("Next Piece");
    QVBoxLayout *nextLayout = new QVBoxLayout(nextGroup);
    nextLayout->addWidget(preview);
    rightLayout->addWidget(nextGroup);

    // Score
    QGroupBox *scoreGroup = new QGroupBox("Score");
    QVBoxLayout *scoreLayout = new QVBoxLayout(scoreGroup);
    QLabel *scoreLabel = new QLabel("0");
    scoreLabel->setFont(QFont("Arial", 22, QFont::Bold));
    scoreLabel->setStyleSheet("color: #4caf50;");
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLayout->addWidget(scoreLabel);
    rightLayout->addWidget(scoreGroup);

    // Level
    QGroupBox *levelGroup = new QGroupBox("Level");
    QVBoxLayout *levelLayout = new QVBoxLayout(levelGroup);
    QLabel *levelLabel = new QLabel("1");
    levelLabel->setFont(QFont("Arial", 22, QFont::Bold));
    levelLabel->setStyleSheet("color: #ff9800;");
    levelLabel->setAlignment(Qt::AlignCenter);
    levelLayout->addWidget(levelLabel);
    rightLayout->addWidget(levelGroup);

    // Buttons
    QPushButton *startBtn = new QPushButton("Start Game");
    QPushButton *leaderboardBtn = new QPushButton("Leaderboard");
    leaderboardBtn->setObjectName("leaderboardBtn");
    rightLayout->addWidget(startBtn);
    rightLayout->addWidget(leaderboardBtn);

    // Controls help
    QLabel *controlsLabel = new QLabel(
        QString::fromUtf8("← →  Move\n"
                          "↑     Rotate\n"
                          "↓     Soft Drop\n"
                          "Space  Hard Drop\n"
                          "P      Pause"));
    controlsLabel->setStyleSheet("color: #888; font-size: 11px;");
    controlsLabel->setAlignment(Qt::AlignCenter);
    rightLayout->addSpacing(8);
    rightLayout->addWidget(controlsLabel);

    rightLayout->addStretch();
    mainLayout->addLayout(rightLayout);

    // ------------------------------------------------------------------
    // Connections
    // ------------------------------------------------------------------
    QObject::connect(board, &TetrisBoard::scoreUpdated, scoreLabel,
                     QOverload<int>::of(&QLabel::setNum));
    QObject::connect(board, &TetrisBoard::levelUpdated, levelLabel,
                     QOverload<int>::of(&QLabel::setNum));

    QObject::connect(board, &TetrisBoard::gameEnded, startBtn, [startBtn](int /*score*/) {
        startBtn->setText("Retry");
        startBtn->setEnabled(true);
    });

    QObject::connect(board, &TetrisBoard::gameEnded, [board, scoreDb](int /*score*/) {
        int s = board->score();
        if (s > 0)
            scoreDb->addScore(s, board->level(), board->linesCleared());
    });

    QObject::connect(board, &TetrisBoard::pauseToggled, [startBtn, leaderboardBtn](bool paused) {
        startBtn->setEnabled(false);          // always disabled while game is active
        leaderboardBtn->setEnabled(!paused);  // disabled during pause
    });

    QObject::connect(startBtn, &QPushButton::clicked, [board, startBtn]() {
        board->startGame();
        startBtn->setText("Playing...");
        startBtn->setEnabled(false);
    });

    QObject::connect(leaderboardBtn, &QPushButton::clicked, [leaderboardDlg]() {
        leaderboardDlg->refresh();
        leaderboardDlg->exec();
    });

    // ------------------------------------------------------------------
    window.show();
    return app.exec();
}
