#ifndef COORDINATE_H
#define COORDINATE_H

#include <QMutex>
#include <QVector>

struct Coordinate {

    QVector<int> width_data; // server에서 받아오는 값
    int angles; // 마이크 각도 값
    QMutex mutex;
};

#endif // COORDINATE_H
