#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>


#define PLAYER_1 0x5a3518
#define PLAYER_COUNT 0x5a352c

unsigned int ammo_offsets[] = {
    0x154, // MTP-57
    0x140, // PISTOL
};


unsigned int health_offset = 0x100;
unsigned int armor_offset = 0x104;


unsigned int x_offset = 0x8;
unsigned int y_offset = 0xC;
unsigned int z_offset = 0x10;

/*
struct player{

    float32 pos_x; // +0x8
    float32 pos_y; // +0xC
    float32 pos_z; // +0x10

    float32 yaw ; // + 0x14
    float32 pitch; // +0x18
    float32 roll ; // +0x1C

    int health;
    int armor;




}

*/


// Fonction pour lire un pointeur à une adresse donnée
void* read_pointer(int mem_fd, unsigned long addr) {
    void* ptr;
    if (lseek(mem_fd, addr, SEEK_SET) == -1) return NULL;
    if (read(mem_fd, &ptr, sizeof(ptr)) != sizeof(ptr)) return NULL;
    return ptr;
}


unsigned long find_base_address(int pid, const char* module_name) {
    char maps_path[256];
    snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);
    
    FILE* maps_file = fopen(maps_path, "r");
    if (!maps_file) return 0;
    
    char line[1024];
    unsigned long base_addr = 0;
    
    while (fgets(line, sizeof(line), maps_file)) {
        // Chercher la ligne contenant le nom du module
        if (strstr(line, module_name) || (module_name == NULL && strstr(line, "r-xp"))) {
            // Parser l'adresse de début
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

int find_pid_by_name(const char* process_name) {
    DIR* proc_dir = opendir("/proc");
    if (!proc_dir) return -1;
    
    struct dirent* entry;
    char path[256];
    char comm[256];
    FILE* comm_file;
    
    while ((entry = readdir(proc_dir)) != NULL) {
        // Vérifier si c'est un dossier numérique (PID)
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

// Fonction pour lire/écrire un entier
int read_int(int mem_fd, unsigned long addr) {
    int value;
    if (lseek(mem_fd, addr, SEEK_SET) == -1) return -1;
    if (read(mem_fd, &value, sizeof(value)) != sizeof(value)) return -1;
    return value;
}

float read_float(int mem_fd, unsigned long addr) {
    float value;
    if (lseek(mem_fd, addr, SEEK_SET) == -1) return -1;
    if (read(mem_fd, &value, sizeof(value)) != sizeof(value)) return -1;
    return value;
}

void write_int(int mem_fd, unsigned long addr, int value) {
    lseek(mem_fd, addr, SEEK_SET);
    write(mem_fd, &value, sizeof(value));
}




int main(void) {
    
    const char* game_name = "linux_64_client";
    
    int pid = find_pid_by_name(game_name);
    if (pid == -1) {
        printf("Processus '%s' non trouvé\n", game_name);
        return 1;
    }
    
    printf("PID trouvé: %d\n", pid);
    
    char mem_path[256];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);
    
    int mem_fd = open(mem_path, O_RDWR);
    if (mem_fd == -1) {
        perror("open mem");
        return 1;
    }
    
    // Trouver la base du jeu
    unsigned long game_base = find_base_address(pid, "linux_64_client");
    printf("BASE du jeu: 0x%lx\n", game_base);
    
    if (game_base == 0) {
        printf("Impossible de trouver la base du jeu\n");
        close(mem_fd);
        return 1;
    }
    
   
    
    // Votre code existant...
    void* player1 = read_pointer(mem_fd, PLAYER_1);
    printf("\nPointeur 0: 0x%lx\n",(unsigned long)player1); 
    
    unsigned long ammo_addr = (unsigned long)player1 + ammo_offsets[0];
    printf("Pointeur 1: 0x%lx\n",ammo_addr);
    
    int current_ammo = read_int(mem_fd, ammo_addr);
    printf("Munitions actuelles: %d\n", current_ammo);
    
    unsigned long ammo_addr2 = (unsigned long)player1 + ammo_offsets[1];
    printf("Pointeur 2: 0x%lx\n",ammo_addr2);
    
    int current_ammo_pistol = read_int(mem_fd, ammo_addr2);
    printf("Munitions actuelles (pistol): %d\n", current_ammo_pistol);

    unsigned long health_addr = (unsigned long)player1 + health_offset;
    printf("Pointeur 3 0x%lx\n",health_addr);
    int health = read_int(mem_fd, health_addr);
    printf("Vie actuelle: %d\n", health);
    
    printf("Position du joueur : x=%f y=%f z=%f\n",read_float(mem_fd,(unsigned long)player1+x_offset),read_float(mem_fd,(unsigned long)player1+y_offset),read_float(mem_fd,(unsigned long)player1+z_offset));

    
    // TEST ACCES AU BOT 1

    void* entity_list_ptr = read_pointer(mem_fd,PLAYER_1+0x8); 

    printf("Pointeur entity_list : 0x%lx\n",(unsigned long)entity_list_ptr);

    void * bot1_ptr=read_pointer(mem_fd,(unsigned long)entity_list_ptr +0x8);
    printf("Pointeur bot 1 : 0x%lx\n",(unsigned long)bot1_ptr);

    int health_bot1= read_int(mem_fd,(unsigned long)bot1_ptr+health_offset);
    printf("Vie bot 1 : %d\n",health_bot1);


    close(mem_fd);
    return 0;
}