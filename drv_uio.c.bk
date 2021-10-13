#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uio_driver.h>

static struct platform_device *pdev;
static struct uio_info *info;
/* static struct timer_list poll_timer; */

static int uio_probe(struct device *dev);
static int uio_remove(struct device *dev);

static struct device_driver uio_driver = {
    .name = "uio_device",
    .bus = &platform_bus_type,
    .probe = uio_probe,
    .remove = uio_remove,
};

static int uio_probe(struct device *dev) {
    printk("uio_probe( %p)\n", dev);

    info = kzalloc(sizeof(struct uio_info), GFP_KERNEL);

    /* fill struct uio_info. */
    info->name = "uio";
    info->version = "0.1";
    info->irq = UIO_IRQ_NONE;
    info->irq_flags = UIO_IRQ_CUSTOM;

    /* set mem name */
    info->mem[0].name = "uio_mem";
    /* set mem addr */
    info->mem[0].addr = 0x0;
    /* set mem type */
    info->mem[0].memtype = UIO_MEM_PHYS;
    /* set mem size */
    info->mem[0].size = 4096UL;

    if (uio_register_device(dev, info))  // register device
        return -ENODEV;
    return 0;
}

static int uio_remove(struct device *dev) {
    uio_unregister_device(info);
    return 0;
}

/* static struct device_driver uio_dummy_driver = { */
/* .name = "kpart", */
/* .bus = &platform_bus_type, */
/* .uio_probe = uio_probe, */
/* .remove = drv_kpart_remove, */
/* }; */

/* static struct platform_device * uio_device; */

static int __init uio_init(void) {
    pdev = platform_device_register_simple("uio_device", -1, NULL, 0);
    return driver_register(&uio_driver);
}

static void __exit uio_exit(void) {
    platform_device_unregister(pdev);
    driver_unregister(&uio_driver);
}

/* static struct platform_driver my_platform_driver = { */
/* .probe=uio_probe, */
/* .remove=uio_remove, */
/* .driver={ */
/* .name="UIO", */
/* .owner=THIS_MODULE, */
/* } */
/* }; */

module_init(uio_init);
module_exit(uio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhangzhao");
MODULE_DESCRIPTION("my uio");

