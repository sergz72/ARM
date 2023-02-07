#ifndef _PWR_H
#define _PWR_H

#define PWR_FLAG_WU                     PWR_CSR_WUF
#define PWR_FLAG_SB                     PWR_CSR_SBF
#define PWR_FLAG_PVDO                   PWR_CSR_PVDO
#define PWR_FLAG_BRR                    PWR_CSR_BRR
#define PWR_FLAG_VOSRDY                 PWR_CSR_VOSRDY
#define PWR_FLAG_ODRDY                  PWR_CSR_ODRDY
#define PWR_FLAG_ODSWRDY                PWR_CSR_ODSWRDY
#define PWR_FLAG_UDRDY                  PWR_CSR_UDSWRDY

/* --- FLAG Legacy ---*/
#define PWR_FLAG_REGRDY                  PWR_FLAG_VOSRDY               

void PWR_BackupAccessCmd(FunctionalState NewState);
void PWR_BackupRegulatorCmd(FunctionalState NewState);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);

#endif
