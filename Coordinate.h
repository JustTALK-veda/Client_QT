#ifndef COORDINATE_H
#define COORDINATE_H

#include <QMutex>
#include <QVector>

struct Coordinate {
    int x = 0;
    int y = 0;
    QVector<int> width_data; // server에서 받아오는 값
    int speaker_num = 0;//server에서 받아오는 값2
    QMutex mutex;
};

#endif // COORDINATE_H
