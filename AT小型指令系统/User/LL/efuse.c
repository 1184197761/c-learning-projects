/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "efuse.h"
#include "typesdef.h"
#include "user.h"

//**************************************************************************************
// &&&&&&&&&&&&&&&&&&&&   MARS EFUSE TEST CASE MAIN FUNCTION &&&&&&&&&&&&&&&&&&&&&&&&&//
//**************************************************************************************
extern int16 rx_ana_dcoc[8]; //8 indicate 8 gain
extern int8 rx_dcoc_res[8][2]; //8 indicate 8 gain, 2 indicate 2 digital i/q compesation value
extern int8 tx_ipa_dcoc_res[2]; //[0] is i-data, [1] is q-data
extern int16 tx_ipa_imb_res_gm;
extern int16 tx_ipa_imb_res_pm;

extern int16 dpd_phase_comp;
extern uint16 dpd_gain_comp;
extern int16 rx_imb_phase_comp[8]; //8 indicate rx 8 gain
extern uint16 rx_imb_gain_comp[8]; //8 indicate rx 8 gain

uint8 xo_cs_post;
uint8 xo_fix_sel;

extern uint8 RF_VREF;
extern uint8 RF_IBPT;
extern uint8 RF_IBCT;
extern uint8 RF_LO_VREF;
extern uint16 RF_VDDI;
extern uint16 RF_VPAT_VDDI_RATIO;

extern uint8 dpd_tx_gain_value;
extern int8 trx_temperature_value;

int16 rx_ana_dcoc_efuse_rd_buf[8]; //8 indicate 8 gain
int8 rx_dcoc_res_efuse_rd_buf[8][2]; //8 indicate 8 gain, 2 indicate 2 digital i/q compesation value
int8 tx_ipa_dcoc_res_efuse_rd_buf[2]; //[0] is i-data, [1] is q-data
int16 tx_ipa_imb_res_gm_efuse_rd_buf;
int16 tx_ipa_imb_res_pm_efuse_rd_buf;

int16 dpd_phase_comp_efuse_rd_buf;
uint16 dpd_gain_comp_efuse_rd_buf;
int16 rx_imb_phase_comp_efuse_rd_buf[8]; //8 indicate rx 8 gain
uint16 rx_imb_gain_comp_efuse_rd_buf[8]; //8 indicate rx 8 gain

uint8 xo_cs_post_efuse_rd_buf;

uint8 rf_trim_val_crc_pBuf[256] = {0};
uint8 rf_trim_val_crc_chk_pBuf[256] = {0};

#define TX4104

/****************************************************************************** 
 * Name:    CRC-8               x8+x2+x+1 
 * Poly:    0x07 
 * Init:    0x00 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x00 
 * Note: 
 *****************************************************************************/  

uint8_t crc8(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for ( i = 0; i < 8; i++ )  
        {  
            if ( crc & 0x80 )  
                crc = (crc << 1) ^ 0x07;  
            else  
                crc <<= 1;  
        }  
    }  
    return crc;  
}  

/****************************************************************************** 
 * Name:    CRC-8 ITU               x8+x2+x+1 
 * Poly:    0x07 
 * Init:    0x00 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x55 
 * Note: 
 *****************************************************************************/  

uint8_t crc8_itu(uint8_t *data, uint8_t length)  
{  
    uint8_t i;  
    uint8_t crc = 0;        // Initial value  
    while(length--)  
    {  
        crc ^= *data++;        // crc ^= *data; data++;  
        for ( i = 0; i < 8; i++ )  
        {  
            if ( crc & 0x80 )  
                crc = (crc << 1) ^ 0x07;  
            else  
                crc <<= 1;  
        }  
    }  
    return crc ^ 0x55;  
}  

void efuse_init(void)
{
//    u8  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    ll_efuse_config();
}

