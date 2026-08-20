#include <QApplication>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QSharedMemory>
#include <QMessageBox>

#include "note_manager.h"
#include "note_data_manager.h"
#include "note_list_widget.h"
#include "win11_style.h"
#include <QSettings>
#include <QPainter>
#include <QPixmap>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    // 单例检查
    QSharedMemory sharedMem("StickyNotesQt_InstanceGuard");
    if (!sharedMem.create(1)) {
        QMessageBox::information(nullptr, "便签",
            "便签应用已在运行中。\n请查看系统托盘图标。");
        return 0;
    }

    // 延迟初始化到事件循环启动后，避免开机时系统未就绪卡住
    QTimer::singleShot(0, [&app]() {
        // 加载已保存的便签数据
        NoteDataManager::instance().loadFromFile(
            QCoreApplication::applicationDirPath() + "/notes_data.json");

        auto* manager = new NoteManager();
        auto* tray = new QSystemTrayIcon(manager);

        // 创建图标 — 从文件中加载
        QString iconPath = QCoreApplication::applicationDirPath()
                           + "/resources/icon/favicon.ico";
        QIcon appIcon(iconPath);
        if (appIcon.isNull()) {
            // 兜底：文件不存在时绘制简易图标
            QPixmap px(32, 32);
            px.fill(Qt::transparent);
            QPainter p(&px);
            p.setRenderHint(QPainter::Antialiasing);
            p.setBrush(QColor("#0078d4"));
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(1, 1, 30, 30, 7, 7);
            p.setPen(QPen(Qt::white, 2));
            p.setFont(QFont("Segoe UI", 18, QFont::Bold));
            p.drawText(px.rect(), Qt::AlignCenter, "N");
            p.end();
            appIcon = QIcon(px);
        }
        tray->setIcon(appIcon);
        QApplication::setWindowIcon(appIcon);
        tray->setToolTip("便签应用");

        QObject::connect(tray, &QSystemTrayIcon::activated,
                        [manager](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::DoubleClick)
                manager->isVisible() ? manager->hide() : manager->show();
        });

        auto* trayMenu = new QMenu();
        trayMenu->setStyleSheet(
            "QMenu{background:rgba(255,255,255,240);"
            " border:1px solid rgba(220,220,220,200);"
            " border-radius:6px; padding:4px;}"
            "QMenu::item{padding:6px 24px; border-radius:4px;}"
            "QMenu::item:selected{background:#e5e5e5;}"
            "QMenu::separator{height:1px; background:#e0e0e0; margin:4px 8px;}");
        trayMenu->addAction("显示主窗口", manager, &QWidget::show);
        trayMenu->addAction("隐藏主窗口", manager, &QWidget::hide);
        trayMenu->addSeparator();
        trayMenu->addAction("退出", qApp, &QApplication::quit);

        tray->setContextMenu(trayMenu);
        tray->show();

        manager->hide();

        // 毛玻璃 + 背景
        QSettings settings("MyCompany", "StickyNotesQt");
        int alpha = settings.value("glass_alpha", 50).toInt();
        enableWin11Acrylic(manager, alpha);
        QString bgPath = settings.value("background_image", "").toString();
        if (!bgPath.isEmpty())
            manager->setBackgroundImage(bgPath);

        // 启动便签
        QString startupIds = settings.value("startup_note_ids", "").toString();
        if (!startupIds.isEmpty()) {
            QStringList ids = startupIds.split(",", Qt::SkipEmptyParts);
            for (const QString& idStr : ids) {
                bool ok;
                int id = idStr.toInt(&ok);
                if (ok && id > 0) {
                    auto* nw = new NoteWidget(id, nullptr);
                    nw->show();
                }
            }
        }

    });

    return app.exec();
}
