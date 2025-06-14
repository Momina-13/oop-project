

#pragma once

#include "raylib.h"
#include "entitylogic.h"

const int MAX_NOTIFICATIONS = 20;

enum class GameState {
    MainMenu,
    Versus,
    Playing,
    GameOver,
    LevelTransition
};

struct HealthChangeNotification {
    const char* text;
    int x, y;
    Color color;
    float timer;
};

class BattleArenaGame {
public:
    BattleArenaGame();
    ~BattleArenaGame();

    void Update();
    void Draw();

private:
    void DrawMainMenu();
    void DrawVersus();
    void DrawBoard();
    void DrawEntities();
    void DrawUI();
    void DrawGameOver();
    void DrawLevelTransition();
    void DrawHealthBars();

    void HandleInput();
    void PlaceRandom(CellType type, int count);
    void Reset();
    void LoadLevel(int level);

    Cell board[BOARD_SIZE][BOARD_SIZE];
    Entity player, enemy;

    GameState state;
    bool playerTurn;
    bool attackPending;
    int currentLevel;

    char inputBuffer[8];
    int inputLength;

    const char* gameOverText;
    EntityLogic* entityLogic;

    HealthChangeNotification notifications[MAX_NOTIFICATIONS];
    int notificationCount;

    Texture2D background;
    Texture2D fightButton;
    Texture2D versusImage;
    Texture2D player1Texture;
    Texture2D player2Texture;
    Texture2D healthTexture;
    Texture2D bombTexture;

    int boardOffsetX;
    int boardOffsetY;

    // ? Correctly placed inside the class:
    bool levelTransitionHandled;
    const char* winnerText;
};
