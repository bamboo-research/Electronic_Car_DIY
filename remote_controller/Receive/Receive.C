#include <STC12C5A60S2.h> 
#include <CH375INC.H>

unsigned char CMD=0xff;
sbit  CH375_INT_WIRE = P3^2;			//P3.2, INT0, ����CH375��INT#����,���ڲ�ѯ�ж�״̬ */
unsigned char idata buf[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char idata Recbuf[6]={0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char Rec_count=0; 
typedef	union _REQUEST_PACK{
	unsigned char  buffer[8];
	struct{
		unsigned char	 bmReuestType;    	//��׼������
		unsigned char	 bRequest;		   	//�������
		unsigned int     wValue;			//����ѡ���
		unsigned int     wIndx;				//����
		unsigned int     wLength;				//���ݳ���
	}r;
} mREQUEST_PACKET,	*mpREQUEST_PACKET;

//�豸������
unsigned char  code DevDes[]={
	0x12			//��������С			
, 	0x01			//����DEVICE
, 	0x10			//USB�淶�汾��Ϣ
, 	0x01
,   0x00			//����룬
,  	0x00			//�������	
,   0x00			//Э����
,  	0x08			//�˵�0�������Ϣ����С
,  	0x3c			//����ID
,   0x41
,   0x03			//��ƷID	
,   0x20
,   0x00			//�豸�汾��Ϣ
,   0x02
,   0x01			//����ֵ	
,   0x02
,   0x00
,   0x01			//�������õ���Ŀ	
};
//����������
unsigned char   code ConDes[]={			//����������
			   		0x09, 0x02, 0x22, 0x00, 0x01, 0x01, 0x04, 0xa0,  0x23,//����������
		       	0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x05,//�ӿ������� 
			   		0x09, 0x21, 0x10, 0x01, 0x00, 0x01, 0x22, 0x41, 0x00,//HID��������
		       	0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0x18   //�˵���������ֻ����������˵㣬����˵���0�˵�
				};		//����������

/*����������*/
unsigned char code Hid_des[]={
0x05, 0x01, 0x09, 0x06,  0xa1, 0x01, 0x05, 0x07,  0x19, 0xe0, 
0x29, 0xe7, 0x15, 0x00,  0x25, 0x01, 0x75, 0x01,  0x95, 0x08, 
0x81, 0x02, 0x95, 0x01,  0x75, 0x08, 0x81, 0x01,  0x95, 0x03, 0x75, 0x01,        
0x05, 0x08, 0x19, 0x01,  0x29, 0x03, 0x91, 0x02,  0x95, 0x01, 0x75, 0x05,  
0x91, 0x01, 0x95, 0x06,  0x75, 0x08, 0x15, 0x00,  0x26, 0xff, 0x00, 0x05,
0x07, 0x19, 0x00, 0x2a,  0xff, 0x00, 0x81, 0x00,  0xc0
};
/*����ȫ�ֱ����Ķ���*/
unsigned char mVarSetupRequest;						//	;USB������
unsigned char mVarSetupLength;						//	�������ݳ���
unsigned char  code * VarSetupDescr;				//������ƫ�Ƶ�ַ
unsigned char VarUsbAddress	;					 	
unsigned char idata UPDATA_FLAG;
bit CH375FLAGERR;									//������0
bit	CH375CONFLAG;

mREQUEST_PACKET  request;

/*Ӳ�����壬����Ӳ���޸�*/
unsigned char volatile xdata CH375_CMD_PORT _at_ 0x81ff;		/* CH375����˿ڵ�I/O��ַ */
unsigned char volatile xdata CH375_DAT_PORT _at_ 0x80ff;		/* CH375���ݶ˿ڵ�I/O��ַ */


/* ��ʱ2΢��,����ȷ */
void Delay1us()
{
	;
}

void	Delay2us( )
{
	unsigned char i;
#define DELAY_START_VALUE	2  								/* ���ݵ�Ƭ����ʱ��ѡ���ֵ,20MHz����Ϊ0,30MHz����Ϊ2 */
	for ( i=DELAY_START_VALUE; i!=0; i-- );
}

void Delay_ms(unsigned char ms)
{
    unsigned char i;
    unsigned char us;
    for(i=0; i<ms; i++)
    {
        us=250;
        while(us--);
    }
}

void CH375_WR_CMD_PORT( unsigned char cmd ) 
{ 				 /* ��CH375������˿�д������,���ڲ�С��4uS,�����Ƭ���Ͽ�����ʱ */
	CH375_CMD_PORT=cmd;
	Delay2us( );
}

void CH375_WR_DAT_PORT( unsigned char dat ) 
{ 				 /* ��CH375�����ݶ˿�д������,���ڲ�С��1.5uS,�����Ƭ���Ͽ�����ʱ */
	CH375_DAT_PORT=dat;
	Delay1us();  											/* ��ΪMCS51��Ƭ����������ʵ����������ʱ */
}

unsigned char CH375_RD_DAT_PORT() 
{  						/* ��CH375�����ݶ˿ڶ�������,���ڲ�С��1.5uS,�����Ƭ���Ͽ�����ʱ */
	Delay1us( );  										/* ��ΪMCS51��Ƭ����������ʵ����������ʱ */
	return( CH375_DAT_PORT );

}
/* CH375��ʼ���ӳ��� */
void	CH375_Init( )
{
/* ����USB����ģʽ, ��Ҫ���� */
	CH375_WR_CMD_PORT( CMD_SET_USB_MODE );
	CH375_WR_DAT_PORT( 1 );  									/* ����Ϊʹ�����ù̼���USB�豸��ʽ */
	for ( ;; ) 
	{  											/* �ȴ������ɹ�,ͨ����Ҫ�ȴ�10uS-20uS */
		if ( CH375_RD_DAT_PORT( )==CMD_RET_SUCCESS ) break;
	}
}

//�˵�0�����ϴ�
void mCh375Ep0Up()
{
	unsigned char i,len;
	if(mVarSetupLength)
	{												//���Ȳ�Ϊ0������峤�ȵ�����
		if(mVarSetupLength<=8)
		{
			len=mVarSetupLength;
			mVarSetupLength=0;
        }	//����С��8����Ҫ��ĳ���
		else
		{
			len=8;
			mVarSetupLength-=8;
		}							                        		//���ȴ���8����8�������ܳ��ȼ�8
	    CH375_WR_CMD_PORT(CMD_WR_USB_DATA3);						//����д�˵�0������
       	CH375_WR_DAT_PORT(len);										//д�볤��
    	for(i=0;i!=len;i++)
        CH375_WR_DAT_PORT(request.buffer[i]);	              		//ѭ��д������
    }
	else
	{
		CH375_WR_CMD_PORT(CMD_WR_USB_DATA3);						//����д�˵�0������
		CH375_WR_DAT_PORT(0);					                   //�ϴ�0�������ݣ�����һ��״̬�׶�
	}
}

//�����������Ա��ϴ�
void mCh375DesUp()
{
	unsigned char k;        
	for (k=0; k!=8; k++ ) 
	{
         request.buffer[k]=*VarSetupDescr;  								//���θ���8����������
         VarSetupDescr++;
    }
}

/*�жϺ��������Ǳ����õ��ǲ�ѯ��ʽ��ֻ����������˵�1�Ͷ˵�0���¼������಻����*/
void	mCH375Interrupt( )
{
	unsigned char InterruptStatus;
	unsigned char length, c1, len;
	
	CH375_WR_CMD_PORT(CMD_GET_STATUS);  									/* ��ȡ�ж�״̬��ȡ���ж����� */
	InterruptStatus =CH375_RD_DAT_PORT();  									/* ��ȡ�ж�״̬ */
	switch(InterruptStatus){  // �����ж�״̬
		case   USB_INT_EP1_OUT:	  											//�ж϶˵��´��ɹ���δ����
			CH375_WR_CMD_PORT(CMD_RD_USB_DATA);									//��������������
			if(	length=CH375_RD_DAT_PORT( )	)
			{										//����Ϊ0����
				for(len=0;len!=length;len++)c1=CH375_RD_DAT_PORT();					//ȡ���´�����
			}
			break;
		case   USB_INT_EP0_SETUP: 											//���ƶ˵㽨���ɹ�
	    	CH375_WR_CMD_PORT(CMD_RD_USB_DATA);
			length=CH375_RD_DAT_PORT();
			for(len=0;len!=length;len++)request.buffer[len]=CH375_RD_DAT_PORT();  // ȡ������
			if(length==0x08){
			    mVarSetupLength=request.buffer[6];							//���ƴ������ݳ����������Ϊ128
				if((c1=request.r.bmReuestType)&0x40){         					 //��������δ����
				}
				if((c1=request.r.bmReuestType)&0x20)
				{          					//������δ����
					if(request.buffer[1]==0x0a)
					{
					}												//SET_IDLE
					else if(request.buffer[1]==0x09)
					{
							UPDATA_FLAG=1;
					}
				}
				if(!((c1=request.r.bmReuestType)&0x60))
				{          				//��׼����
					mVarSetupRequest=request.r.bRequest;							//�ݴ��׼������
					switch(request.r.bRequest){  // ������׼����
						case DEF_USB_CLR_FEATURE:									//�������
							if((c1=request.r.bmReuestType&0x1F)==0X02)
							{					//���Ƕ˵㲻֧��
								switch(request.buffer[4])
								{
									case 0x82:
										CH375_WR_CMD_PORT(CMD_SET_ENDP7);					//����˵�2�ϴ�
										CH375_WR_DAT_PORT(0x8E);                			//����������˵�
										break;
									case 0x02:
										CH375_WR_CMD_PORT(CMD_SET_ENDP6);
										CH375_WR_DAT_PORT(0x80);							//����˵�2�´�
										break;
									case 0x81:
										CH375_WR_CMD_PORT(CMD_SET_ENDP5);					//����˵�1�ϴ�
										CH375_WR_DAT_PORT(0x8E);
										break;
									case 0x01:
										CH375_WR_CMD_PORT(CMD_SET_ENDP4);					//����˵�1�´�
										CH375_WR_DAT_PORT(0x80);
										break;
									default:
										break;
								}
							}
							else
							{
								CH375FLAGERR=1;								    //��֧�ֵ�������ԣ��ô����־
							}
							break;
						case DEF_USB_GET_STATUS:								//���״̬
							request.buffer[0]=0;
							request.buffer[1]=0;								//�ϴ�״̬
							break;
						case DEF_USB_SET_ADDRESS:								//���õ�ַ
							VarUsbAddress=request.buffer[2];					//�ݴ�USB���������ĵ�ַ
							break;
						case DEF_USB_GET_DESCR: 								//���������
							if(request.buffer[3]==1)							//�豸�������ϴ�
								VarSetupDescr=DevDes;
							else if(request.buffer[3]==2)		 					//�����������ϴ�
								VarSetupDescr=ConDes;
							else if(request.buffer[3]==0x22) 
							{
								VarSetupDescr=Hid_des;
							}
								mCh375DesUp();											//������������֧��					          							
							break;
						case DEF_USB_GET_CONFIG:									//�������
							request.buffer[0]=0;									//û��������0
							if(CH375CONFLAG) request.buffer[0]=1;									//�Ѿ�������1����������������涨��
							break;
						case DEF_USB_SET_CONFIG:                 					//��������
							CH375CONFLAG=0;
							if ( request.buffer[2] != 0 ) 
							{
								CH375CONFLAG=1;											//�������ñ�־									
							}
							break;
						case DEF_USB_GET_INTERF:										//�õ��ӿ�
							request.buffer[0]=1;									//�ϴ��ӿ�����������ֻ֧��һ���ӿ�
							break;
						default :
							CH375FLAGERR=1;											//��֧�ֵı�׼����
							break;
					}
				}
			}
			else 
			{  //��֧�ֵĿ��ƴ��䣬����8�ֽڵĿ��ƴ���
				CH375FLAGERR=1;
			}
			if(!CH375FLAGERR) mCh375Ep0Up();										//û�д���/���������ϴ���������Ϊ0�ϴ�Ϊ״̬
			else 
			{
				CH375_WR_CMD_PORT(CMD_SET_ENDP3);								//���ö˵�1ΪSTALL��ָʾһ������
				CH375_WR_DAT_PORT(0x0F);
			}
			break;
		case   USB_INT_EP0_IN:													//���ƶ˵��ϴ��ɹ�
			if(mVarSetupRequest==DEF_USB_GET_DESCR)
			{								//�������ϴ�
				mCh375DesUp();
				mCh375Ep0Up();															
			}
			else if(mVarSetupRequest==DEF_USB_SET_ADDRESS)
			{							//���õ�ַ
				CH375_WR_CMD_PORT(CMD_SET_USB_ADDR);
				CH375_WR_DAT_PORT(VarUsbAddress);								//����USB��ַ,�����´������USB��ַ
			}
			CH375_WR_CMD_PORT (CMD_UNLOCK_USB);								//�ͷŻ�����
			break;
		case   USB_INT_EP0_OUT:													//���ƶ˵��´��ɹ�
			   CH375_WR_CMD_PORT (CMD_UNLOCK_USB);	
			/*CH375_WR_CMD_PORT(CMD_RD_USB_DATA);									//��������������
			if(length=CH375_RD_DAT_PORT())		 //�Ա�������˵��γ���û���ã����������������������������ݵ�������Ҫ��������
			{										//����Ϊ0����
				for(len=0;len!=length;len++){buf[len]=CH375_RD_DAT_PORT();					//ȡ���´�����
		    }
			}*/
			break;
		default:
			if( (InterruptStatus&0x03)==0x03 )
			{									//���߸�λ
				CH375FLAGERR=0;													//������0
				CH375CONFLAG=0;													//������0
				mVarSetupLength=0;													//������������
			}
			else
			{																//���֧��
				;
			}
			CH375_WR_CMD_PORT (CMD_UNLOCK_USB);									//�ͷŻ�����
			break;
	}
}

void UartInit(void)		//9600bps@11.0592MHz
{
	EA=1;
	ES=1;
	SCON = 0x50;		//8λ����,�ɱ䲨����
	TMOD &= 0x0F;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xFD;		//�趨��ʱ��ֵ
	TH1 = 0xFD;		//�趨��ʱ����װֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
}

main( ) 
{
	unsigned char i;
	Delay_ms(300);	
	CH375_Init( );  /* ��ʼ��CH376 */
 	UartInit();   

	while(1)
	{		

		if(CH375_INT_WIRE == 0)
		{
			mCH375Interrupt( );
			
			if(UPDATA_FLAG==0x01)
			{ 	
				ES=0;
				for(i=0;i!=8;i++)buf[i+2]=Recbuf[i];		//�����յ������ݸ�ֵ������buf
				CH375_WR_CMD_PORT(CMD_WR_USB_DATA5);	
				CH375_WR_DAT_PORT(8);
				for(i=0;i!=8;i++)CH375_WR_DAT_PORT(buf[i]);	//����buf�ڵ����ݷ��͵�USB
				ES=1;					
			}
		}
	}

}
void uart_receive(void) interrupt 4
{
	unsigned char ch;
	ES=0;
	if(RI)
	{
		ch=SBUF;
		Recbuf[Rec_count]=ch;			
		Rec_count++;					
		if(Rec_count>4)Rec_count=0;
	}
	RI=0;
	ES=1;
}