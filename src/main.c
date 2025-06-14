#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#define MAX_LONGUEUR_MOT 50
#define CAPACITE_DICT_INITIALE 100000
#define NIVEAU_MAX 10
#define LONGUEUR_MOT_MAX_REQUISE 6

typedef struct
{
    char **mots;
    int taille;
    int capacite;
} Dictionnaire;

typedef struct
{
    char nomUtilisateur[50];
    int niveauActuel;
} Utilisateur;

typedef struct
{
    int niveau;
    char caracteres[10];
    int nombreMotsRequises;
    int longueursMotsRequises[5];
} Niveau;

Font bellePolice;
Utilisateur utilisateurCourant;
Niveau niveauCourant;
char texteSaisi[100] = "";
bool estConnecte = false;
bool afficherEcranConnexion = true;
bool afficherEcranJeu = false;
bool afficherNiveauTermine = false;
bool afficherJeuTermine = false;
char message[100] = "";
char motsSaisis[10][MAX_LONGUEUR_MOT];
int compteMots = 0;
Dictionnaire dictionnaire = {0};

Texture2D bgLogin;
Texture2D bgGame;
Texture2D bgLevelComplete;
Texture2D bgGameComplete;

Color BACKGROUND_COLOR = {23, 42, 58, 255};
Color PRIMARY_COLOR = {62, 66, 72, 255};
Color SECONDARY_COLOR = {41, 128, 185, 255};
Color ACCENT_COLOR = {46, 204, 113, 255};
Color ERROR_COLOR = {231, 76, 60, 255};
Color TEXT_COLOR_LIGHT = WHITE;
Color TEXT_COLOR_DARK = {20, 20, 20, 255};

// Function prototypes
void InitialiserJeu();
void ChargerDonneesUtilisateur();
void SauvegarderDonneesUtilisateur();
void ChargerDonneesNiveau(int niveau);
void SauvegarderProgressionJeu();
void ChargerTextures();
void DechargerTextures();
bool ChargerDictionnaire(const char *fichier);
bool EstMotDansDictionnaire(const char *mot);
void LibererDictionnaire();
bool VerifierMot(const char *mot);
void DessinerEcranConnexion();
void DessinerEcranJeu();
void DessinerNiveauTermine();
void DessinerJeuTermine();
bool Button(Rectangle bounds, const char *text);
void FloatingEffect(float *y, float speed, float amplitude);
void HandleTextInput(char *buffer, int maxLength, Rectangle textBox);

// Level definition functions
void DefinirNiveau_1();
void DefinirNiveau_2();
void DefinirNiveau_3();
void DefinirNiveau_4();
void DefinirNiveau_5();
void DefinirNiveau_6();
void DefinirNiveau_7();
void DefinirNiveau_8();
void DefinirNiveau_9();
void DefinirNiveau_10();

void ChargerTextures()
{
    bgLogin = LoadTexture("assets/bg_login.png");
    bgGame = LoadTexture("assets/bg_game.png");
    bgLevelComplete = LoadTexture("assets/bg_level_complete.png");
    bgGameComplete = LoadTexture("assets/bg_game_complete.png");
}

void DechargerTextures()
{
    UnloadTexture(bgLogin);
    UnloadTexture(bgGame);
    UnloadTexture(bgLevelComplete);
    UnloadTexture(bgGameComplete);
}

bool Button(Rectangle bounds, const char *text)
{
    bool clicked = false;
    Vector2 mousePoint = GetMousePosition();
    Color btnColor = PRIMARY_COLOR;

    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        btnColor = ColorAlpha(PRIMARY_COLOR, 0.8f);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            clicked = true;
    }

    DrawRectangleRounded(bounds, 0.3f, 10, btnColor);
    int textWidth = MeasureText(text, 20);
    DrawText(text, bounds.x + (bounds.width - textWidth) / 2, bounds.y + (bounds.height - 20) / 2, 20, TEXT_COLOR_LIGHT);
    return clicked;
}

void FloatingEffect(float *y, float speed, float amplitude)
{
    *y += sinf(GetTime() * speed) * amplitude;
}

