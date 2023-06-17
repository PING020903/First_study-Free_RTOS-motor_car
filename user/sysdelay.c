#include "sysdelay.h"



void sys_delay_us( uint32_t n )
{
	SysTick->CTRL = 0; // Disable SysTick
	SysTick->LOAD = n * 168 - 1; // 计数值 
	SysTick->VAL = 0; // Clear current value as well as count flag
	SysTick->CTRL = 5; // Enable SysTick timer with processor clock(168MHz)
	while ( (SysTick->CTRL & 0x00010000) == 0 );// Wait until count flag is set
	SysTick->CTRL = 0; // Disable SysTick	
}



void sys_delay_ms( uint32_t n )
{
	while ( n-- )
	{
		SysTick->CTRL = 0; // Disable SysTick
		SysTick->LOAD = 168000 - 1; // 计数值 Count from n to 0 (168000-1+1=168000 cycles)
		SysTick->VAL = 0; // Clear current value as well as count flag
		SysTick->CTRL = 5; // Enable SysTick timer with processor clock(168MHz)
		while ( (SysTick->CTRL & 0x00010000) == 0 );// Wait until count flag is set


	}

	SysTick->CTRL = 0; // Disable SysTick	
}
