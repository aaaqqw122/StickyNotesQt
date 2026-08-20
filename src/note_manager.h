#pragma once
#include <QStackedWidget>
#include <QPushButton>
#include <QShowEvent>
#include <QPixmap>
#include <QString>
#include <QPaintEvent>
#include "note_widget.h"
#include "custom_title_bar.h"

class NoteListWidget;

class NoteManager : public QWidget {
    Q_OBJECT
public:
    explicit NoteManager(QWidget* parent = nullptr);
    NoteListWidget* getNoteListWidget() const { return notesPage; }
    void setBackgroundImage(const QString& path);

protected:
    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onCreateNoteRequested();
    void onNoteItemClicked(int noteId);

private:
    QStackedWidget* stack;
    NoteListWidget* notesPage;
    NoteWidget* currentNoteWidget = nullptr;
    CustomTitleBar* custom_title_bar;
    QPixmap bgPixmap;
    QString bgImagePath;
};
