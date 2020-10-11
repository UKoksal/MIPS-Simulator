#include "cpu.hpp"

bool overflow(uint32_t op1, uint32_t op2);
uint32_t select_byte(const uint32_t &word, const int &address );
bool valid_read(const uint32_t &address);
int insordata(const uint32_t &address);
bool is_dmem_address(const uint32_t &address);
int32_t sign_extend_imm(const uint32_t &immediate);


cpu::cpu(const std::string &bin){
  imem.resize(IMEM_LENGTH/4);
  dmem.resize(DMEM_LENGTH/4, 0);
  std::ifstream binStream(bin, std::ios::in|std::ios::binary);
  if(binStream.is_open()){
	   binStream.seekg(0, binStream.end);
	    int lengthBin = binStream.tellg();
      char* memblock = new char [lengthBin];
      binStream.seekg(0, binStream.beg);
      binStream.read(memblock, lengthBin);
      std::cerr<<"length of binary "<<lengthBin<<std::endl;
      std::string ins = "";
      int index = 0;
      //std::cerr<<"memblock read"<<memblock[0]<<std::endl;
      /*for(int i=0; i<lengthBin; i++){ //should read fstream bin as groups of 32 bits representing the

        //ins.append(1u, memblock[i]);
        ins+=memblock[i];
        //std::cerr<<ins<<std::endl;
        //std::cerr<<(std::bitset<8>)memblock[i]<<std::endl;
        //index++;
        if(ins.length()==32){
          //std::cerr<<"inslength"<<ins.length()<<std::endl;
          uint32_t value = strtoul(ins.c_str(), NULL, 2);
          imem[index] = value;
          ins = "";
          index++;
        }

      }*/
      //std::cerr<<"index "<<index<<std::endl;
      //std::bitset<8>memblock;
      /*for(int i = lengthBin/4; i>=0; i--){
        std::bitset<8>bits = (std::bitset<8>)memblock;
        imem[index] = bits>>(i*32);
        index++;
      }*/
      uint32_t word = 0x0;
      for (int i=0; i<lengthBin; i++){
        //std::cout << std::bitset<8>(memblock[i]) << "\t" << std::endl;
        std::bitset<8> b(memblock[i]);
        uint32_t set = b.to_ulong();
        word = word | (set << (3-i%4)*8);
//      std::cout << std::bitset<32>(word) << std::endl;
        if ((i+1)%4 == 0){
          //imem.push_back(word);
          imem[index] = word;
          //std::cerr<<"errins"<<imem[index]<<std::endl;
          //std::cout << std::bitset<32>(word) << std::endl << std::endl ;
          word = 0x0;
          index++;
        }

      }

      final_valid_ins = index;
      //for(int i=0; i<index; i++){
        //std::cerr<<imem[i]<<std::endl;
      //}
      //for(int i=0; i<imem.size(); i++){
        //std::cout<<imem[i]<<std::endl;
      //}
      //std::cerr<<"first "<<imem[0]<<std::endl;
      //std::cerr<<"second "<<imem[1]<<std::endl;
      //std::cerr<<"memblock"<<memblock<<std::endl;

      binStream.seekg(0, binStream.beg);
      binStream.close();
    }
  //for(int i=0; i<imem.size(); i++){
  //  std::cerr<<imem[i];
  //}
  //std::cerr<<std::endl;
  //std::cerr<<"size "<<imem.size()<<std::endl;
  /*for(int i=0; i<DMEM_LENGTH/4; i++){
    dmem[i] = 0;
  }*/
  for(int i=0; i<32; i++){
    registers.push_back(0);
  }
  lo_register = 0;
  hi_register = 0;
  pc = 1; //this probably should be changed to IMEM_OFFSET instead
  pc_n = 2;


}

