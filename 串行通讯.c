#include <at89x51.h>
#include <intrins.h>

/*************************************************����3�������********************************************************/
unsigned char ledCondition=0;										//����8·LED����ʾЧ��
/**********************************************����4�͹���5�������****************************************************/
unsigned char PWM_brightnessControl_count=0;						//��¼�жϴ���
/*************************************************����6�������********************************************************/
bit direc_flag=0;             										//ռ�ձȸ��·���
double PWM_breathingLamp_count=0.0; 								//��¼�жϴ���
double PWM_VLAUE=5.0;    											//ռ�ձȱȶ�ֵ
unsigned int HUXI_COUNT=0;    										//ռ�ձȸ���ʱ��(��¼�жϴ���)
/*************************************************����7�������********************************************************/
unsigned char flag=0;
unsigned char temp=0;												//��ʱ����
unsigned char comdata=0;
unsigned char temperature_data_H=0,temperature_data_L=0;			//�¶ȸ�8λ���¶ȵ�8λ
unsigned char temperature_data_H_temp=0,temperature_data_L_temp=0;	//У���õ��¶ȸ�8λ���¶ȵ�8λ
unsigned char humidity_data_H=0,humidity_data_L=0;					//ʪ�ȸ�8λ��ʪ�ȵ�8λ
unsigned char humidity_data_H_temp=0,humidity_data_L_temp=0;		//У���õ�ʪ�ȸ�8λ��ʪ�ȵ�8λ
unsigned char check_data_temp=0;									//У���õ�У��8λ
/***********************************************���п�ͨ���������*****************************************************/
unsigned char receiveData;											//��Ŵ��ڽ��յ�������(��ģ��ʶ���ķ���ֵ)

//���ܣ���ʱ10΢��
void  Delay_10us(void)
{
	unsigned char i;
	i--;
	i--;
	i--;
	i--;
	i--;
	i--;
}

//���ܣ���ʱ0.1����
void Delay(unsigned int j)
{      
	unsigned char i;
	for(;j>0;j--)
	{ 	
		for(i=0;i<27;i++);
	}
}
				 
//���ڷ��ͺ���
void sendCharData(unsigned char Data)
{
	while(1)
	{
		if((RI==0)&&(TI==0))
		{
			SBUF=Data;
			break;
		}
   }
}

//��ʱ��T1�жϳ�ʼ������
void timer1_init(void)
{		
				                        			//��ʱ��T1Ϊ������ʽ2(�Զ����س�ֵ��8λ��ʱ��)
	TH1=0xfd;
	TL1=0xfd;										//������9600bps(11.0592Mhz����)
	TR1=1;											//������ʱ��T1,�����������ʷ�����					
	SCON=0x50;										//���崮�пڹ�����ʽ1,������9600bps,�����ڽ���
//	SM0=0;											//���崮�пڹ�����ʽ1
//	SM1=1;
//	REN=1; 											//�����ڽ���
	EA=1;  											//�����ж�
	ES=1;  											//�������ж�
}

//����1������LED�� �� ����2��Ϩ��LED��
void controlLed()
{	
	while(receiveData==0x01)						//ʶ���˺������ơ�,����ʶ��ģ���Ӵ������1,����LED��
	{		  
		P0=0x00; 									//����ȫ��LED��
	}
	while(receiveData==0x02)						//ʶ���˺����صơ�,����ʶ��ģ���Ӵ������2,Ϩ��LED��
	{			   
		P0=0xff;									//Ϩ��ȫ��LED��
	}
	P0=0xff;										//ָ��仯ʱ,Ϩ��ȫ��LED��	
}

//����3����ˮ��
void waterLamp(void)
{	
	unsigned char i;								   	
	while(receiveData==0x03)
	{				  
		ledCondition=0x7F;							//LED�Ƴ�ʼ����ʾЧ��Ϊ 0111 1111
		for(i=0;i<8;i++)
		{
			P0=ledCondition;
			Delay(1000);	  						//��ʱ0.1s	  
			ledCondition=_cror_(ledCondition,1); 	//ѭ������ Ч����0111 1111 -> 1011 1111
			if(receiveData!=0x03)
			{
				P0=0xff;							//ָ��仯ʱ,Ϩ��ȫ��LED��
				break;									
			}	
		}						  
	}
}			