int32 rf_para_efuse_write(void)
{
    uint8  prog_trim_value_byte[272] = {0};
    uint8  efuse_result = 0;
    uint32 fail_cnt = 0;
    uint32 un_empty_cnt = 0;
    
    uint8  crc_sw = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;

    for(int cnt=0; cnt<272; cnt++)
    {
        prog_trim_value_byte[cnt] = 0; //clear the efuse write buffer
    }
    
    //rx ana dcoc calibration data, 8 rx gain ana div into 4 group, each group using 2 bytes efuse to instore
    for(int rx_gain_index=0; rx_gain_index<4; rx_gain_index++) {
    #ifdef TX4104
        prog_trim_value_byte[98+3*rx_gain_index]  = rx_ana_dcoc[2*rx_gain_index]&0xff;
        prog_trim_value_byte[99+3*rx_gain_index]  = ((rx_ana_dcoc[2*rx_gain_index+1]&0x0f)<<4)|((rx_ana_dcoc[2*rx_gain_index]&0xf00)>>8);
        prog_trim_value_byte[100+3*rx_gain_index] = ((rx_ana_dcoc[2*rx_gain_index+1]&0xff0)>>4);
    #else
        prog_trim_value_byte[66+3*rx_gain_index]  = rx_ana_dcoc[2*rx_gain_index]&0xff;
        prog_trim_value_byte[67+3*rx_gain_index]  = ((rx_ana_dcoc[2*rx_gain_index+1]&0x0f)<<4)|((rx_ana_dcoc[2*rx_gain_index]&0xf00)>>8);
        prog_trim_value_byte[68+3*rx_gain_index]  = ((rx_ana_dcoc[2*rx_gain_index+1]&0xff0)>>4);
    #endif
    }
    
    //rx imb calibration data, 8 rx gain ana div into 4 group, each group using 3 bytes+1bit efuse to instore
    for(int rx_gain_index=0; rx_gain_index<4; rx_gain_index++)
    {
    #ifdef TX4104
        prog_trim_value_byte[177+3*rx_gain_index]  = rx_imb_gain_comp[2*rx_gain_index]&0xff;
        prog_trim_value_byte[178+3*rx_gain_index]  = ((rx_imb_gain_comp[2*rx_gain_index+1]&0x0f)<<4)|((rx_imb_gain_comp[2*rx_gain_index]&0xf00)>>8);
        prog_trim_value_byte[179+3*rx_gain_index]  = ((rx_imb_gain_comp[2*rx_gain_index+1]&0xff0)>>4);
        
        prog_trim_value_byte[189+3*rx_gain_index]  = rx_imb_phase_comp[2*rx_gain_index]&0xff;
        prog_trim_value_byte[190+3*rx_gain_index]  = ((rx_imb_phase_comp[2*rx_gain_index+1]&0x0f)<<4)|((rx_imb_phase_comp[2*rx_gain_index]&0xf00)>>8);
        prog_trim_value_byte[191+3*rx_gain_index]  = ((rx_imb_phase_comp[2*rx_gain_index+1]&0xff0)>>4);
    #else
        prog_trim_value_byte[84+3*rx_gain_index]  = rx_imb_gain_comp[2*rx_gain_index]&0xff;
        prog_trim_value_byte[85+3*rx_gain_index]  = ((rx_imb_gain_comp[2*rx_gain_index+1]&0x0f)<<4)|((rx_imb_gain_comp[2*rx_gain_index]&0xf00)>>8);
        prog_trim_value_byte[86+3*rx_gain_index]  = ((rx_imb_gain_comp[2*rx_gain_index+1]&0xff0)>>4);
        
        prog_trim_value_byte[96+3*rx_gain_index]  = rx_imb_phase_comp[2*rx_gain_index]&0xff;
        prog_trim_value_byte[97+3*rx_gain_index]  = ((rx_imb_phase_comp[2*rx_gain_index+1]&0x0f)<<4)|((rx_imb_phase_comp[2*rx_gain_index]&0xf00)>>8);
        prog_trim_value_byte[98+3*rx_gain_index]  = ((rx_imb_phase_comp[2*rx_gain_index+1]&0xff0)>>4);
    #endif
    }
    #ifdef TX4104
    prog_trim_value_byte[201] = ((rx_imb_phase_comp[7]&0x1000)>>5)|((rx_imb_phase_comp[6]&0x1000)>>6)|((rx_imb_phase_comp[5]&0x1000)>>7)\
                                |((rx_imb_phase_comp[4]&0x1000)>>8)|((rx_imb_phase_comp[3]&0x1000)>>9)|((rx_imb_phase_comp[2]&0x1000)>>10)\
                                |((rx_imb_phase_comp[1]&0x1000)>>11)|((rx_imb_phase_comp[0]&0x1000)>>12);
    #else
    prog_trim_value_byte[108] = ((rx_imb_phase_comp[7]&0x1000)>>5)|((rx_imb_phase_comp[6]&0x1000)>>6)|((rx_imb_phase_comp[5]&0x1000)>>7)\
                                |((rx_imb_phase_comp[4]&0x1000)>>8)|((rx_imb_phase_comp[3]&0x1000)>>9)|((rx_imb_phase_comp[2]&0x1000)>>10)\
                                |((rx_imb_phase_comp[1]&0x1000)>>11)|((rx_imb_phase_comp[0]&0x1000)>>12);
    #endif
    //rx dcoc calibration data
    for(int rx_gain_index=0; rx_gain_index<8; rx_gain_index++) {
    #ifdef TX4104
        prog_trim_value_byte[202+2*rx_gain_index]  = rx_dcoc_res[rx_gain_index][0]&0xff;
        prog_trim_value_byte[203+2*rx_gain_index]  = rx_dcoc_res[rx_gain_index][1]&0xff;
    #else
        prog_trim_value_byte[109+2*rx_gain_index]  = rx_dcoc_res[rx_gain_index][0]&0xff;
        prog_trim_value_byte[110+2*rx_gain_index]  = rx_dcoc_res[rx_gain_index][1]&0xff;
    #endif
    }
    
    //tx dcoc calibration data
    //for(int pwr_index=0; pwr_index<8; pwr_index++)
    {
    #ifdef TX4104
        prog_trim_value_byte[135] = tx_ipa_dcoc_res[0];
        prog_trim_value_byte[136] = tx_ipa_dcoc_res[1];
    #else
        prog_trim_value_byte[82] = tx_ipa_dcoc_res[0];
        prog_trim_value_byte[83] = tx_ipa_dcoc_res[1];
    #endif
    }
    
    //tx imb calibration data
    //for(int pwr_index=0; pwr_index<8; pwr_index++)
    {
    #ifdef TX4104
        prog_trim_value_byte[110] = tx_ipa_imb_res_gm & 0xff;
        prog_trim_value_byte[111] = ((tx_ipa_imb_res_pm & 0x0f)<<4)|((tx_ipa_imb_res_gm & 0xf00)>>8);
        prog_trim_value_byte[112] = (tx_ipa_imb_res_pm & 0xff0)>>4;
        prog_trim_value_byte[113] = (tx_ipa_imb_res_pm & 0x1000)>>12;
    #else
        prog_trim_value_byte[78] = tx_ipa_imb_res_gm & 0xff;
        prog_trim_value_byte[79] = ((tx_ipa_imb_res_pm & 0x0f)<<4)|((tx_ipa_imb_res_gm & 0xf00)>>8);
        prog_trim_value_byte[80] = (tx_ipa_imb_res_pm & 0xff0)>>4;
        prog_trim_value_byte[81] = ((xo_cs_post & 0x7f)<<1)|(tx_ipa_imb_res_pm & 0x1000)>>12;
    #endif
    }
    #ifdef TX4104
//    prog_trim_value_byte[252] = dpd_tx_gain_value & 0x7f;
    
    memcpy(rf_trim_val_crc_pBuf, prog_trim_value_byte, 256);
    
    crc_sw = module_area_crc_cal(rf_trim_val_crc_pBuf);
    #endif
    
    //check rx_ana_dcoc + tx_imb program region empty
#ifdef TX4104
    for(int cnt=98;cnt<114;cnt++)
    {
        ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
        if(!(efuse_result==0)) {un_empty_cnt++;}
    }
    //check tx_dcoc program region empty
    for(int cnt=135;cnt<137;cnt++)
    {
        ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
        if(!(efuse_result==0)) {un_empty_cnt++;}
    }
    //check rx_imb + rx_dcoc program region empty
    for(int cnt=177;cnt<218;cnt++)
    {
        ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
        if(!(efuse_result==0)) {un_empty_cnt++;}
    }
    
    ll_efuse_read(252,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0)) {un_empty_cnt++;}
    
    ll_efuse_read(256,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0)) {un_empty_cnt++;}
