#include "ultrasound.h"
#include "delay.h"


/*
*   "get_distance_A" and "get_distance_B", return "unsigned int", the value is in units of "mm"  
*/
void Ultrasound_init( void )
{
	/* 打开端口B E的硬件时钟（就是对硬件供电），默认状态下，所有时钟都是关闭 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//指定6号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//引脚工作在输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度越高，响应时间越短，但是功耗就越高，电磁干扰也越高
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//如果外部没有上拉电阻，就配置推挽输出模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不需要使能上下拉电阻
	GPIO_Init(GPIOA, &GPIO_InitStructure);//A-TRIG
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//B-TRIG
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//A-ECHO
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//B-ECHO

    
	/* 看时序图 */
	PBout(8) = 0;//B-TRIG
    PAout(2) = 0;//A-TRIG
}

int get_distance_A( void )
{
	int32_t t = 0;
	/* 发送10us的触发信号 */
	PBout(8) = 1;
	delay_us(10);
	PBout(8) = 0;

	/* 等待回响信号 */
	while(PBin(9) == 0)// B-ECHO
    {
        t++;
        delay_us(1);
        if( t == 1000 )// timeout
            return -1;
    }

    t = 0;
	/* 测量回响信号 */
	while( PBin(9) )
	{
		t++;
		delay_us(3);//每9us，声音传播3mm
	}	
	return (1 * t) / 2;
}

int get_distance_B( void )
{
	int32_t t = 0;
	/* 发送10us的触发信号 */
	PAout(2) = 1;
	delay_us(10);
	PAout(2) = 0;

	/* 等待回响信号 */
	while(PAin(3) == 0)// A-ECHO
    {
        t++;
        delay_us(1);
        if( t == 1000 )// timeout
            return -1;
    }    

	/* 测量回响信号 */
	while(PAin(3))
	{
		t++;
		delay_us(3);//每9us，声音传播3mm
	}	
	return (1 * t) / 2;
}
