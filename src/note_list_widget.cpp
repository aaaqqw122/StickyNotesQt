#include "note_list_widget.h"
#include "note_data_manager.h"
#include "win11_style.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QRegularExpression>
#include <QSettings>
#include <QApplication>
#include <QTextDocument>
#include <QFileDialog>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QMouseEvent>
#include <QCheckBox>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>

NoteListWidget::NoteListWidget(QWidget* parent)
    : QWidget(parent)
{
    // 主布局：水平排列 [列表区 | 侧边栏]
    auto* rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // ── 左侧：列表区 ──
    auto* listArea = new QWidget(this);
    listArea->setObjectName("listArea");
    listArea->installEventFilter(this);
    auto* listLayout = new QVBoxLayout(listArea);
    listLayout->setContentsMargins(12, 12, 8, 12);
    listLayout->setSpacing(10);

    auto* topRow = new QHBoxLayout();
    auto* createBtn = new QPushButton("+ 新建便签");
    createBtn->setObjectName("PrimaryBtn");
    createBtn->setCursor(Qt::PointingHandCursor);
    createBtn->setFixedHeight(34);

    // 侧边栏展开按钮
    auto* sidebarBtn = new QPushButton("☰", this);
    sidebarBtn->setToolTip("设置面板");
    sidebarBtn->setFixedSize(34, 34);
    sidebarBtn->setStyleSheet(
        "QPushButton{border:1px solid #ccc; border-radius:6px;"
        " background:rgba(255,255,255,180); font-size:16px; color:#555;}"
        "QPushButton:hover{background:rgba(200,200,200,200);}");

    topRow->addWidget(createBtn, 1);
    topRow->addWidget(sidebarBtn);
    listLayout->addLayout(topRow);

    list = new QListWidget(this);
    list->setCursor(Qt::PointingHandCursor);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listLayout->addWidget(list, 1);

    rootLayout->addWidget(listArea, 1);

    // ── 右侧：侧边栏（初始宽度 0）──
    sidebar = new QWidget(this);
    sidebar->setMaximumWidth(0); // 默认收起
    sidebar->setStyleSheet(
        "background:rgba(240,240,240,220);"
        " border-left:1px solid rgba(200,200,200,150);");

    auto* sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(10, 14, 10, 14);
    sideLayout->setSpacing(12);

    auto* sideTitle = new QLabel("设置面板", sidebar);
    sideTitle->setStyleSheet("font-size:14px; font-weight:600; color:#333; background:transparent;");

    // ── 玻璃强度 ──
    auto* glassSection = new QWidget(sidebar);
    glassSection->setStyleSheet("background:transparent;");
    auto* glassSecLayout = new QVBoxLayout(glassSection);
    glassSecLayout->setContentsMargins(0, 0, 0, 0);
    glassSecLayout->setSpacing(4);

    auto* glassHeader = new QLabel("毛玻璃强度", glassSection);
    glassHeader->setStyleSheet("font-size:12px; font-weight:600; color:#666; background:transparent;");

    glassSlider = new QSlider(Qt::Horizontal, glassSection);
    glassSlider->setRange(10, 200);
    glassSlider->setFixedHeight(24);
    glassSlider->setStyleSheet(
        "QSlider::groove:horizontal{height:4px; background:#ddd; border-radius:2px;}"
        "QSlider::handle:horizontal{width:16px; height:16px; margin:-6px 0;"
        " background:#0078d4; border-radius:8px;}"
        "QSlider::handle:horizontal:hover{background:#106ebe;}"
        "QSlider::sub-page:horizontal{background:#0078d4; border-radius:2px;}");

    glassLabel = new QLabel("", glassSection);
    glassLabel->setStyleSheet("font-size:11px; color:#888; background:transparent;");

    QSettings settings("MyCompany", "StickyNotesQt");
    int savedAlpha = settings.value("glass_alpha", 50).toInt();
    glassSlider->setValue(savedAlpha);
    glassLabel->setText(QString("透明度 %1/200").arg(savedAlpha));

    glassSecLayout->addWidget(glassHeader);
    glassSecLayout->addWidget(glassSlider);
    glassSecLayout->addWidget(glassLabel);

    // ── 背景图片 ──
    auto* bgBtn = new QPushButton("选择背景图片", sidebar);
    bgBtn->setToolTip("选择背景图片");
    bgBtn->setFixedHeight(34);
    bgBtn->setCursor(Qt::PointingHandCursor);
    bgBtn->setStyleSheet(
        "QPushButton{text-align:left; padding:0 10px;"
        " border:1px solid #ccc; border-radius:6px;"
        " background:rgba(255,255,255,180); font-size:12px; color:#555;}"
        "QPushButton:hover{background:rgba(200,200,200,200);}");

    // ── 启动便签设置 ──
    auto* startupBtn = new QPushButton("启动便签设置", sidebar);
    startupBtn->setToolTip("选择启动时自动弹出的便签");
    startupBtn->setFixedHeight(34);
    startupBtn->setCursor(Qt::PointingHandCursor);
    startupBtn->setStyleSheet(
        "QPushButton{text-align:left; padding:0 10px;"
        " border:1px solid #ccc; border-radius:6px;"
        " background:rgba(255,255,255,180); font-size:12px; color:#555;}"
        "QPushButton:hover{background:rgba(200,200,200,200);}");

    // ── 开机自启动 ──
    auto* autoStartSection = new QWidget(sidebar);
    autoStartSection->setStyleSheet("background:transparent;");
    auto* autoStartLayout = new QVBoxLayout(autoStartSection);
    autoStartLayout->setContentsMargins(0, 0, 0, 0);
    autoStartLayout->setSpacing(4);

    auto* autoStartLabel = new QLabel("开机自启动", autoStartSection);
    autoStartLabel->setStyleSheet("font-size:12px; color:#555; background:transparent;");

    auto* autoStartCheck = new QCheckBox("", autoStartSection);
    autoStartCheck->setFixedSize(36, 20);
    // 读取当前自启动状态
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                  QSettings::NativeFormat);
    bool autoStart = reg.contains("StickyNotesQt");
    autoStartCheck->setChecked(autoStart);
    // 样式化为开关
    autoStartCheck->setStyleSheet(
        "QCheckBox{spacing:0;}"
        "QCheckBox::indicator{width:36px; height:18px; border-radius:9px;"
        " background:#ccc; border:1px solid #aaa;}"
        "QCheckBox::indicator:checked{background:#0078d4; border-color:#0078d4;}");
    // 用 layouter 让 indicator 居中
    autoStartLayout->addWidget(autoStartLabel);
    autoStartLayout->addWidget(autoStartCheck);

    connect(autoStartCheck, &QCheckBox::toggled, this, [](bool checked) {
        QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                      QSettings::NativeFormat);
        if (checked) {
            reg.setValue("StickyNotesQt",
                         QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        } else {
            reg.remove("StickyNotesQt");
        }
    });

    // ── 删除全部便签 ──
    auto* clearAllBtn = new QPushButton("删除全部便签", sidebar);
    clearAllBtn->setFixedHeight(34);
    clearAllBtn->setCursor(Qt::PointingHandCursor);
    clearAllBtn->setStyleSheet(
        "QPushButton{text-align:left; padding:0 10px;"
        " border:1px solid #e88; border-radius:6px;"
        " background:rgba(255,220,220,180); font-size:12px; color:#c33;}"
        "QPushButton:hover{background:rgba(255,180,180,200);}");

    connect(clearAllBtn, &QPushButton::clicked, this, [this]() {
        auto reply = QMessageBox::question(this, "确认删除",
            "确定要删除全部便签吗？\n此操作不可撤销！",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            auto notes = NoteDataManager::instance().getAllNotes();
            for (const auto& n : notes) {
                NoteDataManager::instance().removeNote(n.id);
            }
            refreshNotes();
            if (auto* w = window())
                w->activateWindow();
        }
    });

    sideLayout->addWidget(sideTitle);
    sideLayout->addWidget(glassSection);
    sideLayout->addWidget(bgBtn);
    sideLayout->addWidget(startupBtn);
    sideLayout->addWidget(autoStartSection);
    sideLayout->addWidget(clearAllBtn);
    sideLayout->addStretch();

    rootLayout->addWidget(sidebar);

    // ── 连接信号 ──
    connect(createBtn, &QPushButton::clicked, this, &NoteListWidget::createNoteRequested);
    connect(list, &QListWidget::itemClicked, this, &NoteListWidget::onNoteItemClicked);
    connect(glassSlider, &QSlider::valueChanged, this, &NoteListWidget::onGlassSliderChanged);
    connect(bgBtn, &QPushButton::clicked, this, &NoteListWidget::backgroundImageRequested);
    connect(startupBtn, &QPushButton::clicked, this, &NoteListWidget::settingsRequested);

    // 安装事件过滤器，拦截点击关闭侧边栏
    list->viewport()->installEventFilter(this);
    listArea->installEventFilter(this);
    createBtn->installEventFilter(this);

    // 侧边栏动画
    connect(sidebarBtn, &QPushButton::clicked, this, [this]() {
        bool isOpen = (sidebar->maximumWidth() > 100);
        int targetWidth = isOpen ? 0 : 220;
        auto* anim = new QPropertyAnimation(sidebar, "maximumWidth", this);
        anim->setDuration(200);
        anim->setStartValue(sidebar->width());
        anim->setEndValue(targetWidth);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    });

    // 点击列表区域自动关闭侧边栏
    connect(list, &QListWidget::pressed, this, [this]() {
        closeSidebar();
    });

    list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(list, &QWidget::customContextMenuRequested,
            this, &NoteListWidget::onNotesListContextMenu);

    refreshNotes();
}

