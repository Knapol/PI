#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// consider chars from [FIRST_CHAR, LAST_CHAR)
#define FIRST_CHAR 33
#define LAST_CHAR 127
#define MAX_CHARS (LAST_CHAR - FIRST_CHAR)
#define MAX_DIGRAMS (LAST_CHAR - FIRST_CHAR) * (LAST_CHAR - FIRST_CHAR)

#define NEWLINE '\n'
#define IN_WORD 1

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2

#define TEST 0   // 1 dla testowania, 0 dla automatycznej oceny

int count[MAX_DIGRAMS] = { 0 };

// function to be used as comparator to qsort (compares counters and thes sorts
// alphabetically when equal)
int cmp (const void *a, const void *b) {
    int va = *(int*)a;
    int vb = *(int*)b;
    if (count[va] == count[vb]) return va - vb; // sort alphabetically if counts equal
    return count[vb] - count[va];
}

// function to be used as comparator to qsort for digrams (compares counters and
// thes sorts alphabetically when equal)
int cmp_di (const void *a, const void *b) {
    int va = *(int*)a;
    int vb = *(int*)b;
    // sort according to second char if counts and the first char equal
    if (count[va] == count[vb] && va / MAX_CHARS == vb / MAX_CHARS) return va % MAX_CHARS - vb % MAX_CHARS;
    // sort according to first char if counts equal
    if (count[va] == count[vb]) return va / MAX_CHARS - vb / MAX_CHARS;
    return count[vb] - count[va];
}

// count number of lines (nl), number of words (nw) and number of characters
// (nc) in the text read from stream
void wc(int *nl, int *nw, int *nc, FILE *stream){
    char c;
    int lines = 0;
    int words = 0;
    int characters = 0;
    int in_word=0;
    while ((c = getc(stream)) != EOF){
        characters++;

        if (c == ' ' || c == '\n' || c == '\0'){
            if (in_word){
                in_word = 0;
                words++;
            }

            if (c == '\n' || c == '\0')
                lines++;
        }
        else
            in_word = 1;
    }
    *nl = lines;
    *nw = words;
    *nc = characters;
}

// count how many times each character from [FIRST_CHAR, LAST_CHAR) occurs
// in the text read from stream. Sort chars according to their respective
// cardinalities (decreasing order). Set n_char and cnt to the char_no - th char
// in the sorted list and its cardinality respectively
void char_count(int char_no, int *n_char, int *cnt, FILE *stream){
    int tab[MAX_CHARS];
    int tab_index[MAX_CHARS];
    char c;
    int testowa[MAX_CHARS];

    for (int i=0; i<MAX_CHARS; i++){
        tab[i]=0;
        tab_index[i]=FIRST_CHAR+i;
        testowa[i]=tab_index[i];
    }

    while ((c = getc(stream)) != EOF){
        int a = c;
        if (a >= FIRST_CHAR && a < LAST_CHAR)
            tab[a-FIRST_CHAR]++;
    }

    for (int i=0; i<MAX_CHARS-1; i++){
        for (int j=0; j<MAX_CHARS-i-1; j++){
            if (tab[tab_index[j]-FIRST_CHAR]<tab[tab_index[j+1]-FIRST_CHAR]){
                int tmp = tab_index[j];
                tab_index[j]=tab_index[j+1];
                tab_index[j+1]=tmp;
            }
        }
    }

    *n_char = tab_index[char_no-1];
    *cnt = tab[tab_index[char_no-1]-FIRST_CHAR];
}

