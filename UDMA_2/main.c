//****************************************************************************************
// Author: Obed Oyandut
// Date  : 09.08.2022
// Version: 1
//****************************************************************************************
// This Program worlks on TIVA TM4C1294XL Evaluation Board.
// The program performs a memory a memory gather using udma.
//****************************************************************************************
//                     ! IMPORTANT !
// This program runs endless. Stop with the "Red Square Button"
// in Debug Mode (Terminate = CTRL + F2)
//****************************************************************************************
// Include the Header File for controller tm4c1294ncpdt

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//************************************************************************************************************************
// Length of control table, source buffer and destination buffer
//************************************************************************************************************************

#define BASE 256
#define LEN 1000

//************************************************************************************************************************
// Text: Extract of the Raven by Edgar Allan Poe.
// The content of each array is copied by udma into the destBuffer array. This is a demonstration of memory gather.
//************************************************************************************************************************


volatile char line1[] = "Once upon a midnight dreary, while I pondered, weak and weary,"; // len=62
volatile char line2[] = "Over many a quaint and curious volume of forgotten lore—";  //56
volatile char line3[] = "While I nodded, nearly napping, suddenly there came a tapping,"; //62
volatile char line4[] = "As of some one gently rapping, rapping at my chamber door."; //58
volatile char line5[] = "“’Tis some visitor,” I muttered, “tapping at my chamber door—"; //61
volatile char line6[] = "Only this and nothing more.”"; //28


volatile char destBuffer[LEN];

//************************************************************************************************************************
// The control table is declared as unsigned int. This means the length is 256. Channel 30 is used. The offset of the pri-
//mary control structure of channel 30 is 120.
//************************************************************************************************************************

volatile unsigned int utable[BASE];

//************************************************************************************************************************
// The atable array is copied into the alternate control section of the base control table. It is then executed by the udma
//*************************************************************************************************************************

volatile unsigned int atable[] = { (unsigned int)&line1[61], (unsigned int)&destBuffer[61], 0x000003E5, 0x00000000,
                                   (unsigned int)&line2[55], (unsigned int)&destBuffer[118], 0x00000385, 0x00000000,
                                   (unsigned int)&line3[61], (unsigned int)&destBuffer[181], 0x000003E5, 0x00000000,
                                   (unsigned int)&line4[57], (unsigned int)&destBuffer[240], 0x000003A5, 0x00000000,
                                   (unsigned int)&line5[60], (unsigned int)&destBuffer[302], 0x000003C5, 0x00000000,
                                   (unsigned int)&line6[27], (unsigned int)&destBuffer[331], 0x000001C2, 0x00000000 };


//************************************************************************************************************************
// Interrupt is executed after udma transfer is done.
//************************************************************************************************************************

void udmaInterrupt(void) {

    destBuffer[332] = '\0';
    printf("destBuffer: %s.\n", destBuffer);
    printf("destBuffer: %s.\n", &destBuffer[63]);
    printf("destBuffer: %s.\n", &destBuffer[120]);
    printf("destBuffer: %s.\n", &destBuffer[183]);
    printf("destBuffer: %s.\n", &destBuffer[242]);
    printf("destBuffer: %s.\n", &destBuffer[304]);
}

//************************************************************************************************************************
// Prints a notification if udma encounters error.
//************************************************************************************************************************

void udmaInterruptError(void) {

    UDMA_ERRCLR_R |= 0x01;
    printf("Udma encounters an error...\n");
}

//*************************************************************************************************************************
// Configure and enable channel 30
//*************************************************************************************************************************

void dmaconfig(void) {
    SYSCTL_RCGCDMA_R |= 0x01;
    while(!(SYSCTL_PRDMA_R & 0x01));
    UDMA_CFG_R |= 0x01;
    UDMA_PRIOSET_R |= (0x01<<30);
    UDMA_ALTCLR_R |= (0x01<<30);
    UDMA_USEBURSTCLR_R |= 0x40000000;
    UDMA_REQMASKCLR_R |= 0x40000000;
    UDMA_CTLBASE_R |= (unsigned int)utable;
    UDMA_ENASET_R |= 0x40000000;
    UDMA_SWREQ_R |= 0x40000000;
}

//**************************************************************************************************************************
// Enable udma interrupts in NVIC
//**************************************************************************************************************************

void enableNVIC(void) {

    NVIC_EN1_R |= (0x03<<44-32);

}



//**************************************************************************************************************************
// The udma process is triggered by the primary control structure. Udma copies content into alternate control structure and
// performs the copy operations.
//**************************************************************************************************************************

void udmaTransfer(void) {

    utable[120] = (unsigned int)&atable[23];
    utable[121] = (unsigned int)&utable[251];
    utable[122] = 0xAA008174;

    enableNVIC();
    dmaconfig();
}

void main(void) {

    udmaTransfer();
    while(1) {

    }
}
