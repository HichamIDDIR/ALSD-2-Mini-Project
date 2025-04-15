#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Platform-specific headers for mkdir
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

// Dictionary configuration
#define MAX_WORD_LENGTH 50
#define INITIAL_DICT_CAPACITY 1000
#define MAX_LEVEL 10
#define MAX_WORD_LENGTH_REQUIRED 6  // Maximum word length required in any level

typedef struct {
    char** words;
    int size;
    int capacity;
} Dictionary;

typedef struct {
    char username[50];
    int currentLevel;
} User;

typedef struct {
    int level;
    char characters[10];
    int requiredWordCount;
    int requiredWordLengths[5];
} Level;

// Global variables
User currentUser;
Level currentLevel;
char inputText[100] = "";
bool isLoggedIn = false;
bool showLoginScreen = true;
bool showGameScreen = false;
bool showLevelComplete = false;
bool showGameOver = false;
char message[100] = "";
char enteredWords[10][50];
int wordCount = 0;
Dictionary dictionary = {0};

// Function declarations
void InitGame();
void LoadUserData();
void SaveUserData();
void LoadLevelData(int level);
void SaveGameProgress();
bool CheckWord(const char* word);
void DrawLoginScreen();
void DrawGameScreen();
void DrawLevelCompleteScreen();
void DrawGameOverScreen();
void ProcessGameLogic();
bool LoadDictionary(const char* filename);
bool IsWordInDictionary(const char* word);
void FreeDictionary();

// Level definitions
void Level_1();
void Level_2();
void Level_3();
void Level_4();
void Level_5();
void Level_6();
void Level_7();
void Level_8();
void Level_9();
void Level_10();

int main(void) {
    InitWindow(800, 600, "Word Game");
    SetTargetFPS(60);
    
    if (!LoadDictionary("words.txt")) {
        TraceLog(LOG_WARNING, "Failed to load dictionary file");
        dictionary.capacity = INITIAL_DICT_CAPACITY;
        dictionary.words = (char**)malloc(dictionary.capacity * sizeof(char*));
        
        const char* defaultWords[] = {"CAT", "DOG", "SUN", "MOON", "STAR", "TREE", "BOOK", "BALL", "FISH", "BIRD", "GOD"};
        dictionary.size = sizeof(defaultWords)/sizeof(defaultWords[0]);
        
        for (int i = 0; i < dictionary.size; i++) {
            dictionary.words[i] = strdup(defaultWords[i]);
        }
    }
    
    InitGame();
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        if (showLoginScreen) {
            DrawLoginScreen();
        } else if (showLevelComplete) {
            DrawLevelCompleteScreen();
        } else if (showGameOver) {
            DrawGameOverScreen();
        } else if (showGameScreen) {
            DrawGameScreen();
            ProcessGameLogic();
        }
        
        EndDrawing();
    }
    
    FreeDictionary();
    CloseWindow();
    return 0;
}

bool LoadDictionary(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return false;
    
    char buffer[MAX_WORD_LENGTH];
    dictionary.capacity = INITIAL_DICT_CAPACITY;
    dictionary.words = (char**)malloc(dictionary.capacity * sizeof(char*));
    dictionary.size = 0;
    
    while (fgets(buffer, MAX_WORD_LENGTH, file)) {
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
            buffer[--len] = '\0';
        }
        
        if (len < 2 || len > MAX_WORD_LENGTH_REQUIRED) continue;
        
        for (char* p = buffer; *p; p++) *p = toupper(*p);
        
        bool exists = false;
        for (int i = 0; i < dictionary.size; i++) {
            if (strcmp(dictionary.words[i], buffer) == 0) {
                exists = true;
                break;
            }
        }
        if (exists) continue;
        
        if (dictionary.size >= dictionary.capacity) {
            dictionary.capacity *= 2;
            char** new_words = (char**)realloc(dictionary.words, dictionary.capacity * sizeof(char*));
            if (!new_words) {
                fclose(file);
                return false;
            }
            dictionary.words = new_words;
        }
        
        dictionary.words[dictionary.size] = strdup(buffer);
        if (!dictionary.words[dictionary.size]) {
            fclose(file);
            return false;
        }
        dictionary.size++;
    }
    
    fclose(file);
    return true;
}

