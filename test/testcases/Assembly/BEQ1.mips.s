#TestID BEQ1
#Instruction BEQ
#Author NA6518
#ExpectedOutput 5
#Description Basic BEQ test delay


addi $3, $0, 3
addi $4,  $0, 3
beq $3, $4, 2
addi $4, $4, 1
addi $5, $5, 2
addi $2, $4, 1

jr $0
