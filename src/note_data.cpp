#include "note_data.h"

void NoteData::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt();
    content = obj["content"].toString();
    windowSize = QSize(obj["w"].toInt(400), obj["h"].toInt(300));
    windowPos  = QPoint(obj["x"].toInt(100), obj["y"].toInt(100));
}

QJsonObject NoteData::toJson() const {
    QJsonObject obj;
    obj["id"]      = id;
    obj["content"] = content;
    obj["w"]       = windowSize.width();
    obj["h"]       = windowSize.height();
    obj["x"]       = windowPos.x();
    obj["y"]       = windowPos.y();
    return obj;
}
