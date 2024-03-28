#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>


struct lan {
    char **mu;
    char *sin;
    char *sout;
    char *sadd;
    int bgflag;
    struct lan *bnext;
    struct lan *next;
    int ptflag;
    int pflag;
    int andflag;
    int orflag;
    int count;
};

void intlan(struct lan *sh) {
    sh->mu = NULL;
    sh->sin = NULL;
    sh->sout = NULL;
    sh->sadd = NULL;
    sh->bnext = NULL;
    sh->next = NULL;
    sh->ptflag = 0;
    sh->bgflag = 0;
    sh->pflag = 0;
    sh->andflag = 0;
    sh->orflag = 0;
    sh->count = 0;
}

struct lan *rwords(char ***mu, int i) {
    struct lan *sh = calloc(1, sizeof(struct lan));
    intlan(sh);
    struct lan *start = sh;
    while (**mu != NULL) {

        if (strcmp(**mu, "<") == 0) {
            (*mu)++;
            sh->sin = **mu;
            (*mu)++;
        } else if (strcmp(**mu, ">") == 0) {
            (*mu)++;
            sh->sout = **mu;
            (*mu)++;
        } else if (strcmp(**mu, ">>") == 0) {
            (*mu)++;
            sh->sadd = **mu;
            (*mu)++;
        } else if (strcmp(**mu, "|") == 0) {
            sh->next = calloc(1, sizeof(struct lan));
            sh->pflag = 1;
            sh = sh->next;
            intlan(sh);
            (*mu)++;
        } else if (strcmp(**mu, "&") == 0) {
            (*mu)++;
            sh->bgflag = 1;
            if (**mu == NULL) {
                printf("There are no after\n");
                return start;
            }
        } else if (strcmp(**mu, "&&") == 0) {
            sh->next = calloc(1, sizeof(struct lan));
            sh = sh->next;
            intlan(sh);
            sh->andflag = 1;
            (*mu)++;
        } else if (strcmp(**mu, "||") == 0) {
            sh->next = calloc(1, sizeof(struct lan));
            sh = sh->next;
            intlan(sh);
            sh->orflag = 1;
            (*mu)++;
        } else if (strcmp(**mu, ";") == 0) {
            sh->next = calloc(1, sizeof(struct lan));
            sh = sh->next;
            intlan(sh);
            sh->ptflag = 1;
            (*mu)++;
        } else if (strcmp(**mu, "(") == 0){
            (*mu)++;
            sh->bnext = rwords(mu, i);
        } else if (strcmp(**mu, ")") == 0){
            (*mu)++;
            return start;
        }
        else {
            sh->count++;
            sh->mu = realloc(sh->mu, sizeof(char *) * (sh->count + 1));
            sh->mu[sh->count] = NULL;
            sh->mu[sh->count - 1] = **mu;
            (*mu)++;
        }
    }
    return start;
}

int wwt(struct lan *sh, int br) {
    int status = 0;
    int file;
    int cflag = 1;
    pid_t pid;
    int i = 0;
    int fd[2];
    while (sh != NULL) {
        int s0 = dup(0);
        int s1 = dup(1);
        if(sh->andflag == 1){
            i = 0;
            cflag = !status;
        }
        if(sh->orflag == 1){
            i = 0;
            cflag = status;
        }
        if(sh->ptflag == 1){
            i = 0;
        }
        if(cflag == 0){
            break;
        }
        if (sh->mu && (strcmp(sh->mu[0], "cd") == 0)) {
            if (sh->mu[1] == NULL) {
                chdir(getenv("HOME"));
            } else if (sh->mu[2] != NULL) {
                fprintf(stderr, "Error: too many arguments\n");
                return !WIFEXITED(status);
            } else {
                if (chdir(sh->mu[1]) == -1) {
                    fprintf(stderr, "Error: no such directory\n");
                }

            }
        } else if (sh->mu && (strcmp(sh->mu[0], "exit") == 0)) {
            return !WIFEXITED(status);
        } else {
            if (i != 0) {
                dup2(fd[0], 0);
                close(fd[0]);
            }
            if (sh->pflag == 1) {
                pipe(fd);
                dup2(fd[1], 1);
                close(fd[1]);
            }

            if ((pid = fork()) == 0) {

                //printf("111 %s\n", sh->sout);
                if (sh->bgflag == 1) {
                    printf("bg chpid: [%d]\n", getpid());
                    int filen = open("/dev/null", O_RDONLY);
                    if (filen == -1) {
                        printf("No /dev/bull\n");
                    }
                    dup2(filen, 0);
                    close(filen);
                    signal(SIGINT, SIG_IGN);
                }
                //printf("222 %s\n", sh->sout);
                if (sh->sin != NULL) {
                    if ((file = open(sh->sin, O_RDONLY, 0666)) == -1) {
                        perror(sh->sin);
                        fprintf(stderr, "Wrong file");
                    }
                    dup2(file, 0);
                    close(file);
                }
                //printf("333 %s\n", sh->sout);
                if (sh->sout != NULL) {
                    //printf("444 %s\n", sh->sout);
                    if ((file = open(sh->sout, O_CREAT | O_WRONLY | O_TRUNC, 0666)) == -1) {
                        perror(sh->sout);
                        fprintf(stderr, "Wrong file");
                    }
                    dup2(file, 1);
                    close(file);
                } else if (sh->sadd != NULL) {
                    if ((file = open(sh->sadd, O_APPEND | O_WRONLY, 0666)) == -1) {
                        perror(sh->sadd);
                        fprintf(stderr, "Wrong file");
                    }

                    dup2(file, 1);
                    close(file);
                }
                if (sh->bgflag == 0) {
                    signal(SIGCHLD, SIG_DFL);
                }
                if(sh->bnext){
                    status = wwt(sh->bnext, 1);
                    return !WIFEXITED(status);
                }
                else {
                    execvp(sh->mu[0], sh->mu);
                    perror(sh->mu[0]);
                    return 1;
                }
            } else {
                if (sh->bgflag == 0) {
                    wait(&status);
                }
            }
        }
        sh = sh->next;
        i++;
        cflag = 1;
        dup2(s0, 0);
        dup2(s1, 1);
        close(s0);
        close(s1);
    }
    if(br){
        return 1;
    }
    return 0;
}