void NoteListWidget::refreshNotes() {
    list->clear();
    auto notes = NoteDataManager::instance().getAllNotes();
    int maxWidth = list->viewport()->width() - 40;
    if (maxWidth < 50) maxWidth = 200;
    QFontMetrics fm(list->font());

    for (const auto& n : notes) {
        QTextDocument doc;
        doc.setHtml(n.content);
        QString preview = doc.toPlainText().simplified();
        if (preview.isEmpty()) preview = "(空便签)";
        preview = fm.elidedText(preview, Qt::ElideRight, maxWidth);
        auto* item = new QListWidgetItem(preview);
        item->setData(Qt::UserRole, n.id);
        list->addItem(item);
    }
}

void NoteListWidget::onNoteItemClicked(QListWidgetItem* item) {
    int id = item->data(Qt::UserRole).toInt();
    if (id > 0)
        emit noteItemClicked(id);
}

void NoteListWidget::onGlassSliderChanged(int value) {
    glassLabel->setText(QString("透明度 %1/200").arg(value));
    QSettings settings("MyCompany", "StickyNotesQt");
    settings.setValue("glass_alpha", value);
    emit glassIntensityChanged(value);
}

void NoteListWidget::closeSidebar() {
    if (sidebar && sidebar->maximumWidth() > 100) {
        auto* anim = new QPropertyAnimation(sidebar, "maximumWidth", this);
        anim->setDuration(150);
        anim->setEndValue(0);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

bool NoteListWidget::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress && sidebar &&
        sidebar->maximumWidth() > 100) {
        // 点击侧边栏外部区域时关闭
        auto* me = static_cast<QMouseEvent*>(event);
        QPoint globalPos = me->globalPosition().toPoint();
        QPoint localPos = sidebar->mapFromGlobal(globalPos);
        if (!sidebar->rect().contains(localPos)) {
            closeSidebar();
        }
    }
    return QWidget::eventFilter(obj, event);
}

