#include "STC12C5A60S2.h" 
#include "intrins.h"
#define key1  0x1d    //黑色     刹车 Z
#define key2  0xE1    //黄色     left shift
#define key3  0x04    //红色     油门    A
#define key4  0x4f    //右          右转    →
#define key5  0x50    //左          左转    ←
sbit SCL=P0^6;      //IIC时钟引脚定义
sbit SDA=P0^7;      //IIC数据引脚定义
unsigned char idata BUF[6]; //接收数据缓存区  X轴高八位，X轴低八位； Y轴高八位，Y轴低八位； Z轴高八位，Z轴第低八位 
unsigned char idata key_buf[5]={0x00,0x00,0x00,0x00,0x00};
unsigned char code key_judge_tab[]={0x01,0x02,0x04,0x08,0x10}; //按键引脚
void UartInit(void);
void Send();
void Clear();
void Input_Data_Process();

void Init_ADXL345(void);             //初始化ADXL345
void  Single_Write_ADXL345(unsigned char REG_Address,unsigned char REG_data);   //单个写入数据
void  Multiple_Read_ADXL345();                                  //连续的读取内部寄存器数据
void ADXL345_Start();
void ADXL345_Stop();
void ADXL345_SendACK(bit ack);
bit  ADXL345_RecvACK();
void ADXL345_SendByte(unsigned char dat);
unsigned char ADXL345_RecvByte();
void ADXL345_Data_Process();

void Delay5us();
void Delay5ms();
void Delay50ms();
void Delay150ms();

signed int idata X_data; 
unsigned char Data,judge;
bit key_flag=0,flag;

void main()
{
    Delay150ms();
    P0M1=0x00;
    P0M0=0x00;
    UartInit();
    Init_ADXL345(); 
    while(1)
    {   

        Data=~P2&0x1f;               //读取按键状态
        
        Multiple_Read_ADXL345();     //读取ADXL345数据
        ADXL345_Data_Process();      //ADXL345数据分析、处理
        Input_Data_Process();        //按键数据分析、处理
        Send();                      //发送键盘代码
        Clear();                     //清空缓存数据
        Delay50ms();    
    }
}
void ADXL345_Data_Process()                   
{
    X_data=BUF[1]<<8|BUF[0];
    if(X_data<-50)Data=Data|0x10;
    if(X_data>50) Data=Data|0x08;
}
void Input_Data_Process()
{
    unsigned char i;
    for(i=0;i<5;i++)
    {
        judge=Data&key_judge_tab[i];
        switch (judge)
        {
            case 0x00: break;
            case 0x01: key_buf[0]=key1;break;
            case 0x02: key_buf[1]=key2;break;
            case 0x04: key_buf[2]=key3;break;
            case 0x08: key_buf[3]=key4;break;
            case 0x10: key_buf[4]=key5;break;
            default:break;
        }
    }   
}
void Multiple_read_ADXL345()
{   
    unsigned char i;
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(0xA6);           //发送设备地址+写信号
    ADXL345_SendByte(0x32);                   //发送存储单元地址，从0x32开始    
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(0xA7);         //发送设备地址+读信号
    for (i=0; i<6; i++){                      //连续读取6个地址数据，存储中BUF
        BUF[i] = ADXL345_RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 5)ADXL345_SendACK(1);//最后一个数据需要回NOACK 
        else ADXL345_SendACK(0); //回应ACK
    }
    ADXL345_Stop();                          //停止信号
    Delay5ms();
}
void Init_ADXL345()
{
   Single_Write_ADXL345(0x2C,0x08);   //速率设定为12.5 
   Single_Write_ADXL345(0x2D,0x08);   //选择电源模式   
   Single_Write_ADXL345(0x31,0x0B);   //测量范围,正负16g，13位模式
}
void Single_Write_ADXL345(unsigned char REG_Address,unsigned char REG_data)
{
    ADXL345_Start();                  //起始信号
    ADXL345_SendByte(0xA6);   //发送设备地址
    ADXL345_SendByte(REG_Address);    //内部寄存器地址
    ADXL345_SendByte(REG_data);       //内部寄存器数据 
    ADXL345_Stop();                   //发送停止信号
}
void ADXL345_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
}
void ADXL345_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}
void ADXL345_SendACK(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}
bit ADXL345_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    flag = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时

    return flag;
}
void ADXL345_SendByte(unsigned char dat)
{
    unsigned char i;

    for (i=0; i<8; i++)         //8位计数器
    {
        SDA=dat&0x80;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
        dat <<= 1;
    }
    ADXL345_RecvACK();
}
unsigned char ADXL345_RecvByte()
{
    unsigned char i;
    unsigned char dat = 0;
    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}
void Send()             
{
    unsigned char i;
    for(i=0;i<5;i++)
    {
        SBUF=key_buf[i];
        while(!TI);
        TI=0;
    }
}
void Clear()
{
    unsigned char i;
    for(i=0;i<5;i++)
    {
        key_buf[i]=0x00;
    }
}


void UartInit(void)     //9600bps@11.0592MHz
{
    EA=1;
    PCON &= 0x7F;       //波特率不倍速
    SCON = 0x50;        //8位数据,可变波特率
    AUXR |= 0x04;       //独立波特率发生器时钟为Fosc,即1T
    BRT = 0xDC;     //设定独立波特率发生器重装值
    AUXR |= 0x01;       //串口1选择独立波特率发生器为波特率发生器
    AUXR |= 0x10;       //启动独立波特率发生器
}



void Delay5us()     //@11.0592MHz
{
    unsigned char i;

    _nop_();
    _nop_();
    _nop_();
    i = 10;
    while (--i);
}

void Delay5ms()     //@11.0592MHz
{
    unsigned char i, j;

    _nop_();
    _nop_();
    i = 54;
    j = 198;
    do
    {
        while (--j);
    } while (--i);
}
void Delay50ms()        //@11.0592MHz
{
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 3;
    j = 26;
    k = 223;
    do
    {
        do
        {
            while (--k);
        } while (--j);
    } while (--i);
}

void Delay150ms()       //@11.0592MHz
{
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 7;
    j = 78;
    k = 167;
    do
    {
        do
        {
            while (--k);
        } while (--j);
    } while (--i);
}

