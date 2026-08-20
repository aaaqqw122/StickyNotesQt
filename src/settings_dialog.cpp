#include "settings_dialog.h"
#include "note_data_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSettings>
#include <QTextDocument>

StartupSettingsDialog::StartupSettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("启动设置");
    setFixedSize(320, 380);
    setStyleSheet(
        "QDialog{background:#f0f0f0; border-radius:8px;}"
        "QLabel{font-size:14px; font-weight:600; color:#1a1a1a;}"
        "QCheckBox{font-size:13px; padding:4px 0;}"
        "QCheckBox::indicator{width:18px; height:18px; border-radius:3px;"
        " border:2px solid #aaa; background:white;}"
        "QCheckBox::indicator:checked{background:#0078d4; border-color:#0078d4;}"
        // 统一按钮风格
        "QPushButton#PrimaryBtn{background:#0078d4; color:white;"
        " border:none; border-radius:4px; padding:6px 20px; font-weight:600;}"
        "QPushButton#PrimaryBtn:hover{background:#106ebe;}"
        "QPushButton{background:transparent; color:#555;"
        " border:1px solid #ccc; border-radius:4px; padding:6px 20px;}"
        "QPushButton:hover{background:#e5e5e5;}");

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(10);

    auto* title = new QLabel("启动时弹出哪些便签？");
    layout->addWidget(title);

    auto* noteList = new QListWidget(this);
    noteList->setStyleSheet(
        "QListWidget{border:1px solid #ddd; border-radius:6px;"
        " background:white; padding:4px;}"
        "QListWidget::item{padding:2px;}");

    QSettings settings("MyCompany", "StickyNotesQt");
    QStringList savedIds = settings.value("startup_note_ids", "").toString().split(",",
        Qt::SkipEmptyParts);
    QSet<QString> selected(savedIds.begin(), savedIds.end());

    auto notes = NoteDataManager::instance().getAllNotes();
    for (const auto& n : notes) {
        QTextDocument doc;
        doc.setHtml(n.content);
        QString preview = doc.toPlainText().left(25).simplified();
        if (preview.isEmpty()) preview = "(空便签)";

        auto* cb = new QCheckBox(QString("  [%1] %2").arg(n.id).arg(preview));
        cb->setChecked(selected.contains(QString::number(n.id)));
        cb->setProperty("noteId", n.id);

        auto* item = new QListWidgetItem();
        noteList->addItem(item);
        noteList->setItemWidget(item, cb);
    }

    layout->addWidget(noteList, 1);

    auto* btnLayout = new QHBoxLayout();
    auto* okBtn = new QPushButton("确定");
    auto* cancelBtn = new QPushButton("取消");
    okBtn->setObjectName("PrimaryBtn");
    okBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setCursor(Qt::PointingHandCursor);

    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(okBtn);
    layout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, this, [this, noteList]() {
        QStringList ids;
        for (int i = 0; i < noteList->count(); ++i) {
            auto* w = noteList->itemWidget(noteList->item(i));
            if (auto* cb = qobject_cast<QCheckBox*>(w)) {
                if (cb->isChecked()) {
                    ids.append(cb->property("noteId").toString());
                }
            }
        }
        QSettings settings("MyCompany", "StickyNotesQt");
        settings.setValue("startup_note_ids", ids.join(","));
        accept();
    });
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}
