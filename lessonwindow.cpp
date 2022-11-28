/**
 * Dana Ahn, Alberto del Rossario, Ethan Kai Falconer, Kevin Matsui, Aidan Naito
 * Assignment 8 : Education App - Koding Stocks
 * CS 3505
 * April 26, 2022
 *
 * LessonWindow is a ui class that holds the interactive portions of our educational app as well as info slides.
 * Slides contain lessons on market shorting, stock options, safe vs volatile companies, and the stock market in general.
 * These interactive portions are used for both the buy and the sell lessons. Within this
 * class there is info stored on stocks.
 */
#include "lessonwindow.h"
#include "ui_lessonwindow.h"
//#include <iostream>

#include "model.h"
#include <QMessageBox>
#include <QTimer>
#include <QPainter>


using namespace std;
/**
 * @brief LessonWindow::LessonWindow A constructor that a few parameters to create a new lesson window with stock info.
 * Instatiates a QWidget, ui, world object, timer, and image for associated lessons.
 * @param model instance of the model that is connected with the lesson window.
 * @param parent
 */
LessonWindow::LessonWindow(Model& model, QWidget *parent) : QWidget(parent), ui(new Ui::LessonWindow),
    world(b2Vec2(0.0f, 50.0f)), timer(this), image(":/icons/dollarSign.png") {

    ui->setupUi(this);

    // lessonwindow to lessonwindow
    connect(ui->nextPushButton, &QPushButton::clicked,
            this, &LessonWindow::nextSlide);
    connect(&timer, &QTimer::timeout, this, &LessonWindow::completionUpdate);

    // lessonwindow to model
    connect(this, &LessonWindow::clickedCompany,
            &model, &Model::companyClicked);
    connect(ui->quantitySpinBox, &QSpinBox::valueChanged,
            &model, &Model::quantityChanged);
    connect(this, &LessonWindow::investClicked,
            &model, &Model::updateUserStocks);
    connect(this, &LessonWindow::nextTimePeriod,
                &model, &Model::changeTimePeriod);

    // model to lessonwindow
    connect(&model, &Model::changeDisplayByPrice,
            this, &LessonWindow::displayChanged);
    connect(&model, &Model::userMoneyChanged,
            this, &LessonWindow::changeUserMoney);
    connect(&model, &Model::exceed,
            this, &LessonWindow::limitWarning);
    connect(&model, &Model::updateWindowByLesson,
            this, &LessonWindow::initializeLessonWindow);
    connect(&model, &Model::answerResult,
            this, &LessonWindow::progressQuestions);
    connect(&model, &Model::updateResultLabel,
            this, &LessonWindow::changeResultLabel);
    connect(&model, &Model::updateCurrentStocks,
            this, &LessonWindow::displayCurrentStocks);


    connect(&model, &Model::finishLoading,
            this, &LessonWindow::lessonLoaded);

    connect(this, &LessonWindow::loadQuiz,
            &model, &Model::loadQuizLesson);

    connect(this, &LessonWindow::loadInteractive,
            &model, &Model::initialLessonState);

    connect(this, &LessonWindow::answerSelected,
            &model, &Model::checkAnswer);

    ui -> resultLabel -> setStyleSheet("QLabel { color : red; }");
    ui -> totalResultLabel -> setStyleSheet("QLabel { color : red; }");
    ui -> totalResultLabel -> setText("");

    ui -> investPushButton -> setStyleSheet("background-color: rgb(222, 78, 75);");
    ui -> warningLabel -> setStyleSheet("QLabel { color : red; }");
    ui -> warningLabel -> hide();
}
/**
 * @brief LessonWindow::initializePhysicsEngine sets up the physics portion of our lesson windows.
 */
void LessonWindow::initializePhysicsEngine() {
    // clear the box2d body vector
    bodies.clear();

    b2BodyDef groundBodyDef;
    //set up ground position (bottom border of physics slides)
    groundBodyDef.position.Set(0.0f, 47.0f);
    //implementing bottom position into the lesson window's world
    b2Body* groundBody = world.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    // The extents are the half-widths of the box.
    groundBox.SetAsBox(50.0f, 10.0f);
    // Add the ground fixture to the ground body.
    groundBody->CreateFixture(&groundBox, 0.0f);
    b2BodyDef bodyDef;

    //configuring settings for physics aspect
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1.0f, 1.0f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.9f;



    float bodyPosition, randomForce;
    //initilaizing physics bodies on the lesson window
    for (int physicsBodies = 0; physicsBodies < 11; physicsBodies++) {
        b2BodyDef bodyDef;
        bodyPosition = rand() % 20 + 10;

        int labelMidpoint = (130 + (ui->completionLabel->width() / 2))/21;
        //right half has a positive force, left half has a negative force
        if(bodyPosition >= labelMidpoint ){
            randomForce = rand() % 1250 + 700;
        } else {
            randomForce = (rand() % -1250) - 1000;
        }
        //setting start position and applying the force.
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(bodyPosition, 6.0f);
        b2Body* body = world.CreateBody(&bodyDef);
        b2Vec2 force(randomForce, -5500);
        body->ApplyForceToCenter(force, true);

        body->CreateFixture(&fixtureDef);
        bodies.push_back(body);
    }
}

