/**
 * Dana Ahn, Alberto del Rossario, Ethan Kai Falconer, Kevin Matsui, Aidan Naito
 * Assignment 8 : Education App - Koding Stocks
 * CS 3505
 * April 26, 2022
 *
 *
 */

#include "model.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <iostream>
#include <cmath>
#include <QPixmap>

using namespace std;

Model::Model(QObject *parent)
    : QObject{parent}
{
}

/**
 * @brief Model::loadQuizLesson - Loads the Quiz based lesson.
 *
 * Reads the .txt file and converts it to JSON to get the necessary data.
 * Images are provided in a separate file path in the resources file
 *
 * @param imgs - image vector to push the loaded images to
 * @param questions - string vector to push the questions to
 * @param possibleAnswers - 2d string vector to push possible answers to for each question
 * @param wrongAnswer - 2d string vector to push wrong answer explanations for each question
 * @param fileName - File name of the corresponding .txt file for the lesson
 */
void Model::loadQuizLesson(QVector<QPixmap> &imgs, QVector<QString>& questions, QVector<QVector<QString>>& possibleAnswers,
                           QVector<QVector<QString>> &wrongAnswer, QString fileName) {
    QString file_path = ":/lessons/" + fileName + ".txt";

    // Clear the correct answers
    correctAnswer.clear();

    try {
        // construct JsonDocument from the file
        QFile file_obj(file_path);

        file_obj.open(QIODevice::ReadOnly);
        QByteArray bytes = file_obj.readAll();

        file_obj.close();

        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson(bytes, &jsonError);

        //load the lesson images
        loadLessonImages(imgs, fileName, document);

        //load the quiz section
        loadLessonQuiz(questions, possibleAnswers, wrongAnswer, document);
    } catch (...) {
    }

    // signals the LessonWindow that lesson is loaded
    emit finishLoading();
}

/**
 * @brief Model::userInitialize initializes user money and stocks in the beginning and when user wants to retry
 */
void Model::userInitialize() {
    if(initialize) {
        user.money = 10000.00;
        user.stocks["Tesla"] = 0;
        user.stocks["Vfiax"] = 0;
        user.stocks["Usb"] = 0;
        user.stocks["Clx"] = 0;
        initialize = false;
    }
    // display formatted user money
    emit userMoneyChanged(formatMoney(user.money));
}

/**
 * @brief Model::formatMoney convert integer to money format to display on the UI
 * @param money
 * @return
 */
QString Model::formatMoney(double money) {
    QString val("$");
    val += QString("%L1").arg(money, 10, 'f', 2, ' ');

    return val;
}

/**
 * @brief Model::companyClicked store company information that user has clicked
 * @param _company
 */
void Model::companyClicked(QString _company) {
    company = _company;
    matchStockPrice(company);
    quantityChanged(0);
}

/**
 * @brief Model::matchStockPrice sets current stock price variable to the company stock price that user has clicked
 * @param company
 */
void Model::matchStockPrice(QString company) {
    if(company == "Tesla") {
        stockPrice = teslaPrices[priceIndex];
    }
    else if(company == "Vfiax") {
        stockPrice = vfiaxPrices[priceIndex];
    }
    else if(company == "Usb") {
        stockPrice = usbPrices[priceIndex];
    }
    else if(company == "Clx") {
        stockPrice = clxPrices[priceIndex];
    }
    else if (company == "Amazon") {
        stockPrice = amazonPrices[priceIndex];
    }
}

/**
 * @brief Model::quantityChanged fire events for user buy/sell behavior
 * @param quantity
 */
