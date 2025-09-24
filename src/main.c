/**
 * @file quiz_ods14.c
 * @author Gemini, com base no protótipo de Prof. Dr. David Buzatto
 * @brief Jogo de Quiz completo sobre a ODS 14 usando Raylib.
 * @version 1.4
 * @date 2025-09-24
 *
 * @copyright Copyright (c) 2025
 *
 * @note Mudanças da v1.4:
 * - Adicionada funcionalidade de tela cheia (fullscreen).
 * - O jogador pode pressionar F11 a qualquer momento para alternar.
 */

#include "raylib/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> // Para embaralhar as perguntas

//---------------------------------------------
// Definições e Constantes
//---------------------------------------------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define TOTAL_QUESTIONS 30
#define QUIZ_QUESTIONS 10
#define NUM_EASY 5
#define NUM_MEDIUM 3
#define NUM_HARD 2
#define LEADERBOARD_SIZE 6
#define MAX_NAME_LENGTH 3

//---------------------------------------------
// Tipos Customizados (Enums, Structs)
//---------------------------------------------
typedef enum {
    SCREEN_MENU,
    SCREEN_HOW_TO_PLAY,
    SCREEN_LEADERBOARD,
    SCREEN_CREDITS,
    SCREEN_ENTER_NAME,
    SCREEN_GAMEPLAY,
    SCREEN_SHOW_ANSWER,
    SCREEN_GAME_OVER
} GameScreen;

typedef enum {
    EASY,
    MEDIUM,
    HARD
} Difficulty;

typedef struct {
    const char* questionText;
    const char* options[4];
    int correctOption;
    Difficulty difficulty;
    int points;
} Question;

typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    int score;
} PlayerScore;

//---------------------------------------------
// Variáveis Globais
//---------------------------------------------
static GameScreen currentScreen = SCREEN_MENU;
static Question questions[TOTAL_QUESTIONS];
static int questionOrder[QUIZ_QUESTIONS];
static int easyQuestionIndices[TOTAL_QUESTIONS];
static int mediumQuestionIndices[TOTAL_QUESTIONS];
static int hardQuestionIndices[TOTAL_QUESTIONS];
static int easyCount = 0, mediumCount = 0, hardCount = 0;
static int currentQuestionIndex = 0;
static int playerScore = 0;
static int selectedAnswer = -1;
static bool isAnswerCorrect = false;
static float answerTimer = 0.0f;
static PlayerScore leaderboard[LEADERBOARD_SIZE];
static char playerName[MAX_NAME_LENGTH + 1] = "\0";
static int nameCharCount = 0;
static Texture2D texMenu;
static Texture2D texQuestion;
static Texture2D texHowToPlay;
static Texture2D texLeaderboard;
static Texture2D texCredits;

//---------------------------------------------
// Protótipos de Funções
//---------------------------------------------
void UpdateDrawFrame(void);
void InitializeQuestions(void);
void LoadLeaderboard(void);

