#include <linux/nfc/pn547_lge_hwadapter.h>

int pn547_get_hw_revision(void)
{
    hw_rev_type hw_revision = HW_REV_MAX;
    hw_revision = lge_get_board_revno();
    dprintk(PN547_DRV_NAME ":ioctl: pn547_read hw revision : %d\n", hw_revision);
    return (int)hw_revision;
}

unsigned int pn547_get_irq_pin(struct pn547_dev *dev)
{
    return dev->client->irq;
}

int pn547_gpio_to_irq(struct pn547_dev *dev)
{
    return dev->client->irq;
}

void pn547_gpio_enable(struct pn547_dev *pn547_dev)
{
    return;
}

void pn547_shutdown_cb(struct pn547_dev *pn547_dev)
{
    return;
}

#ifdef CONFIG_LGE_NFC_USE_PMIC
void pn547_get_clk_source(struct pn547_dev *pn547_dev)
{
    pn547_dev->clk_cont = &cxo_d1.c;
    pn547_dev->clk_pin = &cxo_d1_pin.c;
    // pr_err("%s: xo_cont = %p, xo_pin = %p\n", __func__, pn547_dev->clk_cont, pn547_dev->clk_pin); // for debug
}
#endif

void pn547_parse_dt(struct device *dev, struct pn547_dev *pn547_dev)
{
    struct device_node *np = dev->of_node;

    /* irq gpio info */
    pn547_dev->ven_gpio = of_get_named_gpio_flags(np, "nxp,gpio_ven", 0, NULL);
    pn547_dev->firm_gpio = of_get_named_gpio_flags(np, "nxp,gpio_mode", 0, NULL);
    pn547_dev->irq_gpio = of_get_named_gpio_flags(np, "nxp,gpio_irq", 0, NULL);
}
