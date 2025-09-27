/**
 * @file music_player.c
 * @author Grupo 1
 * @brief Implementação do Music Player com UI animada para o Quiz ODS 14.
 * @version 3.0
 * @copyright Copyright (c) 2025
 *
 * @note Mudanças da v3.0:
 * - Implementada nova escala de volume: slider de 0-100% mapeado para volume de 0-150%.
 * - Volume inicial padrão aumentado para 100% (slider em 50%).
 * - Mantida a hitbox da barra de progresso do usuário, mas com a animação de entrada corrigida.
 */

 #include "raylib/raylib.h"
 #include "raylib/music_player.h"
 #include <stdlib.h>
 #include <stdio.h>
 #include <time.h>
 
 #define TOTAL_MUSICS 6
 #define ANIMATION_SPEED 0.35f
 
 //---------------------------------------------
 // Tipos e Estruturas
 //---------------------------------------------
 typedef enum {
     STATE_HIDDEN,
     STATE_ANIMATING_IN,
     STATE_SHOWN,
     STATE_ANIMATING_OUT
 } AnimationState;
 
 typedef struct {
     const char *title;
     const char *artist;
 } SongInfo;
 
 //---------------------------------------------
 // Variáveis Estáticas
 //---------------------------------------------
 static Music musicPlaylist[TOTAL_MUSICS];
 static SongInfo songInfo[TOTAL_MUSICS];
 static int playlistOrder[TOTAL_MUSICS - 1];
 static int currentMusicIndex = 0;
 static int currentPlaylistPosition = -1;
 
 static bool isMusicPaused = false;
 static float musicVolume = 0.5f; // Slider em 50% = volume real em 100%
 static float volumeBeforeMute = 0.5f;
 static bool isDraggingVolume = false;
 static bool isSeekingSong = false;
 static bool isClosePending = false;
 
 static Texture2D texMusicIcon, texMute, texUnmute, texPause, texPlay, texNext, texPrevious, texClose;
 
 static AnimationState playerState = STATE_HIDDEN;
 static AnimationState sliderState = STATE_HIDDEN;
 static float playerAnimationTimer = 0.0f;
 static float sliderAnimationTimer = 0.0f;
 
 // Protótipos de funções internas
 static void ShuffleIntArray(int *array, int size);
 static void PlayNextSong(void);
 static void PlayPreviousSong(void);
 static float Clamp(float value, float min, float max);
 static const char* FormatTime(float seconds);
 
 //---------------------------------------------
 // Implementação das Funções Públicas
 //---------------------------------------------
 
 void InitMusicPlayer(void) {
     musicPlaylist[0] = LoadMusicStream("resources/musics/bg_music.wav");
     musicPlaylist[1] = LoadMusicStream("resources/musics/bg_music2.mp3");
     musicPlaylist[2] = LoadMusicStream("resources/musics/bg_music3.mp3");
     musicPlaylist[3] = LoadMusicStream("resources/musics/bg_music4.mp3");
     musicPlaylist[4] = LoadMusicStream("resources/musics/bg_music5.mp3");
     musicPlaylist[5] = LoadMusicStream("resources/musics/bg_music6.mp3");
     songInfo[0] = (SongInfo){ "Sunken Heart City", "Cody O'Quinn" };
     songInfo[1] = (SongInfo){ "Farming Fever", "Cody O'Quinn" };
     songInfo[2] = (SongInfo){ "Vanilla Cola", "Cody O'Quinn" };
     songInfo[3] = (SongInfo){ "Gamer Instincts", "Cody O'Quinn" };
     songInfo[4] = (SongInfo){ "This Is Our Only Chance!", "Cody O'Quinn" };
     songInfo[5] = (SongInfo){ "Summy Dreams", "Cody O'Quinn" };
     texMusicIcon = LoadTexture("resources/images/music.png");
     texMute = LoadTexture("resources/images/mute.png");
     texUnmute = LoadTexture("resources/images/unmute.png");
     texPause = LoadTexture("resources/images/pause.png");
     texPlay = LoadTexture("resources/images/play.png");
     texNext = LoadTexture("resources/images/next.png");
     texPrevious = LoadTexture("resources/images/previous.png");
     texClose = LoadTexture("resources/images/x.png");
     for (int i = 0; i < TOTAL_MUSICS; i++) {
         musicPlaylist[i].looping = false;
     }
     for (int i = 0; i < TOTAL_MUSICS - 1; i++) {
         playlistOrder[i] = i + 1;
     }
     ShuffleIntArray(playlistOrder, TOTAL_MUSICS - 1);
     PlayMusicStream(musicPlaylist[currentMusicIndex]);
     UpdateMusicVolume();
 }
 
 void UpdateMusicPlayer(void) {
     float deltaTime = GetFrameTime();
     
     if (!isMusicPaused) {
         UpdateMusicStream(musicPlaylist[currentMusicIndex]);
     }
 
     if (!isMusicPaused && !IsMusicStreamPlaying(musicPlaylist[currentMusicIndex])) {
         PlayNextSong();
     }
     
     if (playerState == STATE_ANIMATING_IN) {
         playerAnimationTimer += deltaTime;
         if (playerAnimationTimer >= ANIMATION_SPEED) {
             playerAnimationTimer = ANIMATION_SPEED;
             playerState = STATE_SHOWN;
             if (sliderState == STATE_HIDDEN) {
                 sliderState = STATE_ANIMATING_IN;
             }
         }
     } else if (playerState == STATE_ANIMATING_OUT) {
         playerAnimationTimer -= deltaTime;
         if (sliderState != STATE_HIDDEN) {
             sliderState = STATE_ANIMATING_OUT;
         }
         if (playerAnimationTimer <= 0.0f) {
             playerAnimationTimer = 0.0f;
             playerState = STATE_HIDDEN;
         }
     }
     
     if (sliderState == STATE_ANIMATING_IN) {
         sliderAnimationTimer += deltaTime;
         if (sliderAnimationTimer >= ANIMATION_SPEED) {
             sliderAnimationTimer = ANIMATION_SPEED;
             sliderState = STATE_SHOWN;
         }
     } else if (sliderState == STATE_ANIMATING_OUT) {
         sliderAnimationTimer -= deltaTime;
         if (sliderAnimationTimer <= 0.0f) {
             sliderAnimationTimer = 0.0f;
             sliderState = STATE_HIDDEN;
             if (isClosePending) {
                 playerState = STATE_ANIMATING_OUT;
                 isClosePending = false;
             }
         }
     }
     
     Vector2 mousePos = GetMousePosition();
     float playerProgress = playerAnimationTimer / ANIMATION_SPEED;
     float locomotiveX = -60.0f + ((22.0f - -60.0f) * playerProgress);
     float togglerX = 22.0f + ((341.0f - 22.0f) * playerProgress);
     Rectangle musicIconRec = { 22, 952, 54, 54 };
     Rectangle closeRec = { togglerX, 952, 54, 54 };
     Rectangle muteRec = { locomotiveX, 952, 54, 54 };
     Rectangle prevRec = { locomotiveX + (115 - 22), 952, 54, 54 };
     Rectangle playPauseRec = { locomotiveX + (182 - 22), 952, 54, 54 };
     Rectangle nextRec = { locomotiveX + (248 - 22), 952, 54, 54 };
     float infoPanelFinalX = 121.0f;
     float infoPanelWidth = 405.0f - 121.0f;
     float infoPanelStartX = - (infoPanelWidth + 20);
     float infoPanelCurrentX = infoPanelStartX + (infoPanelFinalX - infoPanelStartX) * playerProgress;
     Rectangle infoPanelRec = { infoPanelCurrentX, 820, infoPanelWidth, 120 };
     Rectangle progressBarRec = { 132, 880, 263, 10 };
     float sliderProgress = sliderAnimationTimer / ANIMATION_SPEED;
     float sliderHeight = 140 * sliderProgress;
     Rectangle volumeSliderRec = { 44, 952 - sliderHeight - 10, 20, sliderHeight };
 
     if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
         if (playerState == STATE_HIDDEN && CheckCollisionPointRec(mousePos, musicIconRec)) {
             playerState = STATE_ANIMATING_IN;
         } else if (playerState != STATE_HIDDEN && CheckCollisionPointRec(mousePos, closeRec)) {
             if (sliderState != STATE_HIDDEN) {
                 isClosePending = true;
                 sliderState = STATE_ANIMATING_OUT;
             } else {
                 playerState = STATE_ANIMATING_OUT;
             }
         } else if (playerState == STATE_SHOWN) {
             bool actionTaken = false;
             if (CheckCollisionPointRec(mousePos, muteRec)) {
                 if (sliderState == STATE_SHOWN || sliderState == STATE_ANIMATING_IN) {
                     if (musicVolume > 0.01f) {
                         volumeBeforeMute = musicVolume;
                         musicVolume = 0.0f;
                     } else {
                         musicVolume = volumeBeforeMute;
                         if (musicVolume < 0.01f) {
                             musicVolume = 0.5f;
                         }
                     }
                     UpdateMusicVolume();
                 } else {
                     sliderState = STATE_ANIMATING_IN;
                 }
                 actionTaken = true;
             } else if (CheckCollisionPointRec(mousePos, playPauseRec)) {
                 isMusicPaused = !isMusicPaused;
                 if (isMusicPaused) {
                     PauseMusicStream(musicPlaylist[currentMusicIndex]);
                 } else {
                     ResumeMusicStream(musicPlaylist[currentMusicIndex]);
                 }
                 actionTaken = true;
             } else if (CheckCollisionPointRec(mousePos, nextRec)) {
                 PlayNextSong();
                 actionTaken = true;
             } else if (CheckCollisionPointRec(mousePos, prevRec)) {
                 PlayPreviousSong();
                 actionTaken = true;
             } else if (sliderState != STATE_HIDDEN && CheckCollisionPointRec(mousePos, volumeSliderRec)) {
                 isDraggingVolume = true;
                 actionTaken = true;
             } else if (CheckCollisionPointRec(mousePos, progressBarRec)) {
                 isSeekingSong = true;
                 actionTaken = true;
             }
 
             if (sliderState == STATE_SHOWN && !actionTaken) {
                 sliderState = STATE_ANIMATING_OUT;
             }
         }
     }
     
     if (isDraggingVolume) {
         musicVolume = 1.0f - ((mousePos.y - volumeSliderRec.y) / volumeSliderRec.height);
         musicVolume = Clamp(musicVolume, 0.0f, 1.0f);
         if (musicVolume > 0.01f) {
             volumeBeforeMute = musicVolume;
         }
         UpdateMusicVolume();
     }
     
     if (isSeekingSong) {
         float seekRatio = (mousePos.x - progressBarRec.x) / progressBarRec.width;
         seekRatio = Clamp(seekRatio, 0.0f, 1.0f);
         float seekTime = GetMusicTimeLength(musicPlaylist[currentMusicIndex]) * seekRatio;
         SeekMusicStream(musicPlaylist[currentMusicIndex], seekTime);
     }
     
     if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
         isDraggingVolume = false;
         isSeekingSong = false;
     }
 }
 
 void DrawMusicPlayer(void) {
     float playerProgress = playerAnimationTimer / ANIMATION_SPEED;
     float startX = -60.0f;
     float finalX_mute = 22.0f;
     float finalX_close = 341.0f;
     float locomotiveX = startX + (finalX_mute - startX) * playerProgress;
     float prevX = locomotiveX + (115 - 22);
     float playX = locomotiveX + (182 - 22);
     float nextX = locomotiveX + (248 - 22);
     float togglerX = finalX_mute + (finalX_close - finalX_mute) * playerProgress;
     
     const int numButtons = 4;
     float segmentDuration = 1.0f / numButtons;
     float alphaMute = Clamp(playerProgress / segmentDuration, 0.0f, 1.0f);
     float alphaPrev = Clamp((playerProgress - segmentDuration * 1) / segmentDuration, 0.0f, 1.0f);
     float alphaPlay = Clamp((playerProgress - segmentDuration * 2) / segmentDuration, 0.0f, 1.0f);
     float alphaNext = Clamp((playerProgress - segmentDuration * 3) / segmentDuration, 0.0f, 1.0f);
     
     if (playerState != STATE_HIDDEN) {
         float infoPanelFinalX = 121.0f;
         float infoPanelWidth = 405.0f - 121.0f;
         float infoPanelStartX = - (infoPanelWidth + 20);
         float infoPanelCurrentX = infoPanelStartX + (infoPanelFinalX - infoPanelStartX) * playerProgress;
         Rectangle infoPanelRec = { infoPanelCurrentX, 820, infoPanelWidth, 120 };
 
         DrawRectangleRec(infoPanelRec, Fade(BLACK, 0.7f * playerProgress));
         DrawText(songInfo[currentMusicIndex].title, infoPanelRec.x + 15, infoPanelRec.y + 10, 20, Fade(WHITE, playerProgress));
         DrawText(songInfo[currentMusicIndex].artist, infoPanelRec.x + 15, infoPanelRec.y + 35, 16, Fade(LIGHTGRAY, playerProgress));
         
         float songLength = GetMusicTimeLength(musicPlaylist[currentMusicIndex]);
         float songPlayed = GetMusicTimePlayed(musicPlaylist[currentMusicIndex]);
         float progress = (songLength > 0) ? songPlayed / songLength : 0;
         
         Rectangle progressBarRec = { infoPanelRec.x + (132 - 121), infoPanelRec.y + (890 - 820), 263, 7 };
         DrawRectangleRec(progressBarRec, Fade(GRAY, playerProgress));
         DrawRectangle(progressBarRec.x, progressBarRec.y, (int)(progressBarRec.width * progress), progressBarRec.height, Fade(WHITE, playerProgress));
         
         DrawText(FormatTime(songPlayed), progressBarRec.x, progressBarRec.y + 12, 12, Fade(LIGHTGRAY, playerProgress));
         const char* totalTimeText = FormatTime(songLength);
         int totalTimeWidth = MeasureText(totalTimeText, 12);
         DrawText(totalTimeText, progressBarRec.x + progressBarRec.width - totalTimeWidth, progressBarRec.y + 12, 12, Fade(LIGHTGRAY, playerProgress));
 
         DrawTexture(musicVolume <= 0.01f ? texMute : texUnmute, locomotiveX, 952, Fade(WHITE, alphaMute));
         DrawTexture(texPrevious, prevX, 952, Fade(WHITE, alphaPrev));
         DrawTexture(isMusicPaused ? texPlay : texPause, playX, 952, Fade(WHITE, alphaPlay));
         DrawTexture(texNext, nextX, 952, Fade(WHITE, alphaNext));
     }
     
     if (sliderState != STATE_HIDDEN) {
         float sliderProgress = sliderAnimationTimer / ANIMATION_SPEED;
         float sliderHeight = 140 * sliderProgress;
         Rectangle volumeSliderRec = { 44, 952 - sliderHeight - 10, 20, sliderHeight };
         
         DrawRectangleRounded(volumeSliderRec, 0.5f, 4, Fade(LIGHTGRAY, 0.7f * playerProgress));
         
         float filledHeight = sliderHeight * musicVolume;
         Rectangle filledRec = { volumeSliderRec.x, volumeSliderRec.y + (sliderHeight - filledHeight), volumeSliderRec.width, filledHeight };
         DrawRectangleRounded(filledRec, 0.5f, 4, Fade(YELLOW, playerProgress));
     }
     
     if (playerState == STATE_ANIMATING_OUT) {
         DrawTexture(texMusicIcon, togglerX, 952, WHITE);
     } else if (playerState == STATE_HIDDEN) {
         DrawTexture(texMusicIcon, 22, 952, WHITE);
     } else {
         DrawTexture(texClose, togglerX, 952, WHITE);
     }
 }
 
 
 void UnloadMusicPlayer(void) {
     for (int i = 0; i < TOTAL_MUSICS; i++) {
         UnloadMusicStream(musicPlaylist[i]);
     }
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
     float scaledVolume;
 
     if (musicVolume <= 0.5f) {
         scaledVolume = musicVolume * 2.0f;
     } else {
         scaledVolume = 1.0f + (musicVolume - 0.5f);
     }
 
     SetMusicVolume(musicPlaylist[currentMusicIndex], scaledVolume);
 }
 
 static void ShuffleIntArray(int *array, int size) {
     for (int i = size - 1; i > 0; i--) {
         int j = rand() % (i + 1);
         int temp = array[i];
         array[i] = array[j];
         array[j] = temp;
     }
 }
 
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
     if (isMusicPaused) {
         PauseMusicStream(musicPlaylist[currentMusicIndex]);
     }
 }
 
 static void PlayPreviousSong(void) {
     if (GetMusicTimePlayed(musicPlaylist[currentMusicIndex]) > 3.0f) {
         StopMusicStream(musicPlaylist[currentMusicIndex]);
         PlayMusicStream(musicPlaylist[currentMusicIndex]);
         UpdateMusicVolume();
         if (isMusicPaused) {
             PauseMusicStream(musicPlaylist[currentMusicIndex]);
         }
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
     if (isMusicPaused) {
         PauseMusicStream(musicPlaylist[currentMusicIndex]);
     }
 }
 
 static float Clamp(float value, float min, float max) {
     if (value < min) {
         return min;
     }
     if (value > max) {
         return max;
     }
     return value;
 }
 
 static const char* FormatTime(float seconds) {
     if (seconds < 0) {
         seconds = 0;
     }
     int minutes = (int)seconds / 60;
     int secs = (int)seconds % 60;
     static char timeText[6];
     sprintf(timeText, "%d:%02d", minutes, secs);
     return timeText;
 }