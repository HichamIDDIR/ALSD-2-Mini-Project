// =================================================================================================
// 1. INCLUDED ARIES
// These lines include necessary standard libraries and external libraries like Raylib for graphics
// and Raygui for GUI elements.
// =================================================================================================
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION // Define this in exactly one .c file
#include "raygui.h"
#include <stdio.h>   // For standard input/output operations (printf, fopen, etc.)
#include <stdlib.h>  // For general utilities (malloc, free, exit, etc.)
#include <string.h>  // For string manipulation functions (strcpy, strlen, etc.)
#include <stdbool.h> // For boolean type (true, false)
#include <ctype.h>   // For character type functions (toupper, isalpha, etc.)

// Platform-specific headers for mkdir
#ifdef _WIN32
#include <direct.h> // For _mkdir on Windows
#else
#include <sys/stat.h> // For mkdir on POSIX systems
#endif

// =================================================================================================
// 2. PREPROCESSOR DEFINITIONS (CONSTANTS)
// These define constant values used throughout the program, such as colors and game
// configuration parameters.
// =================================================================================================

// define colors
/*#define COLOR_PRIMARY_TEXT BLACK
#define COLOR_SECONDARY_TEXT DARKGRAY
#define COLOR_BOX_BACKGROUND LIGHTGRAY
#define COLOR_ACCENT BLUE
#define COLOR_ERROR RED
#define COLOR_SUCCESS GREEN*/
#define COLOR_PRIMARY_TEXT (Color){13, 31, 35, 255}    // Previously BLACK, now #0D1F23
#define COLOR_SECONDARY_TEXT (Color){19, 46, 56, 255}  // Previously DARKGRAY, now #132E36
#define COLOR_BOX_BACKGROUND (Color){90, 99, 106, 255} // Previously LIGHTGRAY, now #5A636A
#define COLOR_ACCENT WHITE                             // Previously BLUE, now #69818D
#define COLOR_ERROR RED
#define COLOR_SUCCESS GREEN
#define COLOR_FOUND_TEXT (Color){175, 179, 183, 255}
// Configuration du dictionnaire (Dictionary configuration)
#define MAX_LONGUEUR_MOT 50           // Longueur maximale d'un mot (Maximum word length)
#define CAPACITE_DICT_INITIALE 100000 // Capacité initiale du dictionnaire (Initial dictionary capacity)
#define NIVEAU_MAX 10                 // Nombre maximum de niveaux (Maximum number of levels)
#define LONGUEUR_MOT_MAX_REQUISE 6    // Longueur maximale requise pour un niveau (Maximum required word length for a level)

// =================================================================================================
// 3. DATA STRUCTURE DEFINITIONS (TYPEDEFS)
// These define the custom data types (structs) used to organize game data.
// =================================================================================================

// Structure pour le dictionnaire de mots
typedef struct
{
    char **mots;  // Tableau pour stocker les mots (des chaines de caractères)
    int taille;   // Combien de mots il y a ntenant dans le dictionnaire
    int capacite; // La taille maximale du tableau pour l'instant
} Dictionnaire;

// Structure pour garder les infos de l'utilisateur
typedef struct
{
    char nomUtilisateur[50]; // Le nom que l'utilisateur utilise
    int niveauActuel;        // Le niveau où est rendu l'utilisateur
} Utilisateur;

// Structure pour un niveau du jeu
typedef struct
{
    int niveau;                   // Le numéro du niveau (1, 2, 3...)
    char caracteres[10];          // Les lettres qu'on peut utiliser dans ce niveau
    int nombreMotsRequises;       // Combien de mots il faut trouver pour gagner
    int longueursMotsRequises[5]; // Les tailles des mots qu'il faut faire (ex: 3 lettres, 5 lettres...)
} Niveau;

// =================================================================================================
// 4. GLOBAL VARIABLES
// These variables are accessible from any part of the code. They store the game's state.
// =================================================================================================

Utilisateur utilisateurCourant;     // L'utilisateur actuellement connecté (The currently logged-in user)
Niveau niveauCourant;               // Le niveau actuellement en jeu (The current level being played)
char texteSaisi[100] = "";          // Texte saisi par l'utilisateur dans les champs de texte (Text entered by the user in text boxes)
bool estConnecte = false;           // Indicateur si un utilisateur est connecté (Flag indicating if a user is logged in)
bool afficherEcranConnexion = true; // Indicateur pour afficher l'écran de connexion (Flag to display the login screen)
bool afficherEcranJeu = false;      // Indicateur pour afficher l'écran de jeu (Flag to display the game screen)
bool afficherNiveauTermine = false; // Indicateur pour afficher l'écran de niveau terminé (Flag to display the level complete screen)
bool afficherJeuTermine = false;    // Indicateur pour afficher l'écran de jeu terminé (Flag to display the game complete screen)
char message[100] = "";             // Message à afficher à l'utilisateur (e.g., erreurs, succès) (Message to display to the user)
char motsSaisis[10][50];            // Mots valides saisis par l'utilisateur pour le niveau actuel (Valid words entered by the user for the current level)
int compteMots = 0;                 // Nombre de mots valides saisis pour le niveau actuel (Number of valid words entered for the current level)
Dictionnaire dictionnaire = {0};    // Le dictionnaire global de mots valides (The global dictionary of valid words)
const float SCALE_FACTOR = 1.5f;    // Scaling factor for all UI elements

// =================================================================================================
// 5. FUNCTION DECLARATIONS (PROTOTYPES)
// These declare the functions that will be defined later. This gives an overview of the
// program's modular structure.
// =================================================================================================

// --- Game Initialization and State Management ---
void InitialiserJeu();
void ChargerDonneesUtilisateur();
void SauvegarderDonneesUtilisateur();
void ChargerDonneesNiveau(int niveau);
void SauvegarderProgressionJeu();

// --- Dictionary Management ---
bool ChargerDictionnaire(const char *fichier);
bool EstMotDansDictionnaire(const char *mot);
void LibererDictionnaire();

