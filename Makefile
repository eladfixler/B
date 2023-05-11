all: ex21 ex22

ex21: ex21.c
	gcc -w ex21.c -o comp.out

ex22: ex22.c
	gcc -w ex22.c 
	./a.out conf.txt
rm:
	rm *.out
