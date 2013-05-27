#ifndef FTEST_H
#define FTEST_H

#include <stdio.h>
#include <iostream>

#include <fdevice.h>
#include <QtCore/QDebug>

#include <QTimer>


class FTest : public QObject
{
    Q_OBJECT

public:

    firmatator::FDevice* arduino;

    int d;
    int i;
    int a;
    int s;
    int a_ud;
    int s_ud;

    QTimer* activeTimer;

    FTest();


public slots:
    void initialize();
    void start();
    void step();
    void printInputs();
};

#endif // FTEST_H
