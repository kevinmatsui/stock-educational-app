/**
 * Dana Ahn, Alberto del Rossario, Ethan Kai Falconer, Kevin Matsui, Aidan Naito
 * Assignment 8 : Education App - Koding Stocks
 * CS 3505
 * April 26, 2022
 *
 * View & Controller of our program
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Box2D/Box2D.h>
#include "model.h"
#include "lessonwindow.h"

MainWindow::MainWindow(Model& model, LessonWindow * _buySellStocksWindow, b2World * _world, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), world(_world) {
    ui->setupUi(this);

    buySellStocksWindow = _buySellStocksWindow;

    // Buy interactive --> creates user for a single interaction
    connect(this, &MainWindow::initializeUser,
            &model, &Model::userInitialize);

    // View --> starts loading .txt files, images, and other resources for each lesson
    connect(this, &MainWindow::loadLesson,
            _buySellStocksWindow, &LessonWindow::openLesson);

    // View --> focus the physics screen during buy & sell
    connect(_buySellStocksWindow, &LessonWindow::mainWindowFocus,
            this, &MainWindow::setFocus);

    // Buy & Sell --> switching text from buy to sell
    connect(_buySellStocksWindow, &LessonWindow::buySellButtons,
            this, &MainWindow::buySellEnabled);

    // disable sell push button
    ui->les4PushButton->setEnabled(false);

    this -> setStyleSheet("background-color: rgb(190, 228, 190);");
    ui -> titleLabel -> setStyleSheet("QLabel { color : green; }");

    // button UI initial colors
    ui-> les1PushButton ->setStyleSheet(QString("background-color: rgb(95, 177, 95);"));
    ui-> les2PushButton ->setStyleSheet(QString("background-color: rgb(95, 177, 95);"));
    ui-> les3PushButton ->setStyleSheet(QString("background-color: rgb(95, 177, 95);"));
    ui-> les4PushButton ->setStyleSheet(QString("background-color: rgb(95, 177, 95);"));
    ui-> les5PushButton ->setStyleSheet(QString("background-color: rgb(95, 177, 95);"));
    ui-> les6PushButton ->setStyleSheet(QString("background-color: rgb(95, 177, 95);"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// loads the stock market lesson on a button click
void MainWindow::on_les1PushButton_clicked()
{
    emit loadLesson(false, "StockMarket");

    (*buySellStocksWindow).show();
}

// loads the safe vs volatile company lesson on a button click
void MainWindow::on_les2PushButton_clicked()
{
    (*buySellStocksWindow).show();

    emit loadLesson(false, "safeVsVolatile");
}

// loads the interactive buy lesson on a button clcik
void MainWindow::on_les3PushButton_clicked()
{
    (*buySellStocksWindow).show();

    emit initializeUser();
    emit loadLesson(true, "Buy");
}

// loads the interactive sell lesson on a button click
void MainWindow::on_les4PushButton_clicked()
{
    (*buySellStocksWindow).show();
    emit loadLesson(true, "Sell");
}

// loads the stock options lesson on a button click
void MainWindow::on_les5PushButton_clicked()
{
    (*buySellStocksWindow).show();

    emit loadLesson(false, "StockOptions");
}

// loads the market shortage lesson on a button click
void MainWindow::on_les6PushButton_clicked()
{
    (*buySellStocksWindow).show();

    emit loadLesson(false, "MarketShortage");
}

// accepts a signal to set the focus of a specific form
void MainWindow::setFocus(bool focus) {
    QWidget::setEnabled(focus);
}

// Enables and disables the buy and sell button options for the different interactive lessons
void MainWindow::buySellEnabled(bool buyEnabled, bool sellEnabled) {
    ui -> les3PushButton -> setEnabled(buyEnabled);
    ui -> les4PushButton -> setEnabled(sellEnabled);
}
