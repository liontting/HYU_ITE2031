        lw      0       1       five    load reg1 with 5 (symbolic address)
        lw      0       2       ten     load reg2 with 10 (symbolic address)
        add     1       2       3       reg3 = reg1 + reg2 (5+10)
        sw      0       3       neg1    store reg3 to mem8 (symbolic address)
        sw      0       3       6       store reg3 to mem6 (numeric address)
done    halt                            end of program
five    .fill   5
ten     .fill   10
neg1    .fill   -1
