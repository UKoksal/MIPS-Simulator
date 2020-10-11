#TestID ADDIU1
#Instruction ADDIU
#Author NA6518
#ExpectedOutput 0
#Description Test overflow


lui $3, 0xFFFF

addiu $2, $3, 0x6FFF0000
jr $0
