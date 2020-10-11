#ifndef CPU_HPP
#define CPU_HPP


#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <istream>
#include <cmath>

#define IMEM_OFFSET 0x10000000
#define DMEM_OFFSET 0x20000000
#define IMEM_LENGTH 0x1000000
#define DMEM_LENGTH 0x4000000
#define ADDR_GETC   0x30000000


struct instruction{
  uint32_t opcode; //opcode
  uint32_t rs;    //source register
  uint32_t rt;    //second source register
  uint32_t rd;    //destination register
  uint32_t shamt; //shift
  uint32_t funct; //function
  uint32_t immediate; //immediate constant
  uint32_t address; //address
};

class cpu{
  private:
    std::vector<uint32_t> imem; //instruction memory
    uint32_t final_valid_ins; //to indicate where the final inst is

  public:
    std::vector<uint32_t> dmem; //data memory
    std::vector<uint32_t> registers; //registers
    uint32_t lo_register; //special low register
    uint32_t hi_register; //high register
    uint32_t pc;  //program counter
    uint32_t pc_n; //pc_next is introduced to handle delay slot
    cpu(const std::string &bin); //constructor
    void execute();   //execute

    void execute_common(const instruction &i1);    //common instruction before branching to r, i or j
    void execute_r(const instruction &i1);   //execute for r instructions
    void execute_i(const instruction &i1);   //execute for i instructions
    void execute_j(const instruction &i1);   //execute for j instructions

    void extract_common(const uint32_t &ins, instruction &i1);
    void extract_r(const uint32_t &ins, instruction &i1);
    void extract_i(const uint32_t &ins, instruction &i1);
    void extract_j(const uint32_t &ins, instruction &i1);

    void set_register();
    void get_register();
};



#endif
