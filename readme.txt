compila��o:
gcc -Wall -Wextra -g -Iinclude -c cache_simulator.c -o cache_simulator.o
gcc -Wall -Wextra -g -Iinclude -o cache_simulator.exe cache_simulator.o -Llib

execu��o:
cache_simulator.exe <nsets> <bsize> <assoc> <substitui��o> <flag_saida> arquivo_de_entrada.bin

politicas de substitui��o:
random ( <substitui��o> = "r" )
fifo ( <substitui��o> = "f" )
lru ( <substitui��o> = "l" )