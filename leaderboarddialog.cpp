#include "leaderboarddialog.h"
#include "scoredatabase.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

LeaderboardDialog::LeaderboardDialog(ScoreDatabase *db, QWidget *parent)
    : QWidget(parent)
    , m_db(db)
{
    setStyleSheet("background-color: #1e1e1e;");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addSpacing(20);   // align with board / next-piece group

    // Title row: "Top Scores" label + Back button
    auto *titleRow = new QHBoxLayout;

    auto *title = new QLabel("");
    title->setFont(QFont("Arial", 16, QFont::Bold));
    title->setStyleSheet("color: #4caf50; background: transparent;");

    auto *backBtn = new QPushButton("X");
    backBtn->setFocusPolicy(Qt::NoFocus);
    backBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: #4caf50;
            border: none;
            font-size: 16px;
            font-weight: bold;
            padding: 0px;
            min-width: 20px;
            max-width: 20px;
            max-height: 20px;
        }
        QPushButton:hover {
            color: #66bb6a;
        }
    )");
    connect(backBtn, &QPushButton::clicked, this, &LeaderboardDialog::backClicked);

    titleRow->addWidget(title);
    titleRow->addStretch();
    titleRow->addWidget(backBtn);
    layout->addLayout(titleRow);

    // Table
    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"#", "Score", "Lines"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    m_table->setFocusPolicy(Qt::NoFocus);
    m_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet(R"(
        QTableWidget {
            color: #ccc;
            background-color: #2a2a2a;
            border: 1px solid #444;
            gridline-color: #444;
            font-size: 14px;
        }
        QTableWidget::item {
            padding: 4px 8px;
        }
        QHeaderView::section {
            background-color: #333;
            color: #4caf50;
            font-weight: bold;
            border: 1px solid #444;
            padding: 6px;
        }
        QTableWidget::item:alternate {
            background-color: #252525;
        }
    )");
    layout->addWidget(m_table);

    refresh();
}

void LeaderboardDialog::refresh()
{
    auto scores = m_db->topScores(15);
    m_table->setRowCount(scores.size());

    for (int i = 0; i < scores.size(); ++i) {
        const auto &entry = scores[i];

        auto *rankItem = new QTableWidgetItem(QString::number(i + 1));
        rankItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 0, rankItem);

        auto *scoreItem = new QTableWidgetItem(QString::number(entry.score));
        scoreItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 1, scoreItem);

        auto *linesItem = new QTableWidgetItem(QString::number(entry.lines));
        linesItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 2, linesItem);
    }
}
