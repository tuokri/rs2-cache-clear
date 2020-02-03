#pragma once

#include <QtWidgets>

class QPushButton;

class MainWidget : public QWidget
{
Q_OBJECT

public:
    explicit MainWidget(QWidget* parent = nullptr);

    ~MainWidget() override;

private slots:

    void
    onFindCacheButtonClicked();

    void
    onClearCacheButtonClicked();

private:
    QLabel* _infoText;
    QPushButton* _findCacheButton;
    QLineEdit* _cachePathText;
    QPushButton* _clearCacheButton;
    QProgressBar* _clearProgressBar;
    QTextEdit* _logText;
};
