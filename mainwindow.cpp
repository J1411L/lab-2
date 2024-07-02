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
    if (password.length() != 9) { // Проверка длины пароля
        return false;
    }
    int digitCount = 0; // Счетчик цифр
    bool hasNonDigit = false; // Флаг наличия нецифровых символов
    for (const QChar& c : password) {
        if (c.isDigit()) {
            digitCount++; // Увеличение счетчика цифр
        } else {
            hasNonDigit = true; // Установка флага наличия нецифрового символа
        }
    }
    return digitCount >= 3 && hasNonDigit; // Проверка условия валидности
}

// Функция проверки логина на валидность
bool isValidLogin(const QString& login) {
    if (login.length() < 3 || login.length() > 20) { // Проверка длины логина
        return false;
    }
    QRegularExpression regex("[a-zA-Z0-9_]+"); // Регулярное выражение для проверки символов
    QRegularExpressionMatch match = regex.match(login); // Проверка соответствия регулярному выражению
    return match.hasMatch() && match.capturedLength() == login.length(); // Проверка условия валидности
}

// Конструктор класса MainWindow
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    layout(new QVBoxLayout(this)), // Создание главного вертикального макета
    stackedWidget(new QStackedWidget), // Создание стека виджетов для отображения таблиц
    tableNames({"flowerss", "composition", "flowerss_composition", "user", "orders"}) // Список имен таблиц
{
    setupDatabase(); // Настройка базы данных
    setupMainWindow(); // Настройка главного окна
}

// Деструктор класса MainWindow
MainWindow::~MainWindow() {}

// Настройка соединения с базой данных
void MainWindow::setupDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE"); // Добавление базы данных SQLite
    db.setDatabaseName("D:\\data-base\\flowers.db"); // Установка имени базы данных

    if (!db.open()) { // Открытие базы данных и проверка на ошибку
        qWarning() << "ERROR DB"; // Вывод сообщения об ошибке
        exit(1); // Завершение программы
    }
    qDebug() << "Database connection established."; // Вывод сообщения об успешном соединении
}

