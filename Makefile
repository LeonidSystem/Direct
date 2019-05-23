#Example of compilation $< - source.c $@ - source.o
%.o: %.c
   gcc -c -g $< -o $@
   
experiment.o: experiment.c

Experiment: experiment.o
   gcc -o Experiment experiment.o
   
clean:
   rm -rf *.o Experiment
