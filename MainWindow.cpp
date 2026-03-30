#include "MainWindow.h"

#include <QCloseEvent>
#include <QDateEdit>
#include <QFileInfo>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QAction>
#include <QColorDialog>
#include <QToolBar>
#include <QStyle>
#include <QSize>
#include <QStandardPaths>
#include <QDir>
#include <QSet>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    buildUi();
    ensureDataPath();

    QString error;
    if (!m_manager.loadFromFile(m_dataPath, &error)) {
        showError(error);
    }
    refreshTable();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    autoSave();
    event->accept();
}

void MainWindow::buildUi()
{
    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    QHBoxLayout* inputLayout = new QHBoxLayout();

    m_titleEdit = new QLineEdit();
    m_courseEdit = new QLineEdit();
    m_dueDateEdit = new QDateEdit(QDate::currentDate());
    m_dueDateEdit->setCalendarPopup(true);
    m_dueDateEdit->setDisplayFormat("yyyy-MM-dd");

    m_statusCombo = new QComboBox();
    m_statusCombo->addItem("Not started");
    m_statusCombo->addItem("In progress");
    m_statusCombo->addItem("Completed");

    inputLayout->addWidget(new QLabel("Title"));
    inputLayout->addWidget(m_titleEdit);
    inputLayout->addWidget(new QLabel("Course"));
    inputLayout->addWidget(m_courseEdit);
    inputLayout->addWidget(new QLabel("Due Date"));
    inputLayout->addWidget(m_dueDateEdit);
    inputLayout->addWidget(new QLabel("Status"));
    inputLayout->addWidget(m_statusCombo);

    mainLayout->addLayout(inputLayout);

    QToolBar* toolbar = addToolBar("Actions");
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(18, 18));

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("Add");
    m_deleteButton = new QPushButton("Delete");
    m_toggleButton = new QPushButton("Mark Completed");
    m_saveButton = new QPushButton("Save");

    QAction* addAct = new QAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder), "Add", this);
    QAction* quickAddAction = new QAction(style()->standardIcon(QStyle::SP_ArrowRight), "Quick Add", this);
    QAction* deleteAct = new QAction(style()->standardIcon(QStyle::SP_TrashIcon), "Delete", this);
    QAction* toggleAct = new QAction(style()->standardIcon(QStyle::SP_DialogApplyButton), "Mark Completed", this);
    QAction* saveAct = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Save", this);
    QAction* colorAct = new QAction(style()->standardIcon(QStyle::SP_DriveCDIcon), "Course Colors", this);

    toolbar->addAction(addAct);
    toolbar->addAction(quickAddAction);
    toolbar->addSeparator();
    toolbar->addAction(deleteAct);
    toolbar->addAction(toggleAct);
    toolbar->addSeparator();
    toolbar->addAction(saveAct);
    toolbar->addSeparator();
    toolbar->addAction(colorAct);

    m_addButton->setIcon(addAct->icon());
    m_deleteButton->setIcon(deleteAct->icon());
    m_toggleButton->setIcon(toggleAct->icon());
    m_saveButton->setIcon(saveAct->icon());

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addWidget(m_toggleButton);
    buttonLayout->addWidget(m_saveButton);

    mainLayout->addLayout(buttonLayout);

    QHBoxLayout* sortFilterLayout = new QHBoxLayout();
    m_sortCombo = new QComboBox();
    m_sortCombo->addItem("Sort by Due Date");
    m_sortCombo->addItem("Sort by Course");

    m_filterCombo = new QComboBox();
    m_filterCombo->addItem("All");
    m_filterCombo->addItem("Completed");
    m_filterCombo->addItem("Incomplete");

    m_themeCombo = new QComboBox();
    m_themeCombo->addItem("Light");
    m_themeCombo->addItem("Dark");
    m_themeCombo->addItem("Cyberpunk");
    m_themeCombo->addItem("Excel");

    sortFilterLayout->addWidget(new QLabel("Sort"));
    sortFilterLayout->addWidget(m_sortCombo);
    sortFilterLayout->addWidget(new QLabel("Filter"));
    sortFilterLayout->addWidget(m_filterCombo);
    sortFilterLayout->addWidget(new QLabel("Theme"));
    sortFilterLayout->addWidget(m_themeCombo);
    sortFilterLayout->addStretch();

    mainLayout->addLayout(sortFilterLayout);

    m_table = new QTableWidget(0, 5);
    m_table->setHorizontalHeaderLabels({"Title", "Course", "Due Date", "Due In", "Status"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);

    mainLayout->addWidget(m_table);

    setCentralWidget(central);
    setWindowTitle("Study Planner and Assignment Tracker");
    resize(1000, 600);

    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(m_toggleButton, &QPushButton::clicked, this, &MainWindow::onToggleCompletedClicked);
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(addAct, &QAction::triggered, this, &MainWindow::onAddClicked);
    connect(deleteAct, &QAction::triggered, this, &MainWindow::onDeleteClicked);
    connect(toggleAct, &QAction::triggered, this, &MainWindow::onToggleCompletedClicked);
    connect(saveAct, &QAction::triggered, this, &MainWindow::onSaveClicked);
    connect(quickAddAction, &QAction::triggered, this, &MainWindow::onQuickAdd);
    connect(colorAct, &QAction::triggered, this, &MainWindow::onCourseColors);
    connect(m_sortCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onSortChanged);
    connect(m_filterCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onFilterChanged);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, &MainWindow::onSelectionChanged);
    connect(m_table, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableDoubleClicked);
    connect(m_themeCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onThemeChanged);

    quickAddAction->setShortcut(QKeySequence::New);
    addAction(quickAddAction);

    applyTheme(m_themeCombo->currentText());
}

