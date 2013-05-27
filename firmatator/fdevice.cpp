
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <QtCore/QDebug>

#include "util.h"

#include <fdevice.h>

using namespace firmatator;


FDevice::FDevice(QString serialport, int baudrate)
{
    parserBuffer = (uint8_t*) malloc(4096);
    connected = false;

    if (serialport == "")
    {
        QList<SerialPortInfo> list = SerialPortInfo::availablePorts();
        serialport = list.value(0).portName();
    }

    this->serial = new SerialPort();
    this->serial->setRate(baudrate);
    this->serial->setPort(serialport);

    QObject::connect(this->serial, SIGNAL(readyRead()), this, SLOT(processSerial()));
    QObject::connect(this, SIGNAL(deviceReady()), this, SLOT(initialize()));
}

bool FDevice::connect()
{
    connected = this->serial->open(QIODevice::ReadWrite);

    if (connected)
        reportFirmware();

    return connected;
}

void FDevice::disconnect()
{
    this->serial->close();
    connected = false;
    ready = false;
}

void FDevice::initialize()
{
    requestCapabilities();
    reportPins();
}

void FDevice::reportFirmware()
{
    char* buffer = (char*) malloc(3);

    buffer[0] = COMMAND_START_SYSEX;
    buffer[1] = COMMAND_REPORT_FIRMWARE;
    buffer[2] = COMMAND_END_SYSEX;

    QByteArray s = QByteArray(buffer, 3);

    serial->write(s);
    serial->flush();
}

// This informs if a pin can be analog, digital in/out, servo or what (I think);
void FDevice::requestCapabilities()
{
    char* buffer = (char*) malloc(6);
    int len = 0;
    buffer[len++] = COMMAND_START_SYSEX;
    buffer[len++] = COMMAND_ANALOG_MAPPING_QUERY; // read analog to pin # info
    buffer[len++] = COMMAND_END_SYSEX;
    buffer[len++] = COMMAND_START_SYSEX;
    buffer[len++] = COMMAND_CAPABILITY_QUERY; // read capabilities
    buffer[len++] = COMMAND_END_SYSEX;

    QByteArray s = QByteArray(buffer, 6);

    serial->write(s);
    serial->flush();
}

// Check what the hell makes the REPORT_ANALOG / REPORT_DIGITAL
// Maybe it says the device to constantly send information about this pins?
void FDevice::reportPins()
{
    char* buffer = (char*) malloc(2);

    for (int i = 0; i < 16; i++)
    {
        buffer[0] = COMMAND_REPORT_ANALOG | i;
        buffer[1] = 1;

        QByteArray s = QByteArray(buffer, 2);
        serial->write(s);
        serial->flush();
        //qDebug() << "Pido analogica " << i;
    }

    for (int i = 0; i < 2; i++)
    {
        buffer[0] = COMMAND_REPORT_DIGITAL | i;
        buffer[1] = 1;

        QByteArray s = QByteArray(buffer, 2);
        serial->write(s);
        serial->flush();
        //qDebug() << "Pido digital " << i;
     }
}

// Check what the hell makes the PIN_STATE_QUERY
// Similar to the previous one? Im not using it now
void FDevice::requestInputs()
{
    for (int pin = 0; pin < 128; pin++)
    {
        char* buffer = (char*) malloc(4);
        int len=0;

        buffer[0] = COMMAND_START_SYSEX;
        buffer[1] = COMMAND_PIN_STATE_QUERY;
        buffer[2] = pin;
        buffer[3] = COMMAND_END_SYSEX;

        QByteArray s = QByteArray(buffer, 4);

        serial->write(s);
        serial->flush();
    }
}

void FDevice::pinMode(int pin, int mode)
{
    /*
    char* buffer = (char*) malloc(6);
    //char buffer[2];
    buffer[1] = lowerToHex(COMMAND_SET_PIN_MODE);
    buffer[0] = upperToHex(COMMAND_SET_PIN_MODE);
    buffer[3] = lowerToHex(pin);
    buffer[2] = upperToHex(pin);
    buffer[5] = lowerToHex(mode);
    buffer[4] = upperToHex(mode);
    //qDebug() << buffer;

    //QByteArray s = QByteArray(buffer, 6);
    QByteArray s = QByteArray::fromRawData(buffer, 6);
    qDebug() << s;
    //serial->write(QByteArray(buffer), 2);
    */

    char* buffer = (char*) malloc(3);
    buffer[0] = COMMAND_SET_PIN_MODE;
    buffer[1] = pin;
    buffer[2] = mode;
    //QByteArray s = QByteArray::fromRawData(buffer, 3);
    QByteArray s = QByteArray(buffer, 3);
    //qDebug() << s;

    serial->write(s);
    //qDebug() << serial->bytesToWrite();
    serial->flush();

    //qDebug() << "Command: " << (char)(COMMAND_SET_PIN_MODE) << "  --  " << upperToHex(COMMAND_SET_PIN_MODE) << lowerToHex(COMMAND_SET_PIN_MODE) << " / Pin: " << lowerToHex(pin) << " / Mode: " << lowerToHex(mode);
}

