obj-m += i2pmm.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	cp ./*.ko ./module/
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

