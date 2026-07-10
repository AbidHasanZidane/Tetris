#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include <QString>

/// Encapsulates all game audio — background music + SFX.
/// Silently degrades if sound files are missing, so the game
/// works with or without the sounds/ folder.
class AudioManager : public QObject
{
    Q_OBJECT

public:
    explicit AudioManager(const QString &soundDir,
                          QObject *parent = nullptr);
    ~AudioManager() override;

    bool hasAudio() const { return m_hasAudio; }

public slots:
    void setMuted(bool muted);
    bool isMuted() const { return m_muted; }

    // --- Background music ---
    void playBackgroundMusic();
    void stopBackgroundMusic();

    // --- Sound effects ---
    void playSoftDrop();
    void playHardDrop();
    void playRowClear(int count);   // count = number of lines cleared
    void playGameOver();

private:
    void loadSounds();

    QString m_soundDir;
    bool m_muted = false;
    bool m_hasAudio = false;        // true if at least one file loaded

    // Background music (supports mp3/ogg/wav)
    QMediaPlayer *m_bgMusic = nullptr;
    QAudioOutput *m_audioOutput = nullptr;

    // SFX players (low-latency, designed for rapid game sounds)
    QSoundEffect *m_softDropSfx  = nullptr;
    QSoundEffect *m_hardDropSfx  = nullptr;
    QSoundEffect *m_rowClearSfx  = nullptr;
    QSoundEffect *m_gameOverSfx  = nullptr;
};

#endif // AUDIOMANAGER_H
