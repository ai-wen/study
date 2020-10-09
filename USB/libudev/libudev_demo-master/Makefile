#########################################
# libudev example 
#
# Vincent Yuan
# 2017-07-26
#########################################

all: hid_test

CC 			?= gcc
INCLUDES 	?= -I /usr/local/lib -I /home/yuanv/hidapi
LIBS 		 = `pkg-config libudev --libs`

hid_test: hid_test.c
	$(CC) $^ $(INCLUDES) $(LIBS) -o hid_test
