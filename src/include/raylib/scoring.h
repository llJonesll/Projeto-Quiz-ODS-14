/**
 * @file scoring.h
 * @author Grupo 1
 * @brief Interface para o módulo de Pontuação.
 * @version 1.0
 * @copyright Copyright (c) 2025
 */

#ifndef SCORING_H
#define SCORING_H

#include "raylib/questions.h" // Precisa conhecer a estrutura Question

// Constante de tempo usada no cálculo
#define QUESTION_TIME 15.0f

// Inicializa ou reseta a pontuação do jogador atual.
void ResetPlayerScore(void);

// Calcula os pontos ganhos para uma questão com base no tempo restante.
int CalculatePointsEarned(Question q, float timeRemaining);

// Adiciona os pontos calculados à pontuação total do jogador.
void AddToPlayerScore(int points);

// Retorna a pontuação atual do jogador.
int GetPlayerScore(void);

#endif // SCORING_H