//---------------------------------------------
// Banco de Perguntas - ODS 14
//---------------------------------------------
void InitializeQuestions() {
    // ... (O conteúdo desta função é o mesmo, sem alterações) ...
    questions[0] = (Question){"Qual e o objetivo principal da ODS 14?", {"Erradicar a fome", "Vida na agua", "Energia limpa", "Cidades sustentaveis"}, 1, EASY, 10};
    questions[1] = (Question){"Qual destes e a maior ameaca direta aos recifes de coral?", {"Turismo excessivo", "Branqueamento por aquecimento", "Pesca com lanca", "Construcao de portos"}, 1, EASY, 10};
    questions[2] = (Question){"O que significa 'sobrepesca'?", {"Pescar a mais do que o permitido", "Pescar mais rapido que a reposicao", "Usar redes muito grandes", "Pescar fora da temporada"}, 1, EASY, 10};
    questions[3] = (Question){"Qual o principal tipo de poluicao que forma as 'ilhas de lixo' nos oceanos?", {"Petroleo", "Esgoto domestico", "Plastico", "Metais pesados"}, 2, EASY, 10};
    questions[4] = (Question){"As tartarugas marinhas frequentemente confundem sacolas plasticas com qual alimento?", {"Peixes pequenos", "Algas marinhas", "Agua-viva", "Esponjas do mar"}, 2, EASY, 10};
    questions[5] = (Question){"Qual ecossistema costeiro e conhecido por proteger a costa de tempestades?", {"Praias de areia", "Recifes de coral", "Manguezais", "Fundo do oceano"}, 2, EASY, 10};
    questions[6] = (Question){"O aumento do CO2 na atmosfera causa qual fenomeno no oceano?", {"Aumento do nivel do mar", "Mares mais salgados", "Acidificacao", "Mais oxigenio na agua"}, 2, EASY, 10};
    questions[7] = (Question){"O que sao 'areas marinhas protegidas' (AMPs)?", {"Areas so para turismo", "Zonas de exclusao militar", "Areas com regras para proteger a vida marinha", "Locais de pesquisa privados"}, 2, EASY, 10};
    questions[8] = (Question){"Qual a porcentagem aproximada da superficie da Terra coberta por oceanos?", {"50%", "60%", "70%", "80%"}, 2, EASY, 10};
    questions[9] = (Question){"De onde vem a maior parte do oxigenio que respiramos?", {"Florestas tropicais", "Fitoplancton marinho", "Campos agricolas", "Geleiras"}, 1, EASY, 10};
    questions[10] = (Question){"A meta 14.1 da ODS 14 visa prevenir e reduzir significativamente que tipo de poluicao ate 2025?", {"Poluicao sonora", "Poluicao luminosa", "Poluicao marinha de todos os tipos", "Poluicao termica"}, 2, MEDIUM, 20};
    questions[11] = (Question){"O que e a 'pesca fantasma'?", {"Pescar a noite sem luzes", "Pesca ilegal nao declarada", "Equipamentos de pesca perdidos que continuam a capturar animais", "Usar iscas que brilham"}, 2, MEDIUM, 20};
    questions[12] = (Question){"Qual o nome do acordo global que visa combater a poluicao plastica, discutido pela ONU?", {"Acordo de Paris", "Protocolo de Kyoto", "Tratado Global do Plastico", "Convencao de Estocolmo"}, 2, MEDIUM, 20};
    questions[13] = (Question){"O que sao microplasticos?", {"Plasticos visiveis a olho nu", "Pequenos organismos plasticos", "Fragmentos de plastico com menos de 5mm", "Plasticos que afundam rapidamente"}, 2, MEDIUM, 20};
    questions[14] = (Question){"Qual a principal causa da acidificacao dos oceanos?", {"Derramamento de produtos quimicos", "Absorcao de CO2 da atmosfera", "Esgoto nao tratado", "Uso de fertilizantes"}, 1, MEDIUM, 20};
    questions[15] = (Question){"Ate 2020, a meta 14.5 da ODS 14 visava conservar pelo menos quantos % das zonas costeiras e marinhas?", {"5%", "10%", "15%", "20%"}, 1, MEDIUM, 20};
    questions[16] = (Question){"Qual destes organismos e mais vulneravel a acidificacao, por ter estruturas de carbonato de calcio?", {"Baleias", "Corais", "Tubarões", "Algas"}, 1, MEDIUM, 20};
    questions[17] = (Question){"A 'Zona Economica Exclusiva' (ZEE) se estende por ate quantas milhas nauticas da costa de um pais?", {"50", "100", "200", "350"}, 2, MEDIUM, 20};
    questions[18] = (Question){"O que e 'eutrofizacao'?", {"Aumento da temperatura da agua", "Excesso de nutrientes causando explosao de algas", "Reducao do sal na agua", "Aumento da clareza da agua"}, 1, MEDIUM, 20};
    questions[19] = (Question){"Qual a funcao dos subsidios a pesca que a ODS 14.6 busca proibir?", {"Aumentar a seguranca dos pescadores", "Incentivar a sobrepesca", "Financiar a pesquisa marinha", "Promover o ecoturismo"}, 1, MEDIUM, 20};
    questions[20] = (Question){"O 'Giro do Pacifico Norte' e famoso por qual razao ambiental negativa?", {"Maior atividade vulcanica submarina", "Zona de pesca mais produtiva", "Grande acumulacao de lixo plastico", "Rota de migracao de baleias"}, 2, HARD, 30};
    questions[21] = (Question){"Qual a formula quimica da reacao principal que causa a acidificacao do oceano?", {"CO2 + H2O -> H2CO3", "2H2 + O2 -> 2H2O", "NaCl + H2O -> Na+ + Cl-", "CaCO3 -> Ca2+ + CO3 2-"}, 0, HARD, 30};
    questions[22] = (Question){"A Convencao das Nacoes Unidas sobre o Direito do Mar (UNCLOS) foi estabelecida em que ano?", {"1972", "1982", "1992", "2002"}, 1, HARD, 30};
    questions[23] = (Question){"O que e o 'sequestro de carbono azul' (blue carbon)?", {"Captura de CO2 por tecnologias no mar", "Captura de CO2 por ecossistemas costeiros", "O carbono emitido por navios", "Um tipo de combustivel fossil marinho"}, 1, HARD, 30};
    questions[24] = (Question){"Qual organismo microscopico forma a base da maior parte da teia alimentar marinha?", {"Zooplancton", "Krill", "Fitoplancton", "Bacterias"}, 2, HARD, 30};
    questions[25] = (Question){"A 'hipoxia' em zonas costeiras, conhecida como 'zonas mortas', e causada principalmente pela falta de:", {"Luz solar", "Nutrientes", "Salinidade", "Oxigenio"}, 3, HARD, 30};
    questions[26] = (Question){"O conceito de 'Rendimento Maximo Sustentavel' (MSY) na pesca refere-se a:", {"Maior lucro possivel em um ano", "Maior captura que pode ser retirada sem esgotar o estoque", "Numero maximo de barcos permitidos", "Peso maximo de um unico peixe"}, 1, HARD, 30};
    questions[27] = (Question){"Qual destes e um exemplo de 'especie invasora' marinha que causou grandes danos no Caribe?", {"Peixe-palhaco", "Tubarão-baleia", "Peixe-leao", "Moreia"}, 2, HARD, 30};
    questions[28] = (Question){"Qual decada foi proclamada pela ONU como a 'Decada da Ciencia Oceanica para o Desenvolvimento Sustentavel'?", {"2011-2020", "2021-2030", "2031-2040", "2041-2050"}, 1, HARD, 30};
    questions[29] = (Question){"A 'termoclina' em um corpo de agua e uma camada distinta onde o que muda rapidamente com a profundidade?", {"Pressao", "Salinidade", "Temperatura", "Visibilidade"}, 2, HARD, 30};

    for (int i = 0; i < TOTAL_QUESTIONS; i++) {
        switch (questions[i].difficulty) {
            case EASY:   easyQuestionIndices[easyCount++] = i;   break;
            case MEDIUM: mediumQuestionIndices[mediumCount++] = i; break;
            case HARD:   hardQuestionIndices[hardCount++] = i;   break;
        }
    }
}

