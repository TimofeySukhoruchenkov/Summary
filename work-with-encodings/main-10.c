#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define Iriof 12
#define Irof 13
#define Irif 14
#define IrB 15


#define BIGGY 0xFEFF
#define LIL 0xFFFE

#define bigm 0
#define lilm 1

#define YES 1
#define NO 0


int main(int argc,  char *argv[]) {
    int m = lilm;
    unsigned short BOM = LIL;
    FILE *S1 = stdin, *S2 = stdout;
    unsigned short u16;
    unsigned char u8;
    unsigned char b1, b2, b3;
    int correct = NO;


    if(argc > 6){
        fprintf(stderr, "Incorrect representation of input and output files\n");
        return Iriof;
    }
    else if(argc >= 5){
        if(strcmp(argv[1], "-i") == 0){
            S1 = fopen(argv[2], "r");
            if(strcmp(argv[3], "-o") == 0){
                S2 = fopen(argv[4], "w");
            }
            else{
                fprintf(stderr, "Incorrect representation of output file\n");
                return Irof;
            }
        }
        else if(strcmp(argv[1], "-o") == 0){
            S2 = fopen(argv[2], "w");
            if(strcmp(argv[3], "-i") == 0){
                S1 = fopen(argv[4], "r");
            }
            else{
                fprintf(stderr, "Incorrect representation of input file\n");
                return Irif;
            }
        }
        else{
            fprintf(stderr, "Incorrect representation of input and output files\n");
            return Iriof;
        }
        if(argc == 6){
            if(strcmp(argv[5], "-le") == 0) m = lilm;
            else if(strcmp(argv[5], "-be") == 0) m = bigm;
            else{
                fprintf(stderr, "Incorrect representation of BOM\n");
                return IrB;
            }
        }
    }
    else if(argc >= 3){
        if(strcmp(argv[1], "-i") == 0){
            S1 = fopen(argv[2], "r");
        }
        else if(strcmp(argv[1], "-o") == 0){
            S2 = fopen(argv[2], "w");
        }
        else{
            fprintf(stderr, "Incorrect representation of input and output files\n");
            return Iriof;
        }
        if(argc == 4){
            if(strcmp(argv[3], "-le") == 0) m = lilm;
            else if(strcmp(argv[3], "-be") == 0) m = bigm;
            else{
                fprintf(stderr, "Incorrect representation of BOM\n");
                return IrB;
            }
        }
    }


    fread(&b1, 1, 1, S1);
    fread(&b2, 1, 1, S1);
    fread(&b3, 1, 1, S1);

    if(!(((int)b3 == 0xBF) && ((int)b2 == 0xBB) && ((int)b1 == 0xEF))){
        fseek(S1, 0, SEEK_SET);
    }

    if(m == bigm){
        BOM = BIGGY;
    }


    fwrite(&BOM, sizeof(BOM), 1, S2);

    fread(&u8, 1, 1, S1);

    while(!feof(S1)) {
       u16 = 0;
       correct = NO;


       if((u8 >> 7) == 0){
           u16 += u8;
           correct = YES;
       }
       else if((u8 >> 5) == 0x6){
           u16 += (u8 & 0x1F);
           u16 = u16 << 6;

           fread(&u8, 1, 1, S1);

           if((u8 >> 6) == 2){
               u16 += (u8 & 0x3F);
               correct = YES;
           }
           else{
               fprintf(stderr, "Incorrect second byte,\n position: %ld\n", ftell(S1));
           }
       }
       else if((u8 >> 4) == 0xE){
           u16 += (u8 & 0xF);
           u16 = u16 << 6;

           fread(&u8, 1, 1, S1);

          if((u8 >> 6) == 2){
              u16 += (u8 & 0x3F);
              u16 = u16 << 6;

              fread(&u8, 1, 1, S1);

              if((u8 >> 6) == 2){
                  u16 += (u8 & 0x3F);
                  correct = YES;
              }
              else{
                  fprintf(stderr, "Incorrect third byte,\n position: %ld\n", ftell(S1));
              }
          }
          else{
              fprintf(stderr, "Incorrect second byte,\n position: %ld\n", ftell(S1));
          }
       }
       else{
           fprintf(stderr, "Incorrect byte,\n position: %ld\n", ftell(S1));
       }
       if(correct == YES){
           if(m == lilm){
               u8 = (char)(u16 >> 8);
               u16 = u16 << 8;
               u16 += u8;
           }
           fwrite(&u16, sizeof(u16), 1, S2);
       }
        fread(&u8, 1, 1, S1);
    }


    fclose(S1);
    fclose(S2);

    return 0;
}
