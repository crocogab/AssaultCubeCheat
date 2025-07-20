#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <linux/input.h>
#include <time.h>
#include "offsets.h"
#include "memoryManagement.h"
#include "mathTool.h"


const char* game_name = "linux_64_client"; /* assaultcube sous linux */

// Cheat configuration
#define AIM_HEAD_OFFSET 1.0    // Z offset for headshots
#define LOOP_DELAY_MS 50       // Delay between checks in milliseconds
#define STATUS_DISPLAY_INTERVAL 100  // Show status every N loops
#define MAX_MOUSE_DEVICES 20   // Max number of input devices to check
#define MAX_TARGET_DISTANCE 100.0  // Maximum distance to consider for targeting

// Function to find a working mouse device
int find_mouse_device(char* device_path, size_t path_size) {
    char test_path[256];
    for (int i = 0; i < MAX_MOUSE_DEVICES; i++) {
        snprintf(test_path, sizeof(test_path), "/dev/input/event%d", i);
        int fd = open(test_path, O_RDONLY | O_NONBLOCK);
        if (fd >= 0) {
            // Test if this device can give us mouse events
            close(fd);
            strncpy(device_path, test_path, path_size - 1);
            device_path[path_size - 1] = '\0';
            return 0;
        }
    }
    return -1;
} 

int mouse_status(int fd){
    struct input_event ev;
    ssize_t n = read(fd, &ev, sizeof(struct input_event));
    
    if (n == (ssize_t)sizeof(struct input_event)) {
        if (ev.type == EV_KEY && ev.code == BTN_RIGHT) {
            if (ev.value == 1) {
                return 1; // Right click pressed
            } else if (ev.value == 0) {
                return 0; // Right click released
            }
        }
    }
    return -1; // No relevant event or error
}


