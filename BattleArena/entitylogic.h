
#pragma once

#include "raylib.h"
#include <string>


const int BOARD_SIZE = 10;
const int CELL_SIZE = 50;
const int MAX_HEALTH = 100;


enum class CellType { Empty, Health, Bomb };


struct Entity {
    int x;             
    int y;          
    int health;       
    Color color;       
    std::string name;  
};


struct Cell {
    CellType type;
};

class EntityLogic {
public:
    EntityLogic(Entity* player, Entity* enemy, Cell board[BOARD_SIZE][BOARD_SIZE]);

    int TryMove(Entity& entity, int newX, int newY);
    bool CanAttack(const Entity& attacker, const Entity& target) const;
    bool DoAttack(Entity& attacker, Entity& target, bool isPlayerTurn);

private:
    bool IsOccupied(int x, int y) const;
    bool IsWithinBoard(int x, int y) const;

    Entity* player;
    Entity* enemy;
    Cell(*board)[BOARD_SIZE];
};

