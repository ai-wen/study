
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <winscard.h>
#include <reader.h>
//#include <pcsclite.h>

//g++ -g -o get getAtr.cpp -lpcsclite -I /usr/include/PCSC/

int main1(void) 
{
    SCARDCONTEXT m_hContext = NULL;
    SCARDHANDLE  m_hCard = NULL;
    char pmszReaders[512] = {0};
    DWORD cch = 100;
    DWORD ret = 0;
    DWORD _ActiveProtocol = SCARD_PROTOCOL_T1;

    do
    {   
         ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &m_hContext);
         if(SCARD_S_SUCCESS != ret)break;

         ret = SCardListReaders(m_hContext, NULL, pmszReaders, &cch);
         if(SCARD_S_SUCCESS != ret)break;
       
        int i = 0;
        while(i<cch)
        {
            printf("%s\n",pmszReaders+i);
            i += strlen(pmszReaders);
            i ++;

            do
            {
                 ret = SCardConnect(m_hContext, pmszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, 
				        &m_hCard, &_ActiveProtocol);
                if(SCARD_S_SUCCESS != ret)break;

                unsigned char pbAtr[32] = {0};
                DWORD pcbAtrLen = sizeof(pbAtr) ;
                ret = SCardGetAttrib(m_hCard, SCARD_ATTR_ATR_STRING, pbAtr, &pcbAtrLen);
                if(SCARD_S_SUCCESS != ret)break;

                if(pcbAtrLen > 0)
                {
                    printf("\t Atr: ");
                    for(DWORD a=0; a<pcbAtrLen; a++ )
                        printf(" %02x", pbAtr[a]);
                }
                printf("\n\n");
                
            }while (0);
           
           if(m_hCard)
            ret = SCardDisconnect(m_hCard, SCARD_EJECT_CARD),m_hCard = NULL;
        }

    }while(0);

    
    ret = SCardReleaseContext(m_hContext);

    return 0;
}



int discover_devices(char * parameter, char device_pathes[32][256])
{
	SCARDCONTEXT hContext = NULL;
	SCARDHANDLE  hCard = NULL;
	char pmszReaders[1024] = {0};	
	DWORD cch = sizeof(pmszReaders);
	DWORD ret = SCARD_S_SUCCESS;
	DWORD ActiveProtocol = SCARD_PROTOCOL_T1;
	int	  devNumber = 0;

	char* tmpReaders = pmszReaders;
	int i = 0;

	unsigned char cardAtr[]={0x3b, 0x9f, 0x11, 0x81, 0x31, 0xfe, 0x9f, 0x00, 0x6a, 0x6d, 0x54, \
		0x6f, 0x6b, 0x65, 0x6e, 0x2d, 0x50, 0x00, 0x01, 0x81, 0x90, 0x00, 0x6e};
	unsigned char pbAtr[32] = {0};
	DWORD pcbAtrLen = sizeof(pbAtr);

	do
	{   
		ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
		if(SCARD_S_SUCCESS != ret)break;

		ret = SCardListReaders(hContext, NULL, pmszReaders, &cch);
		if(SCARD_S_SUCCESS != ret)break;

		
		while(i<cch)
		{
			tmpReaders += i;
			i = strlen(tmpReaders);
			if(i == 0)
				break;
	
			do
			{
				ret = SCardConnect(hContext, tmpReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &ActiveProtocol);
				if(SCARD_S_SUCCESS != ret)break;

				pcbAtrLen = sizeof(pbAtr);
				memset(pbAtr,0,pcbAtrLen);

				ret = SCardGetAttrib(hCard, SCARD_ATTR_ATR_STRING, pbAtr, &pcbAtrLen);
				if(SCARD_S_SUCCESS != ret)break;

				if(0 == memcmp(cardAtr,pbAtr,sizeof(cardAtr)))
				{
					strncpy(device_pathes[devNumber++], tmpReaders,256);
				}

				//DBG_LOGBUFFER(pbAtr, pcbAtrLen, "Reader:%s  Atr:",tmpReaders);

			}while (0);

			if(hCard)
				ret = SCardDisconnect(hCard, SCARD_EJECT_CARD),hCard = NULL;
			
			tmpReaders += 1;
		}

	}while(0);


	ret = SCardReleaseContext(hContext);

	return devNumber;
}


int TransCmd(char device_pathes[256])
{
	SCARDCONTEXT hContext = NULL;
	SCARDHANDLE  hCard = NULL;
    SCARD_IO_REQUEST io={0};

	DWORD ret = SCARD_S_SUCCESS;
	//DWORD ActiveProtocol = SCARD_PROTOCOL_T1;


    ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
    if(SCARD_S_SUCCESS != ret)
    return ret;

    do
    {
        io.cbPciLength = sizeof(SCARD_IO_REQUEST);
        ret = SCardConnect(hContext, device_pathes, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &io.dwProtocol);
        if(SCARD_S_SUCCESS != ret)break;
        
        ret = SCardBeginTransaction(hCard);
        if(SCARD_S_SUCCESS != ret)break;

        BYTE CAPDU_0[] = {0x01,0x02,0x03,0x04,0x05,0x36,0x07};  //get main folder 0x3F00
        BYTE RAPDU_0[1024];
        DWORD rr0 = 1024;
        ret = SCardTransmit(hCard, &io, CAPDU_0, 7, NULL, RAPDU_0, &rr0);
        //ret = SCardTransmit(m_hCard, SCARD_PCI_RAW, CAPDU, 7, NULL, RAPDU, &cch);

        SCardEndTransaction(hCard, SCARD_LEAVE_CARD);

        if(SCARD_S_SUCCESS != ret)break;

        printf("\t\t RR_LEN: %ld\n", rr0);
        printf("\t\t RAPDU: ");
        for(int i=0; i<rr0; i++)
            printf("0x%02X ",RAPDU_0[i]);
        printf("\n\n");
    }while (0);

    if(hCard)
        SCardDisconnect(hCard, SCARD_EJECT_CARD),hCard = NULL;

	SCardReleaseContext(hContext);

	return ret;
}


int main()
{
    char device_pathes[32][256]={0};
    int num = discover_devices(NULL, device_pathes);
    if(num == 0)
    {
        printf("num = 0\n");
         return 0;
    }

    printf("%x\n",TransCmd(device_pathes[0])) ;

    return 0;
}