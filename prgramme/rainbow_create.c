// rainbow_create : cree les R tables.
// Usage : ./rainbow_create table1 ... tableR [fichier_starts]
//   - ecrit R fichiers de N lignes "pass0 passL".
//   - si un (R+1)eme argument est donne, les pass0 sont lus dedans,
//     sinon ils sont tires au hasard.
// On impose : les passL uniques dans chaque table, les pass0 uniques partout.
// L'unicite est verifiee grace a une table de hachage (cf. common.h).
#include "common.h"
#include <time.h>

int main(int argc, char **argv) {
    if (argc != R + 1 && argc != R + 2) {
        fprintf(stderr, "usage: %s table1 ... table%d [fichier_starts]\n", argv[0], R);
        return 1;
    }
    uint64_t nb = nb_mdp();
    srand((unsigned)time(NULL));

    FILE *sf = NULL;
    if (argc == R + 2) {
        sf = fopen(argv[R + 1], "r");
        if (!sf) { perror("fichier starts"); return 1; }
    }

    // table de hachage des pass0 deja utilises (toutes tables confondues)
    table_hachage *starts = th_creer((uint64_t)2 * N * R);

    for (int t = 0; t < R; t++) {
        FILE *out = fopen(argv[t + 1], "w");
        if (!out) { perror("fichier table"); return 1; }

        // table de hachage des passL de CETTE table (recreee a chaque table)
        table_hachage *ends = th_creer((uint64_t)2 * N);

        int count = 0;
        while (count < N) {
            // 1) trouver un pass0 pas encore utilise
            uint64_t s;
            for (;;) {
                if (sf) {
                    char line[64];
                    if (!fgets(line, sizeof line, sf)) {
                        fprintf(stderr, "pas assez de mots de passe initiaux\n");
                        return 1;
                    }
                    if (line[0] == '\n') continue;
                    s = encode(line);
                } else {
                    s = 0; uint64_t base = 1;
                    for (int k = 0; k < M; k++) { s += (uint64_t)(rand() % 26) * base; base *= 26; }
                }
                if (th_inserer_unique(starts, s, 0)) break;  // accepte si nouveau
            }

            // 2) derouler la chaine du debut a la fin
            uint64_t e = s;
            for (int c = 0; c < L; c++)
                e = reduce(hash_mdp(e), c, t, nb);

            // 3) si ce passL est deja dans la table, on recommence
            if (!th_inserer_unique(ends, e, s)) continue;

            // 4) ecrire "pass0 passL"
            char a[M], b[M];
            decode(s, a);
            decode(e, b);
            fprintf(out, "%.*s %.*s\n", M, a, M, b);
            count++;
        }

        fclose(out);
        th_liberer(ends);
        fprintf(stderr, "table %d/%d ecrite\n", t + 1, R);
    }

    th_liberer(starts);
    if (sf) fclose(sf);
    return 0;
}
