#TestID BGEZ
#Instruction BGEZ
#Author NA6518
#ExpectedOutput 1
#Description Basic BGEZ


addi $3, $0, 4
addi $4,  $4, 3
bgez  $3, 2
addi $4, $4, 1
addi $5, $5, 2
addi $2, $5, 1

jr $0
