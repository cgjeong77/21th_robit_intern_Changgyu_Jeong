#ifndef TASK3_HPP
#define TASK3_HPP

class Monster;

class Player
{
private:
    int HP, MP, x, y;

public:
    Player();
    Player(int x, int y);

    void Attack(Monster& target);
    void Show_Status();
    void X_move(int move);
    void Y_move(int move);
};

class Monster
{
private:
    int HP, x, y;

public:
    Monster();
    Monster(int x, int y, int HP);

    int Be_Attacked();

    int Get_X();
    int Get_Y();
    int Get_HP();
};

#endif