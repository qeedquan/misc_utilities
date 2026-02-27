#!/bin/sh

# tpm2_clear does not clear this index
tpm2_nvdefine -C o -s 8 -a "ownerread|authread|authwrite|nt=1" 0x1500016 -p index
tpm2_nvincrement -C 0x1500016  0x1500016 -P "index"
tpm2_nvread 0x1500016 -P index | xxd -p
