#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// En-têtes spécifiques à la plateforme pour mkdir
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

// Configuration du dictionnaire
#define MAX_LONGUEUR_MOT 50
#define CAPACITE_DICT_INITIALE 100000
#define NIVEAU_MAX 10
#define LONGUEUR_MOT_MAX_REQUISE 6 // Longueur maximale requise pour un niveau

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

// Variables globales
Utilisateur utilisateurCourant;
Niveau niveauCourant;
char texteSaisi[100] = "";
bool estConnecte = false;
bool afficherEcranConnexion = true;
bool afficherEcranJeu = false;
bool afficherNiveauTermine = false;
bool afficherJeuTermine = false;
char message[100] = "";
char motsSaisis[10][50];
int compteMots = 0;
Dictionnaire dictionnaire = {0};

// Déclarations de fonctions
void InitialiserJeu();
void ChargerDonneesUtilisateur();
void SauvegarderDonneesUtilisateur();
void ChargerDonneesNiveau(int niveau);
void SauvegarderProgressionJeu();
bool VerifierMot(const char *mot);
void DessinerEcranConnexion();
void DessinerEcranJeu();
void DessinerEcranNiveauTermine();
void DessinerEcranJeuTermine();
void TraiterLogiqueJeu();
bool ChargerDictionnaire(const char *fichier);
bool EstMotDansDictionnaire(const char *mot);
void LibererDictionnaire();

// Définitions des niveaux
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

int main(void)
{
    InitWindow(800, 600, "Jeu de Mots");
    SetTargetFPS(60);

    if (!ChargerDictionnaire("words.txt"))
    {
        TraceLog(LOG_WARNING, "Echec du chargement du dictionnaire");
        dictionnaire.capacite = CAPACITE_DICT_INITIALE;
        dictionnaire.mots = (char **)malloc(dictionnaire.capacite * sizeof(char *));

        const char *motsParDefaut[] = {"CAT", "DOG", "SUN", "MOON", "STAR", "TREE", "BOOK", "BALL", "FISH", "BIRD", "GOD"};
        dictionnaire.taille = sizeof(motsParDefaut) / sizeof(motsParDefaut[0]);

        for (int i = 0; i < dictionnaire.taille; i++)
        {
            dictionnaire.mots[i] = strdup(motsParDefaut[i]);
        }
    }

    InitialiserJeu();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (afficherEcranConnexion)
        {
            DessinerEcranConnexion();
        }
        else if (afficherNiveauTermine)
        {
            DessinerEcranNiveauTermine();
        }
        else if (afficherJeuTermine)
        {
            DessinerEcranJeuTermine();
        }
        else if (afficherEcranJeu)
        {
            DessinerEcranJeu();
            TraiterLogiqueJeu();
        }

        EndDrawing();
    }

    LibererDictionnaire();
    CloseWindow();
    return 0;
}

bool ChargerDictionnaire(const char *fichier)
{
    FILE *fichierDico = fopen(fichier, "r");
    if (!fichierDico)
        return false;

    char tampon[MAX_LONGUEUR_MOT];
    dictionnaire.capacite = CAPACITE_DICT_INITIALE;
    dictionnaire.mots = (char **)malloc(dictionnaire.capacite * sizeof(char *));
    dictionnaire.taille = 0;

    while (fgets(tampon, MAX_LONGUEUR_MOT, fichierDico))
    {
        size_t longueur = strlen(tampon);
        while (longueur > 0 && (tampon[longueur - 1] == '\n' || tampon[longueur - 1] == '\r'))
        {
            tampon[--longueur] = '\0';
        }

        if (longueur < 2 || longueur > LONGUEUR_MOT_MAX_REQUISE)
            continue;

        for (char *p = tampon; *p; p++)
            *p = toupper(*p);

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
            continue;

        if (dictionnaire.taille >= dictionnaire.capacite)
        {
            dictionnaire.capacite *= 2;
            char **nouveauxMots = (char **)realloc(dictionnaire.mots, dictionnaire.capacite * sizeof(char *));
            if (!nouveauxMots)
            {
                fclose(fichierDico);
                return false;
            }
            dictionnaire.mots = nouveauxMots;
        }

        dictionnaire.mots[dictionnaire.taille] = strdup(tampon);
        if (!dictionnaire.mots[dictionnaire.taille])
        {
            fclose(fichierDico);
            return false;
        }
        dictionnaire.taille++;
    }

    fclose(fichierDico);
    return true;
}

void LibererDictionnaire()
{
    if (dictionnaire.mots)
    {
        for (int i = 0; i < dictionnaire.taille; i++)
            free(dictionnaire.mots[i]);
        free(dictionnaire.mots);
    }
    dictionnaire.mots = NULL;
    dictionnaire.taille = 0;
    dictionnaire.capacite = 0;
}

