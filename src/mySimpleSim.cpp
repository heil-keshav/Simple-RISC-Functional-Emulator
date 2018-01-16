
/*

The project is developed as part of Computer Architecture class
Project Name: Functional/Pipeline Simulator for simpleRISC Processor

Developer's Name:
Developer's Email id:
Date:

*/

/* mySimpleSim.cpp
   Purpose of this file: implementation file for mySimpleSim
*/

#include "mySimpleSim.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>

using namespace std;
//Register file
static unsigned int R[16];
static unsigned int PC=0;
//flags
static bool gt,eq;
//control signals
static bool isSt,isLd,isBeq,isBgt,isRet,isImmediate,isWb,isUBranch,isCall,isAdd,isSub,isCmp,isMul,isDiv,isMod,isLsl,isLsr,isAsr,isOr,isAnd,isNot,isMov,isBranchTaken; 
//stack pointer
static unsigned int sp=14,no_of_inst=0;
//memory
static unsigned char MEM[4000];

//intermediate datapath and control path signals
static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;
unsigned int immx,rs1,rs2,rd,ra,opcode,I,aluResult,A,B,ldResult,Wb_result,modifi,imm,registr;
int branchTarget,branchPC;

void run_simplesim() {
    while(1) {
    fetch();
      if(instruction_word==0xff){
        cout<<"Process Terminated...."<<endl;
        break;}
    decode();     
    execute();
    mem();
    write_back();
  }
}

// it is used to set the reset values
//reset all registers and memory content to 0
void reset_proc() {
    int i;
    for(i=0;i<16;i++){
    R[i]=0;}
    PC=-4;
    gt=0,eq=0;
    isSt=0,isLd=0,isBeq=0,isBgt=0,isRet=0,isImmediate=0,isWb=0,isUBranch=0,isCall=0,isAdd=0,isSub=0,isCmp=0,isMul=0,isDiv=0,isMod=0,isLsl=0,isLsr=0,isAsr=0,isOr=0,isAnd=0,isNot=0,isMov=0;
    isBranchTaken=0;
    branchPC=0;
    branchTarget=0;
    R[14]=(unsigned long int )(2000);
    for(i=0;i<4000;i=i+4){
    write_word(MEM,i,0xff);
    }
}


//reads from the instruction memory and updates the instruction register
void fetch() {

    if(isBranchTaken){
    PC=branchPC;
    }
    else{
        PC=PC+4;
    }
    instruction_word=read_word(MEM,PC);
    cout<<"PC: "<<PC<<endl;
    // PC=PC+4;
     
}
//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void decode() {
    opcode=((instruction_word>>27)%(1<<5));
        cout<<"opcode: "<<opcode;
    imm=(0x0000FFFF & instruction_word);
        cout<<" inst w = "<<hex<<instruction_word<<endl;
    modifi=(instruction_word>>16)%(1<<2);
    if(modifi==0){
    if((imm>>15)%2)
        immx=(signed)imm;
    else
        immx=imm;
    }
    else if(modifi==1)
        immx=imm;
    else if(modifi==2)
        immx=(imm<<16);
   
     

    branchTarget=(instruction_word & 0x07FFFFFF)<<2;
    if((branchTarget>>26)%2)
        branchTarget=((signed)branchTarget<<3)>>3;
    branchTarget=branchTarget+(PC);
    //cout<<"branchTarget "<<branchTarget<<endl;
    rs1=(instruction_word>>18)%(1<<4);
    //cout<<"rs1 "<<rs1<<endl;
    rs2=(instruction_word>>14)%(1<<4);
    //cout<<"rs2 "<<rs2<<endl;
        rd=(instruction_word>>22)%(1<<4);
        //cout<<"rd "<<rd<<endl;
        ra=15;

   
    I=(instruction_word>>26)%(1<<1);
   // cout<<"imm bit "<<I<<endl;
    
    //control unit signals
    isSt=(opcode==15)?1:0;
    isLd=(opcode==14)?1:0;
    isBeq=(opcode==16)?1:0;
    isBgt=(opcode==17)?1:0;
    isRet=(opcode==20)?1:0;
    isImmediate=(I==1)?1:0;
    isWb=(opcode==5 || opcode==13|| opcode==15||opcode==16||opcode==17||opcode==18||opcode==20)?0:1;
    isUBranch=(opcode==18||opcode==19||opcode==20)?1:0;
    isCall=(opcode==19)?1:0;
    isAdd=(opcode==0 || opcode==14 || opcode==15)?1:0;
    isSub=(opcode==1)?1:0;
    isCmp=(opcode==5)?1:0;
    isMul=(opcode==2)?1:0;
    isDiv=(opcode==3)?1:0;
    isMod=(opcode==4)?1:0;
    isLsl=(opcode==10)?1:0;
    isLsr=(opcode==11)?1:0;
    isAsr=(opcode==12)?1:0;
    isOr=(opcode==7)?1:0;
    isAnd=(opcode==6)?1:0;
    isNot=(opcode==8)?1:0;
    isMov=(opcode==9)?1:0;

    //branch target unit
    if(isRet)
        operand1=R[ra];
    else if(!isRet)
        operand1=R[rs1];

    if(isSt)
        operand2=R[rd];
    else if(!isSt)
        operand2=R[rs2];
   // cout<<"operand2 "<<operand2<<endl;


}

