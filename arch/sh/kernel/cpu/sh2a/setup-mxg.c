/*
 * Renesas MX-G (R8A03022BG) Setup
 *
 *  Copyright (C) 2008  Paul Mundt
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/serial.h>
#include <linux/serial_sci.h>

enum {
	UNUSED = 0,

	/* interrupt sources */
	IRQ0, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7,
	IRQ8, IRQ9, IRQ10, IRQ11, IRQ12, IRQ13, IRQ14, IRQ15,

	PINT0, PINT1, PINT2, PINT3, PINT4, PINT5, PINT6, PINT7,

	SINT8, SINT7, SINT6, SINT5, SINT4, SINT3, SINT2, SINT1,

	SCIF0_BRI, SCIF0_ERI, SCIF0_RXI, SCIF0_TXI,
	SCIF1_BRI, SCIF1_ERI, SCIF1_RXI, SCIF1_TXI,

	MTU2_TGI0A, MTU2_TGI0B, MTU2_TGI0C, MTU2_TGI0D,
	MTU2_TCI0V, MTU2_TGI0E, MTU2_TGI0F,
	MTU2_TGI1A, MTU2_TGI1B, MTU2_TCI1V, MTU2_TCI1U,
	MTU2_TGI2A, MTU2_TGI2B, MTU2_TCI2V, MTU2_TCI2U,
	MTU2_TGI3A, MTU2_TGI3B, MTU2_TGI3C, MTU2_TGI3D, MTU2_TCI3V,
	MTU2_TGI4A, MTU2_TGI4B, MTU2_TGI4C, MTU2_TGI4D, MTU2_TCI4V,
	MTU2_TGI5U, MTU2_TGI5V, MTU2_TGI5W,

	/* interrupt groups */
	PINT, SCIF0, SCIF1,
	MTU2_GROUP1, MTU2_GROUP2, MTU2_GROUP3, MTU2_GROUP4, MTU2_GROUP5
};

static struct intc_vect vectors[] __initdata = {
	INTC_IRQ(IRQ0, 64), INTC_IRQ(IRQ1, 65),
	INTC_IRQ(IRQ2, 66), INTC_IRQ(IRQ3, 67),
	INTC_IRQ(IRQ4, 68), INTC_IRQ(IRQ5, 69),
	INTC_IRQ(IRQ6, 70), INTC_IRQ(IRQ7, 71),
	INTC_IRQ(IRQ8, 72), INTC_IRQ(IRQ9, 73),
	INTC_IRQ(IRQ10, 74), INTC_IRQ(IRQ11, 75),
	INTC_IRQ(IRQ12, 76), INTC_IRQ(IRQ13, 77),
	INTC_IRQ(IRQ14, 78), INTC_IRQ(IRQ15, 79),

	INTC_IRQ(PINT0, 80), INTC_IRQ(PINT1, 81),
	INTC_IRQ(PINT2, 82), INTC_IRQ(PINT3, 83),
	INTC_IRQ(PINT4, 84), INTC_IRQ(PINT5, 85),
	INTC_IRQ(PINT6, 86), INTC_IRQ(PINT7, 87),

	INTC_IRQ(SINT8, 94), INTC_IRQ(SINT7, 95),
	INTC_IRQ(SINT6, 96), INTC_IRQ(SINT5, 97),
	INTC_IRQ(SINT4, 98), INTC_IRQ(SINT3, 99),
	INTC_IRQ(SINT2, 100), INTC_IRQ(SINT1, 101),

	INTC_IRQ(SCIF0_RXI, 220), INTC_IRQ(SCIF0_TXI, 221),
	INTC_IRQ(SCIF0_BRI, 222), INTC_IRQ(SCIF0_ERI, 223),
	INTC_IRQ(SCIF1_RXI, 224), INTC_IRQ(SCIF1_TXI, 225),
	INTC_IRQ(SCIF1_BRI, 226), INTC_IRQ(SCIF1_ERI, 227),

	INTC_IRQ(MTU2_TGI0A, 228), INTC_IRQ(MTU2_TGI0B, 229),
	INTC_IRQ(MTU2_TGI0C, 230), INTC_IRQ(MTU2_TGI0D, 231),
	INTC_IRQ(MTU2_TCI0V, 232), INTC_IRQ(MTU2_TGI0E, 233),

	INTC_IRQ(MTU2_TGI0F, 234), INTC_IRQ(MTU2_TGI1A, 235),
	INTC_IRQ(MTU2_TGI1B, 236), INTC_IRQ(MTU2_TCI1V, 237),
	INTC_IRQ(MTU2_TCI1U, 238), INTC_IRQ(MTU2_TGI2A, 239),

	INTC_IRQ(MTU2_TGI2B, 240), INTC_IRQ(MTU2_TCI2V, 241),
	INTC_IRQ(MTU2_TCI2U, 242), INTC_IRQ(MTU2_TGI3A, 243),

	INTC_IRQ(MTU2_TGI3B, 244),
	INTC_IRQ(MTU2_TGI3C, 245),

