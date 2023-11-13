/**
* @file main.c
* Interpréteur de commande d'une formation
* @author CZYKINOWSKI Seweryn - 107 / MAHAMAT Issa - 107
* @version 1.0 11/11/2022
*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#pragma warning(disable:4996)

enum {
    NB_SEMESTRES = 2,
    MIN_UE = 3,
    MAX_UE = 6,
    MAX_MATIERES = 10,
    MAX_EPREUVES = 5,
    MAX_ETUDIANTS = 100,
    MAX_CHAR = 30, // Longueur de chaîne de caractère maximal
    VALEUR_NON_EXISTANTE = -1, // Indiaue qu'un element n'existe pas
    MAX_NOTES = MAX_MATIERES * MAX_EPREUVES * NB_SEMESTRES, // Maximum de note
};

const float MIN_NOTE = 0.f, MAX_NOTE = 20.f; // Bornes des notes

typedef	unsigned int uint; // raccourci d'écriture pour alléger le code
typedef const char cchar; // raccourci d'écriture pour alléger le code
typedef enum{true=1,false=0}bool; // Type booléen vrai / faux

typedef struct {
    char nomEpreuve[MAX_CHAR];
    float coeff[MAX_UE];
} Epreuve;

typedef struct {
    int nbEpreuves; // Nombre d'épreuves de la matière
    char nomMatiere[MAX_CHAR];
    Epreuve epreuve[MAX_EPREUVES];
} Matiere;

typedef struct {
    int nbMatieres; // Nombre de matière du semestre
    Matiere matiere[MAX_MATIERES];
} Semestre;

typedef struct {
    char nom[MAX_CHAR];
    float notes[MAX_NOTES];
    /* Nombre maximum de notes qu'un étudiant peut avoir*/
} Etudiant;

typedef struct {
    int nbUe; // Nombre d'UE
    int nbEtu; // Nombre d'étudiants
    Semestre semestres[NB_SEMESTRES];
    Etudiant etudiant[MAX_ETUDIANTS];
} Formation;

void initialisationFormation(Formation *f) {
    /**
     * Initialise le nombre d'UE, le nombre d'étudiant, le nombre de matières
     * de chaque semestre et le nombre d'épreuve de chaque matière à 0
     * [in][out] Formation *f, formation à initialiser
     */
    f->nbUe = 0;
    f->nbEtu = 0;
    for (uint sem = 0; sem < NB_SEMESTRES; ++sem) {
        f->semestres[sem].nbMatieres = 0;
        for (uint mat = 0; mat < MAX_MATIERES; ++mat) {
            f->semestres[sem].matiere[mat].nbEpreuves = 0;
        }
    }
}

void defUe(Formation *f) {
    /**
     * Définie le nombre d'UE de la formation.
     * Affiche des messages d'erreurs si le nombre d'UE a déjà été défini
     * ou si le nombre d'UE entrée ne respecte pas les conditions (bornes)
     * [in][out] Formation *f, formation où le nombre d'UE est défini
     */
    int nbUeDef;
    scanf("%d", &nbUeDef);
    if (f->nbUe == 0) {
        if (nbUeDef > MAX_UE || nbUeDef < MIN_UE) {
            printf("Le nombre d'UE est incorrect\n");
        } else {
            f->nbUe = nbUeDef;
            printf("Le nombre d'UE est defini\n");
        }
    } else {
        printf("Le nombre d'UE est deja defini\n");
    }
}

bool coeffIncorrect(const Formation *f, uint numSem) {
    /**
     * Vérifie si les coefficients d'un semestre sont incorrects
     * [in] const Formation *f, formation étudié
     * [in] uint numSem, numéro du semestre où on vérifie les coeff
     * Retourne true si les coefficients sont incorrectes et false sinon
     */
    const Semestre* s=&(f->semestres[numSem]);
    float coef, sumCf;
    for (uint ue = 0; ue < f->nbUe; ++ue) {
        sumCf = 0;
        for (uint mat = 0; mat < s->nbMatieres; ++mat) {
            const Matiere* m=&(s->matiere[mat]);
            for (uint epr = 0; epr < m->nbEpreuves; ++epr) {
                const Epreuve* e=&(m->epreuve[epr]);
                coef=e->coeff[ue];
                if (coef<0) {
                    return true;
                }
                sumCf += coef;
            }
        }
        if (sumCf == 0) {
            return true;
        }
    }
    return false;
}

