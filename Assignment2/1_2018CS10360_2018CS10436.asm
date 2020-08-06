.globl main
.text

main:
	#registers
	
	li $t0, 0			#player number in 0 indexing
	li $t1, 0			#row number in 0 indexing
	li $t2, 0			#column number in zero indexing
	li $t6, 0

loop:

	#taking input
	li $v0, 4
	la $a0, player		#print player - see .data
	syscall
	li $v0, 1			#print number of player
	move $a0, $t0
	syscall
	li $v0, 4			#print turn - see .data
	la $a0, turn
	syscall
	li $v0, 4			
	la $a0, row			#ask for row
	syscall
	li $v0, 5			#take row number input
	syscall
	move $t1,$v0	
	li $v0, 4		
	la $a0, col			#ask for col
	syscall
	li $v0, 5			#take col number input
	syscall
	move $t2, $v0

#now we have the row and column numbers in 0 indexing

#now we need to check for valid row and col nums
#for this we can check if row < 0 or row > 2 or col < 0 or col > 2

checkinput:

	blt $t1, 0, invalidinput
	bgt $t1, 2, invalidinput
	blt $t2, 0, invalidinput
	bgt $t2, 2, invalidinput
#check occupied or not
	li $s0, 12
	li $s1, 4
	mult $t1, $s0
	mflo $s2			#row*12
	mult $t2, $s1
	mflo $s3			#col*4
	add $s2, $s2, $s3
	move $s4, $zero
	add $s4, $s2, $s4
	lw $s7, board($s4)
	bne $s7, 0, invalidinput	#in the case that it is occupied already, go to invalidinput
	j workhere					#else just go to the workhere part

invalidinput:
	
	li $v0, 4
	la $a0, wrinput		#print wrong input
	syscall
	j loop

#now change the value of the element of the board
workhere:

	move $s5, $zero
	move $t7, $zero
	sll $t7, $t0, 2
	add $s5, $s5, $t7
	lw $s7, work($s5)	#store the value of the working value we would put on the board (1 or 10) into $s7
	sw $s7, board($s4)	#now store it into the board
	li $s7, 1

#change the number of the player here
	sub $t0, $s7, $t0

#now it is loaded in the board's memory

#locations are as follows
#	0	4	8
#	12	16	20
#	24	28	32	

#now for each col, check the sum

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 0
	lw $s7, board($s5)
	add $s6, $s6, $s7
	addi $s5, $s5, 12
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 12
	lw $s7, board($s5) 
	add $s6, $s6, $s7 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 4 
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 12
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 12
	lw $s7, board($s5) 
	add $s6, $s6, $s7 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 8
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 12
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 12
	lw $s7, board($s5) 
	add $s6, $s6, $s7 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

#now for each row, check the sum

	move $s5, $zero
	li $s6, 0
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5) 
	add $s6, $s6, $s7 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 12
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5)
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5)
	add $s6, $s6, $s7 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 24
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5)
	add $s6, $s6, $s7 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

#main diagonal sum

	move $s5, $zero
	li $s6, 0
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 16
	lw $s7, board($s5)
	add $s6, $s6, $s7
	addi $s5, $s5, 16
	lw $s7, board($s5)
	add $s6, $s6, $s7 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

#alternate diagonal sum

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 8
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 8
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 8
	lw $s7, board($s5)
	add $s6, $s6, $s7 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

#now check the total sum

	move $s5, $zero
	li $s6, 0
	lw $s7, board($s5)
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5)
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5)
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5)
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5)
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5)
	add $s6, $s6, $s7
	addi $s5, $s5, 4
	lw $s7, board($s5) 
	add $s6, $s6, $s7
	
	beq $s6, 45, tied

	move $t4, $zero

printboard2:
	beq $t4, 12, newline2
	beq $t4, 24, newline2
	beq $t4, 36, newline3
	j print2

newline3:
	li $v0, 4
	la $a0, barn
	syscall
	j loop

newline2:
	li $v0, 4
	la $a0, barn
	syscall
	j print2

print2:
	lw $t5, board($t4)
	li $v0, 4
	la $a0, bar
	syscall
	beq $t5, 10, printerO2
	beq $t5, 1, printerX2
	beq $t5, 0, printer_2
	add $t4, $t4, 4
	j printboard2

printer_2:
	li $v0, 4
	la $a0, _
	syscall
	add $t4,$t4,4
	j printboard2

printerX2:

	li $v0, 4
	la $a0, x
	syscall
	add $t4, $t4, 4
	j printboard2

printerO2:
	li $v0, 4
	la $a0, o
	syscall
	add $t4, $t4, 4
	j printboard2

tied:
	li $v0, 4
	la $a0, tie
	syscall
	j printboard

firstwins:
	li $v0, 4
	la $a0, player
	syscall

	li $v0, 1
	li $a0, 0
	syscall

	li $v0, 4
	la $a0, wins
	syscall

	move $t4, $zero
	j printboard

secondwins:
	li $v0, 4
	la $a0, player
	syscall

	li $v0, 1
	li $a0, 1
	syscall

	li $v0, 4
	la $a0, wins
	syscall

	move $t4, $zero
	j printboard

printboard:
	beq $t4, 12, newline
	beq $t4, 24, newline
	beq $t4, 36, newline4
	j print

newline:
	li $v0, 4
	la $a0, barn
	syscall
	j print

newline4:
	li $v0, 4
	la $a0, barn
	syscall
	j exit

print:
	lw $t5, board($t4)
	li $v0, 4
	la $a0, bar
	syscall
	beq $t5, 10, printerO
	beq $t5, 1, printerX
	beq $t5, 0, printer_
	add $t4, $t4, 4
	j printboard

printer_:
	li $v0, 4
	la $a0, _
	syscall
	add $t4,$t4,4
	j printboard

printerX:

	li $v0, 4
	la $a0, x
	syscall
	add $t4, $t4, 4
	j printboard

printerO:
	li $v0, 4
	la $a0, o
	syscall
	add $t4, $t4, 4
	j printboard


exit:
	li $v0, 4
	la $a0, bar
	syscall

	li $v0, 10
	syscall

.data

	board:		.word 0, 0, 0, 0, 0, 0, 0, 0, 0
	work:		.word 1, 10
	work3:		.word 3, 30
	player:		.asciiz "Player "
	turn:		.asciiz "'s turn\n"
	row:		.asciiz "Enter row number in zero indexing\n"
	col:		.asciiz "Enter column number in zero indexing\n"
	wrinput:	.asciiz "Wrong input, please try again\n"
	x:		    .asciiz "X"
	o:		    .asciiz "O"
	endl:		.asciiz "\n"
	wins:		.asciiz " wins\n"
	tie:		.asciiz "Tie\n"
	bar: 		.asciiz "|"
	barn:		.asciiz "|\n|_|_|_|\n"
	_:		    .asciiz " "
