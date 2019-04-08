KERN_DIR = /home/run/work/kernel/linux-4.19
.PHONY:all
all:
	make -C $(KERN_DIR) M=`pwd` modules
	arm-none-linux-gnueabi-gcc i2c_at24cxx_app.c -o i2c_at24cxx_app
	
.PHONY:clean
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf i2c_at24cxx_app 

cp:
	cp i2c_at24cxx_drv.ko ../../../../rootfs/rootfs_3.16.57/drivers/ -f
	cp i2c_at24cxx_app ../../../../rootfs/rootfs_3.16.57/drivers/	

obj-m += i2c_at24cxx_drv.o


