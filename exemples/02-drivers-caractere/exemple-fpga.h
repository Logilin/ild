
#ifndef EXEMPLE_FPGA_H
#define EXEMPLE_FPGA_H

// Le numero magique est code sur 8 bits
#define EXEMPLE_FPGA_IOCTL_MAGIC_NUMBER              'F'
// Les numeros de commandes pour l'espace kernel
#define EXEMPLE_FPGA_IOCTL_COMMAND_GET_ENABLE_IRQ    0x01
#define EXEMPLE_FPGA_IOCTL_COMMAND_SET_ENABLE_IRQ    0x02


#include <asm/ioctl.h>
// Les numeros de commandes pour l'espace utilisateur
#define FPGAIOCGENIRQ _IOR(EXEMPLE_FPGA_IOCTL_MAGIC_NUMBER, EXEMPLE_FPGA_IOCTL_COMMAND_GET_ENABLE_IRQ, int *)
#define FPGAIOCSENIRQ _IOW(EXEMPLE_FPGA_IOCTL_MAGIC_NUMBER, EXEMPLE_FPGA_IOCTL_COMMAND_SET_ENABLE_IRQ, int *)


#endif // EXEMPLE_FPGA_H