bool EstMotDansDictionnaire(const char *mot)
{
    if (dictionnaire.taille == 0)
        return true;

    char motMaj[MAX_LONGUEUR_MOT];
    strcpy(motMaj, mot);
    for (char *p = motMaj; *p; p++)
        *p = toupper(*p);

    for (int i = 0; i < dictionnaire.taille; i++)
    {
        if (strcmp(motMaj, dictionnaire.mots[i]) == 0)
            return true;
    }

    return false;
}

void InitialiserJeu()
{
    memset(&utilisateurCourant, 0, sizeof(Utilisateur));
    memset(&niveauCourant, 0, sizeof(Niveau));
    estConnecte = false;
    afficherEcranConnexion = true;
    afficherEcranJeu = false;
    afficherNiveauTermine = false;
    afficherJeuTermine = false;
    compteMots = 0;
    memset(texteSaisi, 0, sizeof(texteSaisi));
    memset(message, 0, sizeof(message));
    memset(motsSaisis, 0, sizeof(motsSaisis));
}

void ChargerDonneesUtilisateur()
{
    char nomFichier[100];
    sprintf(nomFichier, "users/%s.dat", utilisateurCourant.nomUtilisateur);

    FILE *fichier = fopen(nomFichier, "rb");
    if (fichier)
    {
        fread(&utilisateurCourant, sizeof(Utilisateur), 1, fichier);
        fclose(fichier);
        estConnecte = true;
        afficherEcranConnexion = false;
        afficherEcranJeu = true;
        ChargerDonneesNiveau(utilisateurCourant.niveauActuel);
        strcpy(message, "Bienvenue ! Reprise de la partie.");
    }
    else
    {
        strcpy(message, "Utilisateur non trouvé. Inscrivez-vous.");
    }
}

void SauvegarderDonneesUtilisateur()
{
    char nomDossier[100] = "users";
#ifdef _WIN32
    _mkdir(nomDossier);
#else
    mkdir(nomDossier, 0777);
#endif

    char nomFichier[100];
    sprintf(nomFichier, "users/%s.dat", utilisateurCourant.nomUtilisateur);

    FILE *fichier = fopen(nomFichier, "wb");
    if (fichier)
    {
        utilisateurCourant.niveauActuel = 1;
        fwrite(&utilisateurCourant, sizeof(Utilisateur), 1, fichier);
        fclose(fichier);
        estConnecte = true;
        afficherEcranConnexion = false;
        afficherEcranJeu = true;
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
    }

    niveauCourant.niveau = niveau;
}