//---------------------------------------------
// Funções do Jogo
//---------------------------------------------
void ShuffleIntArray(int *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void SelectAndShuffleQuizQuestions() {
    ShuffleIntArray(easyQuestionIndices, easyCount);
    ShuffleIntArray(mediumQuestionIndices, mediumCount);
    ShuffleIntArray(hardQuestionIndices, hardCount);
    int currentQuizIndex = 0;
    for (int i = 0; i < NUM_EASY && i < easyCount; i++) questionOrder[currentQuizIndex++] = easyQuestionIndices[i];
    for (int i = 0; i < NUM_MEDIUM && i < mediumCount; i++) questionOrder[currentQuizIndex++] = mediumQuestionIndices[i];
    for (int i = 0; i < NUM_HARD && i < hardCount; i++) questionOrder[currentQuizIndex++] = hardQuestionIndices[i];
    ShuffleIntArray(questionOrder, QUIZ_QUESTIONS);
}

void SaveLeaderboard() {
    FILE *file = fopen("leaderboard.dat", "wb");
    if (file != NULL) {
        fwrite(leaderboard, sizeof(PlayerScore), LEADERBOARD_SIZE, file);
        fclose(file);
    }
}

void LoadLeaderboard() {
    FILE *file = fopen("leaderboard.dat", "rb");
    if (file != NULL) {
        fread(leaderboard, sizeof(PlayerScore), LEADERBOARD_SIZE, file);
        fclose(file);
    } else {
        for (int i = 0; i < LEADERBOARD_SIZE; i++) {
            strcpy(leaderboard[i].name, "AAA");
            leaderboard[i].score = 0;
        }
        SaveLeaderboard();
    }
}

void UpdateLeaderboard() {
    int existingIndex = -1;
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        if (strcmp(playerName, leaderboard[i].name) == 0) {
            existingIndex = i;
            break;
        }
    }

    if (existingIndex != -1) {
        if (playerScore > leaderboard[existingIndex].score) {
            leaderboard[existingIndex].score = playerScore;
            for (int i = 0; i < LEADERBOARD_SIZE - 1; i++) {
                for (int j = 0; j < LEADERBOARD_SIZE - i - 1; j++) {
                    if (leaderboard[j].score < leaderboard[j + 1].score) {
                        PlayerScore temp = leaderboard[j];
                        leaderboard[j] = leaderboard[j + 1];
                        leaderboard[j + 1] = temp;
                    }
                }
            }
        }
    } else {
        int insertPosition = -1;
        if (playerScore > leaderboard[LEADERBOARD_SIZE - 1].score) {
            for (int i = 0; i < LEADERBOARD_SIZE; i++) {
                if (playerScore > leaderboard[i].score) {
                    insertPosition = i;
                    break;
                }
            }
            if (insertPosition != -1) {
                for (int i = LEADERBOARD_SIZE - 1; i > insertPosition; i--) {
                    leaderboard[i] = leaderboard[i - 1];
                }
                strcpy(leaderboard[insertPosition].name, playerName);
                leaderboard[insertPosition].score = playerScore;
            }
        }
    }
    SaveLeaderboard();
}

