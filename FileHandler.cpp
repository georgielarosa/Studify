#include "FileHandler.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

bool FileHandler::saveAssignments(const QString& path,
                                  const QVector<Assignment>& assignments,
                                  const QMap<QString, QColor>& courseColors,
                                  QString* error)
{
    QJsonArray array;
    for (const Assignment& assignment : assignments) {
        QJsonObject obj;
        obj["title"] = assignment.title();
        obj["courseName"] = assignment.courseName();
        obj["dueDate"] = assignment.dueDate().toString("yyyy-MM-dd");
        obj["status"] = Assignment::statusToString(assignment.status());
        obj["completed"] = assignment.completed();
        array.append(obj);
    }

    QJsonObject root;
    root["assignments"] = array;

    QJsonObject colorObj;
    for (auto it = courseColors.begin(); it != courseColors.end(); ++it) {
        if (it.value().isValid()) {
            colorObj[it.key()] = it.value().name(QColor::HexRgb);
        }
    }
    root["courseColors"] = colorObj;

    QJsonDocument doc(root);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        if (error) {
            *error = "Unable to open save file.";
        }
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool FileHandler::loadAssignments(const QString& path,
                                  QVector<Assignment>& assignments,
                                  QMap<QString, QColor>& courseColors,
                                  QString* error)
{
    QFile file(path);
    if (!file.exists()) {
        assignments.clear();
        courseColors.clear();
        return true;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        if (error) {
            *error = "Unable to open save file.";
        }
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (error) {
            *error = "Save file is corrupted or invalid JSON.";
        }
        return false;
    }

    QVector<Assignment> loaded;
    QJsonArray array;
    QJsonObject colorsObj;
    if (doc.isArray()) {
        array = doc.array();
    } else if (doc.isObject()) {
        QJsonObject root = doc.object();
        array = root.value("assignments").toArray();
        colorsObj = root.value("courseColors").toObject();
    } else {
        if (error) {
            *error = "Save file is corrupted or invalid JSON.";
        }
        return false;
    }

    for (const QJsonValue& value : array) {
        if (!value.isObject()) {
            continue;
        }
        QJsonObject obj = value.toObject();
        QString title = obj.value("title").toString();
        QString courseName = obj.value("courseName").toString();
        QDate dueDate = QDate::fromString(obj.value("dueDate").toString(), "yyyy-MM-dd");
        QString statusText = obj.value("status").toString();
        bool completed = obj.value("completed").toBool(false);
        Assignment::Status status = Assignment::statusFromString(statusText);
        if (statusText.trimmed().isEmpty()) {
            status = completed ? Assignment::Status::Completed : Assignment::Status::NotStarted;
        }

        if (title.trimmed().isEmpty() || courseName.trimmed().isEmpty() || !dueDate.isValid()) {
            continue;
        }
        loaded.push_back(Assignment(title, courseName, dueDate, status));
    }

    assignments = loaded;
    courseColors.clear();
    for (auto it = colorsObj.begin(); it != colorsObj.end(); ++it) {
        QColor color(it.value().toString());
        if (color.isValid()) {
            courseColors[it.key()] = color;
        }
    }
    return true;
}
