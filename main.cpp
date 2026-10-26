#include "mainwindow.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QStyleHints>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName(QStringLiteral("DGUT"));
    QApplication::setApplicationName(QStringLiteral("小牛文本编辑"));
    QApplication::setApplicationVersion(QStringLiteral("1.0"));

    // 实验要求的是 Qt 5 的浅色外观：本机 Windows 处于深色模式，
    // 这里强制使用浅色配色，保证界面与实验指导书一致。
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
#endif

    // 载入 Qt 自带的中文翻译，让标准对话框按钮显示中文
    const QLocale chinese(QLocale::Chinese, QLocale::China);
    const QString transPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);

    QTranslator qtBaseTranslator;
    if (qtBaseTranslator.load(chinese, QStringLiteral("qtbase"), QStringLiteral("_"), transPath))
        a.installTranslator(&qtBaseTranslator);

    QTranslator qtTranslator;
    if (qtTranslator.load(chinese, QStringLiteral("qt"), QStringLiteral("_"), transPath))
        a.installTranslator(&qtTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
