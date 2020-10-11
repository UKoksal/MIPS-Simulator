#TestID ADD1
#Instruction ADD 
#Author NA6518
#ExpectedOutput -10
#Description ADD with overflow 

lui $6, 0x7FFF
lui $4, 0x6FFF

add $5, $4, $6 
jr $0 



