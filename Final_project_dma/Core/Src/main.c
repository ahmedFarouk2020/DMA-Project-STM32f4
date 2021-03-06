#include "GPIO_Interface.h"
#include "DMA_Interface.h"
#include "UART_Interface.h"

#define NOT_COMPLETED 	0
#define COMPLETED		1


unsigned char scr[100];
unsigned char dest[100];

unsigned App_TransferStatus = NOT_COMPLETED;

void App_TransferNotification(void);


#define INTERRUPT		/* INTERRUPT OR POLLING */

#ifdef INTERRUPT


int main()
{


	DmaConfig_t config[] = {
			{
				DMA2, STREAM5, CHANNEL0,
				(unsigned int)scr,(unsigned int)dest, HIGH,
				100, _1BYTE__1_4FIFO,DMA_FLOW_CONTROL,MEM_TO_MEM, ALL
			}
	};

	// initailize scr with ones
	for(unsigned char i=0; i<100; i++) {
		scr[i] = (i+1);
	}

		/* Led indicator for completion of transmission */
	GPIO_EnableClk(PORTA);


	/* UART init */
	GPIO_Init(PORTA,PIN2,ALTERNATE_FUNCTION_PP);
	UART_BindPin();
	UART_Init();


	DMA_Init(RCC_DMA2);
	DMA_SetConfigurations(&config[0]);

	DMA_StartTransfer(STREAM5);



	unsigned char flag = 0;
	while(1)
	{
		DMA_EnterCriticalSection();

		flag = App_TransferStatus; /* Critical Section */
		App_TransferStatus = NOT_COMPLETED;

		DMA_ExitCriticalSection();


		if(flag == COMPLETED)
		{
			/* loop on destnation array and print its elements */
			for(unsigned char i=0;i<config[0].numberOfTransfers;i++)
			{
				UART_PrintInt(dest[i]);
				UART_Transmit("\r\n");
			}

		}
	}
	return 0;
}






#endif











#ifdef POLLING

int main()
{

	// max number of items 44

	DmaConfig_t config[] = {
			{
				DMA2, STREAM5, CHANNEL0,
				(unsigned int)scr,(unsigned int)dest, HIGH,
				100, _1BYTE__1_4FIFO,DMA_FLOW_CONTROL,MEM_TO_MEM, NO_INT
			}
	};

	// initailize scr with ones
	for(unsigned char i=0; i<100; i++) {
		scr[i] = (i+1);
	}

		/* Led indicator for completion of transmission */
	GPIO_EnableClk(PORTA);


	/* UART init */
	GPIO_Init(PORTA,PIN2,ALTERNATE_FUNCTION_PP);
	UART_BindPin();
	UART_Init();


	DMA_Init(RCC_DMA2);
	DMA_SetConfigurations(&config[0]);

	DMA_StartTransfer(STREAM5);



	unsigned char flag = 0;
		GPIO_Init(PORTA, PIN3,OUTPUT_PUSHPULL);
		GPIO_Init(PORTA, PIN4,OUTPUT_PUSHPULL);
		GPIO_Init(PORTA, PIN5,OUTPUT_PUSHPULL);
		GPIO_Init(PORTA, PIN6,OUTPUT_PUSHPULL);
	while(1)
	{


		StatusReport_t stream_report = DMA_GetTransferState(HALF_TRANSFER_S5, TRANSFER_COMPLETE_S5, TRANSFER_ERROR_S5);

		GPIO_WritePin(PORTA, PIN3, stream_report.fifo_error);
		GPIO_WritePin(PORTA, PIN3, stream_report.half_transfer);
		GPIO_WritePin(PORTA, PIN4, stream_report.transfer_complete);
		GPIO_WritePin(PORTA, PIN5, stream_report.transfer_error);
		flag = stream_report.transfer_complete;

		if(flag == COMPLETED)
		{

					/* loop on destnation array and print its elements */
			for(unsigned char i=0;i<config[0].numberOfTransfers;i++)
			{
				UART_PrintInt(dest[i]);
				UART_Transmit("\r\n");
			}

		}
	}
	return 0;
}

#endif




void App_TransferNotification(void)
{
	App_TransferStatus = COMPLETED;
}
