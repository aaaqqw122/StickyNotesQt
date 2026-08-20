#pragma once
#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QComboBox>
#include <QColor>

class RichTextToolBar : public QToolBar {
    Q_OBJECT
public:
    explicit RichTextToolBar(QWidget* parent = nullptr);

signals:
    void underlineRequested();
    void insertDividerRequested();
    void alignLeftRequested();
    void alignCenterRequested();
    void alignRightRequested();
    void fontSizeRequested(int);
    void backgroundColorRequested(QColor);

private:
    QPushButton* underlineBtn;
    QPushButton* dividerBtn;
    QPushButton* alignLeftBtn;
    QPushButton* alignCenterBtn;
    QPushButton* alignRightBtn;
    QComboBox* fontSizeBox;
    QPushButton* bgColorBtn;
};
