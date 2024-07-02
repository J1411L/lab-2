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
    QSqlDatabase db;
    QVBoxLayout *layout;
    QVector<QPushButton*> buttons;
    QStackedWidget *stackedWidget;
    QStringList tableNames;

    void setupDatabase();
    void createTableButtons(QVBoxLayout *buttonLayout, QVector<QPushButton*> &buttons, const QStringList &tableNames);
    void createTableTab(QStackedWidget *stackedWidget, QSqlRelationalTableModel *model, const QString &tableName);
    void setupMainWindow();
};

#endif // MAINWINDOW_H
