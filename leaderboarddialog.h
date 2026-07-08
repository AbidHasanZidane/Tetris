#ifndef LEADERBOARDDIALOG_H
#define LEADERBOARDDIALOG_H

#include <QDialog>

class ScoreDatabase;
class QTableWidget;

class LeaderboardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LeaderboardDialog(ScoreDatabase *db, QWidget *parent = nullptr);

    void refresh();

private:
    ScoreDatabase *m_db;
    QTableWidget *m_table;
};

#endif // LEADERBOARDDIALOG_H