void Model::quantityChanged(int quantity) {
    double quantPrice = quantity * stockPrice;
    QString sign;

    if(buyLesson) {
        sign = "- ";
    }
    else if(sellLesson) {
        sign = "+ ";
    }

    // if user does not have enough stocks to sell
    if(sellLesson && (user.stocks[company] < quantity)) {
        emit exceed("You do not have enough stocks for this company.");
    }
    // if user does not have enough money to sell
    else if(buyLesson && (user.money - quantPrice < 0)) {
        emit exceed("You cannot exceed the amount of money you currently have.");
    }
    // change view by different quantity of stocks
    else {
        emit changeDisplayByPrice("+ " + QString::number(quantity) + " Stocks: ", formatMoney(stockPrice), sign + formatMoney(quantPrice));
    }

    QString stocks = "TSLA: " + QString::number(user.stocks["Tesla"]) + " stocks\n";
    stocks += "VFIAX: " + QString::number(user.stocks["Vfiax"]) + " stocks\n";
    stocks += "CLX: " + QString::number(user.stocks["Clx"]) + " stocks\n";
    stocks += "USB: " + QString::number(user.stocks["Usb"]) + " stocks\n";
    stocks += "AMZN: " + QString::number(user.stocks["Amazon"]) + " stocks\n";
    emit updateCurrentStocks(stocks);
}

/**
 * @brief Model::updateUserStocks update user value whenever user buy/sell stocks
 * @param quantity
 */
void Model::updateUserStocks(int quantity) {
    // if buying stocks
    if(buyLesson) {
        // increase user's company stocks
        user.stocks[company] += quantity;
        // decrease user money
        user.money -= stockPrice*quantity;
    }
    // if selling stocks
    else if(sellLesson) {
        // decrease user's compnay stocks
        user.stocks[company] -= quantity;
        // increase user money
        user.money += stockPrice*quantity;
    }

    emit userMoneyChanged(formatMoney(user.money));
}

/**
 * @brief Model::loadLessonImages - Load the lesson's images to display
 *
 * Reads the JsonDocument to get the total images and partitions the resource file to get all the images
 *
 * @param images - vector of images to push the loaded images to
 * @param name - name of the lesson
 * @param document - JsonDoc of the lesson
 */
void Model::loadLessonImages(QVector<QPixmap> &images, QString name, const QJsonDocument &document) {
    QJsonObject docObj = document.object();

    // Number of images to load
    int numSlides = docObj.value("numSlides").toInt();
    for (int i = 0; i < numSlides; i++) {
        // partitions the file path to get the images
        QString partition = QString("page%1.jpg").arg(i);
        QString file_path = ":/lessonImage/" + name + "/" + partition;

        // initialize images
        QPixmap img(file_path);

        images.push_back(img);
    }
}

/**
 * @brief Model::loadLessonQuiz - Load the quiz section of the Quiz Lessons
 *
 * Reads Json to get the total questions and partitions the Json to get the corresponding data related to
 * each question
 *
 * @param questions - vector to push the questions to
 * @param possible - 2d vector to push the possible answers for each question
 * @param wrongAnswer - 2d vector to push the wrong answers for each question
 * @param document - JsonDoc of the lesson
 */
void Model::loadLessonQuiz(QVector<QString> &questions, QVector<QVector<QString>> &possible, QVector<QVector<QString>> & wrongAnswer, const QJsonDocument &document) {
    QJsonObject docObj = document.object();
    int numQuestion = docObj.value("numQuestions").toInt();

    QJsonObject lessonPlan = docObj.value("lessonQuiz").toObject();

    // Make JsonObj for each related data
    QJsonObject allQuestions = lessonPlan.value("questions").toObject();
    QJsonObject allPossible = lessonPlan.value("possibleAnswer").toObject();
    QJsonObject allRightAns = lessonPlan.value("rightAnswer").toObject();
    QJsonObject allWrongAns = lessonPlan.value("wrongAnswer").toObject();
    for (int i = 0; i < numQuestion; i ++) {
        // partition each obj for the data for the question
        QString answerIndex, possibleIndex, questionIndex, wrongAnsIndex;
        answerIndex = QString("a%1").arg(i);
        possibleIndex = QString("p%1").arg(i);
        questionIndex = QString("q%1").arg(i);
        wrongAnsIndex = QString("wA%1").arg(i);

        // Loads possible answers and wrong answers
        QJsonArray possibleAns = allPossible.value(possibleIndex).toArray();
        QJsonArray wrongAns = allWrongAns.value(wrongAnsIndex).toArray();

        // construct a usuable vector for possible and wrong answer
        QVector<QString> possibleAnswers = {possibleAns[0].toString(), possibleAns[1].toString(), possibleAns[2].toString(), possibleAns[3].toString()};
        QVector<QString> wrongAnswers = {wrongAns[0].toString(), wrongAns[1].toString(), wrongAns[2].toString(), wrongAns[3].toString()};

        // append the data to the given vectors
        correctAnswer.append(allRightAns.value(answerIndex).toString());
        questions.append(allQuestions.value(questionIndex).toString());
        wrongAnswer.append(wrongAnswers);
        possible.append(possibleAnswers);
    }
}

