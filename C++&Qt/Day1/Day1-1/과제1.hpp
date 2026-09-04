#ifndef TASK1_HPP
#define TASK1_HPP

class Task1
{
private:
    int* arr;

    int size, max, min, sum, i;
    double average;

public:
    void input();
    void calculate();
    void printResult();
    void release();
};

#endif