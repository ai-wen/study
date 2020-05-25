#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "pcsclite.h"
#include "winscard.h"
#include "reader.h"

int main(void) {
    SCARDCONTEXT m_hContext;
    SCARDHANDLE  m_hCard;
    SCARD_IO_REQUEST io;
    char pmszReaders[512];
//  BYTE CAPDU[] = {0x00,0x84,0x00,0x00,0x08};
//  BYTE CAPDU[] = {0xFF,0xA4,0x00,0x00,0x01,0x06};
    BYTE CAPDU_0[] = {0x00,0xA4,0x00,0x00,0x02,0x3F,0x00};  //get main folder 0x3F00
    BYTE CAPDU_1[] = {0x00,0x84,0x00,0x00,0x08};    //get random number,8 bytes

//  BYTE CAPDU_1[] = {0xFF,0xA4,0x00,0x00,0x01,0x06};   //reset, power down and up
    BYTE CAPDU_2[] = {0xFF,0xB0,0x00,0x00,0xFE};    //read card, 0x00 -> 0x08
    BYTE RAPDU_0[1024];
    BYTE RAPDU_1[1024];
    BYTE RAPDU_2[1024];
    DWORD rr0 = 1024, rr1 = 1024, rr2 = 1024;
    DWORD cch = 100;
    DWORD i = 0;
    DWORD ret = 0;

    //Create contex
    printf("Create contex ");
//  ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_hContext);
    ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &m_hContext);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -1;
    }
    else
        printf("succeed !\n");

    //List Reader
    printf("List Reader ");
    ret = SCardListReaders(m_hContext, NULL, pmszReaders, &cch);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -2;
    }
    else
        printf("succeed !\n");

    printf("\tReader: ");
    while(i<cch)
    {
        printf("%s\n",pmszReaders+i);
        i += strlen(pmszReaders);
        i ++;
    }

    //Connect first Reader
    printf("Connect Reader ");
    io.cbPciLength = sizeof(SCARD_IO_REQUEST);
    ret = SCardConnect(m_hContext, pmszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1, &m_hCard, &io.dwProtocol);
//  ret = SCardConnect(m_hContext, pmszReaders, SCARD_SHARE_SHARED|SCARD_SHARE_DIRECT, SCARD_PROTOCOL_RAW|SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1, &m_hCard, &io.dwProtocol);
//  ret = SCardConnect(m_hContext, pmszReaders, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_RAW, &m_hCard, &io.dwProtocol);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -3;
    }
    else
        printf("succeed !\n");
    printf("\t Connect Protocol: %ld %d\n", io.dwProtocol, SCARD_PROTOCOL_RAW);

#if 0
    //use SCardControl to inform the new card driver
    printf("SCardControl: set card type: ");
#define IOCTL_SMARTCARD_SET_CARD_TYPE SCARD_CTL_CODE(2060)
    long nCardType = 15;
    BYTE cbOutBuffer[10];
    SCARDCONTEXT hctx;
    SCARDHANDLE hsc;
    DWORD dwActievProtocol;
    DWORD dwBytesRet;
    ret = SCardControl(m_hCard, IOCTL_SMARTCARD_SET_CARD_TYPE,
            &nCardType, sizeof(nCardType), cbOutBuffer, 10,
            &dwBytesRet);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -3;
    }
    else
        printf("succeed !\n");
    printf("\t Connect Protocol: %ld %d\n", io.dwProtocol, SCARD_PROTOCOL_RAW);
#endif

    //check card status && atr
    printf("Check Card status && atr ");
//  char myReader[128] = {0};
//  DWORD readerLen;
    DWORD pwdState;
    DWORD pwdProtocol;
//  BYTE pbAtr[32] = {0};
    unsigned char pbAtr[32] = {0};
    BYTE pt1[] = "test";
    DWORD pcbAtrLen = sizeof(pbAtr) ;
//  printf("\t AtrLen: %ld\n", pcbAtrLen);
//  ret = SCardStatus(m_hCard, myReader, &readerLen, &pwdState, &pwdProtocol, pbAtr, &pcbAtrLen);
    ret = SCardStatus(m_hCard, pmszReaders, &i, &pwdState, &pwdProtocol, pbAtr, &pcbAtrLen);
//  ret = SCardGetAttrib(m_hCard, SCARD_ATTR_ATR_STRING, pbAtr, &pcbAtrLen);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -3;
    }
    else
        printf("succeed !\n");
