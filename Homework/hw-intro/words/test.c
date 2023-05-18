#include<stdlib.h>
#include<stdio.h>

int main(){
    FILE *infile = fopen("words.txt", "r");
    if(infile == NULL){
        return 1;
    }
    if(infile != NULL){
        char* str = (char*)malloc(sizeof(char) * 100);
        memset(str, 0, 100);
        char ch = fgetc(infile);
        int is_new_word = 0;
        while(ch != EOF){
            if(isalpha(ch)){
                if(is_new_word == 0){
                    is_new_word = 1;
                }else if(is_new_word > 0){
                    is_new_word ++;
                }
                str[is_new_word - 1] = tolower(ch);
            }else{
            if(is_new_word > 1){
                printf("%s\n", str);
            }
                is_new_word = 0;
                free(str);
                str = (char*)malloc(sizeof(char) * 100);
                memset(str, 0, 100);
            }
            ch = fgetc(infile);
        }
        if(is_new_word > 1){
            printf("%s\n", str);
        }
    }
    return 0;
}