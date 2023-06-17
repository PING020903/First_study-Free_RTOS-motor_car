#include "FreeRTOS.h"
#include "task.h"
//#include "semphr.h"
//#include "event_groups.h"

#include "sys.h"
#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include "beep.h"
#include "ultrasound.h"
#include "motorboard.h"
#include "sysdelay.h"
//#include "tools.h"

#define myphone "mix3"
#define phonePasswork "88888888"

#define computer "PING1"
#define computerPasswork "wjpwjp1748"

static TaskHandle_t app_task1_handle = NULL;
static TaskHandle_t app_task2_handle = NULL;
static TaskHandle_t app_task3_handle = NULL;

/* 任务1 */ 
static void app_task1(void* pvParameters);

/* 任务2 */  
static void app_task2(void* pvParameters);

/* 任务3 */  
static void app_task3(void* pvParameters);

int i = 0;
static int rt;
uint8_t buf[ 64 ];
char* p = NULL;
static uint32_t led_num = 0;
static uint32_t led_sta = 0;
static uint32_t motor_num = 0;
volatile static uint32_t motor_in1 = 0;
volatile static uint32_t motor_in2 = 0;
volatile static int distance_1 = 100;
volatile static int distance_2 = 100;


///* 事件标志组句柄 */
//EventGroupHandle_t EventGroupHandler;	

///* 互斥型信号量句柄 */
//static SemaphoreHandle_t MutexSemaphore;


void led_init( void )
{
    //打开端口E的硬件时钟，就是对端口E供电
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE, ENABLE );

	//打开端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE );


	//配置GPIOF的第9 10根
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	//初始化
	GPIO_Init( GPIOF, &GPIO_InitStructure );

	//配置GPIOE的第13 14根
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	//初始化
	GPIO_Init( GPIOE, &GPIO_InitStructure );
}

void io_init( void )
{
    /* PC8(STBY)    A: PC6(PWM),PD6,PD7    B: PA6(PWM),PE5,PC11 */
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE );
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE );
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE );
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE, ENABLE );
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOG, ENABLE );
    
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init( GPIOB, &GPIO_InitStructure );
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_7 | GPIO_Pin_9;
    GPIO_Init( GPIOC, &GPIO_InitStructure );
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init( GPIOD, &GPIO_InitStructure );
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_Init( GPIOE, &GPIO_InitStructure );
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init( GPIOG, &GPIO_InitStructure );
    
}

void wifi_connect( void )
{
        //串口1初始化波特率为115200bps
	usart1_init( 115200 );

	//发送数据
	printf( "This is esp8266 wifi test\r\n" );

	//esp8266初始化
	esp8266_init();

	//退出透传模式，才能输入AT指令
	rt = esp8266_exit_transparent_transmission();
	if ( rt == 0 )
		printf( "esp8266_exit_transparent_transmission success\r\n" );

	sys_delay_ms( 2000 );

	//断开服务器
	rt = esp8266_disconnect_server();
	if ( rt == 0 )
		printf( "esp8266_disconnect_server success\r\n" );
	else
		printf( "esp8266 is no connection to the server\r\n" );


	//重要操作之间要加延时，否则下一条AT指令有问题
	sys_delay_ms( 2000 );

	//进行自检测试
	while ( esp8266_self_test() )
	{
		printf( "esp8266 self testing ...\r\n" );
		sys_delay_ms( 500 );
	}


	//重要操作之间要加延时，否则下一条AT指令有问题
	sys_delay_ms( 2000 );

	//连接热点
	rt = esp8266_connect_ap( computer, computerPasswork );
	if ( rt == 0 )
		printf( "esp8266_connect_ap success\r\n" );
	else
		printf( "esp8266_connect_ap fail\r\n" );


	//重要操作之间要加延时，否则下一条AT指令有问题
	sys_delay_ms( 2000 );

/* ESP8266 仅支持2.4GHz */    
	rt = esp8266_connect_server( "TCP", "192.168.8.54", 8888 );
	if ( rt == 0 )
		printf( "esp8266_connect_server success\r\n" );
	else
		printf( "esp8266_connect_server fail\r\n" );

	sys_delay_ms( 2000 );

	//进入透传模式
	rt = esp8266_entry_transparent_transmission();
	if ( rt == 0 )
		printf( "esp8266_entry_transparent_transmission success\r\n" );
	else
		printf( "esp8266_entry_transparent_transmission fail\r\n" );
                              

	//发送欢迎信息到esp8266	
	strcpy( ( char* ) buf, "Bullshit esp8266! ! ! \r\n" );
    BEEP(1);
    sys_delay_ms(10);

	esp8266_send_bytes( buf, sizeof buf );
    BEEP(0);

}






