#include "ftest.h"

#include <fdevice.h>


#include <QtCore/QDebug>
#include <QTimer>

using namespace firmatator;

FTest::FTest()
{
    d = 0;
    i = 0;
    a = 0;
    s = 0;
    a_ud = 0;
    s_ud = 0;

    //firmatator::FDevice a("/dev/ttyUSB0");
    //arduino = &a;

    //QTimer activeTimer(this);
}


void FTest::initialize()
{
    this->arduino = new FDevice("/dev/ttyUSB0");
    connect(this->arduino, SIGNAL(deviceReady()), this, SLOT(start()));

    qDebug("Connecting...");

    if (arduino->connect())
    {
        qDebug("Succesfully connected");
    }
    else
    {
        qDebug("Error connecting");
    }
}

void FTest::start()
{
    activeTimer = new QTimer(this);
    activeTimer->setInterval(1*1000);
    activeTimer->setSingleShot(false);
    connect(activeTimer, SIGNAL(timeout()), this, SLOT(step()));

    qDebug() << "Initializing ports...";

    arduino->pinMode(13, FDevice::PINMODE_OUTPUT);
    //arduino->pinMode(3, FDevice::PINMODE_OUTPUT);
    arduino->pinMode(3, FDevice::PINMODE_PWM);
    arduino->pinMode(2, FDevice::PINMODE_SERVO);

    arduino->pinMode(8, FDevice::PINMODE_INPUT);
    arduino->pinMode(9, FDevice::PINMODE_INPUT);
    arduino->pinMode(14, FDevice::PINMODE_ANALOG);

    arduino->pinMode(18, FDevice::PINMODE_I2C);
    arduino->pinMode(19, FDevice::PINMODE_I2C);

    int readcmd[] = {0x00};
    int readbyte[] = {0x02};
    arduino->I2CConfig(0, 100);
    arduino->I2CRequest(0x70, readcmd, FDevice::I2C_MODE_READ_ONCE);
    //arduino->I2CRequest(0x70, readcmd, FDevice::I2C_MODE_READ_ONCE);

    qDebug() << "Succesfully initialized.";

    qDebug() << "Starting loop...";
    activeTimer->start();
}

void FTest::step()
{
    if (arduino->available())
    {
        i++;

        qDebug() << "";
        qDebug() << "----------------------------------------------------------";
        qDebug() << "";
        qDebug() << "Iteraction " << dec << i;

        if (i % 2 == 0)
        {
            d = FDevice::DIGITAL_HIGH;
        }
        else
        {
            d = FDevice::DIGITAL_LOW;
        }

        arduino->digitalWrite(13, d);
        //arduino->digitalWrite(3, d);

        if (a >= 250)
            a_ud = 1;
        else if (a <= 0)
            a_ud = 0;

        if (a_ud == 0)
            a += 50;
        else
            a -= 50;

        if (s >= 180)
            s_ud = 1;
        else if (s <= 0)
            s_ud = 0;

        if (s_ud == 0)
            s += 20;
        else
            s -= 20;


        arduino->analogWrite(3, a);
        arduino->setServo(2, s);


        qDebug() << "Digital Out (3):" << d << " / Analog out (3): " << a << " / Servo out (2): " << s;


        QTimer* wait = new QTimer(this);
        wait->setInterval(100);
        wait->setSingleShot(true);
        connect(wait, SIGNAL(timeout()), this, SLOT(printInputs()));
        wait->start();
    }
    else
    {
        qDebug() << "Connection with device lost or device not ready...";
    }
}

void FTest::printInputs()
{
    qDebug() << "Digital In (8): " << arduino->digitalRead(8) << " / Analog In (14, 15, 16, 17, 18, 19): " <<  arduino->analogRead(14) << ", " << arduino->analogRead(15) << ", " << arduino->analogRead(16) << ", " << arduino->analogRead(17) << ", " << arduino->analogRead(18) << ", " << arduino->analogRead(19);
}
