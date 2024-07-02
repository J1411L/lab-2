#include "mainwindow.h"
#include <QSqlDatabase>
#include <QFile>
#include <QDataStream>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTableView>
#include <QFormLayout>
#include <QDialog>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QItemSelectionModel>
#include <QDebug>
#include <QPalette>
#include <QPixmap>
#include <QBrush>
#include <QStyle>
#include <QStyleOption>

// Функция проверки пароля на валидность
bool isValidPassword(const QString& password) {
    if (password.length() != 9) {
        return false;
    }
    int digitCount = 0;
    bool hasNonDigit = false;
    for (const QChar& c : password) {
        if (c.isDigit()) {
            digitCount++;
        } else {
            hasNonDigit = true;
        }
    }
    return digitCount >= 3 && hasNonDigit;
}

// Функция проверки логина на валидность
bool isValidLogin(const QString& login) {
    if (login.length() < 3 || login.length() > 20) {
        return false;
    }
    QRegularExpression regex("[a-zA-Z0-9_]+");
    QRegularExpressionMatch match = regex.match(login);
    return match.hasMatch() && match.capturedLength() == login.length();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    layout(new QVBoxLayout(this)),
    stackedWidget(new QStackedWidget),
    tableNames({"flowerss", "composition", "flowerss_composition", "user", "orders"})
{
    setupDatabase();
    setupMainWindow();
}

MainWindow::~MainWindow() {}

void MainWindow::setupDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:\\data-base\\flowers.db");

    if (!db.open()) {
        qWarning() << "ERROR DB";
        exit(1);
    }
    qDebug() << "Database connection established.";
}

void MainWindow::createTableButtons(QVBoxLayout *buttonLayout, QVector<QPushButton*> &buttons, const QStringList &tableNames) {
    for (const QString &tableName : tableNames) {
        QPushButton* button = new QPushButton(tableName);
        buttonLayout->addWidget(button);
        buttons.append(button);
    }

    for (QPushButton* button : buttons) {
        button->setStyleSheet(R"(
        QPushButton {
            background-color: #FFE4E1;
            color: black;
            padding: 10px 20px;
            border: none;
            border-radius: 20px;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: #FFB6C1;
        }
        )");
    }
}

void MainWindow::createTableTab(QStackedWidget *stackedWidget, QSqlRelationalTableModel *model, const QString &tableName) {
    model->setTable(tableName);
    model->select();

    QTableView *tableView = new QTableView;
    tableView->setModel(model);
    tableView->resizeColumnsToContents();

    tableView->setStyleSheet(R"(
        QTableView {
            background-color: #FFFAFA;
            border-radius: 15px;
            padding: 5px;
        }
        QHeaderView::section {
            background-color: #FFF5EE;
            padding: 5px;
            border: 1px solid #FFF;
        }
        QTableView::item {
            border: none;
        }
    )");

    QPushButton* addButton = new QPushButton("Добавить");
    addButton->setStyleSheet(R"(
        QPushButton {
            background-color: #FFFAFA;
            color: black;
            padding: 10px 20px;
            border: none;
            border-radius: 20px;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: #FFF0F5;
        }
    )");
    QObject::connect(addButton, &QPushButton::clicked, [model, tableView, this, tableName]() {
        QDialog* dialog = new QDialog(this);
        QFormLayout* formLayout = new QFormLayout(dialog);

        QVector<QLineEdit*> lineEdits;
        for (int j = 0; j < model->columnCount(); ++j) {
            QLineEdit* lineEdit = new QLineEdit;
            formLayout->addRow(model->headerData(j, Qt::Horizontal).toString(), lineEdit);
            lineEdits.append(lineEdit);

            if (tableName == "user" && model->headerData(j, Qt::Horizontal).toString().toLower() == "password") {
                lineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(".{8,}")));
            }

            if (tableName == "user" && model->headerData(j, Qt::Horizontal).toString().toLower() == "login") {
                lineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9_]{3,20}")));
            }
        }

        QPushButton* saveButton = new QPushButton("Сохранить");
        saveButton->setStyleSheet(R"(
        QPushButton {
            background-color: #FFFAFA;
            color: black;
            padding: 10px 20px;
            border: none;
            border-radius: 20px;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: #FFF0F5;
        }
    )");
        QObject::connect(saveButton, &QPushButton::clicked, [model, lineEdits, dialog, tableName]() {
            if (tableName == "user") {
                for (int j = 0; j < model->columnCount(); ++j) {
                    if (model->headerData(j, Qt::Horizontal).toString().toLower() == "password") {
                        if (!isValidPassword(lineEdits[j]->text())) {
                            qWarning() << "Пароль должен содержать минимум 9 символов и минимум 3 цифры.";
                            return;
                        }
                    }
                }
            }

            int newRow = model->rowCount();
            model->insertRow(newRow);

            for (int j = 0; j < lineEdits.size(); ++j) {
                model->setData(model->index(newRow, j), lineEdits[j]->text());
            }

            model->submitAll();
            dialog->close();
        });

        formLayout->addWidget(saveButton);
        dialog->exec();
    });

    QPushButton* deleteButton = new QPushButton("Удалить");
    deleteButton->setStyleSheet(R"(
        QPushButton {
            background-color: #FFFAFA;
            color: black;
            padding: 10px 20px;
            border: none;
            border-radius: 20px;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: #FFF0F5;
        }
    )");
    QObject::connect(deleteButton, &QPushButton::clicked, [model, tableView]() {
        QItemSelectionModel* selectionModel = tableView->selectionModel();
        QModelIndexList selectedRows = selectionModel->selectedRows();
        for (int i = selectedRows.count() - 1; i >= 0; --i) {
            model->removeRow(selectedRows.at(i).row());
        }
        model->select();
    });

    QPushButton* updateButton = new QPushButton("Обновить");
    updateButton->setStyleSheet(R"(
        QPushButton {
            background-color: #FFFAFA;
            color: black;
            padding: 10px 20px;
            border: none;
            border-radius: 20px;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: #FFF0F5;
        }
    )");
    QObject::connect(updateButton, &QPushButton::clicked, [model, tableView]() {
        model->submitAll();
        model->select();
        tableView->resizeColumnsToContents();
    });

    QVBoxLayout *tableLayout = new QVBoxLayout;
    tableLayout->addWidget(tableView);
    tableLayout->addWidget(addButton);
    tableLayout->addWidget(deleteButton);
    tableLayout->addWidget(updateButton);

    QWidget *tabWidget = new QWidget;
    tabWidget->setLayout(tableLayout);

    stackedWidget->addWidget(tabWidget);
}

void MainWindow::setupMainWindow() {
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    createTableButtons(buttonLayout, buttons, tableNames);

    for (int i = 0; i < tableNames.size(); ++i) {
        QSqlRelationalTableModel *model = new QSqlRelationalTableModel(nullptr, db);
        createTableTab(stackedWidget, model, tableNames[i]);
    }

    for (int i = 0; i < buttons.size(); ++i) {
        QObject::connect(buttons[i], &QPushButton::clicked, [this, i]() {
            stackedWidget->setCurrentIndex(i);
            stackedWidget->show();
        });
    }

    layout->addLayout(buttonLayout);
    layout->addWidget(stackedWidget);
    stackedWidget->hide();

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    setWindowTitle("Database Viewer");
    resize(800, 600);

   // QPixmap background("D:\\photo\\flow.jpg");
   // QPalette palette;
    //palette.setBrush(QPalette::Window, background);
    //setPalette(palette);
   // setAutoFillBackground(true);
}
