all:
	scp main.cc led1642gw_driver.h root@beaglebone.local:/root
	ssh root@beaglebone.local "g++ -lrt -O3 -std=c++0x -o main main.cc"
