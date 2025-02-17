#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "linkedlist.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onInsertClicked();
    void onDeleteClicked();
    void onSearchClicked();
    void updateDisplay();

private:
    LinkedList linkedList;
    QVBoxLayout *layout;
    QLineEdit *inputField;
    QPushButton *insertButton, *deleteButton, *searchButton;
    QLabel *displayLabel;
};

#endif // MAINWINDOW_H
