#include "AudioManager.h"
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>

// ---------------------------------------------------------------------------
// Expected file names inside the sound directory
// ---------------------------------------------------------------------------
static const QString BG_MUSIC_FILE   = QStringLiteral("bg_music.mp3");
static const QString SOFT_DROP_FILE  = QStringLiteral("soft_drop.wav");
static const QString HARD_DROP_FILE  = QStringLiteral("hard_drop.mp3");
static const QString ROW_CLEAR_FILE  = QStringLiteral("row_clear.wav");
static const QString GAME_OVER_FILE  = QStringLiteral("game_over.wav");



AudioManager::AudioManager(const QString &soundDir, QObject *parent)
    : QObject(parent)
    , m_soundDir(soundDir)
{
    loadSounds();
}

AudioManager::~AudioManager()
{
    // Qt parent-child ownership cleans up the player/effects automatically,
    // but we stop playback explicitly so no dangling audio callbacks fire.
    if (m_bgMusic)
        m_bgMusic->stop();
}

// ---------------------------------------------------------------------------
// Load all sound files; set m_hasAudio if anything loaded successfully.
// ---------------------------------------------------------------------------

void AudioManager::loadSounds()
{
    QDir dir(m_soundDir);

    // --- Background music (QMediaPlayer + QAudioOutput) ---
    QString bgPath = dir.filePath(BG_MUSIC_FILE);
    if (QFileInfo::exists(bgPath)) {
        m_bgMusic = new QMediaPlayer(this);
        m_audioOutput = new QAudioOutput(this);
        m_audioOutput->setVolume(0.4);   // background music at 40 %
        m_bgMusic->setAudioOutput(m_audioOutput);
        m_bgMusic->setSource(QUrl::fromLocalFile(bgPath));
        m_bgMusic->setLoops(QMediaPlayer::Infinite);
        m_hasAudio = true;
    } else {
        qDebug() << "AudioManager: background music not found at" << bgPath;
    }

    // --- SFX (QSoundEffect — low latency, WAV only) ---

    auto loadSfx = [&](const QString &fileName, QSoundEffect *&target, qreal vol = 1.0) {
        QString path = dir.filePath(fileName);
        if (QFileInfo::exists(path)) {
            target = new QSoundEffect(this);
            target->setSource(QUrl::fromLocalFile(path));
            target->setVolume(vol);
            m_hasAudio = true;
        } else {
            qDebug() << "AudioManager: SFX not found at" << path;
        }
    };

    loadSfx(SOFT_DROP_FILE, m_softDropSfx, 0.7);
    loadSfx(HARD_DROP_FILE, m_hardDropSfx, 0.8);
    loadSfx(ROW_CLEAR_FILE, m_rowClearSfx, 0.9);
    loadSfx(GAME_OVER_FILE, m_gameOverSfx, 1.0);
}

// ---------------------------------------------------------------------------
// Mute control
// ---------------------------------------------------------------------------

void AudioManager::setMuted(bool muted)
{
    m_muted = muted;
    if (m_bgMusic) {
        if (muted)
            m_bgMusic->pause();
        else if (m_bgMusic->playbackState() != QMediaPlayer::PlayingState)
            m_bgMusic->play();
    }
    // SFX are fire-and-forget — they simply won't play when muted
}

// ---------------------------------------------------------------------------
// Background music
// ---------------------------------------------------------------------------

void AudioManager::playBackgroundMusic()
{
    if (m_muted || !m_bgMusic) return;
    m_bgMusic->play();
}

void AudioManager::stopBackgroundMusic()
{
    if (m_bgMusic)
        m_bgMusic->stop();
}

// ---------------------------------------------------------------------------
// Sound effects
// ---------------------------------------------------------------------------

void AudioManager::playSoftDrop()
{
    if (m_muted || !m_softDropSfx) return;
    m_softDropSfx->play();
}

void AudioManager::playHardDrop()
{
    if (m_muted || !m_hardDropSfx) return;
    m_hardDropSfx->play();
}

void AudioManager::playRowClear(int /*count*/)
{
    if (m_muted || !m_rowClearSfx) return;
    m_rowClearSfx->play();
}

void AudioManager::playGameOver()
{
    if (m_muted || !m_gameOverSfx) return;
    // Stop the music first so the game-over sound stands out
    stopBackgroundMusic();
    m_gameOverSfx->play();
}