void cpu::execute(){
  std::cerr<<"executing"<<std::endl;
  instruction i1;
  while((pc<imem.size())&&(pc<=final_valid_ins)&&(pc!=0)){
    std::cerr<<"effective pc "<<pc-1<<std::endl;
    extract_common(imem[pc-1], i1);
    std::cerr<<"instruction "<<imem[pc-1]<<std::endl;
    //std::cerr<<"err1"<<std::endl;
    execute_common(i1);
  }
  std::cerr<<"complete"<<std::endl;
  std::cerr<<(registers[2]&0xff)<<std::endl;
  std::exit(registers[2]&0xff);
}

void cpu::execute_common(const instruction &i1){
  if(i1.opcode == 0){
    execute_r(i1);
  }
  else if((i1.opcode==2)||(i1.opcode==3)||(i1.opcode==26)){
    execute_j(i1);
  }
  else{
    execute_i(i1);
  }
}

void cpu::extract_common(const uint32_t &ins, instruction &i1){
  uint32_t opcode = (ins>>26) & 0x3f;
  if(opcode == 0){
    extract_r(ins, i1);
  }
  else if((opcode==2)||(opcode==3)||(opcode==26)){
    extract_j(ins, i1);
  }
  else{
    extract_i(ins, i1);
  }
}

void cpu::extract_r(const uint32_t &ins, instruction &i1){
  i1.opcode = (ins>>26) & 0x3f;
  i1.rs = (ins>>21) & 0x1f;
  i1.rt = (ins>>16) & 0x1f;
  i1.rd = (ins>>11) & 0x1f;
  i1.shamt = (ins>>6) & 0x1f;
  i1.funct = ins & 0x3f;
  i1.immediate = 0;
  i1.address = 0;
}

void cpu::extract_i(const uint32_t &ins, instruction &i1){
  i1.opcode = (ins>>26) & 0x3f;
  i1.rs = (ins>>21) & 0x1f;
  i1.rt = (ins>>16) & 0x1f;
  i1.immediate = ins & 0xffff;
  i1.rd = 0;
  i1.shamt = 0;
  i1.funct = 0;
  i1.address = 0;
}

void cpu::extract_j(const uint32_t &ins, instruction &i1){
  i1.opcode = (ins>>26) & 0x3f;
  i1.address = ins & 0x3ffffff;
  i1.rs = 0;
  i1.rt = 0;
  i1.immediate = 0;
  i1.rd = 0;
  i1.shamt = 0;
  i1.funct = 0;
}

void cpu::set_register(){ //hard coded to test functionality
  registers[2] = 0xf0000000;
  registers[3] = 0x0f000000;
}

void cpu::get_register(){ //hard coded to test functionality
  std::cout<<registers[1]<<std::endl;
  std::cout<<"hi "<<hi_register<<std::endl;
  std::cout<<"lo "<<lo_register<<std::endl;
}

bool overflow(uint32_t op1, uint32_t op2){
  int32_t operand1 = op1;
  int32_t operand2 = op2;
  int32_t tempsum = operand1+operand2;
  if((operand1>0)&&(operand2>0)&&(tempsum<0)){
    std::cerr<<"overflow"<<std::endl;
    return true; //arithmetic exception
  }
  else if ((operand1<0)&&(operand2<0)&&(tempsum>0)){
    std::cerr<<"overflow"<<std::endl;
    return true; //arithmetic exception
  }
  else{
    std::cerr<<"no overflow"<<std::endl;
    return false;
  }
}
uint32_t select_byte(const uint32_t &word, const int &address ){
  if((address%4)==0){
    return (word&0xff000000)>>24;
  }
  else if((address%4)==1){
    return (word&0x00ff0000)>>16;
  }
  else if((address%4)==2){
    return (word&0x0000ff00)>>8;
  }
  else{
    return word&0x000000ff;
  }
}

bool valid_read(const uint32_t &address){
  if((address>=0x10000000)&&(address<0x11000000)){
    return true;
  }
  else if ((address>=0x20000000)&&(address<0x24000000)){
    return true;
  }
  else if ((address>=0x30000000)&&(address<0x30000004)){
    return true;
  }
  else{
    return false;
  }
}