bool FDevice::available()
{
    return connected && ready;
}


int FDevice::digitalRead(int pin)
{
    return digitalInputData[pin];
}

void FDevice::digitalWrite(int pin, int value)
{
    int portNumber = (pin >> 3) & 0x0F;

    if (value == 0)
         digitalOutputData[portNumber] &= ~(1 << (pin & 0x07));
    else
         digitalOutputData[portNumber] |= (1 << (pin & 0x07));

    /*
    char* buffer = (char*) malloc(6);
    //char buffer[2];

    buffer[1] = lowerToHex(COMMAND_DIGITAL_MESSAGE | portNumber);
    buffer[0] = upperToHex(COMMAND_DIGITAL_MESSAGE | portNumber);
    buffer[3] = lowerToHex(digitalOutputData[portNumber] & 0x7F);
    buffer[2] = upperToHex(digitalOutputData[portNumber] & 0x7F);
    buffer[5] = lowerToHex(digitalOutputData[portNumber] >> 7);
    buffer[4] = upperToHex(digitalOutputData[portNumber] >> 7);

    //QByteArray s = QByteArray(buffer, 6);
    QByteArray s = QByteArray::fromRawData(buffer, 6);
    qDebug() << s;
    */

    char* buffer = (char*) malloc(3);
    buffer[0] = COMMAND_DIGITAL_MESSAGE | portNumber;
    buffer[1] = digitalOutputData[portNumber] & 0x7F;
    buffer[2] = digitalOutputData[portNumber] >> 7;
    //QByteArray s = QByteArray::fromRawData(buffer, 3);
    QByteArray s = QByteArray(buffer, 3);
    //qDebug() << s;

    serial->write(s);
    //qDebug() << serial->bytesToWrite();
    serial->flush();


    //qDebug() << "Digital Port Number (Internal array):" <<  digitalOutputData[portNumber];
    //qDebug() << "Command: " << lowerToHex(COMMAND_DIGITAL_MESSAGE | portNumber) << " / LSB: " << lowerToHex(digitalOutputData[portNumber] & 0x7F) << " / MSB: " << lowerToHex(digitalOutputData[portNumber] >> 7);
}

int FDevice::analogRead(int pin)
{
    return analogInputData[pin - 14];
}

void FDevice::analogWrite(int pin, int value)
{
    //this->pinMode(pin, PINMODE_PWM);

    char* buffer = (char*) malloc(3);
    buffer[0] = COMMAND_ANALOG_MESSAGE | (pin & 0x0F);
    buffer[1] = value & 0x7F;
    buffer[2] = value >> 7;

    QByteArray s = QByteArray(buffer, 3);
    //qDebug() << s;

    serial->write(s);
    //qDebug() << serial->bytesToWrite();
    serial->flush();

}

void FDevice::setServo(int pin, int value)
{
    //this->pinMode(pin, PINMODE_PWM);

    this->analogWrite(pin, value);

}

void FDevice::I2CRequest(int addr, int data[], int mode)
{
    char* buffer = (char*) malloc(5 + sizeof(data) * 2);

    int addr_mode;
    if (addr < 0xFF)
        addr_mode = 0x20;                          // 7 bits mode
    else
        addr_mode = 0x20;                       // 10 bits mode

    int pos = 0;
    buffer[pos++] = COMMAND_START_SYSEX;
    buffer[pos++] = COMMAND_I2C_REQUEST;
    buffer[pos++] = addr & 0xFF;                // the lsb of the address (full address for 7bits address, the most common)
    buffer[pos] = (addr & 0x380) >> 7;          // get the 3 msb for the addres in the case of 10bits i2c mode
    qDebug() << "b1: " << toBinary(buffer[pos]);
    buffer[pos] = buffer[pos] || addr_mode;     // add the addr mode to the byte
    qDebug() << "b2: " << toBinary(buffer[pos]);
    buffer[pos] = buffer[pos] || (mode << 2);   // add the read/write mode into position
    qDebug() << "b3: " << toBinary(buffer[pos]) << " / mode: " << toBinary(mode) << " / mode << 2: " << toBinary(0xFF) << "   ";
    pos++;

    for (int i; i < sizeof(data) / sizeof(data[0]); i += 2)
    {
        buffer[pos++] = data[i] & 0x0F;
        buffer[pos++] = (data[i + 1] & 0xF0) >> 8;
    }

    buffer[pos++] = COMMAND_END_SYSEX;

    QByteArray s = QByteArray(buffer, pos);
    //qDebug() << s;

    serial->write(s);
    //qDebug() << serial->bytesToWrite();
    serial->flush();

    qDebug() << "";
    qDebug() << "Envio I2C Request:";

    for (int i = 0; i < s.size(); i++)
    {
        qDebug() << toBinary(s[i]) << "   " << hex << buffer[i];
    }
}