void FreeDictionary() {
    if (dictionary.words) {
        for (int i = 0; i < dictionary.size; i++) free(dictionary.words[i]);
        free(dictionary.words);
    }
    dictionary.words = NULL;
    dictionary.size = 0;
    dictionary.capacity = 0;
}

bool IsWordInDictionary(const char* word) {
    if (dictionary.size == 0) return true;
    
    char upperWord[MAX_WORD_LENGTH];
    strcpy(upperWord, word);
    for (char* p = upperWord; *p; p++) *p = toupper(*p);
    
    for (int i = 0; i < dictionary.size; i++) {
        if (strcmp(upperWord, dictionary.words[i]) == 0) return true;
    }
    
    return false;
}

void InitGame() {
    memset(&currentUser, 0, sizeof(User));
    memset(&currentLevel, 0, sizeof(Level));
    isLoggedIn = false;
    showLoginScreen = true;
    showGameScreen = false;
    showLevelComplete = false;
    showGameOver = false;
    wordCount = 0;
    memset(inputText, 0, sizeof(inputText));
    memset(message, 0, sizeof(message));
    memset(enteredWords, 0, sizeof(enteredWords));
}

void LoadUserData() {
    char filename[100];
    sprintf(filename, "users/%s.dat", currentUser.username);
    
    FILE* file = fopen(filename, "rb");
    if (file) {
        fread(&currentUser, sizeof(User), 1, file);
        fclose(file);
        isLoggedIn = true;
        showLoginScreen = false;
        showGameScreen = true;
        LoadLevelData(currentUser.currentLevel);
        strcpy(message, "Welcome back! Continuing from your last game.");
    } else {
        strcpy(message, "User not found. Please register.");
    }
}

void SaveUserData() {
    char dirname[100] = "users";
    #ifdef _WIN32
    _mkdir(dirname);
    #else
    mkdir(dirname, 0777);
    #endif
    
    char filename[100];
    sprintf(filename, "users/%s.dat", currentUser.username);
    
    FILE* file = fopen(filename, "wb");
    if (file) {
        currentUser.currentLevel = 1;
        fwrite(&currentUser, sizeof(User), 1, file);
        fclose(file);
        isLoggedIn = true;
        showLoginScreen = false;
        showGameScreen = true;
        LoadLevelData(1);
        strcpy(message, "Registration successful! Starting new game.");
    } else {
        strcpy(message, "Error creating user file.");
    }
}

void LoadLevelData(int level) {
    if (level > MAX_LEVEL) {
        showGameOver = true;
        showGameScreen = false;
        return;
    }
    
    memset(&currentLevel, 0, sizeof(Level));
    wordCount = 0;
    memset(enteredWords, 0, sizeof(enteredWords));
    memset(inputText, 0, sizeof(inputText));
    
    switch (level) {
        case 1: Level_1(); break;
        case 2: Level_2(); break;
        case 3: Level_3(); break;
        case 4: Level_4(); break;
        case 5: Level_5(); break;
        case 6: Level_6(); break;
        case 7: Level_7(); break;
        case 8: Level_8(); break;
        case 9: Level_9(); break;
        case 10: Level_10(); break;
    }
    
    currentLevel.level = level;
}

void SaveGameProgress() {
    char filename[100];
    sprintf(filename, "users/%s.dat", currentUser.username);
    
    FILE* file = fopen(filename, "wb");
    if (file) {
        fwrite(&currentUser, sizeof(User), 1, file);
        fclose(file);
        strcpy(message, "Game progress saved!");
    } else {
        strcpy(message, "Error saving game progress.");
    }
}

