#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>


/*

Prends la mémoire en entrée et une adresse et renvoie le pointeur associé à l'adresse mémoire

*/
void* read_pointer(int mem_fd, unsigned long addr) {
    void* ptr;
    if (lseek(mem_fd, addr, SEEK_SET) == -1) return NULL;
    if (read(mem_fd, &ptr, sizeof(ptr)) != sizeof(ptr)) return NULL;
    return ptr;
}

/*
Trouve l'adresse mémoire d'un processus grâce à son nom et son PID
*/

unsigned long find_base_address(int pid, const char* module_name) {
    char maps_path[256];
    snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);
    
    FILE* maps_file = fopen(maps_path, "r");
    if (!maps_file) return 0;
    
    char line[1024];
    unsigned long base_addr = 0;
    
    while (fgets(line, sizeof(line), maps_file)) {
        
        if (strstr(line, module_name) || (module_name == NULL && strstr(line, "r-xp"))) {
           
            char* dash = strchr(line, '-');
            if (dash) {
                *dash = '\0';
                base_addr = strtoul(line, NULL, 16);
                break;
            }
        }
    }
    
    fclose(maps_file);
    return base_addr;
}

/* Renvoie le PID associé à un processus particulier*/
int find_pid_by_name(const char* process_name) {
    DIR* proc_dir = opendir("/proc");
    if (!proc_dir) return -1;
    
    struct dirent* entry;
    char path[256];
    char comm[256];
    FILE* comm_file;
    
    while ((entry = readdir(proc_dir)) != NULL) {
        
        int pid = atoi(entry->d_name);
        if (pid <= 0) continue;
        
        // Lire le nom du processus depuis /proc/PID/comm
        snprintf(path, sizeof(path), "/proc/%d/comm", pid);
        comm_file = fopen(path, "r");
        if (!comm_file) continue;
        
        if (fgets(comm, sizeof(comm), comm_file)) {
            // Enlever le retour à la ligne
            comm[strcspn(comm, "\n")] = 0;
            
            if (strcmp(comm, process_name) == 0) {
                fclose(comm_file);
                closedir(proc_dir);
                return pid;
            }
        }
        fclose(comm_file);
    }
    
    closedir(proc_dir);
    return -1;
}


/* Fonction pour lire un entier à un endroit spécifique de la mémoire */
int read_int(int mem_fd, unsigned long addr) {
    int value;
    if (lseek(mem_fd, addr, SEEK_SET) == -1) return -1;
    if (read(mem_fd, &value, sizeof(value)) != sizeof(value)) return -1;
    return value;
}

/* Fonction pour lire un flottant à un endroit spécifique de la mémoire */
float read_float(int mem_fd, unsigned long addr) {
    float value;
    if (lseek(mem_fd, addr, SEEK_SET) == -1) return -1;
    if (read(mem_fd, &value, sizeof(value)) != sizeof(value)) return -1;
    return value;
}


/* Fonction pour ecrire un entier à un endroit spécifique de la mémoire */
void write_int(int mem_fd, unsigned long addr, int value) {
    lseek(mem_fd, addr, SEEK_SET);
    write(mem_fd, &value, sizeof(value));
}

/* Fonction pour ecrire un flottant à un endroit spécifique de la mémoire */
void write_float(int mem_fd, unsigned long addr, float value) {
    lseek(mem_fd, addr, SEEK_SET);
    write(mem_fd, &value, sizeof(value));
}
