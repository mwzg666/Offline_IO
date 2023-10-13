#include "system.h"

#if 0
void TimerAInit()
{ 
    TA1CTL = TASSEL_1 + MC_1 + TACLR;         // ACLK, upmode, clear TAR
    TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
    TA1CCR0 = 32880;//32770-1;                      // 1s 相当于32776 个ACLK 
    _EINT();
}
#endif

