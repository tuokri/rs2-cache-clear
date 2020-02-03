#include <string>
#include <sstream>

#include <QtWidgets>

#include "MainWidget.hpp"
#include "Utils.hpp"

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
        "Version 1.0.0"
    );

    _findCacheButton = new QPushButton("Find RS2 Configuration Files");
    connect(_findCacheButton, SIGNAL(clicked()), this, SLOT(onFindCacheButtonClicked()));

    _cachePathText = new QLineEdit();
    _cachePathText->setReadOnly(true);

    _clearCacheButton = new QPushButton("Clear RS2 Configuration Files");
    _clearCacheButton->setEnabled(false);
    connect(_clearCacheButton, SIGNAL(clicked()), this, SLOT(onClearCacheButtonClicked()));

    _clearProgressBar = new QProgressBar();
    _clearProgressBar->setValue(0);
    _clearProgressBar->setMinimum(0);
    _clearProgressBar->setTextVisible(false);

    _logText = new QTextEdit();
    _logText->setReadOnly(true);

    int topRowSpan = 10;
    auto mainLayout = new QGridLayout();
    mainLayout->addWidget(_infoText, 0, 0, topRowSpan, 2);
    mainLayout->addWidget(_findCacheButton, topRowSpan + 1, 0);
    mainLayout->addWidget(_cachePathText, topRowSpan + 1, 1);
    mainLayout->addWidget(_clearCacheButton, topRowSpan + 2, 0);
    mainLayout->addWidget(_clearProgressBar, topRowSpan + 2, 1);
    mainLayout->addWidget(_logText, topRowSpan + 3, 0, 1, 2);

    setLayout(mainLayout);
    setMinimumWidth(512);
    setWindowTitle("RS2: Vietnam Configuration Clearing Tool");
}

MainWidget::~MainWidget()
{
    delete _infoText;
    delete _findCacheButton;
    delete _cachePathText;
    delete _clearCacheButton;
    delete _clearProgressBar;
    delete _logText;
}

void MainWidget::onFindCacheButtonClicked()
{
    fs::path path;

    try
    {
        path = Utils::getUserDocumentsPath();
    }
    catch (const std::runtime_error& e)
    {
        std::wstringstream ss;
        ss << "Error: " << e.what() << "!";

        QMessageBox::warning(this, "Warning",
                             QString::fromStdWString(ss.str()), QMessageBox::Ok);
        return;
    }

    QString text = QString::fromStdWString(path.wstring());
    _cachePathText->setText(text);
    _clearProgressBar->setMaximum(Utils::countItemsInPath(path) + 1);
    _clearCacheButton->setEnabled(true);
}

void MainWidget::onClearCacheButtonClicked()
{
    auto path = fs::path(_cachePathText->text().toStdWString());
    Utils::rmdirCallback(path, [&](const std::wstring& log)
    {
        _clearProgressBar->setValue(_clearProgressBar->value() + 1);
        _logText->insertPlainText(QString::fromStdWString(log));
    });
}
