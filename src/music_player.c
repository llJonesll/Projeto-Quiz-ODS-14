/**
 * @file music_player.c
 * @author Grupo 1
 * @brief Implementação do Music Player com UI animada para o Quiz ODS 14.
 * @version 1.0
 * * @copyright Copyright (c) 2025
 */

#include "raylib/raylib.h"
#include "raylib/music_player.h"
#include <stdlib.h> // Para rand() e srand()
#include <time.h>   // Para time()

#define TOTAL_MUSICS 6
#define ANIMATION_SPEED 0.3f // Duração da animação em segundos

//---------------------------------------------
// Tipos e Estados Internos do Player
//---------------------------------------------
typedef enum {
    PLAYER_HIDDEN,
    PLAYER_ANIMATING_IN,
    PLAYER_SHOWN,
    PLAYER_ANIMATING_OUT
} PlayerState;

//---------------------------------------------
// Variáveis Estáticas (visíveis apenas neste arquivo)
//---------------------------------------------
static Music musicPlaylist[TOTAL_MUSICS];
static int playlistOrder[TOTAL_MUSICS - 1];
static int currentMusicIndex = 0;
static int currentPlaylistPosition = -1;

static bool isMusicPaused = false;
static float musicVolume = 0.5f;
static bool isDraggingVolume = false;

static Texture2D texMusicIcon, texMute, texUnmute, texPause, texPlay, texNext, texPrevious, texClose;

static PlayerState playerState = PLAYER_HIDDEN;
static float animationTimer = 0.0f;

// Funções auxiliares internas
static void ShuffleIntArray(int *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

static void PlayNextSong(void);
static void PlayPreviousSong(void);

//---------------------------------------------
// Implementação das Funções Públicas
//---------------------------------------------

void InitMusicPlayer(void) {
    // Carrega Músicas e UI do Player
    musicPlaylist[0] = LoadMusicStream("resources/musics/bg_music.wav");
    musicPlaylist[1] = LoadMusicStream("resources/musics/bg_music2.mp3");
    musicPlaylist[2] = LoadMusicStream("resources/musics/bg_music3.mp3");
    musicPlaylist[3] = LoadMusicStream("resources/musics/bg_music4.mp3");
    musicPlaylist[4] = LoadMusicStream("resources/musics/bg_music5.mp3");
    musicPlaylist[5] = LoadMusicStream("resources/musics/bg_music6.mp3");
    
    texMusicIcon = LoadTexture("resources/images/music.png");
    texMute = LoadTexture("resources/images/mute.png");
    texUnmute = LoadTexture("resources/images/unmute.png");
    texPause = LoadTexture("resources/images/pause.png");
    texPlay = LoadTexture("resources/images/play.png");
    texNext = LoadTexture("resources/images/next.png");
    texPrevious = LoadTexture("resources/images/previous.png");
    texClose = LoadTexture("resources/images/x.png");

    for (int i = 0; i < TOTAL_MUSICS; i++) musicPlaylist[i].looping = false;
    for (int i = 0; i < TOTAL_MUSICS - 1; i++) playlistOrder[i] = i + 1;
    
    ShuffleIntArray(playlistOrder, TOTAL_MUSICS - 1); // Embaralha a playlist secundária
    
    PlayMusicStream(musicPlaylist[currentMusicIndex]);
    SetMusicVolume(musicPlaylist[currentMusicIndex], musicVolume);
}

void UpdateMusicPlayer(void) {
    float deltaTime = GetFrameTime();
    
    if (!isMusicPaused) {
        UpdateMusicStream(musicPlaylist[currentMusicIndex]);
    }

    if (GetMusicTimePlayed(musicPlaylist[currentMusicIndex]) >= GetMusicTimeLength(musicPlaylist[currentMusicIndex])) {
        PlayNextSong();
    }
    
    // Lógica da animação
    if (playerState == PLAYER_ANIMATING_IN) {
        animationTimer += deltaTime;
        if (animationTimer >= ANIMATION_SPEED) {
            animationTimer = ANIMATION_SPEED;
            playerState = PLAYER_SHOWN;
        }
    } else if (playerState == PLAYER_ANIMATING_OUT) {
        animationTimer -= deltaTime;
        if (animationTimer <= 0.0f) {
            animationTimer = 0.0f;
            playerState = PLAYER_HIDDEN;
        }
    }
    
    // Lógica de Input
    Vector2 mousePos = GetMousePosition();

    // Posições Finais dos Botões
    Rectangle finalMusicIconRec = { 22, 952, 54, 54 };
    Rectangle finalCloseRec = { 341, 952, 54, 54 };
    
    Rectangle currentTogglerRec = (playerState == PLAYER_HIDDEN || playerState == PLAYER_ANIMATING_OUT) ? finalMusicIconRec : finalCloseRec;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, currentTogglerRec)) {
        if (playerState == PLAYER_HIDDEN || playerState == PLAYER_ANIMATING_OUT) {
            playerState = PLAYER_ANIMATING_IN;
        } else {
            playerState = PLAYER_ANIMATING_OUT;
        }
    }
    
    if (playerState == PLAYER_SHOWN || playerState == PLAYER_ANIMATING_IN) {
        // Hitboxes dos botões do player
        Rectangle muteRec = { 22, 952, 54, 54 };
        Rectangle prevRec = { 115, 952, 54, 54 };
        Rectangle playPauseRec = { 182, 952, 54, 54 };
        Rectangle nextRec = { 248, 952, 54, 54 };
        Rectangle volumeSliderRec = { 44, 800, 20, 140 }; // Agora vertical

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePos, playPauseRec)) {
                isMusicPaused = !isMusicPaused;
                if (isMusicPaused) PauseMusicStream(musicPlaylist[currentMusicIndex]);
                else ResumeMusicStream(musicPlaylist[currentMusicIndex]);
            } else if (CheckCollisionPointRec(mousePos, nextRec)) {
                PlayNextSong();
            } else if (CheckCollisionPointRec(mousePos, prevRec)) {
                PlayPreviousSong();
            } else if (CheckCollisionPointRec(mousePos, muteRec)) {
                musicVolume = (musicVolume > 0.0f) ? 0.0f : 0.5f;
                SetMusicVolume(musicPlaylist[currentMusicIndex], musicVolume);
            } else if (CheckCollisionPointRec(mousePos, volumeSliderRec)) {
                isDraggingVolume = true;
            }
        }
        
        if (isDraggingVolume) {
            // Lógica para slider vertical
            musicVolume = 1.0f - ((mousePos.y - volumeSliderRec.y) / volumeSliderRec.height);
            if (musicVolume < 0.0f) musicVolume = 0.0f;
            if (musicVolume > 1.0f) musicVolume = 1.0f;
            SetMusicVolume(musicPlaylist[currentMusicIndex], musicVolume);
        }
        
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            isDraggingVolume = false;
        }
    }
}