bool CheckWord(const char* word) {
    int wordLen = strlen(word);
    if (wordLen < 2) {
        strcpy(message, "Word too short (min 2 letters)");
        return false;
    }
    
    bool validLength = false;
    for (int i = 0; i < 5 && currentLevel.requiredWordLengths[i] > 0; i++) {
        if (wordLen == currentLevel.requiredWordLengths[i]) {
            validLength = true;
            break;
        }
    }
    
    if (!validLength) {
        strcpy(message, "Word length doesn't match requirements");
        return false;
    }
    
    int charCount[26] = {0};
    for (int i = 0; currentLevel.characters[i] != '\0'; i++) {
        if (currentLevel.characters[i] >= 'A' && currentLevel.characters[i] <= 'Z') {
            charCount[currentLevel.characters[i] - 'A']++;
        }
    }
    
    for (int i = 0; i < wordLen; i++) {
        char c = word[i];
        if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
        
        if (c < 'A' || c > 'Z') {
            strcpy(message, "Invalid character in word");
            return false;
        }
        
        if (charCount[c - 'A'] <= 0) {
            strcpy(message, "Letter not available");
            return false;
        }
        
        charCount[c - 'A']--;
    }
    
    for (int i = 0; i < wordCount; i++) {
        if (strcasecmp(enteredWords[i], word) == 0) {
            strcpy(message, "Word already used");
            return false;
        }
    }
    
    if (!IsWordInDictionary(word)) {
        strcpy(message, "Not a valid dictionary word");
        return false;
    }
    
    return true;
}

void DrawLoginScreen() {
    DrawText("Word Game - Login", 300, 50, 24, BLACK);
    DrawText("Username:", 250, 150, 20, DARKGRAY);
    GuiTextBox((Rectangle){250, 180, 300, 40}, currentUser.username, 50, true);
    
    if (GuiButton((Rectangle){250, 240, 140, 40}, "Login")) {
        if (strlen(currentUser.username) > 0) LoadUserData();
        else strcpy(message, "Please enter a username");
    }
    
    if (GuiButton((Rectangle){410, 240, 140, 40}, "Register")) {
        if (strlen(currentUser.username) > 0) SaveUserData();
        else strcpy(message, "Please enter a username");
    }
    
    DrawText(message, 250, 550, 18, RED);
}

void DrawGameScreen() {
    // Left-aligned UI elements
    DrawText(TextFormat("Level: %d/%d", currentLevel.level, MAX_LEVEL), 50, 30, 24, BLACK);
    DrawText(TextFormat("Player: %s", currentUser.username), 50, 60, 20, DARKGRAY);
    
    DrawText("Available Characters:", 50, 100, 20, DARKGRAY);
    for (int i = 0; currentLevel.characters[i] != '\0'; i++) {
        DrawRectangle(50 + i * 60, 130, 50, 50, LIGHTGRAY);
        DrawText(TextFormat("%c", currentLevel.characters[i]), 65 + i * 60, 145, 30, BLACK);
    }
    
    DrawText("Form words of these lengths:", 50, 200, 20, DARKGRAY);
    int x = 50;
    for (int i = 0; i < 5 && currentLevel.requiredWordLengths[i] > 0; i++) {
        DrawText(TextFormat("%d letters", currentLevel.requiredWordLengths[i]), x, 230, 18, DARKGRAY);
        x += 120;
    }
    
    DrawText(TextFormat("Words found: %d/%d", wordCount, currentLevel.requiredWordCount), 50, 260, 20, DARKGRAY);
    
    DrawText("Enter a word:", 50, 300, 20, DARKGRAY);
    if (GuiTextBox((Rectangle){50, 330, 300, 40}, inputText, 99, true)) {
        // Input text has changed
    }
    
    if (GuiButton((Rectangle){360, 330, 100, 40}, "Submit")) {
        if (strlen(inputText) > 0) {
            if (CheckWord(inputText)) {
                strcpy(enteredWords[wordCount], inputText);
                wordCount++;
                strcpy(message, "Word accepted!");
                inputText[0] = '\0';
                
                if (wordCount >= currentLevel.requiredWordCount) {
                    currentUser.currentLevel++;
                    showLevelComplete = true;
                    showGameScreen = false;
                    SaveGameProgress();
                }
            }
        } else {
            strcpy(message, "Please enter a word");
        }
    }
    
    DrawText("Entered Words:", 50, 390, 20, DARKGRAY);
    for (int i = 0; i < wordCount; i++) {
        DrawText(enteredWords[i], 50 + (i % 3) * 150, 420 + (i / 3) * 30, 18, DARKBLUE);
    }
    
    if (GuiButton((Rectangle){650, 30, 120, 40}, "Save & Exit")) {
        SaveGameProgress();
        isLoggedIn = false;
        showLoginScreen = true;
        showGameScreen = false;
    }
    
    DrawText(message, 50, 550, 18, RED);
}