void verifCoeff(const Formation *f) {
    /**
     * Vérifie la correction des coefficients dans la formation en paramètre
     * pour un semestre indiquée en entrée utilisateur.
     * Affiche des messages d'erreurs si l'entrée ou les coefficients sont
     * incorrectes, sinon affiche que les coef sont correctes
     * [in] const Formation *f, formation étudié
     */
    uint numSem;
    scanf("%d", &numSem);
    numSem -= 1;
    // Test des conditions des coefficients
    if (f->nbUe == 0) {
        printf("Le nombre d'UE n'est pas defini\n");
    } else if (numSem > 1 || numSem < 0) {
        printf("Le numero de semestre est incorrect\n");
    } else if (f->semestres[numSem].nbMatieres == 0) {
        printf("Le semestre ne contient aucune epreuve\n");
    } else if (coeffIncorrect(f, numSem)) {
        printf("Les coefficients d'au moins une UE de ce semestre sont tous nuls\n");
    } else {
        printf("Coefficients corrects\n");
    }
}

int matiereExiste(const Formation *f, uint numSem, cchar matT[]) {
    /**
     * Vérifie si une matière existe
     * [in] const Formation *f, formation étudié
     * [in] uint numSem, numéro de semestre pour indiquer le semestre étudié
     * [in] cchar matT[], nom de la matière dont on test l'existence
     * Renvoie l'indice de la matière si elle existe et VALEUR_NON_EXISTANTE
     * sinon pour indiquer que la matière n'existe pas
     */
    const Semestre *s=&(f->semestres[numSem]);
    for (int mat = 0; mat < s->nbMatieres; ++mat) {
        const Matiere *m=&(s->matiere[mat]);
        if (strcmp(m->nomMatiere, matT) == 0) {
            return mat;
        }
    }
    return VALEUR_NON_EXISTANTE;
}

int epreuveExiste(const Formation *f, uint numSem, cchar matT[], cchar eprT[]){
    /**
     * Vérifie si une épreuve existe
     * [in] const Formation *f, formation étudié
     * [in] uint numSem, numéro de semestre étudié
     * [in] cchar matT[], nom de la matière testé
     * [in] cchar eprT[], nom de l'epreuve dont on test l'existence
     * Renvoie l'indice de l'épreuve si elle existe et
     * VALEUR_NON_EXISTANTE sinon
     */
    const Semestre *s=&(f->semestres[numSem]);
    for (int mat = 0; mat < s->nbMatieres; ++mat) {
        const Matiere *m=&(s->matiere[mat]);
        if (strcmp(m->nomMatiere, matT) == 0) {
            for (int epr = 0; epr < m->nbEpreuves; ++epr) {
                const Epreuve *e=&(m->epreuve[epr]);
                if (strcmp(e->nomEpreuve, eprT) == 0) {
                    return epr;
                }
            }
        }
    }
    return VALEUR_NON_EXISTANTE;
}

bool ajoutCoeffImpossible(const Formation *f, const float coeff[]) {
    /**
     * Vérifie si l'ajout de coefficients est ipossible
     * [in] const Formation *f, formation étudié
     * [in] const float coeff[], tableau avec coefficients à ajouter
     * Renvoie 1 si l'ajout est impossibl et 0 sinon
     */
    float sumCf = 0;
    for (uint i = 0; i < f->nbUe; ++i) {
        if (coeff[i] < 0) {
            return true;
        }
        sumCf += coeff[i];
    }
    if (sumCf == 0) { /* Si la somme est egal à 0 c'est que tout les
        coefficients sont égals à 0 */
        return true;
    }
    return false;
}


