#include "PlannerManager.h"
#include "FileHandler.h"
#include <algorithm>

bool PlannerManager::addAssignment(const Assignment& assignment, QString* error)
{
    for (const Assignment& existing : m_assignments) {
        if (isDuplicate(existing, assignment)) {
            if (error) {
                *error = "An assignment with the same title, course, and due date already exists.";
            }
            return false;
        }
    }
    m_assignments.push_back(assignment);
    return true;
}

bool PlannerManager::updateAssignment(int index, const Assignment& assignment, QString* error)
{
    if (index < 0 || index >= m_assignments.size()) {
        if (error) {
            *error = "Invalid selection.";
        }
        return false;
    }

    for (int i = 0; i < m_assignments.size(); ++i) {
        if (i == index) {
            continue;
        }
        if (isDuplicate(m_assignments[i], assignment)) {
            if (error) {
                *error = "Updating would create a duplicate assignment.";
            }
            return false;
        }
    }

    m_assignments[index] = assignment;
    return true;
}

bool PlannerManager::removeAssignment(int index, QString* error)
{
    if (index < 0 || index >= m_assignments.size()) {
        if (error) {
            *error = "Invalid selection.";
        }
        return false;
    }
    m_assignments.removeAt(index);
    return true;
}

bool PlannerManager::toggleCompleted(int index, QString* error)
{
    if (index < 0 || index >= m_assignments.size()) {
        if (error) {
            *error = "Invalid selection.";
        }
        return false;
    }
    Assignment::Status current = m_assignments[index].status();
    if (current == Assignment::Status::Completed) {
        m_assignments[index].setStatus(Assignment::Status::NotStarted);
    } else {
        m_assignments[index].setStatus(Assignment::Status::Completed);
    }
    return true;
}

void PlannerManager::sortAssignments(SortMode mode)
{
    std::sort(m_assignments.begin(), m_assignments.end(),
              [mode](const Assignment& a, const Assignment& b) {
                  switch (mode) {
                  case SortMode::DueDate:
                      return a.dueDate() < b.dueDate();
                  case SortMode::CourseName:
                      return a.courseName().toLower() < b.courseName().toLower();
                  }
                  return false;
              });
}

QVector<Assignment> PlannerManager::filteredAssignments(FilterMode mode) const
{
    if (mode == FilterMode::All) {
        return m_assignments;
    }
    QVector<Assignment> result;
    for (const Assignment& assignment : m_assignments) {
        if (mode == FilterMode::Completed && assignment.completed()) {
            result.push_back(assignment);
        } else if (mode == FilterMode::Incomplete && !assignment.completed()) {
            result.push_back(assignment);
        }
    }
    return result;
}

const QVector<Assignment>& PlannerManager::assignments() const
{
    return m_assignments;
}

QVector<Assignment>& PlannerManager::assignments()
{
    return m_assignments;
}

QColor PlannerManager::courseColor(const QString& courseName) const
{
    return m_courseColors.value(courseName.trimmed());
}

void PlannerManager::setCourseColor(const QString& courseName, const QColor& color)
{
    QString key = courseName.trimmed();
    if (key.isEmpty()) {
        return;
    }
    if (color.isValid()) {
        m_courseColors[key] = color;
    } else {
        m_courseColors.remove(key);
    }
}

void PlannerManager::setCourseColors(const QMap<QString, QColor>& colors)
{
    m_courseColors = colors;
}

const QMap<QString, QColor>& PlannerManager::courseColors() const
{
    return m_courseColors;
}

bool PlannerManager::loadFromFile(const QString& path, QString* error)
{
    return FileHandler::loadAssignments(path, m_assignments, m_courseColors, error);
}

bool PlannerManager::saveToFile(const QString& path, QString* error) const
{
    return FileHandler::saveAssignments(path, m_assignments, m_courseColors, error);
}

bool PlannerManager::isDuplicate(const Assignment& a, const Assignment& b)
{
    return a.title().compare(b.title(), Qt::CaseInsensitive) == 0
        && a.courseName().compare(b.courseName(), Qt::CaseInsensitive) == 0
        && a.dueDate() == b.dueDate();
}
