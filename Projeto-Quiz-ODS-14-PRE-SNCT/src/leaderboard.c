/**
 * @file leaderboard.c
 * @author Grupo 1
 * @brief Implementação do módulo de Leaderboard (placar).
 * @version 1.0
 * @copyright Copyright (c) 2025
 */

#include "raylib/leaderboard.h"
#include <stdio.h>
#include <string.h>

//---------------------------------------------
// Variáveis Estáticas (Privadas ao Módulo)
//---------------------------------------------
static PlayerScore leaderboard[LEADERBOARD_SIZE];
static const char* LEADERBOARD_FILE = "leaderboard.dat";

//---------------------------------------------
// Funções Privadas
//---------------------------------------------
static void SaveLeaderboardToFile(void) {
    FILE *file = fopen(LEADERBOARD_FILE, "wb");
    if (file != NULL) {
        fwrite(leaderboard, sizeof(PlayerScore), LEADERBOARD_SIZE, file);
        fclose(file);
    }
}

static void LoadLeaderboardFromFile(void) {
    FILE *file = fopen(LEADERBOARD_FILE, "rb");
    if (file != NULL) {
        fread(leaderboard, sizeof(PlayerScore), LEADERBOARD_SIZE, file);
        fclose(file);
    } else {
        // Se o arquivo não existe, cria um placar padrão
        for (int i = 0; i < LEADERBOARD_SIZE; i++) {
            strcpy(leaderboard[i].name, "AAA");
            leaderboard[i].score = 0;
        }
        SaveLeaderboardToFile();
    }
}

//---------------------------------------------
// Implementação das Funções Públicas
//---------------------------------------------

void InitLeaderboard(void) {
    LoadLeaderboardFromFile();
}

const PlayerScore* GetLeaderboard(void) {
    return leaderboard;
}

void UpdateLeaderboard(const char* newName, int newScore) {
    int insertPosition = -1;

    // Verifica se a nova pontuação é alta o suficiente para entrar no placar
    if (newScore > leaderboard[LEADERBOARD_SIZE - 1].score) {
        // Encontra a posição correta para inserir a nova pontuação
        for (int i = 0; i < LEADERBOARD_SIZE; i++) {
            if (newScore > leaderboard[i].score) {
                insertPosition = i;
                break;
            }
        }

        if (insertPosition != -1) {
            // Move as pontuações mais baixas para baixo
            for (int i = LEADERBOARD_SIZE - 1; i > insertPosition; i--) {
                leaderboard[i] = leaderboard[i - 1];
            }
            
            // Insere a nova pontuação
            strcpy(leaderboard[insertPosition].name, newName);
            leaderboard[insertPosition].score = newScore;
            
            SaveLeaderboardToFile();
        }
    }
}