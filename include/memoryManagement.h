#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

void* read_pointer(int mem_fd, unsigned long addr);
unsigned long find_base_address(int pid, const char* module_name);
int find_pid_by_name(const char* process_name);
int read_int(int mem_fd, unsigned long addr);
float read_float(int mem_fd, unsigned long addr);
void write_int(int mem_fd, unsigned long addr, int value) ;
void write_float(int mem_fd, unsigned long addr, float value) ;

#endif