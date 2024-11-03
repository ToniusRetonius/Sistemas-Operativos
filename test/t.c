#include <linux/init.h>
#include <linux/module.h> // THIS_MODULE
#include <linux/kernel.h> // Kernel cosas
#include <linux/fs.h>     // File operations
#include <linux/cdev.h>   // Char devices
#include <linux/device.h> // Nodos filesystem
#include <linux/uaccess.h> // copy_to_user
#include <linux/slab.h>    // kmalloc
#include <linux/random.h>  // get_random_bytes

static int upper_bound = -1;

static ssize_t azar_read(struct file *filp, char __user *data, size_t size, loff_t *offset) 
{

    unsigned int resultado; 
    unsigned int nro_random; 
    int formateo; 
    // si no se escribió nada :: ret 
    if (upper_bound > -1)
    {
        // hay que devolver un string 
        unsigned char* res = kmalloc(size, GFP_KERNEL);

        // generamos el nro_random
        get_random_bytes(&nro_random, sizeof(nro_random));

        // generamos el resultado
        resultado = nro_random % upper_bound;

        // tenemos que guardar en res
        formateo = snprintf(res, sizeof(res), "%d\n",resultado);

        // como se puede observar en el infome, esta función devuelve info importante
        if (formateo >= sizeof(res) || formateo < 0) 
        {
            kfree(res);
            return -EPERM;
        }

        // en caso contrario : es porque fue bien ==> le pasamos al usuario
        copy_to_user(data, res, formateo);
        
        // liberamos
        kfree(res);
        
        // ret
        return 0;
    }
    
	return -EPERM;
}

static ssize_t azar_write(struct file *filp, const char __user *data, size_t size, loff_t *offset) {
	// variables
    int str_to_int; 
	unsigned long copiamos; 
   	
    // pedimos memo
	char* buffer = kmalloc(size+1, GFP_KERNEL);

	// copiamos del espacio de memoria del usuario
    copiamos = copy_from_user(buffer, data, size);

	// agregamos el caracter nulo
	buffer[size] = '\0';

	// usamos kstrtoint para convertir todo esto a entero ( más info en el informe del taller )
    str_to_int = kstrtoint(buffer, 10, &upper_bound);

	// chequeamos si fue bien la conversión
	if(str_to_int == 0 && upper_bound > -1)
	{
		kree(buffer);
		return 0;
	}

	// liberamos memo
	kfree(buffer);
	return -EPERM;
}

static struct file_operations azar_fops = {
  .owner = THIS_MODULE,
  .read = azar_read,
  .write = azar_write,
};

static dev_t azar_devno;
static struct cdev azar_cdev;
static struct class *azar_class;

static int __init azar_init(void) {
    cdev_init(&azar_cdev, &azar_fops);
    alloc_chrdev_region(&azar_devno, 0, 1, "SO-azar");
    cdev_add(&azar_cdev, azar_devno, 1);

    azar_class = class_create(THIS_MODULE, "azar");
    device_create(azar_class, NULL, azar_devno, NULL, "azar");

    printk(KERN_ALERT "Loading module 'azar'\n");
    return 0;
}

static void __exit azar_exit(void) {
    printk(KERN_ALERT "Unloading module 'azar'\n");

    device_destroy(azar_class, azar_devno);
    class_destroy(azar_class);

    unregister_chrdev_region(azar_devno, 1);
    cdev_del(&azar_cdev);
}

module_init(azar_init);
module_exit(azar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Generador de números al azar");
