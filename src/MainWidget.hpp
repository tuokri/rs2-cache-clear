#pragma once

#include <QtWidgets>

class QPushButton;

class MainWidget : public QWidget
{
Q_OBJECT

public:
    explicit MainWidget(QWidget* parent = nullptr);

    ~MainWidget() override;

public slots:
    void
    onError(const QString& msg);

    void
    setSpinnerEnabled(bool enabled);

    void
    handleRemovedPaths(const QVector<QString>& paths);

    void
    resetProgressBar();

    void
    handleClearCacheWorkerBusy(bool busy);

private:
    QThread* _findCacheThread;
    QThread* _clearCacheThread;
    QLabel* _infoText;
    QLabel* _spinnerLabel;
    QMovie* _spinnerMovie;
    QPushButton* _findCacheButton;
    QLineEdit* _cachePathText;
    QPushButton* _clearCacheButton;
    QProgressBar* _clearProgressBar;
    QTextEdit* _logText;
    QTimer* _clearProgressUpdateTimer;
};
