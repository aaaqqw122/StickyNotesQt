#include "note_manager.h"
#include "note_list_widget.h"
#include "note_widget.h"
#include "note_data_manager.h"
#include "win11_style.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QShowEvent>
#include <QSettings>
#include <QFileDialog>
#include <QPainter>
#include <QFileInfo>
#include "settings_dialog.h"

NoteManager::NoteManager(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("便签");
    resize(380, 460);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setObjectName("NoteManager");
    setStyleSheet("#NoteManager{background:#f0f0f0;}");
    enableWin11RoundedCorners(this);
    // enableWin11Acrylic 在 main.cpp 和 showEvent 中调用

    custom_title_bar = new CustomTitleBar("便签", this);
    connect(custom_title_bar, &CustomTitleBar::minimizeClicked,
            this, [this](){ showMinimized(); });
    connect(custom_title_bar, &CustomTitleBar::closeClicked,
            this, [this](){ close(); });

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(custom_title_bar);

    stack = new QStackedWidget(this);
    notesPage = new NoteListWidget(this);
    stack->addWidget(notesPage);
    layout->addWidget(stack);

    // 应用 Win11 全局样式
    qApp->setStyleSheet(glassStyleSheet());

    connect(notesPage, &NoteListWidget::createNoteRequested,
            this, &NoteManager::onCreateNoteRequested);
    connect(notesPage, &NoteListWidget::noteItemClicked,
            this, &NoteManager::onNoteItemClicked);
    connect(notesPage, &NoteListWidget::backgroundImageRequested,
            this, [this]() {
        QString path = QFileDialog::getOpenFileName(this, "选择背景图片", "",
            "图片文件 (*.png *.jpg *.bmp *.jpeg)");
        if (!path.isEmpty()) {
            setBackgroundImage(path);
            QSettings settings("MyCompany", "StickyNotesQt");
            settings.setValue("background_image", path);
            // 同步到所有打开的便签窗口
            for (auto* w : QApplication::topLevelWidgets()) {
                if (auto* nw = qobject_cast<NoteWidget*>(w))
                    nw->applyBackgroundImage(path);
            }
        }
    });
    connect(notesPage, &NoteListWidget::glassIntensityChanged,
            this, [this](int alpha) {
        enableWin11Acrylic(this, alpha);
        for (auto* w : QApplication::topLevelWidgets()) {
            if (auto* nw = qobject_cast<NoteWidget*>(w)) {
                enableWin11Acrylic(nw, alpha);
                if (!bgImagePath.isEmpty()) {
                    nw->applyBackgroundImage(bgImagePath);
                }
            }
        }
    });
    connect(notesPage, &NoteListWidget::settingsRequested,
            this, [this]() {
        StartupSettingsDialog dlg(this);
        dlg.exec();
    });
    stack->setCurrentWidget(notesPage);
}

void NoteManager::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    // 加载保存的背景图片
    QSettings settings("MyCompany", "StickyNotesQt");
    QString bgPath = settings.value("background_image", "").toString();
    if (!bgPath.isEmpty() && bgPixmap.isNull()) {
        setBackgroundImage(bgPath);
    }
}

void NoteManager::setBackgroundImage(const QString& path) {
    bgImagePath = path;
    bgPixmap = QPixmap(path);
    if (!bgPixmap.isNull()) {
        bgPixmap = bgPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding,
                                    Qt::SmoothTransformation);
    }
    update();
}

void NoteManager::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    if (!bgPixmap.isNull()) {
        QPainter painter(this);
        // 缩放图片覆盖整个窗口
        QPixmap scaled = bgPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding,
                                          Qt::SmoothTransformation);
        int x = (width() - scaled.width()) / 2;
        int y = (height() - scaled.height()) / 2;
        painter.setOpacity(0.5); // 半透明，让玻璃效果透出
        painter.drawPixmap(x, y, scaled);
    }
}

void NoteManager::onCreateNoteRequested() {
    int new_id = NoteDataManager::instance().allocateId();
    if (!NoteDataManager::instance().addNote(new_id))
        return;
    notesPage->refreshNotes();

    currentNoteWidget = new NoteWidget(new_id, nullptr);
    connect(currentNoteWidget, &NoteWidget::destroyed, this,
            [this]() { currentNoteWidget = nullptr; });
    currentNoteWidget->show();
}

void NoteManager::onNoteItemClicked(int noteId) {
    if (currentNoteWidget && currentNoteWidget->isVisible()) {
        currentNoteWidget->activateWindow();
        currentNoteWidget->raise();
        return;
    }
    currentNoteWidget = new NoteWidget(noteId, nullptr);
    connect(currentNoteWidget, &NoteWidget::destroyed, this,
            [this]() { currentNoteWidget = nullptr; });
    connect(currentNoteWidget, &NoteWidget::noteContentChanged,
            notesPage, &NoteListWidget::refreshNotes);
    currentNoteWidget->show();
}
