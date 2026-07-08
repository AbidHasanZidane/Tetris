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
            padding: 6px 10px;
            font-size: 13px;
            font-weight: bold;
            min-width: 100px;
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
    rightLayout->addSpacing(15);   // push the panel down

    // Next piece group
    QGroupBox *nextGroup = new QGroupBox("Next Piece");
    QVBoxLayout *nextLayout = new QVBoxLayout(nextGroup);
    nextLayout->addWidget(preview, 0, Qt::AlignCenter);
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

    // Buttons
    QPushButton *startBtn = new QPushButton("Start Game");
    startBtn->setFocusPolicy(Qt::NoFocus);
    rightLayout->addWidget(startBtn);

    // --- Mode toggle (Regular / Extreme) ---
    QPushButton *modeBtn = new QPushButton("Mode: Regular");
    modeBtn->setObjectName("modeBtn");
    modeBtn->setFocusPolicy(Qt::NoFocus);
    rightLayout->addWidget(modeBtn);

    // --- Sound toggle ---
    QPushButton *soundBtn = new QPushButton("Sound: ON");
    soundBtn->setObjectName("soundBtn");
    soundBtn->setFocusPolicy(Qt::NoFocus);
    rightLayout->addWidget(soundBtn);

    // --- Leaderboard button ---
    QPushButton *leaderboardBtn = new QPushButton("Leaderboard");
    leaderboardBtn->setObjectName("leaderboardBtn");
    leaderboardBtn->setFocusPolicy(Qt::NoFocus);
    rightLayout->addWidget(leaderboardBtn);

    // How to Play toggle
    QPushButton *helpBtn = new QPushButton("How to Play");
    helpBtn->setObjectName("helpBtn");
    helpBtn->setFocusPolicy(Qt::NoFocus);
    helpBtn->setStyleSheet(R"(
        QPushButton#helpBtn {
            background-color: #1565c0;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 10px;
            font-size: 12px;
            font-weight: bold;
            min-width: 100px;
        }
        QPushButton#helpBtn:hover {
            background-color: #1976d2;
        }
        QPushButton#helpBtn:pressed {
            background-color: #0d47a1;
        }
        QPushButton#helpBtn:disabled {
            background-color: #555;
            color: #999;
        }
    )");

    QLabel *controlsLabel = new QLabel(
        QString::fromUtf8("← →   Move\n"
                          "↑      Rotate\n"
                          "↓      Soft Drop\n"
                          "Space  Hard Drop\n"
                          "Esc  Pause"));
    controlsLabel->setStyleSheet("color: #888; font-size: 11px;");
    controlsLabel->setAlignment(Qt::AlignCenter);
    controlsLabel->setVisible(false);

    QObject::connect(helpBtn, &QPushButton::clicked, [controlsLabel]() {
        controlsLabel->setVisible(!controlsLabel->isVisible());
    });

    rightLayout->addWidget(helpBtn);
    rightLayout->addWidget(controlsLabel);

    rightLayout->addStretch();
    mainLayout->addLayout(rightLayout);

    // ------------------------------------------------------------------
    // Connections
    // ------------------------------------------------------------------
    QObject::connect(board, &TetrisBoard::scoreUpdated, scoreLabel,
                     QOverload<int>::of(&QLabel::setNum));

    QObject::connect(board, &TetrisBoard::gameEnded, startBtn, [startBtn, modeBtn](int /*score*/) {
        startBtn->setText("Retry");
        startBtn->setEnabled(true);
        modeBtn->setEnabled(true);
    });

    QObject::connect(board, &TetrisBoard::gameEnded, [board, scoreDb](int /*score*/) {
        int s = board->score();
        if (s > 0)
            scoreDb->addScore(s, board->level(), board->linesCleared());
    });

    QObject::connect(board, &TetrisBoard::pauseToggled,
                     [startBtn, leaderboardBtn, modeBtn, soundBtn](bool paused) {
        startBtn->setEnabled(false);
        leaderboardBtn->setEnabled(!paused);
        modeBtn->setEnabled(!paused);
        soundBtn->setEnabled(!paused);
    });

    QObject::connect(startBtn, &QPushButton::clicked, [board, startBtn, modeBtn]() {
        board->startGame();
        startBtn->setText("Playing...");
        startBtn->setEnabled(false);
        modeBtn->setEnabled(false);
    });

    // Mode toggle: Regular ↔ Extreme
    QObject::connect(modeBtn, &QPushButton::clicked, [modeBtn, game]() {
        static bool extreme = false;
        extreme = !extreme;
        if (extreme) {
            modeBtn->setText("Mode: Extreme");
            modeBtn->setStyleSheet(R"(
                QPushButton { background-color: #333; color: #FF0D72; border: 1px solid #555;
                    border-radius: 4px; padding: 6px 10px; font-size: 13px; font-weight: bold;
                    min-width: 100px; }
                QPushButton:hover { background-color: #444; }
                QPushButton:disabled { background-color: #555; color: #999; }
            )");
            game->setInitialSpeed(Difficulty::Extreme);
            game->setExtremeMode(true);
        } else {
            modeBtn->setText("Mode: Regular");
            modeBtn->setStyleSheet(R"(
                QPushButton { background-color: #333; color: #4caf50; border: 1px solid #555;
                    border-radius: 4px; padding: 6px 10px; font-size: 13px; font-weight: bold;
                    min-width: 100px; }
                QPushButton:hover { background-color: #444; }
                QPushButton:disabled { background-color: #555; color: #999; }
            )");
            game->setInitialSpeed(Difficulty::Regular);
            game->setExtremeMode(false);
        }
    });

    // Sound toggle (dummy — no actual audio)
    QObject::connect(soundBtn, &QPushButton::clicked, [soundBtn]() {
        static bool muted = false;
        muted = !muted;
        if (muted) {
            soundBtn->setText("Sound: OFF");
            soundBtn->setStyleSheet(R"(
                QPushButton { background-color: #333; color: #888; border: 1px solid #555;
                    border-radius: 4px; padding: 6px 10px; font-size: 13px; font-weight: bold;
                    min-width: 100px; }
                QPushButton:hover { background-color: #444; }
                QPushButton:disabled { background-color: #555; color: #999; }
            )");
        } else {
            soundBtn->setText("Sound: ON");
            soundBtn->setStyleSheet(R"(
                QPushButton { background-color: #333; color: #4caf50; border: 1px solid #555;
                    border-radius: 4px; padding: 6px 10px; font-size: 13px; font-weight: bold;
                    min-width: 100px; }
                QPushButton:hover { background-color: #444; }
                QPushButton:disabled { background-color: #555; color: #999; }
            )");
        }
    });

    QObject::connect(leaderboardBtn, &QPushButton::clicked, [leaderboardDlg]() {
        leaderboardDlg->refresh();
        leaderboardDlg->exec();
    });

    // ------------------------------------------------------------------
    window.show();
    return app.exec();
}
