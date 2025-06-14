
#include "game.h"
#include <cstdlib>
#include <ctime>
#include <cstring>

BattleArenaGame::BattleArenaGame() {
    std::srand((unsigned)std::time(nullptr));
    state = GameState::MainMenu;
    currentLevel = 1;
    inputLength = 0;
    inputBuffer[0] = '\0';
    gameOverText = "";
    playerTurn = true;
    attackPending = false;
    notificationCount = 0;
    entityLogic = nullptr;

    background = LoadTexture("boardbackground.jpg");
    fightButton = LoadTexture("fight.jpg");
    versusImage = LoadTexture("player1vs2.jpg");
    player1Texture = LoadTexture("player1.png");
    player2Texture = LoadTexture("player2.png");
    healthTexture = LoadTexture("health.png");
    bombTexture = LoadTexture("bomb.png");

    boardOffsetX = (GetScreenWidth() - (BOARD_SIZE * CELL_SIZE)) / 2;
    boardOffsetY = 0; 
    levelTransitionHandled = false;
    winnerText = "";
    Reset();
}

BattleArenaGame::~BattleArenaGame() {
    if (entityLogic) delete entityLogic;

    UnloadTexture(background);
    UnloadTexture(fightButton);
    UnloadTexture(versusImage);
    UnloadTexture(player1Texture);
    UnloadTexture(player2Texture);
    UnloadTexture(healthTexture);
    UnloadTexture(bombTexture);
}

void BattleArenaGame::Reset() {
    LoadLevel(currentLevel);

}


void BattleArenaGame::LoadLevel(int level) {
    for (int y = 0; y < BOARD_SIZE; y++)
        for (int x = 0; x < BOARD_SIZE; x++)
            board[y][x].type = CellType::Empty;

    player = { 0, 0, MAX_HEALTH, RED, "Player 1" };
    enemy = { 5, 5, MAX_HEALTH, GREEN, "Player 2" };

    if (entityLogic) delete entityLogic;
    entityLogic = new EntityLogic(&player, &enemy, board);

    if (level == 1) {
        PlaceRandom(CellType::Health, 5);
        PlaceRandom(CellType::Bomb, 5);
    }
    else if (level == 2) {
        PlaceRandom(CellType::Health, 2);
        PlaceRandom(CellType::Bomb, 8);
    }

    playerTurn = true;
    attackPending = false;
    inputLength = 0;
    inputBuffer[0] = '\0';
    notificationCount = 0;

    levelTransitionHandled = false; 
    winnerText = "";
}


void BattleArenaGame::PlaceRandom(CellType type, int count) {
    int placed = 0;
    while (placed < count) {
        int x = std::rand() % BOARD_SIZE;
        int y = std::rand() % BOARD_SIZE;
        if ((x == player.x && y == player.y) || (x == enemy.x && y == enemy.y)) continue;
        if (board[y][x].type == CellType::Empty) {
            board[y][x].type = type;
            placed++;
        }
    }
}

void BattleArenaGame::Update() {
    if (state == GameState::MainMenu) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            int buttonX = (GetScreenWidth() - fightButton.width) / 2;
            int buttonY = GetScreenHeight() / 2;
            Rectangle btn = { (float)buttonX, (float)buttonY, (float)fightButton.width, (float)fightButton.height };
            if (CheckCollisionPointRec(mouse, btn)) {
                state = GameState::Versus;
            }
        }
    }

    else if (state == GameState::Versus) {
        if (IsKeyPressed(KEY_ENTER)) {
            state = GameState::Playing;
        }
    }

    else if (state == GameState::Playing) {
        HandleInput();

        if (attackPending) {
            Entity& attacker = playerTurn ? player : enemy;
            Entity& target = playerTurn ? enemy : player;

            if (entityLogic->DoAttack(attacker, target, playerTurn)) {
                if (notificationCount < MAX_NOTIFICATIONS) {
                    notifications[notificationCount++] = { "-20", target.x, target.y, WHITE, 1.0f };
                }
                attackPending = false;
                playerTurn = !playerTurn;
            }
        }

        // Clear old notifications
        int j = 0;
        for (int i = 0; i < notificationCount; i++) {
            notifications[i].timer -= GetFrameTime();
            if (notifications[i].timer > 0) {
                notifications[j++] = notifications[i];
            }
        }
        notificationCount = j;

        
        if (!levelTransitionHandled && (player.health <= 0 || enemy.health <= 0)) {
            levelTransitionHandled = true;

            if (currentLevel == 1) {
                winnerText = (player.health <= 0) ? "Player 2 won Level 1!" : "Player 1 won Level 1!";
                currentLevel = 2;
                state = GameState::LevelTransition;
            }
            else if (currentLevel == 2) {
                gameOverText = (player.health <= 0) ? "Player 2 Wins!" : "Player 1 Wins!";
                state = GameState::GameOver;
            }
        }
    }

    else if (state == GameState::LevelTransition) {
        if (IsKeyPressed(KEY_ENTER)) {
            LoadLevel(currentLevel);
            state = GameState::Playing;
        }
    }

    else if (state == GameState::GameOver) {
        if (IsKeyPressed(KEY_ENTER)) {
            currentLevel = 1;
            Reset();
            state = GameState::MainMenu;
        }
    }
}