void DrawMusicPlayer(void) {
    float animationProgress = animationTimer / ANIMATION_SPEED;

    // Posições de início e fim da animação
    float startX_mute = -60;
    float finalX_mute = 22;
    float startX_prev = -60;
    float finalX_prev = 115;
    float startX_play = -60;
    float finalX_play = 182;
    float startX_next = -60;
    float finalX_next = 248;

    // Interpolação linear para a posição X dos botões
    float muteX = startX_mute + (finalX_mute - startX_mute) * animationProgress;
    float prevX = startX_prev + (finalX_prev - startX_prev) * animationProgress;
    float playX = startX_play + (finalX_play - startX_play) * animationProgress;
    float nextX = startX_next + (finalX_next - startX_next) * animationProgress;

    // Desenha os botões se o player não estiver completamente escondido
    if (playerState != PLAYER_HIDDEN) {
        // Botão Mute/Unmute
        DrawTexture(musicVolume <= 0.01f ? texMute : texUnmute, muteX, 952, WHITE);
        
        // Botão Previous
        DrawTexture(texPrevious, prevX, 952, WHITE);
        
        // Botão Play/Pause
        DrawTexture(isMusicPaused ? texPlay : texPause, playX, 952, WHITE);
        
        // Botão Next
        DrawTexture(texNext, nextX, 952, WHITE);
        
        // Slider de Volume Vertical
        Rectangle volumeSliderRec = { 44, 800, 20, 140 };
        DrawRectangleRec(volumeSliderRec, Fade(LIGHTGRAY, 0.7f * animationProgress));
        DrawRectangle(volumeSliderRec.x, volumeSliderRec.y + (int)(volumeSliderRec.height * (1.0f - musicVolume)), volumeSliderRec.width, (int)(volumeSliderRec.height * musicVolume), Fade(BLUE, animationProgress));
    }
    
    // Desenha o botão principal (Music ou X)
    if (playerState == PLAYER_HIDDEN || playerState == PLAYER_ANIMATING_OUT) {
        DrawTexture(texMusicIcon, 22, 952, WHITE);
    } else {
        DrawTexture(texClose, 341, 952, WHITE);
    }
}

void UnloadMusicPlayer(void) {
    for (int i = 0; i < TOTAL_MUSICS; i++) UnloadMusicStream(musicPlaylist[i]);
    UnloadTexture(texMusicIcon);
    UnloadTexture(texMute);
    UnloadTexture(texUnmute);
    UnloadTexture(texPause);
    UnloadTexture(texPlay);
    UnloadTexture(texNext);
    UnloadTexture(texPrevious);
    UnloadTexture(texClose);
}

void UpdateMusicVolume(void) {
    SetMusicVolume(musicPlaylist[currentMusicIndex], musicVolume);
}

// Implementação das funções de controle da playlist
static void PlayNextSong(void) {
    StopMusicStream(musicPlaylist[currentMusicIndex]);

    if (currentPlaylistPosition == -1) {
        ShuffleIntArray(playlistOrder, TOTAL_MUSICS - 1);
        currentPlaylistPosition = 0;
    } else {
        currentPlaylistPosition++;
        if (currentPlaylistPosition >= TOTAL_MUSICS - 1) {
            ShuffleIntArray(playlistOrder, TOTAL_MUSICS - 1);
            currentPlaylistPosition = 0;
        }
    }
    
    currentMusicIndex = playlistOrder[currentPlaylistPosition];
    PlayMusicStream(musicPlaylist[currentMusicIndex]);
    UpdateMusicVolume();
    if (isMusicPaused) PauseMusicStream(musicPlaylist[currentMusicIndex]);
}

static void PlayPreviousSong(void) {
    if (GetMusicTimePlayed(musicPlaylist[currentMusicIndex]) > 3.0f) {
        StopMusicStream(musicPlaylist[currentMusicIndex]);
        PlayMusicStream(musicPlaylist[currentMusicIndex]);
        UpdateMusicVolume();
        if (isMusicPaused) PauseMusicStream(musicPlaylist[currentMusicIndex]);
        return;
    }

    StopMusicStream(musicPlaylist[currentMusicIndex]);

    if (currentPlaylistPosition <= 0) {
        currentPlaylistPosition = -1;
        currentMusicIndex = 0;
    } else {
        currentPlaylistPosition--;
        currentMusicIndex = playlistOrder[currentPlaylistPosition];
    }
    
    PlayMusicStream(musicPlaylist[currentMusicIndex]);
    UpdateMusicVolume();
    if (isMusicPaused) PauseMusicStream(musicPlaylist[currentMusicIndex]);
}