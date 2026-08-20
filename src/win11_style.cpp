#include "win11_style.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

void enableWin11RoundedCorners(QWidget* widget) {
#ifdef Q_OS_WIN
    if (auto hwnd = reinterpret_cast<HWND>(widget->winId())) {
        const DWORD roundPreference = 2;
        DwmSetWindowAttribute(hwnd, 33, &roundPreference, sizeof(roundPreference));
    }
#else
    Q_UNUSED(widget);
#endif
}

void enableWin11Acrylic(QWidget* widget, int alpha) {
    // 窗口透明度 — 安全调用
    if (widget) {
        qreal opacity = 1.0 - (qBound(10, alpha, 200) - 10) * 0.3 / 190.0;
        widget->setWindowOpacity(opacity);
    }
    enableWin11RoundedCorners(widget);
    Q_UNUSED(alpha);
}

QString glassStyleSheet() {
    return R"(
        QWidget {
            font-family: "Segoe UI Variable", "Segoe UI", "Microsoft YaHei UI", sans-serif;
            font-size: 13px;
            color: #1a1a1a;
        }

        QWidget#NoteManager, QWidget#NoteWidget {
            background: #f0f0f0;
        }

        QWidget#titleBar {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 rgba(255,255,255,180), stop:1 rgba(245,245,245,160));
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            border-bottom: 1px solid rgba(200,200,200,100);
        }
        QWidget#titleBar QLabel {
            font-size: 13px;
            font-weight: 600;
            color: #1a1a1a;
        }

        QPushButton {
            border: none;
            border-radius: 5px;
            padding: 5px 16px;
            background: rgba(200,200,200,140);
            color: #1a1a1a;
            font-size: 13px;
        }
        QPushButton:hover {
            background: rgba(180,180,180,180);
        }
        QPushButton:pressed {
            background: rgba(160,160,160,200);
        }
        QPushButton#PrimaryBtn {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #0078d4, stop:1 #005fb8);
            color: white;
            font-weight: 600;
        }
        QPushButton#PrimaryBtn:hover {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #106ebe, stop:1 #004578);
        }

        QPushButton#TitleBtnMin {
            border: none; border-radius: 4px;
            background: transparent; font-size: 13px; color: #555;
        }
        QPushButton#TitleBtnMin:hover {
            background: rgba(180,180,180,120);
        }
        QPushButton#TitleBtnClose {
            border: none; border-radius: 4px;
            background: transparent; font-size: 15px; color: #555;
        }
        QPushButton#TitleBtnClose:hover {
            background: rgba(232,17,35,200); color: white;
        }

        QListWidget {
            border: none; background: transparent; outline: none;
        }
        QListWidget::item {
            padding: 10px 14px; border-radius: 6px;
            margin: 2px 10px;
            background: rgba(255,255,255,180);
        }
        QListWidget::item:hover {
            background: rgba(255,255,255,220);
        }
        QListWidget::item:selected {
            background: rgba(240,240,240,230);
            color: #1a1a1a;
        }

        QTextBrowser {
            border: none; padding: 12px; font-size: 14px;
            background: rgba(255,255,255,180);
            selection-background-color: #005fb8;
            selection-color: white;
        }

        QToolBar {
            background: rgba(240,240,240,180);
            border-top: 1px solid rgba(200,200,200,100);
            spacing: 4px; padding: 4px 8px;
        }
        QToolBar QPushButton {
            border: 1px solid transparent; border-radius: 4px;
            background: transparent; padding: 4px 8px;
            font-size: 12px; min-width: 28px;
        }
        QToolBar QPushButton:hover {
            background: rgba(200,200,200,150);
            border-color: rgba(180,180,180,120);
        }
        QToolBar QComboBox {
            border: 1px solid rgba(200,200,200,180);
            border-radius: 4px; padding: 2px 6px;
            background: rgba(255,255,255,200);
            min-width: 56px;
        }
        QToolBar QComboBox:hover {
            border-color: #0078d4;
        }
        QToolBar QComboBox::drop-down {
            border: none; width: 16px;
        }

        QMenu {
            background: rgba(255,255,255,230);
            border: 1px solid rgba(220,220,220,200);
            border-radius: 6px; padding: 4px;
        }
        QMenu::item {
            padding: 6px 24px; border-radius: 4px;
        }
        QMenu::item:selected {
            background: rgba(220,220,220,200);
        }
    )";
}