void ajoutEpreuve(Formation *f) {
    /**
     * Ajoute l'epreuve entree par l'utilisateur
     * Affiche des messages si l'ajout est effectué ou
     * si les conditions d'ajout ne sont pas respectés
     * [in][out] Formation *f, formation étudié où l'épreuve est ajouté
     */
    // Définition des variables
    uint numSem;
    cchar mat[MAX_CHAR];
    cchar epr[MAX_CHAR];
    float coeff[MAX_UE];
    scanf("%d %s %s", &numSem, &mat, &epr);
    for (int i = 0; i < f->nbUe; ++i) {
        scanf("%f", &coeff[i]);
    }
    numSem -= 1;
    // Test des erreurs possibles
    if (f->nbUe == 0) {
        printf("Le nombre d'UE n'est pas defini\n");
    } else if (numSem > 1 || numSem < 0) {
        printf("Le numero de semestre est incorrect\n");
    } else if (epreuveExiste(f, numSem, mat, epr) != VALEUR_NON_EXISTANTE) {
        printf("Une meme epreuve existe deja\n");
    } else if (ajoutCoeffImpossible(f, coeff)) {
        printf("Au moins un des coefficients est incorrect\n");
    } else {
        // Ajoute de l'épreuve et/ou de la matière
        Semestre *s=&(f->semestres[numSem]);
        int idMat = matiereExiste(f, numSem, mat);
        if (idMat == VALEUR_NON_EXISTANTE) { // La matière existe pas, ajout
            idMat = s->nbMatieres++;
            printf("Matiere ajoutee a la formation\n");
            strcpy(s->matiere[idMat].nomMatiere, mat);
        }
        int idEp = s->matiere[idMat].nbEpreuves++; /* On récupère et
        incrémente le nombre d'épreuves de la matière */
        strcpy(s->matiere[idMat].epreuve[idEp].nomEpreuve, epr);
        for (int i = 0; i < f->nbUe; ++i) { // Ajout de coeff
            s->matiere[idMat].epreuve[idEp].coeff[i] = coeff[i];
        }
        printf("Epreuve ajoutee a la formation\n");
    }
}

void initialisationEtudiant(Formation *f, cchar nomEtu[]) {
    /**
     * Initialise un etudiant et incrémente le nombre d'étudiant
     * [in][out] Formation *f, formation étudié
     * [in] cchar nomEtu[], nom de l'étudaint intialisé
     */
    uint idEtu = f->nbEtu;
    strcpy(f->etudiant[idEtu].nom, nomEtu);
    for (int i = 0; i < (MAX_NOTES); ++i) {
        f->etudiant[idEtu].notes[i] = VALEUR_NON_EXISTANTE; /* Aucune note */
    }
}

int etudiantExiste(const Formation *f, cchar nomEtu[]) {
    /**
     * Verifie si l'etudiant existe dans la formation
     * [in] const Formation *f, formation etudié
     * [in] cchar nomEtu[], nom de l'étudiant à ajouté
     * Renvoie l'indice de l'etudiant si il existe dans la structure
     * et VALEUR_NON_EXISTANTE sinon
     */
    for (int i = 0; i < f->nbEtu; ++i) {
        if (strcmp(f->etudiant[i].nom, nomEtu) == 0) {
            return i;
        }
    }
    return VALEUR_NON_EXISTANTE;
}

bool noteExiste(const Formation *f, int idEtu, uint idN) {
    /**
     * Verifie si la note existe au sein de la formation
     * [in] const Formation *f, formation étudié
     * [in] int idEtu, id de l'étudiant étudié
     * [in] int idN, id de la note testé
     * Renvoie TRUE si la note existe et FALSE si la note n'existe pas
     */
    return f->etudiant[idEtu].notes[idN] != VALEUR_NON_EXISTANTE;
}

uint idNote(uint idSem, int idMat, int idEpr) {
    /**
     * Calcul l'id d'une note
     * [in] int idSem, id du semestre
     * [in] int idMat, id de la matière
     * [in] int idEpr, id de l'épreuve
     * Renvoie l'id de la note associé aux paramètres
     */
    uint id = 0;
    id += (MAX_EPREUVES * MAX_MATIERES) * idSem; // 50 note par semestre
    id += (MAX_EPREUVES) * idMat; // 10 note par matiere
    id += idEpr; // 1 note par epreuve
    return id;
}

