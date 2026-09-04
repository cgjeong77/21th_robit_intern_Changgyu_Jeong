#include <iostream>
#include <cstdlib>
#include "과제3.hpp"

Player::Player()
{
    HP = 50;
    MP = 7;
    x = 0;
    y = 0;
}

Player::Player(int x, int y)
{
    HP = 50;
    MP = 7;

    this->x = x;
    this->y = y;
}

Monster::Monster()
{
    HP = 50;
    x = 0;
    y = 0;
}

Monster::Monster(int x, int y, int HP)
{
    this->x = x;
    this->y = y;
    this->HP = HP;
}

void Player::X_move(int move)
{
    x = x + move;

    std::cout << "X Position " << move << " moved!" << std::endl;
}

void Player::Y_move(int move)
{
    y = y + move;

    std::cout << "Y Position " << move << " moved!" << std::endl;
}

void Player::Show_Status()
{
    std::cout << "HP:" << HP << std::endl;
    std::cout << "MP:" << MP << std::endl;
    std::cout << "Position:" << x << "," << y << std::endl;
}

int Monster::Get_X()
{
    return x;
}

int Monster::Get_Y()
{
    return y;
}

int Monster::Get_HP()
{
    return HP;
}

int Monster::Be_Attacked()
{
    HP = HP - 10;

    if (HP < 0)
    {
        HP = 0;
    }

    return HP;
}

void Player::Attack(Monster& target)
{
    if (MP <= 0)
    {
        std::cout << "MP 부족!" << std::endl;
        std::exit(0);
    }

    MP = MP - 1;

    if (x == target.Get_X() && y == target.Get_Y())
    {
        target.Be_Attacked();

        std::cout << "공격 성공!" << std::endl;
        std::cout << "남은 체력:" << target.Get_HP() << std::endl;

        if (target.Get_HP() == 0)
        {
            std::cout << "Monster Die!!" << std::endl;
            std::exit(0);
        }
    }
    else
    {
        std::cout << "공격실패!!" << std::endl;
    }
}

int main()
{
    Player player(0, 0);
    Monster monster(5, 4, 50);

    char command;

    while (true)
    {
        std::cout << "Type Command(U/D/R/L/A/S)" << std::endl;
        std::cin >> command;

        switch (command)
        {
        case 'U':
            player.Y_move(1);
            break;

        case 'D':
            player.Y_move(-1);
            break;

        case 'R':
            player.X_move(1);
            break;

        case 'L':
            player.X_move(-1);
            break;

        case 'A':
            player.Attack(monster);
            break;

        case 'S':
            player.Show_Status();
            break;
        }
    }

    return 0;
}