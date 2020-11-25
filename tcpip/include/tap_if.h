#ifndef __TAP_IF_H
#define __TAP_IF_H

#include "main.h"
#include "skbuff.h"

//for tap
#define TAP_ROUTE   "10.0.0.0/24"
#define FAKE_TAP_NETMASK    0x00ffffff  /* 255.255.255.0 */
#define FAKE_TAP_ADDR       0x0200000a  /* 10.0.0.2 */

extern struct netdev *veth;

void veth_init(void);
void veth_exit(void);


#endif