int insordata(const uint32_t &address){
  if((address>=0x10000000)&&(address<0x11000000)){
    return 0; //instruction
  }
  else if ((address>=0x20000000)&&(address<0x24000000)){
    return 1; //data
  }
  else if ((address>=0x30000000)&&(address<0x30000004)){
    return 2; //ADDR_GETC
  }
  else{
    return 3;
  }
}

bool is_dmem_address(const uint32_t &address){
  if ((address>=0x20000000)&&(address<0x24000000)){
    return true; //data
  }
  else{
    return false;
  }
}

int32_t sign_extend_imm(const uint32_t &immediate){
  if((immediate&0x8000)!=0){
    return immediate|0xffff0000;
  }
  else{
    return immediate;
  }
}

void cpu::execute_r(const instruction &i1){
  if(i1.funct == 0b100001){ //ADDU
    uint32_t op1 = registers[i1.rs];
    uint32_t op2 = registers[i1.rt];
    registers[i1.rd] = op1+op2;
    //std::cerr<<"destination register"<<i1.rt<<std::endl;
    pc=pc_n;
    pc_n++;

  }
  else if(i1.funct == 0b001000){  //JR
    //pc = registers[(i1.rs-IMEM_OFFSET)];
    pc = pc_n;
    pc_n = registers[(i1.rs)];
    std::cerr<<"jr "<<i1.rs<<" "<<registers[i1.rs]<<std::endl;
    std::cerr<<"pc "<<pc<<"pc_n "<<pc_n<<std::endl;
  }
  else if (i1.funct == 0b100000){ //ADD
    uint32_t op1 = registers[i1.rs];
    uint32_t op2 = registers[i1.rt];
    int32_t tempsum = op1+op2;
    if(overflow(op1,op2)){
      std::exit(-10); //arithmetic exception
    }
    else{
      registers[i1.rd] = tempsum;

      pc=pc_n;
      pc_n++;
    }
  }
  else if(i1.funct == 0b100100){  //AND
    registers[i1.rd] = registers[i1.rs] & registers[i1.rt];
    pc = pc_n;
    pc_n++;
  }
  else if(i1.funct == 0b011010){  //DIV
    int32_t op1 = registers[i1.rs];
    int32_t op2 = registers[i1.rt];
    if(registers[i1.rt]==0){
      pc = pc_n;
      pc_n++;
    }
    else{
      lo_register = op1/op2;
      hi_register = op1%op2;
      pc = pc_n;
      pc_n++;
    }
  }
  else if(i1.funct == 0b011011){  //DIVU
    if(registers[i1.rt]==0){
      //std::exit(-10);
      pc = pc_n;
      pc_n++;
    }
    else{
      unsigned int op1 = registers[i1.rs];
      unsigned int op2 = registers[i1.rt];
      lo_register = op1/op2;
      hi_register = op1%op2;
      pc = pc_n;
      pc_n++;
    }
  }
  else if(i1.funct == 0b001001){  //JALR

      registers[i1.rd] = pc+2;
      pc = pc_n;
      int32_t op1 = registers[i1.rs];
      pc_n = (op1 - IMEM_OFFSET)/4;
      //std::cerr<<"next ins loc "<<pc_n<<std::endl;

  }
  else if(i1.funct == 0b010000){  //MFHI
      registers[i1.rd] = hi_register;
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b010010){  //MFLO
      registers[i1.rd] = lo_register;
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b010001){  //MTHI
      hi_register = registers[i1.rs];
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b010011){  //MTLO
      lo_register = registers[i1.rs];
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b011000){  //MULT
      int32_t op1 = registers[i1.rs];
      int32_t op2 = registers[i1.rt];
      int64_t mul = op1*op2;
      lo_register = mul&0x00000000ffffffff;
      hi_register = (mul>>32);
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b011001){  //MULTU
      uint64_t op1 = registers[i1.rs];
      uint64_t op2 = registers[i1.rt];
      uint64_t mul = op1*op2;
      lo_register = mul&0x00000000ffffffff;
      hi_register = (mul>>32);
      std::cerr<<"hi "<<hi_register<<" lo "<<lo_register<<std::endl;
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b100101){  //OR
      registers[i1.rd] = registers[i1.rs]|registers[i1.rt];
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b000000){  //SLL
      registers[i1.rd] = (registers[i1.rt]<<i1.shamt);
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b000100){  //SLLV
      registers[i1.rd] = (registers[i1.rt]<<registers[i1.rs]);
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b101010){  //SLT
      int32_t op1 = registers[i1.rs];
      int32_t op2 = registers[i1.rt];
      if(op1<op2){
        registers[i1.rd] = 1;
      }
      else{
        registers[i1.rd] = 0;
      }
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b101011){  //SLTU
      uint32_t op1 = registers[i1.rs];
      uint32_t op2 = registers[i1.rt];
      if(op1<op2){
        registers[i1.rd] = 1;
      }
      else{
        registers[i1.rd] = 0;
      }
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b000011){  //SRA
      int32_t op1 = registers[i1.rt];
      int32_t op2 = i1.shamt;
      int32_t shifted = op1>>op2;
      registers[i1.rd] = shifted;
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b000111){  //SRAV
      int32_t op1 = registers[i1.rt];
      int32_t op2 = registers[i1.rs];
      int32_t shifted = op1>>op2;
      std::cerr<<"shifted "<<shifted<<std::endl;
      registers[i1.rd] = shifted;
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b000010){  //SRL
      registers[i1.rd] = registers[i1.rt]>>i1.shamt;
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b000110){  //SRLV
      registers[i1.rd] = registers[i1.rt]>>registers[i1.rs];
      pc = pc_n;
      pc_n++;
  }
  else if(i1.funct == 0b100010){  //SUB
    uint32_t op1 = registers[i1.rs];
    uint32_t op2 = 0-registers[i1.rt];
    int32_t tempsum = op1+op2;
    if(overflow(op1,op2)){
      std::exit(-10); //arithmetic exception
    }
    else{
      registers[i1.rd] = tempsum;
      pc=pc_n;
      pc_n++;
    }
  }
  else if(i1.funct == 0b100011){  //SUBU
    registers[i1.rd] = registers[i1.rs] - registers[i1.rt];
    pc = pc_n;
    pc_n++;
  }
  else if(i1.funct == 0b100110){  //XOR
    registers[i1.rd] = registers[i1.rs]^registers[i1.rt];
    pc = pc_n;
    pc_n++;
  }
  else{
    std::exit(-12);
  }


}