//executes the ALU operation based on ALUop
void execute() {
    A=operand1;
    if(isImmediate)
    B=immx;
    else
    B=operand2;


//cout<<"B "<<B<<endl;

    if(isAdd){
    aluResult=A+B;
    }   
    if(isSub){
    aluResult=A-B;
    }
    if(isCmp){
    if(A>B){
        gt=1;
        eq=0;
        
    }
    else if(A==B){
        eq=1;
        gt=0;
    }
    else{
        gt=eq=0;
    }
    }
    if(isMul){
    aluResult=A*B;
    }
    if(isDiv){
    aluResult=A/B;
    }
    if(isMod){
    aluResult=A%B;
    }
    if(isLsl){
    aluResult=(A<<B);
    }
    if(isLsr){
    aluResult=(A>>B);
    }
    if(isAsr){
    aluResult=(signed)A>>B;
    }

    if(isOr){
    aluResult=(A | B);
    }
    if(isAnd){
    aluResult=(A & B);
    }
    if(isNot){
    aluResult=-B;
    }
    if(isMov){
    aluResult=B;
    }
    //cout<<"aluResult "<<aluResult<<endl;
    //cout<<"bgt: "<<gt<<endl;
    //cout<<"beq :"<<eq<<endl;

    //branch unit
    if(isRet)
    branchPC=operand1;
    else
    branchPC=branchTarget;

    isBranchTaken=( isUBranch || (isBeq && eq) || (isBgt && gt));

}

//perform the memory operation
void mem() {
    if(isLd){
    ldResult=read_word(MEM,aluResult+no_of_inst*4+4);
    }
    if(isSt){
    write_word(MEM,aluResult+no_of_inst*4+4,operand2);
    }
    


}
//writes the results back to register file
void write_back(){
    
    
        if(!isLd && !isCall){
        Wb_result=aluResult;
        }
        else if(isLd && !isCall){
        Wb_result=ldResult;
        }
        else if(isCall && !isLd){
        Wb_result=PC+4;
        }
        else{
        cout<<"pqpqp\n";
        }
        //cout<<"Wb_result : "<<Wb_result<<endl;
        if(isCall){
        registr=15;
        }
        else{
        registr=rd;
        }

if(isWb){
    R[registr]=Wb_result;

}
}

void load_program_memory(char *file_name) {
  FILE *fp;
  unsigned int address, instruction;
  fp = fopen(file_name, "r");
  if(fp == NULL) {
    printf("Error opening input mem file\n");
    exit(1);
  }
  while(fscanf(fp, "%x %x", &address, &instruction) != EOF) {
    write_word(MEM, address, instruction);
    no_of_inst++;
  }
  fclose(fp);
}

//writes the data memory in "data_out.mem" file
void write_data_memory() {
  FILE *fp;
  unsigned int i;
  fp = fopen("data_out.mem", "w");
  if(fp == NULL) {
    printf("Error opening dataout.mem file for writing\n");
    return;
  }

  for(i=0; i < 4000; i = i+4){
    fprintf(fp, "%x %x\n", i, read_word(MEM, i));
  }
   for(i=0; i<16;i++){
    cout<<"R"<<i<<" "<<R[i]<<endl;  
    }
  fclose(fp);
}



unsigned int read_word(unsigned char *mem, unsigned int address) {
    unsigned int *data;
    data =  (unsigned int*) (mem + address);
    return *data;
}

void write_word(unsigned char *mem, unsigned int address, unsigned int data) {
    unsigned int *data_p;
    data_p = (unsigned int*) (mem + address);
    *data_p = data;
}

