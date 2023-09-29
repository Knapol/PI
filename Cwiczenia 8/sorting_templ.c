#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define TEST 1

typedef struct {
    int day;
    int month;
    int year;
} Date;

/////////////////////////////////////////////////////////////////
// 8.1 funkcja bsearch2

#define FOOD_MAX  30   // max. liczba artykułów
#define ART_MAX   15   // max. długość nazwy artykułu +1
#define RECORD_MAX 40  // max. długość rekordu (linii) w pliku

typedef struct {
    char art[ART_MAX];
    float price;
    float amount;
    Date valid_date;
} Food;

typedef int (*ComparFp)(const void *, const void *);

void *bsearch2(const void *key, void *base, size_t nitems, size_t size, ComparFp compar, char *result) {
    int start = 0;
    int end = nitems-1;
    int s;
    Food *tab = (Food*) base;
    int cmp=0;

    if (nitems==0){
        *(result)=0;
        return tab;
    }

    s = (start+end)/2;
    void *elem;;

    while (start<=end){
        elem = tab+s;
        cmp = compar(key, elem);

        if (cmp == 0){
            *(result) = 1;
            return tab+s;
        }

        if (cmp > 0){
            if (start!=end)
                start = s+1;
        }
        else
        {
            if (start!=end)
                end = s-1;
        }
        int tmp = (start+end)/2;

        if (tmp == s){
            *(result)=0;
            if (cmp > 0)
                return tab+s+1;
            else
                return tab+s;
        }

        s = tmp;
    }
    *(result)=0;
    if (cmp > 0)
        return tab+s+1;
    else
        return tab+s;
}

int datecmp(const void *d1, const void *d2){
    const Date *date1 = d1;
    const Date *date2 = d2;

    int cmp_year = (date1 -> year) - (date2 -> year);
    if (cmp_year != 0)
        return cmp_year;

    int cmp_month = (date1 -> month) - (date2 -> month);
    if (cmp_month != 0)
        return cmp_month;

    int cmp_day = (date1 -> day) - (date2 -> day);
    if (cmp_day != 0)
        return cmp_year;

    return 0;
}

int compar_food(const void *e1, const void *e2){
    const Food *food1 = (Food *)e1;
    const Food *food2 = (Food *)e2;

    int cmp_name = strcmp(food1 -> art, food2 -> art);
    if (cmp_name != 0)
        return cmp_name;

    if (food1 -> price < food2 -> price)
        return -1;

    if (food1 -> price > food2 -> price)
        return 1;

    //double cmp_price = (food1 -> price) - (food2 -> price);
    //if (fabs(cmp_price) > 0.01)
    //    return (int)cmp_price;

    int cmp_date = datecmp(&(food1 -> valid_date), &(food2 -> valid_date));

    return cmp_date;
}

void print_art(Food *p, size_t n, char *art) {
    for (size_t i=0; i<n; i++){
        if (strcmp(p->art, art)==0) {
            Date *date1 = &(p -> valid_date);
            printf("%.2f %.2f %d %d %d\n", p->price, p->amount, date1 -> day, date1 -> month, date1 ->year);
        }
        p++;
    }
}

Food* add_record(Food *tab, size_t tab_size, int *np, ComparFp compar, Food *new) {
    char found;
    Food *food = bsearch2(new, tab, *np, sizeof(Food), compar, &found);

    if(found){
        food -> amount += new -> amount;
    }
    else{
        if (*np + 1 >= tab_size)
            return NULL;

        int counter = *np;

        Food *current = tab + counter;

        while (food <= current){
            *(current+1) = *(current);
            current--;
        }
        *food = *new;

        /*for (Food *c = tab + *np; c >= food; c--)
			memcpy(c+1, c, sizeof(Food));

		memcpy(food, new, sizeof(Food));*/

        /*if (*np == 1){
            Food *x1 = tab;
            Food *x2 = tab+1;
            //Food *x3 = tab+2;
            printf("%s", x1->art);
            printf("%s", x2->art);
            //printf("%s", x3->art);
        }*/
        (*np)++;
    }
    return food;
}

