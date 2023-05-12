//elad fixler 215200684
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include<time.h>
#include<signal.h>


#define BUFFER_SIZE 450
#define LINE_SIZE 150

int main(int argc, char* argv[]) {
    int erorFD = open("errors.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(erorFD, 2);
    if (argc == 1) {
        printf("not enough arguments \n");
        return -1;
    }
    int fd;
    char buffer[BUFFER_SIZE];
    char first_line[LINE_SIZE];
    char second_line[LINE_SIZE];
    char third_line[LINE_SIZE];
    int counter = 0;
    ssize_t num_bytes_read;

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error in: open");
        return -1;
    }
    num_bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    if (num_bytes_read == -1) {
        perror("Error in: read");
        exit(1);
    }
    close(fd);
    int place_counter = 0;
    for(ssize_t i = 0; i < num_bytes_read; i++) {
        if(buffer[i] == '\n') {
            if (counter == 0) {
                first_line[place_counter] = '\0';
            } if (counter== 1) {
                second_line[place_counter] = '\0';
            } if (counter == 2) {
                third_line[place_counter] = '\0';
            }
            counter++;
            place_counter = 0;
        } else {
            if (counter == 0) {
                first_line[place_counter] = buffer[i];
            } if (counter == 1) {
                second_line[place_counter] = buffer[i];
            } if (counter == 2) {
                third_line[place_counter] = buffer[i];
            }
            place_counter++;
        }
    }
    if (counter == 2 && place_counter != 0) {
        third_line[place_counter] = '\0';

    }
    DIR* dir;
    dir = opendir(first_line);
    if (dir == NULL) {
        printf("Not a valid directory\n");
        return -1;
    }
    if (access(second_line, F_OK) != 0) {
        printf("Input file not exist\n");
        closedir(dir);
        return -1;
    }
    if (access(third_line, F_OK) != 0) {
        printf("Output file not exist\n");
        closedir(dir);
        return -1;
    }
    int results = open("results.csv", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (results == -1) {
        perror("Error in: open");
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        char bounded_path[1000] = "";
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (entry->d_type == DT_DIR) {
            //start the compile here
            char dirertory[LINE_SIZE];
            if(getcwd(dirertory, sizeof(dirertory)) == NULL) {
                perror("Eror in: getcwd:");
                return -1;
            }
            strcat(dirertory, "/");
            strcat(dirertory, first_line);
            strcat(dirertory, "/");
            strcat(dirertory, entry->d_name);
            DIR* subdir;
            subdir = opendir(dirertory);
            if (subdir == NULL) {
                perror("Error in: opendir:");
                return -1;
            }
            struct dirent *ent;
            char cfile[LINE_SIZE] = "";
            while ((ent = readdir(subdir)) != NULL) {
                strcpy(cfile, "");
                if (strcmp(ent->d_name + strlen(ent->d_name) - 2, ".c") == 0) {
                    // Found a C file
                    strcat(cfile, dirertory);
                    strcat(cfile, "/");
                    strncat(cfile, ent->d_name, sizeof(cfile) - 1);
                    struct stat file_stat;
                    //printf(cfile);
                    if (stat(cfile, &file_stat) < 0) {
                        perror("Error in: stat");
                        continue;
                    }
                    if (S_ISDIR(file_stat.st_mode)) {
                        continue;
                    }
                    strcat(bounded_path, first_line);
                    strcat(bounded_path, "/");
                    strcat(bounded_path, entry->d_name);
                    strcat(bounded_path, "/");
                    strcat(bounded_path, ent->d_name);
                    break;
                }
            }
            if (strcmp(cfile, "") == 0) {
                //threre is no file grade is 0
                char to_print[LINE_SIZE];
                strcpy(to_print, entry->d_name);
                strcat(to_print, ",0,NO_C_FILE\n");
                ssize_t bits = write(results, to_print, strlen(to_print));
                if(bits < 0) {
                    //oi
                    int a;
                }
            } else {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("Error in: fork");
                    return -1;
                }
                if (pid == 0) {
                    //Child
                    //compile:
                    pid_t pid2 = fork();
                    if (pid2 == -1 ){
                        perror("error in: fork");
                        continue;
                    }
                    if (pid2 == 0) {
                        //child child - here compile
                        execlp("gcc", "gcc","-w", bounded_path, NULL);
                        perror("Error in: execlp");
                        _exit -1;
                    } else {
                        //child parent - here run
                        int status;
                        if (waitpid(pid2, &status, 0) == -1) {
                            perror("Error in: waitpid");
                            return -1;
                        }
                        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                            //now run
                            char outfile[LINE_SIZE];
                            char withPoint[LINE_SIZE] = ".";
                            strcat(withPoint, outfile);
                            const char* path = withPoint;
                            int fd = open("tempOutput.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            char cose[1000];
                            strcat(cose, outfile);
                            int dfIn = open(second_line, O_RDONLY);
                            dup2(dfIn, 0);
                            dup2(fd, 1);
                            close(fd);
                            close(dfIn);
                            execlp("./a.out", "./a.out", NULL);
                            perror("Error in: exec");
                            exit(-1);
                        
                        } else {
                            //cant coplied
                            exit(50);
                            //_exit(status);
                        }
                        exit(1);
                    }
                } else {
                    //parent
                    //int pid2 = 1;
                    pid_t pid2 = fork();
                    if (pid2 == -1) {
                        perror("error in: fork");
                        continue;
                    }
                    if (pid2 == 0) {
                        //parent child count time
                    time_t start_time, current_time;
                    start_time = time(NULL);
                    current_time = start_time;
                    while ((current_time - start_time) < 5) {
                        current_time = time(NULL);
                    }
                    exit(10);
                    } else {
                        //parent parent here wrote outt
                        int status;
                        int returned = wait(&status);
                        printf("pid on timer is: %d, pid returned is: %d\n", pid2, returned);
                        if (returned == -1) {
                            perror("Error in: wait");
                            continue;
                        }
                        if (returned == pid2) {
                            //5 seconds ended
                            printf("5 seconds ended");
                            char to_print[LINE_SIZE];
                                strcpy(to_print, entry->d_name);
                                strcat(to_print, ",20,TIMEOUT\n");
                                ssize_t bits = write(results, to_print, strlen(to_print));
                                if(bits < 0) {
                                    //oi
                                    int a;
                                }

                        } else {
                            //kill(pid2, SIGKILL);
                            int exit_status = WEXITSTATUS(status);
                            if (exit_status == 50){
                                char to_print[LINE_SIZE];
                                strcpy(to_print, entry->d_name);
                                strcat(to_print, ",10,COMPILATION_ERROR\n");
                                ssize_t bits = write(results, to_print, strlen(to_print));
                                if(bits < 0) {
                                    //oi
                                    int a;
                                }
                            }
                            else{
                                pid_t pid3 = fork();
                                if (pid3 < 0) {
                                    perror("Error in: fork");
                                    continue;
                                }
                                if (pid3 == 0) {
                                    //son - run comp
                                    execl("./comp.out", "./comp.out", third_line, "tempOutput.txt");
                                    perror("Error in: exec");
                                    return(-1);
                                } else {
                                    //parnent
                                    int status;
                                    int a = wait(&status);
                                    if (a < 0) {
                                        perror("Error in: wait");
                                        continue;
                                    }
                                    int value_got =WEXITSTATUS(status);
                                    char to_print[LINE_SIZE];
                                    strcpy(to_print, entry->d_name);
                                    if (value_got == 1) {
                                        //same
                                        strcat(to_print, ",100,EXCELLENT\n");
                                    } else if (value_got == 2) {
                                        //diff
                                        strcat(to_print, ",50,WRONG\n");
                                    } else if (value_got == 3) {
                                        //similar
                                        strcat(to_print, ",75,SIMILAR\n");
                                    }
                                        ssize_t bits = write(results, to_print, strlen(to_print));
                                        if(bits < 0) {
                                            //oi
                                            int a;
                                        }
                                }
                                remove("tempOutput.txt");
                            } 
                        }
                    }
                }
            }
            closedir(subdir);
        }
    }
    close(results);
    closedir(dir);
}