//����4��LED�ƽϵ�������ʾ(һ������) �� ����5��LED�ƽϸ�������ʾ(��������)
void brightnessControl(void)
{
	TMOD=0x21;										//ά�ֶ�ʱ��T1Ϊ������ʽ1,���ö�ʱ��T0Ϊ������ʽ1(M1=0,M0=1)
	if(receiveData==0x04)
	{										
		TH0=0xff;									//�߰�λΪ(65536-10)/256
		TL0=0xf7;									//�Ͱ�λΪ(65536-10)%256,����ֵΪ10����������,��ʱԼ10.850684us
		EA=1;										//�����ж�
		ET0=1;										//����ʱ��T0�ж�
		TR0=1;										//������ʱ��T0
		while(receiveData==0x04);
		TR0=0;										//�رն�ʱ��T0
		PWM_brightnessControl_count=0;				//PWM_brightnessControl_count����
		P0=0xff;									//Ϩ��LED��
	}
	if(receiveData==0x05)
	{
		TH0=0xff;									//�߰�λΪ(65536-10)/256
		TL0=0xf7;									//�Ͱ�λΪ(65536-10)%256,����ֵΪ10����������,��ʱԼ10.850684us
		EA=1;										//�����ж�
		ET0=1;										//����ʱ��T0�ж�
		TR0=1;										//������ʱ��T0
		while(receiveData==0x05);					
		TR0=0;										//�رն�ʱ��T0
		PWM_brightnessControl_count=0;				//PWM_brightnessControl_count����
		P0=0xff;									//Ϩ��LED��
	}
}

//����6��������
void breathingLamp(void)
{
	TMOD=0x22;										//ά�ֶ�ʱ��T1Ϊ������ʽ1,���ö�ʱ��T0Ϊ������ʽ2(M1=1,M0=0)
	TH0=0x47;               						//��ʱ�����ֵ����,ÿ��200us����һ���ж�
	TL0=0X47;
  	TR0=1;                  						//��ʱ��0��ʼ��ʱ
 	ET0=1;                  						//����ʱ��0�ж�
 	EA=1;                       					//�����ж�
	while(receiveData==0x06);				
	TR0=0;											//�رն�ʱ��T0
	PWM_breathingLamp_count=0.0;					//��������
    HUXI_COUNT=0;
	P0=0xff;										//Ϩ��LED��
}

//comdata����
void COM(void)							
{
	unsigned char i;
    for(i=0;i<8;i++)	   
	{
		flag=2;	
	   	while((!P2_0)&&flag++);
		Delay_10us();
		Delay_10us();
		Delay_10us();
	  	temp=0;
	    if(P2_0)temp=1;
		flag=2;
		while((P2_0)&&flag++);
	   	//��ʱ������forѭ��		  
	   	if(flag==1)break;
	   	//�ж�����λ��0����1	 	   	   
		// ����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽֵ������λΪ 1 	   	 
		comdata<<=1;
	   	comdata|=temp;        //0
	}
}

//����7����ȡ��������ʪ��ģ������	
void getTemperatureHumidity(void)
{
	while(receiveData==0x07)
	{
		//��������18ms 
	    P2_0=0;
		Delay(180);
		P2_0=1;
		//������������������ ������ʱ20us
		Delay_10us();
		Delay_10us();
		Delay_10us();
		Delay_10us();
		//������Ϊ���� �жϴӻ���Ӧ�ź� 
		P2_0=1;
		//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
		if(!P2_0)		 //T !	  
		{
			flag=2;
			//�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
			while((!P2_0)&&flag++);
			flag=2;
			//�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
			while((P2_0)&&flag++);
			//���ݽ���״̬		 
			COM();
			humidity_data_H_temp=comdata;
			COM();
		 	humidity_data_L_temp=comdata;
			COM();
			temperature_data_H_temp=comdata;
			COM();
			temperature_data_L_temp=comdata;
			COM();
			check_data_temp=comdata;
			P2_0=1;
			//����У�� 
			temp=(temperature_data_H_temp+temperature_data_L_temp+humidity_data_H_temp+humidity_data_L_temp);
			if(temp==check_data_temp)
			{
				temperature_data_H=temperature_data_H_temp;
			   	temperature_data_L=temperature_data_L_temp;
				humidity_data_H=humidity_data_H_temp;
			   	humidity_data_L=humidity_data_L_temp;

				sendCharData(0xEE); 				//���Ϳ�ʼ��־��0xEE
		   		Delay(10);							//��ʱ1ms
				sendCharData(temperature_data_H); 	//�����¶ȸ�8λ(��������)
		   		Delay(10);							//��ʱ1ms
				sendCharData(temperature_data_L);	//�����¶ȵ�8λ(С������)
				Delay(10);							//��ʱ1ms
				sendCharData(humidity_data_H);		//����ʪ�ȸ�8λ(��������)
				Delay(10);							//��ʱ1ms
				sendCharData(humidity_data_L);		//����ʪ�ȵ�8λ(С������)
				Delay(10);							//��ʱ1ms
				sendCharData(0xFF); 				//���ͽ�����־��0xFF
				Delay(10);							//��ʱ1ms

				receiveData=0x00;					//ֻ��ȡһ����ʪ������
			}
		}
	}
}

void main(void)
{	
	IP=0x10;										//���ô��п�Ϊ�����ȼ��ж�,��ʱ��T0Ϊ�����ȼ��ж�
	TMOD=0x21;										//Ĭ�϶�ʱ��T0Ϊ������ʽ1,��ʱ��T1Ϊ������ʽ2
	timer1_init();
	while(1)
	{	
		controlLed(); 								//����1������LED�� �� ����2��Ϩ��LED��
		waterLamp();								//����3����ˮ��
		brightnessControl();						//����4��LED�ƽϵ�������ʾ(һ������) �� ����5��LED�ƽϸ�������ʾ(��������)
		breathingLamp(); 							//����6��������
		getTemperatureHumidity();					//����7����ȡ��������ʪ��ģ������
	}
}