void ajoutNote(Formation *f) {
    /**
     * Ajoute une note à la formation
     * Affiche des messages en fonction de si l'ajout est possible ou sinon les
     * erreurs de saisie qui empêchent l'ajout
     * [in] Formation *f, formation étudié
     */
    // Définition des variables et récupération des valeurs
    uint numSem;
    char nomEtu[MAX_CHAR];
    char nomMat[MAX_CHAR];
    char nomEpr[MAX_CHAR];
    float note;
    scanf("%d %s %s %s %f", &numSem, &nomEtu, &nomMat, &nomEpr, &note);
    numSem -= 1;
    int idMat = matiereExiste(f, numSem, nomMat);
    int idEpr = epreuveExiste(f, numSem, nomMat, nomEpr);
    int idEtu = etudiantExiste(f, nomEtu);
    // Test des erreurs possibles
    if (numSem > 1 || numSem < 0) {
        printf("Le numero de semestre est incorrect\n");
    }
    else if (note > MAX_NOTE || MIN_NOTE > note) {
        printf("Note incorrecte\n");
    }
    else if (idMat == VALEUR_NON_EXISTANTE) {
        printf("Matiere inconnue\n");
    }
    else if (idEpr == VALEUR_NON_EXISTANTE) {
        printf("Epreuve inconnue\n");
    }
    else {
        // Traitement de la commande
        uint idN = idNote(numSem, idMat, idEpr);
        if (idEtu == VALEUR_NON_EXISTANTE) {
            initialisationEtudiant(f, nomEtu);
            f->etudiant[f->nbEtu++].notes[idN] = note;
            printf("Etudiant ajoute a la formation\n");
            printf("Note ajoutee a l'etudiant\n");
        } else if (noteExiste(f, idEtu, idN)) {
            printf("Une note est deja definie pour l'etudiant\n");
        } else {
            f->etudiant[idEtu].notes[idN] = note;
            printf("Note ajoutee a l'etudiant\n");
        }
    }
}

bool notesExiste(const Formation *f, int idEtu, uint numSem) {
    /**
     * Verifie si l'etudiant indiquée par idEtu a des notes rentrees pour
     * toutes les epreuves definies dans le semestre indiquée par numSem
     * [in] Formation *f, formation étudié
     * [in] int idEtu, id de l'étudiant étudié
     * [in] uint numSem, numéro du semestre étudié
     * Renvoie true si c'est le cas ou false sinon.
     */
    const Semestre* sem=&(f->semestres[numSem]);
    for (int idMat = 0; idMat < sem->nbMatieres; ++idMat) {
        const Matiere* mat =&(sem->matiere[idMat]);
        for (int idEpr = 0; idEpr < mat->nbEpreuves; ++idEpr) {
            uint idN = idNote(numSem, idMat, idEpr);
            if (noteExiste(f, idEtu, idN)==false) {
                return false;
            }
        }
    }
    return true;
}

void verifNotes(const Formation *f) {
    /**
     * Verifie la correction des notes d'un etudiant.
     * [in][out] Formation *f, formation étudié
     * Affiche des messages d'erreurs si le numero de semestre est incorrect
     * si l'etudiant n'existe pas ou si il n'a pas toutes les notes entrées*/
    // Définition des variables
    uint numSem;
    char nomEtu[MAX_CHAR];
    scanf("%d %s", &numSem, &nomEtu);
    numSem -= 1;
    int idEtu = etudiantExiste(f, nomEtu);
    // Traitement de la commande
    if (numSem > 1 || numSem < 0) {
        printf("Le numero de semestre est incorrect\n");
    } else if (idEtu == VALEUR_NON_EXISTANTE) {
        printf("Etudiant inconnu\n");
    } else if (notesExiste(f, idEtu, numSem)) {
        printf("Notes correctes\n");
    } else {
        printf("Il manque au moins une note pour cet etudiant\n");
    }
}

int lenMatiereMax(const Formation* f, uint numSem) {
    /**
     * Calcule la longueur du plus long nom de matière
     * [in] Formation* f, formation étudié
     * [in] uint numSem, numéro du semestre étudié
     * Renvoie cette longueur maximal
     */
    const Semestre* sem=&(f->semestres[numSem]);
    int lenMax=strlen("Moyennes");
    for (int mat=0; mat<sem->nbMatieres;++mat) {
        int lenMat=strlen(sem->matiere[mat].nomMatiere);
        if (lenMat>lenMax) {
            lenMax=lenMat;
        }
    }
    return lenMax+1; // On rajoute 1 pour inclure l'espace
}

void afficheNbUE(int nbUe) {
    /**
     * Gère l'affichage des UE pour les tableaux releve et decision
     * [in] int nbUe
     */
    for (int ue=0; ue<nbUe; ++ue) {
        printf("%3s%-2d","UE",ue+1);
    }
    printf("\n");
}

