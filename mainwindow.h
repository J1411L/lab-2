#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QDebug>
#include <QPalette>
#include <QPixmap>
#include <QBrush>


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QSqlDatabase db; // Объект подключения к базе данных
    QVBoxLayout *layout; // Основной компоновщик макета
    QVector<QPushButton*> buttons; // Вектор кнопок для навигации по таблицам
    QStackedWidget *stackedWidget; // Виджет для отображения представлений таблиц
    QStringList tableNames; // Список имен таблиц

    void setupDatabase(); // Настройка подключения к базе данных
    void createTableButtons(QVBoxLayout *buttonLayout, QVector<QPushButton*> &buttons, const QStringList &tableNames); // Создание кнопок для навигации по таблицам
    void createTableTab(QStackedWidget *stackedWidget, QSqlRelationalTableModel *model, const QString &tableName); // Создание вкладки для отображения таблицы
    void setupMainWindow(); // Настройка основного окна приложения
};

#endif // MAINWINDOW_H
