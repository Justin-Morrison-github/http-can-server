
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>     // 
#include <sys/ioctl.h>      // For ioctl and SIOCGIFINDEX
#include <linux/can.h>      // For CAN_RAW and sockaddr_can
#include <net/if.h>         // For ifreq
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"

int open_can_socket(char *can_interface)
{
    int can_sock_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_sock_fd < 0)
    {
        perror("CAN socket could not be opened");
        close(can_sock_fd);
        return -1;
    }
    printf("CAN socket opened succesfully\n");

    struct ifreq ifr;
    strcpy(ifr.ifr_name, can_interface);

    if (ioctl(can_sock_fd, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("ioctl SIOCGIFINDEX");
        close(can_sock_fd);
        return -1;
    }

    // Bind the socket to vcan0
    struct sockaddr_can addr = {0};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(can_sock_fd);
        return -1;
    }
    printf("CAN socket bound to vcan0\n");
    return can_sock_fd;
}