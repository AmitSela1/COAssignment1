sll $sp, $imm1, $imm2, $zero, 1, 10              # set $sp with stack pointer base address 1024
lw $a0, $zero, $imm2, $zero, 0, 0x100            # Load the value of n from memory location 0x100 into $a0
lw $a1, $zero, $imm2, $zero, 0, 0x101            # Load the value of k from memory location 0x101 into $a1
jal $ra, $zero, $zero, $imm2, 0, CalcBinom       # move to CalcBinom label to start calculation
sw $zero, $zero, $imm2, $v0, 0, 0x102            # save the result back in memory at 0x102
halt $zero, $zero, $zero, $zero, 0, 0            # finish program

CalcBinom:
add $sp, $sp, $imm1, $zero, -4, 0                #  make space for saved registers
sw $zero, $sp, $imm1, $s0, 3, 0                  # load $s0 on the stack
sw $zero, $sp, $imm1, $ra, 2, 0                  # load return address ($ra) on the stack
sw $zero, $sp, $imm1, $a0, 1, 0                  # load first argument (n) on the stack
sw $zero, $sp, $imm1, $a1, 0, 0                  # load second argument (k) on the stack
beq $zero, $a0, $a1, $imm1, Base, 0              # check if n equals k, proceed to base case
beq $zero, $a1, $zero, $imm1, Base, 0            # check if k equals 0, proceed to base case
beq $zero, $zero, $zero, $imm1, RecCase, 0       # else proceed with rec calculation

Base:
add $v0, $imm1, $zero, $zero, 1, 0               # Set return value to 1 for base case
beq $zero, $zero, $zero, $imm2, 0, Done          # move to Done and return

RecCase:
sub $a0, $a0, $imm1, $zero, 1, 0                 # get n - 1 for rec call
jal $ra, $zero, $zero, $imm1, CalcBinom, 0       # rec call for binom(n-1, k)
add $s0, $v0, $imm1, $zero, 0, 0                 # store the result of binom(n-1, k) in $s0
lw $a0, $sp, $imm1, $zero, 1, 0                  # restore n to $a0
sub $a0, $a0, $imm1, $zero, 1, 0                 # get n - 1 again
sub $a1, $a1, $imm1, $zero, 1, 0                 # calc k - 1
jal $ra, $zero, $zero, $imm1, CalcBinom, 0       # rec call for binom(n-1, k-1)
add $v0, $v0, $s0, $zero, 0, 0                   # add the two previous results

Done: 
lw $a1, $sp, $imm1, $zero, 0, 0                  # Restore $a1
lw $a0, $sp, $imm1, $zero, 1, 0                  # Restore $a0
lw $ra, $sp, $imm1, $zero, 2, 0                  # Restore $ra
lw $s0, $sp, $imm1, $zero, 3, 0                  # Restore $s0
add $sp, $sp, $imm1, $zero, 4, 0                 # set stack pointer back
beq $zero, $zero, $zero, $ra, 0, 0               # Return to caller
.word 0x100 8
.word 0x101 3