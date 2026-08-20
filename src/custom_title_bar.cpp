#include "custom_title_bar.h"
#include <QHBoxLayout>

CustomTitleBar::CustomTitleBar(const QString& title, QWidget* parent)
    : QWidget(parent)
{
    setObjectName("TitleBar");
    setFixedHeight(38);

    title_label = new QLabel(title, this);
    title_label->setStyleSheet("font-size:13px; font-weight:600; color:#1a1a1a;");

    btn_minimize = new QPushButton("🗕", this);
    btn_minimize->setObjectName("TitleBtnMin");
    btn_minimize->setFixedSize(46, 32);
    btn_minimize->setToolTip("最小化");

    btn_close = new QPushButton("✕", this);
    btn_close->setObjectName("TitleBtnClose");
    btn_close->setFixedSize(46, 32);
    btn_close->setToolTip("关闭");

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(14, 0, 4, 0);
    layout->addWidget(title_label);
    layout->addStretch();
    layout->addWidget(btn_minimize);
    layout->addWidget(btn_close);

    connect(btn_minimize, &QPushButton::clicked, this, &CustomTitleBar::minimizeClicked);
    connect(btn_close,    &QPushButton::clicked, this, &CustomTitleBar::closeClicked);
}

void CustomTitleBar::setTitle(const QString& text) {
    title_label->setText(text);
}

void CustomTitleBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        drag_pos = event->globalPosition().toPoint() -
                   parentWidget()->frameGeometry().topLeft();
    }
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent* event) {
    if (dragging && (event->buttons() & Qt::LeftButton)) {
        if (auto* win = parentWidget())
            win->move(event->globalPosition().toPoint() - drag_pos);
    }
}

void CustomTitleBar::mouseReleaseEvent(QMouseEvent* event) {
    dragging = false;
}