void BattleArenaGame::HandleInput() {
    if (attackPending) return;

    int key = GetCharPressed();
    while (key > 0) {
        if (((key >= '0' && key <= '9') || key == ',') && inputLength < 3) {
            inputBuffer[inputLength++] = (char)key;
            inputBuffer[inputLength] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && inputLength > 0) {
        inputBuffer[--inputLength] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER) && inputLength == 3 && inputBuffer[1] == ',') {
        int x = inputBuffer[0] - '0';
        int y = inputBuffer[2] - '0';

        Entity& mover = playerTurn ? player : enemy;
        Entity& other = playerTurn ? enemy : player;

        int result = entityLogic->TryMove(mover, x, y);

        if (result == 1 && notificationCount < MAX_NOTIFICATIONS)
            notifications[notificationCount++] = { "+10", mover.x, mover.y, WHITE, 1.0f };
        else if (result == 2 && notificationCount < MAX_NOTIFICATIONS - 1) {
            notifications[notificationCount++] = { "-10", mover.x, mover.y, WHITE, 1.0f };
            notifications[notificationCount++] = { "-10", other.x, other.y, WHITE, 1.0f };
        }

        if (result != 0) {
            if (entityLogic->CanAttack(mover, other))
                attackPending = true;
            else
                playerTurn = !playerTurn;
        }

        inputLength = 0;
        inputBuffer[0] = '\0';
    }
}

void BattleArenaGame::Draw() {
    ClearBackground(RAYWHITE);
    DrawTextureEx(background, { 0, 0 }, 0.0f,
        (float)GetScreenWidth() / background.width, WHITE);

    if (state == GameState::MainMenu) DrawMainMenu();
    else if (state == GameState::Versus) DrawVersus();
    else if (state == GameState::Playing) {
        DrawBoard();
        DrawEntities();
        DrawUI();
        DrawHealthBars();
    }
    else if (state == GameState::LevelTransition) DrawLevelTransition();
    else if (state == GameState::GameOver) DrawGameOver();
}

void BattleArenaGame::DrawMainMenu() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Center the "BATTLE ARENA" text
    const char* title = "BATTLE ARENA";
    int titleFontSize = 40;
    int titleWidth = MeasureText(title, titleFontSize);
    int titleX = (screenWidth - titleWidth) / 2;
    int titleY = screenHeight / 4; // quarter way down

    DrawText(title, titleX, titleY, titleFontSize, BLACK);

    // Center the fight button
    int buttonX = (screenWidth - fightButton.width) / 2;
    int buttonY = screenHeight / 2; // half way down

    DrawTexture(fightButton, buttonX, buttonY, WHITE);
}


void BattleArenaGame::DrawVersus() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
   
    int imageX = (screenWidth - versusImage.width) / 2;
    int imageY = (screenHeight - versusImage.height) / 2;
    DrawTexture(versusImage, imageX, imageY, WHITE);
  
    const char* startText = "PRESS ENTER TO START";
    int textFontSize = 30;
    int textWidth = MeasureText(startText, textFontSize);
    int textX = (screenWidth - textWidth) / 2;
    int textY = imageY + versusImage.height + 20;
    DrawText(startText, textX, textY, textFontSize, BLACK);
}

void BattleArenaGame::DrawLevelTransition() {
    if (currentLevel == 2) {
        DrawText("LEVEL 1 COMPLETE!", 200, 180, 40, BLACK);
        DrawText("LEVEL 2 CONTINUE", 200, 230, 30, DARKGRAY);
    }
    else {
        DrawText("LEVEL COMPLETE!", 200, 200, 40, BLACK);
    }

    DrawText("PRESS ENTER TO CONTINUE", 150, 300, 30, BLACK);
}



void BattleArenaGame::DrawBoard() {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            int drawX = boardOffsetX + x * CELL_SIZE;
            int drawY = boardOffsetY + y * CELL_SIZE;

            DrawRectangle(drawX, drawY, CELL_SIZE, CELL_SIZE, BLACK);

            Vector2 pos = { (float)(drawX + 5), (float)(drawY + 5) };
            float scale = (CELL_SIZE - 10) / (float)healthTexture.width;

            if (board[y][x].type == CellType::Health)
                DrawTextureEx(healthTexture, pos, 0, scale, WHITE);
            else if (board[y][x].type == CellType::Bomb)
                DrawTextureEx(bombTexture, pos, 0, scale, WHITE);

            DrawRectangleLines(drawX, drawY, CELL_SIZE, CELL_SIZE, WHITE);
        }
    }
}

