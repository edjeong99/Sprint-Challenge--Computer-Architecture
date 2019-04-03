#include <stdio.h>
#include "cpu.h"

/**
 * Main
 */
int main(int argc, char **argv)
{
  if (argc == 2){
  struct cpu cpu;

  cpu_init(&cpu);
  cpu_load(&cpu, argv[1]);
  cpu_run(&cpu);
  }
  else{
    printf("Please use 1 file name when you execute \n");
    exit(1);
  }
  return 0;
}