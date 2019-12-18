// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2014 Carlo Caione <carlo@caione.org>
 */

#include <linux/of_platform.h>
#include <linux/slab.h>
#include <asm/mach/arch.h>

static unsigned char inline chartonum(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return (c - 'A') + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return (c - 'a') + 10;
    }
    return 0;
}

static void __init meson_mac_init(const char *compatible, unsigned char mac[6]) {
    struct device_node *np;
    struct property *newmac;
    np = of_find_compatible_node(NULL, NULL, compatible);
    if (np == NULL) {
        printk("can't find compatible\n");
        return;
    }
    newmac = kzalloc(sizeof(*newmac) + 6, GFP_KERNEL);
    if (!newmac) {
        printk("new mac alloc error\n");
        goto put_node;
    }
    newmac->value = newmac + 1;
    newmac->length = 6;
    newmac->name = kstrdup("local-mac-address", GFP_KERNEL);
    if (!newmac->name) {
        kfree(newmac);
        printk("name alloc error\n");
        goto put_node;
    }

    printk("update local-mac-address\n");
    memcpy(newmac->value, mac, 6);
    of_update_property(np, newmac);
put_node:
    of_node_put(np);
}

unsigned char boot_mac[6];
static int __init mac_addr_set(char *line) {
    int i = 0;
    printk("******** recv %s\n", line);
    for (i = 0; i < 6 && line[0] != '\0' && line[1] != '\0'; i++) {
        boot_mac[i] = chartonum(line[0]) << 4 | chartonum(line[1]);
        line += 3;
    }
    printk("******** setup mac-addr: %x:%x:%x:%x:%x:%x\n",
            boot_mac[0], boot_mac[1], boot_mac[2], boot_mac[3], boot_mac[4], boot_mac[5]);

    return 1;
}
__setup("mac=", mac_addr_set);

static void __init meson_machine_init(void) {
    meson_mac_init("amlogic,meson8b-dwmac", boot_mac);
}

static const char * const meson_common_board_compat[] = {
	"amlogic,meson6",
	"amlogic,meson8",
	"amlogic,meson8b",
	"amlogic,meson8m2",
	NULL,
};

DT_MACHINE_START(MESON, "Amlogic Meson platform")
	.dt_compat	= meson_common_board_compat,
	.init_machine   = meson_machine_init,
	.l2c_aux_val	= 0,
	.l2c_aux_mask	= ~0,
MACHINE_END
