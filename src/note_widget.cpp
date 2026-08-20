#include "note_widget.h"
#include "note_data_manager.h"
#include "win11_style.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QTextFormat>
#include <QPainter>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QPainterPath>
#include <QShowEvent>
#include <QSettings>
#include <QTimer>
#include <QPainter>
#include <QInputDialog>
#include <QDialog>
#include <QVBoxLayout>

NoteWidget::NoteWidget(int noteId, QWidget* parent)
    : QWidget(parent), note_id(noteId)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setWindowTitle(QString("便签 #%1").arg(noteId));
    setMinimumSize(280, 180);
    resize(420, 380);
    setObjectName("NoteWidget");
    setStyleSheet("#NoteWidget{background:#f0f0f0;}");
    enableWin11RoundedCorners(this);
    // 注意：enableWin11Acrylic（setWindowOpacity）在 showEvent 中调用

    // showEvent 里重新确保 Acrylic 生效

    auto* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);

    // ── 标题栏（可拖动区域）──
    auto* top_bar = new QWidget(this);
    top_bar->setObjectName("titleBar");
    top_bar->setFixedHeight(36);
    top_bar->installEventFilter(this);

    auto* top_layout = new QHBoxLayout(top_bar);
    top_layout->setContentsMargins(4, 0, 4, 0);

    auto* minimize_btn = new QPushButton("🗕", top_bar);
    auto* close_btn    = new QPushButton("✕", top_bar);
    minimize_btn->setFixedSize(46, 30);
    close_btn->setFixedSize(46, 30);
    minimize_btn->setObjectName("TitleBtnMin");
    close_btn->setObjectName("TitleBtnClose");

    top_layout->addStretch();
    top_layout->addWidget(minimize_btn);
    top_layout->addWidget(close_btn);
    main_layout->addWidget(top_bar);

    // ── 富文本编辑器 ──
    edit = new LinkTextBrowser(this);
    edit->setReadOnly(false);
    edit->setOpenExternalLinks(false);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setContextMenuPolicy(Qt::DefaultContextMenu);
    edit->setStyleSheet(
        "LinkTextBrowser{border:none; padding:12px; font-size:14px;"
        " background:rgba(255,255,255,180);"
        " selection-background-color:#005fb8; selection-color:white;}"
        "LinkTextBrowser::viewport{border:none;}");
    edit->installEventFilter(this);
    main_layout->addWidget(edit, 1);

    // 移除 anchorClicked 连接（改用子类 mouseReleaseEvent）

    // ── 富文本工具栏 ──
    toolbar = new RichTextToolBar(this);
    rich_actions = new RichTextActions(edit, this);

    // 插入链接按钮
    auto* linkBtn = new QPushButton("链接", this);
    linkBtn->setToolTip("插入超链接");
    linkBtn->setFixedSize(40, 28);
    linkBtn->setStyleSheet(
        "QPushButton{border:1px solid #ccc; border-radius:4px;"
        " background:white; font-size:11px; color:#005fb8;}"
        "QPushButton:hover{background:#e8f0fe;}");
    toolbar->addWidget(linkBtn);

    connect(linkBtn, &QPushButton::clicked, this, [this]() {
        bool ok1, ok2;
        QString url = QInputDialog::getText(this, "插入链接",
            "链接地址:", QLineEdit::Normal, "https://", &ok1);
        if (!ok1 || url.isEmpty()) return;
        QString text = QInputDialog::getText(this, "插入链接",
            "显示文字:", QLineEdit::Normal, "", &ok2);
        if (!ok2) return;
        if (text.isEmpty()) text = url;
        edit->textCursor().insertHtml(
            QString("<a href=\"%1\">%2</a> ").arg(url, text));
    });

    main_layout->addWidget(toolbar);

    // 连接信号
    connect(toolbar, &RichTextToolBar::underlineRequested,
            rich_actions, &RichTextActions::applyUnderline);
    connect(toolbar, &RichTextToolBar::insertDividerRequested,
            rich_actions, &RichTextActions::insertDivider);
    connect(toolbar, &RichTextToolBar::alignLeftRequested,
            rich_actions, &RichTextActions::alignLeft);
    connect(toolbar, &RichTextToolBar::alignCenterRequested,
            rich_actions, &RichTextActions::alignCenter);
    connect(toolbar, &RichTextToolBar::alignRightRequested,
            rich_actions, &RichTextActions::alignRight);
    connect(toolbar, &RichTextToolBar::fontSizeRequested,
            rich_actions, &RichTextActions::setFontSize);
    connect(toolbar, &RichTextToolBar::backgroundColorRequested,
            rich_actions, &RichTextActions::setBackgroundColor);

    connect(minimize_btn, &QPushButton::clicked, this, [this]() { hide(); });
    connect(close_btn, &QPushButton::clicked, this, [this]() { close(); });

    // 自动保存
    connect(edit, &QTextBrowser::textChanged, this, [this]() {
        saveNoteData();
        NoteDataManager::instance().saveToFile(
            QApplication::applicationDirPath() + "/notes_data.json");
        emit noteContentChanged(note_id);
    });

    // 加载数据
    restoreNoteData();
    NoteData nd = NoteDataManager::instance().getNoteById(note_id);
    if (nd.windowPos != QPoint(100, 100))
        move(nd.windowPos);
    if (nd.windowSize.isValid())
        resize(nd.windowSize);
}