void HandleTextInput(char *buffer, int maxLength, Rectangle textBox)
{
    int key = GetCharPressed();
    while (key > 0)
    {
        if ((key >= 32) && (key <= 125) && (strlen(buffer) < maxLength - 1))
        {
            int length = strlen(buffer);
            buffer[length] = (char)key;
            buffer[length + 1] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        int length = strlen(buffer);
        if (length > 0)
        {
            buffer[length - 1] = '\0';
        }
    }
}

void DessinerEcranConnexion()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    DrawTexture(bgLogin, 0, 0, WHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BACKGROUND_COLOR, 0.5f));

    static float titleY = 100;
    FloatingEffect(&titleY, 2.0f, 2.0f);
    DrawText("WORD MASTER", (screenWidth - MeasureText("WORD MASTER", 40)) / 2, (int)titleY, 40, PRIMARY_COLOR);

    Rectangle inputBox = {(float)screenWidth / 2 - 200, 200, 400, 50};
    DrawRectangleRounded(inputBox, 0.2f, 10, ColorAlpha(TEXT_COLOR_LIGHT, 0.1f));
    DrawText(utilisateurCourant.nomUtilisateur, inputBox.x + 20, inputBox.y + 15, 20, TEXT_COLOR_LIGHT);
    HandleTextInput(utilisateurCourant.nomUtilisateur, 49, inputBox);

    Rectangle loginBtnRec = {(float)screenWidth / 2 - 210, 300, 200, 50};
    Rectangle regBtnRec = {(float)screenWidth / 2 + 10, 300, 200, 50};

    if (Button(loginBtnRec, "LOGIN"))
    {
        if (strlen(utilisateurCourant.nomUtilisateur) > 0)
            ChargerDonneesUtilisateur();
        else
            strcpy(message, "Please enter a username.");
    }

    if (Button(regBtnRec, "REGISTER"))
    {
        if (strlen(utilisateurCourant.nomUtilisateur) > 0)
            SauvegarderDonneesUtilisateur();
        else
            strcpy(message, "Please enter a username.");
    }

    if (strlen(message) > 0)
        DrawText(message, (screenWidth - MeasureText(message, 20)) / 2, 400, 20, ERROR_COLOR);
}

void DessinerEcranJeu()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    DrawTexture(bgGame, 0, 0, WHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BACKGROUND_COLOR, 0.3f));

    DrawText(TextFormat("LEVEL %d", niveauCourant.niveau), 20, 20, 30, PRIMARY_COLOR);
    DrawText(TextFormat("PLAYER: %s", utilisateurCourant.nomUtilisateur), 20, 60, 20, TEXT_COLOR_LIGHT);
    DrawText(TextFormat("Words Found: %d/%d", compteMots, niveauCourant.nombreMotsRequises), screenWidth - 250, 30, 20, ACCENT_COLOR);

    float startX = (float)screenWidth / 2 - (strlen(niveauCourant.caracteres) * 60.0f) / 2.0f;
    for (int i = 0; i < strlen(niveauCourant.caracteres); i++)
    {
        float yPos = 150;
        FloatingEffect(&yPos, 3.0f + i * 0.2f, 3.0f);

        float alpha = 150 + 50 * sinf(GetTime() * 2.0f + i);
        Color lightGray = (Color){57, 61, 66, (unsigned char)alpha};

        DrawRing((Vector2){startX + i * 60, yPos}, 20, 25, 0, 360, 36, lightGray);

        Vector2 pos = {startX + i * 60 - 10, yPos - 15};
        DrawTextEx(bellePolice, TextFormat("%c", niveauCourant.caracteres[i]), (Vector2){pos.x + 2, pos.y + 2}, 30, 2, (Color){0, 0, 0, 150});
        DrawTextEx(bellePolice, TextFormat("%c", niveauCourant.caracteres[i]), pos, 30, 2, TEXT_COLOR_LIGHT);
    }

    Rectangle inputBox = {(float)screenWidth / 2 - 200, 250, 400, 50};
    DrawRectangleRounded(inputBox, 0.2f, 10, ColorAlpha(TEXT_COLOR_LIGHT, 0.1f));
    DrawText(texteSaisi, inputBox.x + 20, inputBox.y + 15, 20, TEXT_COLOR_LIGHT);
    HandleTextInput(texteSaisi, 99, inputBox);

    if (Button((Rectangle){(float)screenWidth / 2 - 100, 320, 200, 50}, "SUBMIT") || (IsKeyPressed(KEY_ENTER) && strlen(texteSaisi) > 0))
    {
        if (VerifierMot(texteSaisi))
        {
            strcpy(motsSaisis[compteMots], texteSaisi);
            for (int k = 0; motsSaisis[compteMots][k]; k++)
                motsSaisis[compteMots][k] = toupper(motsSaisis[compteMots][k]);
            compteMots++;
            strcpy(message, "Word accepted!");

            if (compteMots >= niveauCourant.nombreMotsRequises)
            {
                utilisateurCourant.niveauActuel++;
                SauvegarderProgressionJeu();
                afficherEcranJeu = false;
                afficherNiveauTermine = utilisateurCourant.niveauActuel <= NIVEAU_MAX;
                afficherJeuTermine = !afficherNiveauTermine;
            }
        }
        texteSaisi[0] = '\0';
        while (GetCharPressed() > 0)
        {
        }
    }

    float yPos = 405;
    DrawText("FOUND WORDS:", (screenWidth - MeasureText("FOUND WORDS:", 20)) / 2, yPos - 30, 20, PRIMARY_COLOR);
    for (int i = 0; i < compteMots; i++)
    {
        DrawText(motsSaisis[i], (screenWidth - MeasureText(motsSaisis[i], 20)) / 2, (int)yPos, 20, ACCENT_COLOR);
        yPos += 30;
    }

    if (strlen(message) > 0)
    {
        Color msgColor = (strstr(message, "accepted")) ? ACCENT_COLOR : ERROR_COLOR;
        DrawText(message, (screenWidth - MeasureText(message, 20)) / 2, screenHeight - 50, 20, msgColor);
    }

    if (Button((Rectangle){(float)screenWidth - 220, (float)screenHeight - 70, 200, 50}, "SAVE & QUIT"))
    {
        SauvegarderProgressionJeu();
        InitialiserJeu();
    }
}

