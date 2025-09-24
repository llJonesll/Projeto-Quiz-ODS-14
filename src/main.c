/**
 * @file quiz_ods14.c
 * @author Gemini, com base no protótipo de Prof. Dr. David Buzatto
 * @brief Jogo de Quiz completo sobre a ODS 14 usando Raylib.
 * @version 2.1
 * @date 2025-09-24
 *
 * @copyright Copyright (c) 2025
 *
 * @note Mudanças da v2.1:
 * - Implementado efeito visual de chuva caindo.
 * - Ajustada a frequência e os parâmetros das ondulações da água para simular batidas de chuva.
 * - A animação da água (onda + gotas + chuva visual) agora persiste do SCREEN_ENTER_NAME
 * até o final do quiz (SCREEN_GAME_OVER), aparecendo por baixo das interfaces.
 */

#include "raylib/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> // Para embaralhar as perguntas
#include <math.h> // Para a função sinf da onda

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

// Definições para as ondulações na água
#define MAX_DROPS 60 // Aumentado o número máximo de ondulações ativas
#define DROP_SPAWN_INTERVAL_MIN 0.1f // Intervalo mínimo de tempo entre ondulações (mais frequente)
#define DROP_SPAWN_INTERVAL_MAX 0.3f // Intervalo máximo de tempo entre ondulações (mais frequente)
#define DROP_LIFETIME 1.5f // Tempo de vida da ondulação da gota (ajustado)
#define DROP_AMPLITUDE_MAX 8.0f // Amplitude máxima inicial da ondulação (ajustado para ser mais sutil para chuva)
#define DROP_WAVE_SPEED 250.0f // Velocidade de expansão da ondulação
#define DROP_WAVE_FREQUENCY 0.08f // Frequência da ondulação (mais compacto para chuva)

// <<< NOVAS: Definições para as gotas de chuva visuais >>>
#define MAX_RAINDROPS 500 // Número de gotas de chuva visuais
#define RAINDROP_SPEED_MIN 300 // Velocidade mínima da gota
#define RAINDROP_SPEED_MAX 600 // Velocidade máxima da gota
#define RAINDROP_LENGTH_MIN 40 // Comprimento mínimo da gota
#define RAINDROP_LENGTH_MAX 60 // Comprimento máximo da gota


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

typedef struct {
    Vector2 position; // Posição X da gota (Y é o waterLevel)
    float lifetime;   // Tempo de vida restante da ondulação
    float spawnTime;  // Tempo em que a ondulação foi criada
    float initialAmplitude; // Amplitude inicial da ondulação
} Drop;

// <<< NOVO: Estrutura para uma gota de chuva visual >>>
typedef struct {
    Vector2 startPos;
    float length;
    float speed;
} Raindrop;


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

// Variáveis para controlar a animação da água
static float waterLevel;
static bool isWaterAnimating = false;
static const float WATER_RISE_SPEED = 120.0f;
static const float TARGET_WATER_LEVEL = 150.0f; // Altura final da água (Y menor = mais alto)
static float waveAmplitude = 10.0f;             // Amplitude da onda de fundo (ajustado para ser mais sutil)
static float waveFrequency = 0.005f;            // Frequência da onda de fundo
static float waveSpeed = 80.0f;
static float waveOffset = 0.0f;

static Drop activeDrops[MAX_DROPS];
static float dropSpawnTimer = 0.0f;
static float nextDropSpawnTime = 0.0f;

// <<< NOVAS: Variáveis para as gotas de chuva visuais >>>
static Raindrop raindrops[MAX_RAINDROPS];


//---------------------------------------------
// Protótipos de Funções
//---------------------------------------------
void UpdateDrawFrame(void);
void InitializeQuestions(void);
void LoadLeaderboard(void);

void InitDrops(void);
void SpawnDrop(void);
float GetDropWaveContribution(float x, float currentTime);

// <<< NOVO: Protótipos para gotas de chuva visuais >>>
void InitRaindrops(void);
void UpdateRaindrops(float deltaTime);
void DrawRaindrops(void);