void NoteListWidget::mousePressEvent(QMouseEvent* event) {
    // 点击侧边栏以外的区域时关闭侧边栏
    if (sidebar && sidebar->maximumWidth() > 100) {
        QPoint localPos = sidebar->mapFromGlobal(event->globalPosition().toPoint());
        if (!sidebar->rect().contains(localPos)) {
            closeSidebar();
        }
    }
    QWidget::mousePressEvent(event);
}

void NoteListWidget::contextMenuEvent(QContextMenuEvent* event) {
    onNotesListContextMenu(event->pos());
}

void NoteListWidget::onNotesListContextMenu(const QPoint& pos) {
    auto* item = list->itemAt(pos);
    if (!item) return;

    int noteId = item->data(Qt::UserRole).toInt();
    if (noteId <= 0) return;

    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu{background:white; border:1px solid #e0e0e0; border-radius:6px;"
        " padding:4px;}"
        "QMenu::item{padding:6px 24px; border-radius:4px;}"
        "QMenu::item:selected{background:#e5e5e5;}");

    auto* del = menu.addAction("删除便签");
    connect(del, &QAction::triggered, this, [this, noteId]() {
        NoteDataManager::instance().removeNote(noteId);
        refreshNotes();
        emit noteDeleted(noteId);
        if (auto* w = window())
            w->activateWindow();
    });
    menu.exec(list->mapToGlobal(pos));
}
