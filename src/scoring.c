/**
 * @file scoring.c
 * @author Grupo 1
 * @brief Implementação do módulo de Pontuação.
 * @version 1.0
 * @copyright Copyright (c) 2025
 */

#include "raylib/scoring.h"
#include <math.h> // Para garantir que a divisão seja float

//---------------------------------------------
// Variáveis Estáticas (Privadas ao Módulo)
//---------------------------------------------
static int currentPlayerScore = 0;

//---------------------------------------------
// Implementação das Funções Públicas
//---------------------------------------------

void ResetPlayerScore(void) {
    currentPlayerScore = 0;
}

int CalculatePointsEarned(Question q, float timeRemaining) {
    int basePoints = q.points;
    float timeFactor = timeRemaining / QUESTION_TIME; // Proporção do tempo restante
    
    // Garante que o fator de tempo não seja negativo se o timer estourar um pouco
    if (timeFactor < 0.0f) {
        timeFactor = 0.0f;
    }
    
    // Fórmula: Base * (1 + ProporçãoDoTempo)
    int pointsEarned = (int)(basePoints * (1.0f + timeFactor)); 
    
    return pointsEarned;
}

void AddToPlayerScore(int points) {
    currentPlayerScore += points;
}

int GetPlayerScore(void) {
    return currentPlayerScore;
}