#include "Assignment.h"

Assignment::Assignment(const QString& title,
                       const QString& courseName,
                       const QDate& dueDate,
                       Status status)
    : m_title(title),
      m_courseName(courseName),
      m_dueDate(dueDate),
      m_status(status)
{
}

const QString& Assignment::title() const
{
    return m_title;
}

const QString& Assignment::courseName() const
{
    return m_courseName;
}

const QDate& Assignment::dueDate() const
{
    return m_dueDate;
}

Assignment::Status Assignment::status() const
{
    return m_status;
}

bool Assignment::completed() const
{
    return m_status == Status::Completed;
}

void Assignment::setTitle(const QString& title)
{
    m_title = title;
}

void Assignment::setCourseName(const QString& courseName)
{
    m_courseName = courseName;
}

void Assignment::setDueDate(const QDate& dueDate)
{
    m_dueDate = dueDate;
}

void Assignment::setStatus(Status status)
{
    m_status = status;
}

void Assignment::markCompleted(bool completed)
{
    m_status = completed ? Status::Completed : Status::NotStarted;
}

QString Assignment::toDisplayString() const
{
    return QString("%1 (%2) - Due: %3 - %4")
        .arg(m_title)
        .arg(m_courseName)
        .arg(m_dueDate.toString("yyyy-MM-dd"))
        .arg(statusToString(m_status));
}

QString Assignment::statusToString(Status status)
{
    switch (status) {
    case Status::NotStarted:
        return "Not started";
    case Status::InProgress:
        return "In progress";
    case Status::Completed:
        return "Completed";
    }
    return "Not started";
}

Assignment::Status Assignment::statusFromString(const QString& text)
{
    QString normalized = text.trimmed().toLower();
    if (normalized == "in progress") {
        return Status::InProgress;
    }
    if (normalized == "completed") {
        return Status::Completed;
    }
    return Status::NotStarted;
}
