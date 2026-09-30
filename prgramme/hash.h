// Ne modifiez pas ce fichier, sauf pour modifier les constantes M, L, N, et R : mettez votre code ailleurs !
// Seule autre exception: si c'est plus pratique pour la compilation, il est autorisé de séparer ce fichier en deux (hash.h et hash.c)

#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>

#define M 6 // taille mot de passe
#define L 1000 // longueur chaine
#define N 100000 // nb chaines par table
#define R 10 // nb tables

#define SEED 0x0af380103007be01

typedef uint64_t pwhash;

pwhash target_hash_function (const void *data) {
    uint8_t*p=(uint8_t*)data,*e=p+M;
    uint64_t r=0x14020a57acced8b7,x,h=SEED;
    while(p+8<=e)memcpy(&x,p,8),x*=r,p+=8,x=x<<31|x>>33,h=h*r^x,h=h<<31|h>>33;
    while(p<e)h=h*r^*(p++);
    return(h=h*r+M,h^=h>>31,h*=r,h^=h>>31,h*=r,h^=h>>31,h*=r,h);
}

#endif

/*
Quelques infos qui peuvent vous aider :
Les hashs dans les crackme sont représentés en hexadécimal, on obtient cette représentation avec un print en utilisant le specifier %#018lX
Et donc pour les lire depuis un fichier en tant qu'entiers, on peut utiliser fscanf avec le specifier %lX (ou regarder du coté de strtoul et al).
Pour vos lectures dans les fichiers, attention à la façon dont vous traiter le \n à la fin de chaque ligne : il ne fait pas partie des caractères du hash ou du mot de passe.
*/
