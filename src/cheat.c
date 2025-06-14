#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <linux/input.h>
#include "offsets.h"
#include "memoryManagement.h"
#include "mathTool.h"


const char* game_name = "linux_64_client"; /* assaultcube sous linux */
const char* device = "/dev/input/event6"; 

int mouse_status(int fd){
    struct input_event ev;
    ssize_t n = read(fd, &ev, sizeof(struct input_event));
    
    if (n == (ssize_t)sizeof(struct input_event)) {
        if (ev.type == EV_KEY && ev.code == BTN_RIGHT) {
            if (ev.value == 1) {
                return 1; // Clic droit pressé
            } else if (ev.value == 0) {
                return 0; // Clic droit relâché
            }
        }
    }
    return -1; // Pas d'événement de clic droit ou erreur
}


int main(){
    int pid = find_pid_by_name(game_name);

    

    int fd = open(device, O_RDONLY);
    if (fd < 0) {
        perror("Erreur lors de l'ouverture du périphérique");
        return 1;
    }



    
    if (pid == -1) {
        printf("Processus '%s' non trouvé\n", game_name);
        return 1;
    }

    char mem_path[256];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);
    
    int mem_fd = open(mem_path, O_RDWR);
    if (mem_fd == -1) {
        perror("open mem");
        return 1;
    }

    /* On va afficher la position et la vie de tous les autres joueurs de la partie et dire si ils sont ou pas dans notre champ de vision*/
    void * player = read_pointer(mem_fd,PLAYER_1); /* on  récupère le pointeur associé à notre joueur*/
    
    Vec3 entity_vector;

    int player_nb = read_int(mem_fd,PLAYER_COUNT);
    
    void * entity_list_ptr=read_pointer(mem_fd,PLAYER_1+0x8);
    int was_right_clicked = 0;
    while (1){
        int mouse_state = mouse_status(fd);
        int right_clicked = (mouse_state == 1);
        int new_click = right_clicked && !was_right_clicked;

        

        // Convertir les float en double pour mathTool
        Vec3 player_pos = {
            (double)read_float(mem_fd,(unsigned long)player + X_OFFSET),
            (double)read_float(mem_fd,(unsigned long)player + Y_OFFSET),
            (double)read_float(mem_fd,(unsigned long)player + Z_OFFSET)
        };

        double player_yaw = (double)read_float(mem_fd,(unsigned long)player+YAW_OFFSET);
        double player_pitch = (double)read_float(mem_fd,(unsigned long)player+PITCH_OFFSET);

        for (unsigned int i=0x8 ; i<0x8 * ((unsigned int)(player_nb)); i+=0x8){

            /* ici i multiple de 0x8 et correspond au pointeur associé au joueur */

            void * entity_ptr=read_pointer(mem_fd,(unsigned long)entity_list_ptr +i);
            int health_entity= read_int(mem_fd,(unsigned long)entity_ptr+HEALTH_OFFSET);
            float entity_x = read_float(mem_fd,(unsigned long)entity_ptr+X_OFFSET);
            float entity_y = read_float(mem_fd,(unsigned long)entity_ptr+Y_OFFSET);
            float entity_z = read_float(mem_fd,(unsigned long)entity_ptr+Z_OFFSET);

            entity_vector.x=entity_x;
            entity_vector.y=entity_y;
            entity_vector.z=entity_z;

            Vec3 entity_pos = {(double)entity_x, (double)entity_y, (double)entity_z};
            
            bool looking = isLookingAt(player_pos, player_yaw, player_pitch, entity_pos);
            
            //printf("Entity_ptr %d : 0x%lu \n",i/0x8,(unsigned long)entity_ptr);
            
            //printf("\nEntité %u : vie = %d x=%f y=%f z=%f\n",i/0x8,health_entity,entity_vector.x,entity_vector.y,entity_vector.z);
            //printf("Joueur regarde l'entité %u : %s\n", i/0x8, looking ? "OUI" : "NON");
            if (new_click && looking){
                /* ON va positionner le joueur sur la tete du bot*/
                Vec2 aimAngles = getYawPitch(player_pos, entity_pos);
                write_float(mem_fd,(unsigned long)player + YAW_OFFSET,aimAngles.x);
                write_float(mem_fd,(unsigned long)player + PITCH_OFFSET,aimAngles.y);

                printf("\nEntité %u : vie = %d x=%f y=%f z=%f\n",i/0x8,health_entity,entity_vector.x,entity_vector.y,entity_vector.z);
                printf("Joueur regarde l'entité %u : %s\n", i/0x8, looking ? "OUI" : "NON");
            }
            
        }
        //printf("PLAYER x = %f y= %f z= %f\n",player_vector.x,player_vector.y,player_vector.z);
        sleep(0.5);
    }
    

    close(fd);
    return 0;
}