void DessinerNiveauTermine()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    DrawTexture(bgLevelComplete, 0, 0, WHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BACKGROUND_COLOR, 0.4f));

    char levelMsg[100];
    sprintf(levelMsg, "LEVEL %d COMPLETE!", niveauCourant.niveau - 1);
    DrawText(levelMsg, (screenWidth - MeasureText(levelMsg, 40)) / 2, screenHeight / 2 - 100, 40, ACCENT_COLOR);

    if (Button((Rectangle){(float)screenWidth / 2 - 100, (float)screenHeight / 2, 200, 50}, "NEXT LEVEL"))
    {
        ChargerDonneesNiveau(utilisateurCourant.niveauActuel);
        afficherNiveauTermine = false;
        afficherEcranJeu = true;
        strcpy(message, "");
    }
}

void DessinerJeuTermine()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    DrawTexture(bgGameComplete, 0, 0, WHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BACKGROUND_COLOR, 0.4f));

    DrawText("CONGRATULATIONS!", (screenWidth - MeasureText("CONGRATULATIONS!", 50)) / 2, screenHeight / 2 - 150, 50, ACCENT_COLOR);
    DrawText("You've mastered all the words!", (screenWidth - MeasureText("You've mastered all the words!", 30)) / 2, screenHeight / 2 - 80, 30, TEXT_COLOR_LIGHT);

    if (Button((Rectangle){(float)screenWidth / 2 - 100, (float)screenHeight / 2, 200, 50}, "PLAY AGAIN?"))
    {
        utilisateurCourant.niveauActuel = 1;
        SauvegarderProgressionJeu();
        ChargerDonneesNiveau(1);
        afficherJeuTermine = false;
        afficherEcranJeu = true;
        strcpy(message, "");
    }
    if (Button((Rectangle){(float)screenWidth / 2 - 100, (float)screenHeight / 2 + 70, 200, 50}, "MAIN MENU"))
        InitialiserJeu();
}

void InitialiserJeu()
{
    bellePolice = LoadFont("assets/fonts/Montserrat-Regular.ttf");
    if (bellePolice.texture.id == 0)
    {
        TraceLog(LOG_WARNING, "Failed to load font, using default");
        bellePolice = GetFontDefault();
    }
    afficherEcranConnexion = true;
    afficherEcranJeu = false;
    afficherNiveauTermine = false;
    afficherJeuTermine = false;
    texteSaisi[0] = '\0';
    message[0] = '\0';
    compteMots = 0;
    for (int i = 0; i < 10; i++)
        motsSaisis[i][0] = '\0';
}

