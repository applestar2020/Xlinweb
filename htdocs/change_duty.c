#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#define PWM_DUTY_PATH "/run/media/mmcblk1p1/workspace/xlinweb/htdocs/duty"

char InputBuffer[4096];

int DecodeAndProcessData(char *input);



int main(int argc, char *argv[])
{
    int ContentLength; /*数据长度*/
    int x;
    int i;
    char *p;
    char *pRequestMethod;            /*   METHOD属性值   */
    setvbuf(stdin, NULL, _IONBF, 0); /*关闭stdin的缓冲*/

    /*   从环境变量REQUEST_METHOD中得到METHOD属性值   */
    pRequestMethod = getenv("REQUEST_METHOD");
    if (pRequestMethod == NULL)
    {
        printf("request = null\n");
        return 0;
    }
    if (strcasecmp(pRequestMethod, "POST") == 0)
    {
        // printf("the method is POST!\n");
        p = getenv("CONTENT_LENGTH"); //从环境变量CONTENT_LENGTH中得到数据长度
        if (p != NULL)
        {
            ContentLength = atoi(p);
        }
        else
        {
            ContentLength = 0;
        }
        if (ContentLength > sizeof(InputBuffer) - 1)
        {
            ContentLength = sizeof(InputBuffer) - 1;
        }

        i = 0;
        while (i < ContentLength)
        { //从stdin中得到Form数据
            x = fgetc(stdin);
            if (x == EOF)
                break;
            InputBuffer[i++] = x;
        }
        InputBuffer[i] = '\0';
        ContentLength = i;
        DecodeAndProcessData(InputBuffer); //具体译码和处理数据，该函数代码略
    }
    else if (strcasecmp(pRequestMethod, "GET") == 0)
    {
        // printf("<p>OK the method is GET!\n</p>");
        p = getenv("QUERY_STRING"); //从环境变量QUERY_STRING中得到Form数据
        if (p != NULL)
        {
            strncpy(InputBuffer, p, sizeof(InputBuffer));
            DecodeAndProcessData(InputBuffer); //具体译码和处理数据，该函数代码略
        }
    }
    return 0;
}

int DecodeAndProcessData(char *input) //具体译码和处理数据
{
    // int duty=0;
    // duty=atoi(input);

    int fd;
    fd = open(PWM_DUTY_PATH, O_RDWR);
    if (fd < 0)
        exit(1);
    write(fd, input, strlen(input));
    close(fd);

    return 0;
}
