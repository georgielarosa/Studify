# UML Class Diagram

```mermaid
classDiagram
class Status {
  <<enumeration>>
  NotStarted
  InProgress
  Completed
}

class Assignment {
  -QString m_title
  -QString m_courseName
  -QDate m_dueDate
  -Status m_status
  +Assignment()
  +Assignment(title, courseName, dueDate, status)
  +QString title() const
  +QString courseName() const
  +QDate dueDate() const
  +Status status() const
  +bool completed() const
  +void setTitle(title)
  +void setCourseName(courseName)
  +void setDueDate(dueDate)
  +void setStatus(status)
  +void markCompleted(completed)
  +QString toDisplayString() const
  +QString statusToString(status)
  +Status statusFromString(text)
}

class PlannerManager {
  +enum SortMode { DueDate, CourseName }
  +enum FilterMode { All, Completed, Incomplete }
  -QVector~Assignment~ m_assignments
  -QMap~QString,QColor~ m_courseColors
  +bool addAssignment(assignment, error)
  +bool updateAssignment(index, assignment, error)
  +bool removeAssignment(index, error)
  +bool toggleCompleted(index, error)
  +void sortAssignments(mode)
  +QVector~Assignment~ filteredAssignments(mode) const
  +const QVector~Assignment~& assignments() const
  +QVector~Assignment~& assignments()
  +QColor courseColor(courseName) const
  +void setCourseColor(courseName, color)
  +void setCourseColors(colors)
  +const QMap~QString,QColor~& courseColors() const
  +bool loadFromFile(path, error)
  +bool saveToFile(path, error) const
  +static bool isDuplicate(a, b)
}

class FileHandler {
  +static bool saveAssignments(path, assignments, courseColors, error)
  +static bool loadAssignments(path, assignments, courseColors, error)
}

class MainWindow {
  -PlannerManager m_manager
  -QString m_dataPath
  +MainWindow(parent)
  +void closeEvent(event)
  +void onAddClicked()
  +void onDeleteClicked()
  +void onToggleCompletedClicked()
  +void onSaveClicked()
  +void onSortChanged()
  +void onFilterChanged()
  +void onSelectionChanged()
  +void onTableDoubleClicked(row,col)
  +void onQuickAdd()
  +void onThemeChanged()
  +void onCourseColors()
  -void buildUi()
  -void refreshTable()
  -Assignment readInputs(error) const
  -void autoSave()
  -int selectedRow() const
  -int rowToIndex(row) const
  -void ensureDataPath()
  -bool editAssignmentDialog(index)
  -bool quickAddDialog()
  -void selectRowByIndex(index)
  -void clearInputs()
  -void applyTheme(themeName)
}

PlannerManager o-- Assignment : manages
PlannerManager ..> FileHandler : load/save
FileHandler ..> Assignment : serialize
MainWindow --> PlannerManager : uses
Assignment --> Status : uses
```
