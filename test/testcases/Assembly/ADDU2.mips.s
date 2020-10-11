#TestID ADDU2
#Instruction ADDU
#Author NA6518
#ExpectedOutput 0
#Description Basic ADDIU


lui $3, 0x8000
lui $4, 0X8000

addu $2, $3, $4
jr $0
