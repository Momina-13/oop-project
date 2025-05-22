# 🏹 Battle Arena: Arrow Clash

A terminal-based, turn-based game written in C++. You are a lone fighter armed with arrows, battling through three increasingly difficult levels.

## 🎮 Game Objective

Defeat enemies using accurate arrow shots. Each level increases the enemy's health. Land your arrows accurately — a hit reduces the enemy's HP, but a miss will cost you your own!

- Level 1 Enemy HP: 100
- Level 2 Enemy HP: 200
- Level 3 Enemy HP: 400

**Player starts with 100 HP**. If it reaches 0, it's game over.

## 🕹️ How to Play

- Enemy is always at position (0, 0)
- You input (x, y) as coordinates for your arrow shot
- If the arrow lands within 5 units from the enemy, it hits (enemy loses 20 HP)
- If it misses, you take 15 damage

## 💻 How to Run

```bash
g++ main.cpp -o battle_arena
./battle_arena