int read_stream(Food *tab, size_t size, int no, FILE *stream) {
    Food food;
    int counter = 0;

    for (int i=0; i<no; i++){
        fscanf(stream, "%s %f %f %d %d %d", food.art, &food.price, &food.amount, &food.valid_date.day, &food.valid_date.month, &food.valid_date.year);
        add_record(tab, size, &counter, compar_food, &food);
    }
    return counter;
}


int date_compare(const void *p1, const void *p2){
    const Food *f1 = p1;
    const Food *f2 = p2;

    const Date *date1 = &(f1->valid_date);
    const Date *date2 = &(f2->valid_date);

    int cmp_year = (date1 -> year) - (date2 -> year);
    if (cmp_year != 0)
        return cmp_year;

    int cmp_month = (date1 -> month) - (date2 -> month);
    if (cmp_month != 0)
        return cmp_month;

    int cmp_day = (date1 -> day) - (date2 -> day);
    if (cmp_day != 0)
        return cmp_day;

    return 0;
}

int read_stream0(Food *tab, size_t size, int no, FILE *stream) {
    Food food;
    int counter = 0;

    for (int i=0; i<no; i++){
        fscanf(stream, "%s %f %f %d %d %d", food.art, &food.price, &food.amount, &food.valid_date.day, &food.valid_date.month, &food.valid_date.year);
        add_record(tab, size, &counter, compar_food, &food);
    }
    qsort(tab, counter, sizeof(Food), date_compare);
    return counter;
}

float value(Food *food_tab, size_t n, Date curr_date, int anticip) {
    int day = curr_date.day;
    int max_day;
    if (curr_date.month == 2)
        max_day = 28;
    else if (curr_date.month == 4 || curr_date.month == 6 || curr_date.month == 9 || curr_date.month == 11)
        max_day = 30;
    else
        max_day = 31;

    if (day+anticip > max_day){
        if (curr_date.month == 12){
            curr_date.month = 1;
            curr_date.year++;
        }
        else
            curr_date.month++;
        curr_date.day = day+anticip-max_day;
    }

    Food tmp;
    tmp.valid_date = curr_date;

    char found;
    Food *food = bsearch2(&tmp, food_tab, n, sizeof(Food), date_compare, &found);

    Food *curr = food;
    float sum = 0;
    while (date_compare(food,curr) == 0){
        sum += curr -> price * curr -> amount;
        curr++;
    }
    curr = food;
    curr--;
    while (date_compare(food,curr) == 0){
        sum += curr -> price * curr -> amount;
        curr--;
    }
    return sum;
}

/////////////////////////////////////////////////////////////////
// 8.3 Sukcesja

#define CHILD_MAX  20   // maksymalna liczba przesuwanej w tablicy grupy osób (elementów tablicy)

enum Sex {F, M};
enum BOOL {no, yes};

struct Bit_data {
    enum Sex sex:1;
    enum BOOL pretendent:1;   // =no - nie pretenduje (panuje albo nie żyje) ale jest w zbiorze dla spójności relacji.
};

typedef struct {
    char *name;
    struct Bit_data bits;
    Date born;
    char *parent;
} Person;

typedef struct {
    char *par_name;
    int index;
    int kids;
} Parent;    // strukturę można rozbudować o pole liczby dzieci

const Date primo_date = { 28, 10, 2011 }; // Data zmiany kolejności sukcesji

int fill_indices_tab(Parent *idx_tab, Person *pers_tab, int size) {
    int cnt=0;
    for (int i=1; i<size; i++){
        if (cnt == 0 || strcmp((idx_tab+cnt-1) -> par_name, (pers_tab+i) -> parent) != 0) {
            //printf("\nxd\n");
            (idx_tab + cnt) -> index = i;
            (idx_tab + cnt) -> par_name = ((pers_tab+i)->parent);
            (idx_tab + cnt) -> kids = 1;
            //printf("%s",(idx_tab+cnt)->par_name);
            //printf("%s",(pers_tab+i)->parent);
            cnt++;
        }
        else {
            //printf("\nxd\n");
            (idx_tab + cnt - 1)->kids += 1;
        }
    }

    /*for (int j=0; j<cnt; j++){
        printf("%d %s %d\n", (idx_tab+j)->index, (idx_tab+j)->par_name, (idx_tab+j)->kids);
    }*/

    return cnt;
}

