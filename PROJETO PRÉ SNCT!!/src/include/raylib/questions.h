/**
 * @file questions.h
 * @author Grupo 1
 * @brief Interface para o módulo do Banco de Questões.
 * @version 1.0
 * @copyright Copyright (c) 2025
 */

#ifndef QUESTIONS_H
#define QUESTIONS_H

#include "raylib/raylib.h"

//---------------------------------------------
// Definições e Tipos Públicos
//---------------------------------------------

#define QUIZ_QUESTION_COUNT 20 // Total de perguntas em um jogo

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

//---------------------------------------------
// Protótipos de Funções Públicas
//---------------------------------------------

// Carrega todas as perguntas na memória
void InitializeQuestions(void);

// Seleciona e embaralha as perguntas para um novo quiz
void SelectAndShuffleQuizQuestions(int *questionOrder);

// Retorna uma pergunta específica com base no índice da ordem do quiz
Question GetQuestionFromOrder(int orderIndex);

#endif // QUESTIONS_H