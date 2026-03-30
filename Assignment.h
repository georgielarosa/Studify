#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <QString>
#include <QDate>

class Assignment
{
public:
    enum class Status {
        NotStarted,
        InProgress,
        Completed
    };

    Assignment() = default;
    Assignment(const QString& title,
               const QString& courseName,
               const QDate& dueDate,
               Status status);

    const QString& title() const;
    const QString& courseName() const;
    const QDate& dueDate() const;
    Status status() const;
    bool completed() const;

    void setTitle(const QString& title);
    void setCourseName(const QString& courseName);
    void setDueDate(const QDate& dueDate);
    void setStatus(Status status);
    void markCompleted(bool completed);

    QString toDisplayString() const;

    static QString statusToString(Status status);
    static Status statusFromString(const QString& text);

private:
    QString m_title;
    QString m_courseName;
    QDate m_dueDate;
    Status m_status = Status::NotStarted;
};

#endif
