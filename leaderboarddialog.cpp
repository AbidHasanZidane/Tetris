#include "leaderboarddialog.h"
#include "scoredatabase.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QDateTime>

LeaderboardDialog::LeaderboardDialog(ScoreDatabase *db, QWidget *parent)
    : QDialog(parent)
    , m_db(db)
{
    setWindowTitle("Leaderboard");
    setFixedSize(480, 420);
    setStyleSheet("background-color: #1e1e1e;");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Title
    auto *title = new QLabel("Top Scores");
    title->setAlignment(Qt::AlignCenter);
    title->setFont(QFont("Arial", 18, QFont::Bold));
    title->setStyleSheet("color: #4caf50;");
    layout->addWidget(title);

    // Table
    m_table = new QTableWidget(0, 5, this);
    m_table->setHorizontalHeaderLabels({"#", "Score", "Level", "Lines", "Date"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    m_table->setFocusPolicy(Qt::NoFocus);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet(R"(
        QTableWidget {
            color: #ccc;
            background-color: #2a2a2a;
            border: 1px solid #444;
            gridline-color: #444;
            font-size: 13px;
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
    auto scores = m_db->topScores(50);
    m_table->setRowCount(scores.size());

    for (int i = 0; i < scores.size(); ++i) {
        const auto &entry = scores[i];

        auto *rankItem = new QTableWidgetItem(QString::number(i + 1));
        rankItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 0, rankItem);

        auto *scoreItem = new QTableWidgetItem(QString::number(entry.score));
        scoreItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 1, scoreItem);

        auto *levelItem = new QTableWidgetItem(QString::number(entry.level));
        levelItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 2, levelItem);

        auto *linesItem = new QTableWidgetItem(QString::number(entry.lines));
        linesItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 3, linesItem);

        QString dateStr = entry.date.toString("yyyy-MM-dd  hh:mm");
        auto *dateItem = new QTableWidgetItem(dateStr);
        dateItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 4, dateItem);
    }
}
