#ifndef PLANNERMANAGER_H
#define PLANNERMANAGER_H

#include "Assignment.h"
#include <QMap>
#include <QColor>
#include <QVector>

class PlannerManager
{
public:
    enum class SortMode {
        DueDate,
        CourseName
    };

    enum class FilterMode {
        All,
        Completed,
        Incomplete
    };

    bool addAssignment(const Assignment& assignment, QString* error = nullptr);
    bool updateAssignment(int index, const Assignment& assignment, QString* error = nullptr);
    bool removeAssignment(int index, QString* error = nullptr);
    bool toggleCompleted(int index, QString* error = nullptr);

    void sortAssignments(SortMode mode);
    QVector<Assignment> filteredAssignments(FilterMode mode) const;

    const QVector<Assignment>& assignments() const;
    QVector<Assignment>& assignments();
    QColor courseColor(const QString& courseName) const;
    void setCourseColor(const QString& courseName, const QColor& color);
    void setCourseColors(const QMap<QString, QColor>& colors);
    const QMap<QString, QColor>& courseColors() const;

    bool loadFromFile(const QString& path, QString* error = nullptr);
    bool saveToFile(const QString& path, QString* error = nullptr) const;

    static bool isDuplicate(const Assignment& a, const Assignment& b);

private:
    QVector<Assignment> m_assignments;
    QMap<QString, QColor> m_courseColors;
};

#endif
