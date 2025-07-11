// #include "TcpThread.h"
// #include <QTcpSocket>
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <QDebug>
// #include "Coordinate.h"
// #include <QVector>

// TcpThread::TcpThread(Coordinate* coord, const QString& ip, int port)
//     : m_coord(coord), m_ip(ip), m_port(port) {}

// void TcpThread::run() {
//     QTcpSocket socket;
//     //클라가 너무 빨리 연결하려고 하는지 로그 타임스탬프
//     qDebug() << "[TcpThread][" << QDateTime::currentDateTime().toString("hh:mm:ss")
//              << "] connectToHost() 시도";

//     socket.connectToHost(m_ip, m_port);

//     if (!socket.waitForConnected(1000)) {
//         qDebug() << "[TcpThread] 연결 실패:" << socket.errorString();
//         return;
//     }
//     qDebug() << "[TcpThread] 연결 성공";

//     while (true) {
//         if (socket.waitForReadyRead(1000)) { //데이터가 올때까지 대기,데이터를 받으면 바로 처리&오지않으면 계속 기다림
//             QByteArray data = socket.readAll();
//             QJsonParseError parseError;
//             QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

//             if (parseError.error != QJsonParseError::NoError) {
//                 qDebug() << "[TcpThread] JSON 파싱 실패:" << parseError.errorString();
//                 continue;
//             }
//             if (!doc.isObject()) {
//                 qDebug() << "[TcpThread] JSON이 객체 형식이 아님";
//                 continue;
//             }

//             QJsonObject obj = doc.object();

//             // width_data 파싱
//             std::vector<int> parsedWidths;

//            // std::vector<int> parseAngles;

//             if (obj.contains("parsing width") && obj["parsing width"].isArray()) {
//                 QJsonArray arr = obj["parsing width"].toArray();
//                 parsedWidths.reserve(arr.size());
//                 for (const QJsonValue& v : arr) {
//                     parsedWidths.push_back(v.toInt());
//                 }
//             }

//             // if (obj.contains("angle") && obj["angle"].toInt()) {
//             //     int parsedAngle = obj["angle"].toInt();
//             //     m_coord->angles = parsedAngle;  // angle 값을 저장
//             //     qDebug() << "[TcpThread] 받은 angle 값:" << m_coord->angles;
//             // }


//             qDebug() << "[TcpThread] 받은 width_data:" << parsedWidths;
//             // if (obj.contains("parseAngles") && obj["parseAngles"].isArray()) {
//             //     QJsonArray arr = obj["parseAngles"].toArray();
//             //     parsedWidths.reserve(arr.size());
//             //     for (const QJsonValue& v : arr) {
//             //         parseAngles.push_back(v.toInt());
//             //     }
//             // }
//             //speaker_num 파싱
//            // int parsedSpeaker = obj.value("speaker").toInt();
//             // 크롭할 영역이 정상적인지 확인
//             // if (parsedWidths.size() == 4) {
//             //     int x = parsedWidths[0];
//             //     int y = parsedWidths[1];
//             //     int w = parsedWidths[2];
//             //     int h = parsedWidths[3];

//                 // // 영상 크기를 벗어나지 않는지 확인
//                 // if (x + w <= 3840 && y + h <= 720) {
//                 //     // 크롭 영역을 지정하여 크롭된 이미지를 얻기
//                 //     QRect cropRect(x, y, w, h);
//                 //     QPixmap originalPix;  // 원본 이미지 받아오기 (이 부분은 영상이 어디서 오느냐에 따라 달라짐)
//                 //     QPixmap croppedPix = originalPix.copy(cropRect);

//                 //     // 크롭된 이미지를 QLabel에 표시
//                 //     labels[index]->setPixmap(croppedPix.scaled(labels[index]->size(), Qt::KeepAspectRatio));
//                 // } else {
//                 //     qDebug() << "[TcpThread] 크롭 영역이 영상 크기를 벗어남";
//                 // }
//             }

//             //공유 구조체에 저장 (뮤텍스 잠금)
//             {
//                 QMutexLocker locker(&m_coord->mutex);
//                 QVector<int> qWidths;
//                 qWidths.reserve(parsedWidths.size());
//                 for (int w : parsedWidths) qWidths.append(w);
//                 // QVector<int> qAngles;
//                 // qAngles.reserve(parseAngles.size());
//                 // for (int angle : parseAngles) {
//                 //     qAngles.append(angle);
//                 // }
//                 if (obj.contains("angle") && obj["angle"].toInt()) {
//                     int parsedAngle = obj["angle"].toInt();
//                     m_coord->angles = parsedAngle;  // angle 값 저장

//                 }

//                 m_coord->width_data = qWidths;

//                 //m_coord->speaker_num = parsedSpeaker;
//                 //m_coord->angles = qAngles;
//             }
//             qDebug() << "[TcpThread] updated width_data:" << m_coord->width_data
//                      << ",angles값:" << m_coord->angles;
//         }

//         // 스레드 중단 요청 처리
//         if (QThread::currentThread()->isInterruptionRequested()) {
//             break;
//         }
//     }

//     socket.disconnectFromHost();
//     qDebug() << "[TcpThread] 종료";
// }
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
