#pragma once
#include <QWidget>
#include <QTextBrowser>
#include <QLabel>
#include <QPushButton>
#include <QPoint>
#include <QPixmap>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QContextMenuEvent>
#include "rich_text_toolbar.h"
#include "rich_text_actions.h"

// 支持链接点击的文本浏览器
class LinkTextBrowser : public QTextBrowser {
public:
    explicit LinkTextBrowser(QWidget* parent = nullptr) : QTextBrowser(parent) {}
protected:
    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            QString href = anchorAt(event->pos());
            if (!href.isEmpty()) {
                QDesktopServices::openUrl(QUrl(href));
                return;
            }
        }
        QTextBrowser::mouseReleaseEvent(event);
    }
    void contextMenuEvent(QContextMenuEvent* event) override {
        QString href = anchorAt(event->pos());
        if (!href.isEmpty()) {
            QDesktopServices::openUrl(QUrl(href));
            return;
        }
        // 不显示默认菜单
    }
};

class NoteWidget : public QWidget {
    Q_OBJECT
public:
    explicit NoteWidget(int noteId, QWidget* parent = nullptr);
    ~NoteWidget();
    int getNoteId() const { return note_id; }
    void applyBackgroundImage(const QString& path);

signals:
    void noteContentChanged(int noteId);

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void saveNoteData();
    void restoreNoteData();

    int note_id;
    LinkTextBrowser* edit;
    RichTextToolBar* toolbar;
    RichTextActions* rich_actions;
    QPixmap bgPixmap;

    bool resizing = false;
    QPoint resize_start_pos;
    QRect resize_start_rect;
    bool dragging = false;
    QPoint drag_start_pos;

    bool isOnEdge(const QPoint& pos);
};
