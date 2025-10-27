/**
 * @file quiz_ods14.c
 * @author Grupo 1
 * @brief Jogo de Quiz completo sobre a ODS 14 usando Raylib.
 * @version 5.5.1
 * @copyright Copyright (c) 2025
 *
 * @note Mudanças da v5.5.1 (Correção):
 * - Restaurado NOVAMENTE o bloco de código que desenha a tela de Menu Principal.
 */

#include "raylib/raylib.h"
#include "raylib/music_player.h"
#include "raylib/water_fx.h"
#include "raylib/questions.h"
#include "raylib/leaderboard.h"
#include "raylib/scoring.h"
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
// QUESTION_TIME agora está em scoring.h

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
// static int playerScore = 0; // Gerenciado por scoring.c
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
static bool isHoveringBtnStart = false, isHoveringBtnHowToPlay = false, isHoveringBtnLeaderboard = false, isHoveringBtnCredits = false, isHoveringBtnBack = false, isHoveringBtnExit = false;
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
    ResetPlayerScore();
    selectedAnswer = -1;
    currentScreen = SCREEN_GAMEPLAY;
    questionTimer = QUESTION_TIME;
}

void DrawTextWrappedCentered(Font font, const char *text, Rectangle rec, float fontSize, float textSpacing, Color color) {
    char textToProcess[1024]; strcpy(textToProcess, text); char lines[20][256]; int lineCount = 0; char *textCopy = (char*)MemAlloc(strlen(textToProcess) + 1); strcpy(textCopy, textToProcess); char *word = strtok(textCopy, " "); char currentLine[256] = { 0 }; while (word != NULL) { char testLine[256]; strcpy(testLine, currentLine); if (strlen(currentLine) > 0) strcat(testLine, " "); strcat(testLine, word); if (MeasureTextEx(font, testLine, fontSize, textSpacing).x > rec.width) { strcpy(lines[lineCount], currentLine); lineCount++; strcpy(currentLine, word); } else { strcpy(currentLine, testLine); } word = strtok(NULL, " "); } strcpy(lines[lineCount], currentLine); lineCount++; MemFree(textCopy); float totalTextHeight = lineCount * fontSize + (lineCount - 1) * textSpacing; float startY = rec.y + (rec.height - totalTextHeight) / 2; for (int i = 0; i < lineCount; i++) { float lineWidth = MeasureTextEx(font, lines[i], fontSize, textSpacing).x; float startX = rec.x + (rec.width - lineWidth) / 2; DrawTextEx(font, lines[i], (Vector2){startX, startY + i * (fontSize + textSpacing)}, fontSize, textSpacing, color); }
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
    InitLeaderboard();
    ResetPlayerScore();
    
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }

    UnloadTexture(texMenu); UnloadTexture(texQuestion); UnloadTexture(texHowToPlay);
    UnloadTexture(texLeaderboard); UnloadTexture(texCredits); UnloadTexture(texLogo);
    UnloadFont(fontMontserrat);
    UnloadSound(selectSfx); UnloadSound(buttonSfx); UnloadSound(correctSfx);
    UnloadSound(wrongSfx); UnloadSound(typeSfx); UnloadSound(victorySfx);
    UnloadMusicStream(rainMusic);

    UnloadMusicPlayer();

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
    
    if (notificationTimer > 0) {
        notificationTimer -= deltaTime;
        if (notificationTimer < 0) {
            notificationTimer = 0;
            pointsGainedNotification = 0;
        }
    }

    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }
    
    switch (currentScreen) {
        case SCREEN_MENU: {
            Rectangle btnStart = { 691, 554, 538, 73 };
            Rectangle btnHowToPlay = { 691, 638, 538, 73 };
            Rectangle btnLeaderboard = { 691, 722, 538, 73 };
            Rectangle btnCredits = { 1612, 961, 259, 55 };
            Rectangle btnExit = { 787, 916, 347, 100 };
            bool isMouseOverStart = CheckCollisionPointRec(mousePos, btnStart);
            if (isMouseOverStart && !isHoveringBtnStart) PlaySound(selectSfx);
            isHoveringBtnStart = isMouseOverStart;
            bool isMouseOverHowToPlay = CheckCollisionPointRec(mousePos, btnHowToPlay);
            if (isMouseOverHowToPlay && !isHoveringBtnHowToPlay) PlaySound(selectSfx);
            isHoveringBtnHowToPlay = isMouseOverHowToPlay;
            bool isMouseOverLeaderboard = CheckCollisionPointRec(mousePos, btnLeaderboard);
            if (isMouseOverLeaderboard && !isHoveringBtnLeaderboard) PlaySound(selectSfx);
            isHoveringBtnLeaderboard = isMouseOverLeaderboard;
            bool isMouseOverCredits = CheckCollisionPointRec(mousePos, btnCredits);
            if (isMouseOverCredits && !isHoveringBtnCredits) PlaySound(selectSfx);
            isHoveringBtnCredits = isMouseOverCredits;
            bool isMouseOverExit = CheckCollisionPointRec(mousePos, btnExit);
            if (isMouseOverExit && !isHoveringBtnExit) PlaySound(selectSfx);
            isHoveringBtnExit = isMouseOverExit;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, btnStart)) {
                    if (hasVisitedHowToPlay) {
                        PlaySound(buttonSfx); PlayMusicStream(rainMusic);
                        currentScreen = SCREEN_ENTER_NAME; StartWaterAnimation();
                        playerName[0] = '\0'; nameCharCount = 0;
                    } else {
                        PlaySound(wrongSfx); menuNotificationText = "Atencao! Por favor, leia 'Como Jogar' antes de iniciar."; menuNotificationTimer = 3.0f;
                    }
                }
                if (CheckCollisionPointRec(mousePos, btnHowToPlay)) { PlaySound(buttonSfx); hasVisitedHowToPlay = true; currentScreen = SCREEN_HOW_TO_PLAY; }
                if (CheckCollisionPointRec(mousePos, btnLeaderboard)) { PlaySound(buttonSfx); currentScreen = SCREEN_LEADERBOARD; }
                if (CheckCollisionPointRec(mousePos, btnCredits)) { PlaySound(buttonSfx); currentScreen = SCREEN_CREDITS; }
                if (CheckCollisionPointRec(mousePos, btnExit)) { PlaySound(buttonSfx); CloseWindow(); }
            }
        } break;
        case SCREEN_HOW_TO_PLAY: case SCREEN_LEADERBOARD: case SCREEN_CREDITS: {
            Rectangle btnBack = { 787, 891, 347, 100 };
            bool isMouseOverBack = CheckCollisionPointRec(mousePos, btnBack);
            if (isMouseOverBack && !isHoveringBtnBack) PlaySound(selectSfx);
            isHoveringBtnBack = isMouseOverBack;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, btnBack)) {
                PlaySound(buttonSfx); GoToMenu(); 
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
                Rectangle optionClickRects[4] = { { 150, 500, 570, 80 }, { 1200, 500, 570, 80 }, { 150, 720, 570, 80 }, { 1200, 720, 570, 80 } };
                bool answerConfirmed = false;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    for (int i = 0; i < 4; i++) {
                        if (CheckCollisionPointRec(mousePos, optionClickRects[i])) {
                            selectedAnswer = i; PlaySound(selectSfx); answerConfirmed = true; break; 
                        }
                    }
                }
                int keyPressed = -1;
                if (IsKeyPressed(KEY_A)) keyPressed = 0; if (IsKeyPressed(KEY_B)) keyPressed = 1;
                if (IsKeyPressed(KEY_C)) keyPressed = 2; if (IsKeyPressed(KEY_D)) keyPressed = 3;
                if (keyPressed != -1) { selectedAnswer = keyPressed; PlaySound(selectSfx); }
                if (IsKeyPressed(KEY_ENTER) && selectedAnswer != -1) { answerConfirmed = true; }

                if (answerConfirmed) {
                    Question q = GetQuestionFromOrder(questionOrder[currentQuestionIndex]);
                    isAnswerCorrect = (selectedAnswer == q.correctOption);
                    if (isAnswerCorrect) {
                        PlaySound(correctSfx); 
                        int pointsEarned = CalculatePointsEarned(q, questionTimer);
                        AddToPlayerScore(pointsEarned);
                        pointsGainedNotification = pointsEarned; 
                        notificationTimer = 2.0f;
                    } else { PlaySound(wrongSfx); }
                    currentScreen = SCREEN_SHOW_ANSWER; answerTimer = 2.0f;
                }
            } else if (currentScreen == SCREEN_SHOW_ANSWER) {
                answerTimer -= deltaTime;
                if (answerTimer <= 0) {
                    currentQuestionIndex++; selectedAnswer = -1;
                    if (currentQuestionIndex >= QUIZ_QUESTION_COUNT) { UpdateLeaderboard(playerName, GetPlayerScore()); currentScreen = SCREEN_GAME_OVER; } 
                    else { currentScreen = SCREEN_GAMEPLAY; questionTimer = QUESTION_TIME; }
                }
            } else if (currentScreen == SCREEN_GAME_OVER) {
                 if (IsKeyPressed(KEY_ENTER)) { PlaySound(victorySfx); currentScreen = SCREEN_LEADERBOARD; ResetWaterFx(); }
            }
        } break;
        default: break;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (currentScreen) {
        // <<< BLOCO DE DESENHO DO MENU RESTAURADO >>>
        case SCREEN_MENU: {
            DrawTexture(texMenu, 0, 0, WHITE);
            Rectangle btnStart = { 691, 554, 538, 73 };
            Rectangle btnHowToPlay = { 691, 638, 538, 73 };
            Rectangle btnLeaderboard = { 691, 722, 538, 73 };
            Rectangle btnCredits = { 1612, 961, 259, 55 };
            Rectangle btnExit = { 787, 916, 347, 100 };
            if (CheckCollisionPointRec(mousePos, btnStart)) DrawRectangleLinesEx(btnStart, 4, BLUE);
            if (CheckCollisionPointRec(mousePos, btnHowToPlay)) DrawRectangleLinesEx(btnHowToPlay, 4, BLUE);
            if (CheckCollisionPointRec(mousePos, btnLeaderboard)) DrawRectangleLinesEx(btnLeaderboard, 4, BLUE);
            if (CheckCollisionPointRec(mousePos, btnCredits)) DrawRectangleLinesEx(btnCredits, 4, BLUE);
            if (CheckCollisionPointRec(mousePos, btnExit)) DrawRectangleLinesEx(btnExit, 4, BLUE);
            if (menuNotificationTimer > 0 && menuNotificationText != NULL) {
                float alpha = 1.0f; if (menuNotificationTimer < 0.5f) alpha = menuNotificationTimer / 0.5f;
                Vector2 textSize = MeasureTextEx(fontMontserrat, menuNotificationText, 35, 2);
                float rectWidth = textSize.x + 40; float rectHeight = textSize.y + 20;
                Rectangle notificationRect = {(SCREEN_WIDTH - rectWidth) / 2, 850, rectWidth, rectHeight};
                DrawRectangleRec(notificationRect, Fade(BLACK, 0.7f * alpha));
                DrawRectangleLinesEx(notificationRect, 2, Fade(WHITE, alpha));
                DrawTextEx(fontMontserrat, menuNotificationText, (Vector2){notificationRect.x + 20, notificationRect.y + 10}, 35, 2, Fade(YELLOW, alpha));
            }
        } break;
        case SCREEN_HOW_TO_PLAY: { DrawTexture(texHowToPlay, 0, 0, WHITE); Rectangle btnBack = { 820, 911, 280, 70 }; if (CheckCollisionPointRec(mousePos, btnBack)) DrawRectangleLinesEx(btnBack, 4, BLUE); } break;
        case SCREEN_CREDITS: { DrawTexture(texCredits, 0, 0, WHITE); Rectangle btnBack = { 820, 911, 280, 70 }; if (CheckCollisionPointRec(mousePos, btnBack)) DrawRectangleLinesEx(btnBack, 4, BLUE); } break;
        case SCREEN_LEADERBOARD: { 
            DrawTexture(texLeaderboard, 0, 0, WHITE); 
            Rectangle btnBack = { 820, 911, 280, 70 }; 
            if (CheckCollisionPointRec(mousePos, btnBack)) DrawRectangleLinesEx(btnBack, 4, BLUE); 
            const PlayerScore* leaderboard = GetLeaderboard();
            int startY = 420; int stepY = 49; int nameCenterX = 958; int scoreX = 1120; int fontSize = 35; float spacing = 2.0f; 
            for (int i = 0; i < LEADERBOARD_SIZE; i++) { 
                const char* nameText = leaderboard[i].name; 
                Vector2 nameTextSize = MeasureTextEx(fontMontserrat, nameText, fontSize, spacing); 
                DrawTextEx(fontMontserrat, nameText, (Vector2){nameCenterX - (nameTextSize.x / 2), startY + (i * stepY)}, fontSize, spacing, BLACK); 
                const char* scoreText = TextFormat("%03d", leaderboard[i].score); 
                DrawTextEx(fontMontserrat, scoreText, (Vector2){scoreX, startY + (i * stepY)}, fontSize, spacing, BLACK); 
            } 
        } break;
        case SCREEN_ENTER_NAME: { DrawWaterFx(currentTime); const char* title = "Tudo pronto para comecar!"; const char* subtitle = "Aguarde a maré subir..."; if (IsWaterAnimationFinished()) { subtitle = "Digite suas iniciais (3 letras):"; } DrawTextEx(fontMontserrat, title, (Vector2){SCREEN_WIDTH/2 - MeasureTextEx(fontMontserrat, title, 60, 2).x/2, 300}, 60, 2, RAYWHITE); DrawTextEx(fontMontserrat, subtitle, (Vector2){SCREEN_WIDTH/2 - MeasureTextEx(fontMontserrat, subtitle, 40, 2).x/2, 500}, 40, 2, LIGHTGRAY); if (IsWaterAnimationFinished()) { const char* hint = "Pressione ENTER para iniciar o quiz"; DrawRectangle(SCREEN_WIDTH/2 - 150, 560, 300, 80, RAYWHITE); DrawRectangleLines(SCREEN_WIDTH/2 - 150, 560, 300, 80, DARKGRAY); DrawTextEx(fontMontserrat, playerName, (Vector2){SCREEN_WIDTH/2 - MeasureTextEx(fontMontserrat, playerName, 60, 2).x/2, 570}, 60, 2, DARKBLUE); if (nameCharCount < MAX_NAME_LENGTH && ((int)(GetTime()*2.0f)) % 2 == 0) { Vector2 textSize = MeasureTextEx(fontMontserrat, playerName, 60, 2); DrawTextEx(fontMontserrat, "_", (Vector2){SCREEN_WIDTH/2 - textSize.x/2 + textSize.x, 570}, 60, 2, DARKBLUE); } DrawTextEx(fontMontserrat, hint, (Vector2){SCREEN_WIDTH/2 - MeasureTextEx(fontMontserrat, hint, 30, 2).x/2, 700}, 30, 2, LIGHTGRAY); } } break;
        case SCREEN_GAMEPLAY: case SCREEN_SHOW_ANSWER: { DrawTexture(texQuestion, 0, 0, WHITE); DrawWaterFx(currentTime); if (currentScreen == SCREEN_GAMEPLAY) { float timerPercentage = questionTimer / QUESTION_TIME; if (timerPercentage < 0) timerPercentage = 0; float barWidth = 800; float barX = (SCREEN_WIDTH - barWidth) / 2; Color timerColor = GREEN; if (timerPercentage < 0.5f) timerColor = YELLOW; if (timerPercentage < 0.25f) timerColor = RED; DrawRectangle(barX, 80, barWidth, 30, Fade(DARKGRAY, 0.8f)); DrawRectangle(barX, 80, barWidth * timerPercentage, 30, timerColor); DrawRectangleLines(barX, 80, barWidth, 30, DARKBLUE); } Question q = GetQuestionFromOrder(questionOrder[currentQuestionIndex]); Rectangle questionRec = { 157, 243, 1595, 155 }; DrawTextWrappedCentered(fontMontserrat, q.questionText, questionRec, 40, 5, WHITE); Rectangle optionTextRects[4] = { {215, 505, 500, 72}, {1262, 506, 500, 72}, {217, 726, 500, 72}, {1258, 732, 500, 72} }; const char* optionLetters[] = {"A", "B", "C", "D"}; Vector2 letterPositions[] = { {157, 503}, {1204, 504}, {159, 724}, {1200, 730} }; for (int i = 0; i < 4; i++) { Color letterColor = WHITE; if (currentScreen == SCREEN_GAMEPLAY) { if (i == selectedAnswer) letterColor = YELLOW; } else if (currentScreen == SCREEN_SHOW_ANSWER) { if (i == q.correctOption) letterColor = GREEN; else if (i == selectedAnswer && !isAnswerCorrect) letterColor = RED; else if (selectedAnswer == -1 && i == q.correctOption) letterColor = ORANGE; } DrawTextEx(fontMontserrat, optionLetters[i], letterPositions[i], 73, 2.0f, letterColor); DrawTextWrappedCentered(fontMontserrat, q.options[i], optionTextRects[i], 36, 3, WHITE); } const char* questionTextStr = TextFormat("Questao: %02d/%d", currentQuestionIndex + 1, QUIZ_QUESTION_COUNT); DrawTextEx(fontMontserrat, questionTextStr, (Vector2){40, 30}, 40, 2.0f, DARKBLUE); const char* difficultyText; Color difficultyColor; switch(q.difficulty) { case EASY: difficultyText = "FACIL"; difficultyColor = GREEN; break; case MEDIUM: difficultyText = "MEDIA"; difficultyColor = YELLOW; break; case HARD: difficultyText = "DIFICIL"; difficultyColor = RED; break; } DrawTextEx(fontMontserrat, difficultyText, (Vector2){40, 85}, 30, 2.0f, difficultyColor); DrawTexture(texLogo, 0, 0, WHITE); const char* scoreText = TextFormat("Pontos: %03d", GetPlayerScore()); DrawTextEx(fontMontserrat, scoreText, (Vector2){1650, 30}, 40, 2.0f, DARKBLUE); if (notificationTimer > 0) { const char* notificationText = TextFormat("+%d PONTOS", pointsGainedNotification); Vector2 scoreTextSize = MeasureTextEx(fontMontserrat, scoreText, 40, 2.0f); Vector2 notificationTextSize = MeasureTextEx(fontMontserrat, notificationText, 25, 2.0f); float alpha = notificationTimer / 2.0f; DrawTextEx(fontMontserrat, notificationText, (Vector2){1650 + (scoreTextSize.x - scoreTextSize.x) / 2, 30 + 45}, 25, 2.0f, Fade(GREEN, alpha)); } } break;
        case SCREEN_GAME_OVER: { DrawWaterFx(currentTime); const char* title = "FIM DE JOGO!"; const char* scoreText = TextFormat("Sua pontuacao final: %d", GetPlayerScore()); const char* hint = "Pressione ENTER para ver o placar"; DrawTextEx(fontMontserrat, title, (Vector2){SCREEN_WIDTH/2 - MeasureTextEx(fontMontserrat, title, 80, 2).x/2, 350}, 80, 2, RAYWHITE); DrawTextEx(fontMontserrat, scoreText, (Vector2){SCREEN_WIDTH/2 - MeasureTextEx(fontMontserrat, scoreText, 50, 2).x/2, 500}, 50, 2, RAYWHITE); DrawTextEx(fontMontserrat, hint, (Vector2){SCREEN_WIDTH/2 - MeasureTextEx(fontMontserrat, hint, 30, 2).x/2, 700}, 30, 2, LIGHTGRAY); } break;
        default: break;
    }

    DrawMusicPlayer();
    
    EndDrawing();
}