int main( void )
{
    /* 设置系统中断优先级分组4 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* 系统定时器中断频率为configTICK_RATE_HZ */
	SysTick_Config(SystemCoreClock / configTICK_RATE_HZ);
    
    
    led_init();
    beep_init();
    io_init();
    
    Ultrasound_init();
    motor_board_init( true );
    wifi_connect();
    

    
    PFout( 9 ) = PFout( 10 ) = 1;
	PEout( 13 ) = PEout( 14 ) = 1;
    
    /* 创建app_task1任务 */
	xTaskCreate((TaskFunction_t )app_task1,  		/* 任务入口函数 */
			  (const char*    )"app_task1",			/* 任务名字 */
			  (uint16_t       )512,  				/* 任务栈大小，512字=2048字节 */
			  (void*          )NULL,				/* 任务入口函数参数 */
			  (UBaseType_t    )4, 					/* 任务的优先级 */
			  (TaskHandle_t*  )&app_task1_handle);	/* 任务控制块指针 */ 
#if 1	
	/* 创建app_task2任务 */		  
	xTaskCreate((TaskFunction_t )app_task2,  		/* 任务入口函数 */
			  (const char*    )"app_task2",			/* 任务名字 */
			  (uint16_t       )512,  				/* 任务栈大小，512字=2048字节 */
			  (void*          )NULL,				/* 任务入口函数参数 */
			  (UBaseType_t    )4, 					/* 任务的优先级 */
			  (TaskHandle_t*  )&app_task2_handle);	/* 任务控制块指针 */ 
#endif
              
#if 1
    xTaskCreate((TaskFunction_t )app_task3,  		/* 任务入口函数 */
			  (const char*    )"app_task3",			/* 任务名字 */
			  (uint16_t       )512,  				/* 任务栈大小，512字=2048字节 */
			  (void*          )NULL,				/* 任务入口函数参数 */
			  (UBaseType_t    )3, 					/* 任务的优先级 */
			  (TaskHandle_t*  )&app_task3_handle);	/* 任务控制块指针 */ 
#endif       

	
              
    /* 开启任务调度 */
	vTaskStartScheduler(); 
  
          
	while(1);
}

static void app_task1(void* pvParameters)
{
	for(;;)
	{
        PFout( 10 ) = ~PFout( 10 );
		vTaskDelay(200);
	}
}

static void app_task2(void* pvParameters)
{
	for(;;)
	{
        
        printf("task2\r\n");
        distance_1 = get_distance_A();
        distance_2 = get_distance_B();


        if( -1 == distance_1 || -1 == distance_2)
            goto it;
        if( 66 > distance_2 && motor_in1 == 1 && motor_in2 == 0)
        {
            motor_board_stop( true );
            vTaskDelay(200);
            motor_board_stop( false );
            printf(" 100, 1-> %d, 2-> %d\r\n", distance_1, distance_2);
            PEout( 14 ) = 0;    PEout( 13 ) = 1;
            PFout( 10 ) = 1;    PFout( 9 ) = 1;
        }
        if( 66 > distance_1 && motor_in1 == 0 && motor_in2 == 1)
        {
            motor_board_stop( true );
            vTaskDelay(200);
            motor_board_stop( false );
            printf(" 100, 1-> %d, 2-> %d\r\n", distance_1, distance_2);
            PEout( 14 ) = 0;    PEout( 13 ) = 1;
            PFout( 10 ) = 1;    PFout( 9 ) = 1;
        }
        it:
        vTaskDelay(10);
	}
}

