#ifndef LEADERBOARDDIALOG_H
#define LEADERBOARDDIALOG_H

#include <QWidget>

class ScoreDatabase;
class QTableWidget;

/// Inline leaderboard widget (no longer a separate dialog).
/// Emits backClicked() so the main window can hide it and restore
/// the normal game-info panel.
class LeaderboardDialog : public QWidget
{
    Q_OBJECT

public:
    explicit LeaderboardDialog(ScoreDatabase *db, QWidget *parent = nullptr);

    void refresh();

signals:
    void backClicked();

private:
    ScoreDatabase *m_db;
    QTableWidget *m_table;
};

#endif // LEADERBOARDDIALOG_H
