        lw      0       1       five    load reg1 with 5 (symbolic address)
        lw      0       2       ten     load reg2 with 10 (symbolic address)
        add     1       2       3       reg3 = reg1 + reg2 (5+10)
        jalr    1       4               store PC+1 to reg4 and jump 
        add     1       2       3       reg3 = reg1 + reg2 (5+10)
        add     1       2       3       reg3 = reg1 + reg2 (5+10)
        add     1       2       3       reg3 = reg1 + reg2 (5+10)
done    halt                            end of program
five    .fill   5
ten     .fill   10
