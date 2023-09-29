#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2
#define IN_STRING 4
#define IN_ID 8

#define MAX_ID_LEN 64
#define MAX_IDS 1024

#define TEST 0  // 1 - dla testowania,  0 - dla automatycznej oceny

int index_cmp(const void*, const void*);
int cmp(const void*, const void*);

char tab[MAX_IDS][MAX_ID_LEN];

char *keywords[] = {
        "auto", "break", "case", "char",
        "const", "continue", "default", "do",
        "double", "else", "enum", "extern",
        "float", "for", "goto", "if",
        "int", "long", "register", "return",
        "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while"
};

int find_idents(FILE *stream){
    char c;
    char before;
    char word[MAX_ID_LEN];
    char list_of_words[MAX_IDS][MAX_ID_LEN];

    int index = 0;
    int counter=0;

    int inside = 0;
    int in_block_com = 0;
    int in_line_com = 0;
    int in_string = 0;
    int in_char = 0;
    int start = 0;

    while ((c=getc(stream)) != EOF){
        int a = c;
        if (in_block_com == 0 && in_line_com == 0 && in_string == 0 && in_char == 0)
            inside = 0;
        else
            inside = 1;

        if (index == 0 && ((a >= 65 && a <= 90) || (a >= 97 && a <= 122) || (a == 95)) && (inside == 0)){
            word[index]=c;
            index++;
        }
        else if (index > 0 && ((a >= 65 && a <= 90) || (a >= 97 && a <= 122) || (a >= 48 && a <= 57) || (a == 95)) && inside == 0){
            word[index]=c;
            index++;
        }
        else if (inside == 0){
            word[index] = '\0';
            int flag = 0;
            for (int i=0; i<32; i++){
                if (strcmp(word, keywords[i])==0){
                    flag = 1;
                    break;
                }
            }
            for (int i=0; i<counter; i++){
                if (strcmp(word, list_of_words[i])==0){
                    flag = 1;
                    break;
                }
            }

            if (flag == 0 && word[0]!='\0'){
                strcpy(list_of_words[counter], word);
                counter++;
            }
            index = 0;
            word[0] = '\0';
        }

        if (c=='/' && before=='/' && in_line_com==0)
            in_line_com = 1;

        if (before=='/' && c=='*' && in_block_com==0)
            in_block_com = 1;
        else if (before=='*' && c=='/' && in_block_com==1)
            in_block_com = 0;

        if (c=='"' && in_string==0)
            in_string = 1;
        else if (c=='"' && in_string==1 && before != '\\')
            in_string = 0;

        if (c=='\'' && in_char==0)
            in_char = 1;
        else if (c=='\'' && in_char==1)
            in_char = 0;

        if (c=='\n' || c=='\r')
            in_line_com = 0;

        before = c;
    }

    return counter;
}

int cmp(const void* first_arg, const void* second_arg) {
    char *a = *(char**)first_arg;
    char *b = *(char**)second_arg;
    return strcmp(a, b);
}

int index_cmp(const void* first_arg, const void* second_arg) {
    int a = *(int*)first_arg;
    int b = *(int*)second_arg;
    return strcmp(tab[a], tab[b]);
}

int main(void) {
    char file_name[40];
    FILE *stream;

    if(TEST)  stream = stdin;
    else {
        scanf("%s",file_name);
        stream = fopen(file_name,"r");
        if(stream == NULL) {
            printf("fopen failed\n");
            return -1;
        }
    }
    printf("%d\n", find_idents(stream));
    return 0;
}

