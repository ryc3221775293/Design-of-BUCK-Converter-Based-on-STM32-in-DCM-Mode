#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "timer.h"
#include "adc.h"
#include "pid.h"
#include <math.h> 
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/************************************************
 DCM模式下的BUCK变换器数字系统控制
 博客：https://blog.csdn.net/tichimi3375
 作者：唯恋殊雨
************************************************/

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define ADC_TASK_PRIO		2
//任务堆栈大小	
#define ADC_STK_SIZE 		256  
//任务句柄
TaskHandle_t ADCTask_Handler;
//任务函数
void adc_task(void *pvParameters);

//任务优先级
#define KEYPROCESS_TASK_PRIO 3
//任务堆栈大小	 
#define KEYPROCESS_STK_SIZE  256 
//任务句柄
TaskHandle_t Keyprocess_Handler;
//任务函数
void Keyprocess_task(void *pvParameters);

//任务优先级
#define PID_TASK_PRIO 4
//任务堆栈大小	 
#define PID_STK_SIZE  256 
//任务句柄
TaskHandle_t PID_Handler;
//任务函数
void pid_task(void *pvParameters);

#define KEYMSG_Q_NUM    4  		//按键消息队列的数量 
#define MESSAGE_Q_NUM   4   	//发送数据的消息队列的数量
QueueHandle_t Key_Queue;   		//按键值消息队列句柄
QueueHandle_t Message_Queue;	//信息队列句柄

//进入低功耗模式前需要处理的事情
//ulExpectedIdleTime：低功耗模式运行时间
void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
	//关闭某些低功耗模式下不使用的外设时钟，此处只是演示性代码
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,DISABLE);
}

//退出低功耗模式以后需要处理的事情
//ulExpectedIdleTime：低功耗模式运行时间
void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
	//退出低功耗模式以后打开那些被关闭的外设时钟，此处只是演示性代码
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);	              
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	 
	uart_init(115200);					//初始化串口
	LED_Init();
	PID_init();
	KEY_Init();
	TIM3_PWM_Init(899,0);	 //不分频。PWM频率=72000000/900=80Khz
	Adc_Init();		  		//ADC初始化	
	my_mem_init(SRAMIN);            	//初始化内部内存池
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
	//创建消息队列
	Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(u16));        //创建消息Key_Queue
    Message_Queue=xQueueCreate(MESSAGE_Q_NUM,sizeof(u16)); //创建消息Message_Queue,队列项长度是串口接收缓冲区长度
	
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )adc_task,             
                (const char*    )"adc_task",           
                (uint16_t       )ADC_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )ADC_TASK_PRIO,        
                (TaskHandle_t*  )&ADCTask_Handler);  
				
	//创建TASK2任务
    xTaskCreate((TaskFunction_t )Keyprocess_task,     
                (const char*    )"keyprocess_task",   
                (uint16_t       )KEYPROCESS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )KEYPROCESS_TASK_PRIO,
                (TaskHandle_t*  )&Keyprocess_Handler);
				
    //创建TASK3任务
    xTaskCreate((TaskFunction_t )pid_task,     
                (const char*    )"pid_task",   
                (uint16_t       )PID_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )PID_TASK_PRIO,
                (TaskHandle_t*  )&PID_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
//ADC任务函数
void adc_task(void *pvParameters)
{
	u16 adcx;
	BaseType_t err;
	while(1)
	{
		adcx=Get_Adc_Average(ADC_Channel_1,10);	
		if ((Message_Queue!=NULL) && (adcx))
		{
			err=xQueueSend(Message_Queue,&adcx,10);
            if(err==errQUEUE_FULL)   	//发送按键值
            {
                printf("队列Message_Queue已满，数据发送失败!\r\n");
            }
		}
		vTaskDelay(10);                           //延时10ms，也就是10个时钟节拍	
	}
}

//Keyprocess_task函数
void Keyprocess_task(void *pvParameters)
{
	u8 key;
	u16 pwm=0;
    BaseType_t err;
	while(1)
	{
		key=KEY_Scan(0);            	//扫描按键
		switch(key)
		{
			case WKUP_PRES:		//KEY_UP控制LED1
				pwm += 100;
				break;
			case KEY1_PRES:		//KEY1控制蜂鸣器
				pwm += 200;
				break;
			case KEY0_PRES:		//KEY0刷新LCD背景
				pwm -= 100;
				break;
		}
		if((Key_Queue!=NULL)&&(pwm!=0))   	//消息队列Key_Queue创建成功,并且按键被按下
        {
            err=xQueueSend(Key_Queue,&pwm,10);
            if(err==errQUEUE_FULL)   	//发送按键值
            {
                printf("队列Key_Queue已满，数据发送失败!\r\n");
            }
        }
		vTaskDelay(10);                           //延时10ms，也就是10个时钟节拍	
	}
	
}

//PID_task函数
void pid_task(void *pvParameters)
{
	u16 adcx,pwm;
	extern PID V_PID;
	while(1)
	{		
		if(Message_Queue!=NULL && Key_Queue!=NULL)
        {
            if(xQueueReceive(Message_Queue,&adcx,portMAX_DELAY) && xQueueReceive(Key_Queue,&pwm,portMAX_DELAY))//请求消息Message_Queue和Key_Queue
            {
				V_PID.setpulse = pwm;
				V_PID.backpulse =adcx;		//电压反馈值
				TIM_SetCompare2(TIM3,V_PIDCalc(&V_PID)+3600);
			}
		}
		vTaskDelay(10);      //延时10ms，也就是10个时钟节拍
	}
}

