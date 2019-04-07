用法：

1.在用户空间创建一个i2c设备Client
例如： echo at24cxx 0x50 /sys/bus/i2c/devices/i2c-0/new_device

2.安装驱动程序
insmod i2c_at24cxx_drv.ko

3.测试
./i2c_at24cxx_app /dev/at24cxx w 0 kernel

./i2c_at24cxx_app /dev/at24cxx r 0 6