// Создание кнопок для переключения между таблицами
void MainWindow::createTableButtons(QVBoxLayout *buttonLayout, QVector<QPushButton*> &buttons, const QStringList &tableNames) {
    for (const QString &tableName : tableNames) { // Проход по всем именам таблиц
        QPushButton* button = new QPushButton(tableName); // Создание кнопки с названием таблицы
        buttonLayout->addWidget(button); // Добавление кнопки в макет
        buttons.append(button); // Добавление кнопки в вектор кнопок
    }

    for (QPushButton* button : buttons) { // Установка стиля для каждой кнопки
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

// Создание вкладки с таблицей
void MainWindow::createTableTab(QStackedWidget *stackedWidget, QSqlRelationalTableModel *model, const QString &tableName) {
    model->setTable(tableName); // Установка таблицы для модели
    model->select(); // Выбор данных из таблицы

    QTableView *tableView = new QTableView;
    tableView->setModel(model); // Установка модели для представления таблицы
    tableView->resizeColumnsToContents(); // Подгонка размеров столбцов под содержимое

    // Настройка стиля для таблицы
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

    // Создание кнопки "Добавить" и стилизация
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

    // Обработчик нажатия кнопки "Добавить"
    QObject::connect(addButton, &QPushButton::clicked, [model, tableView, this, tableName]() {
        QDialog* dialog = new QDialog(this); // Создание диалогового окна для ввода данных
        QFormLayout* formLayout = new QFormLayout(dialog);

        QVector<QLineEdit*> lineEdits; // Вектор для хранения полей ввода
        for (int j = 0; j < model->columnCount(); ++j) {
            QLineEdit* lineEdit = new QLineEdit;
            formLayout->addRow(model->headerData(j, Qt::Horizontal).toString(), lineEdit); // Добавление поля ввода в форму
            lineEdits.append(lineEdit);

            // Добавление валидатора для поля "password" в таблице "user"
            if (tableName == "user" && model->headerData(j, Qt::Horizontal).toString().toLower() == "password") {
                lineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(".{8,}")));
            }

            // Добавление валидатора для поля "login" в таблице "user"
            if (tableName == "user" && model->headerData(j, Qt::Horizontal).toString().toLower() == "login") {
                lineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9_]{3,20}")));
            }
        }

        // Создание кнопки "Сохранить" и стилизация
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

        // Обработчик нажатия кнопки "Сохранить"
        QObject::connect(saveButton, &QPushButton::clicked, [model, lineEdits, dialog, tableName]() {
            // Проверка валидности пароля для таблицы "user"
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
            model->insertRow(newRow); // Вставка новой строки в модель

            for (int j = 0; j < lineEdits.size(); ++j) {
                model->setData(model->index(newRow, j), lineEdits[j]->text()); // Заполнение новой строки данными из полей ввода
            }

            model->submitAll(); // Сохранение изменений в модели
            dialog->close(); // Закрытие диалогового окна
        });

        formLayout->addWidget(saveButton); // Добавление кнопки "Сохранить" в форму
        dialog->exec(); // Запуск диалогового окна
    });

    // Создание кнопки "Удалить" и стилизация
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

    // Обработчик нажатия кнопки "Удалить"
    QObject::connect(deleteButton, &QPushButton::clicked, [model, tableView]() {
        QItemSelectionModel* selectionModel = tableView->selectionModel();
        QModelIndexList selectedRows = selectionModel->selectedRows(); // Получение списка выбранных строк
        for (int i = selectedRows.count() - 1; i >= 0; --i) {
            model->removeRow(selectedRows.at(i).row()); // Удаление выбранных строк
        }
        model->select(); // Обновление данных в модели
    });

    // Создание кнопки "Обновить" и стилизация
    QPushButton* updateButton = new QPushButton("Обновить");
    updateButton->setStyleSheet(R"(
        QPushButton {
            background-color: #FFFAFA;
            color: black;
            padding: 10px 20px;
            border: none;
            border-radius: 20px;
            font-size: 16px.
        }
        QPushButton:hover {
            background-color: #FFF0F5;
        }
    )");

    // Обработчик нажатия кнопки "Обновить"
    QObject::connect(updateButton, &QPushButton::clicked, [model, tableView]() {
        model->submitAll(); // Сохранение всех изменений в модели
        model->select(); // Обновление данных в модели
        tableView->resizeColumnsToContents(); // Подгонка размеров столбцов под содержимое
    });

    // Создание макета для таблицы и добавление элементов
    QVBoxLayout *tableLayout = new QVBoxLayout;
    tableLayout->addWidget(tableView);
    tableLayout->addWidget(addButton);
    tableLayout->addWidget(deleteButton);
    tableLayout->addWidget(updateButton);

    // Создание виджета для вкладки и установка макета
    QWidget *tabWidget = new QWidget;
    tabWidget->setLayout(tableLayout);

    // Добавление вкладки в стек виджетов
    stackedWidget->addWidget(tabWidget);
}

// Настройка главного окна
void MainWindow::setupMainWindow() {
    QVBoxLayout *buttonLayout = new QVBoxLayout(); // Создание вертикального макета для кнопок
    createTableButtons(buttonLayout, buttons, tableNames); // Создание кнопок для таблиц

    for (int i = 0; i < tableNames.size(); ++i) {
        QSqlRelationalTableModel *model = new QSqlRelationalTableModel(nullptr, db);
        createTableTab(stackedWidget, model, tableNames[i]); // Создание вкладок для каждой таблицы
    }

    for (int i = 0; i < buttons.size(); ++i) {
        QObject::connect(buttons[i], &QPushButton::clicked, [this, i]() {
            stackedWidget->setCurrentIndex(i); // Переключение на соответствующую вкладку при нажатии кнопки
            stackedWidget->show(); // Показ стека виджетов
        });
    }

    layout->addLayout(buttonLayout); // Добавление макета с кнопками в главный макет
    layout->addWidget(stackedWidget); // Добавление стека виджетов в главный макет
    stackedWidget->hide(); // Скрытие стека виджетов до выбора таблицы

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(layout); // Установка главного макета на центральный виджет
    setCentralWidget(centralWidget); // Установка центрального виджета в главное окно
    setWindowTitle("Database Viewer"); // Установка заголовка окна
    resize(800, 600); // Установка размера окна

    // Закомментированный код для установки фона
    // QPixmap background("D:\\photo\\flow.jpg");
    // QPalette palette;
    // palette.setBrush(QPalette::Window, background);
    // setPalette(palette);
    // setAutoFillBackground(true);
}
