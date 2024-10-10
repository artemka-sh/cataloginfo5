#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>

#define BUF_SIZE 1024

//uncomment to work
//#define _DEBUG   

#ifdef _DEBUG
    #define dprintf(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define dprintf(format, ...) 
#endif
    
int dirs = 0; // каталогов;

int empty_dirs = 0;// пустых каталогов;

int only_dirs = 0;// каталогов, содержащих только каталоги( не включая содержимого подкаталогов;

int files = 0;// файлов;

int less_then_100b_t = 0;// файлов, размером меньше 100 b и с символом “t” в названии;

int more_then_1kb = 0;// файлов, размером больше 1Kb и название начинается с символа “P”;

int less_then_100Kb_txt = 0;// файлов, размером меньше 100 Kb и с расширением “txt”.


void listFilesInDirectory(const char *directory_path) {
    DIR *dir = opendir(directory_path);
    dprintf("%s\n", directory_path);
    if (dir == NULL) {
        perror("open error 1: ");
        printf("%s\n", directory_path);
        return;
    }
    dirs++;

    int _localdirs = 0;
    bool _localOnlyDirs = true;
    struct dirent *entry;
    struct stat file_info;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') 
            continue;

        _localdirs++;
        dprintf("file: %s\n", entry->d_name);

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", directory_path, entry->d_name);
        
        if (stat(full_path, &file_info) == 0) 
            //  if directory
            if (S_ISDIR(file_info.st_mode)) {
                dprintf("DIR.\n");
                dirs++;
                listFilesInDirectory(full_path);
            } 
            else 
            {
                dprintf("FILE.\n");
                files++;
                _localOnlyDirs = false;
                const char *dot = strrchr(entry->d_name, '.');

                if(file_info.st_size/1024 < 100)
                {

                    if (dot != NULL) {
                        // dprintf("Расширение файла: %s\n", dot + 1);
                        if(strcmp(dot + 1, "txt") == 0)
                        {
                            less_then_100Kb_txt++;
                        }
                    } 
                }       

                char *t_position = strchr(entry->d_name, 't');
                if(file_info.st_size < 100 && t_position != NULL)
                {
                    if(dot != NULL && t_position < dot)
                    less_then_100b_t++;
                }


                if(file_info.st_size > 1024 && entry->d_name[0] == 'P')
                {
                    more_then_1kb++;
                }
                    
            }

        dprintf("\n");

        
    }
    if(_localdirs == 0)
    {
        empty_dirs++;
    }
    if(_localOnlyDirs)
    {
        only_dirs++;
    }

    closedir(dir);
}

int main(int argc, char** argv) {
    char path[BUF_SIZE];           
    char input[BUF_SIZE] = "/";
    if(argv[1] != NULL)
        strcat(input, argv[1]);     // example run:      ./"name of executable" ../../   
    else
        input[0] = '\0';    // delete / for opendir funciton

    if (getcwd(path, sizeof(path)) != NULL) {
        strcat(path, input);
        *(strrchr(path, '/') ) = '\0';
        printf("Current work catalog: %s\n", path);
    } else {
        perror("fail to get call catalog"); //try path = '.' if you get this error
        return EXIT_FAILURE;
    }
    
    listFilesInDirectory(path);

    printf("%s%d\n", "just dirs: ", dirs);
    printf("%s%d\n", "empty dirs: ", empty_dirs);
    printf("%s%d\n", "only dirs dirs: ", only_dirs);
    printf("%s%d\n", "files: ", files);
    printf("%s%d\n", "files less 100b with <<t>> in name: ", less_then_100b_t);
    printf("%s%d\n", "files more kb, starts <<P>>: ", more_then_1kb);
    printf("%s%d\n", "less 100kb txt's: ", less_then_100Kb_txt);
    
    return 0;
}