bool ChargerDictionnaire(const char *fichier)
{
    FILE *f = fopen(fichier, "r");
    if (!f)
        return false;

    dictionnaire.mots = malloc(CAPACITE_DICT_INITIALE * sizeof(char *));
    dictionnaire.taille = 0;
    dictionnaire.capacite = CAPACITE_DICT_INITIALE;

    char buffer[MAX_LONGUEUR_MOT + 2];
    while (fgets(buffer, sizeof(buffer), f))
    {
        buffer[strcspn(buffer, "\r\n")] = 0;
        if (dictionnaire.taille >= dictionnaire.capacite)
        {
            dictionnaire.capacite *= 2;
            char **temp = realloc(dictionnaire.mots, dictionnaire.capacite * sizeof(char *));
            if (!temp)
            {
                LibererDictionnaire();
                fclose(f);
                return false;
            }
            dictionnaire.mots = temp;
        }
        dictionnaire.mots[dictionnaire.taille] = strdup(buffer);
        if (!dictionnaire.mots[dictionnaire.taille++])
        {
            LibererDictionnaire();
            fclose(f);
            return false;
        }
    }
    fclose(f);
    return true;
}

void LibererDictionnaire()
{
    if (dictionnaire.mots)
    {
        for (int i = 0; i < dictionnaire.taille; i++)
            free(dictionnaire.mots[i]);
        free(dictionnaire.mots);
        dictionnaire.mots = NULL;
    }
    dictionnaire.taille = 0;
    dictionnaire.capacite = 0;
}

bool EstMotDansDictionnaire(const char *mot)
{
    if (!dictionnaire.mots || dictionnaire.taille == 0)
        return false;

    char motUpper[MAX_LONGUEUR_MOT];
    strcpy(motUpper, mot);
    for (int i = 0; motUpper[i]; i++)
        motUpper[i] = toupper(motUpper[i]);

    for (int i = 0; i < dictionnaire.taille; i++)
    {
        char dictWordUpper[MAX_LONGUEUR_MOT];
        strcpy(dictWordUpper, dictionnaire.mots[i]);
        for (int k = 0; dictWordUpper[k]; k++)
            dictWordUpper[k] = toupper(dictWordUpper[k]);
        if (strcmp(motUpper, dictWordUpper) == 0)
            return true;
    }
    return false;
}

void SauvegarderDonneesUtilisateur()
{
#if defined(_WIN32)
    _mkdir("users");
#else
    mkdir("users", 0777);
#endif

    char filepath[100];
    sprintf(filepath, "users/%s.dat", utilisateurCourant.nomUtilisateur);
    FILE *f = fopen(filepath, "wb");
    if (!f)
        return;

    utilisateurCourant.niveauActuel = 1;
    fwrite(&utilisateurCourant, sizeof(Utilisateur), 1, f);
    fclose(f);
    estConnecte = true;
    afficherEcranConnexion = false;
    ChargerDonneesNiveau(1);
    afficherEcranJeu = true;
}

void ChargerDonneesUtilisateur()
{
    char filepath[100];
    sprintf(filepath, "users/%s.dat", utilisateurCourant.nomUtilisateur);
    FILE *f = fopen(filepath, "rb");
    if (!f)
        return;

    fread(&utilisateurCourant, sizeof(Utilisateur), 1, f);
    fclose(f);
    estConnecte = true;
    afficherEcranConnexion = false;
    ChargerDonneesNiveau(utilisateurCourant.niveauActuel);
    afficherEcranJeu = true;
}

void ChargerDonneesNiveau(int niveauNum)
{
    compteMots = 0;
    texteSaisi[0] = '\0';
    strcpy(message, "");

    if (niveauNum > NIVEAU_MAX)
    {
        afficherJeuTermine = true;
        return;
    }

    niveauCourant.niveau = niveauNum;
    switch (niveauNum)
    {
    case 1:
        DefinirNiveau_1();
        break;
    case 2:
        DefinirNiveau_2();
        break;
    case 3:
        DefinirNiveau_3();
        break;
    case 4:
        DefinirNiveau_4();
        break;
    case 5:
        DefinirNiveau_5();
        break;
    case 6:
        DefinirNiveau_6();
        break;
    case 7:
        DefinirNiveau_7();
        break;
    case 8:
        DefinirNiveau_8();
        break;
    case 9:
        DefinirNiveau_9();
        break;
    case 10:
        DefinirNiveau_10();
        break;
    default:
        DefinirNiveau_1();
    }
}

