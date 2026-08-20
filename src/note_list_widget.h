#pragma once
#include <QWidget>
#include <QListWidget>
#include <QString>
#include <QSlider>
#include <QLabel>

class NoteListWidget : public QWidget {
    Q_OBJECT
public:
    explicit NoteListWidget(QWidget* parent = nullptr);

public slots:
    void refreshNotes();

signals:
    void noteItemClicked(int noteId);
    void createNoteRequested();
    void refreshNotesRequested();
    void noteDeleted(int noteId);
    void glassIntensityChanged(int alpha);
    void backgroundImageRequested();
    void settingsRequested();

private slots:
    void onNoteItemClicked(QListWidgetItem* item);
    void onGlassSliderChanged(int value);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void onNotesListContextMenu(const QPoint& pos);

private:
    void closeSidebar();

private:
    QListWidget* list;
    QSlider* glassSlider;
    QLabel* glassLabel;
    QWidget* sidebar;
};