void MainWindow::onAddClicked()
{
    QString error;
    Assignment assignment = readInputs(&error);
    if (!error.isEmpty()) {
        showError(error);
        return;
    }

    if (!m_manager.addAssignment(assignment, &error)) {
        showError(error);
        return;
    }

    autoSave();
    refreshTable();
    clearInputs();
}


void MainWindow::onDeleteClicked()
{
    int row = selectedRow();
    if (row < 0) {
        showInfo("Select an assignment to delete.");
        return;
    }

    int index = rowToIndex(row);
    QString error;
    if (!m_manager.removeAssignment(index, &error)) {
        showError(error);
        return;
    }

    autoSave();
    refreshTable();
}

void MainWindow::onToggleCompletedClicked()
{
    int row = selectedRow();
    if (row < 0) {
        showInfo("Select an assignment to mark completed.");
        return;
    }

    int index = rowToIndex(row);
    QString error;
    if (!m_manager.toggleCompleted(index, &error)) {
        showError(error);
        return;
    }

    autoSave();
    refreshTable();
}

void MainWindow::onSaveClicked()
{
    autoSave();
    showInfo("Assignments saved.");
}

void MainWindow::onSortChanged()
{
    PlannerManager::SortMode mode = PlannerManager::SortMode::DueDate;
    if (m_sortCombo->currentIndex() == 1) {
        mode = PlannerManager::SortMode::CourseName;
    }
    m_manager.sortAssignments(mode);
    refreshTable();
}

void MainWindow::onFilterChanged()
{
    refreshTable();
}

void MainWindow::onSelectionChanged()
{
    // Keep inputs blank when selecting rows; edits happen on double-click only.
    clearInputs();
}

void MainWindow::onTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    int index = rowToIndex(row);
    if (index >= 0) {
        editAssignmentDialog(index);
    }
}

void MainWindow::onQuickAdd()
{
    quickAddDialog();
}

void MainWindow::onThemeChanged()
{
    applyTheme(m_themeCombo->currentText());
}

