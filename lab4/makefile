PROGS = main.o ptools.o

hw4:$(PROGS)
	g++ $^ -o $@ -g -lcapstone

%.o:%.cpp
	g++ $< -c -o $@ -g