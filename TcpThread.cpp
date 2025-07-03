#include "TcpThread.h"
#include <QStringList>

void TcpThread::run() {
    QTcpSocket socket;
    socket.connectToHost("192.168.0.60",8554);  // IP 및 포트 조정

    if (!socket.waitForConnected(3000)) {
        qDebug() << "[TcpThread] 연결 실패";
        return;
    }
       qDebug() << "[TcpThread] 연결 성공";
    while (true) {
        if (socket.waitForReadyRead(1000)) {
            QByteArray data = socket.readAll();
            QString str = QString::fromUtf8(data).trimmed();
            QStringList parts = str.split(",");

            if (parts.size() >= 2) {
                int x = parts[0].toInt();
                int y = parts[1].toInt();

                m_coord->mutex.lock();
                m_coord->x = x;
                m_coord->y = y;
                m_coord->mutex.unlock();

                qDebug() << "[TcpThread] 받은 좌표:" << x << y;
            }
        }
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
    }

    socket.disconnectFromHost();
    qDebug() << "[TcpThread] 종료";
}