void MainWindow::onCourseColors()
{
    QSet<QString> courseSet;
    for (const Assignment& assignment : m_manager.assignments()) {
        courseSet.insert(assignment.courseName());
    }
    if (courseSet.isEmpty()) {
        showInfo("No courses available yet.");
        return;
    }

    QStringList courses = courseSet.values();
    courses.sort(Qt::CaseInsensitive);

    QMap<QString, QColor> pendingColors = m_manager.courseColors();

    QDialog dialog(this);
    dialog.setWindowTitle("Course Colors");
    dialog.setStyleSheet(qApp->styleSheet());

    QVBoxLayout mainLayout(&dialog);
    QLabel info("Pick a color for each course (Auto uses default).");
    mainLayout.addWidget(&info);

    QFormLayout form;
    mainLayout.addLayout(&form);

    struct ColorOption {
        QString name;
        QColor color;
    };
    const QVector<ColorOption> palette = {
        {"Blue", QColor("#9ec5ff")},
        {"Green", QColor("#a7e8b5")},
        {"Orange", QColor("#ffd3a5")},
        {"Purple", QColor("#d6b5ff")},
        {"Pink", QColor("#ffb3d9")},
        {"Teal", QColor("#9fe7e7")},
        {"Gray", QColor("#d6d6d6")}
    };

    for (const QString& course : courses) {
        QComboBox* combo = new QComboBox(&dialog);
        combo->addItem("Auto");
        combo->setItemData(0, QColor(), Qt::UserRole);

        for (const ColorOption& option : palette) {
            combo->addItem(option.name);
            int idx = combo->count() - 1;
            combo->setItemData(idx, option.color, Qt::UserRole);
            combo->setItemData(idx, option.color, Qt::DecorationRole);
        }

        combo->addItem("Custom...");
        combo->setItemData(combo->count() - 1, true, Qt::UserRole + 1);

        QColor current = pendingColors.value(course);
        if (current.isValid()) {
            int match = -1;
            for (int i = 0; i < combo->count(); ++i) {
                QColor c = combo->itemData(i, Qt::UserRole).value<QColor>();
                if (c.isValid() && c == current) {
                    match = i;
                    break;
                }
            }
            if (match >= 0) {
                combo->setCurrentIndex(match);
            } else {
                int customIndex = combo->count() - 1;
                combo->insertItem(customIndex, "Custom");
                combo->setItemData(customIndex, current, Qt::UserRole);
                combo->setItemData(customIndex, current, Qt::DecorationRole);
                combo->setItemData(customIndex, true, Qt::UserRole + 2);
                combo->setCurrentIndex(customIndex);
            }
        } else {
            combo->setCurrentIndex(0);
        }

        combo->setProperty("course", course);
        combo->setProperty("lastIndex", combo->currentIndex());

        connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), &dialog, [&, combo](int idx) {
            QString courseName = combo->property("course").toString();
            bool isCustomRequest = combo->itemData(idx, Qt::UserRole + 1).toBool();
            if (isCustomRequest) {
                int prev = combo->property("lastIndex").toInt();
                QColor start = pendingColors.value(courseName);
                QColor picked = QColorDialog::getColor(start.isValid() ? start : QColor("#ffffff"),
                                                       &dialog,
                                                       "Pick a color");
                if (!picked.isValid()) {
                    combo->setCurrentIndex(prev);
                    return;
                }
                int customValueIndex = -1;
                for (int i = 0; i < combo->count(); ++i) {
                    if (combo->itemData(i, Qt::UserRole + 2).toBool()) {
                        customValueIndex = i;
                        break;
                    }
                }
                if (customValueIndex < 0) {
                    customValueIndex = combo->count() - 1;
                    combo->insertItem(customValueIndex, "Custom");
                    combo->setItemData(customValueIndex, true, Qt::UserRole + 2);
                }
                combo->setItemData(customValueIndex, picked, Qt::UserRole);
                combo->setItemData(customValueIndex, picked, Qt::DecorationRole);
                combo->setCurrentIndex(customValueIndex);
                pendingColors[courseName] = picked;
                combo->setProperty("lastIndex", customValueIndex);
                return;
            }

            QColor selected = combo->itemData(idx, Qt::UserRole).value<QColor>();
            if (!selected.isValid()) {
                pendingColors.remove(courseName);
            } else {
                pendingColors[courseName] = selected;
            }
            combo->setProperty("lastIndex", idx);
        });

        form.addRow(course, combo);
    }

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout.addWidget(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        m_manager.setCourseColors(pendingColors);
        autoSave();
        refreshTable();
    }
}

