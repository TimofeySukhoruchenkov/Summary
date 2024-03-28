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


int main(int argc,  char *argv[]) {
    int m = lilm;
    unsigned long ct;
    FILE *S1 = stdin, *S2 = stdout;
    unsigned short u16;
    char u81, u82, u83;


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

    ct = fread(&u16, sizeof(char), 2, S1);
    if(ct == 1){
        fprintf(stderr, "Wrong number of bytes\n");
        return 16;
    }
    if(u16 == BIGGY){
        m = bigm;
    }
    else if(u16 == LIL){
        m = lilm;
    }
    else{
        fseek(S1, 0, SEEK_SET);
        printf("No BOM in the file\n");
    }

    ct = fread(&u16, sizeof(char), 2, S1);

    while(ct == 2){
        if(m == lilm){
            u81 = (char)(u16 >> 8);
            u16 = u16 << 8;
            u16 += u81;
        }

        if(u16 < 128){

            u81 = (char)u16;

            fwrite(&u81, 1, 1,  S2);

        }
        else if(u16 < 2048){

            u81 = (char)(u16 & 0x3F);
            u81 += 128;
            u16 = u16 >> 6;
            u82 = (char)u16;
            u82 += 192;


            fwrite(&u82, 1, 1,  S2);
            fwrite(&u81, 1, 1,  S2);

        }
        else{

            u81 = (char)(u16 & 0x3F);
            u81 += 128;
            u16 = u16 >> 6;
            u82 = (char)(u16 & 0x3F);
            u82 += 128;
            u83 = (char)(u16 >> 6);
            u83 += 224;

            fwrite(&u83, 1, 1,  S2);
            fwrite(&u82, 1, 1,  S2);
            fwrite(&u81, 1, 1,  S2);

        }

        ct = fread(&u16, sizeof(char), 2, S1);
    }

    if (ct == 1){
       fprintf(stderr, "Wrong number of bytes\n");
    }

    fclose(S1);
    fclose(S2);

    return 0;
}
