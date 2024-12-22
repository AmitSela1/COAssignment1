    add $s1, $zero, $zero, $zero, 0, 0            # Initialize row index to 0
    lw $s0, $zero, $imm1, $zero, 0x100, 0           # get circle radius from memory
    beq $zero, $s0, $zero, $imm2, 0, TERMINATE    # finish if radius is zero
    mac $s0, $s0, $s0, $zero, 0, 0                # Calculate radius squared 
ROW_SCAN: blt $zero, $s1, $imm1, $imm2, 256, COLUMN_SCAN    # Loop over screen rows
    beq $zero, $zero, $zero, $imm2, 0, TERMINATE  # stop program after last row
COLUMN_SCAN: blt $zero, $s2, $imm1, $imm2, 256, PIXEL_CALC  # Loop over screen columns
    add $s2, $zero, $zero, $zero, 0, 0           # Reset column index to 0 at the beggining of each row
    add $s1, $s1, $imm1, $zero, 1, 0             #  row index ++
    beq $zero, $zero, $zero, $imm2, 0, ROW_SCAN    # Return to row scanning
PIXEL_CALC:   # Determine if current pixel is within the circle
    add $a0, $zero, $zero, $zero, 0, 0           # Prepare for coordinate transformation
    sub $t0, $s1, $imm1, $zero, 127, 0              # Transform row to x coordinate
    mac $t0, $t0, $t0, $zero, 0, 0               # calc x^2
    sub $t1, $s2, $imm1, $zero, 127, 0            # Transform column to y coordinate
    mac $a0, $t1, $t1, $t0, 0, 0                 # calc y^2 + x^2
    jal $ra, $zero, $zero, $imm2, 0, DRAW_PIXEL  # set pixel color based on distance
    add $s2, $s2, $zero, $imm1, 1, 0             #  column index ++
    beq $zero, $zero, $zero, $imm2, 0, COLUMN_SCAN # Proceed to next pixel in the row
TERMINATE: halt $zero, $zero, $zero, $zero, 0, 0    # finish program 
DRAW_PIXEL: ble $zero, $a0, $s0, $imm2, 0, IN_CIRCLE # Pixel inside circle if within radius squared
OUT_CIRCLE: add $a2, $imm1, $zero, $zero, 0, 0    # set pixel black 
    beq $zero, $zero, $zero, $imm2, 0, DRAW      # Proceed to draw the pixel
IN_CIRCLE: add $a2, $imm1, $zero, $zero, 255, 0  # set pixel white 
DRAW: mac $t0, $s1, $imm1, $s2, 256, 0           # calc pixel index
    out $zero, $imm1, $zero, $t0, 20, 0           # Set monitor address
    out $zero, $imm1, $zero, $a2, 21, 0          # Set pixel color
    out $zero, $imm1, $zero, $imm2, 22, 1         # Write pixel
    beq $zero, $zero, $zero, $ra, 0, 0           # jump to PIXEL_CALC
    .word 0x100 10                               # Preset value for demonstration