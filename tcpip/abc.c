
#include <linux/if_tun.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <error.h>
#include <string.h>     //memset
#include <arpa/inet.h>  //ntohs

#define TAP_NAME    "mytap"

int tap_alloc(int flags) {
	struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

	if ((fd = open(clonedev, O_RDWR)) < 0) {
        printf("open error.\r\n");
        return fd;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags;
    strncpy(ifr.ifr_name, TAP_NAME, strlen(TAP_NAME) + 1);

	if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
		printf("ioctl error.\r\n");
        close(fd);
		return err;
	}
    printf("open tun device %s for reading..\r\n", ifr.ifr_name);
    return fd;
}


int main() {
    int tap_fd, nread;
    unsigned char buffer[1500] = {0};

    //IFF_TUN
    //IFF_TAP
    //IFF_NO_PI: do not provide packet infomation
    tap_fd = tap_alloc(IFF_TAP | IFF_NO_PI);
    if (tap_fd < 0) {
        printf("tap_alloc error\r\n");
        return -1;
    }
    sprintf(buffer, "sudo ifconfig %s 192.168.1.123/24", TAP_NAME);
	system(buffer);//配置网卡,这里要是 上一步传递 dev_name 了这里就不能是 tap0了吧
	//system("sudo route add -net 192.168.0.0 netmask 255.255.255.0 gw 192.168.0.1 ");//配置路由

	unsigned char out_buffer[120];
    int offset;
    while(1) {
        nread = read(tap_fd, buffer, sizeof(buffer));
        if (nread < 0) {
            printf ("read error.\r\n");
            break;
        }
        //--------parse ethernet frame-----------
        printf("read %d bytes from tap device:--------------\r\n", nread);
		if (nread < 14 + 20) {
			printf("too short packet!\r\n");
            break;
		}
		out_buffer[0] = '\0';
		for(offset = 0; offset < 6; offset++)
			sprintf(out_buffer + strlen(out_buffer), "0x%2x ", buffer[offset]);
		printf("dest mac:%s\r\n", out_buffer);
        
		out_buffer[0] = '\0';
		for( ; offset < 12; offset++)
			sprintf(out_buffer + strlen(out_buffer), "0x%2x ", buffer[offset]);
		printf("source mac:%s\r\n", out_buffer);
        if (buffer[12] == 0x08 && buffer[13] == 0x06) {
            printf("type: ARP\r\n");
        } else if (buffer[12] == 0x08 && buffer[13] == 0x00) {
            printf("type: IP\r\n");
            printf("src ip: %d.%d.%d.%d\r\n", buffer[26], buffer[27], buffer[28], buffer[29]);
            printf("dst ip: %d.%d.%d.%d\r\n", buffer[30], buffer[31], buffer[32], buffer[33]);
        } else if (buffer[12] == 0x86 && buffer[13] == 0xdd) {
            printf("type: IPv6\r\n");
        } else if (buffer[12] == 0x88 && buffer[13] == 0x64) {
            printf("type: PPPoE\r\n");
        } else if (buffer[12] == 0x88 && buffer[13] == 0x47) {
            printf("type: MPLS Label\r\n");
        } else {
            printf("type: unknown\r\n");
        }
    }
    close(tap_fd);
    return 0;
}

