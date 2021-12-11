#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#define PS_LED_BASE 329
#define PS_LED_INDEX 44
#define PS_LED_ADDR (PS_LED_BASE + PS_LED_INDEX)

#define PL_LED_BASE 507
#define PL_LED_INDEX 1
#define PL_LED_ADDR (PL_LED_BASE + PL_LED_INDEX)


#define PS_KEY_BASE 329
#define PS_KEY_INDEX 26
#define PS_KEY_ADDR (PS_KEY_BASE + PS_KEY_INDEX)

#define PL_KEY_BASE 503
#define PL_KEY_INDEX 0
#define PL_KEY_ADDR (PL_KEY_BASE + PL_KEY_INDEX)

char InputBuffer[4096];

int DecodeAndProcessData(char *input); /*具体译码和处理数据，该函数代码略*/
void led_ctl_init(int addr);
void led_ctl_on(int addr, bool isON);
void Gpio_Key_Init(int addr);
bool Gpio_Key_Detct(int addr);


void led_ctl_init(int addr)
{
   int lGpioFd;
   char lCache[100];
   lGpioFd = open("/sys/class/gpio/export", O_WRONLY);
   if (lGpioFd < 0)
   {
      printf("open export fail\n");
      return;
   }
   int len = sprintf(lCache, "%d", addr);
   write(lGpioFd, lCache, len + 1);
   close(lGpioFd);
   sprintf(lCache, "/sys/class/gpio/gpio%d/direction", addr);
   lGpioFd = open(lCache, O_RDWR);
   if (lGpioFd < 0)
   {
      printf("open direction fail\n");
      return;
   }
   write(lGpioFd, "out", 4);
   close(lGpioFd);
}

void led_ctl_on(int addr, bool isON)
{
   int lGpioFd;
   char lCache[100];
   sprintf(lCache, "/sys/class/gpio/gpio%d/value", addr);
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

void Gpio_Key_Init(int addr)
{
   int lGpioFd;
   char lCache[100];
   lGpioFd = open("/sys/class/gpio/export", O_WRONLY);
   if (lGpioFd < 0)
   {
      printf("open export fail\n");
      return;
   }
   int len = sprintf(lCache, "%d", addr);
   write(lGpioFd, lCache, len + 1);
   close(lGpioFd);
   sprintf(lCache, "/sys/class/gpio/gpio%d/direction", addr);
   lGpioFd = open(lCache, O_RDWR);
   if (lGpioFd < 0)
   {
      printf("open direction fail\n");
      return;
   }
   write(lGpioFd, "in", 4);
   close(lGpioFd);
}

bool Gpio_Key_Detct(int addr)
{
   int lGpioFd;
   char lCache[100];
   char val;
   sprintf(lCache, "/sys/class/gpio/gpio%d/value", addr);
   //    printf("%s\n",lCache);

   int size = 0;
   bool state = false;

   lGpioFd = open(lCache, O_RDONLY);
   if (lGpioFd < 0)
   {
      printf("open value fail\n");
      return state;
   }
   if ((size = read(lGpioFd, &val, 1)) < 0)
   {
      perror("read:");
      return false;
   }
   if (val == '0')
   {
      state=false;
   }
   else
   {
      state=true;
   }
   close(lGpioFd);
   
   return state;
}

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
   char obj[10], state[10];
   size_t k = 0;
   size_t len = strlen(input);
   while (k < len && input[k] != '&')
   {
      obj[k] = input[k];
      k++;
   }
   obj[k] = '\0';
   // printf("obj:%s\n",obj);
   if (strcasecmp(obj, "key") == 0)
   {
      // 按键检测
      // printf("key detect\n");
      
      Gpio_Key_Init(PS_KEY_ADDR);
      Gpio_Key_Init(PL_KEY_ADDR);
      int s1=Gpio_Key_Detct(PS_KEY_ADDR);
      int s2=Gpio_Key_Detct(PL_KEY_ADDR);
      printf("%d&%d",s1,s2);
   }
   else
   {
      // led
      k++;
      int i = 0;
      while (k < len)
         state[i++] = input[k++];
      state[i] = '\0';
      if (strcasecmp(obj, "psled") == 0)
      {
         // printf("psled\n");
         led_ctl_init(PS_LED_ADDR);
         led_ctl_on(PS_LED_ADDR,atoi(state));
      }
      else if (strcasecmp(obj, "plled") == 0)
      {
         // printf("plled\n");
         led_ctl_init(PL_LED_ADDR);
         led_ctl_on(PL_LED_ADDR,atoi(state));
      }
      // printf("state:%s\n", state);
   }
   return 0;
}
