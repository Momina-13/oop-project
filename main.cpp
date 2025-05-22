
---

## 💻 `main.cpp` (Full Code)

```cpp
#include <iostream>
#include <cmath>
using namespace std;

// ----------- Character Base Class --------------
class Character {
protected:
    int health;
public:
    Character(int h) : health(h) {}
    virtual void takeDamage(int dmg) {
        health -= dmg;
        if (health < 0) health = 0;
    }
    int getHealth() const { return health; }
    bool isAlive() const { return health > 0; }
};

// ----------- Player Class --------------
class Player : public Character {
public:
    Player() : Character(100) {}
};

// ----------- Enemy Class --------------
class Enemy : public Character {
public:
    Enemy(int levelHealth) : Character(levelHealth) {}
};

// ----------- Game Controller Class --------------
class Game {
private:
    Player player;
    Enemy* enemy;
    int level;
    const double HIT_RADIUS = 5.0;

    bool isHit(double x, double y) {
        double distance = sqrt(x * x + y * y);
        return distance <= HIT_RADIUS;
    }

    void nextLevel() {
        level++;
        int enemyHealth = (level == 1) ? 100 : (level == 2) ? 200 : 400;
        delete enemy;
        enemy = new Enemy(enemyHealth);
        cout << "\n🎯 Level " << level << " Begins! Enemy HP: " << enemyHealth << "\n\n";
    }

public:
    Game() : level(0), enemy(nullptr) {
        nextLevel();
    }

    ~Game() {
        delete enemy;
    }

    void play() {
        while (player.isAlive()) {
            cout << "Player HP: " << player.getHealth()
                 << " | Enemy HP: " << enemy->getHealth() << "\n";
            cout << "Enter arrow coordinates (x y): ";
            double x, y;
            cin >> x >> y;

            if (isHit(x, y)) {
                enemy->takeDamage(20);
                cout << "🎯 Hit! Enemy took 20 damage.\n";
            } else {
                player.takeDamage(15);
                cout << "❌ Miss! You took 15 damage.\n";
            }

            if (!enemy->isAlive()) {
                if (level == 3) {
                    cout << "\n🏆 Congratulations! You defeated all enemies!\n";
                    break;
                }
                nextLevel();
            }

            if (!player.isAlive()) {
                cout << "\n💀 Game Over! You were defeated.\n";
                break;
            }
        }
    }
};

// ----------- Main Function --------------
int main() {
    cout << "=== 🏹 Battle Arena: Arrow Clash ===\n";
    Game game;
    game.play();
    return 0;
}
