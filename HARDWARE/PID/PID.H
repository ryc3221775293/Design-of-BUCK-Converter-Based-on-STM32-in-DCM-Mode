#ifndef __PID_H
#define __PID_H

#include "delay.h"
#include "stm32f10x.h"

/************************************************
 DCM模式下的BUCK变换器数字系统控制
 博客：https://blog.csdn.net/tichimi3375
 作者：唯恋殊雨
************************************************/

#define initial_lue 1000   ////////霍尔传感器电流初始值 理论值2.5/3.3*4096=3103  

 
/////////////////PID调节////////////////////////////////////////////////////
#define  Pv  0.1  //0.6//0.1		     振荡 0.05	 振荡至稳定0.1		   
#define  Iv  0.5//0.08					      0.04			   0.03		   
#define  Dv  0


#define D_MAX 620
#define D_MIN 0 
//定义PID
typedef struct PID		
{
	int setpulse;		    //设定值
  	int backpulse;			//反馈值
	int last_error;  	
	int pre_error;
	//int u;		
	float P;		    	
	float I;		    
	float D;		    						
	int motorout;	  		//控制输出值	
}PID;

void PID_init(void);
unsigned int V_PIDCalc( PID *pp );		//voltage

#endif
