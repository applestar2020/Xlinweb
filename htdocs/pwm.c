//#include "APP.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <linux/input.h>

#define PWM_BASE 503
#define PWM_INDEX 4
#define PWM_ADDR (PWM_BASE + PWM_INDEX)

#define PWM_DUTY_PATH "/run/media/mmcblk1p1/workspace/xlinweb/htdocs/duty"

// 定义全局变量控制占空比，通过线程读取文件来改变占空比
static int duty = 10;

pthread_mutex_t lock;

/* Blink LED */
static void *read_duty(void *dummy)
{
    int fd;
    fd = open(PWM_DUTY_PATH, O_RDWR);
    if (fd < 0)
        exit(1);
    while (1)
    {
        int x = 0;
        char out[20];
        lseek(fd, 0, SEEK_SET);
        int size = read(fd, out, 12);
        if (size < 0)
        {
            perror("read error:");
        }
        x = atoi(out);

        if (duty != x)
        {
            pthread_mutex_lock(&lock);
            duty = x;
            duty %= 101;
            pthread_mutex_unlock(&lock);
        }
        usleep(1000000);
    }
}

void delay_(int t)
{
    while (t--)
        usleep(10);
}

void led_ctl_init(void)
{
    int lGpioFd;
    char lCache[100];
    lGpioFd = open("/sys/class/gpio/export", O_WRONLY);
    if (lGpioFd < 0)
    {
        printf("open export fail\n");
        return;
    }
    int len = sprintf(lCache, "%d", PWM_ADDR);
    write(lGpioFd, lCache, len + 1);
    close(lGpioFd);
    sprintf(lCache, "/sys/class/gpio/gpio%d/direction", PWM_ADDR);
    lGpioFd = open(lCache, O_RDWR);
    if (lGpioFd < 0)
    {
        printf("open direction fail\n");
        return;
    }
    write(lGpioFd, "out", 4);
    close(lGpioFd);
}

void led_ctl_on(bool isON)
{
    int lGpioFd;
    char lCache[100];
    sprintf(lCache, "/sys/class/gpio/gpio%d/value", PWM_ADDR);
    lGpioFd = open(lCache, O_RDWR);
    if (lGpioFd < 0)
    {
        printf("open value fail\n");
        return;
    }
    if (isON)
    {
        write(lGpioFd, "1", 2);
    }
    else
    {
        write(lGpioFd, "0", 2);
    }
    close(lGpioFd);
    return;
}

int main(void)
{
    //    Application();
    printf("pwm fan\n");

    int x = 1;
    led_ctl_init();
    led_ctl_on(true);

    pthread_t pth;
    /* Create thread */
    pthread_mutex_init(&lock, NULL);
    pthread_create(&pth, NULL, read_duty, "Blinking LED...");

    while (true)
    {
        led_ctl_on(true);
        delay_(100 - duty);
        led_ctl_on(false);
        delay_(duty);
    }
    printf("end\n");
    return 0;
}
