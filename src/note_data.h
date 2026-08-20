#pragma once
#include <QString>
#include <QSize>
#include <QPoint>
#include <QJsonObject>
#include <QDateTime>

struct NoteData {
    int id = 0;
    QString content;           // 便签 HTML 内容
    QSize windowSize{400, 300};
    QPoint windowPos{100, 100};

    void fromJson(const QJsonObject& obj);
    QJsonObject toJson() const;
};
