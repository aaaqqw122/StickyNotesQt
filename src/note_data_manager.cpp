#include "note_data_manager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

NoteDataManager& NoteDataManager::instance() {
    static NoteDataManager inst;
    return inst;
}

bool NoteDataManager::addNote(int id) {
    QMutexLocker locker(&mutex_);
    for (const auto& n : notes_) {
        if (n.id == id) return false;
    }
    NoteData nd;
    nd.id = id;
    notes_.push_back(nd);
    return true;
}

bool NoteDataManager::removeNote(int id) {
    QMutexLocker locker(&mutex_);
    auto it = std::remove_if(notes_.begin(), notes_.end(),
                             [id](const NoteData& n) { return n.id == id; });
    if (it == notes_.end()) return false;
    notes_.erase(it, notes_.end());
    return true;
}

void NoteDataManager::updateNote(int id, const NoteData& noteData) {
    QMutexLocker locker(&mutex_);
    for (auto& n : notes_) {
        if (n.id == id) {
            n = noteData;
            return;
        }
    }
}

NoteData NoteDataManager::getNoteById(int id) const {
    QMutexLocker locker(&mutex_);
    for (const auto& n : notes_) {
        if (n.id == id) return n;
    }
    NoteData empty;
    empty.id = id;
    return empty;
}

std::vector<NoteData> NoteDataManager::getAllNotes() {
    QMutexLocker locker(&mutex_);
    return notes_;
}

QList<int> NoteDataManager::getAllNoteIds() {
    QMutexLocker locker(&mutex_);
    QList<int> ids;
    for (const auto& n : notes_)
        ids.append(n.id);
    return ids;
}

int NoteDataManager::allocateId() {
    QMutexLocker locker(&mutex_);
    int id = 1;
    // 收集已用 ID
    QSet<int> used;
    for (const auto& n : notes_)
        used.insert(n.id);
    while (used.contains(id))
        ++id;
    return id;
}

void NoteDataManager::saveToFile(const QString& filePath) {
    QMutexLocker locker(&mutex_);
    QJsonArray arr;
    for (const auto& n : notes_)
        arr.append(n.toJson());

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        file.write(QJsonDocument(arr).toJson());
}

void NoteDataManager::loadFromFile(const QString& filePath) {
    QMutexLocker locker(&mutex_);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) return;

    notes_.clear();
    for (const auto& v : doc.array()) {
        if (v.isObject()) {
            NoteData nd;
            nd.fromJson(v.toObject());
            notes_.push_back(nd);
        }
    }
}
