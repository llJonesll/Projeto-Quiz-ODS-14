/**
 * @file leaderboard.h
 * @author Grupo 1
 * @brief Interface para o módulo de Leaderboard (placar).
 * @version 1.0
 * @copyright Copyright (c) 2025
 */

#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#define LEADERBOARD_SIZE 6
#define MAX_NAME_LENGTH 3

typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    int score;
} PlayerScore;

// Carrega o placar do arquivo ou cria um novo se não existir.
void InitLeaderboard(void);

// Tenta adicionar um novo placar, reordena e salva em arquivo.
void UpdateLeaderboard(const char* newName, int newScore);

// Retorna um ponteiro constante para os dados do placar para desenho.
const PlayerScore* GetLeaderboard(void);

int GetPlayerRank(int finalScore);
#endif // LEADERBOARD_H