void moyennesUe(const Formation *f, uint numSem, int idEtu) {
    /**
     * Réalise le calcul des moyennes d'un étudiant pour un semestre
     * Affiche les moyennes de chaque matière à chaque UE, et la moyenne
     * des UE
     * [in] const Formation *f, formation étudié
     * [in] uint numSem, numéro de semestre étudié
     * [in] int idEtu, ID de l'étudiant étudié
     */
    // Définition des variables
    float sMat; // Somme des notes de matières
    float sCoeffMat; // Somme des coefficients par matières
    const Semestre *sem = &(f->semestres[numSem]);
    float sUe[MAX_UE]; // Somme des notes pour l'UE
    float sCoeffUe[MAX_UE]; // Somme des coefficients pour les UE
    for (int i = 0; i < f->nbUe; ++i) {
        sUe[i] = 0; // On initialise tout à 0
        sCoeffUe[i] = 0;
    }
    int lenMax=lenMatiereMax(f,numSem);
    printf("%*s",lenMax+1,"");
    afficheNbUE(f->nbUe);
    // Calcul des moyennes
    for (int mat = 0; mat < sem->nbMatieres; ++mat) {
        const Matiere *m = &(sem->matiere[mat]);
        int lenMat= strlen(m->nomMatiere);
        printf("%s%*s", m->nomMatiere,lenMax-lenMat,"");
        for (int ue = 0; ue < f->nbUe; ++ue) {
            sMat = 0;
            sCoeffMat = 0;
            for (int epr = 0; epr < m->nbEpreuves; ++epr) {
                const Epreuve *e = &(m->epreuve[epr]);
                uint idN = idNote(numSem, mat, epr);
                float coeffSel = e->coeff[ue];
                float noteSel = f->etudiant[idEtu].notes[idN] * coeffSel;
                sMat += noteSel;
                sCoeffMat += coeffSel;
            }
            // Affichage des moyennes par 
            if (sCoeffMat == 0) {
                printf("%5s","ND ");
            }
            else {
                float moy=floorf(sMat / sCoeffMat * 10.f) / 10.f;
                printf("%4.1f ", moy);
                sUe[ue] += sMat;
                sCoeffUe[ue] += sCoeffMat;
            }
        }
        printf("\n");
    }
    /* Affichage des moyennes par UE */
    printf("--\nMoyennes%*s",lenMax-strlen("Moyennes")+1,"");
    for (int ue = 0; ue < f->nbUe; ++ue) {
        float moy= floorf(sUe[ue] / sCoeffUe[ue] * 10.f) / 10.f;
        printf("%4.1f ",moy);
    }
    printf("\n");
}

void releve(const Formation *f) {
    /**
     * Traitement de la commande releve
     * const Formation *f [in] formation étudié
     * affiche des erreurs si il y a des informations erronées
     * ou pas disponibles pour le releve
     */
    uint numSem;
    char etu[MAX_CHAR];
    scanf("%d %s", &numSem, &etu);
    numSem -= 1;
    int idEtu = etudiantExiste(f, etu);
    if (numSem > 1 || numSem < 0) {
        printf("Le numero de semestre est incorrect\n");
    } else if (coeffIncorrect(f, numSem)) {
        printf("Les coefficients de ce semestre sont incorrects\n");
    } else if (idEtu == VALEUR_NON_EXISTANTE) {
        printf("Etudiant inconnu\n");
    } else if (!notesExiste(f, idEtu, numSem)) {
        printf("Il manque au moins une note pour cet etudiant\n");
    } else {
        moyennesUe(f, numSem, idEtu);
    }
}

void moyennes_annee(const Formation *f, int idEtu, float tabM[]) {
    /**
     * Réalise le calul et l'affichage de moyennes des UE à chaque semestre
     * pour l'etudiant indqiuée par idEtu dans la formation
     * [in] const Formation *f, formation étudié
     * [in] idEtu, ID de l'étudiant étudié
     * [out] tabM, tableau des moyennes de l'année
     */
    int lenMax=strlen("Moyennes annuelles");
    printf("%*s",lenMax+1,"");
    afficheNbUE(f->nbUe);
    float sUe;
    float sCoeffUe;
    float moyennesUe[NB_SEMESTRES][MAX_UE];
    /* Calcul des moyennes des UE et Semestres */
    for (int sem = 0; sem < NB_SEMESTRES; ++sem) {
        printf("S%d%*s", sem + 1,lenMax-1,"");
        const Semestre *s = &(f->semestres[sem]);
        for (int ue = 0; ue < f->nbUe; ++ue) {
            sUe = 0;
            sCoeffUe = 0;
            for (int mat = 0; mat < s->nbMatieres; ++mat) {
                const Matiere *m = &(s->matiere[mat]);
                for (int epr = 0; epr < m->nbEpreuves; ++epr) {
                    const Epreuve *e = &(m->epreuve[epr]);
                    uint idN = idNote(sem, mat, epr);
                    float coeffSel = e->coeff[ue];
                    float noteSel = f->etudiant[idEtu].notes[idN] * coeffSel;
                    sUe += noteSel;
                    sCoeffUe += coeffSel;
                }
            }
            float moyUe = floorf(sUe / sCoeffUe * 10.f) / 10.f;
            moyennesUe[sem][ue] = sUe / sCoeffUe;
            printf("%4.1f ", moyUe);
        }
        printf("\n");
    }
    for (int ue=0;ue<f->nbUe; ++ue) {
        tabM[ue]= floorf((moyennesUe[0][ue] + moyennesUe[1][ue]) / 2 * 10.f) / 10.f;
    }
}

