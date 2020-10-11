#TestID ADDU
#Instruction ADDU
#Author NA6518
#ExpectedOutput 2
#Description Basic ADDIU


addi $3, $0, 1
addi $4, $0, 1

addu $2, $3, $4
jr $0