//---------------------------------------------
// Banco de Perguntas - ODS 14
//---------------------------------------------
void InitializeQuestions() {
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
            case EASY:   easyQuestionIndices[easyCount++] = i;  break;
            case MEDIUM: mediumQuestionIndices[mediumCount++] = i; break;
            case HARD:   hardQuestionIndices[hardCount++] = i;  break;
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

// Funções para Ondulações (Drops)
void InitDrops(void) {
    for (int i = 0; i < MAX_DROPS; i++) {
        activeDrops[i].lifetime = 0.0f; // Marca como inativa
    }
    nextDropSpawnTime = GetRandomValue(DROP_SPAWN_INTERVAL_MIN * 100, DROP_SPAWN_INTERVAL_MAX * 100) / 100.0f;
}

void SpawnDrop(void) {
    for (int i = 0; i < MAX_DROPS; i++) {
        if (activeDrops[i].lifetime <= 0.0f) { // Encontrou um slot inativo
            activeDrops[i].position.x = (float)GetRandomValue(0, SCREEN_WIDTH);
            activeDrops[i].lifetime = DROP_LIFETIME;
            activeDrops[i].spawnTime = GetTime();
            activeDrops[i].initialAmplitude = (float)GetRandomValue(DROP_AMPLITUDE_MAX * 50, DROP_AMPLITUDE_MAX * 100) / 100.0f;
            break;
        }
    }
}

float GetDropWaveContribution(float x, float currentTime) {
    float totalContribution = 0.0f;
    for (int i = 0; i < MAX_DROPS; i++) {
        if (activeDrops[i].lifetime > 0.0f) {
            float timeElapsed = currentTime - activeDrops[i].spawnTime;
            float distanceToDrop = fabsf(x - activeDrops[i].position.x);
            
            float waveRadius = timeElapsed * DROP_WAVE_SPEED;
            
            // A ondulação só afeta dentro de um certo raio e atenua com a distância e tempo
            if (distanceToDrop < waveRadius) {
                float attenuationFactor = 1.0f - (distanceToDrop / waveRadius);
                float decayFactor = activeDrops[i].lifetime / DROP_LIFETIME; // Diminui com o tempo de vida da gota

                totalContribution += sinf(distanceToDrop * DROP_WAVE_FREQUENCY - timeElapsed * 15.0f) * // Ajuste de fase para ondulação
                                     activeDrops[i].initialAmplitude * attenuationFactor * decayFactor;
            }
        }
    }
    return totalContribution;
}

// <<< NOVAS FUNÇÕES PARA GOTAS DE CHUVA VISUAIS >>>
void InitRaindrops(void) {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i].startPos = (Vector2){ (float)GetRandomValue(0, SCREEN_WIDTH), (float)GetRandomValue(-SCREEN_HEIGHT, 0) }; // Começam acima da tela
        raindrops[i].length = (float)GetRandomValue(RAINDROP_LENGTH_MIN, RAINDROP_LENGTH_MAX);
        raindrops[i].speed = (float)GetRandomValue(RAINDROP_SPEED_MIN, RAINDROP_SPEED_MAX);
    }
}

void UpdateRaindrops(float deltaTime) {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i].startPos.y += raindrops[i].speed * deltaTime;
        // Se a gota sair da tela, reinicia no topo (incluindo acima do nível da água)
        if (raindrops[i].startPos.y > SCREEN_HEIGHT) {
            raindrops[i].startPos = (Vector2){ (float)GetRandomValue(0, SCREEN_WIDTH), (float)GetRandomValue(-SCREEN_HEIGHT, 0) };
            raindrops[i].length = (float)GetRandomValue(RAINDROP_LENGTH_MIN, RAINDROP_LENGTH_MAX);
            raindrops[i].speed = (float)GetRandomValue(RAINDROP_SPEED_MIN, RAINDROP_SPEED_MAX);
        }
    }
}