//  memcpy(pbAtr, pt1, 16);
    printf("\t Reader: %s, lenth: %ld\n", pmszReaders, i);
//  printf("\t%s, %ld\n", myReader, readerLen);
                                            
    printf("\t Status: %ld = 0x%lx\n", pwdState, pwdState);
    printf("\t Protocol: %ld = 0x%lx\n", pwdProtocol, pwdProtocol);
    printf("\t AtrLen: %ld\n", pcbAtrLen);
    if(pcbAtrLen > 0)
    {
        printf("\t Atr: ");
        for(DWORD a=0; a<pcbAtrLen; a++ )
            //      printf(" %c", pbAtr[a]);
            printf(" %02x", pbAtr[a]);
    }
    printf("\n\n");

//#if 0
    //begin transaction
    printf("SCardBeginTransaction ");
    ret = SCardBeginTransaction(m_hCard);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -4;
    }
    else
        printf("succeed !\n");
//#endif

    //Transmit APDU
    printf("Transmit APDU:\n");
//#if 0
//  printf("\t Reset Card: ");
//  ret = SCardTransmit(m_hCard, &io, CAPDU_1, 6, NULL, RAPDU_1, &rr1);
    printf("\t Select main folder 0x3F00: ");
    ret = SCardTransmit(m_hCard, &io, CAPDU_0, 7, NULL, RAPDU_0, &rr0);
//  ret = SCardTransmit(m_hCard, SCARD_PCI_RAW, CAPDU, 7, NULL, RAPDU, &cch);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -4;
    }
    else
        printf("succeed !\n");

    //echo
    printf("\t\t CAPDU: ");
    for(i=0; i<7; i++)
    {
        sprintf(pmszReaders,"0x%02X ",CAPDU_0[i]);
        printf("%s",pmszReaders);
    }
    printf("\n");
    printf("\t\t RR_LEN: %ld\n", rr0);
    printf("\t\t RAPDU: ");
    for(i=0; i<rr0; i++)
    {
        sprintf(pmszReaders,"0x%02X ",RAPDU_0[i]);
        printf("%s",pmszReaders);
    }
//#endif
    printf("\n\t Get random number: ");
    ret = SCardTransmit(m_hCard, &io, CAPDU_1, 5, NULL, RAPDU_1, &rr1);
//  ret = SCardTransmit(m_hCard, SCARD_PCI_RAW, CAPDU, 7, NULL, RAPDU, &cch);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -4;
    }
    else
        printf("succeed !\n");

    //echo
    printf("\t\t CAPDU: ");
    for(i=0; i<5; i++)
    {
        sprintf(pmszReaders,"0x%02X ",CAPDU_1[i]);
        printf("%s",pmszReaders);
    }
    printf("\n");
    printf("\t\t RR_LEN: %ld\n", rr1);
    printf("\t\t RAPDU: ");
    for(i=0; i<rr1; i++)
    {
        sprintf(pmszReaders,"0x%02X ",RAPDU_1[i]);
        printf("%s",pmszReaders);
    }
//#endif
    printf("\n\t Read Card: ");
    ret = SCardTransmit(m_hCard, &io, CAPDU_2, 5, NULL, RAPDU_2, &rr2);
//  ret = SCardTransmit(m_hCard, SCARD_PCI_RAW, CAPDU, 7, NULL, RAPDU, &cch);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -4;
    }
    else
        printf("succeed !\n");

    //echo
    printf("\t\t CAPDU: ");
    for(i=0; i<5; i++)
    {
        sprintf(pmszReaders,"0x%02X ",CAPDU_2[i]);
        printf("%s",pmszReaders);
    }
    printf("\n");
    printf("\t\t RR_LEN: %ld\n", rr2);
    printf("\t\t RAPDU: ");
    for(i=0; i<rr2; i++)
    {
        sprintf(pmszReaders,"0x%02X ",RAPDU_2[i]);
        printf("%s",pmszReaders);
    }
    printf("\n\n");

    //DisConnect
    printf("Card disconnect: ");
    ret = SCardDisconnect(m_hCard, SCARD_EJECT_CARD);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -3;
    }
    else
        printf("succeed !\n");

    //Eject
    printf("Card eject ");
    ret = SCardReleaseContext(m_hContext);
    if(SCARD_S_SUCCESS != ret)
    {
        printf("error: 0x%lx = %ld\n", ret, ret);
        return -3;
    }
    else
        printf("succeed !\n");

    return 0;
}
