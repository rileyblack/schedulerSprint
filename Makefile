default: sealTheDeal

sealTheDeal: sealTheDeal.c
        gcc -pthread -o sealTheDeal.out sealTheDeal.c

make clean:
        rm *.out cpu_scheduling_output_file.txt