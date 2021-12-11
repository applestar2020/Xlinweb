#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>
#include <assert.h>

#define SYS_PATH_IIO "/sys/bus/iio/devices/iio:device0"

// 温度
#define IN_TEMP_OFFSET "in_temp0_ps_temp_offset"
#define IN_TEMP_RAW "in_temp0_ps_temp_raw"
#define IN_TEMP_SCALE "in_temp0_ps_temp_scale"

// 电压
#define IN_VOLT_SCALE "in_voltage2_vccint_scale"
#define IN_VOLT_RAW "in_voltage2_vccint_raw"

static char gNodeName[120];

float get_val(int fd)
{
    char out[20];
    lseek(fd, 0, SEEK_SET);
    int size = read(fd, out, 12);
    if(size<0)
    {
        perror("read error:");
    }
    return atof(out);
}

void get_data()
{
    float in_temp_offest, in_temp_raw, in_temp_scale;
    float in_volt_scale, in_volt_raw;
    int temp = 0, volt = 0; // 温度  *10  ,电压mv

    char upset[20];
    int raw_data = 0, offest = 0;
    float scale, true_data;

    int fd = -1;
    int offset = 0;

// 获取温度的参数并转换为温度
    sprintf(gNodeName, "%s/%s", SYS_PATH_IIO, IN_TEMP_OFFSET);
    fd = open(gNodeName, O_RDWR);
    in_temp_offest=get_val(fd);
    close(fd);

    sprintf(gNodeName, "%s/%s", SYS_PATH_IIO, IN_TEMP_RAW);
    fd = open(gNodeName, O_RDWR);
    in_temp_raw=get_val(fd);
    close(fd);

    sprintf(gNodeName, "%s/%s", SYS_PATH_IIO, IN_TEMP_SCALE);
    fd = open(gNodeName, O_RDWR);
    in_temp_scale=get_val(fd);
    close(fd);

    temp=(int)((in_temp_offest+in_temp_raw)*in_temp_scale)/100;

    // 获取电压的参数并转换为电压  mv
    sprintf(gNodeName, "%s/%s", SYS_PATH_IIO, IN_VOLT_RAW);
    fd = open(gNodeName, O_RDWR);
    in_volt_raw=get_val(fd);
    close(fd);

    sprintf(gNodeName, "%s/%s", SYS_PATH_IIO, IN_VOLT_SCALE);
    fd = open(gNodeName, O_RDWR);
    in_volt_scale=get_val(fd);
    close(fd);

    volt=(int)(in_volt_raw*in_volt_scale);

    if(temp!=0 && volt!=0)
    {
        printf("%.1f&%d",temp/10.0,volt);
    }
}

int main(int argc, char *argv[])
{
    get_data();

    // int fd, len;
    // char *buf = "Hello World!\n", Out[30];
    // fd = open("a.txt", O_CREAT | O_TRUNC | O_RDWR, 0600);
    // printf("open file:a.txt fd = %d\n", fd);
    // len = strlen(buf);
    // int size = write(fd, buf, len);
    // close(fd);
    //Begin to read the file
    // fd = open("a.txt", O_RDWR, 0600);
    // lseek(fd, 0, SEEK_SET); //Before to read the file,you should call the function to make the fd point to begin of files
    // int size = read(fd, Out, 12);
    // printf("size = %d\nread from file:\n %s\n", size, Out);
    // printf("%.2f\n",atof(Out));
    // close(fd);

    return 0;
}
