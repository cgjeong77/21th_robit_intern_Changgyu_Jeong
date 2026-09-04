#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>
#include "과제2.hpp"

void Task2::input()
{
    std::cout << "점의 개수를 입력하세요: ";
    std::cin >> count;

    std::cout << "좌표의 최소값을 입력하세요: ";
    std::cin >> minRange;

    std::cout << "좌표의 최대값을 입력하세요: ";
    std::cin >> maxRange;

    points = new Point[count];
}

void Task2::generatePoints()
{
    std::srand((unsigned int)std::time(nullptr));

    for (int i = 0; i < count; i++)
    {
        points[i].x = minRange + std::rand() % (maxRange - minRange + 1);
        points[i].y = minRange + std::rand() % (maxRange - minRange + 1);

        std::cout << "Point " << i + 1
                  << ". x=" << points[i].x
                  << ", y=" << points[i].y
                  << std::endl;
    }
}

void Task2::calculateDistance()
{
    double dx = points[0].x - points[1].x;
    double dy = points[0].y - points[1].y;

    minDist = std::sqrt(dx * dx + dy * dy);
    maxDist = minDist;

    minPoint1 = 0;
    minPoint2 = 1;
    maxPoint1 = 0;
    maxPoint2 = 1;

    for (int i = 0; i < count; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            dx = points[i].x - points[j].x;
            dy = points[i].y - points[j].y;

            double dist = std::sqrt(dx * dx + dy * dy);

            if (dist < minDist)
            {
                minDist = dist;
                minPoint1 = i;
                minPoint2 = j;
            }

            if (dist > maxDist)
            {
                maxDist = dist;
                maxPoint1 = i;
                maxPoint2 = j;
            }
        }
    }
}

void Task2::printResult()
{
    std::cout << std::fixed << std::setprecision(5);

    std::cout << std::endl;
    std::cout << "MinDist = " << minDist << std::endl;

    std::cout << "Pair of Min Coor.(x,y): "
              << "P1(" << points[minPoint1].x << ","
              << points[minPoint1].y << ") & "
              << "P2(" << points[minPoint2].x << ","
              << points[minPoint2].y << ")"
              << std::endl;

    std::cout << std::endl;
    std::cout << "MaxDist = " << maxDist << std::endl;

    std::cout << "Pair of Max Coor.(x,y): "
              << "P1(" << points[maxPoint1].x << ","
              << points[maxPoint1].y << ") & "
              << "P2(" << points[maxPoint2].x << ","
              << points[maxPoint2].y << ")"
              << std::endl;
}

void Task2::release()
{
    delete[] points;
}

int main()
{
    Task2 task;

    task.input();
    task.generatePoints();
    task.calculateDistance();
    task.printResult();
    task.release();

    return 0;
}