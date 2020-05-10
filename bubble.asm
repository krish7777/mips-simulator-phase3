.data
array:
	.word 0, 0, 11, 11, 4, 2, 6, 1, 99, 8, 200, 17, 110, 152, 209, 180
.text
.globl main
main:
	lui $s0, 0
	lw $s1, 0($s0)  
	lw $s3, 8($s0)
	lw $s4, 12($s0)
	la $t1, 16($s0)
loop1:   
	beq $s1,$s4,exit1
	lw $s2, 4($s0)
loop2:
	beq $s2,$s3,exit2   
	lw $s5, 0($t1)
	lw $s6, 4($t1)
	slt $t0,$s5,$s6
	beq $t0,$zero,swap
begin:
	addi $t1,$t1,1
	addi $s2,$s2,1;
	j loop2
exit2:    
	addi $s1,$s1,1
	la $t1, 16($s0)
	sub $s3,$s3,1
	j loop1
swap:      
	add $s5,$s5,$s6
	sub $s6,$s5,$s6
	sub $s5,$s5,$s6
	sw $s5, 0($t1)  
	sw $s6, 4($t1)   
	j begin
exit1: 
	addi $s7,$s7,7     
	jr $ra
