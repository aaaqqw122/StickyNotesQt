#pragma once
#include "note_data.h"
#include <vector>
#include <QString>
#include <QMutex>
#include <QList>

class NoteDataManager {
public:
    static NoteDataManager& instance();

    bool addNote(int id);
    bool removeNote(int id);
    void updateNote(int id, const NoteData& noteData);
    NoteData getNoteById(int id) const;

    std::vector<NoteData> getAllNotes();
    QList<int> getAllNoteIds();
    int allocateId();

    void saveToFile(const QString& filePath);
    void loadFromFile(const QString& filePath);

private:
    NoteDataManager() = default;
    ~NoteDataManager() = default;
    NoteDataManager(const NoteDataManager&) = delete;
    NoteDataManager& operator=(const NoteDataManager&) = delete;

    std::vector<NoteData> notes_;
    mutable QMutex mutex_;
};
