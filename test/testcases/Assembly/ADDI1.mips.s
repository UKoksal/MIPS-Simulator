#TestID ADD1
#Instruction ADD 
#Author NA6518
#ExpectedOutput -10 
#Description ADDI with overflow 

lui $4, 0x7FFF 
ori $5, $4, 0xFFFF

addi $2, $5, 1 
jr $0 



