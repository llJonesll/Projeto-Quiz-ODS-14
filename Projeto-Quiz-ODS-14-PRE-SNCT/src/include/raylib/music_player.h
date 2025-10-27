/**
 * @file music_player.h
 * @author Grupo 1
 * @brief Interface do Music Player para o Quiz ODS 14.
 * @version 1.0
 * * @copyright Copyright (c) 2025
 */

#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

// Funções que o arquivo principal poderá chamar
void InitMusicPlayer(void);
void UpdateMusicPlayer(void);
void DrawMusicPlayer(void);
void UnloadMusicPlayer(void);
void UpdateMusicVolume(void); // Função para garantir que o volume seja aplicado

#endif // MUSIC_PLAYER_H