/**
 * @brief LessonWindow::paintEvent updating view on every paint action.
 */
void LessonWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    //if the lesson is done then run the physics aspect.
    if(completed) {
        for (b2Body* body : bodies) {
            b2Vec2 position = body->GetPosition();
            painter.drawImage((int)(position.x*20), (int)(position.y*20), image);
        }
    }
    painter.end();
}

/**
 * @brief LessonWindow::updatePhysicsBodies updating the position of the entities in the physcis aspect.
 */
void LessonWindow::updatePhysicsBodies() {
    float timeStep = 1.0f / 60.0f;
    int velocityIterations = 1;
    int positionIterations = 1;

    // Instruct the world to perform a single step of simulation.
    // It is generally best to keep the time step and iterations fixed.
    world.Step(timeStep, velocityIterations, positionIterations);

    update();
}

/**
 * @brief LessonWindow::nextSlide Determines the next part of the lesson depending on if its an interactive lesson or not.
 */
void LessonWindow::nextSlide() {
    if (!interactive) {
        //checks for if on the question portion of a lesson.
        if (curInfoIndex  == (maxInfoIndex - 1)) {
            // checks for interactive buy or sell lesson, if not show question
            if (!(buyWindow || sellWindow)) {
                ui->nextPushButton->setEnabled(false);
                displayQuestion();
            }
            // for entering interactive lesson, initialize companys
            else {
                imageDisplay = ui->graphLabel;
                interactive = true;
                //going to interactive window
                ui->stackedWidget->setCurrentIndex(1);
                displayGraph();
                initializeCompany("Tesla", ui->teslaPushButton);

                if(sellWindow) {
                    ui -> nextPushButton -> setText("Next 3 months");
                    emit nextTimePeriod(timeIndex);
                }
            }
        }
        //if not at the end of a lesson then go next slide by incrementing.
        else {
            curInfoIndex++;
            imageDisplay->setPixmap(slides[curInfoIndex]);
        }
    }
    //entering the interactive lesson portion of code.
    else {
        if (buyWindow && ui -> moneyLabel -> text() != "$ 10,000.00") {
            displayCompletion("Buy");
        } else if(sellWindow) {
            ui -> stackedWidget -> setCurrentIndex(1);
            imageDisplay = ui->graphLabel;
            timeIndex++;
            //updating view to next time period
            emit nextTimePeriod(timeIndex);
            if(timeIndex < 4) {
                curCompany = 0; // initializing to tesla company
                displayGraph();
                initializeCompany("Tesla", ui->teslaPushButton);
            }else {
                displayCompletion("Sell");
                ui -> warningLabel -> hide();
                timeIndex = 0;
            }
        }
    }
}

/**
 * @brief LessonWindow::displayCurrentStocks displays user's stock quantities whenever user buy/sell
 * @param stocks
 */
void LessonWindow::displayCurrentStocks(QString stocks) {
    ui ->stockLabel->setText(stocks);
}

/**
 * @brief LessonWindow::displayGraph - Displays the graph on imageDisplay label
 *
 * The graph depends on the current company selected and the time index. Draws them
 * at a scaled to the label
 */
