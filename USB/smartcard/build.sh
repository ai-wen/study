#!/bin/bash

echo "libccid pcscd libdbus-1-dev libpcsclite-dev"

g++ -g -o smartcard smartcard.cpp -lpcsclite -I /usr/include/PCSC/
g++ -g -o smartcard smartcard.cpp -lpcsclite -I /usr/include/PCSC/