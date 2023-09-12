#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>
#define PATH_SEP '/'
#include "utils.h"

#define FILE_COLOR  "\x1b[34m"
#define EXEC_COLOR  "\x1b[92m"
#define PNG_COLOR   "\x1b[35m"
#define BOLD        "\x1b[1m"
#define RESET       "\x1b[0m"
#define INITIAL_DEPTH  100


//external variable
bool TREE[INITIAL_DEPTH];
char buf[BUFSIZ];

void print_pad(int, bool);
void _tree(char * dirpath, char * parentpath, int padding);
void tree(char * dirpath);

#define VLINE 0x2502
#define HLINE 0x2500
#define VHLINE 0x251c
#define LTLINE 0x2514



int cmp_name(const void * f1, const void * f2){
    int result = strcmp((*(fdtls **) f1 )->file_name, (*(fdtls **) f2 )->file_name);
    return result;
}

void _tree(char * dirpath, char * parentpath, int padding){
    DIR * path;
    char abspath[BUFSIZ];
    struct dirent * path_file; 

    path = opendir(dirpath);

    if(path == NULL) return;
    if(chdir(dirpath) != -1){
        getcwd(abspath, BUFSIZ);
    }

    if(parentpath == NULL){
        printf("%s\n", dirpath);
    }

    fdtl_l * fl = create_file_list();
    if(fl == NULL){
        return;
    }

    while((path_file = readdir(path)) != NULL){
        if(strcmp(path_file->d_name, ".") == 0 ||path_file->d_name[0] == '.' || strcmp(path_file->d_name, "..") == 0)
            continue;
        fdtls * file = create_file(path_file->d_name);
        if(file == NULL) continue;
        append_file(fl, file);
    }

    qsort(fl->file_list, fl->current, sizeof(fdtls *), cmp_name);

    for(int i = 0; i < fl->current; i++){
        fdtls * current_file = fl->file_list[i];
        int is_last = (i != fl->current -1);
                   
        if(S_ISDIR(current_file->fs.st_mode)){
            TREE[padding] = is_last;
            print_pad(padding, is_last);
            
            printf("%s%s%s%s\n",BOLD,FILE_COLOR, current_file->file_name, RESET);
            _tree(current_file->file_name, abspath, padding+1);
        }
        else{
            print_pad(padding, is_last); 
            unsigned mode= current_file->fs.st_mode;
            if(S_IXUSR & mode) 
                printf("%s%s%s%s\n",BOLD, EXEC_COLOR,  current_file->file_name, RESET);
            else
                printf("%s\n",  current_file->file_name);
        }

    }
    destroy_fdtl_l(fl);

    if(parentpath != NULL)
        chdir(parentpath);
}


void print_pad(int padding, bool is_last){
    wchar_t sym[] = {LTLINE, HLINE, HLINE, '\0'};
    wchar_t sym2[] = {VHLINE, HLINE, HLINE, '\0'} ; //symbol for the tree
                                                    
    for(int i = 0; i < padding; i++){
        if(TREE[i])
            printf("%lc   ", VLINE);
        
        else
            printf("    ");
    }

    printf("%ls ", is_last? sym2: sym);
}


void tree(char * dirpath){
    _tree(dirpath, NULL, 0);
}


int main(int argc, char * argv[]){
    setlocale(LC_CTYPE, "en_US.UTF-8");
    setvbuf(stdin, buf, _IOFBF, BUFSIZ);
    memset(TREE, false, INITIAL_DEPTH);
    if(argc == 1){
        tree(".");
    }
    else{
        for(int i = 1; i < argc; i++)
            tree(argv[i]);
    }
}
