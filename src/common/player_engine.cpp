#include "config.h"

#include "player_engine.h"
#include "playlist_model.h"

#include "mpv_proxy.h"
#ifdef ENABLE_VPU_PLATFORM
#include "vpu_proxy.h"
#endif

namespace dmr {

PlayerEngine::PlayerEngine(QWidget *parent)
    :QWidget(parent)
{
    auto *l = new QVBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);

    //FIXME: need to dynamically change this
#ifdef ENABLE_VPU_PLATFORM
    _current = new VpuProxy(this);
#else
    _current = new MpvProxy(this);
#endif
    if (_current) {
        connect(_current, &Backend::stateChanged, this, &PlayerEngine::onBackendStateChanged);
        connect(_current, &Backend::tracksChanged, this, &PlayerEngine::tracksChanged);
        connect(_current, &Backend::elapsedChanged, this, &PlayerEngine::elapsedChanged);
        connect(_current, &Backend::fileLoaded, this, &PlayerEngine::fileLoaded);
        connect(_current, &Backend::muteChanged, this, &PlayerEngine::muteChanged);
        connect(_current, &Backend::volumeChanged, this, &PlayerEngine::volumeChanged);
        connect(_current, &Backend::sidChanged, this, &PlayerEngine::sidChanged);
        connect(_current, &Backend::aidChanged, this, &PlayerEngine::aidChanged);
        connect(_current, &Backend::notifyScreenshot, this, &PlayerEngine::notifyScreenshot);
        l->addWidget(_current);
    }

    setLayout(l);


    _playlist = new PlaylistModel(this);
}

PlayerEngine::~PlayerEngine()
{
    disconnect(_playlist, 0, 0, 0);
    delete _playlist;
    _playlist = nullptr;

    if (_current) {
        disconnect(_current, 0, 0, 0);
        delete _current;
        _current = nullptr;
    }
    qDebug() << __func__;
}

void PlayerEngine::onBackendStateChanged()
{
    if (!_current) return;

    switch (_current->state()) {
        case Backend::PlayState::Playing:
            _state = CoreState::Playing;
            break;
        case Backend::PlayState::Paused:
            _state = CoreState::Paused;
            break;
        case Backend::PlayState::Stopped:
            _state = CoreState::Idle;
            break;
    }

    emit stateChanged();
}

const PlayingMovieInfo& PlayerEngine::playingMovieInfo()
{
    static PlayingMovieInfo empty;

    if (!_current) return empty;
    return _current->playingMovieInfo();
}

int PlayerEngine::aid() const
{
    if (state() == CoreState::Idle) { return 0; }
    if (!_current) return 0;

    return _current->aid();
}

int PlayerEngine::sid() const
{
    if (state() == CoreState::Idle) { return 0; }
    if (!_current) return 0;

    return _current->sid();
}

void PlayerEngine::loadSubtitle(const QFileInfo& fi)
{
    if (state() == CoreState::Idle) { return; }
    if (!_current) return;

    _current->loadSubtitle(fi);
}

void PlayerEngine::selectSubtitle(int id)
{
    if (!_current) return;
    _current->selectSubtitle(id);
}

bool PlayerEngine::isSubVisible()
{
    if (state() == CoreState::Idle) { return false; }
    if (!_current) return false;

    return _current->isSubVisible();
}

void PlayerEngine::toggleSubtitle()
{
    if (!_current) return;
    _current->toggleSubtitle();

}

void PlayerEngine::selectTrack(int id)
{
    if (!_current) return;
    _current->selectTrack(id);
}

void PlayerEngine::volumeUp()
{
    if (!_current) return;
    _current->volumeUp();
}

void PlayerEngine::changeVolume(int val)
{
    if (!_current) return;
    _current->changeVolume(val);
}

void PlayerEngine::volumeDown()
{
    if (!_current) return;
    _current->volumeDown();
}

int PlayerEngine::volume() const
{
    if (!_current) return 100;
    return _current->volume();
}

bool PlayerEngine::muted() const
{
    if (!_current) return false;
    return _current->muted();
}

void PlayerEngine::toggleMute()
{
    if (!_current) return;
    _current->toggleMute();
}

//FIXME: TODO: update _current according to file 
void PlayerEngine::requestPlay(int id)
{
    if (!_current) return;
    const auto& item = _playlist->items()[id];
    _current->setPlayFile(item.info.absoluteFilePath());
    if (_current->isPlayable()) {
        _current->play();
    } else {
        // TODO: delete and try next backend?
    }
}

void PlayerEngine::play()
{
    if (state() == CoreState::Idle) {
        next();
    }
}

void PlayerEngine::prev()
{
    _playlist->playPrev();
}

void PlayerEngine::next()
{
    _playlist->playNext();
}

void PlayerEngine::clearPlaylist()
{
    _playlist->clear();
}

void PlayerEngine::pauseResume()
{
    if (!_current) return;
    if (_state == CoreState::Idle)
        return;

    _current->pauseResume();
}

void PlayerEngine::stop()
{
    if (!_current) return;
    _current->stop();
}

bool PlayerEngine::paused()
{
    return _state == CoreState::Paused;
}

QImage PlayerEngine::takeScreenshot()
{
    return _current->takeScreenshot();
}

void PlayerEngine::burstScreenshot()
{
    _current->burstScreenshot();
}

void PlayerEngine::stopBurstScreenshot()
{
    _current->stopBurstScreenshot();
}

void PlayerEngine::seekForward(int secs)
{
    if (state() == CoreState::Idle) return;

    _current->seekForward(secs);
}

void PlayerEngine::seekBackward(int secs)
{
    if (state() == CoreState::Idle) return;

    _current->seekBackward(secs);
}

void PlayerEngine::addPlayFile(const QFileInfo& fi)
{
    _playlist->append(fi);
}

qint64 PlayerEngine::duration() const
{
    if (!_current) return 0;
    return _current->duration();
}


qint64 PlayerEngine::elapsed() const
{
    if (!_current) return 0;
    return _current->elapsed();
}

void PlayerEngine::setVideoAspect(double r)
{
    if (_current)
        _current->setVideoAspect(r);
}

double PlayerEngine::videoAspect() const
{
    if (!_current) return 0.0;
    return _current->videoAspect();
}

} // end of namespace dmr