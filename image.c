#include <stdlib.h>
#include <stdio.h>
#include "lib/BmpLib.h"
#include "image.h"

Image* alloueImage(int largeur, int hauteur) {
    if (largeur < 1 || hauteur < 1) {
        return NULL;
    }

    Image* img = malloc(sizeof(Image));
    img->hauteur = hauteur;
    img->largeur = largeur;

    //Initialisation des lignes
    img->bleu  = (short int**)malloc(sizeof(short int*) * hauteur);
    img->rouge = (short int**)malloc(sizeof(short int*) * hauteur);
    img->vert  = (short int**)malloc(sizeof(short int*) * hauteur);

    //Initialisation des colonnes (pour chaque ligne)
    for (int i = 0; i < hauteur; i++) {
        img->bleu[i]  = (short int*)malloc(sizeof(short int) * largeur);
        img->rouge[i] = (short int*)malloc(sizeof(short int) * largeur);
        img->vert[i]  = (short int*)malloc(sizeof(short int) * largeur);
    }

    return img;
}

void libereImage(Image** ptrImage) {
    for(int y=0; y<(*ptrImage)->hauteur; y++){
        free((*ptrImage)->bleu[y]);
        free((*ptrImage)->rouge[y]);
        free((*ptrImage)->vert[y]);
    }
    free((*ptrImage)->bleu);
    free((*ptrImage)->rouge);
    free((*ptrImage)->vert);

    free(*ptrImage);
    *ptrImage = NULL;
}

Image* chargeImage(char* nom) {
    DonneesImageRGB* rgb = lisBMPRGB(nom);
    if (rgb == NULL) return NULL;
    
    Image* img = alloueImage(rgb->largeurImage, rgb->hauteurImage);
    int x = 0, y = 0; int cpt = 0;
    for (int i = 0; i < img->hauteur * img->largeur * 3; i+=3) {
        x = cpt % img->largeur;
        if (x == 0 && i > 0) y++;
         img->bleu[y][x]  = (short int)rgb->donneesRGB[i];
         img->vert[y][x]  = (short int)rgb->donneesRGB[i+1];
         img->rouge[y][x] = (short int)rgb->donneesRGB[i+2];
         
         cpt++;
    }
   
    libereDonneesImageRGB(&rgb);
    return img;
}

void sauveImage(Image* img, char* sortie) {
    DonneesImageRGB* out = malloc(sizeof(DonneesImageRGB));
    out->largeurImage = img->largeur;
    out->hauteurImage = img->hauteur;

    out->donneesRGB = (unsigned char*)malloc(sizeof(unsigned char) * img->largeur * img->hauteur * 3);
    int x = 0, y = 0, cpt = 0;
    for (int i = 0; i < img->hauteur * img->largeur *3; i+=3) {
        x = cpt % img->largeur;
        out->donneesRGB[i] = (unsigned char)img->bleu[y][x];
        out->donneesRGB[i+1] = (unsigned char)img->vert[y][x];
        out->donneesRGB[i+2] = (unsigned char)img->rouge[y][x];
        if (x == 0 && i > 0) y++;
        cpt++;
    }

    ecrisBMPRGB_Dans(out, sortie);
    free(out->donneesRGB);
    free(out);
    printf("Image '%s' enregistree\n", sortie);
}

/* Duplication d'une structure image et de son contenu */
Image* dupliqueImage(Image* img){
    Image* duplicata = alloueImage(img->largeur, img->hauteur);
    for (int y = 0; y < duplicata->hauteur; ++y) {
        for (int x = 0; x < duplicata->largeur; ++x) {
            duplicata->bleu[y][x] = img->bleu[y][x];
            duplicata->vert[y][x] = img->vert[y][x];
            duplicata->rouge[y][x] = img->rouge[y][x];
        }
    }
    return duplicata;
}

/* Retourne le maximum entre deux entiers */
int maxi(int a, int b) {
    return (a > b) ? a : b;
}

/* Retourne le minmum entre deux entiers */
int mini(int a, int b) {
    return (a < b) ? a : b;
}

/* Difference entre le contenu de deux structures images */
/* Renvoie la difference b - a */
Image* differenceImage(Image* a, Image *b) {
    int largeur = mini(a->largeur, b->largeur);
    int hauteur = mini(a->hauteur, b->hauteur);
    Image* resultat = alloueImage(largeur, hauteur);

    for (int y = 0; y < resultat->hauteur; ++y) {
        for (int x = 0; x < resultat->largeur; ++x) {
            resultat->bleu[y][x] = (short int)abs(b->bleu[y][x] - a->bleu[y][x]);
            resultat->vert[y][x] = (short int)abs(b->vert[y][x] - a->vert[y][x]);
            resultat->rouge[y][x] = (short int)abs(b->rouge[y][x]- a->rouge[y][x]);
       }
    }

    return resultat;
}