void SauvegarderProgressionJeu()
{
    if (!estConnecte)
        return;

    char filepath[100];
    sprintf(filepath, "users/%s.dat", utilisateurCourant.nomUtilisateur);
    FILE *f = fopen(filepath, "wb");
    if (!f)
        return;

    fwrite(&utilisateurCourant, sizeof(Utilisateur), 1, f);
    fclose(f);
}

bool VerifierMot(const char *mot)
{
    int len = strlen(mot);
    char motUpper[MAX_LONGUEUR_MOT];
    strcpy(motUpper, mot);
    for (int i = 0; motUpper[i]; i++)
        motUpper[i] = toupper(motUpper[i]);

    if (len < 2)
    {
        strcpy(message, "Word too short (min 2 letters).");
        return false;
    }

    bool longueurCorrecte = false;
    for (int i = 0; i < 5 && niveauCourant.longueursMotsRequises[i] > 0; i++)
    {
        if (len == niveauCourant.longueursMotsRequises[i])
        {
            longueurCorrecte = true;
            break;
        }
    }

    if (!longueurCorrecte)
    {
        char reqLengthsStr[50] = "Required lengths: ";
        for (int i = 0; i < 5 && niveauCourant.longueursMotsRequises[i] > 0; i++)
        {
            char numStr[5];
            sprintf(numStr, "%d", niveauCourant.longueursMotsRequises[i]);
            strcat(reqLengthsStr, numStr);
            if (i < 4 && niveauCourant.longueursMotsRequises[i + 1] > 0)
                strcat(reqLengthsStr, ", ");
        }
        sprintf(message, "Incorrect word length. %s", reqLengthsStr);
        return false;
    }

    char tempCaracteresDisponibles[11];
    strcpy(tempCaracteresDisponibles, niveauCourant.caracteres);
    for (int i = 0; i < len; i++)
    {
        char *foundChar = strchr(tempCaracteresDisponibles, motUpper[i]);
        if (!foundChar)
        {
            sprintf(message, "Letter '%c' not available.", motUpper[i]);
            return false;
        }
        *foundChar = '-';
    }

    for (int i = 0; i < compteMots; i++)
    {
        if (strcmp(motsSaisis[i], motUpper) == 0)
        {
            strcpy(message, "Word already found!");
            return false;
        }
    }

    if (!EstMotDansDictionnaire(motUpper))
    {
        strcpy(message, "Word not in dictionary.");
        return false;
    }

    return true;
}

// Level definitions
void DefinirNiveau_1()
{
    strcpy(niveauCourant.caracteres, "EARSTN");
    niveauCourant.nombreMotsRequises = 3;
    niveauCourant.longueursMotsRequises[0] = 3;
    niveauCourant.longueursMotsRequises[1] = 4;
    niveauCourant.longueursMotsRequises[2] = 5;
    niveauCourant.longueursMotsRequises[3] = 0;
    niveauCourant.longueursMotsRequises[4] = 0;
}

void DefinirNiveau_2()
{
    strcpy(niveauCourant.caracteres, "LAPRES");
    niveauCourant.nombreMotsRequises = 3;
    niveauCourant.longueursMotsRequises[0] = 4;
    niveauCourant.longueursMotsRequises[1] = 5;
    niveauCourant.longueursMotsRequises[2] = 6;
    niveauCourant.longueursMotsRequises[3] = 0;
    niveauCourant.longueursMotsRequises[4] = 0;
}

void DefinirNiveau_3()
{
    strcpy(niveauCourant.caracteres, "DOLENT");
    niveauCourant.nombreMotsRequises = 4;
    niveauCourant.longueursMotsRequises[0] = 4;
    niveauCourant.longueursMotsRequises[1] = 5;
    niveauCourant.longueursMotsRequises[2] = 6;
    niveauCourant.longueursMotsRequises[3] = 0;
    niveauCourant.longueursMotsRequises[4] = 0;
}