void MainWindow::refreshTable()
{
    m_table->setRowCount(0);
    m_rowToIndex.clear();

    PlannerManager::FilterMode filterMode = PlannerManager::FilterMode::All;
    if (m_filterCombo->currentIndex() == 1) {
        filterMode = PlannerManager::FilterMode::Completed;
    } else if (m_filterCombo->currentIndex() == 2) {
        filterMode = PlannerManager::FilterMode::Incomplete;
    }

    const QVector<Assignment>& assignments = m_manager.assignments();
    for (int i = 0; i < assignments.size(); ++i) {
        const Assignment& assignment = assignments[i];
        if (filterMode == PlannerManager::FilterMode::Completed && !assignment.completed()) {
            continue;
        }
        if (filterMode == PlannerManager::FilterMode::Incomplete && assignment.completed()) {
            continue;
        }

        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(assignment.title()));
        m_table->setItem(row, 1, new QTableWidgetItem(assignment.courseName()));
        m_table->setItem(row, 2, new QTableWidgetItem(assignment.dueDate().toString("yyyy-MM-dd")));
        QDate today = QDate::currentDate();
        int daysLeft = today.daysTo(assignment.dueDate());
        QString dueInText;
        if (assignment.completed()) {
            dueInText = "-";
        } else if (daysLeft < 0) {
            dueInText = QString("Overdue %1").arg(-daysLeft);
        } else if (daysLeft == 0) {
            dueInText = "Due today";
        } else {
            dueInText = QString("Due in %1").arg(daysLeft);
        }
        m_table->setItem(row, 3, new QTableWidgetItem(dueInText));
        m_table->setItem(row, 4, new QTableWidgetItem(Assignment::statusToString(assignment.status())));

        QColor courseColor = m_manager.courseColor(assignment.courseName());
        if (!courseColor.isValid()) {
            courseColor = QColor::fromHsl(qHash(assignment.courseName().toLower()) % 360, 80, 210);
        }
        if (QTableWidgetItem* courseItem = m_table->item(row, 1)) {
            courseItem->setBackground(courseColor);
            int luminance = (courseColor.red() * 299 + courseColor.green() * 587 + courseColor.blue() * 114) / 1000;
            courseItem->setForeground(luminance > 140 ? Qt::black : Qt::white);
        }

        m_rowToIndex.push_back(i);
    }
}


void MainWindow::showError(const QString& message)
{
    QMessageBox box(this);
    box.setIcon(QMessageBox::Critical);
    box.setWindowTitle("Error");
    box.setText(message);
    box.setStandardButtons(QMessageBox::Ok);
    box.setOption(QMessageBox::Option::DontUseNativeDialog, true);
    box.setStyleSheet(qApp->styleSheet());
    box.exec();
}

void MainWindow::showInfo(const QString& message)
{
    QMessageBox box(this);
    box.setIcon(QMessageBox::Information);
    box.setWindowTitle("Info");
    box.setText(message);
    box.setStandardButtons(QMessageBox::Ok);
    box.setOption(QMessageBox::Option::DontUseNativeDialog, true);
    box.setStyleSheet(qApp->styleSheet());
    box.exec();
}

Assignment MainWindow::readInputs(QString* error) const
{
    QString title = m_titleEdit->text().trimmed();
    QString course = m_courseEdit->text().trimmed();
    QDate dueDate = m_dueDateEdit->date();
    QString statusText = m_statusCombo->currentText();
    Assignment::Status status = Assignment::statusFromString(statusText);

    if (title.isEmpty() || course.isEmpty()) {
        if (error) {
            *error = "Title and course cannot be blank.";
        }
        return Assignment();
    }
    if (!dueDate.isValid()) {
        if (error) {
            *error = "Due date is invalid.";
        }
        return Assignment();
    }

    return Assignment(title, course, dueDate, status);
}

void MainWindow::autoSave()
{
    QString error;
    if (!m_manager.saveToFile(m_dataPath, &error)) {
        showError(error);
    }
}

int MainWindow::selectedRow() const
{
    QList<QTableWidgetSelectionRange> ranges = m_table->selectedRanges();
    if (ranges.isEmpty()) {
        return -1;
    }
    return ranges.first().topRow();
}

int MainWindow::rowToIndex(int row) const
{
    if (row < 0 || row >= m_rowToIndex.size()) {
        return -1;
    }
    return m_rowToIndex[row];
}

