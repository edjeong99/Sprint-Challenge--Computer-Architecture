#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_LEN 6

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */

unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index){
  return cpu->ram[index];
}

void cpu_ram_write(struct cpu *cpu, unsigned char index, unsigned char value){
  cpu->ram[index] = value;
}

void cpu_load(struct cpu *cpu, char *file)
{
  // char data[DATA_LEN] = {
  //   // From print8.ls8
  //   0b10000010, // LDI R0,8
  //   0b00000000,
  //   0b00001000,
  //   0b01000111, // PRN R0
  //   0b00000000,
  //   0b00000001  // HLT
  // };
  // int address = 0;
  // for (int i = 0; i < DATA_LEN; i++) {
  //   cpu->ram[address++] = data[i];
  // }

  // TODO: Replace this with something less hard-coded
  FILE *fp;
  int buffer, address = 0;
 
  unsigned char line[1024];
  fp = fopen(file, "r");
   if (fp == NULL) {
    perror("File not found.");
    exit(1);
  }
// read one line at a time and assign it to cpu->ram
  while(fgets(line,sizeof line,fp)!= NULL) {     
    char *endptr;
      unsigned char val = strtoul(line, &endptr, 2);
          if(line == endptr){ // if there is no binary num in line
         printf("Skipping : %s\n", line);
         continue;
       }
       cpu->ram[address] = val ;
       if(line == endptr){
         printf("Skipping : %s\n", line);
         continue;
       }
    //    printf ("ram: %x\n",cpu->ram[address]);
      address++;
    }

    
  fclose(fp);


}



/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{

// not sure about how to use enum alu_op
  switch (op) {
    case MUL:
      // TODO
      cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
    //  printf("ALU MUL executing reg %d and reg %d result = %d\n",regA, regB, cpu->registers[regA] );
      break;
    case ADD:
      cpu->registers[regA] = cpu->registers[regA] + cpu->registers[regB];
   //    printf("ADD reg %d and reg %d result = %d\n",regA, regB, cpu->registers[regA] );
      break;

    case CMP:
      if(cpu->registers[regA] == cpu->registers[regB]){
          cpu->FL =  0b00000001;
      }
      if(cpu->registers[regA] > cpu->registers[regB]){
        //set the Greater-than G flag to 1, otherwise set it to 0
        cpu->FL =  0b00000010;
      }
      if(cpu->registers[regA] < cpu->registers[regB]){
        // set the Less-than L flag to 1, otherwise set it to 0
         cpu->FL =  0b00000100;
      }
       printf("CMP flag is = %x\n",cpu->FL );
      break;
  
    
  }
}



void handle_LDI(struct cpu *cpu, unsigned char operandA, unsigned char operandB, unsigned char num_operands) { 
    cpu->registers[operandA] = operandB;
   // printf("LDI executing reg %d = %d\n", operandA, operandB);
    cpu->PC += 1 + num_operands;
}
void handle_PRN(struct cpu *cpu, unsigned char operandA,  unsigned char num_operands) { 
    printf("PRN %d\n",  cpu->registers[operandA]);
    cpu->PC += 1 + num_operands;
}

int handle_HLT(struct cpu *cpu ) { 
   // printf("HLT executing\n");
    return 0;
}

// void handle_MUL(struct cpu *cpu, unsigned char operandA,  unsigned char operandB, unsigned char num_operands) { 
//     alu(cpu, ALU_MUL, operandA, operandB );
//     printf("MUL executing reg %d and reg %d result = %d\n",operandA, operandB, cpu->registers[operandA] );
//     cpu->PC += 1 + num_operands;
// }

// handle push for stack
void handle_PUSH(struct cpu *cpu, unsigned char value,  unsigned char num_operands){
  cpu->registers[SP]--;
  cpu->ram[cpu->registers[SP]] = value;
  cpu->PC += 1 + num_operands;
  // printf("PUSH address %d and value =  %d\n",cpu->registers[07],value);
}
// pop the stack and return the value 
unsigned char handle_POP(struct cpu *cpu,  unsigned char num_operands){
  cpu->PC += 1 + num_operands; 
  if( cpu->registers[SP] < 0xF4){  // 0xF4 above is reserved area   
    cpu->registers[SP]++;
  //  printf("POP address %d and value =  %d\n",cpu->registers[07]-1,cpu->ram[cpu->registers[07]-1]);
    return cpu->ram[cpu->registers[SP]-1];
  }
  return NULL;
}

