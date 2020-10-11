#TestID BGEZAL
#Instruction BGEZAL
#Author NA6518
#ExpectedOutput 2
#Description Basic BGEZAL


addi $3, $0, 4
addi $4,  $4, 3
bgezal  $3, addition
addi $4, $4, 1
addi $5, $5, 2
addi $2, $6, 1

jr $0

addition:
addi $6, $0, 1
jr $31
