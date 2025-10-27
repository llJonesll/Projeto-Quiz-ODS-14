/**
 * @file water_fx.c
 * @author Grupo 1
 * @brief Implementação do módulo de Efeitos de Água (Water Effects).
 * @version 1.0
 * @copyright Copyright (c) 2025
 */

#include "raylib/water_fx.h"
#include <math.h>

//---------------------------------------------
// Definições e Constantes (Privadas ao Módulo)
//---------------------------------------------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define MAX_DROPS 3000
#define DROP_SPAWN_INTERVAL_MIN 0.0f
#define DROP_SPAWN_INTERVAL_MAX 0.1f
#define DROP_LIFETIME 1.3f
#define DROP_AMPLITUDE_MAX 9.0f
#define DROP_WAVE_SPEED 250.0f
#define DROP_WAVE_FREQUENCY 0.1f
#define MAX_RAINDROPS 3000
#define RAINDROP_SPEED_MIN 300
#define RAINDROP_SPEED_MAX 600
#define RAINDROP_LENGTH_MIN 15
#define RAINDROP_LENGTH_MAX 35

//---------------------------------------------
// Tipos (Privados ao Módulo)
//---------------------------------------------
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
// Variáveis Estáticas (Privadas ao Módulo)
//---------------------------------------------
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
static float waterRiseDelayTimer = 0.0f;

//---------------------------------------------
// Funções Privadas
//---------------------------------------------
static void InitDrops(void) {
    for (int i = 0; i < MAX_DROPS; i++) {
        activeDrops[i].lifetime = 0.0f;
    }
    nextDropSpawnTime = GetRandomValue(DROP_SPAWN_INTERVAL_MIN * 100, DROP_SPAWN_INTERVAL_MAX * 100) / 100.0f;
}

static void InitRaindrops(void) {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i].startPos = (Vector2){ (float)GetRandomValue(0, SCREEN_WIDTH), (float)GetRandomValue(-SCREEN_HEIGHT, 0) };
        raindrops[i].length = (float)GetRandomValue(RAINDROP_LENGTH_MIN, RAINDROP_LENGTH_MAX);
        raindrops[i].speed = (float)GetRandomValue(RAINDROP_SPEED_MIN, RAINDROP_SPEED_MAX);
    }
}

static void SpawnDropAt(float x, float amplitude) {
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

static void SpawnRandomDrop(void) {
    SpawnDropAt((float)GetRandomValue(0, SCREEN_WIDTH), (float)GetRandomValue(DROP_AMPLITUDE_MAX * 50, DROP_AMPLITUDE_MAX * 100) / 100.0f);
}

static float GetDropWaveContribution(float x, float currentTime) {
    float totalContribution = 0.0f;
    for (int i = 0; i < MAX_DROPS; i++) {
        if (activeDrops[i].lifetime > 0.0f) {
            float timeElapsed = currentTime - activeDrops[i].spawnTime;
            float distanceToDrop = fabsf(x - activeDrops[i].position.x);
            float waveRadius = timeElapsed * DROP_WAVE_SPEED;
            if (distanceToDrop < waveRadius) {
                float attenuationFactor = 1.0f - (distanceToDrop / waveRadius);
                float decayFactor = activeDrops[i].lifetime / DROP_LIFETIME;
                totalContribution += sinf(distanceToDrop * DROP_WAVE_FREQUENCY - timeElapsed * 15.0f) * activeDrops[i].initialAmplitude * attenuationFactor * decayFactor;
            }
        }
    }
    return totalContribution;
}

static void UpdateInternalRaindrops(float deltaTime) {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i].startPos.y += raindrops[i].speed * deltaTime;
        if (raindrops[i].startPos.y > waterLevel + 20) {
            raindrops[i].startPos = (Vector2){ (float)GetRandomValue(0, SCREEN_WIDTH), (float)GetRandomValue(-400, -50) };
        }
    }
}

static void DrawInternalWaterAndRain(float currentTime) {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        Vector2 endPos = { raindrops[i].startPos.x, raindrops[i].startPos.y + raindrops[i].length };
        if (endPos.y > waterLevel + 20) {
            endPos.y = waterLevel + 20;
        }
        if (raindrops[i].startPos.y < waterLevel + 20) {
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

//---------------------------------------------
// Funções Públicas do Módulo
//---------------------------------------------

void InitWaterFx(void) {
    waterLevel = SCREEN_HEIGHT;
    isWaterAnimating = false;
    InitDrops();
    InitRaindrops();
}

void ResetWaterFx(void) {
    isWaterAnimating = false;
    waterLevel = SCREEN_HEIGHT;
    waveOffset = 0.0f;
}

void StartWaterAnimation(void) {
    isWaterAnimating = true;
    waterLevel = SCREEN_HEIGHT;
    waterRiseDelayTimer = 2.0f;
    dropSpawnTimer = 0.0f;
    InitDrops();
    InitRaindrops();
    nextDropSpawnTime = GetRandomValue(DROP_SPAWN_INTERVAL_MIN * 100, DROP_SPAWN_INTERVAL_MAX * 100) / 100.0f;
}

void UpdateWaterFx(float deltaTime, float currentTime, Vector2 mousePos) {
    if (!isWaterAnimating) return;

    if (waterRiseDelayTimer > 0) {
        waterRiseDelayTimer -= deltaTime;
    }

    if (waterRiseDelayTimer <= 0 && waterLevel > TARGET_WATER_LEVEL) {
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
    UpdateInternalRaindrops(deltaTime);
    
    if (mouseSplashCooldown > 0) mouseSplashCooldown -= deltaTime;

    float baseWaveAtMouse = sinf((mousePos.x + waveOffset) * waveFrequency) * waveAmplitude;
    float dropRipplesAtMouse = GetDropWaveContribution(mousePos.x, currentTime);
    float waterSurfaceYAtMouse = waterLevel + baseWaveAtMouse + dropRipplesAtMouse;

    if (mousePos.y > waterSurfaceYAtMouse && mouseSplashCooldown <= 0.0f) {
        SpawnDropAt(mousePos.x, DROP_AMPLITUDE_MAX * 1.5f);
        mouseSplashCooldown = 0.1f;
    }
}

void DrawWaterFx(float currentTime) {
    if (isWaterAnimating) {
        DrawInternalWaterAndRain(currentTime);
    }
}

bool IsWaterAnimationFinished(void) {
    return (waterLevel <= TARGET_WATER_LEVEL);
}