/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Main function and logic for the game. Simplified template for game
 * development in C using Raylib (https://www.raylib.com/).
 * 
 * @copyright Copyright (c) 2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*---------------------------------------------
 * Library headers.
 *-------------------------------------------*/
#include "raylib/raylib.h"
//#include "raylib/raymath.h"

/*---------------------------------------------
 * Project headers.
 *-------------------------------------------*/


/*---------------------------------------------
 * Macros. 
 *-------------------------------------------*/


/*--------------------------------------------
 * Constants. 
 *------------------------------------------*/


/*---------------------------------------------
 * Custom types (enums, structs, unions, etc.)
 *-------------------------------------------*/


/*---------------------------------------------
 * Global variables.
 *-------------------------------------------*/


/*---------------------------------------------
 * Function prototypes. 
 
 *-------------------------------------------*/

 // Structs for buttons
typedef struct {
    Rectangle rect;
    const char *text;
    Color color;
} Button;

// Function to draw buttons
void DrawButton(Button button) {
    DrawRectangleRec(button.rect, button.color);
    DrawRectangleLinesEx(button.rect, 2, BLACK);
    int textWidth = MeasureText(button.text, 20);
    DrawText(button.text, button.rect.x + (button.rect.width / 2) - (textWidth / 2), button.rect.y + (button.rect.height / 2) - 10, 20, DARKBLUE);
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void update( float delta );

/**
 * @brief Draws the state of the game.
 */
void draw( void );

/**
 * @brief Game entry point.
 */
int main( void ) {

    // local variables and initial user input

    // antialiasing
    SetConfigFlags( FLAG_MSAA_4X_HINT );

    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Quiz - Navegando pela ODS 14");
    SetTargetFPS(60);

    // Define colors
    Color lightBlue = {200, 220, 240, 255};
    Color buttonNormal = {190, 210, 230, 255};
    Color buttonHover = {150, 180, 210, 255};

    // Define rectangles for elements
    Rectangle titleRect = {screenWidth / 2 - 200, screenHeight / 2 - 150, 400, 50};
    Rectangle quizLogoRect = {screenWidth / 2 - 50, screenHeight / 2 - 220, 100, 100};
    Rectangle creditsRect = {screenWidth - 130, screenHeight - 40, 80, 20};

    // Define buttons
    Button startButton = {(Rectangle){screenWidth / 2 - 100, screenHeight / 2, 200, 40}, "Iniciar", buttonNormal};
    Button howToPlayButton = {(Rectangle){screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 40}, "Como jogar", buttonNormal};
    Button leaderBoardButton = {(Rectangle){screenWidth / 2 - 100, screenHeight / 2 + 100, 200, 40}, "LeaderBoard", buttonNormal};

    bool showCredits = false;

    while (!WindowShouldClose()) {
        // Update
        Vector2 mousePoint = GetMousePosition();

        // Check for button hover
        if (CheckCollisionPointRec(mousePoint, startButton.rect)) {
            startButton.color = buttonHover;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Action for Start button
                printf("Botão Iniciar clicado!\n");
            }
        } else {
            startButton.color = buttonNormal;
        }

        if (CheckCollisionPointRec(mousePoint, howToPlayButton.rect)) {
            howToPlayButton.color = buttonHover;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Action for How to Play button
                printf("Botão Como jogar clicado!\n");
            }
        } else {
            howToPlayButton.color = buttonNormal;
        }

        if (CheckCollisionPointRec(mousePoint, leaderBoardButton.rect)) {
            leaderBoardButton.color = buttonHover;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Action for LeaderBoard button
                printf("Botão LeaderBoard clicado!\n");
            }
        } else {
            leaderBoardButton.color = buttonNormal;
        }
        
        // Check for Credits button click
        if (CheckCollisionPointRec(mousePoint, creditsRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showCredits = true;
            printf("Botão Créditos clicado!\n");
        }

        // Draw
        BeginDrawing();
        ClearBackground(lightBlue);

        // Draw title and logo text
        DrawText("QUIZ", screenWidth / 2 - MeasureText("QUIZ", 40) / 2, screenHeight / 2 - 200, 40, DARKBLUE);
        DrawText("Navegando pela", screenWidth / 2 - MeasureText("Navegando pela", 30) / 2, screenHeight / 2 - 120, 30, DARKBLUE);
        DrawText("ODS 14", screenWidth / 2 - MeasureText("ODS 14", 40) / 2, screenHeight / 2 - 80, 40, DARKBLUE);
        
        // Draw buttons
        DrawButton(startButton);
        DrawButton(howToPlayButton);
        DrawButton(leaderBoardButton);

        // Draw Credits text
        DrawText("CRÉDITOS", creditsRect.x, creditsRect.y, 20, DARKBLUE);
        
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}