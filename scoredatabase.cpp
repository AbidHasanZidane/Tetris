#include "scoredatabase.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

ScoreDatabase::ScoreDatabase(QObject *parent)
    : QObject(parent)
{
}

bool ScoreDatabase::init()
{
    // Store the database in the app's local data folder
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    m_dbPath = dataDir + "/scores.db";

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_dbPath);

    if (!db.open()) {
        qWarning() << "Failed to open score database:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS scores ("
        "  id    INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  score INTEGER NOT NULL,"
        "  level INTEGER NOT NULL,"
        "  lines INTEGER NOT NULL,"
        "  date  TEXT    NOT NULL"
        ")"
    );
    if (!ok) {
        qWarning() << "Failed to create scores table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Score database opened at" << m_dbPath;
    return true;
}

bool ScoreDatabase::addScore(int score, int level, int lines)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO scores (score, level, lines, date) "
        "VALUES (:score, :level, :lines, :date)"
    );
    query.bindValue(":score", score);
    query.bindValue(":level", level);
    query.bindValue(":lines", lines);
    query.bindValue(":date", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        qWarning() << "Failed to insert score:" << query.lastError().text();
        return false;
    }
    return true;
}

QVector<ScoreEntry> ScoreDatabase::topScores(int limit) const
{
    QVector<ScoreEntry> results;

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare(
        "SELECT score, level, lines, date "
        "FROM scores "
        "ORDER BY score DESC "
        "LIMIT :limit"
    );
    query.bindValue(":limit", limit);

    if (!query.exec()) {
        qWarning() << "Failed to fetch scores:" << query.lastError().text();
        return results;
    }

    while (query.next()) {
        ScoreEntry entry;
        entry.score = query.value(0).toInt();
        entry.level = query.value(1).toInt();
        entry.lines = query.value(2).toInt();
        entry.date  = QDateTime::fromString(query.value(3).toString(), Qt::ISODate);
        results.append(entry);
    }
    return results;
}

void ScoreDatabase::clearAll()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if (!query.exec("DELETE FROM scores")) {
        qWarning() << "Failed to clear scores:" << query.lastError().text();
    }
}