void affichageDecisionFinale(float tabM[], int nbUe) {
    /**
     * Realise l'affichage de moyennes annuelles
     * ainsi que de la décision finale
     * [in] float tabM[], tableau des moyennes annuelles
     * [in] int nbUe, nombre d'UE de la formation
     */
    char acquisition[MAX_CHAR]=""; /* On ajoutera au fur et à mesure les
    UE acquis dans la chaîne directement pour un affichage ultérieur*/
    int lenMax=strlen("Moyennes annuelles");
    printf("--\nMoyennes annuelles ");
    float ueAcquises = 0;
    bool ueAcquis=false;
    for (int ue = 0; ue < nbUe; ++ue) {
        if (tabM[ue] >= 10.0) {
            if (ueAcquis == false) {
                ueAcquis=true;
            }
            else {
                sprintf(acquisition+strlen(acquisition), ", ");
            }
            ueAcquises += 1;
            sprintf(acquisition+strlen(acquisition), "UE%d",ue+1);
        }
        else {
            printf(" ");
        }
        printf("%2.1f ", tabM[ue]);
    }
    printf("\nAcquisition");
    printf("%*s",lenMax-strlen("Acquisition")+1,"");
    if (ueAcquises == 0) {
        printf("Aucune\n");
    }
    else {
        printf("%s\n", acquisition);
    }
    printf("Devenir%*s",lenMax-strlen("Devenir")+1,"");
    if ((ueAcquises / nbUe) <= 0.5) {
        printf("Redoublement\n");
    } else {
        printf("Passage\n");
    }
}

void decision(const Formation *f) {
    /**
     * Réalise le traitement pour la commande decision
     * Affiche des messages d'erreurs si les données dans la structure
     * formation sont éronnés ou incomplètes ou si l'entrée est éronné
     * [in] Formation *f, formation étudié
     */
    char etu[MAX_CHAR];
    scanf("%s", etu);
    int idEtu = etudiantExiste(f, etu);
    if (idEtu == VALEUR_NON_EXISTANTE) {
        printf("Etudiant inconnu\n");
    } else if (coeffIncorrect(f, 0) || coeffIncorrect(f, 1)) {
        printf("Les coefficients d'au moins un semestre sont incorrects\n");
    } else if (!notesExiste(f, idEtu, 0) || !notesExiste(f, idEtu, 1)) {
        printf("Il manque au moins une note pour cet etudiant\n");
    } else {
        float tabMoyennes[MAX_UE];
        /* On utilise un tableau pour stocker les moyennes d'UE à chaque
         * semestre pour pouvoir réaliser la moyenne pour chaque UE */
        moyennes_annee(f, idEtu, tabMoyennes);
        affichageDecisionFinale(tabMoyennes, f->nbUe);
    }
}

int main() {
    Formation formation;
    initialisationFormation(&formation);
    char cde[MAX_CHAR]; // Stockage de la commande
    do {
        scanf("%s", cde);
        if (strcmp(cde, "formation") == 0) {
            defUe(&formation);
        } else if (strcmp(cde, "epreuve") == 0) {
            ajoutEpreuve(&formation);
        } else if (strcmp(cde, "coefficients") == 0) {
            verifCoeff(&formation);
        } else if (strcmp(cde, "note") == 0) {
            ajoutNote(&formation);
        } else if (strcmp(cde, "notes") == 0) {
            verifNotes(&formation);
        } else if (strcmp(cde, "releve") == 0) {
            releve(&formation);
        } else if (strcmp(cde, "decision") == 0) {
            decision(&formation);
        }
    } while (strcmp("exit", cde) != 0);
}
