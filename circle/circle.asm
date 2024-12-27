    //hi*********
    
    add $s1, $zero, $zero, $zero, 0, 0            # set row index to 0
    lw $s0, $zero, $imm1, $zero, 0x100, 0           # load circle radius from memory
    beq $zero, $s0, $zero, $imm2, 0, terminate    # exit if the radius is zero
    mac $s0, $s0, $s0, $zero, 0, 0                # compute the square of the radius 
row_scan: blt $zero, $s1, $imm1, $imm2, 256, column_scan    # loop through each screen row
    beq $zero, $zero, $zero, $imm2, 0, terminate  # terminate after the last row
column_scan: blt $zero, $s2, $imm1, $imm2, 256, pixel_calc  # loop through each screen column
    add $s2, $zero, $zero, $zero, 0, 0           # reset column index at the start of each row
    add $s1, $s1, $imm1, $zero, 1, 0             # increment row index
    beq $zero, $zero, $zero, $imm2, 0, row_scan    # return to row scanning
pixel_calc:   # check if the current pixel is inside the circle
    add $a0, $zero, $zero, $zero, 0, 0           # prepare for coordinate transformation
    sub $t0, $s1, $imm1, $zero, 127, 0              # convert row to x coordinate
    mac $t0, $t0, $t0, $zero, 0, 0               # calculate x^2
    sub $t1, $s2, $imm1, $zero, 127, 0            # convert column to y coordinate
    mac $a0, $t1, $t1, $t0, 0, 0                 # calculate y^2 + x^2
    jal $ra, $zero, $zero, $imm2, 0, draw_pixel  # set pixel color based on the calculated distance
    add $s2, $s2, $zero, $imm1, 1, 0             # increment column index
    beq $zero, $zero, $zero, $imm2, 0, column_scan # move to the next pixel in the row
terminate: halt $zero, $zero, $zero, $zero, 0, 0    # end the program
draw_pixel: ble $zero, $a0, $s0, $imm2, 0, in_circle # if pixel is inside the circle (within radius squared)
out_circle: add $a2, $imm1, $zero, $zero, 0, 0    # set pixel to black (outside circle)
    beq $zero, $zero, $zero, $imm2, 0, draw      # continue to draw the pixel
in_circle: add $a2, $imm1, $zero, $zero, 255, 0  # set pixel to white (inside circle)
draw: mac $t0, $s1, $imm1, $s2, 256, 0           # calculate pixel index
    out $zero, $imm1, $zero, $t0, 20, 0           # set the monitor address for the pixel
    out $zero, $imm1, $zero, $a2, 21, 0          # set the pixel color (black or white)
    out $zero, $imm1, $zero, $imm2, 22, 1         # write the pixel to the screen
    beq $zero, $zero, $zero, $ra, 0, 0           # return to pixel calculation
    .word 0x100 10              # data section (value is placeholder)            # data section (value is placeholder)      # Preset value for demonstration