// count how many times each digram (a pair of characters, from [FIRST_CHAR,
// LAST_CHAR) each) occurs in the text read from stream. Sort digrams according
// to their respective cardinalities (decreasing order). Set digram[0] and
// digram[1] to the first and the second char in the digram_no - th digram_char
// in the sorted list. Set digram[2] to its cardinality.
void digram_count(int digram_no, int digram[], FILE *stream){
    char c;
    char before;
    int start = 0;
    int digrams[MAX_CHARS*MAX_CHARS];

    for (int i=0; i<MAX_CHARS*MAX_CHARS; i++)
        digrams[i]=i;

    while ((c = getc(stream)) != EOF){
        if (start == 0){
            int a = c;
            if (a >= FIRST_CHAR && a < LAST_CHAR){
                before = c;
                start = 1;
            }
        }
        else{
            int a = c;
            int b = before;
            if (a >= FIRST_CHAR && a < LAST_CHAR){
                count[(b-FIRST_CHAR)*MAX_CHARS+a-FIRST_CHAR]++;
                before = c;
            }
            else
                start = 0;
        }
    }

    qsort(digrams, MAX_CHARS*MAX_CHARS, sizeof(int), cmp_di);

    digram[0]=digrams[digram_no-1]/MAX_CHARS+FIRST_CHAR;
    digram[1]=digrams[digram_no-1]%MAX_CHARS+FIRST_CHAR;
    digram[2]=count[digrams[digram_no-1]];
}

// Count block and line comments in the text read from stream. Set
// line_comment_counter and block_comment_counter accordingly
void find_comments(int *line_comment_counter, int *block_comment_counter, FILE *stream){
    char c;
    char before;
    int start = 0;
    int in_block = 0;
    int in_comment = 0;

    *line_comment_counter = 0;
    *block_comment_counter = 0;

    while ((c = getc(stream)) != EOF){
        if (c=='\n')
            in_comment = 0;

        if ((start == 0) && (c == '/' || c == '*')){
            if (c=='/' && in_block == 1)
                continue;
            if (c=='*' && in_block == 0)
                continue;
            before = c;
            start = 1;
        }
        else{
            if (before == '/' && c == '/' && in_comment==0){
                start=0;
                in_comment = 1;
                (*line_comment_counter)++;
            }
            else if (before == '/' && c == '*' && in_comment==0){
                start = 0;
                in_block = 1;
            }
            else if (before == '*' && c == '/' && in_block == 1){
                start = 0;
                in_block = 0;
                (*block_comment_counter)++;
            }
            else
                start = 0;
        }
    }
}

#define MAX_LINE 128

int read_line() {
    char line[MAX_LINE];
    int n;

    fgets (line, MAX_LINE, stdin); // to get the whole line
    sscanf (line, "%d", &n);
    return n;
}

int main(void) {
    int to_do;
    int nl, nw, nc, char_no, n_char, cnt;
    int line_comment_counter, block_comment_counter;
    int digram[3];

    char file_name[40];
    FILE *stream;

    if(TEST) printf("Wpisz nr zadania ");
    to_do = read_line();      //    scanf ("%d", &to_do);
    if(TEST)  stream = stdin;
    else {
        scanf("%s",file_name);  read_line();
        stream = fopen(file_name,"r");
        if(stream == NULL) {
            printf("fopen failed\n");
            return -1;
        }
    }

//	to_do = read_line();
    switch (to_do) {
        case 1: // wc()
            wc (&nl, &nw, &nc, stream);
            printf("%d %d %d\n", nl, nw, nc);
            break;
        case 2: // char_count()
            if(TEST) printf("Wpisz numer znaku ");
            char_no = read_line();  //    scanf("%d",&char_no);
            char_count(char_no, &n_char, &cnt, stream);
            printf("%c %d\n", n_char, cnt);
            break;
        case 3: // digram_count()
            if(TEST) printf("Wpisz numer digramu ");
            char_no = read_line();   //  scanf("%d",&char_no);
            digram_count(char_no, digram, stream);
            printf("%c%c %d\n", digram[0], digram[1], digram[2]);
            break;
        case 4:
            find_comments(&line_comment_counter, &block_comment_counter, stream);
            printf("%d %d\n", block_comment_counter, line_comment_counter);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}