// --- Game Logic ---
bool VerifierMot(const char *mot);

// --- UI Drawing Functions ---
void DessinerEcranConnexion();
void DessinerEcranJeu();
void DessinerEcranNiveauTermine();
void DessinerEcranJeuTermine();

// --- Level Definitions ---
void Niveau_1();
void Niveau_2();
void Niveau_3();
void Niveau_4();
void Niveau_5();
void Niveau_6();
void Niveau_7();
void Niveau_8();
void Niveau_9();
void Niveau_10();

// =================================================================================================
// 6. MAIN FUNCTION (`main`)
// This is the entry point of the program. Execution begins here.
// =================================================================================================

int main(void)
{
    const int screenWidth = (int)(800 * SCALE_FACTOR);  // Largeur de l'écran.
    const int screenHeight = (int)(600 * SCALE_FACTOR); // Hauteur de l'écran.

    InitWindow(screenWidth, screenHeight, "Jeu de Mots");      // Initialise la fenêtre du jeu.
    SetTargetFPS(60);                                          // Définit le taux de rafraîchissement cible.
    GuiSetStyle(DEFAULT, TEXT_SIZE, (int)(10 * SCALE_FACTOR)); // Ajuste la taille du texte par défaut de Raygui.

    Color colorTop = (Color){175, 179, 183, 255}; // Couleur du haut du gradient de fond.
    Color colorBottom = (Color){13, 31, 36, 255}; // Couleur du bas du gradient de fond.
    if (!ChargerDictionnaire("words.txt"))        // Tente de charger le dictionnaire depuis un fichier.
    {
        TraceLog(LOG_WARNING, "Echec du chargement du dictionnaire");                // Affiche un avertissement si le chargement échoue.
        dictionnaire.capacite = CAPACITE_DICT_INITIALE;                              // Initialise la capacité du dictionnaire.
        dictionnaire.mots = (char **)malloc(dictionnaire.capacite * sizeof(char *)); // Alloue de la mémoire pour les mots.
        if (dictionnaire.mots == NULL)                                               // Vérifie si l'allocation a réussi.
        {
            TraceLog(LOG_ERROR, "Failed to allocate memory for default dictionary"); // Erreur d'allocation mémoire.
            CloseWindow();
            return 1; // Indique un échec.
        }

        const char *motsParDefaut[] = {"CAT", "DOG", "SUN", "MOON", "STAR", "TREE", "BOOK", "BALL", "FISH", "BIRD", "GOD"}; // Mots par défaut si le fichier ne charge pas.
        dictionnaire.taille = sizeof(motsParDefaut) / sizeof(motsParDefaut[0]);                                             // Définit la taille du dictionnaire par défaut.

        for (int i = 0; i < dictionnaire.taille; i++) // Boucle pour copier les mots par défaut.
        {
            dictionnaire.mots[i] = strdup(motsParDefaut[i]); // Duplique la chaîne de caractères.
            if (dictionnaire.mots[i] == NULL)                // Vérifie si la duplication a réussi.
            {
                TraceLog(LOG_ERROR, "Failed to strdup default dictionary word"); // Erreur lors de la duplication.
                CloseWindow();
                return 1;
            }
        }
    }

    InitialiserJeu(); // Initialise les variables et l'état du jeu.

    while (!WindowShouldClose()) // Boucle principale du jeu.
    {
        BeginDrawing();
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, colorTop, colorBottom); // Dessine le fond avec un gradient.
        if (afficherEcranConnexion)                                                     // Si l'écran de connexion doit être affiché.
        {
            DessinerEcranConnexion();
        }
        else if (afficherNiveauTermine) // Si l'écran de niveau terminé doit être affiché.
        {
            DessinerEcranNiveauTermine();
        }
        else if (afficherJeuTermine) // Si l'écran de fin de jeu doit être affiché.
        {
            DessinerEcranJeuTermine();
        }
        else if (afficherEcranJeu) // Si l'écran de jeu principal doit être affiché.
        {
            DessinerEcranJeu();
        }

        EndDrawing();
    }

    LibererDictionnaire(); // Libère la mémoire allouée pour le dictionnaire.
    CloseWindow();         // Ferme la fenêtre du jeu.
    return 0;              // Indique que le programme s'est terminé correctement.
}
// =================================================================================================
// 7. DICTIONARY MANAGEMENT FUNCTION DEFINITIONS
// =================================================================================================

