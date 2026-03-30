#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "Assignment.h"
#include <QMap>
#include <QColor>
#include <QVector>
#include <QString>

class FileHandler
{
public:
    static bool saveAssignments(const QString& path,
                                const QVector<Assignment>& assignments,
                                const QMap<QString, QColor>& courseColors,
                                QString* error = nullptr);
    static bool loadAssignments(const QString& path,
                                QVector<Assignment>& assignments,
                                QMap<QString, QColor>& courseColors,
                                QString* error = nullptr);
};

#endif
