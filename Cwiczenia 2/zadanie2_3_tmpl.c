#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

int rand_from_interval(int a, int b) {
    if (a>b)
        return INT_MIN;

    if (b-a > RAND_MAX)
        return INT_MAX;

    if (a==b)
        return a;

    return a+rand()%(b-a+1);
}

void rand_permutation(int n, int tab[]){
    if (n <= 0 || n > 52)
        return;
    else{
        for (int i=0; i<n; i++)
            tab[i]=i;

        for (int i=0; i<n-1; i++){
            int k = rand_from_interval(i,n-1);
            int temp = tab[i];
            tab[i]=tab[k];
            tab[k]=temp;
        }
    }
}

#define CBUFF_SIZE  52

//gracz1 to gracz A, analogicznie gracz2 to gracz B

int gracz1[CBUFF_SIZE], gracz2[CBUFF_SIZE];
int out1=0;
int len1=0;
int out2=0;
int len2=0;

double cbuff_push(int cbuff[], int cli_nr, int len, int out) {
    /*if (len < CBUFF_SIZE){
        cbuff[len+out] = cli_nr;
        return 0.;
    }else{
        return INFINITY;
    }*/
    if (len >= CBUFF_SIZE){
        return INFINITY;
    }
    else{
        cbuff[(out+len)%CBUFF_SIZE]=cli_nr;
        len+=1;
    }
}

int cbuff_pop(int cbuff[], int len, int out) {
    /*if (len > 0){
        int nr = cbuff[out];
        for (int i=0; i<len-1; i++)
            cbuff[i]=cbuff[i+1];
        len-=1;
        cbuff[len]=0;
        return nr;
    }
    else
        return -1;*/
    if (len > 0){
        int temp = cbuff[out];
        if (out+1<CBUFF_SIZE)
            out++;
        else
            out = 0;
        len--;
        return out;
    }
    else{
        return -1;
    }
}

void cbuff_print(int cbuff[], int len, int out) {
    /*for (int i=0; i<len; i++)
        printf("%d ", cbuff[i]);*/
    for (int i=0; i<len; i++)
        printf("%d ", cbuff[(out+i)%CBUFF_SIZE]);
}

void give_cards(int zbior[], int gracz[], int size, int tryb, int out){
    if (tryb == 0) {
        for (int i=0; i < (size/2); i++) {
            cbuff_push(gracz, zbior[i], len1, out);
            len1+=1;
        }
        return;
    }

    if (tryb == 1) {
        for (int i = (size / 2); i < size; i++) {
            cbuff_push(gracz, zbior[i], len2, out);
            len2+=1;
        }
        return;
    }
}

int main(void) {
    int seed, kod, max_conflicts;
    scanf("%d %d %d", &seed, &kod, &max_conflicts);

    int conflicts = 0;

    int size = 52;
    int talia[size];

    int stol[size];

    srand(seed);
    rand_permutation(size, talia);

    give_cards(talia, gracz1, size, 0, out1);
    give_cards(talia, gracz2, size, 1, out2);

    for (int i=0; i<size; i++){
        stol[i]=0;
    }

    int game = 1;
    while(conflicts < max_conflicts && game == 1){

        int conflict = 1;
        int depth = 0;
        int stol_len = 0;

        while (conflict==1) {

            if (len1>0 && len2>0) {
                stol[stol_len]=gracz1[out1];
                stol_len+=1;
                out1=cbuff_pop(gracz1, len1, out1);
                len1 -= 1;

                stol[stol_len]=gracz2[out2];
                stol_len+=1;
                out2 = cbuff_pop(gracz2, len2, out2);
                len2 -= 1;
            }else if((len1<=0 || len2<=0) && depth > 0){
                printf("1 %d %d ", len1+((stol_len)/2), len2+((stol_len)/2));
                game=0;
                break;
            }
            else if (len1<=0 && len2>=0){
                printf("3 \n");
                cbuff_print(gracz2,len2,out2);
                game = 0;
                break;
            }
            else if (len1>=0 && len2<=0){
                printf("2 %d", conflicts);
                game = 0;
                break;
            }

            if (depth%2 == 0){
                conflicts += 1;
                if (conflicts > max_conflicts){
                    printf("\n %d \n %d \n %d \n", len1, len2, stol_len);
                    len1 += stol_len/2;
                    len2 += stol_len/2;
                    printf("0 \n %d \n %d ", len1, len2);
                    game = 0;
                    break;
                }

                if ((stol[stol_len-2] / 4 > stol[stol_len-1] / 4)) {
                    conflict = 0;
                    for (int i = 0; i < stol_len; i+=2) {
                        cbuff_push(gracz1, stol[i], len1, out1);
                        len1 += 1;
                        stol[i]=0;
                    }
                    for (int i = 1; i < stol_len; i+=2) {
                        cbuff_push(gracz1, stol[i], len1, out1);
                        len1 += 1;
                        stol[i]=0;
                    }
                    stol_len = 0;

                }
                else if((stol[stol_len-2] / 4) < (stol[stol_len-1] / 4)){
                    conflict = 0;
                    for (int i = 1; i < stol_len; i+=2) {
                        cbuff_push(gracz2, stol[i], len2, out2);
                        len2 += 1;
                        stol[i]=0;
                    }

                    for (int i = 0; i < stol_len; i+=2) {
                        cbuff_push(gracz2, stol[i], len2, out2);
                        len2 += 1;
                        stol[i]=0;
                    }
                    stol_len = 0;
                }
                else if (kod == 1){
                    conflict = 0;
                    cbuff_push(gracz1, stol[0], len1, out1);
                    len1+=1;
                    cbuff_push(gracz2, stol[1], len2, out2);
                    len2+=1;
                    stol[0]=0;
                    stol[1]=0;
                }
            }
            depth+=1;
        }
    }
    return 0;
}