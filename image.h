/*
* Image.h
*/
#ifndef IMAGE_H
#define IMAGE_H

typedef struct Image {
	int largeur, hauteur;
	short int** rouge, ** vert, ** bleu;
} Image;

Image* alloueImage(int largeur, int hauteur);
void libereImage(Image** ptrImage);
Image* chargeImage(char* nom);
void sauveImage(Image* monImage, char* nom);

#endif