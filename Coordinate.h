#ifndef COORDINATE_H
#define COORDINATE_H

#include <QMutex>
#include <QVector>

struct Coordinate {

    QVector<int> width_data; // server에서 받아오는 값
    QVector<int> angle_data;
    QMutex mutex;
};

#endif // COORDINATE_H
