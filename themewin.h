#ifndef THEMEWIN_H
#define THEMEWIN_H

#include <QWidget>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dwmapi.h>
#endif

// 本机 Windows 处于深色模式，Qt 6 会把标题栏画成深色。
// 实验指导书使用的是 Qt 5 的浅色外观，这里显式关闭窗口的深色标题栏。
inline void applyLightTitleBar(QWidget *widget)
{
#ifdef Q_OS_WIN
    if (!widget)
        return;

    const HWND hwnd = reinterpret_cast<HWND>(widget->winId());
    BOOL dark = FALSE;
    // 20: Windows 11 22H2 及以后；19: 更早的 Windows 10/11 版本
    DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
    DwmSetWindowAttribute(hwnd, 19, &dark, sizeof(dark));
#else
    Q_UNUSED(widget)
#endif
}

#endif // THEMEWIN_H
