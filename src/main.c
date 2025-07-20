#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include "offsets.h"
#include "memoryManagement.h"

unsigned int ammo_offsets[] = {
    0x154, // MTP-57
    0x140, // PISTOL
};

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

    unsigned long health_addr = (unsigned long)player1 + HEALTH_OFFSET;
    printf("Pointeur 3 0x%lx\n",health_addr);
    int health = read_int(mem_fd, health_addr);
    printf("Vie actuelle: %d\n", health);
    
    printf("Position du joueur : x=%f y=%f z=%f\n",read_float(mem_fd,(unsigned long)player1+X_OFFSET),read_float(mem_fd,(unsigned long)player1+Y_OFFSET),read_float(mem_fd,(unsigned long)player1+Z_OFFSET));

    
    // TEST ACCES AU BOT 1

    void* entity_list_ptr = read_pointer(mem_fd,PLAYER_1+0x8); 

    printf("Pointeur entity_list : 0x%lx\n",(unsigned long)entity_list_ptr);

    void * bot1_ptr=read_pointer(mem_fd,(unsigned long)entity_list_ptr +0x8);
    printf("Pointeur bot 1 : 0x%lx\n",(unsigned long)bot1_ptr);

    int health_bot1= read_int(mem_fd,(unsigned long)bot1_ptr+HEALTH_OFFSET);
    printf("Vie bot 1 : %d\n",health_bot1);


    close(mem_fd);
    return 0;
}
