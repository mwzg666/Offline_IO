#ifndef MCP4725_H_
#define MCP4725_H_

#define MCP4725_aV_ADDR 0x60//alphy  vth mcp4725 address
#define MCP4725_BH_ADDR 0x61//beita vth high mcp4725 address
#define MCP4725_BL_ADDR 0X62//beita vth low mcp4725 address
#define MCP4725_HV_ADDR 0X63//high voltage mcp4725 address
//mcp4725的参考电压,该值如果变化了，需要修改MCP4725_OutVol函数里面的常量值
//修改值为4096/MCP4725_REF_VOL的值
#define MCP4725_REF_VOL 3000
#define MCP4725_PD_MODE 0x00


//函数声明
void MCP4725_OutVol(BYTE addr,unsigned int voltage);
void MCP4725_OutVol2(BYTE addr,unsigned int voltage);

unsigned int MCP4725_Read(BYTE addr);
void MCP4725_Write(void);

#endif
