#TestID BEQ2
#Instruction BEQ
#Author NA6518
#ExpectedOutput 3
#Description Basic BEQ branch not taken


addi $3, $0, 3
addi $4,  $4, 2
beq  $3, $4, 2
addi $4, $4, 1
addi $5, $5, 2
addi $2, $5, 1

jr $0