void StartGame() {
    SelectAndShuffleQuizQuestions();
    currentQuestionIndex = 0;
    playerScore = 0;
    selectedAnswer = -1;
    currentScreen = SCREEN_GAMEPLAY;
}

//---------------------------------------------
// Ponto de Entrada Principal (main)
//---------------------------------------------
int main(void) {
    srand(time(NULL));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Quiz - Navegando pela ODS 14");
    SetTargetFPS(60);

    texMenu = LoadTexture("resources/tela_menu.png");
    texQuestion = LoadTexture("resources/tela_pergunta.png");
    texHowToPlay = LoadTexture("resources/tela_comojogar.png");
    texLeaderboard = LoadTexture("resources/tela_leaderboard.png");
    texCredits = LoadTexture("resources/tela_creditos.png");

    InitializeQuestions();
    LoadLeaderboard();

    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }

    UnloadTexture(texMenu);
    UnloadTexture(texQuestion);
    UnloadTexture(texHowToPlay);
    UnloadTexture(texLeaderboard);
    UnloadTexture(texCredits);

    CloseWindow();
    return 0;
}

//---------------------------------------------
// Loop Principal de Atualização e Desenho
//---------------------------------------------
void UpdateDrawFrame(void) {
    // <<< NOVO: Verificação para alternar tela cheia >>>
    // Pressione F11 para entrar ou sair do modo de tela cheia
    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }

    Vector2 mousePos = GetMousePosition();
    // --- Lógica de Atualização (Update) ---
    switch (currentScreen) {
        case SCREEN_MENU: {

            Rectangle btnStart = { 691, 554, 538, 73 };
            Rectangle btnHowToPlay = { 691, 638, 538, 73 };
            Rectangle btnLeaderboard = { 691, 722, 538, 73 };
            Rectangle btnCredits = { 1612, 991, 259, 55 };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, btnStart)) {
                    playerName[0] = '\0'; nameCharCount = 0;
                    currentScreen = SCREEN_ENTER_NAME;
                }
                if (CheckCollisionPointRec(mousePos, btnHowToPlay)) currentScreen = SCREEN_HOW_TO_PLAY;
                if (CheckCollisionPointRec(mousePos, btnLeaderboard)) currentScreen = SCREEN_LEADERBOARD;
                if (CheckCollisionPointRec(mousePos, btnCredits)) currentScreen = SCREEN_CREDITS;
            }
        } break;
        case SCREEN_HOW_TO_PLAY: case SCREEN_LEADERBOARD: case SCREEN_CREDITS: {
              Rectangle btnBack = { 787, 891, 347, 100 };
              if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, btnBack)) {
                  currentScreen = SCREEN_MENU;
              }
        } break;
        case SCREEN_ENTER_NAME: {
            int key = GetKeyPressed();
            if ((key >= KEY_A && key <= KEY_Z) && (nameCharCount < MAX_NAME_LENGTH)) {
                playerName[nameCharCount++] = (char)key;
                playerName[nameCharCount] = '\0';
            }
            if (IsKeyPressed(KEY_BACKSPACE)) {
                nameCharCount--;
                if (nameCharCount < 0) nameCharCount = 0;
                playerName[nameCharCount] = '\0';
            }
            if (IsKeyPressed(KEY_ENTER) && nameCharCount > 0) {
                StartGame();
            }
        } break;
        case SCREEN_GAMEPLAY: {
            if (IsKeyPressed(KEY_A)) selectedAnswer = 0;
            if (IsKeyPressed(KEY_B)) selectedAnswer = 1;
            if (IsKeyPressed(KEY_C)) selectedAnswer = 2;
            if (IsKeyPressed(KEY_D)) selectedAnswer = 3;
            if (IsKeyPressed(KEY_ENTER) && selectedAnswer != -1) {
                int qIndex = questionOrder[currentQuestionIndex];
                isAnswerCorrect = (selectedAnswer == questions[qIndex].correctOption);
                if (isAnswerCorrect) playerScore += questions[qIndex].points;
                currentScreen = SCREEN_SHOW_ANSWER;
                answerTimer = 2.0f;
            }
        } break;
        case SCREEN_SHOW_ANSWER: {
            answerTimer -= GetFrameTime();
            if (answerTimer <= 0) {
                currentQuestionIndex++;
                selectedAnswer = -1;
                if (currentQuestionIndex >= QUIZ_QUESTIONS) {
                    UpdateLeaderboard();
                    currentScreen = SCREEN_GAME_OVER;
                } else {
                    currentScreen = SCREEN_GAMEPLAY;
                }
            }
        } break;
        case SCREEN_GAME_OVER: {
            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = SCREEN_LEADERBOARD;
            }
        } break;
        default: break;
    }

    // --- Lógica de Desenho (Draw) ---
    BeginDrawing();
    ClearBackground(RAYWHITE);
    switch (currentScreen) {
        case SCREEN_MENU: DrawTexture(texMenu, 0, 0, WHITE); break;
        case SCREEN_HOW_TO_PLAY: DrawTexture(texHowToPlay, 0, 0, WHITE); break;
        case SCREEN_CREDITS: DrawTexture(texCredits, 0, 0, WHITE); break;
        case SCREEN_LEADERBOARD: {
            DrawTexture(texLeaderboard, 0, 0, WHITE);
            int startY = 482;
            int stepY = 78;
            int nameCenterX = 1010;
            int scoreX = 1180;
            int fontSize = 50;
            for (int i = 0; i < LEADERBOARD_SIZE; i++) {
                const char* nameText = leaderboard[i].name;
                int nameTextWidth = MeasureText(nameText, fontSize);
                DrawText(nameText, nameCenterX - (nameTextWidth / 2), startY + (i * stepY), fontSize, DARKBLUE);
                const char* scoreText = TextFormat("%05d", leaderboard[i].score);
                DrawText(scoreText, scoreX, startY + (i * stepY), fontSize, DARKBLUE);
            }
        } break;
        case SCREEN_ENTER_NAME: {
            ClearBackground(DARKBLUE);
            DrawText("Tudo pronto para comecar!", SCREEN_WIDTH/2 - MeasureText("Tudo pronto para comecar!", 60)/2, 300, 60, RAYWHITE);
            DrawText("Digite suas iniciais (3 letras):", SCREEN_WIDTH/2 - MeasureText("Digite suas iniciais (3 letras):", 40)/2, 500, 40, LIGHTGRAY);
            DrawRectangle(SCREEN_WIDTH/2 - 150, 560, 300, 80, RAYWHITE);
            DrawRectangleLines(SCREEN_WIDTH/2 - 150, 560, 300, 80, DARKGRAY);
            DrawText(playerName, SCREEN_WIDTH/2 - MeasureText(playerName, 60)/2, 570, 60, DARKBLUE);
            if (nameCharCount < MAX_NAME_LENGTH && ((int)(GetTime()*2.0f)) % 2 == 0) {
                 DrawText("_", SCREEN_WIDTH/2 - MeasureText(playerName, 60)/2 + MeasureText(playerName, 60), 570, 60, DARKBLUE);
            }
            DrawText("Pressione ENTER para iniciar o quiz", SCREEN_WIDTH/2 - MeasureText("Pressione ENTER para iniciar o quiz", 30)/2, 700, 30, LIGHTGRAY);
        } break;
        case SCREEN_GAMEPLAY: case SCREEN_SHOW_ANSWER: {
            DrawTexture(texQuestion, 0, 0, WHITE);
            int qIndex = questionOrder[currentQuestionIndex];
            Question q = questions[qIndex];
            int textWidth = MeasureText(q.questionText, 28);
            DrawText(q.questionText, 240 + (800 - textWidth) / 2, 170, 28, WHITE);
            DrawText(q.options[0], 380, 555, 36, BLACK);
            DrawText(q.options[1], 1090, 555, 36, BLACK);
            DrawText(q.options[2], 380, 735, 36, BLACK);
            DrawText(q.options[3], 1090, 735, 36, BLACK);
            Rectangle optRects[4] = {
                {354, 530, 500, 95}, {1064, 530, 500, 95},
                {354, 710, 500, 95}, {1064, 710, 500, 95}
            };
            if (selectedAnswer != -1) DrawRectangleLinesEx(optRects[selectedAnswer], 5, YELLOW);
            if (currentScreen == SCREEN_SHOW_ANSWER) {
                DrawRectangleRec(optRects[q.correctOption], (Color){0, 228, 48, 100});
                if (!isAnswerCorrect) DrawRectangleRec(optRects[selectedAnswer], (Color){255, 0, 0, 100});
            }
            DrawText(TextFormat("Questao: %02d/%d", currentQuestionIndex + 1, QUIZ_QUESTIONS), 40, 30, 40, DARKBLUE);
            DrawText(TextFormat("Pontos: %03d", playerScore), 1650, 30, 40, DARKBLUE);
        } break;
        case SCREEN_GAME_OVER: {
            ClearBackground(DARKBLUE);
            DrawText("FIM DE JOGO!", SCREEN_WIDTH/2 - MeasureText("FIM DE JOGO!", 80)/2, 350, 80, RAYWHITE);
            DrawText(TextFormat("Sua pontuacao final: %d", playerScore), SCREEN_WIDTH/2 - MeasureText(TextFormat("Sua pontuacao final: %d", playerScore), 50)/2, 500, 50, RAYWHITE);
            DrawText("Pressione ENTER para ver o placar", SCREEN_WIDTH/2 - MeasureText("Pressione ENTER para ver o placar", 30)/2, 700, 30, LIGHTGRAY);
        } break;
        default: break;
    }
    EndDrawing();
}