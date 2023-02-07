#ifndef _IWDG_H
#define _IWDG_H

#define IWDG_WriteAccess_Enable     ((unsigned short)0x5555)
#define IWDG_WriteAccess_Disable    ((unsigned short)0x0000)

#define IWDG_Prescaler_4            ((unsigned char)0x00)
#define IWDG_Prescaler_8            ((unsigned char)0x01)
#define IWDG_Prescaler_16           ((unsigned char)0x02)
#define IWDG_Prescaler_32           ((unsigned char)0x03)
#define IWDG_Prescaler_64           ((unsigned char)0x04)
#define IWDG_Prescaler_128          ((unsigned char)0x05)
#define IWDG_Prescaler_256          ((unsigned char)0x06)

#define IWDG_FLAG_PVU               ((unsigned short)0x0001)
#define IWDG_FLAG_RVU               ((unsigned short)0x0002)
#define IWDG_FLAG_WVU               ((unsigned short)0x0004)

/* Exported functions --------------------------------------------------------*/

/* Prescaler and Counter configuration functions ******************************/
void IWDG_WriteAccessCmd(unsigned short IWDG_WriteAccess);
void IWDG_SetPrescaler(unsigned char IWDG_Prescaler);
void IWDG_SetReload(unsigned short Reload);
void IWDG_ReloadCounter(void);
void IWDG_WindowRegister(unsigned short value);

/* IWDG activation function ***************************************************/
void IWDG_Enable(void);

/* Flag management function ***************************************************/
FlagStatus IWDG_GetFlagStatus(unsigned short IWDG_FLAG);

#endif /* _IWDG_H */
