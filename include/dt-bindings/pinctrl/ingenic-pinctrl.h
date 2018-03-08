#ifndef _DT_INGENIC_PINCTL_H__
#define _DT_INGENIC_PINCTL_H__
#include <dt-bindings/gpio/gpio.h>

#define PINCTL_FUNCTION0	0
#define PINCTL_FUNCTION1	1
#define PINCTL_FUNCTION2	2
#define PINCTL_FUNCTION3	3
#define PINCTL_FUNCHILVL	4
#define PINCTL_FUNCLOLVL	5
#define PINCTL_FUNCTIONS	6

#define PINCTL_CFG_TYPES	1
#define PINCTL_CFG_PULL		0
#define PINCTL_CFG_TYPE_MSK	0x3

#define PINCTL_CFGVAL_SFT	1
#define PINCTL_NOBIAS		((0 << PINCTL_CFGVAL_SFT) | PINCTL_CFG_PULL)
#define PINCTL_PULLEN		((1 << PINCTL_CFGVAL_SFT) | PINCTL_CFG_PULL)

#define INGENIC_GPIO_NOBIAS		0
#define INGENIC_GPIO_PULLEN		1
#define INGENIC_GPIO_BIAS_MASK		2
#define INGENIC_GPIO_BIAS_SFT		0

#define INGENIC_GPIO_PULL(x)		(((x) >> INGENIC_GPIO_BIAS_SFT) & INGENIC_GPIO_BIAS_MASK)

#endif /*_DT_INGENIC_PINCTL_H__*/