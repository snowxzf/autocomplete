#include "autocomplete.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int compare(const void *a, const void *b) {
    return strcmp(((term *)a)->term, ((term *)b)->term);
}

void read_in_terms(term **terms, int *pnterms, char *filename){
    FILE *file =fopen(filename, "r");
    
    char line[200];
    
    //get first line fo code
    fgets(line, sizeof(line), file); 
    *pnterms=atoi(line);

    *terms = (term *)malloc(sizeof(term)*(*pnterms));
    if (!*terms){
        exit(1);
    }
    
    for(int i =0;i<*pnterms;i++){
        double weight;
        char name[200];
        if (fgets(line, sizeof(line), file)) {
            sscanf(line, "%lf\t%[^\n]", &weight, name);

            (*terms)[i].weight = weight;
            strcpy((*terms)[i].term, name);  // Copy the term properly
        }
    }
    qsort(*terms, *pnterms, sizeof(term), compare);
}

int lowest_match(term *terms, int nterms, char *substr) {
    int low = 0;
    int high = nterms - 1;
    int result = -1;
    int len = strlen(substr);
    //binary search
    while (low <= high) {
        int mid = (low + high) / 2;

        // Compare only the first 'len' characters of the term
        int cmp = strncmp(terms[mid].term, substr, len);

        if (cmp >= 0) { // Move left if equal or greater
            if (cmp == 0){
                result = mid;// maybe it is this one
            }  // Store possible lowest index
            high = mid - 1;//move lower
        } else { // Move right if less than
            low = mid + 1;
        }
    }

    return result; // Return first match or -1 if not found
}

int highest_match(term *terms, int nterms, char *substr){
    int low = lowest_match(terms, nterms, substr);//get the result from other side
    int high = nterms - 1;
    int result = -1;
    int len = strlen(substr);
    //binary search
    while (low <= high) {
        int mid = (low + high) / 2;
        // compare only the first 'len' characters of the term
        int cmp = strncmp(terms[mid].term, substr, len);

        if (cmp <= 0) { // Move RIGHT if equal or greater
            if (cmp == 0){
                result = mid;// maybe it is this one
            }  // Store possible lowest index
            low = mid + 1;//move HIGHER
        } else { // right if less than
            high=mid-1;
        }
    }
    return result;
}
int compare_answer(const void* a, const void* b) {
    double diff = ((const term*)a)->weight - ((const term*)b)->weight;
    return (diff < 0) - (diff > 0);
}

void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr){
    int lowest_index = lowest_match(terms, nterms, substr);
    int highest_index = highest_match(terms, nterms, substr);
    *n_answer = highest_index - lowest_index + 1;

    //printf("%d\n", *n_answer);
    *answer = (term*)malloc(sizeof(term)*(*n_answer));
    if (!*answer){
        exit(1);
    }
    if(*n_answer ==1){
        strcpy((*answer)[0].term, terms[lowest_index].term);
        (*answer)[0].weight=terms[lowest_index].weight;
    }else{
        for(int i=lowest_index;i<=highest_index;i++){
            strcpy((*answer)[i-lowest_index].term, terms[i].term);
            (*answer)[i-lowest_index].weight=terms[i].weight;
        }
    }
    qsort(*answer, *n_answer, sizeof(term), compare_answer);
}

int main(){
    term *terms;
    char *filename="cities.txt";
    int nterms;
    int *pnterms = &nterms;
    read_in_terms(&terms, pnterms, filename);
    
    char *substr = "A";
    int result_high = highest_match(terms, nterms, substr);
    int result_low=lowest_match(terms, nterms, substr);
    printf("Closest (lowest) to %s was: %s\n", substr, terms[result_low].term);
    printf("Closest (highest) to %s was: %s\n", substr, terms[result_high].term);
    
    
    term *answers;
    int n_ans;
    int *n_answers=&n_ans;
    autocomplete(&answers, n_answers, terms, nterms, substr);
    for(int i=0;i<179;i++){
        printf("answer %d: %s, %f\n", i, answers[i].term, answers[i].weight);
    }
    // Free memory after use
    free(terms);
    free(answers);
    

}