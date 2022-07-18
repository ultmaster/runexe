g++ -o args.o args.cpp
g++ -o memory1.o memory1.cpp
g++ -o memory2.o memory2.cpp
g++ -o memory3.o memory3.cpp
g++ -o while1.o while1.c
g++ -o canteenInter.o canteenInter.cpp
g++ -o canteenSol.o canteenSol.cpp
g++ library.cpp -L. -llimiter -pthread -o lib_test