#else
    //check rx + tx program region empty
    for(int cnt=66;cnt<125;cnt++)
    {
        ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
        if(!(efuse_result==0)) {un_empty_cnt++;}
    }
#endif
    if(!un_empty_cnt)
    {
#ifdef TX4104
        //program efuse 97~112 BYTE
        for(int cnt=98;cnt<114;cnt++)
        {
          //program rx_ana_dcoc + tx_imb data
          ll_efuse_program(cnt,prog_trim_value_byte[cnt]);
        }
        //read efuse 97~112 Byte for check program is or not program succesfull
        for(int cnt=98;cnt<114;cnt++)
        {
            //read data
            ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
            if(!(efuse_result==prog_trim_value_byte[cnt])) {fail_cnt++;}
        }
        
        if(!fail_cnt)
        {
            //program efuse 134~135 BYTE
            for(int cnt=135;cnt<137;cnt++)
            {
              //program rx_ana_dcoc + tx_imb data
              ll_efuse_program(cnt,prog_trim_value_byte[cnt]);
            }
            //read efuse 134~135 Byte for check program is or not program succesfull
            for(int cnt=135;cnt<137;cnt++)
            {
                //read data
                ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
                if(!(efuse_result==prog_trim_value_byte[cnt])) {fail_cnt++;}
            }
            if(!fail_cnt)
            {
                //program efuse 176~216 BYTE
                for(int cnt=177;cnt<218;cnt++)
                {
                  //program rx_ana_dcoc + tx_imb data
                  ll_efuse_program(cnt,prog_trim_value_byte[cnt]);
                }
                //read efuse 176~216 Byte for check program is or not program succesfull
                for(int cnt=177;cnt<218;cnt++)
                {
                    //read data
                    ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
                    if(!(efuse_result==prog_trim_value_byte[cnt])) {fail_cnt++;}
                }
                if(!fail_cnt)
                {
                    ll_efuse_program(252,prog_trim_value_byte[252]);
                    ll_efuse_read(252,&efuse_result,1,efuse_rd_cycle);
                    if(!(efuse_result==prog_trim_value_byte[252])) {fail_cnt++;}
                    
//                    if(!fail_cnt)
//                    {
//                        //program efuse 255
//                        ll_efuse_program(256,crc_sw);
//                        //read efuse 255 Byte for check program is or not program succesfull
//                        ll_efuse_read(256,&efuse_result,1,efuse_rd_cycle);
//                        if(!(efuse_result==crc_sw)) {fail_cnt++;}
//                    }
                }
            }
        }
        //program efuse result
        if(!(fail_cnt==0))
        {
            return RET_ERR;
        }
        else
        {
            return RET_OK;
        }
#else
    //program efuse 66~125 BYTE
        for(int cnt=66;cnt<125;cnt++)
        {
          //program rx_ana_dcoc + tx_imb data
          ll_efuse_program(cnt,prog_trim_value_byte[cnt]);
        }
        //read efuse 66~125 Byte for check program is or not program succesfull
        for(int cnt=66;cnt<125;cnt++)
        {
            //read data
            ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
            if(!(efuse_result==prog_trim_value_byte[cnt])) {fail_cnt++;}
        }
        //program efuse result
        if(!(fail_cnt==0))
        {
            return RET_ERR;
        }
        else
        {
            return RET_OK;
        }
#endif
    }
    else
    {
        return RET_OK;
    }
}

