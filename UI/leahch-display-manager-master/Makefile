all: tcdm tcdm-greeter

tcdm: tcdm.c
	gcc -Wall -o $@ $^ `pkg-config --cflags --libs gtk+-3.0` -l pam

tcdm-greeter: tcdm-greeter.c pam.c
	gcc -Wall -o $@ $^ `pkg-config --cflags --libs gtk+-3.0` -l pam

.PHONY: clean

clean:
	rm -f tcdm tcdm-greeter

install:
	cp tcdm /usr/bin/
	cp tcdm-greeter /usr/bin/
	mkdir -p /etc/tcdm/
	cp tcdm.conf /etc/tcdm/
	cp tcdm.css /etc/tcdm/
	cp tcdm.ui /etc/tcdm/
	cp tcdm.pam /etc/pam.d/
	cp mount_fuji.jpg /etc/tcdm/
	cp tcdm.service /lib/systemd/system/
	systemctl daemon-reload
