#include <QtWidgets>

#include "MainWidget.hpp"
#include "Worker.hpp"

namespace fs = std::filesystem;

MainWidget::MainWidget(QWidget* parent) : QWidget(parent)
{
    _infoText = new QLabel();
    _infoText->setTextFormat(Qt::RichText);
    _infoText->setTextInteractionFlags(Qt::TextBrowserInteraction);
    _infoText->setOpenExternalLinks(true);
    _infoText->setWordWrap(true);
    _infoText->setText(
        "Helper tool for clearing RS2 configuration and cache files. "
        "Clearing the configuration and cache files may resolve some common problems, such as, "
        "unresponsive UI or no visible servers in the server browser.<br><br>"
        "Clearing the configuration <b>WILL</b> reset your settings, such as, custom "
        "key binds and graphics settings. All custom workshop content will have to be re-downloaded.<br><br>"
        "Your character progress, level, experience or achievements will <b>NOT</b> be reset.<br><br>"
        "By fluudah: <a href=\"https://steamcommunity.com/id/fluudah\">"
        "https://steamcommunity.com/id/fluudah</a><br>"
        "Source code of this tool: <a href=\"https://github.com/tuokri/rs2-config-clear\">"
        "https://github.com/tuokri/rs2-config-clear</a><br><br>"
        "Version 1.0.4"
    );

    _spinnerLabel = new QLabel();
    _spinnerMovie = new QMovie(":/spinner.gif");
    _spinnerLabel->setMovie(_spinnerMovie);
    _spinnerLabel->setMinimumHeight(_spinnerMovie->scaledSize().height() + 1); // TODO: doesn't work.
    QSizePolicy spPolicy = _spinnerLabel->sizePolicy();
    spPolicy.setRetainSizeWhenHidden(true);
    _spinnerLabel->setSizePolicy(spPolicy);

    _findCacheButton = new QPushButton("Find RS2 Configuration Files");

    _cachePathText = new QLineEdit();
    _cachePathText->setReadOnly(true);

    _clearCacheButton = new QPushButton("Clear RS2 Configuration Files");
    _clearCacheButton->setEnabled(false);

    _clearProgressBar = new QProgressBar();
    _clearProgressBar->setValue(0);
    _clearProgressBar->setMinimum(0);
    _clearProgressBar->setTextVisible(false);

    _logText = new QTextEdit();
    _logText->setReadOnly(true);

    int incRowSpan = 10;
    auto gridLayout = new QGridLayout();
    gridLayout->addWidget(_infoText, 0, 0, incRowSpan, 2);
    gridLayout->addWidget(_spinnerLabel, ++incRowSpan, 0, 1, 1);
    gridLayout->addWidget(_findCacheButton, ++incRowSpan, 0);
    gridLayout->addWidget(_cachePathText, incRowSpan, 1);
    gridLayout->addWidget(_clearCacheButton, ++incRowSpan, 0);
    gridLayout->addWidget(_clearProgressBar, incRowSpan, 1);
    gridLayout->addWidget(_logText, ++incRowSpan, 0, 1, 2);

    setLayout(gridLayout);
    setMinimumWidth(512);
    setWindowTitle("RS2: Vietnam Configuration Clearing Tool");

    _findCacheThread = new QThread();
    _clearCacheThread = new QThread();

    auto findCacheWorker = new FindCacheWorker();
    auto clearCacheWorker = new ClearCacheWorker();

    _clearProgressUpdateTimer = new QTimer();
    _clearProgressUpdateTimer->setInterval(5);
    connect(_clearProgressUpdateTimer, SIGNAL(timeout()), clearCacheWorker, SLOT(emitReadyWork()));

    connect(_findCacheButton, SIGNAL(clicked()), findCacheWorker, SLOT(doWork()));
    connect(_findCacheButton, SIGNAL(clicked()), this, SLOT(resetProgressBar()));
    connect(findCacheWorker, SIGNAL(busy(bool)), _findCacheButton, SLOT(setDisabled(bool)));
    connect(findCacheWorker, SIGNAL(busy(bool)), this, SLOT(setSpinnerEnabled(bool)));
    connect(findCacheWorker, SIGNAL(busy(bool)), _clearCacheButton, SLOT(setDisabled(bool)));
    connect(findCacheWorker, SIGNAL(documentsPathResult(QString)), _cachePathText, SLOT(setText(QString)));
    connect(findCacheWorker, SIGNAL(failure(QString&)), this, SLOT(onError(QString&)));
    connect(findCacheWorker, SIGNAL(success(bool)), _clearCacheButton, SLOT(setEnabled(bool)));
    connect(findCacheWorker, SIGNAL(itemCountResult(int)), _clearProgressBar, SLOT(setMaximum(int)));
    connect(findCacheWorker, SIGNAL(documentsPathResult(QString)), clearCacheWorker, SLOT(setPath(QString)));

    connect(_clearCacheButton, SIGNAL(clicked()), clearCacheWorker, SLOT(doWork()));
    connect(_clearCacheButton, SIGNAL(clicked()), _clearProgressUpdateTimer, SLOT(start()));
    connect(clearCacheWorker, SIGNAL(removedPaths(QVector<QString>)),
            this, SLOT(handleRemovedPaths(QVector<QString>)));
    connect(clearCacheWorker, SIGNAL(progress(int)), _clearProgressBar, SLOT(setValue(int)));
    connect(clearCacheWorker, SIGNAL(busy(bool)), this, SLOT(handleClearCacheWorkerBusy(bool)));

    connect(_findCacheThread, SIGNAL(finished()), findCacheWorker, SLOT(deleteLater()));
    connect(_findCacheThread, SIGNAL(finished()), _findCacheThread, SLOT(deleteLater()));
    connect(_clearCacheThread, SIGNAL(finished()), clearCacheWorker, SLOT(deleteLater()));
    connect(_clearCacheThread, SIGNAL(finished()), _clearCacheThread, SLOT(deleteLater()));

    findCacheWorker->moveToThread(_findCacheThread);
    clearCacheWorker->moveToThread(_clearCacheThread);

    _findCacheThread->start();
    _clearCacheThread->start();
}