int binary_cmp(const void *p1, const void *p2){
    const Parent *parent1 = (Parent *)p1;
    const Parent *parent2 = (Parent *)p2;

    int cmp_name = strcmp(parent1 -> par_name, parent2 -> par_name);
    return cmp_name;
}

void print_persons(const Person *person_tab, int n) {
    for(int i=1; i<=n; ++i, ++person_tab) printf("%2d %12s %s\n", i, person_tab->name, person_tab->parent);
    return;
}

void persons_shiftings(Person *person_tab, int size, Parent *idx_tab, int no_parents) {
    Person tmp_tab[size];
    for (int i=0; i<size; i++){
        Parent tmp;
        tmp.par_name = (person_tab+i)->name;

        Parent *child = bsearch(&tmp, idx_tab, no_parents, sizeof(Parent), binary_cmp);

        if (child == NULL)
            continue;

        int cnt = child -> kids;
        int first = child -> index;
        //printf("%d",first);
        //printf("%d %s\n", i, (person_tab+i)->name);

        for (int j=0; j<cnt; j++) {
            tmp_tab[j] = *(person_tab + first + j);
            //printf("%s ", tmp_tab[j].name);
        }

        int index = first-1;
        while (person_tab+index+cnt != person_tab+i){
            memcpy(person_tab+index+cnt, person_tab+index, sizeof(Person));
            index--;
        }

        for (int j=0; j<cnt; j++){
            *(person_tab+i+j+1) = tmp_tab[j];
        }

        child -> index = i+1;
        Parent *curr_child = child-1;
        // && strcmp(curr_child -> par_name, (person_tab)-> name ) != 0
        while (curr_child >=idx_tab){
            curr_child -> index += cnt;
            curr_child--;
        }

        /*print_persons(person_tab, size);
        printf("\n");

        for (int j=0; j<no_parents; j++){
            printf("%d %s %d\n", (idx_tab+j)->index, (idx_tab+j)->par_name, (idx_tab+j)->kids);
        }*/
    }
    //print_persons(person_tab, size);
    //printf("\n");
}

int cleaning(Person *person_tab,int n) {
    int index = 0;
    int last = 0;
    while (index<n-1){
        if ((person_tab+index) -> bits.pretendent == no){
            //printf("%s\n", (person_tab+index) -> name);

        }
        else {
            memcpy(person_tab+last, person_tab+index,sizeof(Person));
            last++;
        }
        index++;
    }
    if ((person_tab+index) -> bits.pretendent == yes){
        memcpy(person_tab+last, person_tab+index,sizeof(Person));
        last++;
    }

    return last;
}

void print_person(const Person *p) {
    printf("%s\n", p->name);
}

int first_sort(const void *p1, const void *p2){
    const Person *person1 = (Person *)p1;
    const Person *person2 = (Person *)p2;

    if (person1 -> parent == NULL){
        return -1;
    }

    if (person2 -> parent == NULL){
        return 1;
    }

    int cmp_name = strcmp(person1 -> parent, person2 -> parent);

    if (cmp_name == 0){
        int cmp_date = datecmp(&(person1 -> born), &(person2-> born));

        if (person1 -> born.year < primo_date.year && person2 -> born.year < primo_date.year){
            if (person1 -> bits.sex == M && person2 -> bits.sex == F )
                return -1;

            if (person1 -> bits.sex == F && person2 -> bits.sex == M)
                return 1;
        }

        return cmp_date;
    }

    return cmp_name;
}

int create_list(Person *person_tab, int n) {
    int size;
    Parent tab[n];
    qsort(person_tab, n, sizeof(Person), first_sort);
    //print_persons(person_tab, n);
    size = fill_indices_tab(tab, person_tab, n);

    persons_shiftings(person_tab, n, tab, size);

    n = cleaning(person_tab, n);
    //print_persons(person_tab, n);

    //printf("\nxd\n");
    return n;
}

////////////////////////////////////////////////////////////////

