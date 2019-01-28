#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xae4b0ceb, "module_layout" },
	{ 0x12c554f0, "cdev_del" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xea01506a, "cdev_add" },
	{ 0x1023a56c, "cdev_init" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x27e1a049, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "1C662F6FB5FDD9C6A4C354D");
