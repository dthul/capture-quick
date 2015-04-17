#include "triggerbox.h"

#include <chrono>
#include <iostream>
#include <thread>

#include <QSerialPortInfo>

const quint16 VID = 0x0483; // STMicroelectronics
const quint16 PID = 0x374B; // Nucleo-F401RE

// TODO: make singleton since only one serial port connection
// can be open at a time anyway.
TriggerBox::TriggerBox(QObject *parent) : QObject(parent)
{
}

TriggerBox::~TriggerBox()
{
    if (serialPort) {
        if (serialPort->isOpen())
            serialPort->close();
        delete serialPort;
    }
}

void TriggerBox::init() {
    serialPort = new QSerialPort();
    QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
    for (auto info : portInfos) {
        if (info.vendorIdentifier() == VID && info.productIdentifier() == PID) {
            serialPort->setPort(info);
            //serialPort->setBaudRate(QSerialPort::Baud9600);
        }
    }
    // serialPort.setBaudRate(QSerialPort::Baud9600); // TODO: which baud rate?
    if (!serialPort->open(QIODevice::ReadWrite)) {
        std::cerr << "Failed to open serial connection to the trigger box" << std::endl;
        // TODO: throw error instead?
    }
    else {
        std::cout << "Serial connection opened" << std::endl;
        //connect(serialPort, &QSerialPort::readyRead, this, &TriggerBox::readSerial);
        serialPort->setBaudRate(QSerialPort::Baud9600);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
    }
}

void TriggerBox::readSerial() {
    QByteArray data = serialPort->readAll();
    std::cout << "Read: " << QString(data).toStdString() << std::endl;
}

void TriggerBox::focusAll() {
    if (!serialPort || !serialPort->isWritable()) {
        std::cerr << "Serial port is not writable, will not focus" << std::endl;
        return;
    }
    write("1111111111F ");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    write("R ");
}

void TriggerBox::triggerAll() {
    if (!serialPort || !serialPort->isWritable()) {
        std::cerr << "Serial port is not writable, will not trigger" << std::endl;
        return;
    }
    write("1111111111S ");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    write("R");
}

bool TriggerBox::write(const QString& data) {
    if (!serialPort || !serialPort->isWritable())
        return false;
    serialPort->readAll();
    QByteArray bytes = data.toLatin1();
    char const * const cdata = bytes.data();
    for (int i = 0; i < data.size(); ++i) {
        qint64 bytesWritten = serialPort->write(cdata + i, 1);
        if (bytesWritten == -1) {
            std::cerr << "Failed to write the data to the trigger box: " << serialPort->errorString().toStdString() << std::endl;
            return false;
        } else if (bytesWritten != 1) {
            std::cerr << "Failed to write all the data to the trigger box: " << serialPort->errorString().toStdString() << std::endl;
            return false;
        } else if (!serialPort->waitForBytesWritten(5000)) {
            std::cerr << "Operation timed out or an error occurred for the trigger box: " << serialPort->errorString().toStdString() << std::endl;
            return false;
        }
        else {
            std::cout << "Wrote: " << bytes.at(i) << std::endl;
        }
        if (!serialPort->waitForReadyRead(5000)) {
            std::cerr << "Got no response" << std::endl;
            return false;
        }
        char response;
        qint64 bytesRead = serialPort->read(&response, 1);
        if (bytesRead == -1) {
            std::cerr << "Failed to read the data from the trigger box: " << serialPort->errorString().toStdString() << std::endl;
            return false;
        } else if (bytesWritten != 1) {
            std::cerr << "Failed to read all the data from the trigger box: " << serialPort->errorString().toStdString() << std::endl;
            return false;
        } else {
            std::cout << "Read: " << response << std::endl;
        }
    }
    return true;
}
