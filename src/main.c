/**
 * @file quiz_ods14.c
 * @author Gemini, com base no protótipo de Prof. Dr. David Buzatto
 * @brief Jogo de Quiz completo sobre a ODS 14 usando Raylib.
 * @version 2.6
 * @date 2025-09-24
 *
 * @copyright Copyright (c) 2025
 *
 * @note Mudanças da v2.6:
 * - Implementada pontuação diferenciada por dificuldade (Fácil: 10, Média: 25, Difícil: 50).
 * - Adicionada a exibição da dificuldade da pergunta (FÁCIL, MÉDIA, DIFÍCIL) com cores.
 * - Adicionada uma notificação visual de "+X PONTOS" que aparece por 2 segundos ao acertar
 * uma questão, com efeito de fade-out.
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

#define QUESTION_TIME 15.0f

// Definições para as ondulações na água
#define MAX_DROPS 60
#define DROP_SPAWN_INTERVAL_MIN 0.1f
#define DROP_SPAWN_INTERVAL_MAX 0.3f
#define DROP_LIFETIME 1.5f
#define DROP_AMPLITUDE_MAX 8.0f
#define DROP_WAVE_SPEED 250.0f
#define DROP_WAVE_FREQUENCY 0.08f

// Definições para as gotas de chuva visuais
#define MAX_RAINDROPS 500
#define RAINDROP_SPEED_MIN 300
#define RAINDROP_SPEED_MAX 600
#define RAINDROP_LENGTH_MIN 15
#define RAINDROP_LENGTH_MAX 35


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
    Vector2 position;
    float lifetime;
    float spawnTime;
    float initialAmplitude;
} Drop;

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
static const float WATER_RISE_SPEED = 110.0f;
static const float TARGET_WATER_LEVEL = 150.0f;
static float waveAmplitude = 10.0f;
static float waveFrequency = 0.005f;
static float waveSpeed = 80.0f;
static float waveOffset = 0.0f;

static Drop activeDrops[MAX_DROPS];
static float dropSpawnTimer = 0.0f;
static float nextDropSpawnTime = 0.0f;

static Raindrop raindrops[MAX_RAINDROPS];

static float mouseSplashCooldown = 0.0f;

static float questionTimer;

// <<< NOVAS: Variáveis para notificação de pontos >>>
static int pointsGainedNotification = 0;
static float notificationTimer = 0.0f;


//---------------------------------------------
// Protótipos de Funções
//---------------------------------------------
void UpdateDrawFrame(void);
void InitializeQuestions(void);
void LoadLeaderboard(void);

void InitDrops(void);
void SpawnRandomDrop(void);
void SpawnDropAt(float x, float amplitude);
float GetDropWaveContribution(float x, float currentTime);

void InitRaindrops(void);
void UpdateRaindrops(float deltaTime);
void DrawWaterAndRain(float currentTime);

void DrawTextWrappedCentered(const char *text, Rectangle rec, float fontSize, float spacing, Color color);


//---------------------------------------------
// Banco de Perguntas - ODS 14
//---------------------------------------------
void InitializeQuestions() {
    // Fáceis - 10 Pontos
    questions[0] = (Question){"Qual o principal objetivo da ODS 14: Vida na Agua?", {"Conservar e usar de forma sustentavel os oceanos e mares", "Aumentar a producao de peixes para alimentacao", "Promover o turismo em todas as areas costeiras", "Incentivar a extracao de petroleo no fundo do mar"}, 0, EASY, 10};
    questions[1] = (Question){"Qual material representa a maior parte do lixo encontrado nos oceanos?", {"Vidro", "Plastico", "Metal", "Papel"}, 1, EASY, 10};
    questions[2] = (Question){"O que causa o 'branqueamento' dos corais?", {"Excesso de peixes na regiao", "Sombras de barcos passando", "Aumento da temperatura da agua", "Falta de sal no mar"}, 2, EASY, 10};
    questions[3] = (Question){"Por que as tartarugas marinhas frequentemente comem sacolas plasticas?", {"Porque sao coloridas e chamativas", "Porque gostam do sabor do plastico", "Porque sentem o cheiro de comida nelas", "Porque confundem com aguas-vivas, seu alimento"}, 3, EASY, 10};
    questions[4] = (Question){"Qual a porcentagem aproximada da superficie da Terra que e coberta por oceanos?", {"Cerca de 70%", "Cerca de 50%", "Cerca de 90%", "Cerca de 30%"}, 0, EASY, 10};
    questions[5] = (Question){"Qual destes ecossistemas costeiros e um 'bercario' para muitas especies marinhas e protege a costa contra erosoes?", {"Praias rochosas", "Manguezais", "Dunas de areia", "Falesias"}, 1, EASY, 10};
    questions[6] = (Question){"De onde vem a maior parte do oxigenio que respiramos?", {"Das arvores da Amazonia", "De grandes plantacoes de soja", "Das algas e fitoplancton marinho", "Liberado por atividade vulcanica"}, 2, EASY, 10};
    questions[7] = (Question){"O que e a 'sobrepesca'?", {"Pescar apenas peixes muito grandes", "Usar barcos de pesca muito rapidos", "Pescar durante a noite", "Capturar peixes mais rapido do que eles conseguem se reproduzir"}, 3, EASY, 10};
    questions[8] = (Question){"O derramamento de qual substancia causa grandes desastres ambientais, matando aves e peixes?", {"Petroleo", "Areia", "Sal", "Açucar"}, 0, EASY, 10};
    questions[9] = (Question){"Qual a principal funcao das Areas Marinhas Protegidas (AMPs)?", {"Servir como rota para navios cargueiros", "Proteger a vida marinha e os habitats", "Ser um local exclusivo para esportes aquaticos", "Area de testes para equipamentos militares"}, 1, EASY, 10};

    // Médias - 25 Pontos
    questions[10] = (Question){"Qual fenomeno e causado pela absorcao de CO2 da atmosfera pelos oceanos, prejudicando organismos com conchas?", {"Eutrofizacao", "Salinizacao", "Acidificacao", "Termoclina"}, 2, MEDIUM, 25};
    questions[11] = (Question){"O que sao 'microplasticos'?", {"Pequenos aparelhos eletronicos descartados no mar", "Marcas de plastico biodegradavel", "Organismos que se alimentam de plastico", "Fragmentos de plastico com menos de 5mm"}, 3, MEDIUM, 25};
    questions[12] = (Question){"A 'pesca fantasma' se refere a:", {"Equipamentos de pesca perdidos que continuam a capturar animais", "Pescar em locais assombrados por lendas", "Um tipo de pesca ilegal feita a noite", "Usar iscas que brilham no escuro para atrair peixes"}, 0, MEDIUM, 25};
    questions[13] = (Question){"O que e 'eutrofizacao', um problema comum em areas costeiras?", {"A agua fica mais clara e transparente", "Excesso de nutrientes que causa proliferacao de algas e falta de oxigenio", "Aumento da temperatura media da agua", "Diminuicao da quantidade de sal na agua"}, 1, MEDIUM, 25};
    questions[14] = (Question){"Qual a importancia dos manguezais para o meio ambiente?", {"Servem de barreira de protecao e sao bercarios de especies", "Sao as principais fontes de areia para as praias", "Aumentam a forca das ondas na costa", "Sao locais ideais para a construcao de portos"}, 0, MEDIUM, 25};
    questions[15] = (Question){"Qual destes e um exemplo de especie invasora que causa problemas no Atlantico, incluindo no Brasil?", {"Peixe-palhaco", "Peixe-leao", "Tubarão-martelo", "Cavalo-marinho"}, 1, MEDIUM, 25};
    questions[16] = (Question){"A poluicao sonora nos oceanos, causada por navios e sonares, afeta principalmente qual capacidade das baleias e golfinhos?", {"Sua visao noturna", "Seu olfato para encontrar comida", "Sua comunicacao e localizacao", "Sua capacidade de flutuar"}, 2, MEDIUM, 25};
    questions[17] = (Question){"O que e 'carbono azul' (blue carbon)?", {"Um tipo raro de coral azul", "A poluicao de carbono emitida por navios", "Um combustivel fossil encontrado no fundo do mar", "Carbono capturado e armazenado por ecossistemas marinhos costeiros"}, 3, MEDIUM, 25};
    questions[18] = (Question){"A meta 14.1 da ODS 14 foca em reduzir qual tipo de problema ate 2025?", {"O numero de naufragios de navios", "A poluicao marinha de todos os tipos, especialmente de fontes terrestres", "O barulho gerado por turbinas eolicas no mar", "A quantidade de sal extraida para consumo"}, 1, MEDIUM, 25};
    questions[19] = (Question){"Por que o descongelamento das geleiras e uma ameaca aos oceanos?", {"Aumenta a salinidade da agua", "Cria novas rotas de navegacao", "Eleva o nivel do mar e altera as correntes marinhas", "Diminui a quantidade de peixes"}, 2, MEDIUM, 25};

    // Difíceis - 50 Pontos
    questions[20] = (Question){"O 'Giro do Pacifico Norte' e uma area oceanica conhecida por:", {"Ter as aguas mais quentes do planeta", "Ser a maior rota de migracao de baleias", "Ser o local de uma grande acumulacao de lixo plastico", "Possuir a maior quantidade de vulcoes submarinos ativos"}, 2, HARD, 50};
    questions[21] = (Question){"A Convencao das Nacoes Unidas sobre o Direito do Mar (UNCLOS) define a Zona Economica Exclusiva (ZEE) como uma faixa de ate:", {"500 milhas nauticas da costa", "12 milhas nauticas da costa", "50 milhas nauticas da costa", "200 milhas nauticas da costa"}, 3, HARD, 50};
    questions[22] = (Question){"O que e o 'Rendimento Maximo Sustentavel' (MSY) na gestao da pesca?", {"A maior quantidade de peixes que pode ser capturada sem esgotar o estoque", "O lucro maximo que uma empresa de pesca pode ter por lei", "O peso maximo que um unico barco de pesca pode transportar", "A velocidade maxima permitida para barcos de pesca em alto mar"}, 0, HARD, 50};
    questions[23] = (Question){"Qual organismo microscopico e a base da maior parte da cadeia alimentar marinha e produz oxigenio?", {"Krill", "Fitoplancton", "Zooplancton", "Bacterias marinhas"}, 1, HARD, 50};
    questions[24] = (Question){"A 'hipoxia', que cria as 'zonas mortas' no oceano, e uma condicao de:", {"Falta de luz solar no fundo do mar", "Agua com niveis extremamente baixos de oxigenio", "Excesso de sal na agua, tornando-a inabitavel", "Temperatura da agua abaixo de zero"}, 1, HARD, 50};
    questions[25] = (Question){"Qual a principal reacao quimica que descreve a acidificacao dos oceanos?", {"O2 + 2H2 -> 2H2O", "NaCl -> Na+ + Cl-", "CO2 + H2O -> H2CO3", "CaCO3 -> CaO + CO2"}, 2, HARD, 50};
    questions[26] = (Question){"A decada de 2021-2030 foi proclamada pela ONU como a Decada da:", {"Exploracao de Recursos Minerais Marinhos", "Protecao das Aves Marinhas", "Navegacao Sustentavel", "Ciencia Oceanica para o Desenvolvimento Sustentavel"}, 3, HARD, 50};
    questions[27] = (Question){"A 'biomagnificacao' e um processo perigoso onde:", {"Toxinas se acumulam em concentracoes maiores ao longo da cadeia alimentar", "Organismos marinhos crescem a um tamanho anormal", "A biodiversidade de uma area aumenta rapidamente", "A quantidade de sal aumenta em um organismo"}, 0, HARD, 50};
    questions[28] = (Question){"Qual destes subsidios a pesca a ODS 14.6 busca eliminar?", {"Subsidios para combustivel de pequenos pescadores", "Subsidios que contribuem para a sobrepesca e a pesca ilegal", "Financiamento para pesquisas sobre a vida marinha", "Ajuda de custo para a seguranca dos pescadores"}, 1, HARD, 50};
    questions[29] = (Question){"A 'termoclina' e uma camada no oceano onde ocorre uma rapida mudanca de:", {"Salinidade", "Pressao", "Temperatura", "Visibilidade"}, 2, HARD, 50};
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
    questionTimer = QUESTION_TIME;
}

// Funções para Ondulações (Drops)
void InitDrops(void) {
    for (int i = 0; i < MAX_DROPS; i++) {
        activeDrops[i].lifetime = 0.0f;
    }
    nextDropSpawnTime = GetRandomValue(DROP_SPAWN_INTERVAL_MIN * 100, DROP_SPAWN_INTERVAL_MAX * 100) / 100.0f;
}

void SpawnRandomDrop(void) {
    SpawnDropAt((float)GetRandomValue(0, SCREEN_WIDTH), (float)GetRandomValue(DROP_AMPLITUDE_MAX * 50, DROP_AMPLITUDE_MAX * 100) / 100.0f);
}

void SpawnDropAt(float x, float amplitude) {
    for (int i = 0; i < MAX_DROPS; i++) {
        if (activeDrops[i].lifetime <= 0.0f) {
            activeDrops[i].position.x = x;
            activeDrops[i].lifetime = DROP_LIFETIME;
            activeDrops[i].spawnTime = GetTime();
            activeDrops[i].initialAmplitude = amplitude;
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
            
            if (distanceToDrop < waveRadius) {
                float attenuationFactor = 1.0f - (distanceToDrop / waveRadius);
                float decayFactor = activeDrops[i].lifetime / DROP_LIFETIME;

                totalContribution += sinf(distanceToDrop * DROP_WAVE_FREQUENCY - timeElapsed * 15.0f) *
                                     activeDrops[i].initialAmplitude * attenuationFactor * decayFactor;
            }
        }
    }
    return totalContribution;
}

// Funções para Gotas de Chuva Visuais
void InitRaindrops(void) {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i].startPos = (Vector2){ (float)GetRandomValue(0, SCREEN_WIDTH), (float)GetRandomValue(-SCREEN_HEIGHT, 0) };
        raindrops[i].length = (float)GetRandomValue(RAINDROP_LENGTH_MIN, RAINDROP_LENGTH_MAX);
        raindrops[i].speed = (float)GetRandomValue(RAINDROP_SPEED_MIN, RAINDROP_SPEED_MAX);
    }
}

void UpdateRaindrops(float deltaTime) {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i].startPos.y += raindrops[i].speed * deltaTime;
        if (raindrops[i].startPos.y > waterLevel) {
            raindrops[i].startPos = (Vector2){ (float)GetRandomValue(0, SCREEN_WIDTH), (float)GetRandomValue(-400, -50) };
        }
    }
}

void DrawWaterAndRain(float currentTime) {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        Vector2 endPos = { raindrops[i].startPos.x, raindrops[i].startPos.y + raindrops[i].length };
        if (endPos.y > waterLevel) {
            endPos.y = waterLevel;
        }
        if (raindrops[i].startPos.y < waterLevel) {
            DrawLineEx(raindrops[i].startPos, endPos, 2.0f, (Color){ 200, 230, 255, 180 });
        }
    }
    
    const int segmentWidth = 5;
    for (int x = 0; x < SCREEN_WIDTH; x += segmentWidth) {
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

void DrawTextWrappedCentered(const char *text, Rectangle rec, float fontSize, float spacing, Color color) {
    char textToProcess[1024];
    strcpy(textToProcess, text);

    char lines[20][256];
    int lineCount = 0;
    
    char *textCopy = (char*)MemAlloc(strlen(textToProcess) + 1);
    strcpy(textCopy, textToProcess);
    
    char *word = strtok(textCopy, " ");
    char currentLine[256] = { 0 };
    
    while (word != NULL) {
        char testLine[256];
        strcpy(testLine, currentLine);
        if (strlen(currentLine) > 0) strcat(testLine, " ");
        strcat(testLine, word);

        if (MeasureText(testLine, fontSize) > rec.width) {
            strcpy(lines[lineCount], currentLine);
            lineCount++;
            
            strcpy(currentLine, word);
        } else {
            strcpy(currentLine, testLine);
        }
        word = strtok(NULL, " ");
    }
    
    strcpy(lines[lineCount], currentLine);
    lineCount++;
    MemFree(textCopy);

    float totalTextHeight = lineCount * fontSize + (lineCount - 1) * spacing;
    
    float startY = rec.y + (rec.height - totalTextHeight) / 2;

    for (int i = 0; i < lineCount; i++) {
        float lineWidth = MeasureText(lines[i], fontSize);
        float startX = rec.x + (rec.width - lineWidth) / 2;
        DrawText(lines[i], startX, startY + i * (fontSize + spacing), fontSize, color);
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
    InitRaindrops();

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
        ShowCursor();
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
                    isWaterAnimating = true;
                    waterLevel = SCREEN_HEIGHT;
                    playerName[0] = '\0';
                    nameCharCount = 0;
                    dropSpawnTimer = 0.0f;
                    InitDrops();
                    InitRaindrops();
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
                isWaterAnimating = false;
                waterLevel = SCREEN_HEIGHT;
                waveOffset = 0.0f;
            }
        } break;
        case SCREEN_ENTER_NAME:
        case SCREEN_GAMEPLAY:
        case SCREEN_SHOW_ANSWER:
        case SCREEN_GAME_OVER: {
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
                questionTimer -= deltaTime;
                if (questionTimer <= 0) {
                    isAnswerCorrect = false;
                    selectedAnswer = -1;
                    currentScreen = SCREEN_SHOW_ANSWER;
                    answerTimer = 2.0f;
                }

                if (IsKeyPressed(KEY_A)) selectedAnswer = 0;
                if (IsKeyPressed(KEY_B)) selectedAnswer = 1;
                if (IsKeyPressed(KEY_C)) selectedAnswer = 2;
                if (IsKeyPressed(KEY_D)) selectedAnswer = 3;
                if (IsKeyPressed(KEY_ENTER) && selectedAnswer != -1) {
                    int qIndex = questionOrder[currentQuestionIndex];
                    isAnswerCorrect = (selectedAnswer == questions[qIndex].correctOption);
                    if (isAnswerCorrect) {
                        playerScore += questions[qIndex].points;
                        // <<< NOVO: Ativa a notificação de pontos >>>
                        pointsGainedNotification = questions[qIndex].points;
                        notificationTimer = 2.0f; // Duração de 2 segundos
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
                        questionTimer = QUESTION_TIME;
                    }
                }
            } else if (currentScreen == SCREEN_GAME_OVER) {
                 if (IsKeyPressed(KEY_ENTER)) {
                    currentScreen = SCREEN_LEADERBOARD;
                    isWaterAnimating = false;
                    waterLevel = SCREEN_HEIGHT;
                    waveOffset = 0.0f;
                 }
            }
        } break;
        default: break;
    }

    if (isWaterAnimating) {
        if (waterLevel > TARGET_WATER_LEVEL) {
            waterLevel -= WATER_RISE_SPEED * deltaTime;
            if (waterLevel < TARGET_WATER_LEVEL) waterLevel = TARGET_WATER_LEVEL;
        }
        waveOffset += waveSpeed * deltaTime;
        if (waveOffset > SCREEN_WIDTH * 2) waveOffset = 0;

        dropSpawnTimer += deltaTime;
        if (dropSpawnTimer >= nextDropSpawnTime) {
            SpawnRandomDrop();
            dropSpawnTimer = 0.0f;
            nextDropSpawnTime = GetRandomValue(DROP_SPAWN_INTERVAL_MIN * 100, DROP_SPAWN_INTERVAL_MAX * 100) / 100.0f;
        }

        for (int i = 0; i < MAX_DROPS; i++) {
            if (activeDrops[i].lifetime > 0.0f) {
                activeDrops[i].lifetime -= deltaTime;
            }
        }
        UpdateRaindrops(deltaTime);
        
        if (mouseSplashCooldown > 0) mouseSplashCooldown -= deltaTime;

        // <<< NOVO: Lógica de atualização do timer da notificação >>>
        if (notificationTimer > 0) {
            notificationTimer -= deltaTime;
            if (notificationTimer <= 0) {
                pointsGainedNotification = 0; // Desativa a notificação quando o tempo acaba
            }
        }

        float baseWaveAtMouse = sinf((mousePos.x + waveOffset) * waveFrequency) * waveAmplitude;
        float dropRipplesAtMouse = GetDropWaveContribution(mousePos.x, currentTime);
        float waterSurfaceYAtMouse = waterLevel + baseWaveAtMouse + dropRipplesAtMouse;

        if (mousePos.y > waterSurfaceYAtMouse && mouseSplashCooldown <= 0.0f) {
            SpawnDropAt(mousePos.x, DROP_AMPLITUDE_MAX * 1.5f);
            mouseSplashCooldown = 0.1f;
        }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (currentScreen) {
        case SCREEN_MENU: {
            DrawTexture(texMenu, 0, 0, WHITE);
            Rectangle btnStart = { 691, 554, 538, 73 };
            Rectangle btnHowToPlay = { 691, 638, 538, 73 };
            Rectangle btnLeaderboard = { 691, 722, 538, 73 };
            Rectangle btnCredits = { 1612, 991, 259, 55 };

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
            if (isWaterAnimating) DrawWaterAndRain(currentTime);
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
            if (isWaterAnimating) DrawWaterAndRain(currentTime);
            
            if (currentScreen == SCREEN_GAMEPLAY) {
                float timerPercentage = questionTimer / QUESTION_TIME;
                if (timerPercentage < 0) timerPercentage = 0;
                float barWidth = 800;
                float barX = (SCREEN_WIDTH - barWidth) / 2;
                
                Color timerColor = (Color){ 0, 228, 48, 255 };
                if (timerPercentage < 0.5f) timerColor = (Color){ 253, 249, 0, 255 };
                if (timerPercentage < 0.25f) timerColor = (Color){ 255, 0, 0, 255 };

                DrawRectangle(barX, 80, barWidth, 30, Fade(DARKGRAY, 0.8f));
                DrawRectangle(barX, 80, barWidth * timerPercentage, 30, timerColor);
                DrawRectangleLines(barX, 80, barWidth, 30, DARKBLUE);
            }
            
            int qIndex = questionOrder[currentQuestionIndex];
            Question q = questions[qIndex];
            
            Rectangle questionRec = { 157, 243, 1595, 155 };
            DrawTextWrappedCentered(q.questionText, questionRec, 40, 5, WHITE);
            
            Rectangle optionTextRects[4] = {
                {215, 505, 500, 72},
                {1262, 506, 500, 72},
                {217, 726, 500, 72},
                {1258, 732, 500, 72}
            };

            const char* optionLetters[] = {"A", "B", "C", "D"};
            Vector2 letterPositions[] = {
                {157, 503}, {1204, 504}, {159, 724}, {1200, 730}
            };
            
            for (int i = 0; i < 4; i++) {
                Color letterColor = WHITE;

                if (currentScreen == SCREEN_GAMEPLAY) {
                    if (i == selectedAnswer) {
                        letterColor = YELLOW;
                    }
                } else if (currentScreen == SCREEN_SHOW_ANSWER) {
                    if (i == q.correctOption) {
                        letterColor = (Color){0, 228, 48, 255};
                    } 
                    else if (i == selectedAnswer && !isAnswerCorrect) {
                        letterColor = (Color){255, 0, 0, 255};
                    }
                }

                DrawText(optionLetters[i], letterPositions[i].x, letterPositions[i].y, 73, letterColor);
                DrawTextWrappedCentered(q.options[i], optionTextRects[i], 36, 3, WHITE);
            }
            
            // <<< NOVO: Desenho da exibição de dificuldade e notificação de pontos >>>
            const char* questionTextStr = TextFormat("Questao: %02d/%d", currentQuestionIndex + 1, QUIZ_QUESTIONS);
            int questionTextWidth = MeasureText(questionTextStr, 40);
            DrawText(questionTextStr, 40, 30, 40, DARKBLUE);

            const char* difficultyText;
            Color difficultyColor;
            switch(q.difficulty) {
                case EASY: difficultyText = "FACIL"; difficultyColor = GREEN; break;
                case MEDIUM: difficultyText = "MEDIA"; difficultyColor = YELLOW; break;
                case HARD: difficultyText = "DIFICIL"; difficultyColor = RED; break;
            }
            DrawText(difficultyText, 40, 85, 30, difficultyColor);
            
            DrawText(TextFormat("Pontos: %03d", playerScore), 1650, 30, 40, DARKBLUE);

            // Desenha a notificação de pontos ganhos com fade-out
            if (notificationTimer > 0) {
                const char* notificationText = TextFormat("+%d PONTOS", pointsGainedNotification);
                int notificationTextWidth = MeasureText(notificationText, 25);
                float alpha = notificationTimer / 2.0f; // Fade out ao longo de 2 segundos
                // Posição centralizada abaixo do placar
                DrawText(notificationText, 1650 + (MeasureText(TextFormat("Pontos: %03d", playerScore), 40) - notificationTextWidth) / 2, 30 + 45, 25, Fade(GREEN, alpha));
            }

        } break;
        case SCREEN_GAME_OVER: {
            if (isWaterAnimating) DrawWaterAndRain(currentTime);
            DrawText("FIM DE JOGO!", SCREEN_WIDTH/2 - MeasureText("FIM DE JOGO!", 80)/2, 350, 80, RAYWHITE);
            DrawText(TextFormat("Sua pontuacao final: %d", playerScore), SCREEN_WIDTH/2 - MeasureText(TextFormat("Sua pontuacao final: %d", playerScore), 50)/2, 500, 50, RAYWHITE);
            DrawText("Pressione ENTER para ver o placar", SCREEN_WIDTH/2 - MeasureText("Pressione ENTER para ver o placar", 30)/2, 700, 30, LIGHTGRAY);
        } break;
        default: break;
    }
    EndDrawing();
}