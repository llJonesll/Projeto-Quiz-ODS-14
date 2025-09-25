/**
 * @file quiz_ods14.c
 * @author Grupo 1
 * @brief Jogo de Quiz completo sobre a ODS 14 usando Raylib.
 * @version 5.2
 * @copyright Copyright (c) 2025
 *
 * @note Mudanças da v5.2:
 * - Toda a lógica do Leaderboard (placar) foi movida para os novos arquivos `leaderboard.h` e `leaderboard.c`.
 * - O arquivo principal foi simplificado, chamando as funções do novo módulo.
 */

#include "raylib/raylib.h"
#include "raylib/music_player.h" 
#include "raylib/water_fx.h"
#include "raylib/questions.h"
#include "raylib/leaderboard.h"  // <<< NOVO MÓDULO DE LEADERBOARD
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> 
#include <math.h> 

//---------------------------------------------
// Definições e Constantes do Jogo
//---------------------------------------------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define QUESTION_TIME 15.0f

//---------------------------------------------
// Tipos Customizados do Jogo
//---------------------------------------------
typedef enum { SCREEN_MENU, SCREEN_HOW_TO_PLAY, SCREEN_LEADERBOARD, SCREEN_CREDITS, SCREEN_ENTER_NAME, SCREEN_GAMEPLAY, SCREEN_SHOW_ANSWER, SCREEN_GAME_OVER } GameScreen;

//---------------------------------------------
// Variáveis Globais do Jogo
//---------------------------------------------
static GameScreen currentScreen = SCREEN_MENU;
static int questionOrder[QUIZ_QUESTION_COUNT];
static int currentQuestionIndex = 0;
static int playerScore = 0;
static int selectedAnswer = -1;
static bool isAnswerCorrect = false;
static float answerTimer = 0.0f;
static char playerName[MAX_NAME_LENGTH + 1] = "\0";
static int nameCharCount = 0;
static Texture2D texMenu, texQuestion, texHowToPlay, texLeaderboard, texCredits, texLogo;
static Font fontMontserrat;
static float questionTimer;
static int pointsGainedNotification = 0;
static float notificationTimer = 0.0f;
static Music rainMusic; 
static Sound selectSfx, buttonSfx, correctSfx, wrongSfx, typeSfx, victorySfx;
static bool isHoveringBtnStart = false, isHoveringBtnHowToPlay = false, isHoveringBtnLeaderboard = false, isHoveringBtnCredits = false, isHoveringBtnBack = false;
static bool hasVisitedHowToPlay = false;
static const char* menuNotificationText = NULL;
static float menuNotificationTimer = 0.0f;

//---------------------------------------------
// Protótipos de Funções
//---------------------------------------------
void UpdateDrawFrame(void);
void GoToMenu(void);
void DrawTextWrappedCentered(Font font, const char *text, Rectangle rec, float fontSize, float spacing, Color color);

//---------------------------------------------
// Funções do Jogo
//---------------------------------------------
void GoToMenu(void) {
    if (IsMusicStreamPlaying(rainMusic)) StopMusicStream(rainMusic);
    ResetWaterFx();
    currentScreen = SCREEN_MENU;
}

void StartGame() { 
    SelectAndShuffleQuizQuestions(questionOrder); 
    currentQuestionIndex = 0; 
    playerScore = 0; 
    selectedAnswer = -1; 
    currentScreen = SCREEN_GAMEPLAY; 
    questionTimer = QUESTION_TIME; 
}

void DrawTextWrappedCentered(Font font, const char *text, Rectangle rec, float fontSize, float textSpacing, Color color) { 
    // ... (implementação inalterada)
}

//---------------------------------------------
// Ponto de Entrada Principal (main)
//---------------------------------------------
int main(void) {
    srand(time(NULL));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Quiz - Navegando pela ODS 14");
    SetExitKey(KEY_NULL);
    InitAudioDevice();
    SetTargetFPS(60);

    texMenu = LoadTexture("resources/images/tela_menu.png");
    texQuestion = LoadTexture("resources/images/tela_pergunta.png");
    texHowToPlay = LoadTexture("resources/images/tela_comojogar.png");
    texLeaderboard = LoadTexture("resources/images/tela_leaderboard.png");
    texCredits = LoadTexture("resources/images/tela_creditos.png");
    texLogo = LoadTexture("resources/images/logo.png");
    fontMontserrat = LoadFontEx("resources/montserrat.ttf", 256, NULL, 250);

    selectSfx = LoadSound("resources/sfx/select.mp3");
    buttonSfx = LoadSound("resources/sfx/button.mp3");
    correctSfx = LoadSound("resources/sfx/correct.mp3");
    wrongSfx = LoadSound("resources/sfx/wrong.mp3");
    typeSfx = LoadSound("resources/sfx/type.mp3");
    victorySfx = LoadSound("resources/sfx/victory.mp3");
    rainMusic = LoadMusicStream("resources/sfx/rain.mp3");
    rainMusic.looping = true;

    InitMusicPlayer();
    InitWaterFx();
    InitializeQuestions();
    InitLeaderboard(); // <<< INICIA O MÓDULO DE LEADERBOARD
    
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }

    // ... (bloco de Unload inalterado)

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