MainWidget::~MainWidget()
{
    _clearProgressUpdateTimer->stop();
    while (_clearProgressUpdateTimer->isActive());
    delete _clearProgressUpdateTimer;

    _findCacheThread->quit();
    while (!_findCacheThread->isFinished());
    qDebug() << __FUNCDNAME__ << ": _findCacheThread done";

    _clearCacheThread->quit();
    while (!_clearCacheThread->isFinished());
    qDebug() << __FUNCDNAME__ << ": _clearCacheThread done";

    delete _findCacheThread;
    delete _clearCacheThread;
    delete _infoText;
    delete _spinnerLabel;
    delete _spinnerMovie;
    delete _findCacheButton;
    delete _cachePathText;
    delete _clearCacheButton;
    delete _clearProgressBar;
    delete _logText;
}

void
MainWidget::onError(const QString& msg)
{
    QMessageBox::warning(this, "Warning",
                         msg, QMessageBox::Ok);
}

void
MainWidget::setSpinnerEnabled(bool enabled)
{
    if (enabled)
    {
        _spinnerLabel->show();
        _spinnerMovie->start();
    }
    else
    {
        _spinnerMovie->stop();
        _spinnerLabel->hide();
    }
}

void
MainWidget::handleRemovedPaths(const QVector<QString>& paths)
{
    for (const auto& p: paths)
    {
        _logText->append("Removed: " + p);
    }

    if (_clearProgressBar->value() == _clearProgressBar->maximum())
    {
        _clearProgressUpdateTimer->stop();
        _findCacheButton->setEnabled(true);
    }
}

void
MainWidget::resetProgressBar()
{
    _clearProgressBar->setValue(0);
}

void MainWidget::handleClearCacheWorkerBusy(bool busy)
{
    if (busy)
    {
        _findCacheButton->setEnabled(false);
        _clearCacheButton->setEnabled(false);
    }
    // Don't enable on busy(false), because timer might still be
    // updating log text box.
}