// call set next operand in SP and set PC to call address
void handle_CALL(struct cpu *cpu, unsigned char value,  unsigned char num_operands){
  cpu->registers[SP]--;
  cpu->ram[cpu->registers[SP]] = cpu->PC+2;
  cpu->PC = cpu->registers[value];
  // printf("CALL reg num = %d   cpu->PC (register[value]) =  %d\n",value, cpu->PC);
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction
  unsigned char reg_num, val, IR, operandA, operandB, num_operands;
  int mask;  // mask is used to check vertain value in binary num.

  while (running) {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    IR = cpu->ram[cpu->PC];
 
  // 2. Figure out how many operands this next instruction requires
    num_operands = IR >> 6;

 // 3. Get the appropriate value(s) of the operands following this instruction
    if(num_operands > 0){  // if more than 1 operands
      operandA = cpu->ram[cpu->PC + 1];
 //   printf("operandA  = %d\n", operandA);

    }
    if (num_operands > 1){ // if there are two operands
      operandB = cpu->ram[cpu->PC + 2];
   //   printf("operandB  = %d\n", operandB);
    }

   
   // check alu flag and see if ALU bit is true
    mask = 0b00100000;
    // 
    if ((IR & mask) == 0b00100000)
    {
      alu(cpu, IR, operandA, operandB);
      cpu->PC += 1 + num_operands;
    }   

    else{ // if it's not alu operation
    // 4. switch() over it to decide on a course of action.
        printf("IR is %d\n", IR);
        switch (IR) {

    // 5. Do whatever the instruction should do according to the spec.
    // 6. Move the PC to the next instruction.

            case LDI:    
                handle_LDI(cpu, operandA, operandB, num_operands);
                break;

           case PRN:
                handle_PRN(cpu, operandA, num_operands);
                break;

            case HLT:
                running = handle_HLT(cpu);
                break;

          
          // MUL multiply values in two registers and put the result in the 1st register
          //  case MUL:
          //       handle_MUL(cpu, operandA, operandB, num_operands);
          //       break;

           case PUSH:
                handle_PUSH(cpu, cpu->registers[operandA], num_operands);
                break;

           case POP:
                cpu->registers[operandA] = handle_POP(cpu, num_operands);
                break;


          
          case CALL:
            handle_CALL(cpu, operandA, num_operands);
            break;
          //RET: return PC from subroutine by moving PC to the address stored on top of stack
          
          // RET set PC to where stack pops
          case RET:
            cpu->PC = handle_POP(cpu, num_operands);   
            printf("RET  cpu->PC = %d\n", cpu->PC);     
            break;

          case JMP:
            cpu->PC = cpu->registers[operandA];   
            printf("JMP  cpu->PC = %d\n", cpu->PC);     
            break;

          case JEQ:
            if(cpu->FL & 0b00000001 ){ // if equal flag is true
              cpu->PC = cpu->registers[operandA];   
              printf("JEQ EQUAL  cpu->PC = %d\n", cpu->PC);     
            }  
            else{
              cpu->PC += 1 + num_operands;
               printf("JEQ NOT EQUAL  cpu->PC = %d\n", cpu->PC);
            }
            break;

          case JNE:
            if(!(cpu->FL & 0b00000001) ){ // if equal flag is false
              cpu->PC = cpu->registers[operandA];   
              printf("JNE worked cpu->PC = %d\n", cpu->PC);     
            }
             else{
              cpu->PC += 1 + num_operands;
               printf("JNE NOT worked  cpu->PC = %d\n", cpu->PC);
            }
            break;

          default:
                printf("Unrecognized instruction\n");
                exit(1);
                break;

    

  }
    }
}
}
/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers

  cpu->PC = 0;
  memset(cpu->registers, 0, sizeof(cpu->registers)); 
  memset(cpu->ram, 0, sizeof(cpu->ram)); 
  cpu->registers[SP] = 0xF4; // begining point for Stack Pointer (SP)

}