/**
 * @brief Model::loadLessonGraphs - Loads the lesson graphs for interactive portions
 *
 * Loads graphs from file folder in resource folder
 *
 * @param graphs - vector of images to push graphs
 */
void Model::loadLessonGraphs(QVector<QVector<QPixmap>> &graphs) {
    //defaults the graphs to load in 4 for the sell lesson
    int numGraphs = 5;
    int initGraph = 1;

    //buy only needs 1 graph for each company
    if (buyLesson) {
        numGraphs = 1;
        initGraph = 0;
    }

    // loads graphs
    for (int i = 0; i < companyNames.count(); i ++) {
        for (int graphIndex = initGraph; graphIndex < numGraphs; graphIndex++) {
            QString file_path = QString(":/graphs/Graphs/" + companyNames[i] + "%1").arg(graphIndex);
            QPixmap img(file_path);

            graphs[i].push_back(img);
        }
    }
}

/**
 * @brief Model::initialLessonState initialize lesson state for buy/sell window
 * @param slides
 * @param graphs
 * @param lesson
 */
void Model::initialLessonState(QVector<QPixmap>& slides, QVector<QVector<QPixmap>>& graphs, QString lesson) {
    QString file_path = ":/lessons/" + lesson + ".txt";
    try {
        QFile file_obj(file_path);

        file_obj.open(QIODevice::ReadOnly);
        QByteArray bytes = file_obj.readAll();

        file_obj.close();

        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson(bytes, &jsonError);
        qDebug() << jsonError.error;

        if(lesson == "Buy") {
            // store index for buy element
            priceIndex = teslaPrices.size() - 1;
            buyLesson = true;
            sellLesson = false;
        }
        else if(lesson == "Sell") {
            // if index was for buy
            if(priceIndex == teslaPrices.size() - 1){
                // initialize index for sell
                priceIndex = 0;
            }
            sellLesson = true;
            buyLesson = false;
            initialize = true;
        }
        loadLessonGraphs(graphs);
        emit updateWindowByLesson(lesson);

        loadLessonImages(slides, lesson, document);

    } catch (...) {}

    emit finishLoading();
}

/**
 * @brief Model::changeTimePeriod calculates how much percentage the user has gained in between time period and display.
 * @param index
 */
void Model::changeTimePeriod(int index) {
    priceIndex = index;
    double resultNum = user.money - 10000;
    QString resultStr = "You have ";
    if(resultNum <= 0) {
        resultStr += "lost ";
    }
    else {
        resultStr += "gained ";
    }
    resultNum = (abs(resultNum) / 10000) * 100;
    emit updateResultLabel(resultStr + QString::number(resultNum) + " % of the initial given money.");
}

/**
 * @brief Model::checkAnswer - Checks the answer for the inputted question with its copy of correct answers
 *
 * @param answer - answer selected ("a", "b", "c", "d")
 * @param curQuestion - the current question that is being answered
 */
void Model::checkAnswer(QString answer, int curQuestion) {
    // informs the lesson window if the answer is correct
     emit answerResult(correctAnswer[curQuestion] == answer);
}

