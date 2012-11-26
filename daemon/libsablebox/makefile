libsablebox.a: sablebox.o
	ar -rs libsablebox.a sablebox.o

scaffold: scaffold.o sablebox.o 
	gcc scaffold.o sablebox.o -o scaffold

scaffold.o: sablebox.h scaffold.c
	gcc scaffold.c -c

sablebox.o: sablebox.h sablebox.c
	gcc sablebox.c -c

test: scaffold helloworld re_seg re_div0 mle_dyn mle_static tle 1003re 1500ac 2000mle tle block
	./scaffold
	
