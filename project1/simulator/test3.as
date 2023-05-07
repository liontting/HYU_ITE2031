        lw      0       1       five    load reg1 with 5 (symbolic address)
        lw      1       2       0       load reg2 with 1 (numeric address)
        nor     1       2       3       reg3 = reg1 nor reg2 ( ~(5 | 1) )
done    halt                            end of program
five    .fill   5
one     .fill   1
