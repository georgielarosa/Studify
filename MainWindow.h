#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

#include "PlannerManager.h"

class QLineEdit;
class QDateEdit;
class QComboBox;
class QPushButton;
class QTableWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onAddClicked();
    void onDeleteClicked();
    void onToggleCompletedClicked();
    void onSaveClicked();
    void onSortChanged();
    void onFilterChanged();
    void onSelectionChanged();
    void onTableDoubleClicked(int row, int column);
    void onQuickAdd();
    void onThemeChanged();
    void onCourseColors();

private:
    void buildUi();
    void refreshTable();
    void showError(const QString& message);
    void showInfo(const QString& message);
    Assignment readInputs(QString* error) const;
    void autoSave();
    int selectedRow() const;
    int rowToIndex(int row) const;
    void ensureDataPath();
    bool editAssignmentDialog(int index);
    bool quickAddDialog();
    void selectRowByIndex(int index);
    void clearInputs();
    void applyTheme(const QString& themeName);

    PlannerManager m_manager;
    QString m_dataPath;

    QLineEdit* m_titleEdit = nullptr;
    QLineEdit* m_courseEdit = nullptr;
    QDateEdit* m_dueDateEdit = nullptr;
    QComboBox* m_statusCombo = nullptr;
    QComboBox* m_sortCombo = nullptr;
    QComboBox* m_filterCombo = nullptr;
    QComboBox* m_themeCombo = nullptr;
    QPushButton* m_addButton = nullptr;
    QPushButton* m_deleteButton = nullptr;
    QPushButton* m_toggleButton = nullptr;
    QPushButton* m_saveButton = nullptr;
    QTableWidget* m_table = nullptr;
    QVector<int> m_rowToIndex;
};

#endif