int32 rf_para_efuse_read(int32 check_flag)
{
    uint8  prog_trim_value_byte[272] = {0};
    uint8  efuse_result = 0;
    uint32 fail_cnt = 0;
    uint32 un_empty_cnt = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    // read rx_ana_dcoc data
    #ifdef TX4104
    for(int cnt=98;cnt<110;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    for(int rx_gain_index=0; rx_gain_index<4; rx_gain_index++) {
        rx_ana_dcoc_efuse_rd_buf[2*rx_gain_index] = ((prog_trim_value_byte[99+3*rx_gain_index]&0x0f)<<8)|(prog_trim_value_byte[98+3*rx_gain_index]&0xff);
        rx_ana_dcoc_efuse_rd_buf[2*rx_gain_index+1] = ((prog_trim_value_byte[100+3*rx_gain_index]&0xff)<<4)|((prog_trim_value_byte[99+3*rx_gain_index]&0xf0)>>4);
    }
    #else
    for(int cnt=66;cnt<78;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    for(int rx_gain_index=0; rx_gain_index<4; rx_gain_index++) {
        rx_ana_dcoc_efuse_rd_buf[2*rx_gain_index] = ((prog_trim_value_byte[67+3*rx_gain_index]&0x0f)<<8)|(prog_trim_value_byte[66+3*rx_gain_index]&0xff);
        rx_ana_dcoc_efuse_rd_buf[2*rx_gain_index+1] = ((prog_trim_value_byte[68+3*rx_gain_index]&0xff)<<4)|((prog_trim_value_byte[67+3*rx_gain_index]&0xf0)>>4);
    }
    #endif
    
    //read rx_imb data
    #ifdef TX4104
    for(int cnt=177;cnt<202;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    for(int rx_gain_index=0; rx_gain_index<4; rx_gain_index++)
    {
        rx_imb_gain_comp_efuse_rd_buf[2*rx_gain_index]   = ((prog_trim_value_byte[178+3*rx_gain_index]&0x0f)<<8)|(prog_trim_value_byte[177+3*rx_gain_index]&0xff);
        rx_imb_gain_comp_efuse_rd_buf[2*rx_gain_index+1] = ((prog_trim_value_byte[179+3*rx_gain_index]&0xff)<<4)|((prog_trim_value_byte[178+3*rx_gain_index]&0xf0)>>4);
        
        rx_imb_phase_comp_efuse_rd_buf[2*rx_gain_index]   = ((prog_trim_value_byte[201]&(0x01<<(7-2*rx_gain_index)))<<(2*rx_gain_index+5))\
                                                            |((prog_trim_value_byte[190+3*rx_gain_index]&0x0f)<<8)|(prog_trim_value_byte[189+3*rx_gain_index]&0xff);
        rx_imb_phase_comp_efuse_rd_buf[2*rx_gain_index+1] = ((prog_trim_value_byte[201]&(0x01<<(6-2*rx_gain_index)))<<(2*rx_gain_index+1+5))\
                                                            |((prog_trim_value_byte[191+3*rx_gain_index]&0xff)<<4)|((prog_trim_value_byte[190+3*rx_gain_index]&0xf0)>>4);
    }
    #else
    for(int cnt=84;cnt<109;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    for(int rx_gain_index=0; rx_gain_index<4; rx_gain_index++)
    {
        rx_imb_gain_comp_efuse_rd_buf[2*rx_gain_index]   = ((prog_trim_value_byte[85+3*rx_gain_index]&0x0f)<<8)|(prog_trim_value_byte[84+3*rx_gain_index]&0xff);
        rx_imb_gain_comp_efuse_rd_buf[2*rx_gain_index+1] = ((prog_trim_value_byte[86+3*rx_gain_index]&0xff)<<4)|((prog_trim_value_byte[85+3*rx_gain_index]&0xf0)>>4);
        
        rx_imb_phase_comp_efuse_rd_buf[2*rx_gain_index]   = ((prog_trim_value_byte[108]&(0x01<<(7-2*rx_gain_index)))<<(2*rx_gain_index+5))\
                                                            |((prog_trim_value_byte[97+3*rx_gain_index]&0x0f)<<8)|(prog_trim_value_byte[96+3*rx_gain_index]&0xff);
        rx_imb_phase_comp_efuse_rd_buf[2*rx_gain_index+1] = ((prog_trim_value_byte[108]&(0x01<<(6-2*rx_gain_index)))<<(2*rx_gain_index+1+5))\
                                                            |((prog_trim_value_byte[98+3*rx_gain_index]&0xff)<<4)|((prog_trim_value_byte[97+3*rx_gain_index]&0xf0)>>4);
    }
    #endif
    //read rx_dcoc data
    #ifdef TX4104
    for(int cnt=202;cnt<218;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    for(int rx_gain_index=0; rx_gain_index<8; rx_gain_index++) {
        rx_dcoc_res_efuse_rd_buf[rx_gain_index][0] = prog_trim_value_byte[202+2*rx_gain_index];
        rx_dcoc_res_efuse_rd_buf[rx_gain_index][1] = prog_trim_value_byte[203+2*rx_gain_index];
    }
    #else
    for(int cnt=109;cnt<125;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    for(int rx_gain_index=0; rx_gain_index<8; rx_gain_index++) {
        rx_dcoc_res_efuse_rd_buf[rx_gain_index][0] = prog_trim_value_byte[109+2*rx_gain_index];
        rx_dcoc_res_efuse_rd_buf[rx_gain_index][1] = prog_trim_value_byte[110+2*rx_gain_index];
    }
    #endif
    
    //read tx_dcoc data
    #ifdef TX4104
    for(int cnt=135;cnt<137;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    
    tx_ipa_dcoc_res_efuse_rd_buf[0] = prog_trim_value_byte[135];
    tx_ipa_dcoc_res_efuse_rd_buf[1] = prog_trim_value_byte[136];
    #else
    for(int cnt=82;cnt<84;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    
    tx_ipa_dcoc_res_efuse_rd_buf[0] = prog_trim_value_byte[82];
    tx_ipa_dcoc_res_efuse_rd_buf[1] = prog_trim_value_byte[83];
    #endif
    
    #ifdef TX4104
    for(int cnt=110;cnt<135;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    //read tx_imb data
    tx_ipa_imb_res_gm_efuse_rd_buf = ((prog_trim_value_byte[111]&0x0f)<<8)|(prog_trim_value_byte[110]&0xff);
    tx_ipa_imb_res_pm_efuse_rd_buf = ((prog_trim_value_byte[113]&0x01)<<12)|((prog_trim_value_byte[112]&0xff)<<4)|((prog_trim_value_byte[111]&0xf0)>>4);
        
    ll_efuse_read(252,&prog_trim_value_byte[252],1,efuse_rd_cycle);
    xo_cs_post_efuse_rd_buf = prog_trim_value_byte[252]&0x7f;
    
    #else
    for(int cnt=78;cnt<82;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    //read tx_imb data
    tx_ipa_imb_res_gm_efuse_rd_buf = ((prog_trim_value_byte[79]&0x0f)<<8)|(prog_trim_value_byte[78]&0xff);
    tx_ipa_imb_res_pm_efuse_rd_buf = ((prog_trim_value_byte[81]&0x01)<<12)|((prog_trim_value_byte[80]&0xff)<<4)|((prog_trim_value_byte[79]&0xf0)>>4);
    xo_cs_post_efuse_rd_buf = (prog_trim_value_byte[81]&0xfe)>>1;
    #endif
    if(check_flag)
    {
        for(int gain_index=0;gain_index<8;gain_index++)
        {
            if(rx_ana_dcoc_efuse_rd_buf[gain_index] != rx_ana_dcoc[gain_index])
            {
                return RET_ERR;
            }
            else if(rx_imb_gain_comp_efuse_rd_buf[gain_index] != rx_imb_gain_comp[gain_index])
            {
                return RET_ERR;
            }
            else if(rx_imb_phase_comp_efuse_rd_buf[gain_index] != rx_imb_phase_comp[gain_index])
            {
                return RET_ERR;
            }
            else if(rx_dcoc_res_efuse_rd_buf[gain_index][0] != rx_dcoc_res[gain_index][0])
            {
                return RET_ERR;
            }
            else if(rx_dcoc_res_efuse_rd_buf[gain_index][1] != rx_dcoc_res[gain_index][1])
            {
                return RET_ERR;
            }
            else if((tx_ipa_dcoc_res_efuse_rd_buf[0] != tx_ipa_dcoc_res[0])||(tx_ipa_dcoc_res_efuse_rd_buf[1] != tx_ipa_dcoc_res[1]))
            {
                return RET_ERR;
            }
            else if((tx_ipa_imb_res_gm_efuse_rd_buf != tx_ipa_imb_res_gm)||(tx_ipa_imb_res_pm_efuse_rd_buf != tx_ipa_imb_res_pm))
            {
                return RET_ERR;
            }
            else if(xo_cs_post_efuse_rd_buf != xo_cs_post)
            {
                return RET_ERR;
            }
        }
    }
    return RET_OK;

}

uint8 module_area_crc_cal(uint8 *module_area_data)
{
    uint8 crc_sw, crc_hw;
//    uint8 *pBuf = (uint8 *)0x20042000;
//    uint8 *pInitBuf = (uint8 *)pBuf;
    uint8 pBuf[189] = {0};
    #ifdef TX4104
    for(int cnt=67;cnt<256;cnt++)//256
    {
        //read data
        pBuf[cnt-67] = module_area_data[cnt];
//        *pInitBuf = module_area_data[cnt];
//        pInitBuf++;
    }
    crc_sw = crc8_itu((uint8_t *)pBuf, 189);
    #else
    for(int cnt=66;cnt<127;cnt++)//256
    {
        //read data
        pBuf[cnt-66] = module_area_data[cnt];
    }
    crc_sw = crc8(pBuf, 61);
    #endif
    
    return crc_sw;
}

int32 module_area_crc_check(void)
{
    uint8 crc_sw, crc_hw;
//    uint8 *pBuf = (uint8 *)0x20042000;
//    uint8 *pInitBuf = (uint8 *)pBuf;
    uint8 pBuf[189] = {0};

    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    crc_sw = 0;
    crc_hw = 0;
    #ifdef TX4104
    for(int cnt=67;cnt<256;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&pBuf[cnt-67],1,efuse_rd_cycle);
//        ll_efuse_read(cnt,pInitBuf,1,efuse_rd_cycle);
//        pInitBuf++;
    }
    
    ll_efuse_read(256,&crc_hw,1,efuse_rd_cycle);
    
    memcpy(rf_trim_val_crc_chk_pBuf, pBuf, 189);
    
    crc_sw = crc8_itu((uint8_t *)rf_trim_val_crc_chk_pBuf, 189);
    
    #else
    for(int cnt=66;cnt<127;cnt++)
    {
        //read data
        ll_efuse_read(cnt,&pBuf[cnt-66],1,efuse_rd_cycle);
    }
    ll_efuse_read(127,&crc_hw,1,efuse_rd_cycle);
    crc_sw = crc8(pBuf, 61);
    #endif
	
	return RET_OK;
	
    if ((crc_sw == crc_hw)&&(crc_hw != 0)) {
        return RET_OK;
    }
    else{
        return RET_ERR;
    }
}

int32 module_area_crc_empty(void)
{
    uint8  prog_trim_value_byte[272] = {0};
    uint8  module_area_crc = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    #ifdef TX4104
    ll_efuse_read(256,&module_area_crc,1,efuse_rd_cycle);
    #else
    ll_efuse_read(127,&module_area_crc,1,efuse_rd_cycle);
    #endif
    if(module_area_crc==0){
        return RET_OK;
    }
    return RET_ERR;
}

int32 read_efuse_data(void)
{
    uint8 crc_sw, crc_hw;
    
    uint8  prog_trim_value_byte[272] = {0};
    uint8  efuse_result = 0;
    uint32 fail_cnt = 0;
    uint32 un_empty_cnt = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    for(int cnt=0; cnt<272; cnt++)
    {
        prog_trim_value_byte[cnt] = 0;
    }
    
    for(int cnt=0; cnt<272; cnt++)//66
    {
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
    }
    
    return RET_OK;
}

int32 xtal_bios_id_tempr_efuse_write(int8 XTAL_ID,int8 MODULE_ID,int8 BIOS_ID)
{
    uint8  prog_trim_value_byte[272] = {0};
    uint8  efuse_result = 0;
    uint32 fail_cnt = 0;
    uint32 un_empty_cnt = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    for(int cnt=0; cnt<272; cnt++)
    {
        prog_trim_value_byte[cnt] = 0; //clear the efuse write buffer
    }
    #ifdef TX4104
    prog_trim_value_byte[253] = ((MODULE_ID&0x0f)<<4)|(XTAL_ID&0x0f);
    prog_trim_value_byte[254] = BIOS_ID&0xff;
    prog_trim_value_byte[255] = trx_temperature_value&0xff;
    
    //check program region empty
    for(int cnt=253;cnt<256;cnt++)
    {
        ll_efuse_read(cnt,&prog_trim_value_byte[cnt],1,efuse_rd_cycle);
        if(!(efuse_result==0)) {un_empty_cnt++;}
    }
    #else
    prog_trim_value_byte[125] = ((MODULE_ID&0x0f)<<4)|(XTAL_ID&0x0f);
    prog_trim_value_byte[126] = BIOS_ID&0xff;
    
    //check program region empty
    for(int cnt=125;cnt<127;cnt++)
    {
        ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
        if(!(efuse_result==0)) {un_empty_cnt++;}
    }
    #endif
    if(!un_empty_cnt)
    {
        #ifdef TX4104
        for(int cnt=253;cnt<256;cnt++)
        {
            ll_efuse_program(cnt,prog_trim_value_byte[cnt]);
        }

        for(int cnt=253;cnt<256;cnt++)
        {
            //read data
            ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
            if(!(efuse_result==prog_trim_value_byte[cnt])) {fail_cnt++;}
        }
        #else
        for(int cnt=125;cnt<127;cnt++)
        {
            ll_efuse_program(cnt,prog_trim_value_byte[cnt]);
        }

        for(int cnt=125;cnt<127;cnt++)
        {
            //read data
            ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
            if(!(efuse_result==prog_trim_value_byte[cnt])) {fail_cnt++;}
        }
        #endif
        //program efuse result
        if(!(fail_cnt==0))
        {
            return RET_ERR;
        }
        else
        {
            return RET_OK;
        }
    }
    else
    {
        return RET_OK;
    }
}

int32 chip_type_efuse_write(int8 chip_type)
{
    uint8  prog_trim_value_byte[272] = {0};
    uint8  efuse_result = 0;
    uint32 fail_cnt = 0;
    uint32 un_empty_cnt = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    for(int cnt=0; cnt<272; cnt++)
    {
        prog_trim_value_byte[cnt] = 0; //clear the efuse write buffer
    }
    #ifdef TX4104
    prog_trim_value_byte[257] = chip_type;
    
    //check program region empty
    ll_efuse_read(257,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0)) {un_empty_cnt++;}
    #else
    
    #endif
    if(!un_empty_cnt)
    {
        #ifdef TX4104
        ll_efuse_program(257,prog_trim_value_byte[257]);
        //read data
        ll_efuse_read(257,&efuse_result,1,efuse_rd_cycle);
        if(!(efuse_result==prog_trim_value_byte[257])) {fail_cnt++;}
        #else
        
        #endif
        //program efuse result
        if(!(fail_cnt==0))
        {
            return RET_ERR;
        }
        else
        {
            return RET_OK;
        }
    }
    else
    {
        return RET_OK;
    }
}

int32 chip_type_efuse_read(void)
{
    uint8  prog_trim_value_byte[272] = {0};
    uint8  efuse_result = 0;
    uint32 fail_cnt = 0;
    uint32 un_empty_cnt = 0;
	int8 package_id=0;

    uint8  crc_sw = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    for(int cnt=0; cnt<272; cnt++)
    {
        prog_trim_value_byte[cnt] = 0; //clear the efuse write buffer
    }
    #ifdef TX4104
    //check program region empty

    uint16 cnt=0;
    cnt = 257;
    ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0))
    {
        prog_trim_value_byte[cnt] = efuse_result;
        package_id = prog_trim_value_byte[cnt];
    }

	return package_id;
    
    #else
    
    #endif
}

int32 xtal_cap_efuse_read(void)
{
    uint8  prog_trim_value_byte[272] = {0};
    uint8  efuse_result = 0;
    uint32 fail_cnt = 0;
    uint32 un_empty_cnt = 0;
    
    uint8  crc_sw = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    for(int cnt=0; cnt<272; cnt++)
    {
        prog_trim_value_byte[cnt] = 0; //clear the efuse write buffer
    }
    #ifdef TX4104
    //check program region empty
    for(int cnt=260; cnt>257; cnt--)
    {
        ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
        if(!(efuse_result==0))
        {
            prog_trim_value_byte[cnt] = efuse_result;
            xo_fix_sel = (prog_trim_value_byte[cnt] & 0x000000c0)>>6;
            xo_cs_post = (prog_trim_value_byte[cnt] & 0x0000003f)<<1;
            
            return RET_OK;
        }
    }
    ll_efuse_read(252,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0))
    {
        prog_trim_value_byte[252] = efuse_result;
        xo_fix_sel = (prog_trim_value_byte[252] & 0x000000c0)>>6;
        xo_cs_post = (prog_trim_value_byte[252] & 0x0000003f)<<1;
        
        return RET_OK;
    }
    return RET_ERR;
    
    #else
    
    #endif

}

int32 xtal_cap_efuse_write(void)
{
    uint8  prog_trim_value_byte[272] = {0};
    uint8  efuse_result = 0;
    uint32 fail_cnt = 0;
    uint32 un_empty_cnt = 0;
    
    uint8  crc_sw = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    for(int cnt=0; cnt<272; cnt++)
    {
        prog_trim_value_byte[cnt] = 0; //clear the efuse write buffer
    }
    #ifdef TX4104
    prog_trim_value_byte[252] = ((xo_fix_sel & 0x3)<<6)|((xo_cs_post & 0x7e)>>1);
    prog_trim_value_byte[258] = ((xo_fix_sel & 0x3)<<6)|((xo_cs_post & 0x7e)>>1);
    prog_trim_value_byte[259] = ((xo_fix_sel & 0x3)<<6)|((xo_cs_post & 0x7e)>>1);
    prog_trim_value_byte[260] = ((xo_fix_sel & 0x3)<<6)|((xo_cs_post & 0x7e)>>1);
    
    //check program region empty
    ll_efuse_read(260,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0))
    {
        return RET_ERR;
    }
    ll_efuse_read(252,&efuse_result,1,efuse_rd_cycle);
    if(efuse_result==0)
    {
        ll_efuse_program(252,prog_trim_value_byte[252]);
        //read data
        ll_efuse_read(252,&efuse_result,1,efuse_rd_cycle);
        if(!(efuse_result==prog_trim_value_byte[252])) {fail_cnt++;}
    }
    else
    {
        for(int cnt=258;cnt<261;cnt++)
        {
            ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
            if(efuse_result==0)
            {
                ll_efuse_program(cnt,prog_trim_value_byte[cnt]);
                //read data
                ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
                if(!(efuse_result==prog_trim_value_byte[cnt]))
                {
                    fail_cnt++;
                    break;
                }
                break;
            }
        }
    }
    
    #else
    
    #endif
    
    //program efuse result
    if(!(fail_cnt==0))
    {
        return RET_ERR;
    }
    else
    {
        return RET_OK;
    }
}

int32 rf_pmu_dc_efuse_read(void)
{
    uint8  prog_trim_value_byte[272] = {0};
    uint8  efuse_result = 0;
    uint32 fail_cnt = 0;
    uint32 un_empty_cnt = 0;
    
    uint8  crc_sw = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    for(int cnt=0; cnt<272; cnt++)
    {
        prog_trim_value_byte[cnt] = 0; //clear the efuse write buffer
    }
    #ifdef TX4104
    //check program region empty

    uint16  cnt=0;
    cnt = 54;
    ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0))
    {
        prog_trim_value_byte[cnt] = efuse_result;
        RF_LO_VREF = prog_trim_value_byte[cnt] & 0x0f;
    }

    cnt = 53;
    ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0))
    {
        prog_trim_value_byte[cnt] = efuse_result;
        RF_IBPT = prog_trim_value_byte[cnt] & 0x0f;
        RF_IBCT = (prog_trim_value_byte[cnt] & 0xf0)>>4;
    }
    
    cnt = 52;
    ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0))
    {
        prog_trim_value_byte[cnt] = efuse_result;
        RF_VREF = (prog_trim_value_byte[cnt] & 0xf0)>>4;
        
        return RET_OK;
    }

    return RET_ERR;
    
    #else
    
    #endif
}