// Charge le dictionnaire depuis un fichier
bool ChargerDictionnaire(const char *fichier)
{
    // Ouvre le fichier en mode lecture
    FILE *fichierDico = fopen(fichier, "r");
    if (!fichierDico)
        return false; // Erreur si le fichier ne s'ouvre pas

    char tampon[MAX_LONGUEUR_MOT]; // Pour lire chaque mot du fichier

    // Initialise le dictionnaire
    dictionnaire.capacite = CAPACITE_DICT_INITIALE;
    dictionnaire.mots = (char **)malloc(dictionnaire.capacite * sizeof(char *));
    if (!dictionnaire.mots)
    {
        fclose(fichierDico);
        return false; // Erreur si l'allocation échoue
    }
    dictionnaire.taille = 0; // Pas encore de mots

    // Lit chaque ligne du fichier
    while (fgets(tampon, MAX_LONGUEUR_MOT, fichierDico))
    {
        // Enlève le retour à la ligne à la fin du mot
        size_t longueur = strlen(tampon);
        while (longueur > 0 && (tampon[longueur - 1] == '\n' || tampon[longueur - 1] == '\r'))
        {
            tampon[--longueur] = '\0';
        }

        // Ignore l mots trop courts ou trop longs
        if (longueur < 2 || longueur > LONGUEUR_MOT_MAX_REQUISE)
            continue;

        // Met le mot en majuscules
        for (char *p = tampon; *p; p++)
            *p = toupper(*p);

        // Vérifie si le mot existe déjà dans le dictionnaire
        bool existe = false;
        for (int i = 0; i < dictionnaire.taille; i++)
        {
            if (strcmp(dictionnaire.mots[i], tampon) == 0)
            {
                existe = true;
                break;
            }
        }
        if (existe)
            continue; // Passe au mot suivant si déjà présent

        // Augmente la taille du tableau si nécessaire
        if (dictionnaire.taille >= dictionnaire.capacite)
        {
            dictionnaire.capacite *= 2; // Double la capacité
            char **nouveauxMots = (char **)realloc(dictionnaire.mots, dictionnaire.capacite * sizeof(char *));
            if (!nouveauxMots)
            {
                fclose(fichierDico);
                // Libère la mémoire avant de quitter
                for (int k = 0; k < dictionnaire.taille; ++k)
                    free(dictionnaire.mots[k]);
                free(dictionnaire.mots);
                dictionnaire.mots = NULL;
                return false; // Erreur si le réallocation échoue
            }
            dictionnaire.mots = nouveauxMots;
        }

        // Copie le mot dans le dictionnaire
        dictionnaire.mots[dictionnaire.taille] = strdup(tampon);
        if (!dictionnaire.mots[dictionnaire.taille])
        {
            fclose(fichierDico);
            // Libère la mémoire en cas d'erreur
            for (int k = 0; k < dictionnaire.taille; ++k)
                free(dictionnaire.mots[k]);
            free(dictionnaire.mots);
            dictionnaire.mots = NULL;
            return false; // Erreur si la copie échoue
        }
        dictionnaire.taille++; // Un mot de plus !
    }

    fclose(fichierDico);
    return true; // Tout s'est bien passé
}

// Libère la mémoire utilisée par le dictionnaire
void LibererDictionnaire()
{
    // On vérifie d'abord si le dictionnaire existe
    if (dictionnaire.mots)
    {
        // On libère chaque mot un par un
        for (int i = 0; i < dictionnaire.taille; i++)
            free(dictionnaire.mots[i]); // Au revoir le mot !

        // On libère le tableau principal
        free(dictionnaire.mots); // Poubelle !
    }

    // On remet tout à zéro pour être propre
    dictionnaire.mots = NULL;  // Plus de mots
    dictionnaire.taille = 0;   // Taille à 0
    dictionnaire.capacite = 0; // Capacité à 0
}

// Vérifie si un mot est dans le dictionnaire
bool EstMotDansDictionnaire(const char *mot)
{
    // Si le dictionnaire est vide mais initialisé, on accepte tous les mots (mode facile ?)
    if (dictionnaire.taille == 0 && dictionnaire.capacite > 0)
        return true;

    // On crée une copie du mot en majuscules
    char motMaj[MAX_LONGUEUR_MOT];
    strcpy(motMaj, mot); // On copie le mot original

    // On met tout en majuscules lettre par lettre
    for (char *p = motMaj; *p; p++)
        *p = toupper(*p);

    // On cherche le mot dans tous les mots du dictionnaire
    for (int i = 0; i < dictionnaire.taille; i++)
    {
        // Si on trouve le mot, c'est gagné !
        if (strcmp(motMaj, dictionnaire.mots[i]) == 0)
            return true;
    }

    // Si on arrive ici, c'est que le mot n'existe pas :(
    return false;
}

// =================================================================================================
// 8. GAME INITIAZATION AND STATE MANAGEMENT FUNCTION DEFINITIONS
// =================================================================================================

void InitialiserJeu()
{
    memset(&utilisateurCourant, 0, sizeof(Utilisateur)); // Réinitialise les données de l'utilisateur courant.
    memset(&niveauCourant, 0, sizeof(Niveau));           // Réinitialise les données du niveau courant.
    estConnecte = false;                                 // L'utilisateur n'est plus connecté.
    afficherEcranConnexion = true;                       // On commence par afficher l'écran de connexion.
    afficherEcranJeu = false;
    afficherNiveauTermine = false;
    afficherJeuTermine = false;
    compteMots = 0;                            // Réinitialise le compteur de mots trouvés.
    memset(texteSaisi, 0, sizeof(texteSaisi)); // Efface le texte saisi par l'utilisateur.
    memset(message, 0, sizeof(message));       // Efface le message à afficher à l'utilisateur.
    memset(motsSaisis, 0, sizeof(motsSaisis)); // Efface la liste des mots saisis.
}

// Charge les données d'un utilisateur depuis un fichier
void ChargerDonneesUtilisateur()
{
    // Crée le nom du fichier avec le nom d'utilisateur
    char nomFichier[100];
    sprintf(nomFichier, "users/%s.dat", utilisateurCourant.nomUtilisateur);

    // Essaye d'ouvrir le fichier en mode binaire
    FILE *fichier = fopen(nomFichier, "rb");
    if (fichier) // Si le fichier existe
    {
        // Lit les données de l'utilisateur
        fread(&utilisateurCourant, sizeof(Utilisateur), 1, fichier);
        fclose(fichier); // Ferme le fichier

        // Met à jour l'état du jeu
        estConnecte = true;             // L'utilisateur est connecté
        afficherEcranConnexion = false; // Cache l'écran de connexion
        afficherEcranJeu = true;        // Affiche l'écran de jeu

        // Charge le niveau où l'utilisateur était
        ChargerDonneesNiveau(utilisateurCourant.niveauActuel);

        // Affiche un message de bienvenue
        strcpy(message, "Bienvenue ! Reprise de la partie.");
    }
    else // Si le fichier n'existe pas
    {
        // Affiche un message d'erreur
        strcpy(message, "Utilisateur non trouvé. Inscrivez-vous.");
    }
}

