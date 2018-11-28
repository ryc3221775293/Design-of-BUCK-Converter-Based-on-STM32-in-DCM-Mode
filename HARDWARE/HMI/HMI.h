#ifndef __HMI_H
#define __HMI_H	 
#include "sys.h"

/************************************************
 DCM模式下的BUCK变换器数字系统控制
 博客：https://blog.csdn.net/tichimi3375
 作者：唯恋殊雨
************************************************/

void CurveCommand(int ID, int channel, int value);
void NumberCommand(int ID,int num);


#endif

