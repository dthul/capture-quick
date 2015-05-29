#pragma once

#include <QMutex>
#include <QObject>
#include <QThread>
#include <QtSerialPort/QtSerialPort>

class TriggerBox : public QObject
{
    Q_OBJECT
public:
    ~TriggerBox();
    static TriggerBox* getInstance();
signals:

public slots:
    void halfPressShutterAll(const int milliseconds = 200);
    void pressShutterAll(const int milliseconds = 200);
private:
    bool connect();
    void disconnect();
    void readSerial();
    bool ensureConnection();
    explicit TriggerBox(QObject *parent = 0);
    bool write(const QString& data);
    QSerialPort *m_serialPort;
    QThread m_thread;
    QMutex m_mutex;
    static TriggerBox* triggerBox;
    static QMutex s_mutex;
};
