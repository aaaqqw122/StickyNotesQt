#include "rich_text_actions.h"
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>

RichTextActions::RichTextActions(QTextEdit* edit, QObject* parent)
    : QObject(parent), edit_(edit) {}

void RichTextActions::applyUnderline() {
    QTextCursor cursor = edit_->textCursor();
    QTextCharFormat fmt;
    fmt.setFontUnderline(true);
    cursor.mergeCharFormat(fmt);
}

void RichTextActions::insertDivider() {
    edit_->textCursor().insertHtml("<hr>");
}

void RichTextActions::alignLeft() {
    QTextBlockFormat fmt;
    fmt.setAlignment(Qt::AlignLeft);
    edit_->textCursor().mergeBlockFormat(fmt);
}

void RichTextActions::alignCenter() {
    QTextBlockFormat fmt;
    fmt.setAlignment(Qt::AlignCenter);
    edit_->textCursor().mergeBlockFormat(fmt);
}

void RichTextActions::alignRight() {
    QTextBlockFormat fmt;
    fmt.setAlignment(Qt::AlignRight);
    edit_->textCursor().mergeBlockFormat(fmt);
}

void RichTextActions::setFontSize(int size) {
    QTextCharFormat fmt;
    fmt.setFontPointSize(size);
    edit_->textCursor().mergeCharFormat(fmt);
}

void RichTextActions::setBackgroundColor(QColor color) {
    QTextCharFormat fmt;
    fmt.setBackground(color);
    edit_->textCursor().mergeCharFormat(fmt);
}
