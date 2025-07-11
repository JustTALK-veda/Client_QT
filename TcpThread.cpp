#include "TcpThread.h"
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "Coordinate.h"
#include <QVector>

TcpThread::TcpThread(Coordinate* coord, const QString& ip, int port)
    : m_coord(coord), m_ip(ip), m_port(port) {}

void TcpThread::run() {
    QTcpSocket socket;
    //클라가 너무 빨리 연결하려고 하는지 로그 타임스탬프
    qDebug() << "[TcpThread][" << QDateTime::currentDateTime().toString("hh:mm:ss")
             << "] connectToHost() 시도";

    socket.connectToHost(m_ip, m_port);

    if (!socket.waitForConnected(1000)) {
        qDebug() << "[TcpThread] 연결 실패:" << socket.errorString();
        return;
    }
    qDebug() << "[TcpThread] 연결 성공";

    while (true) {
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

            // width_data 파싱
            std::vector<int> parsedWidths;

           // std::vector<int> parseAngles;

            if (obj.contains("parsing width") && obj["parsing width"].isArray()) {
                QJsonArray arr = obj["parsing width"].toArray();
                parsedWidths.reserve(arr.size());
                for (const QJsonValue& v : arr) {
                    parsedWidths.push_back(v.toInt());
                }
            }

            // if (obj.contains("angle") && obj["angle"].toInt()) {
            //     int parsedAngle = obj["angle"].toInt();
            //     m_coord->angles = parsedAngle;  // angle 값을 저장
            //     qDebug() << "[TcpThread] 받은 angle 값:" << m_coord->angles;
            // }


            qDebug() << "[TcpThread] 받은 width_data:" << parsedWidths;
            // if (obj.contains("parseAngles") && obj["parseAngles"].isArray()) {
            //     QJsonArray arr = obj["parseAngles"].toArray();
            //     parsedWidths.reserve(arr.size());
            //     for (const QJsonValue& v : arr) {
            //         parseAngles.push_back(v.toInt());
            //     }
            // }
            //speaker_num 파싱
           // int parsedSpeaker = obj.value("speaker").toInt();


            //공유 구조체에 저장 (뮤텍스 잠금)
            {
                QMutexLocker locker(&m_coord->mutex);
                QVector<int> qWidths;
                qWidths.reserve(parsedWidths.size());
                for (int w : parsedWidths) qWidths.append(w);
                // QVector<int> qAngles;
                // qAngles.reserve(parseAngles.size());
                // for (int angle : parseAngles) {
                //     qAngles.append(angle);
                // }
                if (obj.contains("angle") && obj["angle"].toInt()) {
                    int parsedAngle = obj["angle"].toInt();
                    m_coord->angles = parsedAngle;  // angle 값 저장

                }

                m_coord->width_data = qWidths;

                //m_coord->speaker_num = parsedSpeaker;
                //m_coord->angles = qAngles;
            }
            qDebug() << "[TcpThread] updated width_data:" << m_coord->width_data
                     << ",angles값:" << m_coord->angles;
        }

        // 스레드 중단 요청 처리
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
    }

    socket.disconnectFromHost();
    qDebug() << "[TcpThread] 종료";
}