// Sauvegarde les données de l'utilisateur
void SauvegarderDonneesUtilisateur()
{
    // Crée le dossier 'users' si besoin
    char nomDossier[100] = "users";
#ifdef _WIN32
    _mkdir(nomDossier); // Création dossier Windows
#else
    mkdir(nomDossier, 0777); // Création dossier Linux/Unix
#endif

    // Prépare le chemin du fichier
    char nomFichier[100];
    sprintf(nomFichier, "users/%s.dat", utilisateurCourant.nomUtilisateur);

    // Ouvre le fichier en écriture binaire
    FILE *fichier = fopen(nomFichier, "wb");
    if (fichier)
    {
        // Met à jour et sauvegarde les données
        utilisateurCourant.niveauActuel = 1;
        fwrite(&utilisateurCourant, sizeof(Utilisateur), 1, fichier);
        fclose(fichier);

        // Met à jour l'interface
        estConnecte = true;
        afficherEcranConnexion = false;
        afficherEcranJeu = true;

        // Charge le niveau 1
        ChargerDonneesNiveau(1);
        strcpy(message, "Inscription réussie ! Nouvelle partie.");
    }
    else
    {
        strcpy(message, "Erreur création fichier utilisateur.");
    }
}

void ChargerDonneesNiveau(int niveau)
{
    if (niveau > NIVEAU_MAX)
    {
        afficherJeuTermine = true;
        afficherEcranJeu = false;
        return;
    }

    memset(&niveauCourant, 0, sizeof(Niveau));
    compteMots = 0;
    memset(motsSaisis, 0, sizeof(motsSaisis));
    memset(texteSaisi, 0, sizeof(texteSaisi));
    // message[0] = '\0'; // Clear message on level change if desired

    switch (niveau)
    {
    case 1:
        Niveau_1();
        break;
    case 2:
        Niveau_2();
        break;
    case 3:
        Niveau_3();
        break;
    case 4:
        Niveau_4();
        break;
    case 5:
        Niveau_5();
        break;
    case 6:
        Niveau_6();
        break;
    case 7:
        Niveau_7();
        break;
    case 8:
        Niveau_8();
        break;
    case 9:
        Niveau_9();
        break;
    case 10:
        Niveau_10();
        break;
    default: // Should ideally not happen if NIVEAU_MAX is respected
        TraceLog(LOG_WARNING, "Niveau invalide: %d. Chargement du niveau 1.", niveau);
        Niveau_1();
        niveau = 1; // Ensure a valid state
        break;
    }

    niveauCourant.niveau = niveau;
}

void SauvegarderProgressionJeu()
{
    if (!estConnecte || strlen(utilisateurCourant.nomUtilisateur) == 0)
    {
        return;
    }
    char nomFichier[100];
    sprintf(nomFichier, "users/%s.dat", utilisateurCourant.nomUtilisateur);

    FILE *fichier = fopen(nomFichier, "wb");
    if (fichier)
    {
        fwrite(&utilisateurCourant, sizeof(Utilisateur), 1, fichier);
        fclose(fichier);
        strcpy(message, "Progression sauvegardée !");
    }
    else
    {
        strcpy(message, "Erreur sauvegarde progression.");
    }
}

// =================================================================================================
// 9. GAME LOGIC FUNCTION DEFINITIONS
// =================================================================================================

// Vérifie si le mot saisi est valide
bool VerifierMot(const char *mot)
{
    int longueurMot = strlen(mot);

    // 1. Vérifier la longueur minimale du mot
    if (longueurMot < 2)
    {
        strcpy(message, "Mot trop court (min 2 lettres)");
        return false;
    }

    // 2. Vérifier si la longueur du mot correspond à une des longueurs requises pour le niveau
    bool longueurValide = false;
    for (int i = 0; i < 5 && niveauCourant.longueursMotsRequises[i] > 0; i++)
    {
        if (longueurMot == niveauCourant.longueursMotsRequises[i])
        {
            longueurValide = true;
            break;
        }
    }
    if (!longueurValide)
    {
        strcpy(message, "Longueur mot incorrecte");
        return false;
    }

    // 3. Vérifier si le mot utilise uniquement les caractères disponibles et en quantité suffisante
    int compteCaracteresDisponibles[26] = {0};
    for (int i = 0; niveauCourant.caracteres[i] != '\0'; i++)
    {
        if (niveauCourant.caracteres[i] >= 'A' && niveauCourant.caracteres[i] <= 'Z')
        {
            compteCaracteresDisponibles[niveauCourant.caracteres[i] - 'A']++;
        }
    }

    char motMajLocal[MAX_LONGUEUR_MOT]; // ici on met le mot en majuscules
    strcpy(motMajLocal, mot);
    for (char *p = motMajLocal; *p; ++p)
        *p = toupper(*p);

    for (int i = 0; i < longueurMot; i++)
    {
        char c = motMajLocal[i];

        if (c < 'A' || c > 'Z') // au cas où c'est pas une lettre
        {
            strcpy(message, "Caractère invalide dans le mot.");
            return false;
        }

        if (compteCaracteresDisponibles[c - 'A'] <= 0)
        {
            sprintf(message, "Lettre '%c' non disponible ou en excès.", c);
            return false;
        }
        compteCaracteresDisponibles[c - 'A']--;
    }

    // 4. Vérifier si le mot a déjà été saisi dans ce niveau
    for (int i = 0; i < compteMots; i++)
    {
        if (strcasecmp(motsSaisis[i], mot) == 0)
        {
            strcpy(message, "Mot déjà utilisé");
            return false;
        }
    }

    // 5. Vérifier si le mot est dans le dictionnaire
    if (!EstMotDansDictionnaire(mot))
    {
        strcpy(message, "Mot non valide");
        return false;
    }

    return true;
}

// =================================================================================================
// 10. UI DRAWING FUNCTION DEFINITIONS
// =================================================================================================

