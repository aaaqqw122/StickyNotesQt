#pragma once
#include <QObject>
#include <QTextEdit>
#include <QColor>

class RichTextActions : public QObject {
    Q_OBJECT
public:
    explicit RichTextActions(QTextEdit* edit, QObject* parent = nullptr);

public slots:
    void applyUnderline();
    void insertDivider();
    void alignLeft();
    void alignCenter();
    void alignRight();
    void setFontSize(int size);
    void setBackgroundColor(QColor color);

private:
    QTextEdit* edit_;
};
