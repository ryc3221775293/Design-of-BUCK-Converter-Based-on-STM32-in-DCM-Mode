#include "pid.h"
/************************************************
 DCM模式下的BUCK变换器数字系统控制
 博客：https://blog.csdn.net/tichimi3375
 作者：唯恋殊雨
************************************************/


PID V_PID;
void PID_init(void)
{	
  	V_PID.setpulse = 0 ;	      	//电压设定值
  	V_PID.backpulse = 0 ;			//电压反馈值		
  	V_PID.last_error = 0 ;	 
  	V_PID.pre_error = 0 ;	  
  	V_PID.P = Pv;
  	V_PID.I = Iv;
  	V_PID.D = Dv;	
  	V_PID.motorout = 0 ;		    //控制输出值
}

unsigned int V_PIDCalc( PID *pp )
{
    int error;
    
	error = pp->setpulse - pp->backpulse;
    //pp->motorout +=( int) (pp->P*error - pp->I*pp->last_error);
  	pp->motorout +=( int) (pp->P*(error-pp->last_error) + pp->I*error + pp->D*(error-2*pp->last_error+pp->pre_error));
 		
 	pp->pre_error = pp->last_error;	 
 	pp->last_error = error;
    	
    if( pp->motorout >= D_MAX) 		
		   pp->motorout = D_MAX;		
    else if( pp->motorout <= D_MIN)	//电压PID，防止调节最低溢出 
		   pp->motorout = D_MIN;			
  	
  	return (pp->motorout);			// 返回预调节占空比
}