void DrawRaindrops(void) {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        // Desenha a gota apenas se ela estiver acima do nível da água ou abaixo do nível da água com uma pequena transparência.
        // Isso impede que as gotas de chuva visíveis pareçam entrar na água e sumir abruptamente.
        // Alternativamente, podemos fazer as gotas terminarem no waterLevel para o efeito de "batida" mais direto.
        // Vamos fazer as gotas sumirem no waterLevel para o efeito de batida.
        if (raindrops[i].startPos.y < waterLevel) {
             DrawLineV(raindrops[i].startPos, (Vector2){ raindrops[i].startPos.x, raindrops[i].startPos.y + raindrops[i].length }, (Color){ 200, 230, 255, 180 });
        } else if (raindrops[i].startPos.y - raindrops[i].length < waterLevel) {
            // Desenha apenas a parte da gota que ainda está acima da água
            DrawLineV((Vector2){ raindrops[i].startPos.x, waterLevel - 2}, (Vector2){ raindrops[i].startPos.x, raindrops[i].startPos.y + raindrops[i].length }, (Color){ 200, 230, 255, 180 });
        }
    }
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
    waterLevel = SCREEN_HEIGHT;
    InitDrops();
    InitRaindrops(); // <<< NOVO: Inicializa as gotas de chuva visuais >>>

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
    float deltaTime = GetFrameTime();
    float currentTime = GetTime();

    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }

    Vector2 mousePos = GetMousePosition();
    switch (currentScreen) {
        case SCREEN_MENU: {
            Rectangle btnStart = { 691, 554, 538, 73 };
            Rectangle btnHowToPlay = { 691, 638, 538, 73 };
            Rectangle btnLeaderboard = { 691, 722, 538, 73 };
            Rectangle btnCredits = { 1612, 991, 259, 55 };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, btnStart)) {
                    currentScreen = SCREEN_ENTER_NAME;
                    isWaterAnimating = true; // Ativa a água aqui
                    waterLevel = SCREEN_HEIGHT;
                    playerName[0] = '\0';
                    nameCharCount = 0;
                    dropSpawnTimer = 0.0f;
                    InitDrops();
                    InitRaindrops(); // Reinicia as gotas de chuva visuais
                    nextDropSpawnTime = GetRandomValue(DROP_SPAWN_INTERVAL_MIN * 100, DROP_SPAWN_INTERVAL_MAX * 100) / 100.0f;
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
                isWaterAnimating = false; // Desativa a água ao voltar ao menu
                waterLevel = SCREEN_HEIGHT;
                waveOffset = 0.0f;
            }
        } break;
        case SCREEN_ENTER_NAME:
        case SCREEN_GAMEPLAY:
        case SCREEN_SHOW_ANSWER:
        case SCREEN_GAME_OVER: { // Essas telas agora têm a água animada
            int key = GetKeyPressed();
            if (currentScreen == SCREEN_ENTER_NAME) {
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
            } else if (currentScreen == SCREEN_GAMEPLAY) {
                if (IsKeyPressed(KEY_A)) selectedAnswer = 0;
                if (IsKeyPressed(KEY_B)) selectedAnswer = 1;
                if (IsKeyPressed(KEY_C)) selectedAnswer = 2;
                if (IsKeyPressed(KEY_D)) selectedAnswer = 3;
                if (IsKeyPressed(KEY_ENTER) && selectedAnswer != -1) {
                    int qIndex = questionOrder[currentQuestionIndex];
                    isAnswerCorrect = (selectedAnswer == questions[qIndex].correctOption);
                    if (isAnswerCorrect) {
                        playerScore += questions[qIndex].points;
                    }
                    currentScreen = SCREEN_SHOW_ANSWER;
                    answerTimer = 2.0f;
                }
            } else if (currentScreen == SCREEN_SHOW_ANSWER) {
                answerTimer -= deltaTime;
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
            } else if (currentScreen == SCREEN_GAME_OVER) {
                 if (IsKeyPressed(KEY_ENTER)) {
                    currentScreen = SCREEN_LEADERBOARD;
                    isWaterAnimating = false; // Desativa a água ao sair do GAME_OVER
                    waterLevel = SCREEN_HEIGHT; // Reseta a água
                    waveOffset = 0.0f;
                 }
            }
        } break;
        default: break;
    }

    // Lógica de animação da água e onda
    if (isWaterAnimating) {
        if (waterLevel > TARGET_WATER_LEVEL) {
            waterLevel -= WATER_RISE_SPEED * deltaTime;
            if (waterLevel < TARGET_WATER_LEVEL) waterLevel = TARGET_WATER_LEVEL;
        }
        waveOffset += waveSpeed * deltaTime;
        if (waveOffset > SCREEN_WIDTH * 2) waveOffset = 0;

        dropSpawnTimer += deltaTime;
        if (dropSpawnTimer >= nextDropSpawnTime) {
            SpawnDrop();
            dropSpawnTimer = 0.0f;
            nextDropSpawnTime = GetRandomValue(DROP_SPAWN_INTERVAL_MIN * 100, DROP_SPAWN_INTERVAL_MAX * 100) / 100.0f;
        }

        for (int i = 0; i < MAX_DROPS; i++) {
            if (activeDrops[i].lifetime > 0.0f) {
                activeDrops[i].lifetime -= deltaTime;
            }
        }
        // <<< NOVO: Atualiza as gotas de chuva visuais >>>
        UpdateRaindrops(deltaTime);
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (isWaterAnimating) {
        // Desenha as gotas de chuva visuais ANTES da água, para que pareçam estar caindo sobre ela.
        DrawRaindrops(); // <<< NOVO: Desenha as gotas de chuva visuais >>>
        
        // Desenho da onda com triângulos
        const int segmentWidth = 5;
        for (int x = 0; x < SCREEN_WIDTH; x += segmentWidth)
        {
            float baseWaveY1 = sinf((x + waveOffset) * waveFrequency) * waveAmplitude;
            float baseWaveY2 = sinf((x + segmentWidth + waveOffset) * waveFrequency) * waveAmplitude;

            float dropWaveY1 = GetDropWaveContribution((float)x, currentTime);
            float dropWaveY2 = GetDropWaveContribution((float)x + segmentWidth, currentTime);

            Vector2 p1 = { (float)x, waterLevel + baseWaveY1 + dropWaveY1 };
            Vector2 p2 = { (float)x + segmentWidth, waterLevel + baseWaveY2 + dropWaveY2 };
            
            Vector2 p3 = { (float)x, (float)SCREEN_HEIGHT };
            Vector2 p4 = { (float)x + segmentWidth, (float)SCREEN_HEIGHT };
            
            DrawTriangle(p1, p3, p2, (Color){ 20, 80, 180, 200 });
            DrawTriangle(p2, p3, p4, (Color){ 20, 80, 180, 200 });
        }
    }

    // O restante da interface do jogo é desenhado SOBRE a água
    switch (currentScreen) {
        case SCREEN_MENU: {
            DrawTexture(texMenu, 0, 0, WHITE);
            Rectangle btnStart = { 691, 554, 538, 73 };
            Rectangle btnHowToPlay = { 691, 638, 538, 80 };
            Rectangle btnLeaderboard = { 691, 722, 538, 73 };
            Rectangle btnCredits = { 1607, 991, 270, 55 };

            if (CheckCollisionPointRec(mousePos, btnStart)) DrawRectangleLinesEx(btnStart, 4, BLUE);
            if (CheckCollisionPointRec(mousePos, btnHowToPlay)) DrawRectangleLinesEx(btnHowToPlay, 4, BLUE);
            if (CheckCollisionPointRec(mousePos, btnLeaderboard)) DrawRectangleLinesEx(btnLeaderboard, 4, BLUE);
            if (CheckCollisionPointRec(mousePos, btnCredits)) DrawRectangleLinesEx(btnCredits, 4, BLUE);
        } break;
        case SCREEN_HOW_TO_PLAY: {
            DrawTexture(texHowToPlay, 0, 0, WHITE);
            Rectangle btnBack = { 820, 911, 280, 70 };
            if (CheckCollisionPointRec(mousePos, btnBack)) DrawRectangleLinesEx(btnBack, 4, BLUE);
        } break;
        case SCREEN_CREDITS: {
            DrawTexture(texCredits, 0, 0, WHITE);
            Rectangle btnBack = { 820, 911, 280, 70 };
            if (CheckCollisionPointRec(mousePos, btnBack)) DrawRectangleLinesEx(btnBack, 4, BLUE);
        } break;
        case SCREEN_LEADERBOARD: {
            DrawTexture(texLeaderboard, 0, 0, WHITE);
            Rectangle btnBack = { 820, 911, 280, 70 };
            if (CheckCollisionPointRec(mousePos, btnBack)) DrawRectangleLinesEx(btnBack, 4, BLUE);
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
            if (selectedAnswer != -1) DrawRectangleLinesEx(optRects[selectedAnswer], 5, BLUE);
            if (currentScreen == SCREEN_SHOW_ANSWER) {
                DrawRectangleRec(optRects[q.correctOption], (Color){0, 228, 48, 100});
                if (!isAnswerCorrect) DrawRectangleRec(optRects[selectedAnswer], (Color){255, 0, 0, 100});
            }
            DrawText(TextFormat("Questao: %02d/%d", currentQuestionIndex + 1, QUIZ_QUESTIONS), 40, 30, 40, DARKBLUE);
            DrawText(TextFormat("Pontos: %03d", playerScore), 1650, 30, 40, DARKBLUE);
        } break;
        case SCREEN_GAME_OVER: {
            DrawText("FIM DE JOGO!", SCREEN_WIDTH/2 - MeasureText("FIM DE JOGO!", 80)/2, 350, 80, RAYWHITE);
            DrawText(TextFormat("Sua pontuacao final: %d", playerScore), SCREEN_WIDTH/2 - MeasureText(TextFormat("Sua pontuacao final: %d", playerScore), 50)/2, 500, 50, RAYWHITE);
            DrawText("Pressione ENTER para ver o placar", SCREEN_WIDTH/2 - MeasureText("Pressione ENTER para ver o placar", 30)/2, 700, 30, LIGHTGRAY);
        } break;
        default: break;
    }
    EndDrawing();
}