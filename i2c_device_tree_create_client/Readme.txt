用法：

1.使用设备树文件,内核启动时创建一个i2c设备Client
例如：

&i2c0 {
    status = "okay";

    eeprom@50 {
        compatible = "atmel,24c02";
        reg = <0x50>;
    };  
};

 

2.安装驱动程序
insmod i2c_at24cxx_drv.ko

3.测试
./i2c_at24cxx_app /dev/at24cxx w 0 kernel

./i2c_at24cxx_app /dev/at24cxx r 0 6