static void app_task3(void* pvParameters)
{
	for(;;)
	{
        
        
        if ( g_esp8266_rx_end && g_esp8266_transparent_transmission_sta )
		{
			if ( strstr( ( const char* ) g_esp8266_rx_buf, "led" ) )
			{
				strtok( ( char* ) g_esp8266_rx_buf, "-" );

				p = strtok( NULL, "-" );    led_num = atoi( p );/* 分离第一个'-'跟随的字符串 */

				p = strtok( NULL, "-" );    led_sta = atoi( p );/* 分离第二个'-'跟随的字符串 */

				if ( led_num == 1 )PFout( 9 ) = !led_sta;
				if ( led_num == 2 )PFout( 10 ) = !led_sta;
				if ( led_num == 3 )PEout( 13 ) = !led_sta;
				if ( led_num == 4 )PEout( 14 ) = !led_sta;
				esp8266_send_str( "led set success\r\n" );
			}

            if ( strstr( ( const char* ) g_esp8266_rx_buf, "motor" ) )
            {
                strtok( ( char* ) g_esp8266_rx_buf, "-" );
                
                p = strtok( NULL, "-" );    motor_num = atoi( p );/* 分离第一个'-'跟随的字符串 */
                
                p = strtok( NULL, "-" );    motor_in1 = atoi( p );/* 分离第二个'-'跟随的字符串 */
                
                p = strtok( NULL, "-" );    motor_in2 = atoi( p );/* 分离第三个'-'跟随的字符串 */
                
                if( motor_num == 1 ){PDout( 6 ) = motor_in1;  PDout( 7 ) = motor_in2;}
                if( motor_num == 2 ){PCout( 11 ) = motor_in1;  PEout( 5 ) = motor_in2;}
                if( motor_num == 3 ){PAout( 4 ) = motor_in1;  PGout( 15 ) = motor_in2;}
                if( motor_num == 4 ){PCout( 9 ) = motor_in1;  PBout( 6 ) = motor_in2;}
                esp8266_send_str( "motor set success\r\n" );
            }
            
            if ( strstr( ( const char* ) g_esp8266_rx_buf, "car" ) )
            {
                strtok( ( char* ) g_esp8266_rx_buf, "-" );
                
                p = strtok( NULL, "-" );    motor_in1 = atoi( p );/* 分离第一个'-'跟随的字符串 */
                
                p = strtok( NULL, "-" );    motor_in2 = atoi( p );/* 分离第二个'-'跟随的字符串 */
                PEout( 14 ) = 1;    PEout( 13 ) = 1;    PFout( 10 ) = 1;    PFout( 9 ) = 1;
                PDout( 6 ) = motor_in1;  PDout( 7 ) = motor_in2;    PCout( 11 ) = motor_in1;    PEout( 5 ) = motor_in2;
                PGout( 15 ) = motor_in2;  PAout( 4 ) = motor_in1;   PBout( 6 ) = motor_in2; PCout( 9 ) = motor_in1;
                if( motor_in1 == 1 && motor_in2 == 0){PFout( 9 ) = 0;PFout( 10 ) = 1;}
                if( motor_in1 == 0 && motor_in2 == 1){PFout( 10 ) = 0;PFout( 9 ) = 1;}
                if( motor_in1 == 1 && motor_in2 == 1){PFout( 10 ) = 0;PFout( 9 ) = 0;}
                if( motor_in1 == 0 && motor_in2 == 0){PEout( 14 ) = 0;PFout( 10 ) = 1;PFout( 9 ) = 1;}
                
                esp8266_send_str( "motor set success\r\n" );
            }
			//清空接收缓冲区、接收计数值、接收结束标志位
			memset( ( void* ) g_esp8266_rx_buf, 0, sizeof g_esp8266_rx_buf );
			g_esp8266_rx_cnt = 0;
			g_esp8266_rx_end = 0;

		}
        vTaskDelay(100);
	}
}

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

void vApplicationTickHook( void )
{

}
