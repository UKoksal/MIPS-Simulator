#include <iostream>
#include <string>
#include <fstream>
#include "cpu.hpp"


int main(int argc, char *argv[]){
	std::string binName = argv[1];
	std::cerr<<binName<<std::endl;
	//std::ifstream binStream(argv[1], std::ios::binary);
	//binStream.seekg(0, binStream.end);
	//int lengthBin = binStream.tellg();
	cpu mips(binName);
	//mips.set_register();
	//std::cerr<<1;
	mips.execute();
	//std::cerr<<2;
	//mips.get_register();
	std::cerr<<0<<std::endl;





	return 0;
}