void DessinerEcranConnexion()
{
    const int screenWidth = GetScreenWidth(); // Largeur de l'écran (mise à l'échelle).
    int titleFontSize = (int)(24 * SCALE_FACTOR);
    int labelFontSize = (int)(20 * SCALE_FACTOR);
    int errorFontSize = (int)(20 * SCALE_FACTOR);

    float titleY = 100 * SCALE_FACTOR;
    float usernameLabelY = 200 * SCALE_FACTOR;
    float textBoxY = 230 * SCALE_FACTOR;
    float buttonsY = 290 * SCALE_FACTOR;
    float messageY = 350 * SCALE_FACTOR;

    float elementWidth = 300 * SCALE_FACTOR; // Largeur des éléments.
    float buttonWidth = 140 * SCALE_FACTOR;
    // float buttonSpacing = (300 - 140 - 140) * SCALE_FACTOR; // Variable non utilisée.
    float firstButtonX = (screenWidth - elementWidth) / 2.0f;
    // float secondButtonX = firstButtonX + buttonWidth + ((300 - 2 * 140) * SCALE_FACTOR); // Variable non utilisée.

    float originalButton2OffsetX = 160 * SCALE_FACTOR; // Décalage pour le deuxième bouton.

    DrawText("Jeu de Mots - Connexion", (screenWidth - MeasureText("Jeu de Mots - Connexion", titleFontSize)) / 2, (int)titleY, titleFontSize, COLOR_PRIMARY_TEXT);
    DrawText("Nom d'utilisateur:", (int)(firstButtonX), (int)usernameLabelY, labelFontSize, COLOR_SECONDARY_TEXT);

    Rectangle textBoxRect = {firstButtonX, textBoxY, elementWidth, 40 * SCALE_FACTOR};
    GuiTextBox(textBoxRect, utilisateurCourant.nomUtilisateur, 50, GuiIsLocked() ? false : true); // Crée la boîte de texte.

    Rectangle loginButtonRect = {firstButtonX, buttonsY, buttonWidth, 40 * SCALE_FACTOR};
    Rectangle registerButtonRect = {firstButtonX + originalButton2OffsetX, buttonsY, buttonWidth, 40 * SCALE_FACTOR};

    if (!GuiIsLocked() && GuiButton(loginButtonRect, "Connexion")) // Si on clique sur le bouton connexion.
    {
        if (strlen(utilisateurCourant.nomUtilisateur) > 0)
            ChargerDonneesUtilisateur();
        else
            strcpy(message, "Entrez un nom d'utilisateur");
    }

    if (!GuiIsLocked() && GuiButton(registerButtonRect, "Inscription")) // Si on clique sur le bouton inscription.
    {
        if (strlen(utilisateurCourant.nomUtilisateur) > 0)
            SauvegarderDonneesUtilisateur();
        else
            strcpy(message, "Entrez un nom d'utilisateur");
    }
    if (strlen(message) > 0) // Si il y a un message à afficher.
    {
        DrawText(message, (screenWidth - MeasureText(message, errorFontSize)) / 2, (int)messageY, errorFontSize, COLOR_ERROR);
    }
}

