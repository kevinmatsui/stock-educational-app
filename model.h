/**
 * Dana Ahn, Alberto del Rossario, Ethan Kai Falconer, Kevin Matsui, Aidan Naito
 * Assignment 8 : Education App - Koding Stocks
 * CS 3505
 * April 26, 2022
 *
 * This is header file that implements different features for the stock market educational game
 */

#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QPixmap>

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = nullptr);

    QString formatMoney(double money);
    void loadQuizLesson(QVector<QPixmap>& images, QVector<QString> &questions, QVector<QVector<QString>> &possibleAnswer,
                        QVector<QVector<QString>> &wrongAnswer, QString fileName);
    void loadInteractiveFormat();

signals:
    void changeDisplayByPrice(QString quantity, QString price, QString quantPrice);
    void userMoneyChanged(QString userMoney);
    void exceed(QString warningStr);
    void updateWindowByLesson(QString lesson);
    void finishLoading();
    void answerResult(bool result);
    void updateResultLabel(QString result);
    void updateCurrentStocks(QString stocks);

public slots:
    void quantityChanged(int quantity);
    void userInitialize();
    void updateUserStocks(int quantity);
    void checkAnswer(QString answer, int curQuestion);

    void companyClicked(QString _company);
    void initialLessonState(QVector<QPixmap>& text, QVector<QVector<QPixmap>>& graphs, QString lesson);
    void changeTimePeriod(int index);
private:
    struct User {
        double money;
        std::map<QString, int> stocks;
    };
    User user;

    bool buyLesson = false;
    bool sellLesson = false;

    bool initialize = true;

    QString company;
    double stockPrice;
    int priceIndex;

    QVector<QString> correctAnswer;
    QVector<QString> companyNames{"TSLA", "S&P", "CLX", "USBank", "AMZN"};

    QVector<double> teslaPrices{1028.41, 1003.80, 668.06, 609.89, 691.05};
    QVector<double> vfiaxPrices{309.34, 337.72, 365.05, 393.93, 281.94};
    QVector<double> clxPrices{220.69, 148.24, 188.99, 188.58, 148.27};
    QVector<double> usbPrices{46.12, 50.94, 60.85, 35.56, 37.87};
    QVector<double> amazonPrices{3158.30, 3019.79, 3089.49, 3346.83, 3153.46};

    void matchStockPrice(QString company);

    void loadLessonText(QVector<QString>& text, const QJsonDocument& fileName);
    void loadLessonGraphs(QVector<QVector<QPixmap>>& graphs);
    void loadLessonImages(QVector<QPixmap>& images, QString name, const QJsonDocument & document);
    void loadLessonQuiz(QVector<QString>& questions, QVector<QVector<QString>>& possible, QVector<QVector<QString>>& wrongAnswer, const QJsonDocument& document);
};

#endif // MODEL_H
