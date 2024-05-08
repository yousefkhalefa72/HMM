LD_PRELOAD: ./build HMM.c
	gcc -o ./build/lib.so -g -shared -fPIC HMM.c
	export LD_PRELOAD="./build/lib.so"
un_LD_PRELOAD:
	unset LD_PRELOAD

shared: ./build/test.o ./build/shared/libHMM.so
	gcc ./build/test.o ./build/shared/libHMM.so -o ./build/shared/test.out
./build/shared/libHMM.so: ./build/shared/IHMM.o
	gcc --shared -o ./build/shared/libHMM.so ./build/shared/IHMM.o
./build/shared/IHMM.o: HMM.c ./build/shared
	gcc -c -fPIC -o ./build/shared/IHMM.o HMM.c 


static: ./build/test.o ./build/static/lib.a ./build/static
	gcc ./build/test.o ./build/static/lib.a -o ./build/static/test.out
./build/static/lib.a: ./build/static/HMM.o
	ar -r ./build/static/lib.a ./build/static/HMM.o 
./build/static/HMM.o: HMM.c ./build/static
	gcc -c HMM.c -o ./build/static/HMM.o


./build/test.o: test.c ./build
	gcc -c test.c -o ./build/test.o


./build/shared: ./build
	mkdir ./build/shared
./build/static: ./build
	mkdir ./build/static

./build:
	mkdir ./build
clean:
	-rm -rf ./build 
