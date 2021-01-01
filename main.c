#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "wav.h"
#include "image.h"

#define MIN -32768
#define MAX  32767

void wav2bmp(const char *wav_name, const char *bmp_name);
void bmp2wav(const char *wav_name, const char *bmp_name);
void printHeader(WavHeader* header);

int main(int argc, char const *argv[])
{
    if(argc != 4){
        printf("Erreur : Entrer 3 arguments : action fichier_entree fichier_sortie\n");
        return 1;
    }
    #define action argv[1]
    #define f_entree argv[2]
    #define f_sortie argv[3]

    if(strcmp("wav2bmp", action) == 0){
        wav2bmp(f_entree, f_sortie);
    }else if(strcmp("bmp2wav", action) == 0){
        bmp2wav(f_entree, f_sortie);
    }else{
        puts("\naction non reconnue\n");
        puts("Merci d'entrer wav2bmp ou bmp2wav comme 1er argument.\n");
        return 2;
    }
    return 0;
}

WavHeader generateHeader(uint32_t datachunk_size){
    WavHeader header;

    strcpy(header.chunk_id    , "RIFF"  );
    strcpy(header.format      , "WAVE"  );
    strcpy(header.fmtchunk_id , "fmt"   );
    header.fmtchunk_id[3] = 32;
    strcpy(header.datachunk_id, "data"  );

    header.fmtchunk_size   = 16          ;
    header.audio_format    = 1           ;
    header.num_channels    = 1           ;
    header.sample_rate     = 44100       ;
    header.byte_rate       = 88200       ;
    header.block_align     = 2           ;
    header.bps             = 16          ;

    header.datachunk_size  = datachunk_size; // to set
    header.chunk_size      = datachunk_size + 36 ; // datachunk_size + 36

    return header;
}

void wav2bmp(const char *wav_name, const char *bmp_name){
    int16_t *samples = NULL;
    WavHeader *header = NULL;

    wavread((char*)wav_name, &header, &samples);

    printf("No. of channels: %d\n",     header->num_channels);
    printf("Sample rate:     %d\n",     header->sample_rate);
    printf("Bit rate:        %dkbps\n", header->byte_rate*8 / 1000);
    printf("Bits per sample: %d\n\n",     header->bps);
    printf("Datachunck size: %d\n\n",     header->datachunk_size);

    printHeader(header);

    //Division en 3 canaux possible : division par 3
    int theorie = header->datachunk_size / 3;
    //On cherche la puissance de deux dont le carré fait a peu près cette valeur
    int next = sqrt(theorie);
    printf("theorie = %d pixels = squared %dpx\n",theorie, next);
    const int nb_sample = next*next*3;
    printf("nb samples theorie = %d\n",nb_sample);


    Image* img = alloueImage(next,next);
    int cpt = 0;

    for(int y= next-1; y>0; y--){
        for(int x=0; x < next; x++){
            const unsigned char r =  round((double)(((samples[cpt+2] - (double)MIN) / ((double)MAX-(double)MIN)) * 255.0));
            const unsigned char g =  round((double)(((samples[cpt+1] - (double)MIN) / ((double)MAX-(double)MIN)) * 255.0));
            const unsigned char b =  round((double)(((samples[cpt+0] - (double)MIN) / ((double)MAX-(double)MIN)) * 255.0));

            img->rouge[y][x] = r;
            img->vert[y][x]  = g;
            img->bleu[y][x]  = b;

            cpt++;
        }
    }

    sauveImage(img, (char*)bmp_name);

    free(header);
    free(samples);

    libereImage(&img);
}


void bmp2wav(const char *bmp_name, const char *wav_name ){
    WavHeader *header = (WavHeader*) malloc(sizeof(WavHeader));
    int16_t *samples = NULL;

    Image* img = chargeImage((char*)bmp_name);

    printf("largeur image = %d\n",img->largeur);
    uint32_t sample_count= (uint32_t) img->largeur * (uint32_t)img->largeur * (uint32_t)3;
    printf("nb sample = %d\n",sample_count);

    *header = generateHeader(sample_count);
    samples = (int16_t*)malloc(sample_count);

    printHeader(header);

    int cpt=0;
    for(int y= img->hauteur-1; y>0; y--){
        for(int x=0; x < img->largeur; x++){
            samples[cpt+0] = (img->bleu[y][x]/255.0) * ((double)MAX-(double)MIN) + (double)MIN;
            samples[cpt+1] = (img->vert[y][x]/255.0) * ((double)MAX-(double)MIN) + (double)MIN;
            samples[cpt+2] = (img->rouge[y][x]/255.0) * ((double)MAX-(double)MIN) + (double)MIN;
            cpt++;
            
        }
    }

    // Modify the header values & samples before writing the new file
    wavwrite((char*)wav_name, header, sizeof(header), samples);
    printf("Sortie wave '%s' enregistree\n",wav_name);
    free(samples);
    free(header);

    libereImage(&img);
}

void printHeader(WavHeader* header){
    printf("\n\n\n Header infos : \n");
    printf("char     chunk_id[4]     = %s\n", header->chunk_id      ); 
    printf("char     format[4]       = %s\n", header->format        );
    printf("char     fmtchunk_id[4]  = %s\n", header->fmtchunk_id   );
    printf("uint32_t fmtchunk_size   = %d\n", header->fmtchunk_size );       
    printf("uint16_t audio_format    = %d\n", header->audio_format  );     
    printf("uint16_t num_channels    = %d\n", header->num_channels  );     
    printf("uint32_t sample_rate     = %d\n", header->sample_rate   );   
    printf("uint32_t byte_rate       = %d\n", header->byte_rate     );
    printf("uint16_t block_align     = %d\n", header->block_align   );   
    printf("uint16_t bps             = %d\n", header->bps           );
    printf("char     datachunk_id[4] = %s\n", header->datachunk_id  );        
    printf("uint32_t datachunk_size  = %d\n", header->datachunk_size);   
    printf("uint32_t chunk_size      = %d\n", header->chunk_size);   
    printf("\n ----------------------------- \n");
}