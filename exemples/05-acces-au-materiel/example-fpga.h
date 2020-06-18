
#ifndef EXAMPLE_FPGA_H
#define EXAMPLE_FPGA_H

// Le numero magique est code sur 8 bits
#define EXAMPLE_FPGA_IOCTL_MAGIC_NUMBER              'F'
// Les numeros de commandes pour l'espace kernel
#define EXAMPLE_FPGA_IOCTL_COMMAND_GET_ENABLE_IRQ    0x01
#define EXAMPLE_FPGA_IOCTL_COMMAND_SET_ENABLE_IRQ    0x02


#include <asm/ioctl.h>
// Les numeros de commandes pour l'espace utilisateur
#define FPGAIOCGENIRQ _IOR(EXAMPLE_FPGA_IOCTL_MAGIC_NUMBER, EXAMPLE_FPGA_IOCTL_COMMAND_GET_ENABLE_IRQ, int *)
#define FPGAIOCSENIRQ _IOW(EXAMPLE_FPGA_IOCTL_MAGIC_NUMBER, EXAMPLE_FPGA_IOCTL_COMMAND_SET_ENABLE_IRQ, int *)


#endif // EXAMPLE_FPGA_H