void DrawLevelCompleteScreen() {
    DrawText("Level Complete!", 300, 150, 30, GREEN);
    
    if (GuiButton((Rectangle){300, 250, 200, 40}, "Continue to Next Level")) {
        LoadLevelData(currentUser.currentLevel);
        showLevelComplete = false;
        showGameScreen = true;
    }
}

void DrawGameOverScreen() {
    DrawText("CONGRATULATIONS!", 250, 100, 40, GOLD);
    DrawText("You've completed all levels!", 250, 150, 24, BLACK);
    
    if (GuiButton((Rectangle){300, 300, 200, 40}, "Play Again")) {
        currentUser.currentLevel = 1;
        LoadLevelData(currentUser.currentLevel);
        showGameOver = false;
        showGameScreen = true;
    }
    
    if (GuiButton((Rectangle){300, 360, 200, 40}, "Main Menu")) {
        InitGame();
        showGameOver = false;
        showLoginScreen = true;
    }
}

void ProcessGameLogic() {
    // Game logic without timer
}

// Level definitions (modified to not require words longer than 6 letters)
void Level_1() {
    strcpy(currentLevel.characters, "CATDOG");
    currentLevel.requiredWordCount = 3;
    currentLevel.requiredWordLengths[0] = 3;
    currentLevel.requiredWordLengths[1] = 4;
}

void Level_2() {
    strcpy(currentLevel.characters, "HOUSEP");
    currentLevel.requiredWordCount = 3;
    currentLevel.requiredWordLengths[0] = 4;
    currentLevel.requiredWordLengths[1] = 5;
}

void Level_3() {
    strcpy(currentLevel.characters, "GARDEN");
    currentLevel.requiredWordCount = 4;
    currentLevel.requiredWordLengths[0] = 4;
    currentLevel.requiredWordLengths[1] = 5;
    currentLevel.requiredWordLengths[2] = 6;
}

void Level_4() {
    strcpy(currentLevel.characters, "TEACHER");
    currentLevel.requiredWordCount = 4;
    currentLevel.requiredWordLengths[0] = 4;
    currentLevel.requiredWordLengths[1] = 5;
    currentLevel.requiredWordLengths[2] = 6;
}

void Level_5() {
    strcpy(currentLevel.characters, "MUSICAL");
    currentLevel.requiredWordCount = 4;
    currentLevel.requiredWordLengths[0] = 5;
    currentLevel.requiredWordLengths[1] = 6;
}

void Level_6() {
    strcpy(currentLevel.characters, "BEAUTIF");
    currentLevel.requiredWordCount = 5;
    currentLevel.requiredWordLengths[0] = 4;
    currentLevel.requiredWordLengths[1] = 5;
    currentLevel.requiredWordLengths[2] = 6;
}

void Level_7() {
    strcpy(currentLevel.characters, "SCIENCE");
    currentLevel.requiredWordCount = 5;
    currentLevel.requiredWordLengths[0] = 5;
    currentLevel.requiredWordLengths[1] = 6;
}

void Level_8() {
    strcpy(currentLevel.characters, "MATHEMAT");
    currentLevel.requiredWordCount = 5;
    currentLevel.requiredWordLengths[0] = 5;
    currentLevel.requiredWordLengths[1] = 6;
}

void Level_9() {
    strcpy(currentLevel.characters, "PHILOSO");
    currentLevel.requiredWordCount = 6;
    currentLevel.requiredWordLengths[0] = 5;
    currentLevel.requiredWordLengths[1] = 6;
}

void Level_10() {
    strcpy(currentLevel.characters, "DICTION");
    currentLevel.requiredWordCount = 7;
    currentLevel.requiredWordLengths[0] = 5;
    currentLevel.requiredWordLengths[1] = 6;
}