void SauvegarderProgressionJeu()
{
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

bool VerifierMot(const char *mot)
{
    int longueurMot = strlen(mot);
    if (longueurMot < 2)
    {
        strcpy(message, "Mot trop court (min 2 lettres)");
        return false;
    }

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

    int compteCaracteres[26] = {0};
    for (int i = 0; niveauCourant.caracteres[i] != '\0'; i++)
    {
        if (niveauCourant.caracteres[i] >= 'A' && niveauCourant.caracteres[i] <= 'Z')
        {
            compteCaracteres[niveauCourant.caracteres[i] - 'A']++;
        }
    }

    for (int i = 0; i < longueurMot; i++)
    {
        char c = mot[i];
        if (c >= 'a' && c <= 'z')
            c = c - 'a' + 'A';

        if (c < 'A' || c > 'Z')
        {
            strcpy(message, "Caractère invalide");
            return false;
        }

        if (compteCaracteres[c - 'A'] <= 0)
        {
            strcpy(message, "Lettre non disponible");
            return false;
        }

        compteCaracteres[c - 'A']--;
    }

    for (int i = 0; i < compteMots; i++)
    {
        if (strcasecmp(motsSaisis[i], mot) == 0)
        {
            strcpy(message, "Mot déjà utilisé");
            return false;
        }
    }

    if (!EstMotDansDictionnaire(mot))
    {
        strcpy(message, "Mot non valide");
        return false;
    }

    return true;
}

void DessinerEcranConnexion()
{
    DrawText("Jeu de Mots - Connexion", 300, 50, 24, BLACK);
    DrawText("Nom d'utilisateur:", 250, 150, 20, DARKGRAY);
    GuiTextBox((Rectangle){250, 180, 300, 40}, utilisateurCourant.nomUtilisateur, 50, true);

    if (GuiButton((Rectangle){250, 240, 140, 40}, "Connexion"))
    {
        if (strlen(utilisateurCourant.nomUtilisateur) > 0)
            ChargerDonneesUtilisateur();
        else
            strcpy(message, "Entrez un nom d'utilisateur");
    }

    if (GuiButton((Rectangle){410, 240, 140, 40}, "Inscription"))
    {
        if (strlen(utilisateurCourant.nomUtilisateur) > 0)
            SauvegarderDonneesUtilisateur();
        else
            strcpy(message, "Entrez un nom d'utilisateur");
    }

    DrawText(message, 250, 550, 18, RED);
}

void DessinerEcranJeu()
{
    // Éléments UI alignés à gauche
    DrawText(TextFormat("Niveau: %d/%d", niveauCourant.niveau, NIVEAU_MAX), 50, 30, 24, BLACK);
    DrawText(TextFormat("Joueur: %s", utilisateurCourant.nomUtilisateur), 50, 60, 20, DARKGRAY);

    DrawText("Caractères disponibles:", 50, 100, 20, DARKGRAY);
    for (int i = 0; niveauCourant.caracteres[i] != '\0'; i++)
    {
        DrawRectangle(50 + i * 60, 130, 50, 50, LIGHTGRAY);
        DrawText(TextFormat("%c", niveauCourant.caracteres[i]), 65 + i * 60, 145, 30, BLACK);
    }

    DrawText("Formez des mots de ces longueurs:", 50, 200, 20, DARKGRAY);
    int x = 50;
    for (int i = 0; i < 5 && niveauCourant.longueursMotsRequises[i] > 0; i++)
    {
        DrawText(TextFormat("%d lettres", niveauCourant.longueursMotsRequises[i]), x, 230, 18, DARKGRAY);
        x += 120;
    }

    DrawText(TextFormat("Mots trouvés: %d/%d", compteMots, niveauCourant.nombreMotsRequises), 50, 260, 20, DARKGRAY);

    DrawText("Entrez un mot:", 50, 300, 20, DARKGRAY);
    bool textBoxActive = GuiTextBox((Rectangle){50, 330, 300, 40}, texteSaisi, 99, true);

    if ((textBoxActive && IsKeyPressed(KEY_ENTER)) || GuiButton((Rectangle){360, 330, 100, 40}, "Valider"))
    {
        if (strlen(texteSaisi) > 0)
        {
            if (VerifierMot(texteSaisi))
            {
                strcpy(motsSaisis[compteMots], texteSaisi);
                compteMots++;
                strcpy(message, "Mot accepté !");
                texteSaisi[0] = '\0';

                if (compteMots >= niveauCourant.nombreMotsRequises)
                {
                    utilisateurCourant.niveauActuel++;
                    afficherNiveauTermine = true;
                    afficherEcranJeu = false;
                    SauvegarderProgressionJeu();
                }
            }
        }
        else
        {
            strcpy(message, "Entrez un mot");
        }
    }

    DrawText("Mots saisis:", 50, 390, 20, DARKGRAY);
    for (int i = 0; i < compteMots; i++)
    {
        DrawText(motsSaisis[i], 50 + (i % 3) * 150, 420 + (i / 3) * 30, 18, DARKBLUE);
    }

    if (GuiButton((Rectangle){650, 30, 120, 40}, "Sauver & Quitter"))
    {
        SauvegarderProgressionJeu();
        estConnecte = false;
        afficherEcranConnexion = true;
        afficherEcranJeu = false;
    }

    DrawText(message, 50, 550, 18, RED);
}

void DessinerEcranNiveauTermine()
{
    DrawText("Niveau Terminé !", 300, 150, 30, GREEN);

    if (GuiButton((Rectangle){300, 250, 200, 40}, "Niveau Suivant"))
    {
        ChargerDonneesNiveau(utilisateurCourant.niveauActuel);
        afficherNiveauTermine = false;
        afficherEcranJeu = true;
    }
}

void DessinerEcranJeuTermine()
{
    DrawText("FÉLICITATIONS !", 250, 100, 40, GOLD);
    DrawText("Vous avez terminé tous les niveaux !", 250, 150, 24, BLACK);

    if (GuiButton((Rectangle){300, 300, 200, 40}, "Rejouer"))
    {
        utilisateurCourant.niveauActuel = 1;
        ChargerDonneesNiveau(1);
        afficherJeuTermine = false;
        afficherEcranJeu = true;
    }

    if (GuiButton((Rectangle){300, 360, 200, 40}, "Menu Principal"))
    {
        InitialiserJeu();
        afficherJeuTermine = false;
        afficherEcranConnexion = true;
    }
}

void TraiterLogiqueJeu()
{
    // Logique de jeu sans timer
}

// Définitions des niveaux (modifiés pour ne pas nécessiter de mots > 6 lettres)
void Niveau_1()
{
    strcpy(niveauCourant.caracteres, "TEASRN");
    niveauCourant.nombreMotsRequises = 3;
    niveauCourant.longueursMotsRequises[0] = 3;
    niveauCourant.longueursMotsRequises[1] = 4;
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