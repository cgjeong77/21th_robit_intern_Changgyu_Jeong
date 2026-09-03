#ifndef TASK2_HPP
#define TASK2_HPP

struct Point
{
    int x, y;
};

class Task2
{
private:
    Point* points;

    int count, minRange, maxRange;
    double minDist, maxDist;

    int minPoint1, minPoint2;
    int maxPoint1, maxPoint2;

public:
    void input();
    void generatePoints();
    void calculateDistance();
    void printResult();
    void release();
};

#endif