void DefinirNiveau_4()
{
    strcpy(niveauCourant.caracteres, "CHAMPS");
    niveauCourant.nombreMotsRequises = 4;
    niveauCourant.longueursMotsRequises[0] = 4;
    niveauCourant.longueursMotsRequises[1] = 5;
    niveauCourant.longueursMotsRequises[2] = 6;
    niveauCourant.longueursMotsRequises[3] = 0;
    niveauCourant.longueursMotsRequises[4] = 0;
}

void DefinirNiveau_5()
{
    strcpy(niveauCourant.caracteres, "QUENCHI");
    niveauCourant.nombreMotsRequises = 4;
    niveauCourant.longueursMotsRequises[0] = 5;
    niveauCourant.longueursMotsRequises[1] = 6;
    niveauCourant.longueursMotsRequises[2] = 7;
    niveauCourant.longueursMotsRequises[3] = 0;
    niveauCourant.longueursMotsRequises[4] = 0;
}

void DefinirNiveau_6()
{
    strcpy(niveauCourant.caracteres, "FLEURIT");
    niveauCourant.nombreMotsRequises = 5;
    niveauCourant.longueursMotsRequises[0] = 4;
    niveauCourant.longueursMotsRequises[1] = 5;
    niveauCourant.longueursMotsRequises[2] = 6;
    niveauCourant.longueursMotsRequises[3] = 7;
    niveauCourant.longueursMotsRequises[4] = 0;
}

void DefinirNiveau_7()
{
    strcpy(niveauCourant.caracteres, "TEGRAVI");
    niveauCourant.nombreMotsRequises = 5;
    niveauCourant.longueursMotsRequises[0] = 5;
    niveauCourant.longueursMotsRequises[1] = 6;
    niveauCourant.longueursMotsRequises[2] = 7;
    niveauCourant.longueursMotsRequises[3] = 0;
    niveauCourant.longueursMotsRequises[4] = 0;
}

void DefinirNiveau_8()
{
    strcpy(niveauCourant.caracteres, "VBRAINT");
    niveauCourant.nombreMotsRequises = 5;
    niveauCourant.longueursMotsRequises[0] = 5;
    niveauCourant.longueursMotsRequises[1] = 6;
    niveauCourant.longueursMotsRequises[2] = 7;
    niveauCourant.longueursMotsRequises[3] = 0;
    niveauCourant.longueursMotsRequises[4] = 0;
}

void DefinirNiveau_9()
{
    strcpy(niveauCourant.caracteres, "CREAFTI");
    niveauCourant.nombreMotsRequises = 6;
    niveauCourant.longueursMotsRequises[0] = 5;
    niveauCourant.longueursMotsRequises[1] = 6;
    niveauCourant.longueursMotsRequises[2] = 7;
    niveauCourant.longueursMotsRequises[3] = 0;
    niveauCourant.longueursMotsRequises[4] = 0;
}

void DefinirNiveau_10()
{
    strcpy(niveauCourant.caracteres, "ECLATANT");
    niveauCourant.nombreMotsRequises = 7;
    niveauCourant.longueursMotsRequises[0] = 5;
    niveauCourant.longueursMotsRequises[1] = 6;
    niveauCourant.longueursMotsRequises[2] = 7;
    niveauCourant.longueursMotsRequises[3] = 8;
    niveauCourant.longueursMotsRequises[4] = 0;
}

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Word Master");
    SetTargetFPS(60);

    ChargerTextures();

    if (!ChargerDictionnaire("words.txt"))
        TraceLog(LOG_ERROR, "Failed to load dictionary");

    InitialiserJeu();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        if (afficherEcranConnexion)
            DessinerEcranConnexion();
        else if (afficherNiveauTermine)
            DessinerNiveauTermine();
        else if (afficherJeuTermine)
            DessinerJeuTermine();
        else if (afficherEcranJeu)
            DessinerEcranJeu();
        else
            DrawText("Loading...", screenWidth / 2 - 50, screenHeight / 2 - 20, 20, TEXT_COLOR_LIGHT);

        EndDrawing();
    }

    DechargerTextures();
    LibererDictionnaire();
    CloseWindow();
    UnloadFont(bellePolice);

    return 0;
}