void FDevice::I2CConfig(int pinState, int delay)
{
    char* buffer = (char*) malloc(6);

    int pos = 0;
    buffer[pos++] = COMMAND_START_SYSEX;
    buffer[pos++] = COMMAND_I2C_CONFIG;
    buffer[pos++] = pinState;
    buffer[pos++] = delay & 0xFF;
    buffer[pos++] = delay >> 7;
    buffer[pos++] = COMMAND_END_SYSEX;

    qDebug() << "Envio I2C Config:";

    QByteArray s = QByteArray(buffer, pos);
    //qDebug() << s;

    serial->write(s);
    //qDebug() << serial->bytesToWrite();
    serial->flush();

    for (int i = 0; i < s.size(); i++)
    {
        qDebug() << toBinary(s[i]) << "   " << hex << buffer[i];
    }
}

void FDevice::processSerial()
{
    int len = this->serial->bytesAvailable();

    //qDebug() << "Read buffer: " << len;

    QByteArray r = this->serial->readAll();

    //qDebug() << "Datos leidos: " << r;

    for (int i = 0; i< r.length(); i++)
    {
        uint8_t c = r[i];
        uint8_t msn = c & 0xF0;

        if (msn == COMMAND_ANALOG_MESSAGE || msn == COMMAND_DIGITAL_MESSAGE || c == COMMAND_REPORT_VERSION)
        {
            parserCommandLenght = 3;
            parserReceivedCount = 0;
        }
        else if (msn == COMMAND_REPORT_ANALOG || msn == COMMAND_REPORT_DIGITAL)
        {
            parserCommandLenght = 2;
            parserReceivedCount = 0;
        }
        else if (c == COMMAND_START_SYSEX)
        {
            parserCommandLenght = 4096; // r.length(); //sizeof(parse_buf);
            parserReceivedCount = 0;
        }
        else if (c == COMMAND_END_SYSEX)
        {
            parserCommandLenght = parserReceivedCount + 1;
        }
        else if (c & 0x80)
        {
            parserCommandLenght = 1;
            parserReceivedCount = 0;
        }


        //qDebug() << "Constructing command... " << parserReceivedCount + 1 << "  /  " << parserCommandLenght;


        if (parserReceivedCount <= parserCommandLenght)
        {
            parserBuffer[parserReceivedCount] = c;
            parserReceivedCount++;

            if (parserReceivedCount == parserCommandLenght)
            {
                receiving = false;
                parseBuffer();
                parserReceivedCount = parserCommandLenght = 0;
            }
        }
        else if (parserReceivedCount >= parserCommandLenght - 1)
        {
            receiving = false;
            parserReceivedCount = parserCommandLenght = 0;
        }
    }
}

