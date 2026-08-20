#include "rich_text_toolbar.h"
#include <QColorDialog>

RichTextToolBar::RichTextToolBar(QWidget* parent)
    : QToolBar(parent)
{
    setMovable(false);
    setStyleSheet(
        "QToolBar{background:#f3f3f3; border-top:1px solid #e5e5e5;"
        " spacing:4px; padding:4px 6px;}");

    auto makeBtn = [this](const QString& text, const QString& tip,
                          const QString& extraStyle = "") -> QPushButton* {
        auto* btn = new QPushButton(text, this);
        btn->setToolTip(tip);
        btn->setFixedSize(32, 28);
        btn->setStyleSheet(
            "QPushButton{border:1px solid transparent; border-radius:4px;"
            " background:transparent; font-size:12px; padding:2px;}" +
            extraStyle +
            "QPushButton:hover{background:#d0d0d0; border-color:#c0c0c0;}");
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };

    underlineBtn = makeBtn("U", "下划线",
        "QPushButton{font-weight:bold; text-decoration:underline;}");
    addWidget(underlineBtn);
    connect(underlineBtn, &QPushButton::clicked, this, &RichTextToolBar::underlineRequested);

    dividerBtn = makeBtn("—", "分割线");
    addWidget(dividerBtn);
    connect(dividerBtn, &QPushButton::clicked, this, &RichTextToolBar::insertDividerRequested);

    addSeparator();

    alignLeftBtn   = makeBtn("≡", "左对齐");
    alignCenterBtn = makeBtn("≡", "居中");
    alignRightBtn  = makeBtn("≡", "右对齐");
    addWidget(alignLeftBtn);
    addWidget(alignCenterBtn);
    addWidget(alignRightBtn);
    connect(alignLeftBtn,   &QPushButton::clicked, this, &RichTextToolBar::alignLeftRequested);
    connect(alignCenterBtn, &QPushButton::clicked, this, &RichTextToolBar::alignCenterRequested);
    connect(alignRightBtn,  &QPushButton::clicked, this, &RichTextToolBar::alignRightRequested);

    addSeparator();

    fontSizeBox = new QComboBox(this);
    fontSizeBox->addItems({"12", "14", "16", "18", "24", "32"});
    fontSizeBox->setCurrentText("14");
    fontSizeBox->setFixedWidth(56);
    fontSizeBox->setToolTip("字号");
    fontSizeBox->setStyleSheet(
        "QComboBox{border:1px solid #d0d0d0; border-radius:4px; padding:2px 4px;"
        " background:white; font-size:12px;}"
        "QComboBox:hover{border-color:#005fb8;}"
        "QComboBox::drop-down{border:none; width:16px;}");
    addWidget(fontSizeBox);
    connect(fontSizeBox, &QComboBox::currentTextChanged, this, [this](const QString& t) {
        emit fontSizeRequested(t.toInt());
    });

    bgColorBtn = makeBtn("A", "背景色",
        "QPushButton{text-decoration:underline; color:#005fb8;}");
    addWidget(bgColorBtn);
    connect(bgColorBtn, &QPushButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(Qt::white, this, "选择背景色");
        if (c.isValid()) emit backgroundColorRequested(c);
    });
}
