#pragma once

#include <QObject>
#include <QtSerialPort/QtSerialPort>

class TriggerBox : public QObject
{
    Q_OBJECT
public:
    explicit TriggerBox(QObject *parent = 0);
    ~TriggerBox();

signals:

public slots:
    void focusAll();
    void triggerAll();
    void init();
    void readSerial();
private:
    bool write(const QString& data);
    QSerialPort *serialPort;
    //boost::asio::serial_port *bserialPort;
};
