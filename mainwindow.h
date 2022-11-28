/**
 * Dana Ahn, Alberto del Rossario, Ethan Kai Falconer, Kevin Matsui, Aidan Naito
 * Assignment 8 : Education App - Koding Stocks
 * CS 3505
 * April 26, 2022
 *
 * This is the header file for the view that allows user to click lesson buttons
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Box2D/Box2D.h>
#include "lessonwindow.h"
#include "model.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Model& model, LessonWindow * _buySellStocksWindow, b2World * _world, QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void initializeUser();
    void lessonClicked(QString lesson);
    void loadLesson(bool interactive, QString lesName);

private slots:
    void on_les1PushButton_clicked();

    void on_les3PushButton_clicked();

    void on_les4PushButton_clicked();

    void setFocus(bool focus);
    void buySellEnabled(bool buyEnabled, bool sellEnabled);

    void on_les2PushButton_clicked();

    void on_les5PushButton_clicked();

    void on_les6PushButton_clicked();

private:
    Ui::MainWindow *ui;
    b2World * world;
    LessonWindow * buySellStocksWindow;
};
#endif // MAINWINDOW_H
