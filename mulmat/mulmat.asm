add $s0, $zero, $zero, $zero, 0, 0                     # Initialize row counter i to 0

RowIteration: # Loop over each row of the result matrix
    add $s1, $zero, $zero, $zero, 0, 0                 # Initialize column counter j to 0 for each row

ColIteration:     
    add $s2, $zero, $zero, $zero, 0, 0                 # Initialize element counter k for the dot product
    add $t1, $zero, $zero, $zero, 0, 0                 # Reset accumulator for C[i][j]
    sll $t2, $s0, $imm1, $zero, 2, 0                   # Calculate base address for row i of C
    add $t2, $t2, $s1, $zero, 0, 0                     # Add column j offset for C[i][j]
    add $t2, $t2, $imm1, $zero, 0x120, 0               # Adjust base address to the C matrix region in memory

ElemCalc:          
    sll $t0, $s2, $imm1, $zero, 2, 0                   # Calculate base address for row k of B
    add $t0, $t0, $s1, $zero, 0, 0                     # Add column j offset for B[k][j]
    add $t0, $t0, $imm1, $zero, 0x110, 0               # Adjust base address to the B matrix region in memory
    lw $t0, $t0, $zero, $zero, 0, 0                    # Load value of B[k][j] into $t0
    sll $a0, $s0, $imm1, $zero, 2, 0                   # Calculate base address for row i of A
    add $a0, $a0, $s2, $zero, 0, 0                     # Add column k offset for A[i][k]
    add $a0, $a0, $imm1, $zero, 0x100, 0               # Adjust base address to the A matrix region in memory
    lw $a0, $a0, $zero, $zero, 0, 0                    # Load value of A[i][k] into $a0
    mac $t1, $t0, $a0, $t1, 0, 0                       # Accumulate product of A[i][k] and B[k][j] into $t1
    add $s2, $s2, $zero, $imm1, 1, 0                   # Increment k (move to the next element in the row/column)
    bne $zero, $imm1, $s2, $imm2, 4, ElemCalc          # If k < 4, repeat the calculation for next element
    sw $t1, $t2, $zero, $zero, 0, 0                    # Store the result of C[i][j] into memory
    add $s1, $s1, $zero, $imm1, 1, 0                   # Increment j (move to the next column)
    bne $zero, $imm1, $s1, $imm2, 4, ColIteration      # If j < 4, process the next column
    add $s0, $s0, $zero, $imm1, 1, 0                   # Increment i (move to the next row)
    bne $zero, $imm1, $s0, $imm2, 4, RowIteration      # If i < 4, process the next row
    halt $zero, $zero, $zero, $zero, 0, 0              # End the program

   
  			    .word 0x100 0x0
				.word 0x101 0x1
				.word 0x102 0x2
				.word 0x103 0x3
				.word 0x104 0xA
				.word 0x105 0x7
				.word 0x106 0xC
				.word 0x107 0x7
				.word 0x108 0x8
				.word 0x109 0x9
				.word 0x10A 0xF
				.word 0x10B 0xB
				.word 0x10C 0xC
				.word 0x10D 0x2
				.word 0x10E 0xE
				.word 0x10F 0x1
				.word 0x110 0x0
				.word 0x111	0xD
				.word 0x112	0x2
				.word 0x113	0x8
				.word 0x114	0x3
				.word 0x115	0x9
				.word 0x116	0x6
				.word 0x117	0xC
				.word 0x118	0x8
				.word 0x119	0x9
				.word 0x11A	0xB
				.word 0x11B	0x2
				.word 0x11C	0x0
				.word 0x11D	0x3
				.word 0x11E	0x5
				.word 0x11F 0x8
