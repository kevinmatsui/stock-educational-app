/**
 * Dana Ahn, Alberto del Rossario, Ethan Kai Falconer, Kevin Matsui, Aidan Naito
 * Assignment 8 : Education App - Koding Stocks
 * CS 3505
 * April 26, 2022
 *
 * This is how our program starts
 */

#include "mainwindow.h"
#include "model.h"

#include <QApplication>
#include <Box2D/Box2D.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    B2_NOT_USED(argc);
    B2_NOT_USED(argv);

    // Define the gravity vector.
    b2Vec2 gravity(0.0f, -10.0f);

    // Construct a world object, which will hold and simulate the rigid bodies.
    b2World world(gravity);

    Model model;
    LessonWindow lesson(model);
    MainWindow w(model, &lesson, &world);
    w.show();

    return a.exec();
}