void LessonWindow::displayGraph() {
    imageDisplay->setPixmap(graphs[curCompany].at(timeIndex));
    imageDisplay->setScaledContents(true);
    imageDisplay->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

/**
 * @brief LessonWindow::displayCompletion - Displays the completion screen depending
 * on the lesson.
 *
 * Sell window displays your profits
 *
 * @param lesson - Lesson Name
 */
void LessonWindow::displayCompletion(QString lesson) {
    emit mainWindowFocus(false);

    // Special behavior in Sell (displays profit)
    if(lesson == "Sell") {
        QString result = ui -> resultLabel -> text();
        ui -> totalResultLabel -> setText(result);
        if(result.indexOf("lost") != -1) {
            // Display fail images if profit isn't gained
            image = QImage(":/icons/fail.png");
        }
        else {
            // Display completion image if profit is gained
            image = QImage(":/icons/cash.png");
        }
    }

    ui -> nextPushButton -> hide();
    ui-> completionLabel -> setText("You Completed " + lesson + " Lesson !");
    ui -> stackedWidget -> setCurrentIndex(2);
    completed = true;
    initializePhysicsEngine();

    // display physics engine
    timer.start(10);
}

/**
 * @brief LessonWindow::progressQuestions - Progresses to the next question if the
 * given answer is correct or not. If the answer isn't correct, it displays the reason
 * why it's not correct on the button.
 *
 * @param answer - boolean: T if answer is correct
 */
void LessonWindow::progressQuestions(bool answer) {
    // progress to next question if answer is T
    if (answer) {
        curQuestion++;

        // if stll questions keep displaying question
        if (curQuestion < maxQuestion) {
            displayQuestion();
        } else {
            displayCompletion(curLessonName);
        }
    } else {
        // display the wrong answer explanation on button
        selection->setText(wrongAnswer[curQuestion].at(answerIndex));
    }
}

/**
 * @brief LessonWindow::lessonLoaded - Displays the priliminary elements
 * when the lesson finishes loading from the model
 */
void LessonWindow::lessonLoaded() {
    // Set the max indicies
    maxInfoIndex = slides.count();
    maxQuestion = questions.count();

    ui->nextPushButton->setEnabled(true);

    // Display the first image
    imageDisplay = ui->infoLabel;
    if (buyWindow || sellWindow) {
        imageDisplay = ui->buyInfoLabel;
    }

    imageDisplay->setPixmap(slides[curInfoIndex]);
    imageDisplay->setScaledContents(true);
    imageDisplay->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

/**
 * @brief LessonWindow::openLesson - Starts the loading process for the lesson plan
 *
 * Loads in lesson images, answers, possible questions, and graphs to display
 *
 * @param interactive - T if the lesson is interactive
 * @param lessonName - lesson name and file name
 */
void LessonWindow::openLesson(bool interactive, QString lessonName) {
    resetLesson();

    curLessonName = lessonName;

    // In case UI opens before model and window is ready
    ui->nextPushButton->setEnabled(false);
    ui->infoLabel->setText("Wait for lesson to load");

    // Loads interactive vs quiz section
    if (interactive) {
        // Interactive needs to load images and graphs
        emit loadInteractive(slides, graphs, lessonName);

        ui->stackedWidget->setCurrentIndex(3);
    } else {
        ui->stackedWidget->setCurrentIndex(0);

        ui->aOption->setEnabled(false);
        ui->bOption->setEnabled(false);
        ui->cOption->setEnabled(false);
        ui->dOption->setEnabled(false);

        // Quiz section needs images, questions, possible answer, and wrong answers
        emit loadQuiz(slides, questions, possibleAnswer, wrongAnswer, lessonName);
    }

}

/**
 * @brief LessonWindow::displayQuestion - Displays the question on the screen for quiz section
 * lessons
 */
void LessonWindow::displayQuestion() {
    //Displays the question and grabs the possible answers for that question
    ui->infoLabel->setText(questions[curQuestion]);
    QVector<QString> answerSelection = possibleAnswer[curQuestion];

    //Enable all option buttons
    ui->aOption->setEnabled(true);
    ui->bOption->setEnabled(true);
    ui->cOption->setEnabled(true);
    ui->dOption->setEnabled(true);

    //Display answer options on buttons
    ui->aOption->setText(answerSelection[0]);
    ui->bOption->setText(answerSelection[1]);
    ui->cOption->setText(answerSelection[2]);
    ui->dOption->setText(answerSelection[3]);
}

/**
 * @brief LessonWindow::resetLesson - Resets the LessonWindow UI and data structures to
 * prepare for the next lesson to be loaded
 */
void LessonWindow::resetLesson() {
    // Clears the data structures holding lesson UI elements
    info.clear();
    questions.clear();
    possibleAnswer.clear();
    wrongAnswer.clear();
    slides.clear();
    graphs.clear();
    graphs.resize(5);

    // Puts button text to blank
    ui->aOption->setText("");
    ui->bOption->setText("");
    ui->cOption->setText("");
    ui->dOption->setText("");

    // Resets the indices
    curInfoIndex = 0;
    curQuestion = 0;
    curCompany = 0;
    sellWindow = false;
    buyWindow = false;

    interactive = false;
    ui -> nextPushButton -> setText("Next");
}

/**
 * @brief LessonWindow::~LessonWindow - Destroys LessonWindow
 */
LessonWindow::~LessonWindow()
{
    delete ui;
}

/**
 * @brief LessonWindow::on_investPushButton_clicked - For Buy Lesson, makes sure the user wants
 * to buy stocks before committing the "transaction"
 */
void LessonWindow::on_investPushButton_clicked()
{
    // Constructs message to prompt the user
    int quantity = ui->quantitySpinBox->value();
    QString lesson = ui ->investPushButton->text();
    QString questionMessage = "Would you like to " + lesson.toLower() + " " + QString::number(quantity) + " stocks from this company?";

    // Prompts the user if they want to buy the stocks
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Koding Stocks", questionMessage,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // If user wants to buy the stocks, committ the transaction to model
        emit investClicked(quantity);
        ui->quantitySpinBox->setValue(0);
    }
}

/**
 * @brief LessonWindow::displayChanged - Updates the UI to show the different market prices for the different companies and
 * shows your stocks and price of your "purchase"
 *
 * @param quantity - The amount of stocks
 * @param price - The market price for a share
 * @param quantPrice - The total cost to buy the amount of stocks
 */
void LessonWindow::displayChanged(QString quantity, QString price, QString quantPrice){
    ui->quantityLabel->setText(quantity);
    ui->priceLabel->setText(price);
    ui->quantPriceLabel->setText(quantPrice);
}

/**
 * @brief LessonWindow::changeUserMoney - Updates the user money when making a transaction
 *
 * @param userMoney - the new money to display
 */
void LessonWindow::changeUserMoney(QString userMoney) {
    ui->moneyLabel->setText(userMoney);
}

/**
 * @brief LessonWindow::limitWarning - Displays warning pop-up when user
 * does something they are not supposed to
 *
 * @param warningStr - Warning Msg
 */
void LessonWindow::limitWarning(QString warningStr) {
    QMessageBox::warning(this, "Koding Stocks", warningStr);
    int curQuant = ui->quantitySpinBox->value();
    ui->quantitySpinBox->setValue(curQuant-1);
}

/**
 * @brief LessonWindow::on_vfinxPushButton_clicked - Changes graph and price labels to show
 * the stock value of Vfinx company
 */
void LessonWindow::on_vfinxPushButton_clicked()
{
    curCompany = 1;
    displayGraph();

    initializeCompany("Vfiax", ui -> vfinxPushButton);
}

/**
 * @brief LessonWindow::on_teslaPushButton_clicked - Changes graph and price labels to reflect
 * the stock value of Tesla company
 */
void LessonWindow::on_teslaPushButton_clicked()
{
    curCompany = 0;
    displayGraph();

    initializeCompany("Tesla", ui -> teslaPushButton);
}

/**
 * @brief LessonWindow::on_snapPushButton_clicked - Changes graph and price labels to reflect
 * the stock value to Clorax company
 */
void LessonWindow::on_snapPushButton_clicked()
{
    curCompany = 2;
    displayGraph();

    initializeCompany("Clx", ui -> snapPushButton);
}

/**
 * @brief LessonWindow::on_usbPushButton_clicked - Changes graph and price label to reflect
 * the stock value to US Bank company
 */
void LessonWindow::on_usbPushButton_clicked()
{
    curCompany = 3;
    displayGraph();

    initializeCompany("Usb", ui -> usbPushButton);
}

/**
 * @brief LessonWindow::on_amazonPushButton_clicked - Changes graph and price label to reflect
 * the stock value to Amazon company
 */
void LessonWindow::on_amazonPushButton_clicked()
{
    curCompany = 4;
    displayGraph();

    initializeCompany("Amazon", ui -> amazonPushButton);
}

/**
 * @brief LessonWindow::initializeCompany - Changes UI and prepares the model
 *  to show the selected company stock value
 *
 * @param company - Company name selected
 * @param button - Button of Company selected
 */
void LessonWindow::initializeCompany(QString company, QPushButton * button) {
    //sends signal to model
    emit clickedCompany(company);

    //reset button colors and stock quantity
    initializeButtonColors();
    button ->setStyleSheet(QString("background-color: rgb(95, 177, 95);"));
    ui -> quantitySpinBox -> setValue(0);
}

/**
 * @brief LessonWindow::initializeButtonColors - Resets all company button colors
 */
void LessonWindow::initializeButtonColors() {
    // button UI initial colors
    ui-> teslaPushButton -> setStyleSheet(QString("background-color: rgb(190, 228, 190);"));
    ui-> vfinxPushButton ->setStyleSheet(QString("background-color: rgb(190, 228, 190);"));
    ui-> snapPushButton ->setStyleSheet(QString("background-color: rgb(190, 228, 190);"));
    ui-> usbPushButton ->setStyleSheet(QString("background-color: rgb(190, 228, 190);"));
    ui-> amazonPushButton ->setStyleSheet(QString("background-color: rgb(190, 228, 190);"));
}

/**
 * @brief LessonWindow::initializeLessonWindow - Initializes LessonWindow for the buy and sell
 * lessons
 *
 * @param lesson - Lesson Name (Buy/Sell)
 */
void LessonWindow::initializeLessonWindow(QString lesson) {
    // interactive buy lesson
    if(lesson == "Buy") {
        ui -> investPushButton -> setText("Buy");
        buyWindow = true;
        sellWindow = false;
        ui -> nextPushButton -> setEnabled(true);
        ui -> resultLabel -> setText("");
    }
    // interactive sell lesson
    else if(lesson == "Sell") {
        ui -> investPushButton -> setText("Sell");
        ui -> warningLabel -> show();
        sellWindow = true;
        buyWindow = false;
    }
    // initializing Tesla for the buy and sell interactive lessons
    initializeCompany("Tesla", ui->teslaPushButton);
}

/**
 * @brief LessonWindow::completionUpdate - Updates the completion screen
 * and the physics bodies to show movement
 */
void LessonWindow::completionUpdate() {
    world.Step(1.0/60.0, 6, 2);

    update();
}

/**
 * @brief LessonWindow::on_aOption_clicked - The A option was selected for quiz section.
 * Sends the selection to be checked for correctness in model
 */
void LessonWindow::on_aOption_clicked()
{
    selection = ui->aOption;
    selection->setDisabled(true);

    answerIndex = 0;

    emit answerSelected("a", curQuestion);
}

/**
 * @brief LessonWindow::on_bOption_pressed - The B option was selected for quiz section.
 * Sends the selection to be checked for correctness in the model.
 */
void LessonWindow::on_bOption_pressed()
{
    selection = ui->bOption;
    selection->setDisabled(true);

    answerIndex = 1;

    emit answerSelected("b", curQuestion);
}

/**
 * @brief LessonWindow::on_cOption_clicked - The C option was selected for quiz section.
 * Sends the selection to be checked for correctness in the model.
 */
void LessonWindow::on_cOption_clicked()
{
    selection = ui->cOption;
    selection->setDisabled(true);

    answerIndex = 2;

    emit answerSelected("c", curQuestion);
}

/**
 * @brief LessonWindow::on_dOption_clicked - The D option was selected for quiz section.
 * Sends the selection to be checked for correctness in the model
 */
void LessonWindow::on_dOption_clicked()
{
    selection = ui->dOption;
    selection->setDisabled(true);

    answerIndex = 3;

    emit answerSelected("d", curQuestion);
}

/**
 * @brief LessonWindow::changeResultLabel - FOR SELL LESSON: updates the label to reflect your net gain
 * or loss percentage with respect to the original starting budget
 *
 * @param result - The new result
 */
void LessonWindow::changeResultLabel(QString result) {
    ui -> resultLabel -> setText(result);
}


/**
 * @brief LessonWindow::on_completetionButton_clicked - Returns the user to the main window and resets UI to base state. Doesn't clear
 * any data structures
 */
void LessonWindow::on_completetionButton_clicked()
{
    //Hides the current lesson.
    this->hide();
    completed = false;
    timer.stop();
    ui -> nextPushButton -> show();
    update();
    emit mainWindowFocus(true);
    if(ui -> completionLabel -> text() == "You Completed Buy Lesson !"){
        emit buySellButtons(false, true);
    }
    else if(ui -> completionLabel -> text() == "You Completed Sell Lesson !") {
        emit buySellButtons(false, false);
        QString result = ui -> totalResultLabel -> text();
        if(result.indexOf("lost") != -1) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Koding Stocks", result + " Would you like to try again?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                emit buySellButtons(true, false);
            }
        }
        ui -> totalResultLabel -> setText("");
        image = QImage(":/icons/dollarSign.png");
    }
}

/**
 * @brief LessonWindow::closeEvent - Catchs a close to make sure UI is reset to base state
 * @param event
 */
void LessonWindow::closeEvent(QCloseEvent* event) {
    on_completetionButton_clicked();
}
