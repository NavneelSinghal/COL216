.globl main  					# declaring main as global function
.text 

main:
	move $t0, $zero             # $t0 - address, initialising the temporary register $t0 with zero, 
	li $t1, 0                   # $t1 - register to store the value from the first matrix when read from the memory
	li $t2, 0		    		# $t2 - stores (xi-yi)
	li $t3, 0		   			# $t3 - stores (xi-yi)^2
	li $t4, 0		   			# $t4 - accumulator for (xi-yi)^2
	move $t6, $zero				# $t6 - address, initialising the temporary register $t6 with zero

	li $v0, 4                   # printing message to prompt user to enter data 
	la $a0, msg         	    # msg contains message to be printed, it is defined in the .data
	syscall

	loop:
		li $v0, 5           	# getting integer input from user, input is stored in $v0
		syscall
		sw $v0, m1($t0)     	# store the input in the matrix m1
		addi $t0, $t0, 4    	# adding 4 to address containing register, to get next word
		bne $t0, 36, loop   	# if(address != 36) goto loop 0<= $t0<= 36

	move $t0, $zero             # initialising address register back to zero

	li $v0, 4           	    # printing message to prompt user to enter data 
	la $a0, msg2        	    # msg2 contains message to be printed, it is defined in the .data
	syscall
	loop2:
		li $v0,5            	# getting integer input from user, input is stored in $v0
		syscall
		lw $t1, m1($t0)     	# read value of corresponding first element matrix element from memory and store in register $t1
		sw $v0, m2($t6)			# store the value in the matrix
		sub $t2, $t1, $v0   	# (xi-y1) is stored in $t2   
		mult $t2, $t2       	# find (xi-yi)^2, result stored in $lo
		mflo $t3            	# move from $lo to $t3
		add $t4, $t4, $t3   	# $t4 is the accumulator registor
		addi $t0, $t0, 4    	# adding 4 to address containing registor, to get next word
		bne $t0, 36, loop2  	# if(address != 36) goto loop2
	li $v0, 4
	la $a1, msg3
	syscall

	li $t5, 9                   # register $t5 = 9               
	div $t4, $t5		    	# divide accumulated sum by 9, quotient is stored in $lo, reminder is stored in $hi
	mflo $s0		    		# move value from $lo to $s0
	mfhi $s1                    # move remainder from $hi to $s1
	sll $s1, $s1, 1             # multiply remainder by 2
	
	bgt $s1, $t5, else          # if(2*remainder > divisor) then add 1 to quotient 

	li $v0, 1		    		# printing integer value (quotient) without adding 1 (ie. rounding down)
	move $a0, $s0
	syscall
	j	exit

	else:
		addi $s0, $s0, 1
		li $v0, 1		    	# printing integer value (quotient) after adding 1 (ie. rounding up)
		move $a0, $s0
		syscall
	
	exit:
		li $v0, 10          	# terminate the program (ie. exit)
		syscall

.data  
	m1:	.word	0 : 9	    	# memory allocated to store the 9 elements of first matrix 
	m2: .word	0 : 9			# memory allocated to store the 9 elements of second matrix
	msg:	.asciiz "Enter elements of first matrix: "
	msg2:   .asciiz "Enter elements of second matrix: "
	msg3:	.asciiz "The answer is: "