int main(int argc, char *argv[]){
    // Simple help system
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printf("=== AssaultCube Aimbot ===\n");
            printf("Usage: %s [options]\n\n", argv[0]);
            printf("Options:\n");
            printf("  -h, --help    Show this help message\n");
            printf("  -v, --version Show version information\n\n");
            printf("Controls:\n");
            printf("  - Right-click while looking at an enemy to auto-aim\n");
            printf("  - The aimbot targets the closest enemy you're looking at\n");
            printf("  - Press Ctrl+C to exit\n\n");
            printf("Configuration:\n");
            printf("  - Head offset: %.1f units\n", AIM_HEAD_OFFSET);
            printf("  - Max target distance: %.1f units\n", MAX_TARGET_DISTANCE);
            printf("  - Update frequency: %d ms\n", LOOP_DELAY_MS);
            return 0;
        }
        if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
            printf("AssaultCube Aimbot v2.0 - Improved Edition\n");
            printf("Features: Auto mouse detection, distance-based targeting, headshot aim\n");
            return 0;
        }
    }
    int pid = find_pid_by_name(game_name);
    if (pid == -1) {
        printf("Processus '%s' non trouvé\n", game_name);
        return 1;
    }
    printf("Found game process with PID: %d\n", pid);

    // Find and open mouse device
    char device_path[256];
    if (find_mouse_device(device_path, sizeof(device_path)) != 0) {
        printf("Erreur: impossible de trouver un périphérique de souris\n");
        return 1;
    }
    printf("Using mouse device: %s\n", device_path);

    int fd = open(device_path, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Erreur lors de l'ouverture du périphérique");
        return 1;
    }

    char mem_path[256];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);
    
    int mem_fd = open(mem_path, O_RDWR);
    if (mem_fd == -1) {
        perror("open mem");
        return 1;
    }

    printf("=== AssaultCube Aimbot Started ===\n");
    printf("Controls: Right-click while looking at an enemy to auto-aim\n");
    printf("Press Ctrl+C to exit\n\n");

    /* Get game memory information */
    void * player = read_pointer(mem_fd,PLAYER_1); /* on  récupère le pointeur associé à notre joueur*/
    if (!player) {
        printf("Erreur: impossible de lire le pointeur du joueur\n");
        close(mem_fd);
        close(fd);
        return 1;
    }
    
    Vec3 entity_vector;

    int player_nb = read_int(mem_fd,PLAYER_COUNT);
    printf("Detected %d players in game\n", player_nb);
    
    void * entity_list_ptr=read_pointer(mem_fd,PLAYER_1+0x8);
    int was_right_clicked = 0;
    int loop_count = 0;
    
    while (1){
        loop_count++;
        int mouse_state = mouse_status(fd);
        int right_clicked = (mouse_state == 1);
        int new_click = right_clicked && !was_right_clicked;

        // Show status every STATUS_DISPLAY_INTERVAL loops
        if (loop_count % STATUS_DISPLAY_INTERVAL == 0) {
            printf("Aimbot running... (loop %d)\n", loop_count);
        }

        // Convertir les float en double pour mathTool
        Vec3 player_pos = {
            (double)read_float(mem_fd,(unsigned long)player + X_OFFSET),
            (double)read_float(mem_fd,(unsigned long)player + Y_OFFSET),
            (double)read_float(mem_fd,(unsigned long)player + Z_OFFSET)
        };

        double player_yaw = (double)read_float(mem_fd,(unsigned long)player+YAW_OFFSET);
        double player_pitch = (double)read_float(mem_fd,(unsigned long)player+PITCH_OFFSET);

        // Variables for finding the best target
        double closest_distance = MAX_TARGET_DISTANCE;
        Vec3 best_target = {0, 0, 0};
        bool found_target = false;
        int target_entity_id = -1;

        for (unsigned int i=0x8 ; i<0x8 * ((unsigned int)(player_nb)); i+=0x8){

            /* ici i multiple de 0x8 et correspond au pointeur associé au joueur */

            void * entity_ptr=read_pointer(mem_fd,(unsigned long)entity_list_ptr +i);
            int health_entity= read_int(mem_fd,(unsigned long)entity_ptr+HEALTH_OFFSET);
            float entity_x = read_float(mem_fd,(unsigned long)entity_ptr+X_OFFSET);
            float entity_y = read_float(mem_fd,(unsigned long)entity_ptr+Y_OFFSET);
            float entity_z = read_float(mem_fd,(unsigned long)entity_ptr+Z_OFFSET);

            // Skip dead enemies
            if (health_entity <= 0) {
                continue;
            }

            entity_vector.x=entity_x;
            entity_vector.y=entity_y;
            entity_vector.z=entity_z;

            Vec3 entity_pos = {(double)entity_x, (double)entity_y, (double)entity_z};
            double dist = distance3D(player_pos, entity_pos);
            
            bool looking = isLookingAt(player_pos, player_yaw, player_pitch, entity_pos);
            
            // If right-clicking and looking at this enemy, check if it's the closest
            if (new_click && looking && dist < closest_distance) {
                closest_distance = dist;
                best_target = entity_pos;
                found_target = true;
                target_entity_id = i/0x8;
                
                printf("Found target %d at distance %.2f\n", target_entity_id, dist);
            }
            
            //printf("Entity_ptr %d : 0x%lu \n",i/0x8,(unsigned long)entity_ptr);
            //printf("\nEntité %u : vie = %d x=%f y=%f z=%f\n",i/0x8,health_entity,entity_vector.x,entity_vector.y,entity_vector.z);
            //printf("Joueur regarde l'entité %u : %s\n", i/0x8, looking ? "OUI" : "NON");
            
        }

        // Aim at the best target found
        if (found_target) {
            /* Target the head (add slight Z offset for headshots) */
            Vec3 head_pos = {best_target.x, best_target.y, best_target.z + AIM_HEAD_OFFSET};
            Vec2 aimAngles = getYawPitch(player_pos, head_pos);
            write_float(mem_fd,(unsigned long)player + YAW_OFFSET, (float)aimAngles.x);
            write_float(mem_fd,(unsigned long)player + PITCH_OFFSET, (float)aimAngles.y);

            printf("\n*** AIMBOT ACTIVATED ***\n");
            printf("Targeting entity %d at distance %.2f\n", target_entity_id, closest_distance);
            printf("Target position: x=%.2f y=%.2f z=%.2f\n", best_target.x, best_target.y, best_target.z);
            printf("************************\n");
        }
        //printf("PLAYER x = %f y= %f z= %f\n",player_vector.x,player_vector.y,player_vector.z);
        
        // More responsive timing using configuration
        struct timespec ts = {0, LOOP_DELAY_MS * 1000000}; // Convert ms to nanoseconds  
        nanosleep(&ts, NULL);
        
        was_right_clicked = right_clicked;
    }
    
    close(fd);
    return 0;
}
