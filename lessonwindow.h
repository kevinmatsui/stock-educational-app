/**
 * Dana Ahn, Alberto del Rossario, Ethan Kai Falconer, Kevin Matsui, Aidan Naito
 * Assignment 8 : Education App - Koding Stocks
 * CS 3505
 * April 26, 2022
 *
 * This is the header file for the view that displays different UI for each lessons.
 */
#ifndef LESSONWINDOW_H
#define LESSONWINDOW_H

#include <QWidget>
#include <model.h>
#include <QPushButton>
#include <QLabel>
#include <Box2D/Box2D.h>
#include <QImage>
#include <QTimer>

namespace Ui {
class LessonWindow;
}

class LessonWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LessonWindow(Model & model, QWidget *parent = nullptr);
    ~LessonWindow();
public slots:
    void openLesson(bool interactive, QString lessonNum);
    void lessonLoaded();
    void progressQuestions(bool answer);

private slots:

    void on_cOption_clicked();

    void on_investPushButton_clicked();

    void on_vfinxPushButton_clicked();

    void on_snapPushButton_clicked();

    void on_usbPushButton_clicked();

    void on_teslaPushButton_clicked();

    // added slots
    void displayChanged(QString quantity, QString price, QString quantPrice);
    void changeUserMoney(QString userMoney);
    void limitWarning(QString warningStr);
    void initializeLessonWindow(QString lesson);
    void changeResultLabel(QString result);
    void updatePhysicsBodies();
    void displayCurrentStocks(QString stocks);

    void on_aOption_clicked();

    void on_bOption_pressed();

    void on_dOption_clicked();

    void completionUpdate();

    void on_completetionButton_clicked();


    void on_amazonPushButton_clicked();

signals:
    void investClicked(int quantity);
    void clickedCompany(QString company);
    void loadQuiz(QVector<QPixmap>& slides, QVector<QString>& questions, QVector<QVector<QString>>& possibleAnswer, QVector<QVector<QString>>& wrongAnswer,QString fileName);
    void loadInteractive(QVector<QPixmap>& slides, QVector<QVector<QPixmap>>& graphs, QString fileName);
    void answerSelected(QString selected, int curQuestion);
    void nextTimePeriod(int timeIndex);
    void mainWindowFocus(bool enabled);

    void buySellButtons(bool buyEnabled, bool sellEnabled);

private:
    Ui::LessonWindow *ui;

    // physics engine
    b2World world;
    QTimer timer;
    QVector<b2Body*> bodies;
    b2FixtureDef fixtureDef;

//    double money = 10000.00;
    QVector<QString> info, questions;
    QVector<QPixmap> slides;
    QVector<QVector<QPixmap>> graphs;
    QVector<QVector<QString>> possibleAnswer, wrongAnswer;
    int curQuestion, maxQuestion, curInfoIndex, maxInfoIndex = 1, answerIndex;
    QString curLessonName;
    QPushButton* selection;
    QLabel* imageDisplay;
    bool sellWindow = false;
    bool buyWindow = false;
    bool interactive = false;
    int timeIndex = 0, curCompany;

    bool completed = false;

    QImage image;

    void nextSlide();
    void displayQuestion();

    void initializeButtonColors();
    void initializeCompany(QString company, QPushButton * button);
    void resetLesson();
    void displayGraph();

    void displayCompletion(QString lesson);

    void initializePhysicsEngine();
    void closeEvent(QCloseEvent*);

protected:
    void paintEvent(QPaintEvent*);
};

#endif // LESSONWINDOW_H
