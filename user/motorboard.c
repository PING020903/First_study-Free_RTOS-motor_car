#include "motorboard.h"
void motor_board_init( bool i )
{
    /* board_1 */
    PCout( 6 ) = 1;//PWM_A
    PAout( 6 ) = 1;//PWM_B
    PCout( 8 ) = i;//STBY
    
    /* board_2 */
    PBout( 7 ) = 1;//PWM_A
    PEout( 6 ) = 1;//PWM_B
    PCout( 7 ) = i;//STBY
}
void motor_board_stop( bool i )
{
    /* IN1=1,IN2=1 制动 */
    /* IN1=0,IN2=0 断电 */
    
    /* board_1 */
    PDout( 7 ) = i;//AIN_1
    PDout( 6 ) = i;//AIN_2    
    PCout( 11 ) = i;//BIN_1
    PEout( 5 ) = i;//BIN_2
    
    /* board_2 */
    PAout( 4 ) = i;//AIN_1
    PGout( 15 ) = i;//AIN_2
    PCout( 9 ) = i;//BIN_1
    PBout( 6 ) = i;//BIN_2
}

void motor_board1( void )
{
    PDout( 6 ) = 1;//AIN_1
    PDout( 7 ) = 0;//AIN_2
    PCout( 11 ) = 0;//BIN_1
    PEout( 5 ) = 1;//BIN_2
}

void motor_board2( void )
{
    PAout( 4 ) = 1;//AIN_1
    PGout( 15 ) = 0;//AIN_2
    PCout( 9 ) = 0;//BIN_1
    PBout( 6 ) = 1;//BIN_2
}
