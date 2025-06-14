
#include "entitylogic.h"
#include "raylib.h"

EntityLogic::EntityLogic(Entity* p, Entity* e, Cell b[BOARD_SIZE][BOARD_SIZE]) {
    player = p;
    enemy = e;
    board = b;
}

bool EntityLogic::IsOccupied(int x, int y) const {
    return (player->x == x && player->y == y) || (enemy->x == x && enemy->y == y);
}

bool EntityLogic::IsWithinBoard(int x, int y) const {
    return (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE);
}


int EntityLogic::TryMove(Entity& entity, int newX, int newY) {
    if (!IsWithinBoard(newX, newY) || IsOccupied(newX, newY)) {
        return 0; 
    }

    entity.x = newX;
    entity.y = newY;

    Cell& cell = board[newY][newX];

    if (cell.type == CellType::Health) {
        entity.health += 10;
        if (entity.health > MAX_HEALTH) {
            entity.health = MAX_HEALTH;
        }
        cell.type = CellType::Empty;
        return 1; 
    }
    else if (cell.type == CellType::Bomb) {
        if (&entity == player) {
            enemy->health -= 10;
            if (enemy->health < 0) enemy->health = 0;
        }
        else {
            player->health -= 10;
            if (player->health < 0) player->health = 0;
        }
        cell.type = CellType::Empty;
        return 2; 
    }

    return -1; 
}

bool EntityLogic::CanAttack(const Entity& attacker, const Entity& target) const {
    int dx = attacker.x - target.x;
    if (dx < 0) dx = -dx;

    int dy = attacker.y - target.y;
    if (dy < 0) dy = -dy;

    int distance = dx + dy;
    return (distance <= 3);
}


bool EntityLogic::DoAttack(Entity& attacker, Entity& target, bool isPlayerTurn) {
    if (!CanAttack(attacker, target)) return false;

    if (isPlayerTurn && IsKeyPressed(KEY_SPACE)) {
        target.health -= 20;
        if (target.health < 0) target.health = 0;
        return true;
    }
    else if (!isPlayerTurn && IsKeyPressed(KEY_F)) {
        target.health -= 20;
        if (target.health < 0) target.health = 0;
        return true;
    }

    return false;
}