int32 T_VPAT_VDDI_RATIO_efuse_read(void)
{
    uint8  prog_trim_value_byte[272] = {0};
    uint8  efuse_result = 0;
    uint32 fail_cnt = 0;
    uint32 un_empty_cnt = 0;
    
    uint8  crc_sw = 0;
    
    uint32  efuse_wr_cycle = SYS_CLK/LL_EFUSE_PROGRAM_FREQ;
    uint32  power_cycle = SYS_CLK/LL_EFUSE_POWER_FREQ;
    uint32  efuse_rd_cycle = SYS_CLK/LL_EFUSE_READ_FREQ;
    
    for(int cnt=0; cnt<272; cnt++)
    {
        prog_trim_value_byte[cnt] = 0; //clear the efuse write buffer
    }
    #ifdef TX4104
    //check program region empty

    uint16  cnt=0;
    cnt = 58;
	RF_VPAT_VDDI_RATIO = 0;
    ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0))
    {
        prog_trim_value_byte[cnt] = efuse_result;
        RF_VPAT_VDDI_RATIO = ((prog_trim_value_byte[cnt] & 0xfc)>>2)<<6;
    }

    cnt = 59;
    ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0))
    {
        prog_trim_value_byte[cnt] = efuse_result;
        RF_VPAT_VDDI_RATIO |= prog_trim_value_byte[cnt] & 0x3f;
    }
    
    cnt = 65;
	RF_VDDI=0;
    ll_efuse_read(cnt,&efuse_result,1,efuse_rd_cycle);
    if(!(efuse_result==0))
    {
        prog_trim_value_byte[cnt] = efuse_result;
        RF_VDDI = prog_trim_value_byte[cnt];
        
        return RET_OK;
    }

    return RET_ERR;
    
    #else
    
    #endif
}
