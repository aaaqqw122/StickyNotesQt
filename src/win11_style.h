#pragma once
#include <QWidget>
#include <QString>

// 启用 Win11 圆角
void enableWin11RoundedCorners(QWidget* widget);

// 启用 DWM Acrylic 毛玻璃模糊（alpha = 10~200，越大越透明）
void enableWin11Acrylic(QWidget* widget, int alpha = 64);

// 全局毛玻璃风格样式表
QString glassStyleSheet();
