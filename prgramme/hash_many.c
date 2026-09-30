// hash_many : hache une liste de mots de passe (pour tester).
// Usage : ./hash_many fichier_entree fichier_sortie
#include "common.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s fichier_entree fichier_sortie\n", argv[0]);
        return 1;
    }
    FILE *in  = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");
    if (!in || !out) { perror("open"); return 1; }

    char line[64];
    while (fgets(line, sizeof line, in)) {
        if (line[0] == '\n') { fprintf(out, "\n"); continue; }
        char buf[M];
        for (int k = 0; k < M; k++) buf[k] = line[k];
        pwhash h = target_hash_function(buf);
        fprintf(out, "%#018lX\n", (unsigned long)h);
    }
    fclose(in);
    fclose(out);
    return 0;
}
