#include "triggerbox.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

#include <QMutexLocker>
#include <QSerialPortInfo>

const quint16 VID = 0x0483; // STMicroelectronics
const quint16 PID = 0x374B; // Nucleo-F401RE

TriggerBox* TriggerBox::triggerBox = nullptr;
QMutex TriggerBox::s_mutex;

#define CHECK_SUCCESS(X)\
    if ((X) != true) {\
        std::cerr << "Serial port is not connected" << std::endl;\
        return;\
    }

TriggerBox* TriggerBox::getInstance() {
    QMutexLocker locker(&s_mutex);
    if (!triggerBox)
        triggerBox = new TriggerBox();
    return triggerBox;
}

TriggerBox::TriggerBox(QObject *parent) :
    QObject(parent),
    m_serialPort(nullptr)
{
    this->moveToThread(&m_thread);
    QObject::connect(&m_thread, &QThread::started, this, &TriggerBox::ensureConnection);
    m_thread.start();
}

TriggerBox::~TriggerBox()
{
    disconnect();
    m_thread.quit();
    m_thread.wait(2000);
}

bool TriggerBox::connect() {
    if (m_serialPort)
        disconnect();
    m_serialPort = new QSerialPort();
    QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
    for (auto info : portInfos) {
        if (info.vendorIdentifier() == VID && info.productIdentifier() == PID) {
            m_serialPort->setPort(info);
        }
    }
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        std::cerr << "Failed to open serial connection to the trigger box" << std::endl;
        // TODO: throw error instead?
        return false;
    }
    else {
        std::cout << "Serial connection opened" << std::endl;
    }
    return true;
}

void TriggerBox::disconnect() {
    if (m_serialPort) {
        if (m_serialPort->isOpen())
            m_serialPort->close();
        delete m_serialPort;
        m_serialPort = nullptr;
    }
}

bool TriggerBox::ensureConnection() {
    if (m_serialPort && m_serialPort->isReadable() && m_serialPort->isWritable() && write("X"))
        return true;
    return connect();
}

void TriggerBox::readSerial() {
    QByteArray data = m_serialPort->readAll();
}

void TriggerBox::halfPressShutterAll(const int milliseconds) {
    QMutexLocker locker(&m_mutex);
    CHECK_SUCCESS(ensureConnection())
    CHECK_SUCCESS(write("1111111111F "))
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    CHECK_SUCCESS(write("R"))
}

void TriggerBox::pressShutterAll(const int milliseconds) {
    QMutexLocker locker(&m_mutex);
    CHECK_SUCCESS(ensureConnection())
    CHECK_SUCCESS(write("1111111111S "))
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    CHECK_SUCCESS(write("R"))
}

bool TriggerBox::write(const QString& data) {
    if (!m_serialPort || !m_serialPort->isWritable())
        return false;
    m_serialPort->readAll();
    QByteArray bytes = data.toLatin1();
    char const * const cdata = bytes.data();
    for (int i = 0; i < data.size(); ++i) {
        qint64 bytesWritten = m_serialPort->write(cdata + i, 1);
        if (bytesWritten == -1) {
            std::cerr << "Failed to write the data to the trigger box: " << m_serialPort->errorString().toStdString() << std::endl;
            disconnect();
            return false;
        } else if (bytesWritten != 1) {
            std::cerr << "Failed to write all the data to the trigger box: " << m_serialPort->errorString().toStdString() << std::endl;
            disconnect();
            return false;
        } else if (!m_serialPort->waitForBytesWritten(5000)) {
            std::cerr << "Operation timed out or an error occurred for the trigger box: " << m_serialPort->errorString().toStdString() << std::endl;
            disconnect();
            return false;
        }
        if (!m_serialPort->waitForReadyRead(5000)) {
            std::cerr << "Got no response" << std::endl;
            disconnect();
            return false;
        }
        char response;
        qint64 bytesRead = m_serialPort->read(&response, 1);
        if (bytesRead == -1) {
            std::cerr << "Failed to read the data from the trigger box: " << m_serialPort->errorString().toStdString() << std::endl;
            disconnect();
            return false;
        } else if (bytesWritten != 1) {
            std::cerr << "Failed to read all the data from the trigger box: " << m_serialPort->errorString().toStdString() << std::endl;
            disconnect();
            return false;
        }
    }
    return true;
}
