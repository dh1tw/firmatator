#include <QtCore/QCoreApplication>

#include <stdio.h>
#include <iostream>

#include <fdevice.h>
#include <QtCore/QDebug>

#include "ftest.h"

using namespace std;
using namespace firmatator;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FTest* t = new FTest();

    t->initialize();

    return a.exec();
}
