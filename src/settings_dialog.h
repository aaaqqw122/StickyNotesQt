#pragma once
#include <QDialog>
#include <QListWidget>
#include <QSet>

// 启动设置弹窗：选择启动时自动弹出的便签
class StartupSettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit StartupSettingsDialog(QWidget* parent = nullptr);
    QList<int> selectedNoteIds() const;
};