void DessinerEcranJeu()
{
    const int screenWidth = GetScreenWidth();   // Récupère la largeur de l'écran.
    const int screenHeight = GetScreenHeight(); // Récupère la hauteur de l'écran.
    char buffer[100];                           // Tampon pour stocker du texte.

    int font24 = (int)(24 * SCALE_FACTOR);         // Taille de police 24 (mise à l'échelle).
    int font20 = (int)(20 * SCALE_FACTOR);         // Taille de police 20 (mise à l'échelle).
    int font18 = (int)(18 * SCALE_FACTOR);         // Taille de police 18 (mise à l'échelle).
    int letterFontSize = (int)(30 * SCALE_FACTOR); // Taille de police pour les lettres disponibles.

    float levelLabelY = 20 * SCALE_FACTOR;                   // Position Y du label du niveau.
    float playerLabelY = 50 * SCALE_FACTOR;                  // Position Y du label du joueur.
    float availableCharsLabelY = 90 * SCALE_FACTOR;          // Position Y du label des caractères disponibles.
    float boxSize = 50 * SCALE_FACTOR;                       // Taille des boîtes pour les caractères.
    float boxSpacing = 10 * SCALE_FACTOR;                    // Espacement entre les boîtes de caractères.
    float boxY = availableCharsLabelY + (30 * SCALE_FACTOR); // Position Y des boîtes de caractères.

    sprintf(buffer, "Niveau: %d/%d", niveauCourant.niveau, NIVEAU_MAX);                                              // Formatage de la chaîne pour afficher le niveau actuel et le maximum.
    DrawText(buffer, (screenWidth - MeasureText(buffer, font24)) / 2, (int)levelLabelY, font24, COLOR_PRIMARY_TEXT); // Dessine le texte du niveau au centre.

    sprintf(buffer, "Joueur: %s", utilisateurCourant.nomUtilisateur);                                                   // Formatage de la chaîne pour afficher le nom du joueur.
    DrawText(buffer, (screenWidth - MeasureText(buffer, font20)) / 2, (int)playerLabelY, font20, COLOR_SECONDARY_TEXT); // Dessine le nom du joueur au centre.

    DrawText("Caractères disponibles:", (screenWidth - MeasureText("Caractères disponibles:", font20)) / 2, (int)availableCharsLabelY, font20, COLOR_SECONDARY_TEXT); // Dessine le label des caractères disponibles.

    int numChars = strlen(niveauCourant.caracteres);                       // Nombre de caractères disponibles.
    float charBlockWidth = numChars * (boxSize + boxSpacing) - boxSpacing; // Largeur totale du bloc de caractères.
    float charStartX = (screenWidth - charBlockWidth) / 2.0f;              // Position X de départ pour le bloc de caractères (centré).

    for (int i = 0; i < numChars; i++) // Boucle pour dessiner chaque caractère disponible.
    {
        float currentBoxX = charStartX + i * (boxSize + boxSpacing);                                                         // Position X de la boîte actuelle.
        DrawRectangleGradientV((int)currentBoxX, (int)boxY, (int)boxSize, (int)boxSize, COLOR_ACCENT, COLOR_BOX_BACKGROUND); // Dessine une boîte avec un gradient vertical.
        char charText[2] = {niveauCourant.caracteres[i], '\0'};                                                              // Crée une chaîne de caractère unique.
        int textWidth = MeasureText(charText, letterFontSize);                                                               // Largeur du texte du caractère.
        int textX = (int)(currentBoxX + (boxSize - textWidth) / 2.0f);                                                       // Position X du texte (centré dans la boîte).
        int textY = (int)(boxY + (boxSize - letterFontSize) / 2.0f);                                                         // Position Y du texte (centré dans la boîte).
        DrawText(charText, textX, textY, letterFontSize, COLOR_PRIMARY_TEXT);                                                // Dessine le caractère.
    }

    float lengthsLabelY = boxY + boxSize + (20 * SCALE_FACTOR);                                                                                                                    // Position Y du label des longueurs de mots requises.
    DrawText("Formez des mots de ces longueurs:", (screenWidth - MeasureText("Formez des mots de ces longueurs:", font20)) / 2, (int)lengthsLabelY, font20, COLOR_SECONDARY_TEXT); // Dessine le label.

    int totalLengthWidth = 0;                                                 // Largeur totale des indications de longueur de mots.
    int lengthCount = 0;                                                      // Nombre de longueurs de mots requises.
    float lengthSpacing = 30 * SCALE_FACTOR;                                  // Espacement entre les indications de longueur.
    int lengthFontSize = (int)(18 * SCALE_FACTOR);                            // Taille de police pour les longueurs de mots.
    for (int i = 0; i < 5 && niveauCourant.longueursMotsRequises[i] > 0; i++) // Boucle pour calculer la largeur totale des indications de longueur.
    {
        sprintf(buffer, "%d lettres", niveauCourant.longueursMotsRequises[i]);
        totalLengthWidth += MeasureText(buffer, lengthFontSize);
        lengthCount++;
    }
    totalLengthWidth += (lengthCount > 0 ? (lengthCount - 1) * (int)lengthSpacing : 0); // Ajoute l'espacement entre les longueurs.

    float lengthStartX = (screenWidth - totalLengthWidth) / 2.0f; // Position X de départ pour les longueurs (centré).
    float currentX = lengthStartX;                                // Position X actuelle pour dessiner une longueur.
    float lengthY = lengthsLabelY + (30 * SCALE_FACTOR);          // Position Y pour dessiner les longueurs.

    for (int i = 0; i < 5 && niveauCourant.longueursMotsRequises[i] > 0; i++) // Boucle pour dessiner les longueurs de mots requises.
    {
        sprintf(buffer, "%d lettres", niveauCourant.longueursMotsRequises[i]);
        DrawText(buffer, (int)currentX, (int)lengthY, lengthFontSize, COLOR_ACCENT);
        currentX += MeasureText(buffer, lengthFontSize) + lengthSpacing;
    }

    float countY = lengthY + (35 * SCALE_FACTOR);                                                         // Position Y pour afficher le nombre de mots trouvés.
    sprintf(buffer, "Mots trouvés: %d/%d", compteMots, niveauCourant.nombreMotsRequises);                 // Formatage du texte.
    DrawText(buffer, (screenWidth - MeasureText(buffer, font20)) / 2, (int)countY, font20, COLOR_ACCENT); // Dessine le nombre de mots trouvés.

    float inputLabelY = countY + (40 * SCALE_FACTOR);                                                                                      // Position Y du label pour l'entrée de mot.
    DrawText("Entrez un mot:", (screenWidth - MeasureText("Entrez un mot:", font20)) / 2, (int)inputLabelY, font20, COLOR_SECONDARY_TEXT); // Dessine le label.

    float inputAreaWidth = (300 + 10 + 100) * SCALE_FACTOR;        // Largeur totale de la zone d'entrée (boîte de texte + espace + bouton).
    float inputAreaStartX = (screenWidth - inputAreaWidth) / 2.0f; // Position X de départ de la zone d'entrée.
    float inputControlsY = inputLabelY + (30 * SCALE_FACTOR);      // Position Y des contrôles d'entrée.
    float textBoxWidth = 300 * SCALE_FACTOR;                       // Largeur de la boîte de texte.
    float buttonWidth = 100 * SCALE_FACTOR;                        // Largeur du bouton.
    float controlHeight = 40 * SCALE_FACTOR;                       // Hauteur des contrôles.
    float inputSpacing = 10 * SCALE_FACTOR;                        // Espacement entre la boîte de texte et le bouton.

    Rectangle textBoxRect = {inputAreaStartX, inputControlsY, textBoxWidth, controlHeight};                             // Rectangle pour la boîte de texte.
    Rectangle buttonRect = {inputAreaStartX + textBoxWidth + inputSpacing, inputControlsY, buttonWidth, controlHeight}; // Rectangle pour le bouton.

    bool textBoxActive = GuiTextBox(textBoxRect, texteSaisi, 99, GuiIsLocked() ? false : true);             // Crée la boîte de texte et vérifie si elle est active.
    if (!GuiIsLocked() && ((textBoxActive && IsKeyPressed(KEY_ENTER)) || GuiButton(buttonRect, "Valider"))) // Si l'interface n'est pas bloquée et qu'on appuie sur Entrée dans la boîte ou qu'on clique sur Valider.
    {
        if (strlen(texteSaisi) > 0) // Vérifie si du texte a été saisi.
        {
            if (VerifierMot(texteSaisi)) // Vérifie si le mot saisi est valide.
            {
                strcpy(motsSaisis[compteMots], texteSaisi);         // Ajoute le mot trouvé à la liste.
                compteMots++;                                       // Incrémente le compteur de mots trouvés.
                strcpy(message, "Mot accepté !");                   // Affiche un message de succès.
                texteSaisi[0] = '\0';                               // Réinitialise la boîte de texte.
                if (compteMots >= niveauCourant.nombreMotsRequises) // Vérifie si tous les mots requis ont été trouvés.
                {
                    utilisateurCourant.niveauActuel++; // Passe au niveau suivant.
                    afficherNiveauTermine = true;      // Indique qu'il faut afficher l'écran de fin de niveau.
                    afficherEcranJeu = false;          // Ne plus afficher l'écran de jeu.
                    SauvegarderProgressionJeu();       // Sauvegarde la progression.
                }
            }
        }
        else
        {
            strcpy(message, "Entrez un mot"); // Affiche un message d'erreur si aucun mot n'a été entré.
        }
    }

    float submittedLabelY = inputControlsY + (55 * SCALE_FACTOR); // Position Y du label des mots saisis.
    if (compteMots > 0)                                           // Si au moins un mot a été saisi.
    {
        DrawText("Mots saisis:", (screenWidth - MeasureText("Mots saisis:", font20)) / 2, (int)submittedLabelY, font20, COLOR_SECONDARY_TEXT); // Dessine le label.
        const int wordsPerRow = 4;                                                                                                             // Nombre de mots à afficher par ligne.
        float wordSpacingY = 30 * SCALE_FACTOR;                                                                                                // Espacement vertical entre les lignes de mots.
        float interWordSpacingX = 20 * SCALE_FACTOR;                                                                                           // Espacement horizontal entre les mots sur la même ligne.

        float currentLineY = submittedLabelY + (30 * SCALE_FACTOR); // Position Y pour la première ligne de mots.
        int wordIndex = 0;                                          // Index du mot actuel dans le tableau des mots saisis.
        while (wordIndex < compteMots)                              // Boucle pour afficher les mots saisis.
        {
            // Calcul de la largeur de la ligne actuelle
            float currentLineWidth = 0;
            int wordsInThisLine = 0;
            for (int i = 0; i < wordsPerRow && (wordIndex + i) < compteMots; ++i)
            {
                currentLineWidth += MeasureText(motsSaisis[wordIndex + i], font18);
                wordsInThisLine++;
            }
            // Ajout de l'espacement entre les mots de cette ligne (sauf le dernier)
            if (wordsInThisLine > 1)
            {
                currentLineWidth += (wordsInThisLine - 1) * interWordSpacingX;
            }

            float lineStartX = (screenWidth - currentLineWidth) / 2.0f; // Position X de départ de la ligne (centrée).
            if (lineStartX < (10 * SCALE_FACTOR))
                lineStartX = (10 * SCALE_FACTOR); // Empêche d'aller trop à gauche.

            float currentWordDrawX = lineStartX; // Position X actuelle pour dessiner un mot.

            for (int i = 0; i < wordsInThisLine; ++i)
            {
                DrawText(motsSaisis[wordIndex], (int)currentWordDrawX, (int)currentLineY, font18, COLOR_FOUND_TEXT); // Dessine le mot.
                currentWordDrawX += MeasureText(motsSaisis[wordIndex], font18) + interWordSpacingX;                  // Met à jour la position X pour le mot suivant.
                wordIndex++;
            }
            currentLineY += wordSpacingY; // Passe à la ligne suivante.
        }
    }

    float saveButtonWidth = 120 * SCALE_FACTOR;                                                                                                // Largeur du bouton de sauvegarde et de sortie.
    float saveButtonHeight = 40 * SCALE_FACTOR;                                                                                                // Hauteur du bouton de sauvegarde et de sortie.
    float saveButtonMargin = 20 * SCALE_FACTOR;                                                                                                // Marge du bouton de sauvegarde et de sortie.
    Rectangle saveButtonRect = {(float)screenWidth - saveButtonWidth - saveButtonMargin, saveButtonMargin, saveButtonWidth, saveButtonHeight}; // Rectangle pour le bouton.
    if (!GuiIsLocked() && GuiButton(saveButtonRect, "Sauver & Quitter"))                                                                       // Si on clique sur le bouton Sauver & Quitter.
    {
        SauvegarderProgressionJeu();   // Sauvegarde la progression.
        estConnecte = false;           // Déconnecte l'utilisateur.
        afficherEcranConnexion = true; // Affiche l'écran de connexion.
        afficherEcranJeu = false;      // Ne plus afficher l'écran de jeu.
    }

    if (strlen(message) > 0) // Si un message est présent.
    {
        Color messageColor = COLOR_ERROR;                                                 // Couleur par défaut du message (erreur).
        if (strstr(message, "accepté") != NULL || strstr(message, "sauvegardée") != NULL) // Si le message contient "accepté" ou "sauvegardée".
        {
            messageColor = COLOR_SUCCESS; // Change la couleur en succès.
        }
        DrawText(message, (screenWidth - MeasureText(message, font18)) / 2, (int)(screenHeight - (40 * SCALE_FACTOR)), font18, messageColor); // Dessine le message en bas de l'écran.
    }
}
void DessinerEcranNiveauTermine()
{
    const int screenWidth = GetScreenWidth();     // Récupère la largeur de l'écran.
    int titleFontSize = (int)(30 * SCALE_FACTOR); // Taille de la police pour le titre.
    float titleY = 150 * SCALE_FACTOR;            // Position Y du titre.
    float buttonY = 250 * SCALE_FACTOR;           // Position Y du bouton.
    float buttonWidth = 200 * SCALE_FACTOR;       // Largeur du bouton.
    float buttonHeight = 40 * SCALE_FACTOR;       // Hauteur du bouton.

    DrawText("Niveau Terminé !", (screenWidth - MeasureText("Niveau Terminé !", titleFontSize)) / 2, (int)titleY, titleFontSize, COLOR_SUCCESS); // Affichage du titre.

    Rectangle nextLevelButtonRect = {(float)(screenWidth - buttonWidth) / 2.0f, buttonY, buttonWidth, buttonHeight}; // Rectangle pour le bouton "Niveau Suivant".

    if (!GuiIsLocked() && (GuiButton(nextLevelButtonRect, "Niveau Suivant") || IsKeyPressed(KEY_ENTER))) // Si le bouton est cliqué ou Entrée pressée.
    {
        ChargerDonneesNiveau(utilisateurCourant.niveauActuel); // Charge les données du prochain niveau.
        afficherNiveauTermine = false;                         // Ne plus afficher cet écran.
        afficherEcranJeu = true;                               // Afficher l'écran de jeu.
        message[0] = '\0';                                     // Réinitialise le message.
    }
}
void DessinerEcranJeuTermine()
{
    const int screenWidth = GetScreenWidth();
    int titleFontSize = (int)(40 * SCALE_FACTOR);
    int subTitleFontSize = (int)(24 * SCALE_FACTOR);

    float titleY = 100 * SCALE_FACTOR;
    float subTitleY = 150 * SCALE_FACTOR;
    float replayButtonY = 300 * SCALE_FACTOR;
    float menuButtonY = 360 * SCALE_FACTOR;
    float buttonWidth = 200 * SCALE_FACTOR;
    float buttonHeight = 40 * SCALE_FACTOR;

    DrawText("FÉLICITATIONS !", (screenWidth - MeasureText("FÉLICITATIONS !", titleFontSize)) / 2, (int)titleY, titleFontSize, GOLD);
    DrawText("Vous avez terminé tous les niveaux !", (screenWidth - MeasureText("Vous avez terminé tous les niveaux !", subTitleFontSize)) / 2, (int)subTitleY, subTitleFontSize, COLOR_PRIMARY_TEXT);

    Rectangle replayButtonRect = {(float)(screenWidth - buttonWidth) / 2.0f, replayButtonY, buttonWidth, buttonHeight};
    if (!GuiIsLocked() && GuiButton(replayButtonRect, "Rejouer"))
    {
        utilisateurCourant.niveauActuel = 1;
        SauvegarderProgressionJeu(); // Save the reset level
        ChargerDonneesNiveau(1);
        afficherJeuTermine = false;
        afficherEcranJeu = true;
        message[0] = '\0';
    }

    Rectangle menuButtonRect = {(float)(screenWidth - buttonWidth) / 2.0f, menuButtonY, buttonWidth, buttonHeight};
    if (!GuiIsLocked() && GuiButton(menuButtonRect, "Menu Principal"))
    {
        InitialiserJeu(); // This will set afficherEcranConnexion = true
    }
}

