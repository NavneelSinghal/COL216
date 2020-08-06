.globl main
.text

main:
	#registers
	
	li $t0, 0			#player number in 0 indexing
	li $t1, 0			#row number in 0 indexing
	li $t2, 0			#column number in zero indexing

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
	move $v0, $t1	
	li $v0, 4		
	la $a0, col			#ask for col
	syscall
	li $v0, 5			#take col number input
	syscall
	move $v0, $t2

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
	lw $s8, board($s4)
	bne $s8, 0, invalidinput	#in the case that it is occupied already, go to invalidinput
	j workhere					#else just go to the workhere part

invalidinput:
	
	li $v0, 4
	la $a0, wrinput		#print wrong input
	syscall
	j loop

#now change the value of the element of the board
workhere:

	move $s5, $zero
	add $s5, $s5, $t0
	lw $s8, work($s5)	#store the value of the working value we would put on the board (1 or 10) into $s8
	sw $s8, board($s4)	#now store it into the board
	li $s8, 1

#change the number of the player here
	sub $t0, $s8, $t0

#now it is loaded in the board's memory

#locations are as follows
#	0	4	8
#	12	16	20
#	24	28	32	

#now for each col, check the sum

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 0
	lw $s8, board($s5)
	add $s6, $s6, $s8
	addi $s5, $s5, 12
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 12
	lw $s8, board($s5) 
	add $s6, $s6, $s8 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 4 
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 12
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 12
	lw $s8, board($s5) 
	add $s6, $s6, $s8 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 8
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 12
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 12
	lw $s8, board($s5) 
	add $s6, $s6, $s8 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

#now for each row, check the sum

	move $s5, $zero
	li $s6, 0
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5) 
	add $s6, $s6, $s8 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 12
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5)
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5)
	add $s6, $s6, $s8 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 24
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5)
	add $s6, $s6, $s8 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

#main diagonal sum

	move $s5, $zero
	li $s6, 0
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 16
	lw $s8, board($s5)
	add $s6, $s6, $s8
	addi $s5, $s5, 16
	lw $s8, board($s5)
	add $s6, $s6, $s8 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

#alternate diagonal sum

	move $s5, $zero
	li $s6, 0
	addi $s5, $s5, 8
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 8
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 8
	lw $s8, board($s5)
	add $s6, $s6, $s8 
	beq $s6, 3, firstwins
	beq $s6, 30, secondwins

#now check the total sum

	move $s5, $zero
	li $s6, 0
	lw $s8, board($s5)
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5)
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5)
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5)
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5)
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5)
	add $s6, $s6, $s8
	addi $s5, $s5, 4
	lw $s8, board($s5) 
	add $s6, $s6, $s8
	
	beq $s6, 45, tied

	j loop

tied:
	#TODO: Print the word tied
	j printboard

firstwins:
	#TODO: Print the sentence "Player 0 wins"
	j printboard

secondwins:
	#TODO: Print the sentence "Player 1 wins"
	j printboard

printboard:

#TODO: complete this part and debug which parts aren't working properly (I couldn't find the bug, so check if it exists in the first place on your laptop too, by running it)

exit:
	li $v0, 10
	syscall
	#program

.data

#note that you need to make some of them registers instead of memory

	board:		.word 0, 0, 0, 0, 0, 0, 0, 0, 0
	work:		.word 1, 10
	work3:		.word 3, 30
	player:		.asciiz "Player "
	turn:		.asciiz "'s turn\n"
	row:		.asciiz "Enter row number in zero indexing\n"
	col:		.asciiz "Enter column number in zero indexing\n"
	wrinput:	.asciiz "Wrong input, please try again\n"
	x:			.asciiz "X"
	o:			.asciiz "O"
	endl:		.asciiz "\n"
	wins:		.asciiz " wins\n"
	tie:		.asciiz "Tie\n"
