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
	{ 0xc86911db, "module_layout" },
	{ 0x3ec8886f, "param_ops_int" },
	{ 0x493a5558, "__register_chrdev" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x861049fb, "kmem_cache_alloc_trace" },
	{ 0x91033013, "kmalloc_caches" },
	{ 0x55c09669, "mutex_unlock" },
	{ 0x5d17121e, "mutex_lock" },
	{ 0x4792c572, "down_interruptible" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x2222e10f, "remove_proc_entry" },
	{ 0x37a0cba, "kfree" },
	{ 0x50eedeb8, "printk" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "A65323216BF36FC2EAF2E2B");