// =================================================================================================
// 11. LEVEL DEFINITION FUNCTION IMPLEMENTATIONS
// =================================================================================================
void Niveau_1()
{
    // Copie la chaîne de caractères "TEASRN" dans le tableau caracteres du niveau courant.
    strcpy(niveauCourant.caracteres, "TEASRN");
    // Définit le nombre de mots requis pour ce niveau à 3.
    niveauCourant.nombreMotsRequises = 3;
    // Définit la longueur du premier mot requis à 3.
    niveauCourant.longueursMotsRequises[0] = 3;
    // Définit la longueur du deuxième mot requis à 4.
    niveauCourant.longueursMotsRequises[1] = 4;
    // Note: La longueur du troisième mot requis n'est pas définie ici.
}
void Niveau_2()
{
    strcpy(niveauCourant.caracteres, "CATERS");
    niveauCourant.nombreMotsRequises = 3;
    niveauCourant.longueursMotsRequises[0] = 4;
    niveauCourant.longueursMotsRequises[1] = 5;
}

void Niveau_3()
{
    strcpy(niveauCourant.caracteres, "PLANES");
    niveauCourant.nombreMotsRequises = 4;
    niveauCourant.longueursMotsRequises[0] = 4;
    niveauCourant.longueursMotsRequises[1] = 5;
    niveauCourant.longueursMotsRequises[2] = 6;
}