int main(void)
{
    // Wszyscy potomkowie Jerzego VI (w kolejności przypadkowej):
    Person person_tab[33]={
            {"Charles III",M,no,14,11,1948,"Elizabeth II"},
            {"Anne",F,yes,15,8,1950,"Elizabeth II"},
            {"Andrew",M,yes,19,2,1960,"Elizabeth II"},
            {"Edward",M,yes,10,3,1964,"Elizabeth II"},
            {"David",M,yes,3,11,1961,"Margaret"},
            {"Sarah",F,yes,1,5,1964,"Margaret"},
            {"William",M,yes,21,6,1982,"Charles III"},
            {"Henry",M,yes,15,9,1984,"Charles III"},
            {"Peter",M,yes,15,11,1977,"Anne"},
            {"Zara",F,yes,15,5,1981,"Anne"},
            {"Beatrice",F,yes,8,8,1988,"Andrew"},
            {"Eugenie",F,yes,23,3,1990,"Andrew"},
            {"James",M,yes,17,12,2007,"Edward"},
            {"Louise",F,yes,8,11,2003,"Edward"},
            {"Charles",M,yes,1,7,1999,"David"},
            {"Margarita",F,yes,14,5,2002,"David"},
            {"Samuel",M,yes,28,7,1996,"Sarah"},
            {"Arthur",M,yes,6,5,2019,"Sarah"},
            {"Georg",M,yes,22,7,2013,"William"},
            {"George VI",M,no,14,12,1895,NULL},
            {"Charlotte",F,yes,22,5,2015,"William"},
            {"Louis",M,yes,23,4,2018,"William"},
            {"Archie",M,yes,6,5,2019,"Henry"},
            {"Lilibet",F,yes,4,6,2021,"Henry"},
            {"Savannah",F,yes,29,12,2010,"Peter"},
            {"Isla",F,yes,29,3,2012,"Peter"},
            {"Mia",F,yes,17,1,2014,"Zara"},
            {"Lena",F,yes,18,6,2018,"Zara"},
            {"Elizabeth II",F,no,21,4,1925,"George VI"},
            {"Margaret",F,no,21,8,1930,"George VI"},
            {"Lucas",M,yes,21,3,2021,"Zara"},
            {"Sienna",F,yes,18,9,2021,"Beatrice"},
            {"August",M,yes,9,2,2021,"Eugenie"}
    };

    int to_do, no;
    size_t size, n;
    Food food_tab[FOOD_MAX];
    char buff[ART_MAX];
    FILE *file;
    if(TEST) printf("Wpisz nr zadania (1 - 3) ");
    scanf("%d", &to_do);

    switch (to_do) {
        case 1:  // bsearch2
            if (TEST) printf("Wpisz liczbe linii danych: ");
            scanf("%d",&no); getchar();
            if(TEST) file = stdin;
            else {
                scanf("%s",buff);
                file = fopen(buff,"r");
//            file = fopen("foods0.txt","r");
                if(file==NULL) { printf("Error 1\n"); break; }
            }
            if (TEST) printf("W %d liniach wpisuj dane: nazwa cena ilosc dd mm yyyy: \n",no);
            n = read_stream(food_tab,FOOD_MAX,no,file);
            if (TEST) printf("Wpisz nazwe artykulu: ");
            scanf("%s",buff);
            print_art(food_tab,n,buff);
            break;
        case 2: // proste sortowanie struktur
            if (TEST) printf("Wpisz liczbe linii danych: ");
            scanf("%d",&no); getchar();
            if(TEST) file = stdin;
            else {
                scanf("%s",buff);
                file = fopen(buff,"r");
//            file = fopen("foods0.txt","r");
                if(file==NULL) { printf("Error 1\n"); break; }
            }
            if (TEST) printf("W %d liniach wpisuj dane: nazwa cena ilosc dd mm yyyy: \n",no);
            n = read_stream0(food_tab,FOOD_MAX,no,file);
            Date curr_date;
            int anticip;
            if (TEST) printf("Wpisz date odniesienia dd mm yyyy: ");
            scanf("%d %d %d",&curr_date.day,&curr_date.month,&curr_date.year);
            if (TEST) printf("Wpisz roznice dni: ");
            scanf("%d",&anticip);
            printf("%.2f\n",value(food_tab,n,curr_date,anticip));
            break;
        case 3: // sukcesja
            if(TEST==1) printf("Wpisz pozycję w kolejce (liczona od 1): ");
            scanf("%d",&no);
            int no_persons = sizeof(person_tab) / sizeof(Person);
            no_persons = create_list(person_tab,no_persons);
            if(TEST) print_persons(person_tab,no_persons);
            else print_person(person_tab+no-1);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
    }
    return 0;
}

