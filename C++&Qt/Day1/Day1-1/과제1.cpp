#include <iostream>
#include <iomanip>
#include "과제1.hpp"

void Task1::input()
{
    std::cout << "몇 개의 원소를 할당하겠습니까? : ";
    std::cin >> size;

    arr = new int[size];

    for (i = 0; i < size; i++)
    {
        std::cout << "정수형 데이터 입력: ";
        std::cin >> arr[i];
    }
}

void Task1::calculate()
{
    this->max = arr[0];
    this->min = arr[0];
    this->sum = 0;

    for (i = 0; i < size; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }

        if (arr[i] < min)
        {
            min = arr[i];
        }

        sum = sum + arr[i];
    }

    this->average = (double)sum / size;
}

void Task1::printResult()
{
    std::cout << "최대값: " << max << std::endl;
    std::cout << "최소값: " << min << std::endl;
    std::cout << "전체합: " << sum << std::endl;
    std::cout << "평균: "
              << std::fixed << std::setprecision(6)
              << average << std::endl;
}

void Task1::release()
{
    delete[] arr;
}

int main()
{
    Task1 task;

    task.input();
    task.calculate();
    task.printResult();
    task.release();

    return 0;
}