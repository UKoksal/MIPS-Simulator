#TestID ADDU1
#Instruction ADDU
#Author NA6518
#ExpectedOutput 9
#Description Basic ADDIU


addi $3, $0, 10
addi $4, $0, -1

addu $2, $3, $4
jr $0