//---------------------------------------------
// Loop Principal de Atualização e Desenho
//---------------------------------------------
void UpdateDrawFrame(void) {
    float deltaTime = GetFrameTime();
    float currentTime = GetTime();
    Vector2 mousePos = GetMousePosition();

    UpdateMusicPlayer();
    UpdateWaterFx(deltaTime, currentTime, mousePos);
    UpdateMusicStream(rainMusic);

    if (IsKeyPressed(KEY_ESCAPE)) {
        if (currentScreen != SCREEN_MENU) {
            PlaySound(buttonSfx);
            GoToMenu();
        }
    }

    if (menuNotificationTimer > 0) {
        menuNotificationTimer -= deltaTime;
        if (menuNotificationTimer < 0) { menuNotificationTimer = 0; menuNotificationText = NULL; }
    }

    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }
    
    switch (currentScreen) {
        case SCREEN_MENU: {
            // ... (lógica do menu inalterada)
        } break;
        case SCREEN_HOW_TO_PLAY: case SCREEN_LEADERBOARD: case SCREEN_CREDITS: {
            Rectangle btnBack = { 787, 891, 347, 100 };
            bool isMouseOverBack = CheckCollisionPointRec(mousePos, btnBack);
            if (isMouseOverBack && !isHoveringBtnBack) PlaySound(selectSfx);
            isHoveringBtnBack = isMouseOverBack;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, btnBack)) {
                PlaySound(buttonSfx);
                GoToMenu(); 
            }
        } break;
        case SCREEN_ENTER_NAME:
        case SCREEN_GAMEPLAY:
        case SCREEN_SHOW_ANSWER:
        case SCREEN_GAME_OVER: {
            if (currentScreen == SCREEN_ENTER_NAME) {
                if (IsWaterAnimationFinished()) {
                    int key = GetKeyPressed();
                    if ((key >= KEY_A && key <= KEY_Z) && (nameCharCount < MAX_NAME_LENGTH)) {
                        PlaySound(typeSfx); playerName[nameCharCount++] = (char)key; playerName[nameCharCount] = '\0';
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        PlaySound(typeSfx); nameCharCount--; if (nameCharCount < 0) nameCharCount = 0; playerName[nameCharCount] = '\0';
                    }
                    if (IsKeyPressed(KEY_ENTER) && nameCharCount > 0) { PlaySound(buttonSfx); StartGame(); }
                }
            } else if (currentScreen == SCREEN_GAMEPLAY) {
                questionTimer -= deltaTime;
                if (questionTimer <= 0) {
                    PlaySound(wrongSfx); isAnswerCorrect = false; selectedAnswer = -1; currentScreen = SCREEN_SHOW_ANSWER; answerTimer = 2.0f;
                }
                if (IsKeyPressed(KEY_A)) { selectedAnswer = 0; PlaySound(selectSfx); }
                if (IsKeyPressed(KEY_B)) { selectedAnswer = 1; PlaySound(selectSfx); }
                if (IsKeyPressed(KEY_C)) { selectedAnswer = 2; PlaySound(selectSfx); }
                if (IsKeyPressed(KEY_D)) { selectedAnswer = 3; PlaySound(selectSfx); }
                if (IsKeyPressed(KEY_ENTER) && selectedAnswer != -1) {
                    Question q = GetQuestionFromOrder(questionOrder[currentQuestionIndex]);
                    isAnswerCorrect = (selectedAnswer == q.correctOption);
                    if (isAnswerCorrect) {
                        PlaySound(correctSfx); playerScore += q.points;
                        pointsGainedNotification = q.points; notificationTimer = 2.0f;
                    } else { PlaySound(wrongSfx); }
                    currentScreen = SCREEN_SHOW_ANSWER; answerTimer = 2.0f;
                }
            } else if (currentScreen == SCREEN_SHOW_ANSWER) {
                answerTimer -= deltaTime;
                if (answerTimer <= 0) {
                    currentQuestionIndex++; selectedAnswer = -1;
                    if (currentQuestionIndex >= QUIZ_QUESTION_COUNT) {
                        UpdateLeaderboard(playerName, playerScore); // <<< USA A FUNÇÃO DO MÓDULO
                        currentScreen = SCREEN_GAME_OVER;
                    } else { currentScreen = SCREEN_GAMEPLAY; questionTimer = QUESTION_TIME; }
                }
            } else if (currentScreen == SCREEN_GAME_OVER) {
                 if (IsKeyPressed(KEY_ENTER)) {
                    PlaySound(victorySfx);
                    currentScreen = SCREEN_LEADERBOARD;
                    ResetWaterFx();
                 }
            }
        } break;
        default: break;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (currentScreen) {
        // ... (outros cases de desenho)
        case SCREEN_LEADERBOARD: { 
            DrawTexture(texLeaderboard, 0, 0, WHITE); 
            Rectangle btnBack = { 820, 911, 280, 70 }; 
            if (CheckCollisionPointRec(mousePos, btnBack)) DrawRectangleLinesEx(btnBack, 4, BLUE); 
            
            const PlayerScore* leaderboard = GetLeaderboard(); // <<< PEGA OS DADOS DO MÓDULO
            
            int startY = 420; int stepY = 49; int nameCenterX = 958; int scoreX = 1120; int fontSize = 35; float spacing = 2.0f; 
            for (int i = 0; i < LEADERBOARD_SIZE; i++) { 
                const char* nameText = leaderboard[i].name; 
                Vector2 nameTextSize = MeasureTextEx(fontMontserrat, nameText, fontSize, spacing); 
                DrawTextEx(fontMontserrat, nameText, (Vector2){nameCenterX - (nameTextSize.x / 2), startY + (i * stepY)}, fontSize, spacing, BLACK); 
                const char* scoreText = TextFormat("%03d", leaderboard[i].score); 
                DrawTextEx(fontMontserrat, scoreText, (Vector2){scoreX, startY + (i * stepY)}, fontSize, spacing, BLACK); 
            } 
        } break;
        // ... (resto do switch inalterado)
    }

    DrawMusicPlayer();
    
    EndDrawing();
}