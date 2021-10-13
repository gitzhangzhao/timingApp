#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xa1ca9524, "module_layout" },
	{ 0xc20dba34, "platform_bus_type" },
	{ 0x64e18ca4, "driver_unregister" },
	{ 0x7915502b, "platform_device_unregister" },
	{ 0x9c4451a7, "driver_register" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x2cbaaf42, "platform_device_register_full" },
	{ 0x752d6b1b, "__uio_register_device" },
	{ 0x89322e4e, "kmem_cache_alloc_trace" },
	{ 0x839fdedc, "kmalloc_caches" },
	{ 0xc5850110, "printk" },
	{ 0x52a5f65c, "uio_unregister_device" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "uio");