NoteWidget::~NoteWidget() {
    saveNoteData();
}

void NoteWidget::saveNoteData() {
    NoteData nd = NoteDataManager::instance().getNoteById(note_id);
    nd.content    = edit->document()->isEmpty() ? "" : edit->toHtml();
    nd.windowSize = size();
    nd.windowPos  = pos();
    NoteDataManager::instance().updateNote(note_id, nd);
}

void NoteWidget::restoreNoteData() {
    QString content = NoteDataManager::instance().getNoteById(note_id).content;
    edit->blockSignals(true);
    if (content.isEmpty())
        edit->clear();
    else
        edit->setHtml(content);
    edit->blockSignals(false);
}

void NoteWidget::closeEvent(QCloseEvent* event) {
    saveNoteData();
    NoteDataManager::instance().saveToFile(
        QApplication::applicationDirPath() + "/notes_data.json");
    QWidget::closeEvent(event);
    deleteLater();
}

void NoteWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    QSettings settings("MyCompany", "StickyNotesQt");
    int alpha = settings.value("glass_alpha", 50).toInt();
    enableWin11Acrylic(this, alpha);
    // 加载背景图片
    QString bgPath = settings.value("background_image", "").toString();
    if (!bgPath.isEmpty() && bgPixmap.isNull()) {
        applyBackgroundImage(bgPath);
    }
}

void NoteWidget::applyBackgroundImage(const QString& path) {
    bgPixmap = QPixmap(path);
    if (!bgPixmap.isNull()) {
        bgPixmap = bgPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding,
                                    Qt::SmoothTransformation);
    }
    update();
}

void NoteWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    if (!bgPixmap.isNull()) {
        QPainter painter(this);
        QPixmap scaled = bgPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding,
                                          Qt::SmoothTransformation);
        int x = (width() - scaled.width()) / 2;
        int y = (height() - scaled.height()) / 2;
        painter.setOpacity(0.4);
        painter.drawPixmap(x, y, scaled);
    }
}

bool NoteWidget::isOnEdge(const QPoint& pos) {
    const int margin = 8;
    QRect r = rect();
    return pos.x() >= r.width() - margin || pos.y() >= r.height() - margin;
}

void NoteWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (isOnEdge(event->pos())) {
            resizing = true;
            resize_start_pos = event->globalPosition().toPoint();
            resize_start_rect = geometry();
        }
        // 标题栏拖动通过 eventFilter 在 titleBar 上处理
    }
    QWidget::mousePressEvent(event);
}

void NoteWidget::mouseMoveEvent(QMouseEvent* event) {
    if (resizing) {
        QPoint delta = event->globalPosition().toPoint() - resize_start_pos;
        QRect r = resize_start_rect;
        r.setWidth(qMax(minimumWidth(),  r.width()  + delta.x()));
        r.setHeight(qMax(minimumHeight(), r.height() + delta.y()));
        setGeometry(r);
    }
    QWidget::mouseMoveEvent(event);
}

void NoteWidget::mouseReleaseEvent(QMouseEvent* event) {
    resizing = false;
    dragging = false;
    QWidget::mouseReleaseEvent(event);
}

void NoteWidget::moveEvent(QMoveEvent* event) {
    QWidget::moveEvent(event);
    NoteDataManager::instance().updateNote(note_id,
        NoteDataManager::instance().getNoteById(note_id));
}

void NoteWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
}

bool NoteWidget::eventFilter(QObject* obj, QEvent* event) {
    // 只在标题栏区域处理窗口拖动
    auto* tb = findChild<QWidget*>("titleBar");
    bool onTitleBar = (obj == tb);
    if (!onTitleBar && tb) {
        // 检查是否是标题栏的子控件
        QObject* p = obj ? obj->parent() : nullptr;
        while (p) {
            if (p == tb) { onTitleBar = true; break; }
            p = p->parent();
        }
    }

    if (onTitleBar && event->type() == QEvent::MouseButtonPress) {
        auto me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            dragging = true;
            drag_start_pos = me->globalPosition().toPoint();
            return true;
        }
    }
    if (onTitleBar && event->type() == QEvent::MouseMove && dragging) {
        auto me = static_cast<QMouseEvent*>(event);
        QPoint delta = me->globalPosition().toPoint() - drag_start_pos;
        move(pos() + delta);
        drag_start_pos = me->globalPosition().toPoint();
        return true;
    }
    if (onTitleBar && event->type() == QEvent::MouseButtonRelease) {
        dragging = false;
        return true;
    }

    return QWidget::eventFilter(obj, event);
}