void BattleArenaGame::DrawEntities() {
    float scale1 = (CELL_SIZE - 20) / (float)player1Texture.width;
    float scale2 = (CELL_SIZE) / (float)player1Texture.width;


    Vector2 pos1 = { (float)(boardOffsetX + player.x * CELL_SIZE + 5), (float)(player.y * CELL_SIZE + 5) };
    Vector2 pos2 = { (float)(boardOffsetX + enemy.x * CELL_SIZE + 5), (float)(enemy.y * CELL_SIZE + 5) };

    DrawTextureEx(player1Texture, pos1, 0, scale1, WHITE);
    DrawTextureEx(player2Texture, pos2, 0, scale2, WHITE);

    for (int i = 0; i < notificationCount; i++) {
        float drawX = (float)(boardOffsetX + notifications[i].x * CELL_SIZE + CELL_SIZE / 2);
        float drawY = (float)(notifications[i].y * CELL_SIZE);
        DrawText(notifications[i].text, (int)drawX, (int)(drawY - (1.0f - notifications[i].timer) * 30), 20, notifications[i].color);
    }
}

void BattleArenaGame::DrawUI() {
    int baseY = BOARD_SIZE * CELL_SIZE + 5;
   
    const char* turnText = playerTurn ? "PLAYER 1 TURN" : "PLAYER 2 TURN";
    int textWidth = MeasureText(turnText, 24);
    int screenWidth = GetScreenWidth();
    int centerX = (screenWidth - textWidth) / 2;




    int boxX = 20, boxY = BOARD_SIZE * CELL_SIZE + 70, boxW = 120, boxH = 32;

    if (!attackPending) {
        DrawText("ENTER X,Y", boxX + 10, boxY + 65, 20, BLACK);
        DrawRectangleLines(boxX + 10, boxY + 40, boxW, boxH - 10, WHITE);
        DrawText(inputBuffer, boxX + 10, boxY + 40, 20, BLACK);

        if (inputLength == 3 &&
            (inputBuffer[1] != ',' || inputBuffer[0] < '0' || inputBuffer[0] > '9' || inputBuffer[2] < '0' || inputBuffer[2] > '9')) {
            DrawText("INVALID COORDINATES! USE X,Y", boxX + 140, boxY + 6, 20, WHITE);
        }
    }

    DrawText("PRESS SPACE TO ATTACK (PLAYER) F TO ATTACK (ENEMY)", 10, BOARD_SIZE * CELL_SIZE + 170, 18, WHITE);

    if (attackPending)
        DrawText("YOU ARE IN RANGE ATTACK!!!", 10, BOARD_SIZE * CELL_SIZE + 130, 24, WHITE);
    else if (entityLogic->CanAttack(playerTurn ? player : enemy, playerTurn ? enemy : player))
        DrawText("ATTACK POSSIBLE AFTER MOVE!", 10, BOARD_SIZE * CELL_SIZE + 20, 20, BLACK);
}

void BattleArenaGame::DrawHealthBars() {
    const char* turnText = playerTurn ? "PLAYER 1 TURN" : "PLAYER 2 TURN";
    int textFontSize = 30;
    int screenWidth = GetScreenWidth();

    int turnTextWidth = MeasureText(turnText, textFontSize);
    int turnTextX = (screenWidth - turnTextWidth) / 2;

    int baseY = BOARD_SIZE * CELL_SIZE + 40; 

    DrawText(turnText, turnTextX, baseY, textFontSize, BLACK);

    int verticalGap = 20;  
    int barTopY = baseY + textFontSize + verticalGap;

 
    int barWidth = 200;
    int barHeight = 20;
    int spacing = 60;


    int totalWidth = (barWidth * 2) + spacing;
    int barStartX = (screenWidth - totalWidth) / 2;

    
    DrawText("PLAYER 1", barStartX, barTopY - 20, 18, BLACK);
    DrawRectangle(barStartX, barTopY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barStartX, barTopY, (int)(barWidth * (player.health / (float)MAX_HEALTH)), barHeight, RED);


    int p2X = barStartX + barWidth + spacing;
    DrawText("PLAYER 2", p2X, barTopY - 20, 18, BLACK);
    DrawRectangle(p2X, barTopY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(p2X, barTopY, (int)(barWidth * (enemy.health / (float)MAX_HEALTH)), barHeight, BLUE);
}


void BattleArenaGame::DrawGameOver() {
    DrawText(gameOverText, 180, 200, 40, MAROON);
    DrawText("PRESS ENTER TO RETURN TO MENU", 120, 300, 30, DARKGRAY);
}
