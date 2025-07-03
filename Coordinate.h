#ifndef COORDINATE_H
#define COORDINATE_H

#include <QMutex>

struct Coordinate {
    int x = 0;
    int y = 0;
    QMutex mutex;
};

#endif // COORDINATE_H
