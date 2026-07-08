#ifndef SCOREDATABASE_H
#define SCOREDATABASE_H

#include <QObject>
#include <QVector>
#include <QDateTime>

struct ScoreEntry {
    int score;
    int level;
    int lines;
    QDateTime date;
};

class ScoreDatabase : public QObject
{
    Q_OBJECT

public:
    explicit ScoreDatabase(QObject *parent = nullptr);

    bool init();
    bool addScore(int score, int level, int lines);
    QVector<ScoreEntry> topScores(int limit = 20) const;
    void clearAll();

private:
    QString m_dbPath;
};

#endif // SCOREDATABASE_H