//���п��жϺ���
void serial() interrupt 4 
{
	if(RI==1)										//����ǽ����ж�,�������湤��
	{
		receiveData=SBUF;							//���ڽ��յ������ݴ��������receiveData
		RI=0;										//�����жϱ�־λ����
		SBUF=receiveData;							
	}
	if(TI==1)										//����Ƿ����ж�,�������湤��
	{
		TI=0; 										//�����жϱ�־λ����
	}
}

//��ʱ��T0�жϺ���
void timer0() interrupt 1
{
	if(receiveData==0x04)							//LED�ƽϵ�������ʾ(һ������)
	{
		TR0=0;										//����ֵ��ʱ,�رն�ʱ��
		TH0=0xff;									//�߰�λΪ(65536-10)/256
		TL0=0xf7;									//�Ͱ�λΪ(65536-10)%256,��10����������,��ʱԼΪ10.850684us
		TR0=1;										//�򿪶�ʱ��T0	
		PWM_brightnessControl_count++;				//ÿ�ж�1�Σ�PWM_brightnessControl_count��1
		if(PWM_brightnessControl_count>=100)  		//��ִ����1��PWM����(�ж�100�Σ���1000����������)
		  	PWM_brightnessControl_count=0;      	//PWM_brightnessControl_count���㣬���¿�ʼ��һ�ֵ�PWM����
		if(PWM_brightnessControl_count<=90)   		//��ִ��PWM���ڵ�ǰ90%���ڼ���
			P0=0xFF;     							//�͸ߵ�ƽ,Ϩ��Led��,ռ�ձ�Ϊ90%
		else 										//��ִ�и���PWM�������²��ֵ��ڼ���
			P0=~P0;									//����LED��
	}
	if(receiveData==0x05)	   						//LED�ƽϸ�������ʾ(��������)
	{
		TR0=0;										//����ֵ��ʱ,�رն�ʱ��
		TH0=0xff;									//�߰�λΪ(65536-10)/256
		TL0=0xf7;									//�Ͱ�λΪ(65536-10)%256,��10����������,��ʱԼΪ10.850684us
		TR0=1;										//�򿪶�ʱ��T0	
		PWM_brightnessControl_count++;				//ÿ�ж�1�Σ�PWM_brightnessControl_count��1
		if(PWM_brightnessControl_count>=100)  		//��ִ����1��PWM����(�ж�100�Σ���1000����������)
		  	PWM_brightnessControl_count=0;      	//PWM_brightnessControl_count���㣬���¿�ʼ��һ�ֵ�PWM����
		if(PWM_brightnessControl_count<=60)   		//��ִ��PWM���ڵ�ǰ60%���ڼ���
			P0=0xFF;     							//�͸ߵ�ƽ,Ϩ��Led��,ռ�ձ�Ϊ60%
		else 										//��ִ�и���PWM�������²��ֵ��ڼ���
			P0=~P0;									//����LED��
	}
	if(receiveData==0x06)			   				//������
	{
		PWM_breathingLamp_count+=0.25;
    	HUXI_COUNT++;								//��ʱ��T0ÿ�ж�1��,HUXI_COUNT�ͼ�1
										
    	if(PWM_breathingLamp_count==PWM_VLAUE)  	//�ж��Ƿ��˵���LED��ʱ��
    		P0=0x00;                  				//����LED��
	    if(PWM_breathingLamp_count==10.0)      	 	//��ǰ���ڽ���
	    {
	        P0=0xff;                    			//Ϩ��LED��
	        PWM_breathingLamp_count=0.0;        	//���¼�ʱ
	    }

	    if((HUXI_COUNT==100)&&(direc_flag==0))		//��ʱ��T0ÿ�ж�100����direc_flag=0,ռ�ձȾ͸���һ��
	    {                               			
	        HUXI_COUNT=0.0;							//ռ�ձȸ���ʱ������
	        PWM_VLAUE+=0.25;						//ռ�ձ�����2.5%
	        if(PWM_VLAUE==9.75)          			//��ռ�ձ�Ϊ97.5%
	            direc_flag=~direc_flag; 			//ռ�ձȸ��ķ���
	    }
	    if((HUXI_COUNT==100)&&(direc_flag==1))		//��ʱ��T0ÿ�ж�100����direc_flag=1,ռ�ձȾ͸���һ��
	    {                               			
	        HUXI_COUNT=0.0;							//ռ�ձȸ���ʱ������
	        PWM_VLAUE-=0.25;						//ռ�ձȼ���2.5%
	        if(PWM_VLAUE==0.25)          			//��ռ�ձ�Ϊ2.5%
	            direc_flag=~direc_flag; 			//ռ�ձȸ��ķ���
	    }							
	}			
}	 		  