void Niveau_4()
{
    strcpy(niveauCourant.caracteres, "BRINGE");
    niveauCourant.nombreMotsRequises = 4;
    niveauCourant.longueursMotsRequises[0] = 4;
    niveauCourant.longueursMotsRequises[1] = 5;
    niveauCourant.longueursMotsRequises[2] = 6;
}

void Niveau_5()
{
    strcpy(niveauCourant.caracteres, "DEMANSI");
    niveauCourant.nombreMotsRequises = 4;
    niveauCourant.longueursMotsRequises[0] = 5;
    niveauCourant.longueursMotsRequises[1] = 6;
}

void Niveau_6()
{
    strcpy(niveauCourant.caracteres, "VIOLETS");
    niveauCourant.nombreMotsRequises = 5;
    niveauCourant.longueursMotsRequises[0] = 4;
    niveauCourant.longueursMotsRequises[1] = 5;
    niveauCourant.longueursMotsRequises[2] = 6;
}

void Niveau_7()
{
    strcpy(niveauCourant.caracteres, "CHANTES");
    niveauCourant.nombreMotsRequises = 5;
    niveauCourant.longueursMotsRequises[0] = 5;
    niveauCourant.longueursMotsRequises[1] = 6;
}

void Niveau_8()
{
    strcpy(niveauCourant.caracteres, "RESCUED");
    niveauCourant.nombreMotsRequises = 5;
    niveauCourant.longueursMotsRequises[0] = 5;
    niveauCourant.longueursMotsRequises[1] = 6;
}

void Niveau_9()
{
    strcpy(niveauCourant.caracteres, "TRAINES");
    niveauCourant.nombreMotsRequises = 6;
    niveauCourant.longueursMotsRequises[0] = 5;
    niveauCourant.longueursMotsRequises[1] = 6;
}

void Niveau_10()
{
    strcpy(niveauCourant.caracteres, "ASSIETTE");
    niveauCourant.nombreMotsRequises = 7;
    niveauCourant.longueursMotsRequises[0] = 5;
    niveauCourant.longueursMotsRequises[1] = 6;
}