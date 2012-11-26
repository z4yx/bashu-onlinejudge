libsablebox.a: sablebox.o
	ar -rs libsablebox.a sablebox.o

scaffold: scaffold.o sablebox.o 
	gcc scaffold.o sablebox.o -o scaffold

scaffold.o: sablebox.h scaffold.c
	gcc scaffold.c -c

sablebox.o: sablebox.h sablebox.c
	gcc sablebox.c -c

helloworld: helloworld.c
	gcc helloworld.c -static -o helloworld
re_seg: re_seg.cpp
	g++ re_seg.cpp -static -o re_seg
re_div0: re_div0.cpp
	g++ re_div0.cpp -static -o re_div0
mle_dyn: mle_dyn.cpp
	g++ mle_dyn.cpp -static -o mle_dyn
mle_static: mle_static.cpp
	g++ mle_static.cpp -static -o mle_static
tle: tle.cpp
	g++ tle.cpp -static -o tle
1003re: 1003re.cpp
	g++ 1003re.cpp -static -o 1003re
1500ac: 1500ac.cpp
	g++ 1500ac.cpp -static -o 1500ac
2000mle: 2000mle.cpp
	g++ 2000mle.cpp -static -o 2000mle
block: block.c
	gcc block.c -static -o block


test: scaffold helloworld re_seg re_div0 mle_dyn mle_static 1003re 1500ac 2000mle tle block
	./scaffold
	
