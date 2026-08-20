#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>

class CustomTitleBar : public QWidget {
    Q_OBJECT
public:
    explicit CustomTitleBar(const QString& title = "便签", QWidget* parent = nullptr);

    void setTitle(const QString& text);

signals:
    void minimizeClicked();
    void closeClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QLabel* title_label;
    QPushButton* btn_minimize;
    QPushButton* btn_close;
    QPoint drag_pos;
    bool dragging = false;
};
