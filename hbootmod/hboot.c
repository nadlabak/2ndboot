#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/devfs_fs_kernel.h>
#include "hboot.h"
#define CTRL_DEVNAME "hbootctrl"
static int dev_major;

// will _never_ return
int __attribute__((__naked__)) do_branch(void *bootlist, uint32_t bootsize, uint32_t new_ttbl, void *func) {
	__asm__ volatile (
		"bx r3\n"
	);
	return 0;
}

static void l1_map(uint32_t *table, uint32_t phys, uint32_t virt, size_t sects, uint32_t flags) {
	uint32_t physbase, virtbase;

	physbase = phys >> 20;
	virtbase = virt >> 20;
	while (sects-- > 0) {
		table[virtbase++] = (physbase++ << 20) | flags;
	}
}

void build_l1_table(uint32_t *table) {
	memset(table, 0, 4*4096);
	l1_map(table, PHYS_OFFSET, PHYS_OFFSET, 64, PMD_TYPE_SECT | PMD_SECT_AP_WRITE | PMD_SECT_WB);
	l1_map(table, PHYS_OFFSET, PAGE_OFFSET, 64, PMD_TYPE_SECT | PMD_SECT_AP_WRITE | PMD_SECT_WB);
	l1_map(table, L2CC_BASE_ADDR, L2CC_BASE_ADDR, 1, PMD_TYPE_SECT | PMD_SECT_AP_WRITE | PMD_SECT_UNCACHED);
}

int hboot_boot(int handle) {
	bootfunc_t boot_entry;
	uint32_t bootsize, listsize;
	void *bootlist;
	uint32_t l1_mem, *l1_table;

	l1_mem = get_high_pages(2);
	if (l1_mem == 0) {
		printk("Failed to allocate new l1 table\n");
		return -ENOMEM;
	}
	if (l1_mem & 0x3fff) {
//		l1_table = (uint32_t*)(((l1_mem >> 14) + 1) << 14);
		printk("unaligned l1 table\n");
		free_high_pages((void*)l1_mem, 2);
		return -EINVAL;
	} else {
		l1_table = (uint32_t*)l1_mem;
	}
	build_l1_table(l1_table);

	boot_entry = get_bootentry(&bootsize, handle);
	if (boot_entry == NULL) {
		return -EINVAL;
	}
	bootlist = get_bootlist(&listsize);
	if (bootlist == NULL) {
		return -ENOMEM;
	}
	preempt_disable();
	local_irq_disable();
	local_fiq_disable();
	do_branch(bootlist, listsize, virt_to_phys(l1_table), boot_entry);

/*	not a chance	*/
#if 0
	set_ttbl_base(l1_old);
	local_fiq_enable();
	local_irq_enable();
	preempt_enable();
#else
	while (1);
#endif
	return -EBUSY;
}

static int hbootctrl_open(struct inode *inode, struct file *file);
static int hbootctrl_release(struct inode *inode, struct file *file);
static int hbootctrl_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int hbootctrl_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);

static struct file_operations hbootctrl_ops = {
	.owner		=	THIS_MODULE,
	.open		=	hbootctrl_open,
	.release	=	hbootctrl_release,
	.ioctl		=	hbootctrl_ioctl,
	.write		=	hbootctrl_write,

};

static int hbootctrl_open(struct inode *inode, struct file *file) {
	printk(KERN_INFO CTRL_DEVNAME ": `open' stub called\n");
	return 0;
}

static int hbootctrl_release(struct inode *inode, struct file *file) {
	printk(KERN_INFO CTRL_DEVNAME ": `releae' stub called\n");
	return 0;
}

static int hbootctrl_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg) {
	int ret;
	int handle;
	struct hboot_buffer_req buf_req;

	ret = 0;
	switch (cmd) {
		case HBOOT_ALLOCATE_BUFFER:
			if (copy_from_user((void*)&buf_req, (void*)arg, sizeof(struct hboot_buffer_req)) != 0) {
				printk(KERN_WARNING CTRL_DEVNAME ": failed to copy buffer request\n");
				return -EINVAL;
			}
			ret = allocate_buffer(buf_req.type, buf_req.size, buf_req.tag);
			break;
		case HBOOT_FREE_BUFFER:
			handle = (int)arg;
			ret = free_buffer(handle);
			break;
		case HBOOT_SELECT_BUFFER:
			handle = (int)arg;
			ret = select_buffer(handle);
			if (ret >= 0) {
				file->f_pos = 0;
			}
			break;
		case HBOOT_BOOT:
			handle = (int)arg;
			ret = hboot_boot(handle);
			break;
		default:
			printk(KERN_INFO CTRL_DEVNAME ": unknown ioctl\n");
			ret = -EINVAL;
			break;
	}
	return ret;
}

static int hbootctrl_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
	return buffer_append_userdata(buf, count, ppos);
}

int __init hboot_init(void) {
	int ret;

	ret = buffers_init();
	if (ret < 0) {
		printk(KERN_WARNING CTRL_DEVNAME ": Failed to initialize buffers table\n");
		return ret;
	}

	ret = register_chrdev(0, CTRL_DEVNAME, &hbootctrl_ops);
	if (ret < 0) {
		printk(KERN_WARNING CTRL_DEVNAME ": Failed to register dev\n");
		buffers_destroy();
		return ret;
	}
	dev_major = ret;

	ret = devfs_mk_cdev(MKDEV(dev_major, 0), S_IFCHR | S_IRUSR | S_IWUSR, CTRL_DEVNAME);
	if (ret < 0) {
		printk(KERN_WARNING CTRL_DEVNAME ": Failed to create dev\n");
		unregister_chrdev(dev_major, CTRL_DEVNAME);
		buffers_destroy();
		return ret;
	}

	printk(KERN_INFO CTRL_DEVNAME ":  Successfully registered dev\n");
	return 0;
}

void __exit hboot_exit(void) {
	if (dev_major) {
		buffers_destroy();
		unregister_chrdev(dev_major, CTRL_DEVNAME);
		devfs_remove(CTRL_DEVNAME);
	}
	return;
}

module_init(hboot_init);
module_exit(hboot_exit);
