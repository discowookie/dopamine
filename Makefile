all:
	scp main.cc led1642gw_driver.h root@beaglebone.local:/root
	ssh root@beaglebone.local "g++ -lrt -O3 -o main main.cc && ./main"
