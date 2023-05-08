        lw      0       1       one     load reg1 with 1 (symbolic address)
        lw      0       2       five    load reg2 with 5 (symbolic address)
        lw      0       3       ten     load reg3 with 10 (symbolic address)
        lw      0       4       neg5    load reg4 with -5 (symbolic address)
        lw      0       5       neg1    load reg5 with -1 (symbolic address)
start   add     1       5       1       decrement reg1
        noop
        add     2       3       4
        add     4       5       2
        add     3       5       5
        beq     0       0       done
        noop
done    halt                            end of program
one     .fill   1
five    .fill   5
ten     .fill   10
neg1    .fill   -1
neg5    .fill   -5
