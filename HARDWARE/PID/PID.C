#include "pid.h"
/************************************************
 DCMģʽ�µ�BUCK�任������ϵͳ����
 ���ͣ�https://blog.csdn.net/tichimi3375
 ���ߣ�Ψ������
************************************************/


PID V_PID;
void PID_init(void)
{	
  	V_PID.setpulse = 0 ;	      	//��ѹ�趨ֵ
  	V_PID.backpulse = 0 ;			//��ѹ����ֵ		
  	V_PID.last_error = 0 ;	 
  	V_PID.pre_error = 0 ;	  
  	V_PID.P = Pv;
  	V_PID.I = Iv;
  	V_PID.D = Dv;	
  	V_PID.motorout = 0 ;		    //�������ֵ
}

unsigned int V_PIDCalc( PID *pp )
{
    int error;
    
	error = pp->setpulse - pp->backpulse;

  	pp->motorout +=( int) (pp->P*(error-pp->last_error) + pp->I*error + pp->D*(error-2*pp->last_error+pp->pre_error));
 		
 	pp->pre_error = pp->last_error;	 
 	pp->last_error = error;
    	
    if( pp->motorout >= D_MAX) 		
		   pp->motorout = D_MAX;		
    else if( pp->motorout <= D_MIN)	//��ѹPID����ֹ���������� 
		   pp->motorout = D_MIN;			
  	
  	return (pp->motorout);			// ����Ԥ����ռ�ձ�
}