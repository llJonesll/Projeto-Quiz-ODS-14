/**
 * @file water_fx.h
 * @author Grupo 1
 * @brief Interface para o módulo de Efeitos de Água (Water Effects).
 * @version 1.0
 * @copyright Copyright (c) 2025
 */

#ifndef WATER_FX_H
#define WATER_FX_H

#include "raylib/raylib.h"
#include <stdbool.h>

// Funções públicas do módulo de água
void InitWaterFx(void);
void UpdateWaterFx(float deltaTime, float currentTime, Vector2 mousePos);
void DrawWaterFx(float currentTime);
void StartWaterAnimation(void);
void ResetWaterFx(void);
bool IsWaterAnimationFinished(void);

#endif // WATER_FX_H