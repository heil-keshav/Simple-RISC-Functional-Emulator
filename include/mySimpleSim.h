#ifndef DATA
#define DATA

void run_simplesim();
void reset_proc();
// void load_program_memory();
void load_program_memory(char *file_name);
void write_data_memory();

//reads from the instruction memory and updates the instruction register
void fetch();
//reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage
void decode();
//executes the ALU operation based on ALUop
void execute();
//perform the memory operation
void mem();
//writes the results back to register file
void write_back();

unsigned int read_word(unsigned char *mem, unsigned int address);
void write_word(unsigned char *mem, unsigned int address, unsigned int data);

#endif