void cpu::execute_i(const instruction &i1){
  if(i1.opcode == 0b001000){ //ADDI
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    int32_t tempsum = op1+op2;
    if((op2&0x8000)!=0){
      op2 = op2|0xffff0000;
    }
    if(overflow(op1, op2)){
      std::exit(-10);
    }
    else{
      registers[i1.rt] = tempsum;
      std::cerr<<"ADDI "<<op1<<" + "<<op2<<" = "<<registers[i1.rt]<<std::endl;
      std::cerr<<"addi "<<i1.rs<<" "<<i1.immediate<< " "<<i1.rt<<std::endl;
      pc = pc_n;
      pc_n++;
    }
  }
  else if(i1.opcode == 0b001001){  //ADDIU
    registers[i1.rt] = registers[i1.rs] + i1.immediate;
    std::cerr<<"rt "<<registers[i1.rt]<<std::endl;
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b001100){  //ANDI
    registers[i1.rt] = registers[i1.rs] & i1.immediate;
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b000100){ //BEQ
    std::cerr<<"beq"<<std::endl;
    int32_t effimm = sign_extend_imm(i1.immediate);
    if(registers[i1.rs]==registers[i1.rt]){
      int temppc = pc_n;
      int32_t errimm = sign_extend_imm(i1.immediate);
      if((errimm+temppc)<=0){
        std::exit(-11);
      }
      pc = pc_n;
      pc_n+=effimm;
    }
    else{
      pc = pc_n;
      pc_n++;
    }
  }
  else if((i1.opcode == 0b000001)&&(i1.rt==0b00001)){ //BGEZ
    int32_t comp = registers[i1.rs];
    if(comp>=0){
      int temppc = pc_n;
      int32_t errimm = sign_extend_imm(i1.immediate);
      if((errimm+temppc)<=0){
        std::exit(-11);
      }

      int32_t effimm = sign_extend_imm(i1.immediate);
      pc = pc_n;
      pc_n+=effimm;
    }
    else{
      pc = pc_n;
      pc_n++;
    }
  }
  else if((i1.opcode == 0b000001)&&(i1.rt==0b10001)){ //BGEZAL
    int32_t comp = registers[i1.rs];
    if(comp>=0){
      int temppc = pc_n;
      int32_t errimm = sign_extend_imm(i1.immediate);
      if((errimm+temppc)<=0){
        std::exit(-11);
      }
      int32_t effimm = sign_extend_imm(i1.immediate);
      registers[31] = pc + 2;
      pc = pc_n;
      pc_n+=effimm;
    }
    else{
      pc = pc_n;
      pc_n++;
    }
  }
  else if(i1.opcode == 0b000111){ //BGTZ
    int32_t comp = registers[i1.rs];
    if(comp>0){
      int temppc = pc_n;
      int32_t errimm = sign_extend_imm(i1.immediate);
      if((errimm+temppc)<=0){
        std::exit(-11);
      }

      int32_t effimm = sign_extend_imm(i1.immediate);
      pc = pc_n;
      pc_n+=effimm;
      //std::cerr<<"pc "<<pc<<" pc_n "<<pc_n<<std::endl;
    }
    else{
      pc = pc_n;
      pc_n++;
    }
  }
  else if(i1.opcode == 0b000110){ //BLEZ
    int32_t comp = registers[i1.rs];
    if(comp<=0){
      int temppc = pc_n;
      int32_t errimm = sign_extend_imm(i1.immediate);
      if((errimm+temppc)<=0){
        std::exit(-11);
      }

      int32_t effimm = sign_extend_imm(i1.immediate);
      pc = pc_n;
      pc_n+=effimm;
    }
    else{
      pc = pc_n;
      pc_n++;
    }
  }
  else if((i1.opcode == 0b000001)&&(i1.rt==0b00000)){ //BLTZ
    int32_t comp = registers[i1.rs];
    if(comp<0){
      int temppc = pc_n;
      int32_t errimm = sign_extend_imm(i1.immediate);
      if((errimm+temppc)<=0){
        std::exit(-11);
      }

      int32_t effimm = sign_extend_imm(i1.immediate);
      pc = pc_n;
      pc_n+=effimm;
    }
    else{
      pc = pc_n;
      pc_n++;
    }
  }
  else if((i1.opcode == 0b000001)&&(i1.rt==0b10000)){ //BLTZAL
    int32_t comp = registers[i1.rs];
    if(comp<0){
      int temppc = pc_n;
      int32_t errimm = sign_extend_imm(i1.immediate);
      if((errimm+temppc)<=0){
        std::exit(-11);
      }

      int32_t effimm = sign_extend_imm(i1.immediate);
      registers[31] = pc + 2;
      pc = pc_n;
      pc_n+=effimm;
    }
    else{
      pc = pc_n;
      pc_n++;
    }
  }
  else if(i1.opcode == 0b000101){ //BNE
    int temppc = pc_n;
    int32_t errimm = sign_extend_imm(i1.immediate);
    if((errimm+temppc)<=0){
      std::exit(-11);
    }

    if(registers[i1.rs]!=registers[i1.rt]){
      int temppc = pc_n;
      int32_t errimm = sign_extend_imm(i1.immediate);
      if((errimm+temppc)<=0){
        std::exit(-11);
      }
      pc = pc_n;
      uint32_t effimm = i1.immediate;
      if((i1.immediate&0x8000)!=0){
        effimm= i1.immediate|0xffff0000;
      }
      pc_n+=effimm;
    }
    else{
      pc = pc_n;
      pc_n++;
    }
  }
  else if(i1.opcode == 0b100000){ //LB - sign extension
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    int address = op1 +op2;
    //std::cerr<<"lb "<<op1<<" "<<op2<<" "<<address<<std::endl;
    if(!valid_read(address)){
      std::exit(-11);
    }
    uint32_t word;
    if(insordata(address)==0){
      address-= IMEM_OFFSET;
      word = imem[address/4];
    }
    else if (insordata(address)==1){
      address-= DMEM_OFFSET;
      word = dmem[address/4];
    }
    else if(insordata(address)==2){
      //address -=ADDR_GETC;
      word = std::getchar();
    }
    //uint32_t word = dmem[address/4];
    uint32_t byte = select_byte(word, address);
    if((byte&0x80)==0){
      registers[i1.rt] = byte;
    }
    else{
      registers[i1.rt] = byte|0xffffff00;
    }
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b100100){ //LBU - zero extension
    int address = registers[i1.rs] + i1.immediate;
    if(!valid_read(address)){
      std::exit(-11);
    }
    uint32_t word;
    if(insordata(address)==0){
      address-= IMEM_OFFSET;
      word = imem[address/4];
    }
    else if (insordata(address)==1){
      address-= DMEM_OFFSET;
      word = dmem[address/4];
    }
    else if(insordata(address)==2){
      //address -=ADDR_GETC;
      word = std::getchar();
    }
    //uint32_t word = dmem[address/4];
    uint32_t byte = select_byte(word, address);
    registers[i1.rt] = byte;
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b100001){ //LH
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    int address = op1 +op2;
    //int address = registers[i1.rs] + i1.immediate;

    if(address%2!=0){
      std::exit(-11); //memory exception
    }
    if(!valid_read(address)){
      std::exit(-11);
    }
    uint32_t word;
    if(insordata(address)==0){
      address-= IMEM_OFFSET;
      word = imem[address/4];
    }
    else if (insordata(address)==1){
      address-= DMEM_OFFSET;
      word = dmem[address/4];
    }
    else if(insordata(address)==2){
      //address -=ADDR_GETC;
      word = std::getchar();
    }
    //uint32_t word = dmem[address/4];
    uint32_t halfword;
    if(address%4==0){
      halfword = (word&0xffff0000)>>16;
    }
    else if(address%4==2){
      halfword = word&0x0000ffff;
    }
    else{
      std::exit(-11);
    }
    if((halfword&0x8000)==0){
      registers[i1.rt] = halfword;
    }
    else{
      registers[i1.rt] = halfword|0xffff0000;
    }
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b100101){ //LHU
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    int address = op1 +op2;
    //int address = registers[i1.rs] + i1.immediate;
    if(address%2!=0){
      std::exit(-11); //memory exception
    }
    if(!valid_read(address)){
      std::exit(-11);
    }
    uint32_t word;
    if(insordata(address)==0){
      address-= IMEM_OFFSET;
      word = imem[address/4];
    }
    else if (insordata(address)==1){
      address-= DMEM_OFFSET;
      word = dmem[address/4];
    }
    else if(insordata(address)==2){
      //address -=ADDR_GETC;
      word = std::getchar();
    }
    uint32_t halfword;
    if(address%4==0){
      halfword = (word&0xffff0000)>>16;
    }
    else if(address%4==2){
      halfword = word&0x0000ffff;
    }
    registers[i1.rt] = halfword;
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b001111){ //LUI
    int32_t ext = sign_extend_imm(i1.immediate);
    registers[i1.rt] = (ext<<16);
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b100011){ //LW
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    int address = op1 +op2;
    //int address = registers[i1.rs] + i1.immediate;
    //std::cerr<<"addr "<<address<<std::endl;
    if(address%4!=0){
      std::exit(-11);
    }
    if(!valid_read(address)){
      std::cerr<<"mem err"<<std::endl;
      std::exit(-11);
    }
    uint32_t word;

    if(insordata(address)==0){
      address-= IMEM_OFFSET;
      word = imem[address/4];
    }
    else if (insordata(address)==1){
      address-= DMEM_OFFSET;
      word = dmem[address/4];
    }
    else if(address==ADDR_GETC){
      word = std::getchar();
    }
    registers[i1.rt] = word;
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b100010){ //LWL
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    int address = op1 +op2;
    //int address = registers[i1.rs]+i1.immediate;
    //uint32_t word = dmem[address/4];
    if(!valid_read(address)){
      std::exit(-11);
    }
    uint32_t word;

    if(insordata(address)==0){
      address-= IMEM_OFFSET;
      word = imem[address/4];
    }
    else if (insordata(address)==1){
      address-= DMEM_OFFSET;
      word = dmem[address/4];
    }
    else if(insordata(address)==2){
      //address -=ADDR_GETC;
      word = std::getchar();
    }
    if(address%4==0){
      registers[i1.rt] = word;
    }
    else if(address%4==1){
      registers[i1.rt] = ((word&0x00ffffff)<<4)|(registers[i1.rt]&0x000000ff);
    }
    else if(address%4==2){
      registers[i1.rt] = ((word&0x0000ffff)<<16)|(registers[i1.rt]&0x0000ffff);
    }
    else{
      registers[i1.rt] = ((word&0x000000ff)<<28)|(registers[i1.rt]&0x00ffffff);
    }
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b100110){ //LWR
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    int address = op1 +op2;
    //int address = registers[i1.rs]+i1.immediate;
    if(!valid_read(address)){
      std::exit(-11);
    }
    uint32_t word;

    if(insordata(address)==0){
      address-= IMEM_OFFSET;
      word = imem[address/4];
    }
    else if (insordata(address)==1){
      address-= DMEM_OFFSET;
      word = dmem[address/4];
    }
    else if(address==ADDR_GETC){
      word = std::getchar();
    }
    if(address%4==3){
      registers[i1.rt] = word;
    }
    else if(address%4==2){
      registers[i1.rt] = ((word&0xffffff00)>>4)|(registers[i1.rt]&0xff000000);
    }
    else if(address%4==1){
      registers[i1.rt] = ((word&0xffff0000)>>16)|(registers[i1.rt]&0xffff0000);
    }
    else{
      registers[i1.rt] = ((word&0xff000000)>>28)|(registers[i1.rt]&0xffffff00);
    }
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b001101){ //ORI
    registers[i1.rt] = registers[i1.rs]|i1.immediate;
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b101000){ //SB
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    int address = op1 +op2;
    //int address = registers[i1.rs]+i1.immediate;
    if(is_dmem_address(address)){
      address-=DMEM_OFFSET;
      uint32_t loadingdata = registers[i1.rt]&0x000000ff;
      if(address%4==0){
        dmem[address/4] = (dmem[address/4]&0x00ffffff)|(loadingdata<<24);
      }
      else if(address%4==1){
        dmem[address/4] = (dmem[address/4]&0xff00ffff)|(loadingdata<<16);
      }
      else if(address%4==2){
        dmem[address/4] = (dmem[address/4]&0xffff00ff)|(loadingdata<<8);
      }
      else{
        dmem[address/4] = (dmem[address/4]&0xffffff00)|(loadingdata);
      }
    }
    else if (address==0x30000007){
      std::cerr<<"output "<<(registers[i1.rt]&0x000000ff)<<std::endl;
      std::putchar(registers[i1.rt]&0x000000ff);
    }
    else{
      std::exit(-11);
    }
    /*uint32_t loadingdata = registers[i1.rt]&0x000000ff;
    if(address%4==0){
      dmem[address/4] = (dmem[address/4]&0x00ffffff)|(loadingdata<<24);
    }
    else if(address%4==1){
      dmem[address/4] = (dmem[address/4]&0xff00ffff)|(loadingdata<<16);
    }
    else if(address%4==2){
      dmem[address/4] = (dmem[address/4]&0xffff00ff)|(loadingdata<<8);
    }
    else{
      dmem[address/4] = (dmem[address/4]&0xffffff00)|(loadingdata);
    }*/
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b101001){ //SH
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    int address = op1 +op2;
    //int address = registers[i1.rs]+i1.immediate;
    if(is_dmem_address(address)){
      address-=DMEM_OFFSET;
      uint32_t loadingdata = registers[i1.rt]&0x0000ffff;
      if(address%4==0){
        dmem[address/4] = (dmem[address/4]&0x0000ffff)|(loadingdata<<16);
      }
      else if(address%4==2){
        dmem[address/4] = (dmem[address/4]&0xffff0000)|(loadingdata);
      }
      else{
        std::exit(-11);
      }
    }
    else if (address==0x30000006){
      std::cerr<<"output "<<(registers[i1.rt]&0x0000ffff)<<std::endl;
      std::putchar(registers[i1.rt]&0x0000ffff);
    }
    else{
      std::exit(-11);
    }
    /*if(address%2!=0){
      std::exit(-11);
    }
    uint32_t loadingdata = registers[i1.rt]&0x0000ffff;
    if(address%4==0){
      dmem[address/4] = (dmem[address/4]&0x0000ffff)|(loadingdata<<16);
    }
    else if(address%4==2){
      dmem[address/4] = (dmem[address/4]&0xffff0000)|(loadingdata);
    }
    else{
      std::exit(-11);
    }*/
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b001010){ //SLTI
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    std::cerr<<"imm "<<op2<<std::endl;
    if(op1<op2){
      registers[i1.rt] = 1;
    }
    else{
      registers[i1.rt] = 0;
    }
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b001011){ //SLTIU
    uint32_t op1 = registers[i1.rs];
    uint32_t op2 = i1.immediate;
    if(op1<op2){
      registers[i1.rt] = 1;
    }
    else{
      registers[i1.rt] = 0;
    }
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b101011){ //SW
    int32_t op1 = registers[i1.rs];
    int32_t op2 = sign_extend_imm(i1.immediate);
    int address = op1 +op2;
    //int address = registers[i1.rs]+i1.immediate;
    if(is_dmem_address(address)){
      address-=DMEM_OFFSET;
      dmem[address/4] = registers[i1.rt];
      std::cerr<<"stored word "<<dmem[address/4]<<std::endl;

    }
    else if (address==0x30000004){
      std::cerr<<"output "<<registers[i1.rt]<<std::endl;
      std::putchar(registers[i1.rt]);
    }
    else{
      std::exit(-11);
    }
    if(address%4!=0){
      std::exit(-11);
    }
    pc = pc_n;
    pc_n++;
  }
  else if(i1.opcode == 0b001110){ //XORI
    registers[i1.rt] = registers[i1.rs]^i1.immediate;
    pc = pc_n;
    pc_n++;
  }
  else{
    std::exit(-12);
  }

}



void cpu::execute_j(const instruction &i1){
  if(i1.opcode == 0b000010){ //J
    pc=pc_n;
    pc_n = (pc&0x3c000000)|i1.address+1; //possible bug, should take IMEM_OFFSET into account

  }
  else if(i1.opcode == 0b000011){ //JAL
    registers[31] = pc + 2;
    pc=pc_n;
    pc_n = (pc&0x3c000000)|i1.address; //possible bug
  }
  else{
    std::exit(-12);
  }


}
