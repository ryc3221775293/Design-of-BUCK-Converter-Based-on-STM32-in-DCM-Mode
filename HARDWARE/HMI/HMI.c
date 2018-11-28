#include "HMI.h"
#include "usart.h"
/************************************************
 DCM模式下的BUCK变换器数字系统控制
 博客：https://blog.csdn.net/tichimi3375
 作者：唯恋殊雨
************************************************/

void CurveCommand(int ID, int channel, int value)
{
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	printf("add %d,%d,%d",ID,channel,value);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	
}
	
void NumberCommand(int ID,int num)
{
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	printf("n%d.val=%d",ID,num);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0XFF);//向串口1发送数据	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束

}

