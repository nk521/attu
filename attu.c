// To compile : gcc attu.c -o attu.o


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STACK_SIZE 128
static int stack[STACK_SIZE];

typedef enum {
    HLT, //0
    PSH, //1
    POP, //2
    ADD, //3
    SUB, //4
    MUL, //5
    DIV, //6
    XOR, //7
    AND, //8
    OR,  //9
    MOD, //10
    SLT, //11
    MOV, //12
    SET, //13
    LOG, //14
    IF,  //15
    IFN, //16
    GLD, //17
    GPT, //18
    NOP  //19
} Instructions;

typedef enum {
    A, B, C, D, E, F, I, J,
    EX,
    EXA,
    int_ptr,
    stk_ptr,
    REGISTER_SIZE
} Registers;
 
static int registers[REGISTER_SIZE];


int *instructions;


int instruction_count = 0;

int max_inst = 4;

int debug = 0;

static bool running = true;

bool is_jmp = false;

#define stk_ptr (registers[stk_ptr])
#define int_ptr (registers[int_ptr])
#define FETCH (instructions[int_ptr])

void executor(int instr) {
    is_jmp = false;
    switch (instr) {
        case HLT: {
            if(debug==1)
                printf("[-] HLT HALTING!\n");
            running = false;
            printf("[!] Exec Finished!\n");
            break;
        }
        case PSH: {
            stk_ptr = stk_ptr + 1;
            int_ptr = int_ptr + 1;
            if(debug==1)
                printf("[-] PSH pushing value (0x%x|%d) to position (0x%x|%d)\n",instructions[int_ptr],instructions[int_ptr],stk_ptr,stk_ptr);
            stack[stk_ptr] = instructions[int_ptr];
            break;
        }
        case POP: {
            if(debug==1)
                printf("[-] POP popping value from stack\n");
            stk_ptr = stk_ptr - 1;
            break;
        }
        case ADD: {
            registers[A] = stack[stk_ptr];
            stk_ptr = stk_ptr - 1;

            registers[B] = stack[stk_ptr];
            

            registers[C] = registers[B] + registers[A];

            stack[stk_ptr] = registers[C];
            printf("\t%d + %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case MUL: {
            registers[A] = stack[stk_ptr];
            stk_ptr = stk_ptr - 1;

            registers[B] = stack[stk_ptr];
            
            registers[C] = registers[B] * registers[A];

            stack[stk_ptr] = registers[C];
            printf("\t%d * %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case DIV: {
            registers[A] = stack[stk_ptr];
            stk_ptr = stk_ptr - 1;

            registers[B] = stack[stk_ptr];
            
            registers[C] = registers[B] / registers[A];

            stack[stk_ptr] = registers[C];
            printf("\t%d / %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case SUB: {
            registers[A] = stack[stk_ptr];
            stk_ptr = stk_ptr - 1;

            registers[B] = stack[stk_ptr];
            
            registers[C] = registers[B] - registers[A];

            stack[stk_ptr] = registers[C];
            printf("\t%d - %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case XOR: {
            registers[A] = stack[stk_ptr];
            stk_ptr = stk_ptr -1;

            registers[B] = stack[stk_ptr];

            registers[C] = registers[B] ^ registers[A];

            stack[stk_ptr] = registers[C];
            printf("\t%d XOR %d = %d\n",registers[B], registers[A], registers[C]);
            break;
        }
        case OR: {
            registers[A] = stack[stk_ptr];
            stk_ptr = stk_ptr -1;

            registers[B] = stack[stk_ptr];

            registers[C] = registers[B] | registers[A];

            stack[stk_ptr] = registers[C];
            printf("\t%d OR %d = %d\n",registers[B], registers[A], registers[C]);
            break;
        }
        case AND: {
            registers[A] = stack[stk_ptr];
            stk_ptr = stk_ptr -1;

            registers[B] = stack[stk_ptr];

            registers[C] = registers[B] & registers[A];

            stack[stk_ptr] = registers[C];
            printf("\t[->]%d AND %d = %d\n",registers[B], registers[A], registers[C]);
            break;
        }
        case SLT: {
            stk_ptr = stk_ptr - 1;
            if(debug==1)
                printf("[-] SLT pushing the output of %d < %d\n", stack[stk_ptr+1], stack[stk_ptr]);
            stack[stk_ptr] = stack[stk_ptr+1] < stack[stk_ptr];
            break;
        }
        case MOV: {
            if(debug==1)
                printf("[-] MOV moving the value in register (0x%x|%d) to register (0x%x|%d)\n",registers[instructions[int_ptr + 1]],registers[instructions[int_ptr + 1]],registers[instructions[int_ptr + 2]],registers[instructions[int_ptr + 2]]);
            registers[instructions[int_ptr + 2]] = registers[instructions[int_ptr + 1]];
            int_ptr = int_ptr + 2;
            break;
        }
        case SET: {
            if(debug==1)
                printf("[-] SET assigning value (0x%x|%d) to register (0x%x|%d)\n",instructions[int_ptr + 2],instructions[int_ptr + 2],registers[instructions[int_ptr + 1]],registers[instructions[int_ptr + 1]]);
            registers[instructions[int_ptr + 1]] = instructions[int_ptr + 2];
            
            int_ptr = int_ptr + 2;
            break;
        }
        case LOG: {
            printf("[LOG->] %d\n", registers[instructions[int_ptr + 1]]);
            int_ptr = int_ptr + 1;
            break;
        }
        case IF: {
            printf("[-] IF\n");
            if (registers[instructions[int_ptr + 1]] == instructions[int_ptr + 2]) {
                int_ptr = instructions[int_ptr + 3];
                is_jmp = true;
            }
            else{
                int_ptr = int_ptr + 3;
            }
            break;
        }
        case IFN: {
            if(debug==1)
                printf("[-] IFN\n");
            if (registers[instructions[int_ptr + 1]] != instructions[int_ptr + 2]) {
                int_ptr = instructions[int_ptr + 3];
                is_jmp = true;
            }
            else {
                int_ptr = int_ptr + 3;
                
            }

            break;
        }
        case GLD: {
            stk_ptr = stk_ptr + 1;
            int_ptr = int_ptr + 1;
            if (debug==1)
                printf("[-] GLD loading register (0x%x|%d) to the stack(0x%x|%d)\n",registers[instructions[int_ptr]],registers[instructions[int_ptr]],stack[stk_ptr],stack[stk_ptr]);
            stack[stk_ptr] = registers[instructions[int_ptr]];
            
            break;
        }
        case GPT: {
            if(debug==1)
                printf("[-] GPT pushing top of stack (0x%x|%d) to the given register (0x%x|%d)\n", stack[stk_ptr], stack[stk_ptr], registers[instructions[int_ptr + 1]], registers[instructions[int_ptr + 1]]);
            registers[instructions[int_ptr + 1]] = stack[stk_ptr];
            int_ptr = int_ptr + 1;
            
            break;
        }
        case NOP: {
            if(debug==1)
                printf("[-] NOP\n");
            break;
        }
        default: {
            printf("[!] ERROR: Unknown Instruction %d\n", instr);
            break;
        }
    }
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("[!] ERROR: no input files\n");
        return -1;
    }

    char *filename = argv[1];

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("[!] ERROR: could not read file `%s`\n", filename);
        return -1;
    }

    instructions = malloc(sizeof(*instructions) * max_inst);

    int num;
    int i = 0;
    while (fscanf(file, "%d", &num) > 0) {
        instructions[i] = num;
        i++;
        if (i >= max_inst) {
            max_inst *= 2;
            instructions = realloc(instructions, sizeof(*instructions) * max_inst);
        }
    }
    
    instruction_count = i;

    fclose(file);

    stk_ptr = -1;
    
    printf("Verbose? (Y/N) : ");
    char a;
    scanf("%s",&a);
    if (!( a != 'Y' && a != 'y')){
        printf("[!] VERBOSE MODE ACTIVE\n");
        debug = 1;
    }

    printf("[!] STARTING EXECUTION\n");

    while (running && int_ptr < instruction_count) {
        executor(FETCH);
        if(!is_jmp){
            int_ptr = int_ptr + 1;
        }
    }

    printf("[!] EXITING!\n");
    free(instructions);

    return 0;
}
