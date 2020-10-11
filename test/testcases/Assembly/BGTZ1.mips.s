#TestID BGTZ1
#Instruction BGTZ
#Author NA6518
#ExpectedOutput 3
#Description Basic BGTZ not taken


addi $3, $0, 0
addi $4,  $0, 3
bgtz  $3, 2
addi $4, $4, 1
addi $5, $0, 2
addi $2, $5, 1

jr $0