	INTC_IRQ(MTU2_TGI3D, 246), INTC_IRQ(MTU2_TCI3V, 247),
	INTC_IRQ(MTU2_TGI4A, 248), INTC_IRQ(MTU2_TGI4B, 249),
	INTC_IRQ(MTU2_TGI4C, 250), INTC_IRQ(MTU2_TGI4D, 251),

	INTC_IRQ(MTU2_TCI4V, 252), INTC_IRQ(MTU2_TGI5U, 253),
	INTC_IRQ(MTU2_TGI5V, 254), INTC_IRQ(MTU2_TGI5W, 255),
};

static struct intc_group groups[] __initdata = {
	INTC_GROUP(PINT, PINT0, PINT1, PINT2, PINT3,
		   PINT4, PINT5, PINT6, PINT7),
	INTC_GROUP(MTU2_GROUP1, MTU2_TGI0A, MTU2_TGI0B, MTU2_TGI0C, MTU2_TGI0D,
		   MTU2_TCI0V, MTU2_TGI0E),
	INTC_GROUP(MTU2_GROUP2, MTU2_TGI0F, MTU2_TGI1A, MTU2_TGI1B,
		   MTU2_TCI1V, MTU2_TCI1U, MTU2_TGI2A),
	INTC_GROUP(MTU2_GROUP3, MTU2_TGI2B, MTU2_TCI2V, MTU2_TCI2U,
		   MTU2_TGI3A),
	INTC_GROUP(MTU2_GROUP4, MTU2_TGI3D, MTU2_TCI3V, MTU2_TGI4A,
		   MTU2_TGI4B, MTU2_TGI4C, MTU2_TGI4D),
	INTC_GROUP(MTU2_GROUP5, MTU2_TCI4V, MTU2_TGI5U, MTU2_TGI5V, MTU2_TGI5W),
	INTC_GROUP(SCIF0, SCIF0_BRI, SCIF0_ERI, SCIF0_RXI, SCIF0_TXI),
	INTC_GROUP(SCIF1, SCIF1_BRI, SCIF1_ERI, SCIF1_RXI, SCIF1_TXI),
};

static struct intc_prio_reg prio_registers[] __initdata = {
	{ 0xfffd9418, 0, 16, 4, /* IPR01 */ { IRQ0, IRQ1, IRQ2, IRQ3 } },
	{ 0xfffd941a, 0, 16, 4, /* IPR02 */ { IRQ4, IRQ5, IRQ6, IRQ7 } },
	{ 0xfffd941c, 0, 16, 4, /* IPR03 */ { IRQ8, IRQ9, IRQ10, IRQ11 } },
	{ 0xfffd941e, 0, 16, 4, /* IPR04 */ { IRQ12, IRQ13, IRQ14, IRQ15 } },
	{ 0xfffd9420, 0, 16, 4, /* IPR05 */ { PINT, 0, 0, 0 } },
	{ 0xfffd9800, 0, 16, 4, /* IPR06 */ { } },
	{ 0xfffd9802, 0, 16, 4, /* IPR07 */ { } },
	{ 0xfffd9804, 0, 16, 4, /* IPR08 */ { } },
	{ 0xfffd9806, 0, 16, 4, /* IPR09 */ { } },
	{ 0xfffd9808, 0, 16, 4, /* IPR10 */ { } },
	{ 0xfffd980a, 0, 16, 4, /* IPR11 */ { } },
	{ 0xfffd980c, 0, 16, 4, /* IPR12 */ { } },
	{ 0xfffd980e, 0, 16, 4, /* IPR13 */ { } },
	{ 0xfffd9810, 0, 16, 4, /* IPR14 */ { 0, 0, 0, SCIF0 } },
	{ 0xfffd9812, 0, 16, 4, /* IPR15 */
		{ SCIF1, MTU2_GROUP1, MTU2_GROUP2, MTU2_GROUP3 } },
	{ 0xfffd9814, 0, 16, 4, /* IPR16 */
		{ MTU2_TGI3B, MTU2_TGI3C, MTU2_GROUP4, MTU2_GROUP5 } },
};

static struct intc_mask_reg mask_registers[] __initdata = {
	{ 0xfffd9408, 0, 16, /* PINTER */
	  { 0, 0, 0, 0, 0, 0, 0, 0,
	    PINT7, PINT6, PINT5, PINT4, PINT3, PINT2, PINT1, PINT0 } },
};

static DECLARE_INTC_DESC(intc_desc, "mxg", vectors, groups,
			 mask_registers, prio_registers, NULL);

static struct plat_sci_port sci_platform_data[] = {
	{
		.mapbase	= 0xff804000,
		.flags		= UPF_BOOT_AUTOCONF,
		.type		= PORT_SCIF,
		.irqs		= { 223, 220, 221, 222 },
	}, {
		.flags = 0,
	}
};

static struct platform_device sci_device = {
	.name		= "sh-sci",
	.id		= -1,
	.dev		= {
		.platform_data	= sci_platform_data,
	},
};

static struct platform_device *mxg_devices[] __initdata = {
	&sci_device,
};

static int __init mxg_devices_setup(void)
{
	return platform_add_devices(mxg_devices,
				    ARRAY_SIZE(mxg_devices));
}
__initcall(mxg_devices_setup);

void __init plat_irq_setup(void)
{
	register_intc_controller(&intc_desc);
}
