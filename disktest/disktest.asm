RAK HAIFA IS MACCABI
out $zero, $zero, $imm1, $imm1, 1, 0          # Enable IRQ1 for disk read/write completion interrupts
out $zero, $imm1, $zero, $imm2, 6, Irq1       # Set the IRQ1 interrupt handler to the address labeled 'Irq1'
add $t0, $imm1, $zero, $zero, 7, 0            # Initialize loop counter $t0 to 7 (for 8 iterations: sectors 0 to 7)
add $t1, $zero, $zero, $zero, 0, 0            # Initialize memory address pointer $t1 to 0 (starting sector)

Loop: 
    beq $zero, $t0, $imm1, $imm2, -1, Stop    # If loop counter $t0 is -1, jump to 'Stop' (end of the loop)
    out $zero, $imm1, $zero, $zero, 16, 0     # Clear the disk buffer before starting the next operation
    out $zero, $imm1, $zero, $t0, 15, 0       # Set the current sector for the read operation to $t0
    out $zero, $imm1, $zero, $imm2, 14, 1     # Issue a disk read command for sector $t0
    add $t2, $imm1, $zero, $zero, 1, 0        # Set $t2 to 1 to indicate that the read operation is in progress

Read: 
    and $zero, $zero, $zero, $zero, 0, 0      # Idle (do nothing) while waiting for the read operation to complete
    beq $zero, $t2, $imm1, $imm2, 1, Read     # If $t2 is still 1, stay in 'Read' loop (read not complete yet)

    add $t1, $imm1, $t0, $zero, 1, 0          # Increment $t1 to point to the next sector (for the write operation)
    out $zero, $imm1, $zero, $zero, 16, 0     # Clear the disk buffer before starting the write operation
    out $zero, $imm1, $zero, $t1, 15, 0       # Set the next sector (i + 1) for the write operation
    out $zero, $imm1, $zero, $imm2, 14, 2     # Issue a disk write command for sector $t1
    add $t2, $imm1, $zero, $zero, 1, 0        # Set $t2 to 1 to indicate that the write operation is in progress

WRITE: 
    and $zero, $zero, $zero, $zero, 0, 0      # Idle (do nothing) while waiting for the write operation to complete
    beq $zero, $t2, $imm1, $imm2, 1, WRITE    # If $t2 is still 1, stay in 'WRITE' loop (write not complete yet)

    add $t0, $imm1, $t0, $zero, -1, 0         # Decrement loop counter $t0 (move to the previous sector)
    bge $zero, $t0, $zero, $imm1, Loop, 0     # If $t0 is non-negative, repeat the loop (continue with the next sector)

Stop: 
    halt $zero, $zero, $zero, $zero, 0, 0     # Halt the program (end of execution)

Irq1: 
    add $t2, $zero, $zero, $zero, 0, 0        # Reset $t2 to 0 to indicate the disk operation is complete
    out $zero, $imm1, $zero, $imm2, 4, 0      # Clear the IRQ1 status to acknowledge the interrupt
    reti $zero, $zero, $zero, $zero, 0, 0     # Return from the interrupt service routine (ISR)
