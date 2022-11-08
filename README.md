compilação:
gcc -Wall -Wextra -g -Iinclude -c cache_simulator.c -o cache_simulator.o
gcc -Wall -Wextra -g -Iinclude -o cache_simulator.exe cache_simulator.o -Llib

execução:
cache_simulator.exe <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada.bin

politicas de substituição:
random ( <substituição> = "r" )
fifo ( <substituição> = "f" )
lru ( <substituição> = "l" )
