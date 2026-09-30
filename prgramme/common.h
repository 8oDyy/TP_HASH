// Fonctions communes aux trois programmes.
#ifndef COMMON_H
#define COMMON_H

#include "hash.h"

// ---------------------------------------------------------------------------
// Representation des mots de passe
// ---------------------------------------------------------------------------
// Un mot de passe (M lettres minuscules) est vu comme un nombre en base 26 :
// la lettre k vaut (lettre - 'a'). Un mot de passe est donc un entier entre 0
// et 26^M - 1. C'est pratique : le comparer ou le ranger revient a manipuler
// un simple entier.

// 26^M : nombre de mots de passe possibles
static inline uint64_t nb_mdp(void) {
    uint64_t p = 1;
    for (int i = 0; i < M; i++) p *= 26;
    return p;
}

// numero -> M lettres (out fait M cases, sans '\0')
static inline void decode(uint64_t idx, char *out) {
    for (int k = 0; k < M; k++) { out[k] = 'a' + idx % 26; idx /= 26; }
}

// M lettres -> numero
static inline uint64_t encode(const char *s) {
    uint64_t idx = 0, base = 1;
    for (int k = 0; k < M; k++) { idx += (uint64_t)(s[k] - 'a') * base; base *= 26; }
    return idx;
}

// hash d'un mot de passe donne par son numero
static inline pwhash hash_mdp(uint64_t idx) {
    char buf[M];
    decode(idx, buf);
    return target_hash_function(buf);
}

// fonction de reduction : transforme un hash en numero de mot de passe.
// On ajoute la colonne et le numero de table pour que la reduction soit
// differente a chaque colonne (rainbow table) et a chaque table.
static inline uint64_t reduce(pwhash h, int col, int table, uint64_t nb) {
    return (h + (uint64_t)col * 2654435761u + (uint64_t)table * 40503u) % nb;
}

// ---------------------------------------------------------------------------
// Table de hachage maison (gestion des collisions par chainage)
// ---------------------------------------------------------------------------
// Chaque "seau" est une liste chainee de maillons. On l'utilise :
//   - a la creation, pour verifier rapidement l'unicite des pass0 et des passL ;
//   - a l'attaque, pour retrouver rapidement un passL et le pass0 associe.

typedef struct maillon {
    uint64_t cle;          // le mot de passe (par ex. un passL)
    uint64_t val;          // une valeur associee (par ex. le pass0)
    struct maillon *suiv;  // maillon suivant dans le meme seau
} maillon;

typedef struct {
    maillon **seaux;       // tableau de listes chainees
    uint64_t taille;       // nombre de seaux
} table_hachage;

static inline table_hachage *th_creer(uint64_t taille) {
    table_hachage *t = malloc(sizeof *t);
    if (!t) { perror("malloc"); exit(1); }
    t->taille = taille;
    t->seaux = calloc(taille, sizeof(maillon *));
    if (!t->seaux) { perror("calloc"); exit(1); }
    return t;
}

// fonction de hachage : donne le numero du seau pour une cle
static inline uint64_t th_seau(const table_hachage *t, uint64_t cle) {
    return (cle * 2654435761u) % t->taille;
}

// cherche une cle ; si trouvee, met la valeur dans *val (si val != NULL)
// et renvoie 1, sinon renvoie 0
static inline int th_chercher(const table_hachage *t, uint64_t cle, uint64_t *val) {
    for (maillon *m = t->seaux[th_seau(t, cle)]; m != NULL; m = m->suiv)
        if (m->cle == cle) {
            if (val) *val = m->val;
            return 1;
        }
    return 0;
}

// insere (cle, val) en tete du seau, sans verifier les doublons
static inline void th_inserer(table_hachage *t, uint64_t cle, uint64_t val) {
    uint64_t i = th_seau(t, cle);
    maillon *m = malloc(sizeof *m);
    if (!m) { perror("malloc"); exit(1); }
    m->cle = cle;
    m->val = val;
    m->suiv = t->seaux[i];
    t->seaux[i] = m;
}

// insere seulement si la cle est absente ;
// renvoie 1 si insere, 0 si la cle etait deja presente
static inline int th_inserer_unique(table_hachage *t, uint64_t cle, uint64_t val) {
    if (th_chercher(t, cle, NULL)) return 0;
    th_inserer(t, cle, val);
    return 1;
}

static inline void th_liberer(table_hachage *t) {
    for (uint64_t i = 0; i < t->taille; i++) {
        maillon *m = t->seaux[i];
        while (m) { maillon *s = m->suiv; free(m); m = s; }
    }
    free(t->seaux);
    free(t);
}

#endif
