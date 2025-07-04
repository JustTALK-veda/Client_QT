#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include <QThread>
#include "Coordinate.h"

class TcpThread : public QThread {
    Q_OBJECT
public:
    TcpThread(Coordinate* coord, const QString& ip, int port);
    void run() override;

private:
    Coordinate* m_coord;
    QString m_ip;
    int m_port;
};

#endif // TCPTHREAD_H
