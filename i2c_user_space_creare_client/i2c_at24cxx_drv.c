
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/capability.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/mutex.h>

#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>



#define DEVICE_NAME "at24cxx"

static struct i2c_client * at24cxx_client;
static struct class *drv_class;

static struct cdev *chr_dev;
static struct device *device ;

static dev_t devnum;


/*
 * open函数,没什么时可做,暂时就绑定i2c设备的客户端到这个设备文件
 */
static int at24cxx_open(struct inode *inode, struct file *file)
{
	file->private_data = at24cxx_client;

	return 0;
}

static ssize_t at24cxx_read(struct file *file, char __user *buf, size_t count,
		loff_t *offset)
{

	unsigned char data[50];
	unsigned char addr;
	int ret = 0;

    /*
     * 读数据包前要发送读的起始寄存器地址,之后才开始读操作
     */

	struct i2c_msg msg[] = {
		[0] = {
			.addr = at24cxx_client->addr,
			.flags = 0,
			.len = 1,
			.buf = &addr,
		},
		[1] = {
			.addr = at24cxx_client->addr,
			.flags = I2C_M_RD,
			.len = count,
			.buf = data,
		}
	};

  
	/*
	 * 获取设备地址
 	 */
	if(copy_from_user(&addr, buf, 1)) {
		return -EFAULT;
	}

	if (i2c_transfer(at24cxx_client->adapter, msg, 2) != 2) {
		printk(KERN_ERR"i2c_transfer fail\n");
		ret = -EIO;
	}


	if (copy_to_user(buf ,data, count) )
		return  -EFAULT;

 
	return count;
}


static ssize_t at24cxx_write(struct file *file, const char __user *buf, 
					size_t count, loff_t *off)
{
	unsigned char data[50];


	/*
	 * 这里的页写没有考虑,页翻转问题,正式的工程代码必须要作出处理
	 */

	struct i2c_msg msg = {
			.addr = at24cxx_client->addr,
			.flags = 0,
			.len = count,
			.buf = data,
	};
	
    /*
     * 要写的数据拷贝到data,第一个字节是寄存器地址
     */
	if(copy_from_user(data, buf, count )) {
		 return -EFAULT;
	}

	if (i2c_transfer(at24cxx_client->adapter,& msg, 1) != 1) {
		printk(KERN_ERR"i2c_transfer fail\n");
		return  -EIO;
	}

 	return count;	
}



static struct file_operations at24cxx_fops = {
	.owner = THIS_MODULE,
	.open  = at24cxx_open,
	.read  = at24cxx_read,
	.write = at24cxx_write,
};


static int at24cxx_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int ret;

	printk(KERN_INFO"at24cxx_probe \n");

	at24cxx_client = client;

	/*
	 * 获取一个主设备号
  	 */
	ret = alloc_chrdev_region(&devnum, 0 , 1, "xxx" );
	if(ret < 0) {
		printk(KERN_ERR"alloc_chrdev_region fail\n");
		goto err_alloc_chrdev_region;
	}

	printk(KERN_INFO"major = %d\b",MAJOR(devnum) );

    /*
     * 获取一个struct cdev结构体,这个结构体是自动释放的
     */

	chr_dev = cdev_alloc();
	if(!chr_dev ) {
		goto err_cdev_alloc;
	}

    /*
     * 初始化cdev
     */	
	cdev_init(chr_dev, &at24cxx_fops);
	chr_dev->owner = THIS_MODULE;

    /*
     * 把这个cdev加入字符设备表
     */
	ret = cdev_add(chr_dev, devnum, 1);
	if(ret) {
		printk(KERN_ERR"cdev_add fail");
		goto err_cdev_add;
	
	}
	/*
 	 * 创建一个设备类
 	 */
	drv_class = class_create(THIS_MODULE, "xxxx");
	if(!drv_class) {
		printk(KERN_ERR"class_create fail\n");
		goto err_class_create;
	}

    /*
     * 创建一个设备
     */
	device = device_create(drv_class,NULL, devnum, NULL, DEVICE_NAME);
	if( !device) {
		printk(KERN_ERR"device_create fail \n");
		goto err_device_create;
	}

	return 0;

err_device_create:
	class_destroy(drv_class);
err_class_create:
err_cdev_add:
	cdev_del(chr_dev);	
err_cdev_alloc:
	unregister_chrdev_region(devnum, 1);
err_alloc_chrdev_region:

	return ret;
}


static int at24cxx_remove(struct i2c_client *client)
{
	device_destroy(drv_class, devnum);
	class_destroy(drv_class);
	cdev_del(chr_dev);
	unregister_chrdev_region(devnum, 1);

	return 0;
}

/*
 * i2c驱动是根据下面这个i2c_device_id表中的名字来匹配,client的名字的
 */
static const struct i2c_device_id at24cxx_id_table[] = {
    { "at24cxx", 0 },
    {}
};


/* Addresses to scan */
static const unsigned short normal_i2c[] = { 0x50, 0x51, 0x52, 0x53, 0x54, I2C_CLIENT_END };

static struct i2c_driver at24cxx_driver = {
	.driver = {
		.name	= "at24cxx",	/* 这个随便起名字 */
	},
	.probe		= at24cxx_probe,
	.remove		= at24cxx_remove,
	.id_table   = at24cxx_id_table,
	.address_list = normal_i2c,
};


module_i2c_driver(at24cxx_driver);
MODULE_LICENSE("GPL");