void FDevice::parseBuffer()
{
    //qDebug() << "Parsing command...";

    uint8_t cmd = (parserBuffer[0] & 0xF0);
    //qDebug() << "COMMAND TO PROCESS > " << hex << cmd << " with " << dec << parserReceivedCount << " paramenters.";

    if (cmd == COMMAND_ANALOG_MESSAGE && parserReceivedCount == 3)
    {
            int analog_ch = (parserBuffer[0] & 0x0F);
            int analog_val = parserBuffer[1] | (parserBuffer[2] << 7);

            analogInputData[analog_ch] = analog_val;

            //qDebug() << "Set analog channel " << analog_ch << " to value " << analog_val;
            /*for (int pin=0; pin < 128; pin++)
            {

                    if (pin_info[pin].analog_channel == analog_ch)
                    {

                        //pin_info[pin].value = analog_val;
                        //printf("pin %d is A%d = %d\n", pin, analog_ch, analog_val);
                    }
            }*/
    }
    else if (cmd == COMMAND_DIGITAL_MESSAGE && parserReceivedCount == 3)
    {
            int port_num = (parserBuffer[0] & 0x0F);
            int port_val = parserBuffer[1] | (parserBuffer[2] << 7);
            int pin = port_num * 8;

            for (int mask=1; mask & 0xFF; mask <<= 1, pin++)
            {
                    uint32_t val = (port_val & mask) ? 1 : 0;
                    digitalInputData[pin] = val;

                    //qDebug() << "Set digital pin " << pin << " to value " << val;
                    /*
                    if (pin_info[pin].mode == MODE_INPUT)
                    {
                            uint32_t val = (port_val & mask) ? 1 : 0;
                            if (pin_info[pin].value != val)
                            {
                                    printf("pin %d is %d\n", pin, val);
                                    wxStaticText *text = (wxStaticText *)
                                      FindWindowById(5000 + pin, scroll);
                                    if (text) text->SetLabel(val ? "High" : "Low");
                                    pin_info[pin].value = val;
                            }
                    }
                    */
            }
    }
    else if (parserBuffer[0] == COMMAND_START_SYSEX && parserBuffer[parserReceivedCount - 1] == COMMAND_END_SYSEX)
    {
        qDebug("Sexy sysex!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

        if (parserBuffer[1] == COMMAND_REPORT_FIRMWARE)
        {
            //qDebug("Firmware report arriving...");
            char name[140];
            int len = 0;

            for (int i = 4; i < parserReceivedCount - 2; i += 2)
            {
                    name[len++] = (parserBuffer[i] & 0x7F) | ((parserBuffer[i+1] & 0x7F) << 7);
            }

            name[len++] = '-';
            name[len++] = parserBuffer[2] + '0';
            name[len++] = '.';
            name[len++] = parserBuffer[3] + '0';
            name[len++] = 0;
            firmataName = name;

            qDebug() << "Firmata version: " << QString::fromStdString(firmataName);

            ready = true;
            emit deviceReady();
        }
        else if (parserBuffer[1] == COMMAND_CAPABILITY_RESPONSE)
        {
                /*
                int pin, i, n;
                for (pin=0; pin < 128; pin++)
                {
                        pin_info[pin].supported_modes = 0;
                }
                for (i=2, n=0, pin=0; i<parse_count; i++)
                {
                        if (parse_buf[i] == 127)
                        {
                                pin++;
                                n = 0;
                                continue;
                        }
                        if (n == 0) {
                                // first byte is supported mode
                                pin_info[pin].supported_modes |= (1<<parse_buf[i]);
                        }
                        n = n ^ 1;
                }
                // send a state query for for every pin with any modes

                requestInputs();

                /*
                for (pin=0; pin < 128; pin++) {
                        uint8_t buf[512];
                        int len=0;
                        if (pin_info[pin].supported_modes)
                        {
                                buf[len++] = START_SYSEX;
                                buf[len++] = PIN_STATE_QUERY;
                                buf[len++] = pin;
                                buf[len++] = END_SYSEX;
                        }
                        port.Write(buf, len);
                        tx_count += len;
                }
                */
        }
        else if (parserBuffer[1] == COMMAND_ANALOG_MAPPING_RESPONSE)
        {
                /*
                int pin=0;
                for (int i=2; i<parse_count-1; i++)
                {
                        pin_info[pin].analog_channel = parse_buf[i];
                        pin++;
                }
                return;
                */
        }
        else if (parserBuffer[1] == COMMAND_PIN_STATE_RESPONSE && parserReceivedCount >= 6)
        {
                /*
                int pin = parse_buf[2];
                pin_info[pin].mode = parse_buf[3];
                pin_info[pin].value = parse_buf[4];
                if (parse_count > 6) pin_info[pin].value |= (parse_buf[5] << 7);
                if (parse_count > 7) pin_info[pin].value |= (parse_buf[6] << 14);
                add_pin(pin);
                */
        }
        //elseif (parserBuffer[1] == COMMAND_)
        else
        {
            qDebug() << "Sysex command not recognized: " << hex << parserBuffer[1];
        }
    }
    else
    {
        qDebug() << "Command not recognized!!!!!!!!!!: " << hex << parserBuffer[0] << "(" << dec << parserReceivedCount << " parameters)";

        for (int i = 0; i < parserReceivedCount; i++)
        {
            qDebug() << parserBuffer[i] << " / " << hex << parserBuffer[i];
        }
    }

    //qDebug() << "Command completed!!";
}