void zomkill() {
    int status;
    pid_t pid;
    pid = waitpid(-1, &status, WNOHANG);
    while (pid > 0) {
        if (WIFEXITED(status)) {
            printf("[%d] end, status: %d\n", pid, status);
        } else {
            printf("[%d] end, status: error\n", pid);
        }
        pid = waitpid(-1, &status, WNOHANG);
    }
}

int alp(char c) {
    return (c != EOF) && (c != '\n') &&
           (c != '|') && (c != '&') &&
           (c != '>') && (c != '<') &&
           (c != '(') && (c != ')') &&
           (c != ';') && (c != ' ') &&
           (c != '"');
}


char *readspecsym(char c, FILE *S) {
    char *str;
    str = NULL;
    if (c == '\n') {
        str = "\n";
    } else if (c == '&') {
        c = getc(S);
        if (c == '&') {
            str = "&&";
        } else {
            ungetc(c, S);
            str = "&";
        }


    } else if (c == '|') {
        c = getc(S);
        if (c == '|') {
            str = "||";

        } else {
            ungetc(c, S);
            str = "|";
        }

    } else if (c == '>') {
        c = getc(S);
        if (c == '>') {
            str = ">>";
        } else {
            ungetc(c, S);
            str = ">";
        }

    } else if (c == '<') {
        str = "<";
    } else if (c == ';') {
        str = ";";
    } else if (c == '(') {
        str = "(";
    } else if (c == ')') {
        str = ")";
    }
    return str;
}

char *readwintoq(FILE *S) {
    int i = 0, l = 10;
    char c;
    char *str = calloc(l, sizeof(char));
    c = getc(S);
    int k = 1;
    while (((alp(c)) | k | c == '"') && (c != '\n')) {
        if (i == l) {
            l *= 2;
            str = realloc(str, l * sizeof(char));
        }
        if (c != '"') {
            str[i++] = c;
        } else {
            k = !k;
        }
        c = getc(S);
    }
    ungetc(c, S);
    str[i] = 0;
    if (k == 1) {
        free(str);
        printf("ERROR - not /2 num of \" \n");
        return "gg";
    } else return str;
}


char *readword(FILE *S) {
    char *str = NULL;
    int i = 0, l = 10;
    char c;

    c = getc(S);

    if (c == EOF) {
        return str;
    }


    while (c == ' ') {
        c = getc(S);
    }
    if (c == '"') {
        return readwintoq(S);
    } else {
        str = readspecsym(c, S);
        if (str != NULL) {
            return str;
        } else {
            str = calloc(l, sizeof(char));
            while (alp(c)) {
                if (i == l) {
                    l *= 2;
                    str = realloc(str, l * sizeof(char));
                }
                str[i++] = c;
                c = getc(S);
            }
            if (c != '"') {
                ungetc(c, S);
            } else {
                c = getc(S);
                int k = 1;
                while ((alp(c)) | k | c == '"' && (c != '\n')) {
                    if (i == l) {
                        l *= 2;
                        str = realloc(str, l * sizeof(char));
                    }
                    if (c != '"') {
                        str[i++] = c;
                    } else {
                        k = !k;
                    }
                    c = getc(S);
                }
                ungetc(c, S);
                if (k == 1) {
                    free(str);
                    printf("ERROR - not /2 num of \" \n");
                    return "gg";
                }
            }
            str[i] = 0;
            return str;
        }


    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, SIG_IGN);
    signal(SIGCHLD, zomkill);
    struct lan *list;
    int i, l = 10;
    FILE *S = stdin;
    int exst;
    char *str;
    char *strn = "\n";
    if (argc == 2) {
        S = fopen(argv[1], "r");
    }

    str = readword(S);
    while (str != NULL) {
        i = 0;
        char **mu = calloc(l, sizeof(char *));

        while ((str != NULL) && (strcmp(str, strn) != 0) && (strcmp(str, "gg") != 0)) {
            if (i == l) {
                l *= 2;
                mu = realloc(mu, l * sizeof(char *));
            }
            mu[i++] = str;
            str = readword(S);
        }
        mu[i] = NULL;
        if ((mu[0] != NULL) && (strcmp(str, "gg") != 0)) {
            char **mul = mu;
            list = rwords(&mul, i);
            exst = wwt(list, 0);
        }

        /*if (strcmp(str, "gg") != 0) {
            for (int j = 0; j < i; j++) {
                printf("%s\n", mu[j]);
            }
        }*/
        for (int j = 0; j < i; j++) {
            if ((strcmp(mu[j], "gg") != 0) &&
                (strcmp(mu[j], ")") != 0) &&
                (strcmp(mu[j], "(") != 0) &&
                (strcmp(mu[j], ">") != 0) &&
                (strcmp(mu[j], ">>") != 0) &&
                (strcmp(mu[j], "<") != 0) &&
                (strcmp(mu[j], ";") != 0) &&
                (strcmp(mu[j], "&") != 0) &&
                (strcmp(mu[j], "&&") != 0) &&
                (strcmp(mu[j], "|") != 0) &&
                (strcmp(mu[j], "||") != 0))
                free(mu[j]);
        }
        free(mu);
        if (exst == 1) {
            break;
        }
        str = readword(S);
    }
    fclose(S);
    return 0;
}
