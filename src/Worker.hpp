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
            qDebug() << __FUNCDNAME__ << ": user documents path: " << path.wstring();
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
    failure(const QString&);

    void
    success(bool);
};

class ClearCacheWorker : public Worker
{
Q_OBJECT

public:
    ClearCacheWorker()
        : Worker(), _path(nullptr), _workDone(new QQueue<QString>()), _mutex(new QMutex()), _progress(0)
    {
    };

    ~ClearCacheWorker() override
    {
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
            emit busy(false);
            return;
        }

        std::wstring pathStr;
        auto iter = fs::recursive_directory_iterator(*_path);
        for (const auto& item: iter)
        {
            _mutex->lock();
            _workDone->enqueue(QString::fromStdWString(item.path().wstring()));
            _mutex->unlock();
        }

        fs::remove_all(*_path);
        _mutex->lock();
        _workDone->enqueue(QString::fromStdWString(_path->wstring()));
        _mutex->unlock();

        emit busy(false);
    }

    void
    setPath(const QString& path)
    {
        _path = std::make_unique<fs::path>(path.toStdWString());
        _progress = 0;
    };

    void
    emitReadyWork()
    {
        // Don't want to choke UI thread by sending too much data.
        int chunkSize = 32;
        int i = 0;
        _mutex->lock();
        QVector<QString> doneNow(0);
        while (!_workDone->empty() && (i++ < chunkSize))
        {
            doneNow.push_back(_workDone->dequeue());
        }
        _progress += doneNow.count();
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
    QMutex* _mutex;
    int _progress;
};
