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
#include "AudioManager.h"

// This is a comment
// This is a comment
// This is a comment
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Tetris");
    app.setStyleSheet(R"(
        QMainWindow {
            background-color: #000000;
        }
        QGroupBox {
            color: #ccc;
            font-size: 16px;
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
            color: #4caf50;
            border: none;
            border-radius: 4px;
            padding: 8px 14px;
            font-size: 16px;
            font-weight: bold;
            min-width: 100px;
            text-align: left;
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
    )");

    // ------------------------------------------------------------------
    // Model
    // ------------------------------------------------------------------
    TetrisGame *game = new TetrisGame;

    // ------------------------------------------------------------------
    // Audio
    // ------------------------------------------------------------------
    AudioManager *audio = new AudioManager(
        QApplication::applicationDirPath() + QStringLiteral("/sounds"));

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

    QWidget *centralWidget = new QWidget;
    centralWidget->setStyleSheet("background-color: #000000;");
    window.setCentralWidget(centralWidget);

    // Outer vertical layout — centers the game area vertically
    QVBoxLayout *outerVLayout = new QVBoxLayout(centralWidget);
    outerVLayout->setContentsMargins(0, 0, 0, 0);

    // Inner horizontal layout — centers board + right panel horizontally
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(40);   // gap between board and right panel

    // -- Left side: game board ------------------------------------------
    NextPieceWidget *preview = new NextPieceWidget;
    TetrisBoard *board = new TetrisBoard(game, preview);

    mainLayout->addStretch();

    // How to Play box — left of board, top-aligned
    QWidget *howToPlayBox = new QWidget;
    QVBoxLayout *htpLayout = new QVBoxLayout(howToPlayBox);
    htpLayout->setContentsMargins(0, 20, 16, 0);
    htpLayout->setSpacing(0);
    QLabel *htpLabel = new QLabel(
        QString::fromUtf8(
            "A: Left\n"
            "D: Right\n"
            "W: Rotate\n"
            "S: Soft Drop\n"
            "Space:  Hard Drop\n"
            "Esc: Pause"));
    htpLabel->setStyleSheet("color: #888; background: transparent; font-size: 16px;");
    htpLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    htpLayout->addWidget(htpLabel);
    htpLayout->addStretch();
    mainLayout->addWidget(howToPlayBox);

    mainLayout->addWidget(board);

    // -- Right side: info panel -----------------------------------------
    // Fixed-width container prevents layout shifts when switching panels
    QWidget *rightPanel = new QWidget;
    rightPanel->setFixedWidth(190);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setSpacing(10);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    // Container for the normal game-info widgets (toggled with leaderboard)
    QWidget *infoPanel = new QWidget;
    QVBoxLayout *infoLayout = new QVBoxLayout(infoPanel);
    infoLayout->setSpacing(10);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->addSpacing(20);   // push the panel down

    // Next piece group
    QGroupBox *nextGroup = new QGroupBox("Next Piece");
    nextGroup->setStyleSheet("QGroupBox::title { color: #4caf50; }");
    nextGroup->setStyleSheet(
        "QGroupBox { margin-left: 10px; margin-right: 2px; }"
        "QGroupBox::title { color: #4caf50; }");
    QVBoxLayout *nextLayout = new QVBoxLayout(nextGroup);
    nextLayout->addWidget(preview, 0, Qt::AlignCenter);
    infoLayout->addWidget(nextGroup);

    // Score
    QGroupBox *scoreGroup = new QGroupBox("Score");
    scoreGroup->setStyleSheet("QGroupBox::title { color: #4caf50; }");
    scoreGroup->setStyleSheet(
        "QGroupBox { margin-left: 10px; margin-right: 2px; }"
        "QGroupBox::title { color: #4caf50; }");
    QVBoxLayout *scoreLayout = new QVBoxLayout(scoreGroup);
    QLabel *scoreLabel = new QLabel("0");
    scoreLabel->setFont(QFont("Courier New", 28, QFont::Black));
    scoreLabel->setStyleSheet("color: #888;");
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLayout->addWidget(scoreLabel);
    infoLayout->addWidget(scoreGroup);

    // Buttons
    QPushButton *startBtn = new QPushButton("Start Game");
    startBtn->setFocusPolicy(Qt::NoFocus);
    infoLayout->addWidget(startBtn);

    // --- Mode toggle (Regular / Extreme)
    QPushButton *modeBtn = new QPushButton("Mode: Regular");
    modeBtn->setObjectName("modeBtn");
    modeBtn->setFocusPolicy(Qt::NoFocus);
    infoLayout->addWidget(modeBtn);

    // --- Sound toggle ---
    QPushButton *soundBtn = new QPushButton("Sound: ON");
    soundBtn->setObjectName("soundBtn");
    soundBtn->setFocusPolicy(Qt::NoFocus);
    infoLayout->addWidget(soundBtn);

    // --- Leaderboard button ---
    QPushButton *leaderboardBtn = new QPushButton("Leaderboard");
    leaderboardBtn->setObjectName("leaderboardBtn");
    leaderboardBtn->setFocusPolicy(Qt::NoFocus);
    infoLayout->addWidget(leaderboardBtn);

    // --- Exit Game button ---
    QPushButton *exitBtn = new QPushButton("Exit Game");
    exitBtn->setFocusPolicy(Qt::NoFocus);
    infoLayout->addWidget(exitBtn);

    infoLayout->addStretch();

    // Add both panels to the right layout — only infoPanel visible initially
    rightLayout->addWidget(infoPanel);
    rightLayout->addWidget(leaderboardDlg);
    leaderboardDlg->setVisible(false);
    mainLayout->addWidget(rightPanel);
    mainLayout->addStretch();

    // Center the game area vertically on a full-screen black background
    outerVLayout->addStretch();
    outerVLayout->addLayout(mainLayout);
    outerVLayout->addStretch();

    // ------------------------------------------------------------------
    // Connections
    // ------------------------------------------------------------------
    // Sound effects
    QObject::connect(game, &TetrisGame::softDropDone,  audio, &AudioManager::playSoftDrop);
    QObject::connect(game, &TetrisGame::hardDropDone,   audio, &AudioManager::playHardDrop);
    QObject::connect(game, &TetrisGame::rowsDestroyed,  audio, &AudioManager::playRowClear);
    QObject::connect(game, &TetrisGame::gameOver,       audio, &AudioManager::playGameOver);

    QObject::connect(board, &TetrisBoard::scoreUpdated,
                     [scoreLabel](int s) { scoreLabel->setNum(s); });

    QObject::connect(board, &TetrisBoard::gameEnded, [startBtn, modeBtn, leaderboardBtn, audio](int /*score*/) {
        audio->stopBackgroundMusic();
        startBtn->setText("Retry");
        startBtn->setEnabled(true);
        modeBtn->setEnabled(true);
        leaderboardBtn->setEnabled(true);
    });

    QObject::connect(board, &TetrisBoard::gameEnded, [board, scoreDb](int /*score*/) {
        int s = board->score();
        if (s > 0)
            scoreDb->addScore(s, board->linesCleared());
    });

    QObject::connect(board, &TetrisBoard::pauseToggled,
                     [startBtn, soundBtn](bool paused) {
        soundBtn->setEnabled(!paused);
    });

    QObject::connect(startBtn, &QPushButton::clicked, [board, startBtn, modeBtn, leaderboardBtn, audio, infoPanel, leaderboardDlg]() {
        board->startGame();
        audio->playBackgroundMusic();
        // Ensure game-info panel is visible (in case leaderboard was open)
        leaderboardDlg->setVisible(false);
        infoPanel->setVisible(true);
        startBtn->setText("Retry");
        startBtn->setEnabled(true);
        modeBtn->setEnabled(false);
        leaderboardBtn->setEnabled(false);
    });

    // Mode toggle: Regular ↔ Extreme
    QObject::connect(modeBtn, &QPushButton::clicked, [modeBtn, game]() {
        static bool extreme = false;
        extreme = !extreme;
        if (extreme) {
            modeBtn->setText("Mode: Extreme");
            game->setInitialSpeed(EXTREME_INITIAL_SPEED);
            game->setExtremeMode(true);
        } else {
            modeBtn->setText("Mode: Regular");
            game->setInitialSpeed(REGULAR_INITIAL_SPEED);
            game->setExtremeMode(false);
        }
    });

    // Sound toggle
    QObject::connect(soundBtn, &QPushButton::clicked, [soundBtn, audio]() {
        static bool muted = false;
        muted = !muted;
        audio->setMuted(muted);
        if (muted) {
            soundBtn->setText("Sound: OFF");
        } else {
            soundBtn->setText("Sound: ON");
        }
    });

    // Exit Game button
    QObject::connect(exitBtn, &QPushButton::clicked, [&window]() {
        window.close();
    });

    // Leaderboard button — show leaderboard inline, hide game info
    QObject::connect(leaderboardBtn, &QPushButton::clicked, [leaderboardDlg, infoPanel]() {
        leaderboardDlg->refresh();
        infoPanel->setVisible(false);
        leaderboardDlg->setVisible(true);
    });

    // Back button on leaderboard — return to game info
    QObject::connect(leaderboardDlg, &LeaderboardDialog::backClicked, [leaderboardDlg, infoPanel]() {
        leaderboardDlg->setVisible(false);
        infoPanel->setVisible(true);
    });

    // ------------------------------------------------------------------
    window.showFullScreen();
    return app.exec();
}
