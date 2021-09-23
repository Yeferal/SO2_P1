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
	{ 0x3364d393, "module_layout" },
	{ 0x80a1a9e4, "cdev_del" },
	{ 0x3d5f7e24, "device_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x501e0e43, "class_destroy" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xea9b5888, "device_create" },
	{ 0x32e12e8, "__class_create" },
	{ 0x408bae9a, "cdev_add" },
	{ 0xd8135e1a, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x44dfea52, "send_sig_info" },
	{ 0xc5850110, "printk" },
	{ 0xa6a28e4e, "current_task" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "AA1B237DF57BEFAB3041B18");
