#include "TcpThread.h"
#include <QSslSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "Coordinate.h"
#include <QVector>
#include <QtCore/qpoint.h>


TcpThread::TcpThread(Coordinate* coord, const QString ip, int port)
    : m_coord(coord), m_ip(ip), m_port(port) {}

void TcpThread::run() {
    QSslSocket socket;

    socket.setPeerVerifyMode(QSslSocket::VerifyNone); //인증서 오류 일단 무시
    socket.ignoreSslErrors();

    //클라가 너무 빨리 연결하려고 하는지 로그 타임스탬프
    qDebug() << "[TcpThread][" << QDateTime::currentDateTime().toString("hh:mm:ss")
             << "] connectToEncryptedHost() 시도";

    socket.connectToHostEncrypted(m_ip, m_port);

    if (!socket.waitForConnected(3000)) {
        qDebug() << "[TcpThread] TLS 연결 실패:" << socket.errorString();
        return;
    }
    qDebug() << "[TcpThread] TLS 연결 성공";

    // 서버가 끊어지면 스레드 중단
    connect(&socket, &QSslSocket::disconnected, this, [this]() {
        requestInterruption();
    }, Qt::DirectConnection);

    while (!isInterruptionRequested() &&
           socket.state() == QAbstractSocket::ConnectedState)
    {
        if (socket.waitForReadyRead(1000)) {
            QByteArray data = socket.readAll();
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

            if (parseError.error != QJsonParseError::NoError) {
                qDebug() << "[TcpThread] JSON 파싱 실패:" << parseError.errorString();
                continue;
            }
            if (!doc.isObject()) {
                qDebug() << "[TcpThread] JSON이 객체 형식이 아님";
                continue;
            }

            QJsonObject obj = doc.object();

            qDebug() << "[TcpThread] obj['angle'] 내용:" << obj["angle"];

            std::vector<int> parsedWidths;

            std::vector<int> parsedAngles;

            if (obj.contains("parsing width") && obj["parsing width"].isArray()) {
                QJsonArray arr = obj["parsing width"].toArray();
                parsedWidths.reserve(arr.size());
                for (const QJsonValue& v : arr) {
                    parsedWidths.push_back(v.toInt());
                }
            }

            if (obj.contains("angle")) {
                const QJsonValue& val = obj["angle"];
                if (val.isArray()) {
                    QJsonArray arr = val.toArray();
                    parsedAngles.reserve(arr.size());
                    for (const QJsonValue& v : arr) {
                        parsedAngles.push_back(v.toInt());
                    }
                } else if (val.isDouble()) {
                    // 단일 값도 받아서 벡터에 하나만 넣기
                    parsedAngles.push_back(val.toInt());
                }
            }


            //공유 구조체에 저장 (뮤텍스 잠금)
            {
                QMutexLocker locker(&m_coord->mutex);
                QVector<int> qWidths;
                qWidths.reserve(parsedWidths.size());
                for (int w : parsedWidths) qWidths.append(w);
                m_coord->width_data = qWidths;

                QVector<int> qAngles;
                qAngles.reserve(parsedAngles.size());
                for (int a : parsedAngles) qAngles.append(a);
                m_coord->angle_data = qAngles;

            }


            qDebug() << "[TcpThread] updated width_data:" << m_coord->width_data << "angle 값"<<m_coord->angle_data;

        }

        // 스레드 중단 요청 처리
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
    }

    socket.disconnectFromHost();
    qDebug() << "[TcpThread] 종료";
}


