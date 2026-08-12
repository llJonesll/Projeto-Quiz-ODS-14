/**
 * @file music_player.h
 * @author Grupo 1
 * @brief Interface do Music Player para o Quiz ODS 14.
 * @version 1.1
 * @copyright Copyright (c) 2025
 */

#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include "raylib/raylib.h"

void InitMusicPlayer(void);
void UpdateMusicPlayer(void);
void UpdateMusicPlayerCustomMouse(Vector2 mousePos); // <<< Suporte a resolução adaptativa
void DrawMusicPlayer(void);
void UnloadMusicPlayer(void);
void UpdateMusicVolume(void); 

#endif // MUSIC_PLAYER_H