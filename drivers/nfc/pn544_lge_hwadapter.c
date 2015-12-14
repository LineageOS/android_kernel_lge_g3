#include <linux/nfc/pn544_lge_hwadapter.h>

bool pn544_validate_boot_mode(void) {
    enum lge_boot_mode_type boot_mode;
    boot_mode = lge_get_boot_mode();
	printk("pn544_probe() boot_mode : %d\n",boot_mode);
    if (boot_mode == LGE_BOOT_MODE_FACTORY || boot_mode == LGE_BOOT_MODE_FACTORY2)
    {
        printk("boot_mode == LGE_BOOT_MODE_FACTORY || boot_mode == LGE_BOOT_MODE_FACTORY2\n");
        return true;
    }
    return false;
}

int pn544_get_hw_revision(void)
{
	hw_rev_type hw_revision = HW_REV_MAX;
	hw_revision = lge_get_board_revno();
	dprintk(PN544_DRV_NAME ":ioctl: pn544_read hw revision : %d\n", hw_revision);
	return (int)hw_revision;
}

unsigned int pn544_get_irq_pin(struct pn544_dev *dev)
{
	return dev->client->irq;
}

int pn544_gpio_to_irq(struct pn544_dev *dev)
{
	return dev->client->irq;
}

void pn544_gpio_enable(struct pn544_dev *pn544_dev)
{
	return;
}

void pn544_shutdown_cb(struct pn544_dev *pn544_dev)
{
	return;
}

