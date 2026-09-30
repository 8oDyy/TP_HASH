// rainbow_attack : attaque une liste de hashs avec les R tables.
// Usage : ./rainbow_attack table1 ... tableR fichier_hashs fichier_sortie
// Le fichier de sortie a autant de lignes que le fichier de hashs
// (mot de passe trouve, ou ligne vide sinon).
//
// Structure de donnees (Question 2) : chaque table est chargee dans une table
// de hachage (passL -> pass0). La recherche d'un passL se fait ainsi en temps
// constant en moyenne, au lieu de parcourir toute la table.
#include "common.h"

int main(int argc, char **argv) {
    if (argc != R + 3) {
        fprintf(stderr, "usage: %s table1 ... table%d fichier_hashs fichier_sortie\n", argv[0], R);
        return 1;
    }
    uint64_t nb = nb_mdp();

    // charger les R tables dans R tables de hachage : passL -> pass0
    table_hachage *tab[R];
    for (int t = 0; t < R; t++) {
        tab[t] = th_creer((uint64_t)2 * N);
        FILE *f = fopen(argv[t + 1], "r");
        if (!f) { perror("fichier table"); return 1; }
        char a[64], b[64];
        while (fscanf(f, "%63s %63s", a, b) == 2)
            th_inserer(tab[t], encode(b), encode(a));   // cle = passL, valeur = pass0
        fclose(f);
    }

    FILE *hf = fopen(argv[R + 1], "r");
    if (!hf) { perror("fichier hashs"); return 1; }
    FILE *of = fopen(argv[R + 2], "w");
    if (!of) { perror("fichier sortie"); return 1; }

    char line[64];
    long total = 0, trouve = 0;
    while (fgets(line, sizeof line, hf)) {
        if (line[0] == '\n') { fprintf(of, "\n"); continue; }
        total++;
        pwhash cible = (pwhash) strtoull(line, NULL, 16);

        int fini = 0;
        char res[M];

        for (int t = 0; t < R && !fini; t++) {
            // On suppose que le mot de passe est en colonne c, pour c de L-1 a 0.
            // Alors cible = h(pass_c), donc pass_{c+1} = r_c(cible), et on avance
            // jusqu'au bout pour obtenir le passL correspondant.
            for (int c = L - 1; c >= 0 && !fini; c--) {
                uint64_t pw = reduce(cible, c, t, nb);
                for (int j = c + 1; j < L; j++)
                    pw = reduce(hash_mdp(pw), j, t, nb);

                uint64_t start;
                if (th_chercher(tab[t], pw, &start)) {
                    // passL reconnu : on reconstruit le candidat (pass_c) depuis pass0
                    uint64_t cand = start;
                    for (int j = 0; j < c; j++)
                        cand = reduce(hash_mdp(cand), j, t, nb);
                    // verification : elimine les fausses alarmes
                    if (hash_mdp(cand) == cible) {
                        decode(cand, res);
                        fini = 1;
                    }
                }
            }
        }

        if (fini) { fprintf(of, "%.*s\n", M, res); trouve++; }
        else        fprintf(of, "\n");
    }

    fclose(hf);
    fclose(of);
    for (int t = 0; t < R; t++) th_liberer(tab[t]);
    fprintf(stderr, "trouve %ld / %ld\n", trouve, total);
    return 0;
}
