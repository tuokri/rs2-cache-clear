#pragma once

#include <filesystem>
#include <string>
#include <sstream>
#include <utility>

#include <QObject>
#include <QQueue>
#include <QDebug>

#include "Utils.hpp"

namespace fs = std::filesystem;

class Worker : public QObject
{
Q_OBJECT

public:
    Worker() = default;

    ~Worker() override = default;

public slots:
    virtual void
    doWork() = 0;

signals:
    void
    busy(bool);
};

class FindCacheWorker : public Worker
{
Q_OBJECT

public slots:
    void
    doWork() override
    {
        emit busy(true);

        fs::path path;
        try
        {
            path = Utils::getUserRS2ConfigDirPath();
            qDebug() << "user documents path: " << path.wstring();
        }
        catch (const std::runtime_error& e)
        {
            std::stringstream ss;
            ss << "Error: " << e.what() << "!";
            emit failure(QString::fromStdString(ss.str()));
            emit success(false);
            emit busy(false);
            return;
        }
        emit itemCountResult(Utils::countItemsInPath(path) + 1);
        emit documentsPathResult(QString::fromStdWString(path.wstring()));
        emit success(true);
        emit busy(false);
    };

signals:
    void
    itemCountResult(int);

    void
    documentsPathResult(QString);

    void
    failure(QString);

    void
    success(bool);
};

class ClearCacheWorker : public Worker
{
Q_OBJECT

public:
    ClearCacheWorker()
    : Worker(), _path(nullptr), _workDone(new QQueue<QString>()), _timer(new QTimer()),
    _mutex(new QMutex()), _progress(0), _timerThread(new QThread())
    {
        connect(this, SIGNAL(this->busy(bool)), this, SLOT(this->handleTimer(bool)));
        connect(_timer, SIGNAL(_timer->timeout()), this, SLOT(this->emitReadyWork()));
        _timer->moveToThread(_timerThread);
        _timer->start(5);
        _timerThread->start();
    };

    ~ClearCacheWorker() override
    {
        _timer->stop();
        _timerThread->quit();
        while (!_timerThread->isFinished());
        delete _timerThread;

        delete _timer;
        delete _mutex;
        delete _workDone;
    };

public slots:
    void
    doWork() override
    {
        emit busy(true);

        if (_path == nullptr)
        {
            return;
        }

        std::wstring pathStr;
        auto iter = fs::recursive_directory_iterator(*_path);
        for (const auto& item: iter)
        {
            // fs::remove(item);
            _mutex->lock();
            _workDone->enqueue(QString::fromStdWString(item.path().wstring()));
            _mutex->unlock();
            QCoreApplication::processEvents(); // This dumb, needs a workaround.
        }

        // fs::remove(path);
        _mutex->lock();
        _workDone->enqueue(QString::fromStdWString(_path->wstring()));
        _mutex->unlock();

        emitReadyWork();
        emit busy(false);
    }

    void
    setPath(const QString& path)
    {
        _path = std::make_unique<fs::path>(path.toStdWString());
    };

private slots:
    void
    handleTimer(bool flag)
    {
        if (flag && !_timer->isActive())
        {
            _timer->start(5);
        }
        else
        {
            _timer->stop();
            emitReadyWork();
        }
    };

    void
    emitReadyWork()
    {
        _mutex->lock();
        int count = _workDone->count();
        QVector<QString> doneNow{count};
        while (!_workDone->empty())
        {
            doneNow.push_back(_workDone->dequeue());
        }
        _progress += count;
        _mutex->unlock();
        emit progress(_progress);
        emit removedPaths(doneNow);
    }

signals:
    void
    progress(int);

    void
    removedPaths(QVector<QString>);

private:
    std::unique_ptr<fs::path> _path;
    QQueue<QString>* _workDone;
    QThread* _timerThread;
    QTimer* _timer;
    QMutex* _mutex;
    int _progress;
};