void MainWindow::ensureDataPath()
{
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(baseDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_dataPath = dir.filePath("assignments.json");
}

void MainWindow::clearInputs()
{
    m_titleEdit->clear();
    m_courseEdit->clear();
    m_dueDateEdit->setDate(QDate::currentDate());
    m_statusCombo->setCurrentIndex(0);
}

void MainWindow::applyTheme(const QString& themeName)
{
    QString theme;
    if (themeName == "Dark") {
        theme = R"(
            QMainWindow { background-color: #1e1e1e; color: #f1f1f1; }
            QWidget { color: #f1f1f1; }
            QDialog, QMessageBox { background-color: #1e1e1e; color: #f1f1f1; }
            QLabel { color: #f1f1f1; }
            QLineEdit, QDateEdit, QComboBox, QTableWidget {
                background-color: #2b2b2b; color: #f1f1f1; border: 1px solid #3a3a3a;
                selection-background-color: #4b6eaf; selection-color: #ffffff;
            }
            QCalendarWidget { background-color: #1e1e1e; color: #f1f1f1; }
            QCalendarWidget QAbstractItemView {
                background-color: #2b2b2b; color: #f1f1f1;
                selection-background-color: #4b6eaf; selection-color: #ffffff;
            }
            QCalendarWidget QToolButton { color: #f1f1f1; background-color: #2b2b2b; }
            QHeaderView::section { background-color: #333333; color: #f1f1f1; border: 1px solid #3a3a3a; }
            QPushButton { background-color: #333333; color: #f1f1f1; border: 1px solid #444444; padding: 4px 10px; }
            QPushButton:hover { background-color: #3b3b3b; }
            QToolBar { background-color: #252525; border-bottom: 1px solid #3a3a3a; }
        )";
    } else if (themeName == "Cyberpunk") {
        theme = R"(
            QMainWindow { background-color: #0b0b16; color: #f8f8f8; }
            QWidget { color: #f8f8f8; }
            QDialog, QMessageBox { background-color: #0b0b16; color: #f8f8f8; }
            QLabel { color: #f8f8f8; }
            QLineEdit, QDateEdit, QComboBox, QTableWidget {
                background-color: #141428; color: #f8f8f8; border: 1px solid #2c2c52;
                selection-background-color: #ff2bd6; selection-color: #0b0b16;
            }
            QCalendarWidget { background-color: #0b0b16; color: #f8f8f8; }
            QCalendarWidget QAbstractItemView {
                background-color: #141428; color: #f8f8f8;
                selection-background-color: #ff2bd6; selection-color: #0b0b16;
            }
            QCalendarWidget QToolButton { color: #00f6ff; background-color: #141428; }
            QHeaderView::section { background-color: #1b1b33; color: #00f6ff; border: 1px solid #2c2c52; }
            QPushButton { background-color: #1b1b33; color: #00f6ff; border: 1px solid #ff2bd6; padding: 4px 10px; }
            QPushButton:hover { background-color: #25254a; }
            QToolBar { background-color: #0f0f1f; border-bottom: 1px solid #2c2c52; }
        )";
    } else if (themeName == "Excel") {
        theme = R"(
            QMainWindow { background-color: #f7fbf9; color: #1f1f1f; }
            QWidget { color: #1f1f1f; }
            QDialog, QMessageBox { background-color: #f7fbf9; color: #1f1f1f; }
            QLabel { color: #1f1f1f; }
            QLineEdit, QDateEdit, QComboBox, QTableWidget {
                background-color: #ffffff; color: #1f1f1f; border: 1px solid #cfd8d3;
                selection-background-color: #b7e1cd; selection-color: #1f1f1f;
            }
            QCalendarWidget { background-color: #f7fbf9; color: #1f1f1f; }
            QCalendarWidget QAbstractItemView {
                background-color: #ffffff; color: #1f1f1f;
                selection-background-color: #b7e1cd; selection-color: #1f1f1f;
            }
            QCalendarWidget QToolButton { color: #1d6f42; background-color: #ffffff; }
            QHeaderView::section { background-color: #1d6f42; color: #ffffff; border: 1px solid #155a34; }
            QPushButton { background-color: #1d6f42; color: #ffffff; border: 1px solid #155a34; padding: 4px 10px; }
            QPushButton:hover { background-color: #218a4f; }
            QToolBar { background-color: #e3f1e9; border-bottom: 1px solid #cfd8d3; }
        )";
    } else {
        theme = R"(
            QMainWindow { background-color: #f5f5f5; color: #1f1f1f; }
            QWidget { color: #1f1f1f; }
            QDialog, QMessageBox { background-color: #f5f5f5; color: #1f1f1f; }
            QLabel { color: #1f1f1f; }
            QLineEdit, QDateEdit, QComboBox, QTableWidget {
                background-color: #ffffff; color: #1f1f1f; border: 1px solid #cfcfcf;
                selection-background-color: #cfe4ff; selection-color: #1f1f1f;
            }
            QCalendarWidget { background-color: #f5f5f5; color: #1f1f1f; }
            QCalendarWidget QAbstractItemView {
                background-color: #ffffff; color: #1f1f1f;
                selection-background-color: #cfe4ff; selection-color: #1f1f1f;
            }
            QCalendarWidget QToolButton { color: #1f1f1f; background-color: #ffffff; }
            QHeaderView::section { background-color: #e7e7e7; color: #1f1f1f; border: 1px solid #cfcfcf; }
            QPushButton { background-color: #efefef; color: #1f1f1f; border: 1px solid #cfcfcf; padding: 4px 10px; }
            QPushButton:hover { background-color: #e0e0e0; }
            QToolBar { background-color: #f0f0f0; border-bottom: 1px solid #d9d9d9; }
        )";
    }

    qApp->setStyleSheet(theme);
}

bool MainWindow::editAssignmentDialog(int index)
{
    if (index < 0 || index >= m_manager.assignments().size()) {
        return false;
    }

    Assignment current = m_manager.assignments().at(index);
    while (true) {
        QDialog dialog(this);
        dialog.setWindowTitle("Edit Assignment");
        dialog.setStyleSheet(qApp->styleSheet());
        QFormLayout form(&dialog);

        QLineEdit titleEdit(current.title());
        QLineEdit courseEdit(current.courseName());
        QDateEdit dueDateEdit(current.dueDate());
        dueDateEdit.setCalendarPopup(true);
        dueDateEdit.setDisplayFormat("yyyy-MM-dd");
        QComboBox statusCombo;
        statusCombo.addItem("Not started");
        statusCombo.addItem("In progress");
        statusCombo.addItem("Completed");
        statusCombo.setCurrentText(Assignment::statusToString(current.status()));

        form.addRow("Title", &titleEdit);
        form.addRow("Course", &courseEdit);
        form.addRow("Due Date", &dueDateEdit);
        form.addRow("Status", &statusCombo);

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        form.addRow(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() != QDialog::Accepted) {
            return false;
        }

        QString title = titleEdit.text().trimmed();
        QString course = courseEdit.text().trimmed();
        QDate dueDate = dueDateEdit.date();
        Assignment::Status status = Assignment::statusFromString(statusCombo.currentText());

        if (title.isEmpty() || course.isEmpty()) {
            showError("Title and course cannot be blank.");
            continue;
        }
        if (!dueDate.isValid()) {
            showError("Due date is invalid.");
            continue;
        }

        Assignment updated(title, course, dueDate, status);
        QString error;
        if (!m_manager.updateAssignment(index, updated, &error)) {
            showError(error);
            continue;
        }

        autoSave();
        refreshTable();
        selectRowByIndex(index);
        return true;
    }
}

bool MainWindow::quickAddDialog()
{
    while (true) {
        QDialog dialog(this);
        dialog.setWindowTitle("Quick Add Assignment");
        dialog.setStyleSheet(qApp->styleSheet());
        QFormLayout form(&dialog);

        QLineEdit titleEdit;
        QLineEdit courseEdit;
        QDateEdit dueDateEdit(QDate::currentDate());
        dueDateEdit.setCalendarPopup(true);
        dueDateEdit.setDisplayFormat("yyyy-MM-dd");

        form.addRow("Title", &titleEdit);
        form.addRow("Course", &courseEdit);
        form.addRow("Due Date", &dueDateEdit);

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        form.addRow(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() != QDialog::Accepted) {
            return false;
        }

        QString title = titleEdit.text().trimmed();
        QString course = courseEdit.text().trimmed();
        QDate dueDate = dueDateEdit.date();

        if (title.isEmpty() || course.isEmpty()) {
            showError("Title and course cannot be blank.");
            continue;
        }
        if (!dueDate.isValid()) {
            showError("Due date is invalid.");
            continue;
        }

        Assignment assignment(title, course, dueDate, Assignment::Status::NotStarted);
        QString error;
        if (!m_manager.addAssignment(assignment, &error)) {
            showError(error);
            continue;
        }

        autoSave();
        refreshTable();
        selectRowByIndex(m_manager.assignments().size() - 1);
        return true;
    }
}

void MainWindow::selectRowByIndex(int index)
{
    for (int row = 0; row < m_rowToIndex.size(); ++row) {
        if (m_rowToIndex[row] == index) {
            m_table->setCurrentCell(row, 0);
            return;
        }
    }
}
