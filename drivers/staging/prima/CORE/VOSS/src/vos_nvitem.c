/*
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*============================================================================
  FILE:         vos_nvitem.c
  OVERVIEW:     This source file contains definitions for vOS NV Item APIs
  DEPENDENCIES: NV, remote API client, WinCE REX
                Copyright (c) 2008 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*                                                                            
                         
                                                                            */
//                                                                       
//                         

/*                                                                            
                
                                                                            */
#include "vos_types.h"
#include "aniGlobal.h"
#include "vos_nvitem.h"
#include "vos_trace.h"
#include "vos_api.h"
#include "wlan_hdd_misc.h"
#include "vos_sched.h"
#include "sme_Api.h"
#include "wlan_nv_parser.h"
#include "wlan_hdd_main.h"
#include <net/cfg80211.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0))
#define IEEE80211_CHAN_NO_80MHZ		1<<7
#endif

#ifdef CONFIG_ENABLE_LINUX_REG

static v_REGDOMAIN_t cur_reg_domain = REGDOMAIN_COUNT;
static char linux_reg_cc[2] = {0, 0};
static v_REGDOMAIN_t temp_reg_domain = REGDOMAIN_COUNT;

#else

/*                                                                                           */
static struct completion change_country_code;

#endif

static char crda_alpha2[2] = {0, 0}; /*                                    */
static char run_time_alpha2[2] = {0, 0}; /*                                            */
static v_BOOL_t crda_regulatory_entry_valid = VOS_FALSE;
static v_BOOL_t crda_regulatory_run_time_entry_valid = VOS_FALSE;

/*                                                                            
                                         
                                                                            */
#define VALIDITY_BITMAP_NV_ID    NV_WLAN_VALIDITY_BITMAP_I
#define VALIDITY_BITMAP_SIZE     32
#define MAX_COUNTRY_COUNT        300
//                                                 
#define VOS_HARD_CODED_MAC    {0, 0x0a, 0xf5, 4, 5, 6}

#define DEFAULT_NV_VALIDITY_BITMAP 0xFFFFFFFF
#define MAGIC_NUMBER            0xCAFEBABE

/*                                                                            
                    
                                                                            */
//                                                              
//                                                                        
//                                                                      

//                                                             
typedef struct
{
   v_U8_t            regDomain;
   v_COUNTRYCODE_t   countryCode;
} CountryInfo_t;
//                                                             
typedef struct
{
   v_U16_t           countryCount;
   CountryInfo_t     countryInfo[MAX_COUNTRY_COUNT];
} CountryInfoTable_t;
/*                                                                            
                          
                                                                            */
/*                                                                            
                              
                                                                            */
//                             
//                                                
//                                             

#ifdef CONFIG_ENABLE_LINUX_REG

static CountryInfoTable_t countryInfoTable =
{
	254,
	{
		{REGDOMAIN_APAC, {'A', 'G'}},
		{REGDOMAIN_APAC, {'B', 'B'}},
		{REGDOMAIN_APAC, {'B', 'H'}},
		{REGDOMAIN_APAC, {'B', 'N'}},
		{REGDOMAIN_APAC, {'B', 'S'}},
		{REGDOMAIN_APAC, {'C', 'L'}},
		{REGDOMAIN_APAC, {'C', 'N'}},
		{REGDOMAIN_APAC, {'E', 'G'}},
		{REGDOMAIN_APAC, {'I', 'N'}},
		{REGDOMAIN_APAC, {'M', 'V'}},
		{REGDOMAIN_APAC, {'M', 'Y'}},
		{REGDOMAIN_APAC, {'P', 'A'}},
		{REGDOMAIN_APAC, {'R', 'U'}},
		{REGDOMAIN_APAC, {'S', 'G'}},
		{REGDOMAIN_APAC, {'S', 'V'}},
		{REGDOMAIN_APAC, {'V', 'E'}},
		{REGDOMAIN_APAC, {'Z', 'M'}},
		{REGDOMAIN_ETSI, {'A', 'D'}},
		{REGDOMAIN_ETSI, {'A', 'L'}},
		{REGDOMAIN_ETSI, {'A', 'T'}},
		{REGDOMAIN_ETSI, {'B', 'A'}},
		{REGDOMAIN_ETSI, {'B', 'E'}},
		{REGDOMAIN_ETSI, {'B', 'G'}},
		{REGDOMAIN_ETSI, {'B', 'Y'}},
		{REGDOMAIN_ETSI, {'C', 'H'}},
		{REGDOMAIN_ETSI, {'C', 'V'}},
		{REGDOMAIN_ETSI, {'C', 'Y'}},
		{REGDOMAIN_ETSI, {'C', 'Z'}},
		{REGDOMAIN_ETSI, {'D', 'E'}},
		{REGDOMAIN_ETSI, {'D', 'K'}},
		{REGDOMAIN_ETSI, {'D', 'Z'}},
		{REGDOMAIN_ETSI, {'E', 'E'}},
		{REGDOMAIN_ETSI, {'E', 'S'}},
		{REGDOMAIN_ETSI, {'E', 'T'}},
		{REGDOMAIN_ETSI, {'F', 'I'}},
		{REGDOMAIN_ETSI, {'F', 'R'}},
		{REGDOMAIN_ETSI, {'G', 'B'}},
		{REGDOMAIN_ETSI, {'G', 'E'}},
		{REGDOMAIN_ETSI, {'G', 'F'}},
		{REGDOMAIN_ETSI, {'G', 'H'}},
		{REGDOMAIN_ETSI, {'G', 'P'}},
		{REGDOMAIN_ETSI, {'G', 'R'}},
		{REGDOMAIN_ETSI, {'H', 'R'}},
		{REGDOMAIN_ETSI, {'H', 'U'}},
		{REGDOMAIN_ETSI, {'I', 'E'}},
		{REGDOMAIN_ETSI, {'I', 'Q'}},
		{REGDOMAIN_ETSI, {'I', 'S'}},
		{REGDOMAIN_ETSI, {'I', 'T'}},
		{REGDOMAIN_ETSI, {'K', 'E'}},
		{REGDOMAIN_ETSI, {'L', 'I'}},
		{REGDOMAIN_ETSI, {'L', 'S'}},
		{REGDOMAIN_ETSI, {'L', 'T'}},
		{REGDOMAIN_ETSI, {'L', 'U'}},
		{REGDOMAIN_ETSI, {'L', 'V'}},
		{REGDOMAIN_ETSI, {'M', 'C'}},
		{REGDOMAIN_ETSI, {'M', 'D'}},
		{REGDOMAIN_ETSI, {'M', 'E'}},
		{REGDOMAIN_ETSI, {'M', 'F'}},
		{REGDOMAIN_ETSI, {'M', 'K'}},
		{REGDOMAIN_ETSI, {'M', 'M'}},
		{REGDOMAIN_ETSI, {'M', 'Q'}},
		{REGDOMAIN_ETSI, {'M', 'R'}},
		{REGDOMAIN_ETSI, {'M', 'S'}},
		{REGDOMAIN_ETSI, {'M', 'T'}},
		{REGDOMAIN_ETSI, {'M', 'U'}},
		{REGDOMAIN_ETSI, {'N', 'L'}},
		{REGDOMAIN_ETSI, {'N', 'O'}},
		{REGDOMAIN_ETSI, {'O', 'M'}},
		{REGDOMAIN_ETSI, {'P', 'F'}},
		{REGDOMAIN_ETSI, {'P', 'L'}},
		{REGDOMAIN_ETSI, {'P', 'T'}},
		{REGDOMAIN_ETSI, {'R', 'E'}},
		{REGDOMAIN_ETSI, {'R', 'O'}},
		{REGDOMAIN_ETSI, {'R', 'S'}},
		{REGDOMAIN_ETSI, {'S', 'E'}},
		{REGDOMAIN_ETSI, {'S', 'I'}},
		{REGDOMAIN_ETSI, {'S', 'K'}},
		{REGDOMAIN_ETSI, {'S', 'M'}},
		{REGDOMAIN_ETSI, {'S', 'N'}},
		{REGDOMAIN_ETSI, {'T', 'C'}},
		{REGDOMAIN_ETSI, {'T', 'F'}},
		{REGDOMAIN_ETSI, {'T', 'R'}},
		{REGDOMAIN_ETSI, {'V', 'A'}},
		{REGDOMAIN_ETSI, {'V', 'G'}},
		{REGDOMAIN_ETSI, {'Y', 'T'}},
		{REGDOMAIN_ETSI, {'Z', 'A'}},
		{REGDOMAIN_FCC, {'U', 'S'}},
		{REGDOMAIN_FCC, {'A', 'I'}},
		{REGDOMAIN_FCC, {'A', 'S'}},
		{REGDOMAIN_FCC, {'B', 'M'}},
		{REGDOMAIN_FCC, {'C', 'A'}},
		{REGDOMAIN_FCC, {'F', 'M'}},
		{REGDOMAIN_FCC, {'G', 'U'}},
		{REGDOMAIN_FCC, {'K', 'Y'}},
		{REGDOMAIN_FCC, {'M', 'P'}},
		{REGDOMAIN_FCC, {'P', 'R'}},
		{REGDOMAIN_FCC, {'P', 'W'}},
		{REGDOMAIN_FCC, {'V', 'I'}},
		{REGDOMAIN_HI_5GHZ, {'B', 'F'}},
		{REGDOMAIN_HI_5GHZ, {'G', 'Y'}},
		{REGDOMAIN_HI_5GHZ, {'H', 'N'}},
		{REGDOMAIN_HI_5GHZ, {'H', 'T'}},
		{REGDOMAIN_HI_5GHZ, {'J', 'M'}},
		{REGDOMAIN_HI_5GHZ, {'M', 'O'}},
		{REGDOMAIN_HI_5GHZ, {'M', 'W'}},
		{REGDOMAIN_HI_5GHZ, {'P', 'K'}},
		{REGDOMAIN_HI_5GHZ, {'Q', 'A'}},
		{REGDOMAIN_HI_5GHZ, {'R', 'W'}},
		{REGDOMAIN_HI_5GHZ, {'T', 'Z'}},
		{REGDOMAIN_JAPAN, {'J', 'P'}},
		{REGDOMAIN_KOREA, {'K', 'R'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'A', 'M'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'I', 'L'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'K', 'W'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'M', 'A'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'N', 'E'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'T', 'N'}},
		{REGDOMAIN_NO_5GHZ, {'A', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'D'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'I'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'J'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'Z'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'D'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'F'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'G'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'I'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'D', 'J'}},
		{REGDOMAIN_NO_5GHZ, {'E', 'R'}},
		{REGDOMAIN_NO_5GHZ, {'F', 'J'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'A'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'N'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'Q'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'W'}},
		{REGDOMAIN_NO_5GHZ, {'K', 'G'}},
		{REGDOMAIN_NO_5GHZ, {'K', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'K', 'Z'}},
		{REGDOMAIN_NO_5GHZ, {'L', 'Y'}},
		{REGDOMAIN_NO_5GHZ, {'M', 'G'}},
		{REGDOMAIN_NO_5GHZ, {'M', 'L'}},
		{REGDOMAIN_NO_5GHZ, {'N', 'C'}},
		{REGDOMAIN_NO_5GHZ, {'N', 'R'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'B'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'C'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'L'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'R'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'T'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'Y'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'Z'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'D'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'G'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'J'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'V'}},
		{REGDOMAIN_NO_5GHZ, {'V', 'U'}},
		{REGDOMAIN_NO_5GHZ, {'Y', 'E'}},
		{REGDOMAIN_NO_5GHZ, {'A', 'Q'}},
		{REGDOMAIN_NO_5GHZ, {'A', 'X'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'V'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'C'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'K'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'U'}},
		{REGDOMAIN_NO_5GHZ, {'E', 'H'}},
		{REGDOMAIN_NO_5GHZ, {'E', 'U'}},
		{REGDOMAIN_NO_5GHZ, {'F', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'G'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'I'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'L'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'S'}},
		{REGDOMAIN_NO_5GHZ, {'H', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'I', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'I', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'I', 'R'}},
		{REGDOMAIN_NO_5GHZ, {'J', 'E'}},
		{REGDOMAIN_NO_5GHZ, {'K', 'P'}},
		{REGDOMAIN_NO_5GHZ, {'M', 'H'}},
		{REGDOMAIN_NO_5GHZ, {'N', 'F'}},
		{REGDOMAIN_NO_5GHZ, {'N', 'U'}},
		{REGDOMAIN_NO_5GHZ, {'P', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'P', 'N'}},
		{REGDOMAIN_NO_5GHZ, {'P', 'S'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'D'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'H'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'J'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'K'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'L'}},
		{REGDOMAIN_NO_5GHZ, {'U', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'W', 'F'}},
		{REGDOMAIN_WORLD, {'A', 'N'}},
		{REGDOMAIN_WORLD, {'A', 'Z'}},
		{REGDOMAIN_WORLD, {'B', 'R'}},
		{REGDOMAIN_WORLD, {'B', 'W'}},
		{REGDOMAIN_WORLD, {'C', 'O'}},
		{REGDOMAIN_WORLD, {'C', 'R'}},
		{REGDOMAIN_WORLD, {'D', 'M'}},
		{REGDOMAIN_WORLD, {'D', 'O'}},
		{REGDOMAIN_WORLD, {'E', 'C'}},
		{REGDOMAIN_WORLD, {'F', 'K'}},
		{REGDOMAIN_WORLD, {'G', 'D'}},
		{REGDOMAIN_WORLD, {'G', 'T'}},
		{REGDOMAIN_WORLD, {'H', 'K'}},
		{REGDOMAIN_WORLD, {'K', 'H'}},
		{REGDOMAIN_WORLD, {'K', 'N'}},
		{REGDOMAIN_WORLD, {'L', 'B'}},
		{REGDOMAIN_WORLD, {'L', 'C'}},
		{REGDOMAIN_WORLD, {'L', 'K'}},
		{REGDOMAIN_WORLD, {'L', 'R'}},
		{REGDOMAIN_WORLD, {'M', 'N'}},
		{REGDOMAIN_WORLD, {'M', 'Z'}},
		{REGDOMAIN_WORLD, {'N', 'A'}},
		{REGDOMAIN_WORLD, {'N', 'I'}},
		{REGDOMAIN_WORLD, {'N', 'Z'}},
		{REGDOMAIN_WORLD, {'P', 'E'}},
		{REGDOMAIN_WORLD, {'P', 'H'}},
		{REGDOMAIN_WORLD, {'P', 'Y'}},
		{REGDOMAIN_WORLD, {'T', 'H'}},
		{REGDOMAIN_WORLD, {'T', 'T'}},
		{REGDOMAIN_WORLD, {'U', 'Y'}},
		{REGDOMAIN_WORLD, {'U', 'Z'}},
		{REGDOMAIN_WORLD, {'V', 'C'}},
		{REGDOMAIN_WORLD, {'V', 'N'}},
		{REGDOMAIN_WORLD, {'Z', 'W'}},
		{REGDOMAIN_WORLD, {'A', 'R'}},
		{REGDOMAIN_WORLD, {'A', 'U'}},
		{REGDOMAIN_WORLD, {'B', 'T'}},
		{REGDOMAIN_WORLD, {'C', 'X'}},
		{REGDOMAIN_WORLD, {'K', 'I'}},
		{REGDOMAIN_WORLD, {'M', 'X'}},
		{REGDOMAIN_WORLD, {'A', 'E'}},
		{REGDOMAIN_WORLD, {'A', 'W'}},
		{REGDOMAIN_WORLD, {'L', 'A'}},
		{REGDOMAIN_WORLD, {'S', 'A'}},
		{REGDOMAIN_WORLD, {'U', 'G'}},
		{REGDOMAIN_WORLD, {'A', 'F'}},
		{REGDOMAIN_WORLD, {'J', 'O'}},
		{REGDOMAIN_WORLD, {'P', 'G'}},
		{REGDOMAIN_WORLD, {'I', 'D'}},
		{REGDOMAIN_WORLD, {'N', 'P'}},
		{REGDOMAIN_WORLD, {'W', 'S'}},
		{REGDOMAIN_WORLD, {'N', 'G'}},
		{REGDOMAIN_WORLD, {'T', 'W'}},
		{REGDOMAIN_WORLD, {'U', 'A'}},
		{REGDOMAIN_JAPAN, {'J', '1'}},
		{REGDOMAIN_JAPAN, {'J', '2'}},
		{REGDOMAIN_JAPAN, {'J', '3'}},
		{REGDOMAIN_JAPAN, {'J', '4'}},
		{REGDOMAIN_JAPAN, {'J', '5'}},
		{REGDOMAIN_KOREA, {'K', '1'}},
		{REGDOMAIN_KOREA, {'K', '2'}},
		{REGDOMAIN_KOREA, {'K', '3'}},
		{REGDOMAIN_KOREA, {'K', '4'}},
	}
};

#else

//                             
//                                                
//                                             
static CountryInfoTable_t countryInfoTable =
{
	254,
	{
		{REGDOMAIN_APAC, {'A', 'G'}},
		{REGDOMAIN_APAC, {'B', 'B'}},
		{REGDOMAIN_APAC, {'B', 'H'}},
		{REGDOMAIN_APAC, {'B', 'N'}},
		{REGDOMAIN_APAC, {'B', 'S'}},
		{REGDOMAIN_APAC, {'C', 'L'}},
		{REGDOMAIN_APAC, {'C', 'N'}},
		{REGDOMAIN_APAC, {'E', 'G'}},
		{REGDOMAIN_APAC, {'I', 'N'}},
		{REGDOMAIN_APAC, {'M', 'V'}},
		{REGDOMAIN_APAC, {'M', 'Y'}},
		{REGDOMAIN_APAC, {'P', 'A'}},
		{REGDOMAIN_APAC, {'R', 'U'}},
		{REGDOMAIN_APAC, {'S', 'G'}},
		{REGDOMAIN_APAC, {'S', 'V'}},
		{REGDOMAIN_APAC, {'V', 'E'}},
		{REGDOMAIN_APAC, {'Z', 'M'}},
		{REGDOMAIN_ETSI, {'A', 'D'}},
		{REGDOMAIN_ETSI, {'A', 'L'}},
		{REGDOMAIN_ETSI, {'A', 'T'}},
		{REGDOMAIN_ETSI, {'B', 'A'}},
		{REGDOMAIN_ETSI, {'B', 'E'}},
		{REGDOMAIN_ETSI, {'B', 'G'}},
		{REGDOMAIN_ETSI, {'B', 'Y'}},
		{REGDOMAIN_ETSI, {'C', 'H'}},
		{REGDOMAIN_ETSI, {'C', 'V'}},
		{REGDOMAIN_ETSI, {'C', 'Y'}},
		{REGDOMAIN_ETSI, {'C', 'Z'}},
		{REGDOMAIN_ETSI, {'D', 'E'}},
		{REGDOMAIN_ETSI, {'D', 'K'}},
		{REGDOMAIN_ETSI, {'D', 'Z'}},
		{REGDOMAIN_ETSI, {'E', 'E'}},
		{REGDOMAIN_ETSI, {'E', 'S'}},
		{REGDOMAIN_ETSI, {'E', 'T'}},
		{REGDOMAIN_ETSI, {'F', 'I'}},
		{REGDOMAIN_ETSI, {'F', 'R'}},
		{REGDOMAIN_ETSI, {'G', 'B'}},
		{REGDOMAIN_ETSI, {'G', 'E'}},
		{REGDOMAIN_ETSI, {'G', 'F'}},
		{REGDOMAIN_ETSI, {'G', 'H'}},
		{REGDOMAIN_ETSI, {'G', 'P'}},
		{REGDOMAIN_ETSI, {'G', 'R'}},
		{REGDOMAIN_ETSI, {'H', 'R'}},
		{REGDOMAIN_ETSI, {'H', 'U'}},
		{REGDOMAIN_ETSI, {'I', 'E'}},
		{REGDOMAIN_ETSI, {'I', 'Q'}},
		{REGDOMAIN_ETSI, {'I', 'S'}},
		{REGDOMAIN_ETSI, {'I', 'T'}},
		{REGDOMAIN_ETSI, {'K', 'E'}},
		{REGDOMAIN_ETSI, {'L', 'I'}},
		{REGDOMAIN_ETSI, {'L', 'S'}},
		{REGDOMAIN_ETSI, {'L', 'T'}},
		{REGDOMAIN_ETSI, {'L', 'U'}},
		{REGDOMAIN_ETSI, {'L', 'V'}},
		{REGDOMAIN_ETSI, {'M', 'C'}},
		{REGDOMAIN_ETSI, {'M', 'D'}},
		{REGDOMAIN_ETSI, {'M', 'E'}},
		{REGDOMAIN_ETSI, {'M', 'F'}},
		{REGDOMAIN_ETSI, {'M', 'K'}},
		{REGDOMAIN_ETSI, {'M', 'M'}},
		{REGDOMAIN_ETSI, {'M', 'Q'}},
		{REGDOMAIN_ETSI, {'M', 'R'}},
		{REGDOMAIN_ETSI, {'M', 'S'}},
		{REGDOMAIN_ETSI, {'M', 'T'}},
		{REGDOMAIN_ETSI, {'M', 'U'}},
		{REGDOMAIN_ETSI, {'N', 'L'}},
		{REGDOMAIN_ETSI, {'N', 'O'}},
		{REGDOMAIN_ETSI, {'O', 'M'}},
		{REGDOMAIN_ETSI, {'P', 'F'}},
		{REGDOMAIN_ETSI, {'P', 'L'}},
		{REGDOMAIN_ETSI, {'P', 'T'}},
		{REGDOMAIN_ETSI, {'R', 'E'}},
		{REGDOMAIN_ETSI, {'R', 'O'}},
		{REGDOMAIN_ETSI, {'R', 'S'}},
		{REGDOMAIN_ETSI, {'S', 'E'}},
		{REGDOMAIN_ETSI, {'S', 'I'}},
		{REGDOMAIN_ETSI, {'S', 'K'}},
		{REGDOMAIN_ETSI, {'S', 'M'}},
		{REGDOMAIN_ETSI, {'S', 'N'}},
		{REGDOMAIN_ETSI, {'T', 'C'}},
		{REGDOMAIN_ETSI, {'T', 'F'}},
		{REGDOMAIN_ETSI, {'T', 'R'}},
		{REGDOMAIN_ETSI, {'V', 'A'}},
		{REGDOMAIN_ETSI, {'V', 'G'}},
		{REGDOMAIN_ETSI, {'Y', 'T'}},
		{REGDOMAIN_ETSI, {'Z', 'A'}},
		{REGDOMAIN_FCC, {'U', 'S'}},
		{REGDOMAIN_FCC, {'A', 'I'}},
		{REGDOMAIN_FCC, {'A', 'S'}},
		{REGDOMAIN_FCC, {'B', 'M'}},
		{REGDOMAIN_FCC, {'C', 'A'}},
		{REGDOMAIN_FCC, {'F', 'M'}},
		{REGDOMAIN_FCC, {'G', 'U'}},
		{REGDOMAIN_FCC, {'K', 'Y'}},
		{REGDOMAIN_FCC, {'M', 'P'}},
		{REGDOMAIN_FCC, {'P', 'R'}},
		{REGDOMAIN_FCC, {'P', 'W'}},
		{REGDOMAIN_FCC, {'V', 'I'}},
		{REGDOMAIN_HI_5GHZ, {'B', 'F'}},
		{REGDOMAIN_HI_5GHZ, {'G', 'Y'}},
		{REGDOMAIN_HI_5GHZ, {'H', 'N'}},
		{REGDOMAIN_HI_5GHZ, {'H', 'T'}},
		{REGDOMAIN_HI_5GHZ, {'J', 'M'}},
		{REGDOMAIN_HI_5GHZ, {'M', 'O'}},
		{REGDOMAIN_HI_5GHZ, {'M', 'W'}},
		{REGDOMAIN_HI_5GHZ, {'P', 'K'}},
		{REGDOMAIN_HI_5GHZ, {'Q', 'A'}},
		{REGDOMAIN_HI_5GHZ, {'R', 'W'}},
		{REGDOMAIN_HI_5GHZ, {'T', 'Z'}},
		{REGDOMAIN_JAPAN, {'J', 'P'}},
		{REGDOMAIN_KOREA, {'K', 'R'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'A', 'M'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'I', 'L'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'K', 'W'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'M', 'A'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'N', 'E'}},
		{REGDOMAIN_N_AMER_EXC_FCC, {'T', 'N'}},
		{REGDOMAIN_NO_5GHZ, {'A', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'D'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'I'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'J'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'Z'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'D'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'F'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'G'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'I'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'D', 'J'}},
		{REGDOMAIN_NO_5GHZ, {'E', 'R'}},
		{REGDOMAIN_NO_5GHZ, {'F', 'J'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'A'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'N'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'Q'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'W'}},
		{REGDOMAIN_NO_5GHZ, {'K', 'G'}},
		{REGDOMAIN_NO_5GHZ, {'K', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'K', 'Z'}},
		{REGDOMAIN_NO_5GHZ, {'L', 'Y'}},
		{REGDOMAIN_NO_5GHZ, {'M', 'G'}},
		{REGDOMAIN_NO_5GHZ, {'M', 'L'}},
		{REGDOMAIN_NO_5GHZ, {'N', 'C'}},
		{REGDOMAIN_NO_5GHZ, {'N', 'R'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'B'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'C'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'L'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'R'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'T'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'Y'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'Z'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'D'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'G'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'J'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'V'}},
		{REGDOMAIN_NO_5GHZ, {'V', 'U'}},
		{REGDOMAIN_NO_5GHZ, {'Y', 'E'}},
		{REGDOMAIN_NO_5GHZ, {'A', 'Q'}},
		{REGDOMAIN_NO_5GHZ, {'A', 'X'}},
		{REGDOMAIN_NO_5GHZ, {'B', 'V'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'C'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'K'}},
		{REGDOMAIN_NO_5GHZ, {'C', 'U'}},
		{REGDOMAIN_NO_5GHZ, {'E', 'H'}},
		{REGDOMAIN_NO_5GHZ, {'E', 'U'}},
		{REGDOMAIN_NO_5GHZ, {'F', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'G'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'I'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'L'}},
		{REGDOMAIN_NO_5GHZ, {'G', 'S'}},
		{REGDOMAIN_NO_5GHZ, {'H', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'I', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'I', 'O'}},
		{REGDOMAIN_NO_5GHZ, {'I', 'R'}},
		{REGDOMAIN_NO_5GHZ, {'J', 'E'}},
		{REGDOMAIN_NO_5GHZ, {'K', 'P'}},
		{REGDOMAIN_NO_5GHZ, {'M', 'H'}},
		{REGDOMAIN_NO_5GHZ, {'N', 'F'}},
		{REGDOMAIN_NO_5GHZ, {'N', 'U'}},
		{REGDOMAIN_NO_5GHZ, {'P', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'P', 'N'}},
		{REGDOMAIN_NO_5GHZ, {'P', 'S'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'D'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'H'}},
		{REGDOMAIN_NO_5GHZ, {'S', 'J'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'K'}},
		{REGDOMAIN_NO_5GHZ, {'T', 'L'}},
		{REGDOMAIN_NO_5GHZ, {'U', 'M'}},
		{REGDOMAIN_NO_5GHZ, {'W', 'F'}},
		{REGDOMAIN_WORLD, {'A', 'N'}},
		{REGDOMAIN_WORLD, {'A', 'Z'}},
		{REGDOMAIN_WORLD, {'B', 'R'}},
		{REGDOMAIN_WORLD, {'B', 'W'}},
		{REGDOMAIN_WORLD, {'C', 'O'}},
		{REGDOMAIN_WORLD, {'C', 'R'}},
		{REGDOMAIN_WORLD, {'D', 'M'}},
		{REGDOMAIN_WORLD, {'D', 'O'}},
		{REGDOMAIN_WORLD, {'E', 'C'}},
		{REGDOMAIN_WORLD, {'F', 'K'}},
		{REGDOMAIN_WORLD, {'G', 'D'}},
		{REGDOMAIN_WORLD, {'G', 'T'}},
		{REGDOMAIN_WORLD, {'H', 'K'}},
		{REGDOMAIN_WORLD, {'K', 'H'}},
		{REGDOMAIN_WORLD, {'K', 'N'}},
		{REGDOMAIN_WORLD, {'L', 'B'}},
		{REGDOMAIN_WORLD, {'L', 'C'}},
		{REGDOMAIN_WORLD, {'L', 'K'}},
		{REGDOMAIN_WORLD, {'L', 'R'}},
		{REGDOMAIN_WORLD, {'M', 'N'}},
		{REGDOMAIN_WORLD, {'M', 'Z'}},
		{REGDOMAIN_WORLD, {'N', 'A'}},
		{REGDOMAIN_WORLD, {'N', 'I'}},
		{REGDOMAIN_WORLD, {'N', 'Z'}},
		{REGDOMAIN_WORLD, {'P', 'E'}},
		{REGDOMAIN_WORLD, {'P', 'H'}},
		{REGDOMAIN_WORLD, {'P', 'Y'}},
		{REGDOMAIN_WORLD, {'T', 'H'}},
		{REGDOMAIN_WORLD, {'T', 'T'}},
		{REGDOMAIN_WORLD, {'U', 'Y'}},
		{REGDOMAIN_WORLD, {'U', 'Z'}},
		{REGDOMAIN_WORLD, {'V', 'C'}},
		{REGDOMAIN_WORLD, {'V', 'N'}},
		{REGDOMAIN_WORLD, {'Z', 'W'}},
		{REGDOMAIN_WORLD, {'A', 'R'}},
		{REGDOMAIN_WORLD, {'A', 'U'}},
		{REGDOMAIN_WORLD, {'B', 'T'}},
		{REGDOMAIN_WORLD, {'C', 'X'}},
		{REGDOMAIN_WORLD, {'K', 'I'}},
		{REGDOMAIN_WORLD, {'M', 'X'}},
		{REGDOMAIN_WORLD, {'A', 'E'}},
		{REGDOMAIN_WORLD, {'A', 'W'}},
		{REGDOMAIN_WORLD, {'L', 'A'}},
		{REGDOMAIN_WORLD, {'S', 'A'}},
		{REGDOMAIN_WORLD, {'U', 'G'}},
		{REGDOMAIN_WORLD, {'A', 'F'}},
		{REGDOMAIN_WORLD, {'J', 'O'}},
		{REGDOMAIN_WORLD, {'P', 'G'}},
		{REGDOMAIN_WORLD, {'I', 'D'}},
		{REGDOMAIN_WORLD, {'N', 'P'}},
		{REGDOMAIN_WORLD, {'W', 'S'}},
		{REGDOMAIN_WORLD, {'N', 'G'}},
		{REGDOMAIN_WORLD, {'T', 'W'}},
		{REGDOMAIN_WORLD, {'U', 'A'}},
		{REGDOMAIN_JAPAN, {'J', '1'}},
		{REGDOMAIN_JAPAN, {'J', '2'}},
		{REGDOMAIN_JAPAN, {'J', '3'}},
		{REGDOMAIN_JAPAN, {'J', '4'}},
		{REGDOMAIN_JAPAN, {'J', '5'}},
		{REGDOMAIN_KOREA, {'K', '1'}},
		{REGDOMAIN_KOREA, {'K', '2'}},
		{REGDOMAIN_KOREA, {'K', '3'}},
		{REGDOMAIN_KOREA, {'K', '4'}},
	}
};

#endif


typedef struct nvEFSTable_s
{
   v_U32_t    nvValidityBitmap;
   sHalNv     halnv;
} nvEFSTable_t;
nvEFSTable_t *gnvEFSTable;
/*                                           */
static nvEFSTable_t *pnvEFSTable;
static v_U8_t *pnvEncodedBuf;
static v_U8_t *pDictFile;
static v_U8_t *pEncodedBuf;
static v_SIZE_t nvReadEncodeBufSize;
static v_SIZE_t nDictionarySize;
static v_U32_t magicNumber;

/*                                
                                                           
                                                   */
typedef struct nvEFSTableV2_s
{
   v_U32_t    nvValidityBitmap;
   sHalNvV2   halnvV2;
} nvEFSTableV2_t;
nvEFSTableV2_t *gnvEFSTableV2;

const tRfChannelProps rfChannels[NUM_RF_CHANNELS] =
{
    //                  
    //                 
    { 2412, 1  , RF_SUBBAND_2_4_GHZ},        //          
    { 2417, 2  , RF_SUBBAND_2_4_GHZ},        //          
    { 2422, 3  , RF_SUBBAND_2_4_GHZ},        //          
    { 2427, 4  , RF_SUBBAND_2_4_GHZ},        //          
    { 2432, 5  , RF_SUBBAND_2_4_GHZ},        //          
    { 2437, 6  , RF_SUBBAND_2_4_GHZ},        //          
    { 2442, 7  , RF_SUBBAND_2_4_GHZ},        //          
    { 2447, 8  , RF_SUBBAND_2_4_GHZ},        //          
    { 2452, 9  , RF_SUBBAND_2_4_GHZ},        //          
    { 2457, 10 , RF_SUBBAND_2_4_GHZ},        //           
    { 2462, 11 , RF_SUBBAND_2_4_GHZ},        //           
    { 2467, 12 , RF_SUBBAND_2_4_GHZ},        //           
    { 2472, 13 , RF_SUBBAND_2_4_GHZ},        //           
    { 2484, 14 , RF_SUBBAND_2_4_GHZ},        //           
    { 4920, 240, RF_SUBBAND_4_9_GHZ},        //            
    { 4940, 244, RF_SUBBAND_4_9_GHZ},        //            
    { 4960, 248, RF_SUBBAND_4_9_GHZ},        //            
    { 4980, 252, RF_SUBBAND_4_9_GHZ},        //            
    { 5040, 208, RF_SUBBAND_4_9_GHZ},        //            
    { 5060, 212, RF_SUBBAND_4_9_GHZ},        //            
    { 5080, 216, RF_SUBBAND_4_9_GHZ},        //            
    { 5180, 36 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5200, 40 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5220, 44 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5240, 48 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5260, 52 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5280, 56 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5300, 60 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5320, 64 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5500, 100, RF_SUBBAND_5_MID_GHZ},      //            
    { 5520, 104, RF_SUBBAND_5_MID_GHZ},      //            
    { 5540, 108, RF_SUBBAND_5_MID_GHZ},      //            
    { 5560, 112, RF_SUBBAND_5_MID_GHZ},      //            
    { 5580, 116, RF_SUBBAND_5_MID_GHZ},      //            
    { 5600, 120, RF_SUBBAND_5_MID_GHZ},      //            
    { 5620, 124, RF_SUBBAND_5_MID_GHZ},      //            
    { 5640, 128, RF_SUBBAND_5_MID_GHZ},      //            
    { 5660, 132, RF_SUBBAND_5_MID_GHZ},      //            
    { 5680, 136, RF_SUBBAND_5_MID_GHZ},      //            
    { 5700, 140, RF_SUBBAND_5_MID_GHZ},      //            
#ifdef FEATURE_WLAN_CH144
    { 5720, 144, RF_SUBBAND_5_MID_GHZ},      //            
#endif /*                    */
    { 5745, 149, RF_SUBBAND_5_HIGH_GHZ},     //            
    { 5765, 153, RF_SUBBAND_5_HIGH_GHZ},     //            
    { 5785, 157, RF_SUBBAND_5_HIGH_GHZ},     //            
    { 5805, 161, RF_SUBBAND_5_HIGH_GHZ},     //            
    { 5825, 165, RF_SUBBAND_5_HIGH_GHZ},     //            
    { 2422, 3  , NUM_RF_SUBBANDS},           //               
    { 2427, 4  , NUM_RF_SUBBANDS},           //               
    { 2432, 5  , NUM_RF_SUBBANDS},           //               
    { 2437, 6  , NUM_RF_SUBBANDS},           //               
    { 2442, 7  , NUM_RF_SUBBANDS},           //               
    { 2447, 8  , NUM_RF_SUBBANDS},           //               
    { 2452, 9  , NUM_RF_SUBBANDS},           //               
    { 2457, 10 , NUM_RF_SUBBANDS},           //                
    { 2462, 11 , NUM_RF_SUBBANDS},           //                
    { 4930, 242, NUM_RF_SUBBANDS},           //                 
    { 4950, 246, NUM_RF_SUBBANDS},           //                 
    { 4970, 250, NUM_RF_SUBBANDS},           //                 
    { 5050, 210, NUM_RF_SUBBANDS},           //                 
    { 5070, 214, NUM_RF_SUBBANDS},           //                 
    { 5190, 38 , NUM_RF_SUBBANDS},           //                
    { 5210, 42 , NUM_RF_SUBBANDS},           //                
    { 5230, 46 , NUM_RF_SUBBANDS},           //                
    { 5250, 50 , NUM_RF_SUBBANDS},           //                
    { 5270, 54 , NUM_RF_SUBBANDS},           //                
    { 5290, 58 , NUM_RF_SUBBANDS},           //                
    { 5310, 62 , NUM_RF_SUBBANDS},           //                
    { 5510, 102, NUM_RF_SUBBANDS},           //                 
    { 5530, 106, NUM_RF_SUBBANDS},           //                 
    { 5550, 110, NUM_RF_SUBBANDS},           //                 
    { 5570, 114, NUM_RF_SUBBANDS},           //                 
    { 5590, 118, NUM_RF_SUBBANDS},           //                 
    { 5610, 122, NUM_RF_SUBBANDS},           //                 
    { 5630, 126, NUM_RF_SUBBANDS},           //                 
    { 5650, 130, NUM_RF_SUBBANDS},           //                 
    { 5670, 134, NUM_RF_SUBBANDS},           //                 
    { 5690, 138, NUM_RF_SUBBANDS},           //                 
#ifdef FEATURE_WLAN_CH144
    { 5730, 142, NUM_RF_SUBBANDS},           //                 
#endif /*                    */
    { 5755, 151, NUM_RF_SUBBANDS},           //                 
    { 5775, 155, NUM_RF_SUBBANDS},           //                 
    { 5795, 159, NUM_RF_SUBBANDS},           //                 
    { 5815, 163, NUM_RF_SUBBANDS},           //                 
};

extern const sHalNv nvDefaults;

const sRegulatoryChannel * regChannels = nvDefaults.tables.regDomains[0].channels;

/*                                                                            
                                         
                                                                            */
VOS_STATUS wlan_write_to_efs (v_U8_t *pData, v_U16_t data_len);
/*                                                                         
                                                 
                                                                       
                                                          
                                                                 
                                                
     
                                                                           */
VOS_STATUS vos_nv_init(void)
{
   return VOS_STATUS_SUCCESS;
}

/*                                                                         
                                                                            
         
                                                                   
                                     
     
                                                                         */
VOS_STATUS vos_nv_get_dictionary_data(void)
{
   VOS_STATUS vosStatus = VOS_STATUS_E_FAILURE;

   if (MAGIC_NUMBER != magicNumber)
   {
      return VOS_STATUS_SUCCESS;
   }

   nDictionarySize = 0;

   vosStatus = vos_get_binary_blob( VOS_BINARY_ID_DICT_CONFIG, NULL,
                                                &nDictionarySize );
   if (VOS_STATUS_E_NOMEM != vosStatus)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 "Error obtaining binary size" );
//       
//                                               
      return VOS_STATUS_SUCCESS;
   }

   //                                                          
   pDictFile = vos_mem_malloc( nDictionarySize );
   if (NULL == pDictFile)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "Unable to allocate memory for the CFG binary [size= %d bytes]",
                 nDictionarySize );
      vosStatus = VOS_STATUS_E_NOMEM;
      goto fail;
   }

   /*                                  */
   vosStatus = vos_get_binary_blob( VOS_BINARY_ID_DICT_CONFIG, pDictFile,
                                                         &nDictionarySize );
   if (!VOS_IS_STATUS_SUCCESS( vosStatus ))
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "Error: Cannot retrieve CFG file image from vOSS. [size= %d bytes]",
                                                             nDictionarySize );
      return VOS_STATUS_SUCCESS;
   }

   VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
         "Dict file image from vOSS. [size= %d bytes]", nDictionarySize );

fail:
   return vosStatus;
}

/*                                                                         
                                               
                                                                   
                      
                           
                      
                         
              
                                            
                                                                 
                                                
     
                                                                           */
VOS_STATUS vos_nv_parseV2bin(tANI_U8 *pnvEncodedBuf, tANI_U32 nvReadBufSize,
   sHalNv *halNv)
{
   sHalNvV2 *nv2Table;
   tANI_U16 copyLoop;
   tANI_U16 channelLoop;
   void *targetPtr;
   void *sourcePtr;

   v_U32_t structSize = 0;

   nv2Table = (sHalNvV2 *)pnvEncodedBuf;
   /*                       */
   vos_mem_copy((char *)&halNv->fields,
                (char *)&nv2Table->fields,
                sizeof(sNvFields));
   structSize += sizeof(sNvFields);
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(sNvFields) %zu, structSize %d",
              __func__, sizeof(sNvFields), structSize);

   /*                                                       */
   vos_mem_copy((char *)halNv->tables.pwrOptimum,
                (char *)nv2Table->tables.pwrOptimum,
                sizeof(halNv->tables.pwrOptimum));
   structSize += sizeof(halNv->tables.pwrOptimum);
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.pwrOptimum) %zu, structSize %d",
              __func__, sizeof(halNv->tables.pwrOptimum), structSize);

   /*                                                 */
   for (copyLoop = 0; copyLoop < NUM_REG_DOMAINS; copyLoop++)
   {
      vos_mem_copy((char *)halNv->tables.regDomains[copyLoop].antennaGain,
             (char *)nv2Table->tables.regDomains[copyLoop].antennaGain,
             sizeof(halNv->tables.regDomains[copyLoop].antennaGain));
      structSize += sizeof(halNv->tables.regDomains[copyLoop].antennaGain);

      vos_mem_copy((char *)halNv->tables.regDomains[copyLoop].bRatePowerOffset,
             (char *)nv2Table->tables.regDomains[copyLoop].bRatePowerOffset,
             sizeof(halNv->tables.regDomains[copyLoop].bRatePowerOffset));
      structSize += sizeof(halNv->tables.regDomains[copyLoop].bRatePowerOffset);
   }

   for (copyLoop = 0; copyLoop < NUM_REG_DOMAINS; copyLoop++)
   {
      targetPtr = (char *)&(halNv->tables.regDomains[copyLoop].channels[0]);
      sourcePtr = (char *)&(nv2Table->tables.regDomains[copyLoop].channels[0]);
      /*                    
                                           */
      for (channelLoop = 0; channelLoop < NUM_RF_CHANNELS; channelLoop++)
      {
#ifdef FEATURE_WLAN_CH144
         if ((RF_CHAN_144 == channelLoop) || (RF_CHAN_BOND_142 == channelLoop))
         {
            /*                       */
            halNv->tables.regDomains[copyLoop].channels[channelLoop].enabled =
                                    NV_CHANNEL_DISABLE;
            targetPtr = targetPtr + sizeof(sRegulatoryChannel);
         }
         else
#endif /*                    */
         {

            vos_mem_copy(targetPtr, sourcePtr, sizeof(sRegulatoryChannel));
            targetPtr = targetPtr + sizeof(sRegulatoryChannel);
            sourcePtr = sourcePtr + sizeof(sRegulatoryChannel);
            structSize += sizeof(halNv->tables.regDomains[copyLoop].antennaGain);
         }
      }
   }
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.regDomains[copyLoop].antennaGain) %zu, structSize %d",
              __func__, sizeof(halNv->tables.regDomains[copyLoop].antennaGain), structSize);

   for (copyLoop = 0; copyLoop < NUM_REG_DOMAINS; copyLoop++)
   {
      targetPtr = (char *)&(halNv->tables.regDomains[copyLoop].gnRatePowerOffset[0]);
      sourcePtr = (char *)&(nv2Table->tables.regDomains[copyLoop].gnRatePowerOffset[0]);
      /*                    
                                           */
      for (channelLoop = 0; channelLoop < NUM_RF_CHANNELS; channelLoop++)
      {
#ifdef FEATURE_WLAN_CH144
         if ((RF_CHAN_144 == channelLoop) || (RF_CHAN_BOND_142 == channelLoop))
         {
            targetPtr = targetPtr + sizeof(uAbsPwrPrecision);
         }
         else
#endif /*                    */
         {
            vos_mem_copy(targetPtr, sourcePtr, sizeof(uAbsPwrPrecision));
            targetPtr = targetPtr + sizeof(uAbsPwrPrecision);
            sourcePtr = sourcePtr + sizeof(uAbsPwrPrecision);
            structSize += sizeof(sizeof(uAbsPwrPrecision));
         }
      }
   }
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(uAbsPwrPrecision) %zu, structSize %d",
              __func__, sizeof(uAbsPwrPrecision), structSize);

   /*                                                             */
   vos_mem_copy((char *)&halNv->tables.defaultCountryTable,
          (char *)&nv2Table->tables.defaultCountryTable,
          sizeof(halNv->tables.defaultCountryTable));
   structSize += sizeof(halNv->tables.defaultCountryTable);
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.defaultCountryTable) %zu, structSize %d",
              __func__, sizeof(halNv->tables.defaultCountryTable), structSize);

   /*                                                      
                         
                                        */
   targetPtr = (char *)&(halNv->tables.plutCharacterized[0]);
   sourcePtr = (char *)&(nv2Table->tables.plutCharacterized[0]);
   for (channelLoop = 0; channelLoop < NUM_RF_CHANNELS; channelLoop++)
   {
#ifdef FEATURE_WLAN_CH144
      if ((RF_CHAN_144 == channelLoop) || (RF_CHAN_BOND_142 == channelLoop))
      {
         targetPtr = targetPtr + sizeof(tTpcPowerTable);
      }
      else
#endif /*                    */
      {
         vos_mem_copy(targetPtr, sourcePtr, sizeof(tTpcPowerTable));
         targetPtr = targetPtr + sizeof(tTpcPowerTable);
         sourcePtr = sourcePtr + sizeof(tTpcPowerTable);
         structSize += sizeof(tTpcPowerTable);
      }
   }
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(tTpcPowerTable) %zu, structSize %d",
              __func__, sizeof(tTpcPowerTable), structSize);

   /*                                                    
                         
                                        */
   targetPtr = (char *)&(halNv->tables.plutPdadcOffset[0]);
   sourcePtr = (char *)&(nv2Table->tables.plutPdadcOffset[0]);
   for (channelLoop = 0; channelLoop < NUM_RF_CHANNELS; channelLoop++)
   {
#ifdef FEATURE_WLAN_CH144
      if ((RF_CHAN_144 == channelLoop) || (RF_CHAN_BOND_142 == channelLoop))
      {
         targetPtr = targetPtr + sizeof(int16);
      }
      else
#endif /*                    */
      {
         vos_mem_copy(targetPtr, sourcePtr, sizeof(int16));
         targetPtr = targetPtr + sizeof(int16);
         sourcePtr = sourcePtr + sizeof(int16);
         structSize += sizeof(int16);
      }
   }
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.plutPdadcOffset) %zu, structSize %d",
              __func__, sizeof(int16), structSize);

   /*                                                                */
   vos_mem_copy((char *)halNv->tables.pwrOptimum_virtualRate,
          (char *)nv2Table->tables.pwrOptimum_virtualRate,
          sizeof(halNv->tables.pwrOptimum_virtualRate));
   structSize += sizeof(halNv->tables.pwrOptimum_virtualRate);
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.pwrOptimum_virtualRate) %zu, structSize %d",
              __func__, sizeof(halNv->tables.pwrOptimum_virtualRate), structSize);

   /*                                                  */
   vos_mem_copy((char *)&halNv->tables.fwConfig,
          (char *)&nv2Table->tables.fwConfig,
          sizeof(halNv->tables.fwConfig));
   structSize += sizeof(halNv->tables.fwConfig);
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.fwConfig) %zu, structSize %d",
              __func__, sizeof(halNv->tables.fwConfig), structSize);

   /*                                                    
                         
                                        */
   for (copyLoop = 0; copyLoop < 2; copyLoop++)
   {
      targetPtr = (char *)&(halNv->tables.rssiChanOffsets[copyLoop].bRssiOffset[0]);
      sourcePtr = (char *)&(nv2Table->tables.rssiChanOffsets[copyLoop].bRssiOffset[0]);
      /*                    
                                           */
      for (channelLoop = 0; channelLoop < NUM_RF_CHANNELS; channelLoop++)
      {
#ifdef FEATURE_WLAN_CH144
         if ((RF_CHAN_144 == channelLoop) || (RF_CHAN_BOND_142 == channelLoop))
         {
            targetPtr = targetPtr + sizeof(int16);
         }
         else
#endif /*                    */
         {
            vos_mem_copy(targetPtr, sourcePtr, sizeof(int16));
            targetPtr = targetPtr + sizeof(int16);
            sourcePtr = sourcePtr + sizeof(int16);
            structSize += sizeof(int16);
         }
      }
   }
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(tables.rssiChanOffsets) %zu, structSize %d",
              __func__, sizeof(int16), structSize);

   for (copyLoop = 0; copyLoop < 2; copyLoop++)
   {
      targetPtr = (char *)&(halNv->tables.rssiChanOffsets[copyLoop].gnRssiOffset[0]);
      sourcePtr = (char *)&(nv2Table->tables.rssiChanOffsets[copyLoop].gnRssiOffset[0]);
      /*                    
                                           */
      for (channelLoop = 0; channelLoop < NUM_RF_CHANNELS; channelLoop++)
      {
#ifdef FEATURE_WLAN_CH144
         if ((RF_CHAN_144 == channelLoop) || (RF_CHAN_BOND_142 == channelLoop))
         {
            targetPtr = targetPtr + sizeof(int16);
         }
         else
#endif /*                    */
         {
            vos_mem_copy(targetPtr, sourcePtr, sizeof(int16));
            targetPtr = targetPtr + sizeof(int16);
            sourcePtr = sourcePtr + sizeof(int16);
            structSize += sizeof(int16);
         }
      }
   }
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(tables.rssiChanOffsets) %zu, structSize %d",
              __func__, sizeof(int16), structSize);

   /*                                                     */
   vos_mem_copy((char *)&halNv->tables.hwCalValues,
          (char *)&nv2Table->tables.hwCalValues,
          sizeof(halNv->tables.hwCalValues));
   structSize += sizeof(halNv->tables.fwConfig);
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.hwCalValues) %zu, structSize %d",
              __func__, sizeof(halNv->tables.hwCalValues), structSize);

   /*                                                    
                         
                                        */
   targetPtr = (char *)&(halNv->tables.antennaPathLoss[0]);
   sourcePtr = (char *)&(nv2Table->tables.antennaPathLoss[0]);
   for (channelLoop = 0; channelLoop < NUM_RF_CHANNELS; channelLoop++)
   {
#ifdef FEATURE_WLAN_CH144
      if ((RF_CHAN_144 == channelLoop) || (RF_CHAN_BOND_142 == channelLoop))
      {
         targetPtr = targetPtr + sizeof(int16);
      }
      else
#endif /*                    */
      {
         vos_mem_copy(targetPtr, sourcePtr, sizeof(int16));
         targetPtr = targetPtr + sizeof(int16);
         sourcePtr = sourcePtr + sizeof(int16);
         structSize += sizeof(int16);
      }
   }
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.antennaPathLoss) %zu, structSize %d",
              __func__, sizeof(int16), structSize);

   /*                                                     
                         
                                        */
   for (copyLoop = 0; copyLoop < NUM_802_11_MODES; copyLoop++)
   {
      targetPtr = (char *)&(halNv->tables.pktTypePwrLimits[copyLoop][0]);
      sourcePtr = (char *)&(nv2Table->tables.pktTypePwrLimits[copyLoop][0]);
      /*                    
                                           */
      for (channelLoop = 0; channelLoop < NUM_RF_CHANNELS; channelLoop++)
      {
#ifdef FEATURE_WLAN_CH144
         if ((RF_CHAN_144 == channelLoop) || (RF_CHAN_BOND_142 == channelLoop))
         {
            targetPtr = targetPtr + sizeof(int16);
         }
         else
#endif /*                    */
         {
            vos_mem_copy(targetPtr, sourcePtr, sizeof(int16));
            targetPtr = targetPtr + sizeof(int16);
            sourcePtr = sourcePtr + sizeof(int16);
            structSize += sizeof(int16);
         }
      }
   }
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.pktTypePwrLimits) %zu, structSize %d",
              __func__, sizeof(int16), structSize);

   /*                                                          */
   vos_mem_copy((char *)&halNv->tables.ofdmCmdPwrOffset,
          (char *)&nv2Table->tables.ofdmCmdPwrOffset,
          sizeof(halNv->tables.ofdmCmdPwrOffset));
   structSize += sizeof(halNv->tables.ofdmCmdPwrOffset);
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.ofdmCmdPwrOffset) %zu, structSize %d",
              __func__, sizeof(halNv->tables.ofdmCmdPwrOffset), structSize);

   /*                                                        */
   vos_mem_copy((char *)&halNv->tables.txbbFilterMode,
          (char *)&nv2Table->tables.txbbFilterMode,
          sizeof(halNv->tables.txbbFilterMode));
   structSize += sizeof(halNv->tables.ofdmCmdPwrOffset);
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
             "%s: sizeof(halNv->tables.txbbFilterMode) %zu, structSize %d",
              __func__, sizeof(halNv->tables.txbbFilterMode), structSize);

   return VOS_STATUS_SUCCESS;
}

/*                                                                         
                                          
                                                         
                                                                 
                                                
     
                                                                           */
VOS_STATUS vos_nv_open(void)
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    v_CONTEXT_t pVosContext= NULL;
    v_SIZE_t bufSize;
    v_SIZE_t nvReadBufSize;
    v_BOOL_t itemIsValid = VOS_FALSE;
/*                                                                  */
#ifdef CUSTOMER_LGE
    hdd_context_t *pHddCtx = NULL;
#endif
/*                                                                */
    v_U32_t dataOffset;
    sHalNv *pnvData = NULL;

    /*                       */
    pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);

    if (NULL == pVosContext)
    {
        return (eHAL_STATUS_FAILURE);
    }

    status = hdd_request_firmware(WLAN_NV_FILE,
                                  ((VosContextType*)(pVosContext))->pHDDContext,
                                  (v_VOID_t**)&pnvEncodedBuf, &nvReadBufSize);

    if ((!VOS_IS_STATUS_SUCCESS( status )) || (!pnvEncodedBuf))
    {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                   "%s: unable to download NV file %s",
                   __func__, WLAN_NV_FILE);
       return VOS_STATUS_E_RESOURCES;
    }

    memcpy(&magicNumber, &pnvEncodedBuf[sizeof(v_U32_t)], sizeof(v_U32_t));

    //                                       
    pEncodedBuf = (v_U8_t *)vos_mem_malloc(nvReadBufSize);

    if (NULL == pEncodedBuf)
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "%s : failed to allocate memory for NV", __func__);
        return VOS_STATUS_E_NOMEM;
    }

    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
              "NV Table Size %zu", sizeof(nvEFSTable_t));

    pnvEFSTable = (nvEFSTable_t *)vos_mem_malloc(sizeof(nvEFSTable_t));
    if (NULL == pnvEFSTable)
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                  "%s : failed to allocate memory for NV", __func__);
        return VOS_STATUS_E_NOMEM;
    }
    vos_mem_zero((void *)pnvEFSTable, sizeof(nvEFSTable_t));

    //                              
    ((VosContextType*)(pVosContext))->nvVersion = E_NV_INVALID;
    if (MAGIC_NUMBER == magicNumber)
    {
        bufSize = sizeof(nvEFSTable_t);
        gnvEFSTable = (nvEFSTable_t*)pnvEncodedBuf;
        pnvData = (sHalNv *)vos_mem_malloc(sizeof(sHalNv));

        if (NULL == pnvData)
        {
            VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                      "%s : failed to allocate memory for NV", __func__);
            return VOS_STATUS_E_NOMEM;
        }

        memset(pnvData, 0, sizeof(sHalNv));

        //                                                              
        dataOffset = sizeof(v_U32_t) + sizeof(v_U32_t);

        status = nvParser(&pnvEncodedBuf[dataOffset],
                     (nvReadBufSize-dataOffset), pnvData);

        //                        
        nvReadEncodeBufSize = nvReadBufSize - sizeof(v_U32_t);

        vos_mem_copy(pEncodedBuf, &pnvEncodedBuf[sizeof(v_U32_t)],
            nvReadEncodeBufSize);

        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                   "readEncodeBufSize %d",nvReadEncodeBufSize);

        if (VOS_STATUS_SUCCESS == status) {
           VOS_TRACE(VOS_MODULE_ID_VOSS,  VOS_TRACE_LEVEL_ERROR,
                       "Embedded NV parsed success !!productId %d couple Type %d wlan RevId %d",
                        pnvData->fields.productId,
                        pnvData->fields.couplerType,
                        pnvData->fields.wlanNvRevId);

           vos_mem_copy(&gnvEFSTable->halnv, pnvData, sizeof(sHalNv));

           nvReadBufSize = sizeof(sHalNv) + sizeof(v_U32_t);
        }
        else
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS,  VOS_TRACE_LEVEL_ERROR,
                       "nvParser failed %d",status);

           nvReadBufSize = 0;

           vos_mem_copy(pEncodedBuf, &nvDefaults, sizeof(sHalNv));

           nvReadEncodeBufSize = sizeof(sHalNv);
        }
        vos_mem_copy(&(pnvEFSTable->halnv), &nvDefaults, sizeof(sHalNv));

        /*                  */
        ((VosContextType*)(pVosContext))->nvVersion = E_NV_V3;
    }
    else
    {
        bufSize = sizeof(nvEFSTableV2_t);

        /*                        */
        vos_mem_copy(&(pnvEFSTable->halnv), &nvDefaults, sizeof(sHalNv));
        /*                                                      */
        gnvEFSTableV2 = (nvEFSTableV2_t * )pnvEncodedBuf;

        /*              
                                           */
        if (nvReadBufSize != bufSize)
        {
            pnvEFSTable->nvValidityBitmap = DEFAULT_NV_VALIDITY_BITMAP;
            VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                      "!!!WARNING: INVALID NV FILE, DRIVER IS USING DEFAULT CAL VALUES %d %d!!!",
                      nvReadBufSize, bufSize);
            return VOS_STATUS_SUCCESS;
        }

        VOS_TRACE(VOS_MODULE_ID_VOSS,  VOS_TRACE_LEVEL_INFO,
                  "NV_2: readBufferSize %zu, EFSV2DefaultSize %zu",
                  nvReadBufSize, sizeof(nvEFSTableV2_t));

        /*                                                  */
        dataOffset = sizeof(v_U32_t);
        nvReadEncodeBufSize = sizeof(sHalNvV2);
        vos_mem_copy(pEncodedBuf,
                     &pnvEncodedBuf[dataOffset],
                     nvReadBufSize - dataOffset);

#ifdef FEATURE_WLAN_CH144
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
           "Default NV2 size %zu", sizeof(nvDefaultsV2));
#else
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
           "Default NV2 size %zu", sizeof(nvDefaults));
#endif /*                    */
        /*                                    */
#ifdef FEATURE_WLAN_CH144
        vos_nv_parseV2bin((tANI_U8 *)&nvDefaultsV2,
                          sizeof(sHalNvV2),
                          &pnvEFSTable->halnv);
#else
        vos_nv_parseV2bin((tANI_U8 *)&nvDefaults,
                          sizeof(sHalNvV2),
                          &pnvEFSTable->halnv);
#endif /*                    */

        /*                           */
        vos_nv_parseV2bin(pEncodedBuf,
                          nvReadEncodeBufSize,
                          &pnvEFSTable->halnv);

        vos_mem_copy((void *)&pnvEFSTable->nvValidityBitmap,
                     pnvEncodedBuf, sizeof(v_U32_t));
        gnvEFSTable = pnvEFSTable;

        /*                  */
        ((VosContextType*)(pVosContext))->nvVersion = E_NV_V2;
    }

    if (NULL != pnvData)
    {
       vos_mem_free(pnvData);
    }

    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "INFO: NV binary file version=%d Driver default NV version=%d, continue...\n",
           gnvEFSTable->halnv.fields.nvVersion, WLAN_NV_VERSION);

     /*                                                    */
    {
       /*                  */
       if (gnvEFSTable->halnv.fields.nvVersion != WLAN_NV_VERSION)
       {
           if ((WLAN_NV_VERSION == NV_VERSION_11N_11AC_FW_CONFIG) &&
               (gnvEFSTable->halnv.fields.nvVersion == NV_VERSION_11N_11AC_COUPER_TYPE))
           {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "!!!WARNING: Using Coupler Type field instead of Fw Config table,\n"
                     "Make sure that this is intented or may impact performance!!!\n");
           }
#ifdef FEATURE_WLAN_CH144
           else if ((WLAN_NV_VERSION == NV_VERSION_CH144_CONFIG) &&
                    (((VosContextType*)(pVosContext))->nvVersion == E_NV_V2))
           {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "!!!WARNING: Default NV is NV3 CH144 "
                     "BIN is NV2, NV2 contents will be used!!!");
           }
#endif /*                    */
           else
           {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "!!!WARNING: NV binary file version doesn't match with Driver default NV version\n"
                     "Driver NV defaults will be used, may impact performance!!!\n");

               return VOS_STATUS_SUCCESS;
           }
       }

       pnvEFSTable->nvValidityBitmap = gnvEFSTable->nvValidityBitmap;
        /*                                                  */
        if (vos_nv_getValidity(VNV_FIELD_IMAGE, &itemIsValid) ==
           VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE) {

                if(vos_nv_read( VNV_FIELD_IMAGE, (v_VOID_t *)&pnvEFSTable->halnv.fields,
                   NULL, sizeof(sNvFields) ) != VOS_STATUS_SUCCESS)
                   goto error;
            }
        }

        if (vos_nv_getValidity(VNV_RATE_TO_POWER_TABLE, &itemIsValid) ==
             VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_RATE_TO_POWER_TABLE,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.pwrOptimum[0],
                  NULL, sizeof(tRateGroupPwr) * NUM_RF_SUBBANDS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_REGULARTORY_DOMAIN_TABLE, &itemIsValid) ==
               VOS_STATUS_SUCCESS)
        {

            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_REGULARTORY_DOMAIN_TABLE,
                (v_VOID_t *)&pnvEFSTable->halnv.tables.regDomains[0],
                 NULL, sizeof(sRegulatoryDomains) * NUM_REG_DOMAINS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_DEFAULT_LOCATION, &itemIsValid) ==
            VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_DEFAULT_LOCATION,
                (v_VOID_t *)&pnvEFSTable->halnv.tables.defaultCountryTable,
                NULL, sizeof(sDefaultCountry) ) !=  VOS_STATUS_SUCCESS)
                    goto error;
            }
/*                                                                  */
#ifdef CUSTOMER_LGE
            pHddCtx = vos_get_context(VOS_MODULE_ID_HDD, pVosContext);
            if (NULL != pHddCtx) {
                if (!vos_mem_compare(pHddCtx->cfg_ini->overrideCountryCode, "000", 3)) {
                    vos_mem_copy(pnvEFSTable->halnv.tables.defaultCountryTable.countryCode, pHddCtx->cfg_ini->overrideCountryCode, 3);
                }
	        } else {
                VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, ("Invalid pHddCtx pointer"));
	        }
#endif
/*                                                                */
        }

        if (vos_nv_getValidity(VNV_TPC_POWER_TABLE, &itemIsValid) ==
            VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_TPC_POWER_TABLE,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.plutCharacterized[0],
                  NULL, sizeof(tTpcPowerTable) * NUM_RF_CHANNELS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_TPC_PDADC_OFFSETS, &itemIsValid) ==
            VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_TPC_PDADC_OFFSETS,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.plutPdadcOffset[0],
                  NULL, sizeof(tANI_U16) * NUM_RF_CHANNELS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }
        if (vos_nv_getValidity(VNV_RSSI_CHANNEL_OFFSETS, &itemIsValid) ==
           VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_RSSI_CHANNEL_OFFSETS,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.rssiChanOffsets[0],
                  NULL, sizeof(sRssiChannelOffsets) * 2 ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_HW_CAL_VALUES, &itemIsValid) ==
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_HW_CAL_VALUES, (v_VOID_t *)&pnvEFSTable->halnv
    .tables.hwCalValues, NULL, sizeof(sHwCalValues) ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_FW_CONFIG, &itemIsValid) ==
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_FW_CONFIG, (v_VOID_t *)&pnvEFSTable->halnv
    .tables.fwConfig, NULL, sizeof(sFwConfig) ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_ANTENNA_PATH_LOSS, &itemIsValid) ==
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_ANTENNA_PATH_LOSS,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.antennaPathLoss[0], NULL,
                sizeof(tANI_S16)*NUM_RF_CHANNELS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }
        if (vos_nv_getValidity(VNV_PACKET_TYPE_POWER_LIMITS, &itemIsValid) ==
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_PACKET_TYPE_POWER_LIMITS,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.pktTypePwrLimits[0], NULL,
                sizeof(tANI_S16)*NUM_802_11_MODES*NUM_RF_CHANNELS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_OFDM_CMD_PWR_OFFSET, &itemIsValid) ==
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_OFDM_CMD_PWR_OFFSET,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.ofdmCmdPwrOffset, NULL,
                                sizeof(sOfdmCmdPwrOffset)) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_TX_BB_FILTER_MODE, &itemIsValid) ==
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
               if(vos_nv_read(VNV_TX_BB_FILTER_MODE,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.txbbFilterMode, NULL,
                sizeof(sTxBbFilterMode)) != VOS_STATUS_SUCCESS)
                   goto error;
            }
        }
        if (vos_nv_getValidity(VNV_TABLE_VIRTUAL_RATE, &itemIsValid) ==
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
               if(vos_nv_read(VNV_TABLE_VIRTUAL_RATE,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.pwrOptimum_virtualRate, NULL,
                sizeof(gnvEFSTable->halnv.tables.pwrOptimum_virtualRate)) != VOS_STATUS_SUCCESS)
                   goto error;
            }
        }
    }

    return VOS_STATUS_SUCCESS;
error:
    vos_mem_free(pnvEFSTable);
    vos_mem_free(pEncodedBuf);
    return eHAL_STATUS_FAILURE ;
}

VOS_STATUS vos_nv_close(void)
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    v_CONTEXT_t pVosContext= NULL;
         /*                       */
    pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
    status = hdd_release_firmware(WLAN_NV_FILE, ((VosContextType*)(pVosContext))->pHDDContext);
    if ( !VOS_IS_STATUS_SUCCESS( status ))
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                         "%s : vos_open failed\n",__func__);
        return VOS_STATUS_E_FAILURE;
    }
    vos_mem_free(pnvEFSTable);
    vos_mem_free(pEncodedBuf);
    vos_mem_free(pDictFile);

    gnvEFSTable=NULL;
    return VOS_STATUS_SUCCESS;
}

/*                                                                         
                                                                     
               
                                                                       
                                                    
                                                                      
                                                                        
                                                                          
                                                
                                                                 
                                                                             
                                                                     
                                                                           
                                                                   
                                         
     
                                                                           */
VOS_STATUS vos_nv_getSupportedCountryCode( v_BYTE_t *pBuffer, v_SIZE_t *pBufferSize,
      v_SIZE_t paddingSize )
{
   v_SIZE_t providedBufferSize = *pBufferSize;
   int i;
   //                                                   
   *pBufferSize = countryInfoTable.countryCount * (VOS_COUNTRY_CODE_LEN + paddingSize );
   if ( NULL == pBuffer || providedBufferSize < *pBufferSize )
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
            ("Insufficient memory for country code list\n"));
      return VOS_STATUS_E_NOMEM;
   }
   for (i = 0; i < countryInfoTable.countryCount; i++)
   {
      memcpy( pBuffer, countryInfoTable.countryInfo[i].countryCode, VOS_COUNTRY_CODE_LEN );
      pBuffer += (VOS_COUNTRY_CODE_LEN + paddingSize );
   }
   return VOS_STATUS_SUCCESS;
}
/*                                                                         
                                                                  
                                          
                                         
     
                                                                           */
VOS_STATUS vos_nv_readTxAntennaCount( v_U8_t *pTxAntennaCount )
{
   sNvFields fieldImage;
   VOS_STATUS status;
   status = vos_nv_read( VNV_FIELD_IMAGE, &fieldImage, NULL,
         sizeof(fieldImage) );
   if (VOS_STATUS_SUCCESS == status)
   {
      *pTxAntennaCount = fieldImage.numOfTxChains;
   }
   return status;
}
/*                                                                         
                                                                  
                                          
                                         
     
                                                                           */
VOS_STATUS vos_nv_readRxAntennaCount( v_U8_t *pRxAntennaCount )
{
   sNvFields fieldImage;
   VOS_STATUS status;
   status = vos_nv_read( VNV_FIELD_IMAGE, &fieldImage, NULL,
         sizeof(fieldImage) );
   if (VOS_STATUS_SUCCESS == status)
   {
      *pRxAntennaCount = fieldImage.numOfRxChains;
   }
   return status;
}

/*                                                                         
                                                         
                                  
                                         
     
                                                                           */
VOS_STATUS vos_nv_readMacAddress( v_MAC_ADDRESS_t pMacAddress )
{
   sNvFields fieldImage;
   VOS_STATUS status;
   status = vos_nv_read( VNV_FIELD_IMAGE, &fieldImage, NULL,
         sizeof(fieldImage) );
   if (VOS_STATUS_SUCCESS == status)
   {
      memcpy( pMacAddress, fieldImage.macAddr, VOS_MAC_ADDRESS_LEN );
   }
   else
   {
      //                                                          
      const v_U8_t macAddr[VOS_MAC_ADDRESS_LEN] = VOS_HARD_CODED_MAC;
      memcpy( pMacAddress, macAddr, VOS_MAC_ADDRESS_LEN );
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_WARN,
                "fail to get MAC address from NV, hardcoded to "MAC_ADDRESS_STR,
                MAC_ADDR_ARRAY(macAddr));
      status = VOS_STATUS_SUCCESS;
   }
   return status;
}

/*                                                                         

                                                                         

                                  
                                                                     

                                         

     

                                                                           */
VOS_STATUS vos_nv_readMultiMacAddress( v_U8_t *pMacAddress,
                                              v_U8_t  macCount )
{
   sNvFields   fieldImage;
   VOS_STATUS  status;
   v_U8_t      countLoop;
   v_U8_t     *pNVMacAddress;

   if((0 == macCount) || (VOS_MAX_CONCURRENCY_PERSONA < macCount) ||
      (NULL == pMacAddress))
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
          " Invalid Parameter from NV Client macCount %d, pMacAddress %p",
          macCount, pMacAddress);
   }

   status = vos_nv_read( VNV_FIELD_IMAGE, &fieldImage, NULL,
                         sizeof(fieldImage) );
   if (VOS_STATUS_SUCCESS == status)
   {
      pNVMacAddress = fieldImage.macAddr;
      for(countLoop = 0; countLoop < macCount; countLoop++)
      {
         vos_mem_copy(pMacAddress + (countLoop * VOS_MAC_ADDRESS_LEN),
                      pNVMacAddress + (countLoop * VOS_MAC_ADDRESS_LEN),
                      VOS_MAC_ADDRESS_LEN);
      }
   }
   else
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 "vos_nv_readMultiMacAddress Get NV Field Fail");
   }

   return status;
}

/*                                                                         
                                                               
                                                                        
                                              
                                                                           
                                                                
                                   
                                                                   
                                                           
                                                               
                                              
     
                                                                           */

VOS_STATUS vos_nv_setValidity( VNV_TYPE type, v_BOOL_t itemIsValid )
{
   v_U32_t lastNvValidityBitmap;
   v_U32_t newNvValidityBitmap;
   VOS_STATUS status = VOS_STATUS_SUCCESS;

   //                                      
   if (VNV_TYPE_COUNT <= type)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("%s: invalid type=%d"), __func__, type );
      return VOS_STATUS_E_INVAL;
   }
   //                         
   lastNvValidityBitmap = gnvEFSTable->nvValidityBitmap;
   //                           
   if (itemIsValid)
   {
       newNvValidityBitmap = lastNvValidityBitmap | (1 << type);
              //                                               
       if (newNvValidityBitmap != lastNvValidityBitmap)
       {
           gnvEFSTable->nvValidityBitmap = newNvValidityBitmap;
       }
   }
   else
   {
       newNvValidityBitmap = lastNvValidityBitmap & (~(1 << type));
       if (newNvValidityBitmap != lastNvValidityBitmap)
       {
           gnvEFSTable->nvValidityBitmap = newNvValidityBitmap;
           status = wlan_write_to_efs((v_U8_t*)gnvEFSTable,sizeof(nvEFSTable_t));
           if (! VOS_IS_STATUS_SUCCESS(status)) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, ("vos_nv_write_to_efs failed!!!\r\n"));
               status = VOS_STATUS_E_FAULT;
           }
       }
   }

   return status;
}
/*                                                                         
                                                               
                                                                    
                                              
                                                                           
                                                                
                                   
                                                                         
                               
                                                                  
                                                               
                                              
     
                                                                           */
VOS_STATUS vos_nv_getValidity( VNV_TYPE type, v_BOOL_t *pItemIsValid )
{
   v_U32_t nvValidityBitmap = gnvEFSTable->nvValidityBitmap;
   //                                      
   if (VNV_TYPE_COUNT <= type)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("%s: invalid type=%d"), __func__, type );
      return VOS_STATUS_E_INVAL;
   }
   *pItemIsValid = (v_BOOL_t)((nvValidityBitmap >> type) & 1);
   return VOS_STATUS_SUCCESS;
}
/*                                                                         
                                                           
                                                                           
                                                                      
                                                         
                                                                       
                                                          
                                                            
                             
                                       
                                        
                                         
                                                           
                                                               
                                                          
                                                      
                                              
     
                                                                           */
VOS_STATUS vos_nv_read( VNV_TYPE type, v_VOID_t *outputVoidBuffer,
      v_VOID_t *defaultBuffer, v_SIZE_t bufferSize )
{
    VOS_STATUS status             = VOS_STATUS_SUCCESS;
    v_SIZE_t itemSize;
    v_BOOL_t itemIsValid = VOS_TRUE;

    //             
    if (VNV_TYPE_COUNT <= type)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("%s: invalid type=%d"), __func__, type );
       return VOS_STATUS_E_INVAL;
    }
    if (NULL == outputVoidBuffer)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
             ("Buffer provided is NULL\r\n") );
       return VOS_STATUS_E_FAULT;
    }
    if (0 == bufferSize)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
             ("NV type=%d is invalid\r\n"), type );
       return VOS_STATUS_E_INVAL;
    }
    //                                    
    status = vos_nv_getValidity( type, &itemIsValid );
   if (!itemIsValid)
   {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_WARN,
            "NV type=%d does not have valid data\r\n", type );
       return VOS_STATUS_E_EMPTY;
   }
   switch(type)
   {
       case VNV_FIELD_IMAGE:
           itemSize = sizeof(gnvEFSTable->halnv.fields);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.fields,bufferSize);
           }
           break;
       case VNV_RATE_TO_POWER_TABLE:
           itemSize = sizeof(gnvEFSTable->halnv.tables.pwrOptimum);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.pwrOptimum[0],bufferSize);
           }
           break;
       case VNV_REGULARTORY_DOMAIN_TABLE:
           itemSize = sizeof(gnvEFSTable->halnv.tables.regDomains);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.regDomains[0],bufferSize);
           }
           break;
       case VNV_DEFAULT_LOCATION:
           itemSize = sizeof(gnvEFSTable->halnv.tables.defaultCountryTable);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.defaultCountryTable,bufferSize);
           }
           break;
       case VNV_TPC_POWER_TABLE:
           itemSize = sizeof(gnvEFSTable->halnv.tables.plutCharacterized);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.plutCharacterized[0],bufferSize);
           }
           break;
       case VNV_TPC_PDADC_OFFSETS:
           itemSize = sizeof(gnvEFSTable->halnv.tables.plutPdadcOffset);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.plutPdadcOffset[0],bufferSize);
           }
           break;
       case VNV_RSSI_CHANNEL_OFFSETS:

           itemSize = sizeof(gnvEFSTable->halnv.tables.rssiChanOffsets);

           if(bufferSize != itemSize) {

               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.rssiChanOffsets[0],bufferSize);
           }
           break;
       case VNV_HW_CAL_VALUES:

           itemSize = sizeof(gnvEFSTable->halnv.tables.hwCalValues);

           if(bufferSize != itemSize) {

               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.hwCalValues,bufferSize);
           }
           break;
       case VNV_FW_CONFIG:

           itemSize = sizeof(gnvEFSTable->halnv.tables.fwConfig);

           if(bufferSize != itemSize) {

               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.fwConfig,bufferSize);
           }
           break;
       case VNV_ANTENNA_PATH_LOSS:
           itemSize = sizeof(gnvEFSTable->halnv.tables.antennaPathLoss);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.antennaPathLoss[0],bufferSize);
           }
           break;
       case VNV_PACKET_TYPE_POWER_LIMITS:
           itemSize = sizeof(gnvEFSTable->halnv.tables.pktTypePwrLimits);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,gnvEFSTable->halnv.tables.pktTypePwrLimits,bufferSize);
           }
           break;
       case VNV_OFDM_CMD_PWR_OFFSET:
           itemSize = sizeof(gnvEFSTable->halnv.tables.ofdmCmdPwrOffset);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.ofdmCmdPwrOffset,bufferSize);
           }
           break;
       case VNV_TX_BB_FILTER_MODE:
           itemSize = sizeof(gnvEFSTable->halnv.tables.txbbFilterMode);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.txbbFilterMode,bufferSize);
           }
           break;


       case VNV_TABLE_VIRTUAL_RATE:
           itemSize = sizeof(gnvEFSTable->halnv.tables.pwrOptimum_virtualRate);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.pwrOptimum_virtualRate,bufferSize);
           }
           break;

       default:
         break;
   }
   return status;
}

/*                                                                         
                                                                 
                                                                            
                                                            
                         
                            
                                   
                                            
                                                           
                                                               
                                                           
                                                      
                                                
     
                                                                           */
VOS_STATUS vos_nv_write(VNV_TYPE type, v_VOID_t *inputVoidBuffer,
      v_SIZE_t bufferSize)
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    v_SIZE_t itemSize;

    //             
    if (VNV_TYPE_COUNT <= type)
    {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 "%s: invalid type=%d", __func__, type);
       return VOS_STATUS_E_INVAL;
    }
    if (NULL == inputVoidBuffer)
    {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 "Buffer provided is NULL");
       return VOS_STATUS_E_FAULT;
    }
    if (0 == bufferSize)
    {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 "NV type=%d is invalid", type);
       return VOS_STATUS_E_INVAL;
    }

    switch (type)
    {
        case VNV_FIELD_IMAGE:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.fields);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "type = %d buffer size=%d is less than data size=%d",
                     type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.fields,
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        case VNV_RATE_TO_POWER_TABLE:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.pwrOptimum);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize,itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.pwrOptimum[0],
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        case VNV_REGULARTORY_DOMAIN_TABLE:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.regDomains);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.regDomains[0],
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        case VNV_DEFAULT_LOCATION:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.defaultCountryTable);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.defaultCountryTable,
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        case VNV_TPC_POWER_TABLE:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.plutCharacterized);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.plutCharacterized[0],
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        case VNV_TPC_PDADC_OFFSETS:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.plutPdadcOffset);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.plutPdadcOffset[0],
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

         case VNV_RSSI_CHANNEL_OFFSETS:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.rssiChanOffsets);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.rssiChanOffsets[0],
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

         case VNV_HW_CAL_VALUES:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.hwCalValues);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.hwCalValues,
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        case VNV_FW_CONFIG:
           itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.fwConfig);
           if (bufferSize != itemSize)
           {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   "type = %d buffer size=%d is less than data size=%d",
                   type, bufferSize, itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else
           {
               memcpy(&gnvEFSTableV2->halnvV2.tables.fwConfig,
                      inputVoidBuffer,
                      bufferSize);
           }
           break;

        case VNV_ANTENNA_PATH_LOSS:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.antennaPathLoss);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.antennaPathLoss[0],
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        case VNV_PACKET_TYPE_POWER_LIMITS:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.pktTypePwrLimits);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(gnvEFSTableV2->halnvV2.tables.pktTypePwrLimits,
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        case VNV_OFDM_CMD_PWR_OFFSET:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.ofdmCmdPwrOffset);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.ofdmCmdPwrOffset,
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        case VNV_TX_BB_FILTER_MODE:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.txbbFilterMode);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.txbbFilterMode,
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        case VNV_TABLE_VIRTUAL_RATE:
            itemSize = sizeof(gnvEFSTableV2->halnvV2.tables.pwrOptimum_virtualRate);
            if (bufferSize != itemSize)
            {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "type = %d buffer size=%d is less than data size=%d",
                    type, bufferSize, itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else
            {
                memcpy(&gnvEFSTableV2->halnvV2.tables.pwrOptimum_virtualRate,
                       inputVoidBuffer,
                       bufferSize);
            }
            break;

        default:
          break;
    }

   if (VOS_STATUS_SUCCESS == status)
   {
      //                               
      status = vos_nv_setValidity(type, VOS_TRUE);
      if (! VOS_IS_STATUS_SUCCESS(status))
      {
          VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "vos_nv_setValidity failed!!!");
          status = VOS_STATUS_E_FAULT;
      }

      status = wlan_write_to_efs((v_U8_t*)gnvEFSTableV2, sizeof(*gnvEFSTableV2));
      if (!VOS_IS_STATUS_SUCCESS(status))
      {
          VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "vos_nv_write_to_efs failed!!!");
          status = VOS_STATUS_E_FAULT;
      }
   }

   return status;
}

/*                                                                         
                                                                          
                                                           
                                                 
                                                           
                                                 
                                                           
                                         
                                      
     
                                                                           */
VOS_STATUS vos_nv_getChannelListWithPower(tChannelListWithPower *channels20MHz /*                        */,
                                          tANI_U8 *num20MHzChannelsFound,
                                          tChannelListWithPower *channels40MHz /*                         */,
                                          tANI_U8 *num40MHzChannelsFound
                                          )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    int i, count;

    //                                                                              
    //                                          

    if( channels20MHz && num20MHzChannelsFound )
    {
        count = 0;
        for( i = 0; i <= RF_CHAN_14; i++ )
        {
            if( regChannels[i].enabled )
            {
                channels20MHz[count].chanId = rfChannels[i].channelNum;
                channels20MHz[count++].pwr  = regChannels[i].pwrLimit;
            }
        }
        for( i = RF_CHAN_36; i <= RF_CHAN_165; i++ )
        {
            if( regChannels[i].enabled )
            {
                channels20MHz[count].chanId = rfChannels[i].channelNum;
                channels20MHz[count++].pwr  = regChannels[i].pwrLimit;
            }
        }
        *num20MHzChannelsFound = (tANI_U8)count;
    }

    if( channels40MHz && num40MHzChannelsFound )
    {
        count = 0;
        //                                           
        for( i = RF_CHAN_BOND_3; i <= RF_CHAN_BOND_11; i++ )
        {

            if( regChannels[i].enabled )
            {
                channels40MHz[count].chanId = rfChannels[i].channelNum;
                channels40MHz[count++].pwr  = regChannels[i].pwrLimit;
            }
        }
        //                                         
        for( i = RF_CHAN_BOND_38; i <= RF_CHAN_BOND_163; i++ )
        {

            if( regChannels[i].enabled )
            {
                channels40MHz[count].chanId = rfChannels[i].channelNum;
                channels40MHz[count++].pwr  = regChannels[i].pwrLimit;
            }
        }
        *num40MHzChannelsFound = (tANI_U8)count;
    }
    return (status);
}

/*                                                                         
                                                                            
                                   
     
                                                                           */

v_REGDOMAIN_t vos_nv_getDefaultRegDomain( void )
{
    return countryInfoTable.countryInfo[0].regDomain;
}

/*                                                                         
                                                                       
                            
                                                 
                                                      
                                                 
                                                      
                                         
                                      
     
                                                                           */
VOS_STATUS vos_nv_getSupportedChannels( v_U8_t *p20MhzChannels, int *pNum20MhzChannels,
                                        v_U8_t *p40MhzChannels, int *pNum40MhzChannels)
{
    VOS_STATUS status = VOS_STATUS_E_INVAL;
    int i, count = 0;

    if( p20MhzChannels && pNum20MhzChannels )
    {
        if( *pNum20MhzChannels >= NUM_RF_CHANNELS )
        {
            for( i = 0; i <= RF_CHAN_14; i++ )
            {
                p20MhzChannels[count++] = rfChannels[i].channelNum;
            }
            for( i = RF_CHAN_36; i <= RF_CHAN_165; i++ )
            {
                p20MhzChannels[count++] = rfChannels[i].channelNum;
            }
            status = VOS_STATUS_SUCCESS;
        }
        *pNum20MhzChannels = count;
    }

    return (status);
}

/*                                                                         
                                                                            
                                                                          
                                         
     
                                                                           */
VOS_STATUS vos_nv_readDefaultCountryTable( uNvTables *tableData )
{

   VOS_STATUS status = VOS_STATUS_SUCCESS;
   memcpy(&tableData->defaultCountryTable, &pnvEFSTable->halnv.tables.defaultCountryTable, sizeof(sDefaultCountry));
   pr_info("DefaultCountry is %c%c\n",
            tableData->defaultCountryTable.countryCode[0],
            tableData->defaultCountryTable.countryCode[1]);
   return status;
}

/*                                                                         
                           
                                                
                                      
                                         
     
                                                                           */
VOS_STATUS vos_nv_getNVBuffer(v_VOID_t **pNvBuffer,v_SIZE_t *pSize)
{
   eNvVersionType nvVersion;

   nvVersion = vos_nv_getNvVersion();

   if (E_NV_V3 == nvVersion)
   {
      /*                                   */
      *pNvBuffer = (v_VOID_t *)(&pnvEFSTable->halnv);
      *pSize = sizeof(sHalNv);
   }
   else if (E_NV_V2 == nvVersion)
   {
      /*                                   */
      *pNvBuffer = (v_VOID_t *)(&gnvEFSTableV2->halnvV2);
      *pSize = sizeof(sHalNvV2);
   }
   else
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "%s : Invalid NV version %d", __func__, nvVersion);
      return VOS_STATUS_E_INVAL;
   }

   return VOS_STATUS_SUCCESS;
}

/*                                                                         
                           
                                                
                                      
                                         
     
                                                                           */
VOS_STATUS vos_nv_getNVEncodedBuffer(v_VOID_t **pNvBuffer,v_SIZE_t *pSize)
{
   /*                                */
   *pNvBuffer = (v_VOID_t *)(pEncodedBuf);
   *pSize = nvReadEncodeBufSize;
   return VOS_STATUS_SUCCESS;
}


VOS_STATUS vos_nv_getNVDictionary(v_VOID_t **pNvBuffer,v_SIZE_t *pSize)
{
    /*                                */
   *pNvBuffer = (v_VOID_t *)(pDictFile);
   *pSize = nDictionarySize;

   return VOS_STATUS_SUCCESS;
}

VOS_STATUS vos_nv_isEmbeddedNV(v_VOID_t)
{
   if (MAGIC_NUMBER == magicNumber)
   {
      return VOS_STATUS_SUCCESS;
   }

   return VOS_STATUS_E_FAILURE;
}

VOS_STATUS vos_nv_setNVEncodedBuffer(v_U8_t *pNvBuffer, v_SIZE_t size)
{
    vos_mem_copy(pEncodedBuf, &pNvBuffer[sizeof(v_U32_t)],
            (size-sizeof(v_U32_t)));

    return VOS_STATUS_SUCCESS;
}

/*                                                                         
                                        
                                                              
                                                         
                      
                       
                  
     
                                                                           */
eNVChannelEnabledType vos_nv_getChannelEnabledState
(
   v_U32_t     rfChannel
)
{
   v_U32_t       channelLoop;
   eRfChannels   channelEnum = INVALID_RF_CHANNEL;

   for(channelLoop = 0; channelLoop <= RF_CHAN_165; channelLoop++)
   {
      if(rfChannels[channelLoop].channelNum == rfChannel)
      {
         channelEnum = (eRfChannels)channelLoop;
         break;
      }
   }

   if(INVALID_RF_CHANNEL == channelEnum)
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "vos_nv_getChannelEnabledState, invalid channel %d", rfChannel);
      return NV_CHANNEL_INVALID;
   }

   return regChannels[channelEnum].enabled;
}

/*                                                                         
                              
             
                                       
                      
                      
                           
     
                                                                           */
eNvVersionType vos_nv_getNvVersion
(
   void
)
{
   VosContextType  *vosCtxt = NULL;

   vosCtxt = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
   if (vosCtxt)
   {
      return vosCtxt->nvVersion;
   }

   return E_NV_INVALID;
}

/*                                                                 
                            
                                                                  */

static int bw20_ch_index_to_bw40_ch_index(int k)
{
   int m = -1;
   if (k >= RF_CHAN_1 && k <= RF_CHAN_14)
   {
      m = k - RF_CHAN_1 + RF_CHAN_BOND_3 ;
      if (m > RF_CHAN_BOND_11)
         m = RF_CHAN_BOND_11;
   }
   else if (k >= RF_CHAN_240 && k <= RF_CHAN_216)
   {
      m = k - RF_CHAN_240 + RF_CHAN_BOND_242 ;
      if (m > RF_CHAN_BOND_214)
         m = RF_CHAN_BOND_214;
   }
   else if (k >= RF_CHAN_36 && k <= RF_CHAN_64)
   {
      m = k - RF_CHAN_36 + RF_CHAN_BOND_38;
      if (m > RF_CHAN_BOND_62)
         m = RF_CHAN_BOND_62;
   }
#ifdef FEATURE_WLAN_CH144
   else if (k >= RF_CHAN_100 && k <= RF_CHAN_144)
#else
   else if (k >= RF_CHAN_100 && k <= RF_CHAN_140)
#endif /*                    */
   {
      m = k - RF_CHAN_100 + RF_CHAN_BOND_102;
#ifdef FEATURE_WLAN_CH144
      if (m > RF_CHAN_BOND_142)
         m = RF_CHAN_BOND_142;
#else
      if (m > RF_CHAN_BOND_138)
         m = RF_CHAN_BOND_138;
#endif /*                    */
   }
   else if (k >= RF_CHAN_149 && k <= RF_CHAN_165)
   {
      m = k - RF_CHAN_149 + RF_CHAN_BOND_151;
      if (m > RF_CHAN_BOND_163)
         m = RF_CHAN_BOND_163;
   }
return m;
}

void crda_regulatory_entry_default(v_U8_t *countryCode, int domain_id)
{
   int k;
   pr_info("Country %c%c domain_id %d\n enable ch 1 - 11.\n",
       countryCode[0], countryCode[1], domain_id);
   for (k = RF_CHAN_1; k <= RF_CHAN_11; k++) {
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].enabled =
           NV_CHANNEL_ENABLE;
       /*                    */
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].pwrLimit = 20;
   }
   /*                                    */
   pr_info(" enable ch 12 - 14 to scan passively by setting DFS flag.\n");
   for (k = RF_CHAN_12; k <= MAX_2_4GHZ_CHANNEL; k++) {
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].enabled =
           NV_CHANNEL_DFS;
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].pwrLimit = 0;
   }
   pr_info(" enable 5GHz to scan passively by setting DFS flag.\n");
   for (k = MIN_5GHZ_CHANNEL; k <= MAX_5GHZ_CHANNEL; k++) {
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].enabled =
           NV_CHANNEL_DFS;
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].pwrLimit = 0;
   }
#ifdef PASSIVE_SCAN_4_9GHZ
   pr_info(" enable 4.9 GHz to scan passively by setting DFS flag.\n");
   for (k = RF_CHAN_240; k <= RF_CHAN_216; k++) {
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].enabled =
           NV_CHANNEL_DFS;
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].pwrLimit = 0;
   }
#endif
  if (domain_id == NUM_REG_DOMAINS-1)
  { /*           */
     crda_alpha2[0] = countryCode[0];
     crda_alpha2[1] = countryCode[1];
     crda_regulatory_entry_valid = VOS_TRUE;
     pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[0] = countryCode[0];
     pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[1] = countryCode[1];
     pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[2] = 'I';
     pnvEFSTable->halnv.tables.defaultCountryTable.regDomain = NUM_REG_DOMAINS-1;
  }
  if (domain_id == NUM_REG_DOMAINS-2)
  { /*                      */
     run_time_alpha2[0] = countryCode[0];
     run_time_alpha2[1] = countryCode[1];
     crda_regulatory_run_time_entry_valid = VOS_TRUE;
  }
}

static int crda_regulatory_entry_post_processing(struct wiphy *wiphy,
                struct regulatory_request *request,
                v_U8_t nBandCapability,
                int domain_id)
{
   if (request->alpha2[0] == '0' && request->alpha2[1] == '0') {
        pr_info("Country 00 special handling to enable passive scan.\n");
        crda_regulatory_entry_default(request->alpha2, domain_id);
   }
   return 0;
}

/*                                                                                   */
static int create_crda_regulatory_entry(struct wiphy *wiphy,
                struct regulatory_request *request,
                v_U8_t nBandCapability)
{
   int i, j, m;
   int k = 0, n = 0;

   if (run_time_alpha2[0]==request->alpha2[0] &&
        run_time_alpha2[1]==request->alpha2[1] &&
        crda_regulatory_run_time_entry_valid == VOS_TRUE)
        return 0; /*                 */

   /*                */
   if (nBandCapability == eCSR_BAND_24)
       pr_info("BandCapability is set to 2G only.\n");
   for (i=0,m=0;i<IEEE80211_NUM_BANDS;i++)
   {
       if (i == IEEE80211_BAND_2GHZ && nBandCapability == eCSR_BAND_5G) //        
          continue;
       else if (i == IEEE80211_BAND_5GHZ && nBandCapability == eCSR_BAND_24) //        
          continue;
       if (wiphy->bands[i] == NULL)
       {
          pr_info("error: wiphy->bands[i] is NULL, i = %d\n", i);
          return -1;
       }
       //                                                                    
       //                                                   
       if (i == 0)
           m = 0;
       else
           m = wiphy->bands[i-1]->n_channels + m;
       for (j=0;j<wiphy->bands[i]->n_channels;j++)
       {
           //                                                                
           //                                                            
           k = m + j;
           n = bw20_ch_index_to_bw40_ch_index(k);
           if (n == -1)
              return -1;
           if (wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_DISABLED)
           {
              if (pnvEFSTable == NULL)
              {
                 pr_info("error: pnvEFSTable is NULL, probably not parsed nv.bin yet\n");
                 return -1;
              }
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[k].enabled =
                 NV_CHANNEL_DISABLE;
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[n].enabled =
                 NV_CHANNEL_DISABLE;
              //                                                                                    
              //                              
           }
           else if (wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_RADAR)
           {
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[k].enabled =
                 NV_CHANNEL_DFS;
              //                                
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[k].pwrLimit =
                 (tANI_S8) ((wiphy->bands[i]->channels[j].max_power)/100);
              if ((wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_NO_HT40) == 0)
              {
                 pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[n].enabled =
                    NV_CHANNEL_DFS;
                 //                                               
                 pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[n].pwrLimit =
                    (tANI_S8) (((wiphy->bands[i]->channels[j].max_power)/100)-3);
              }
           }
           else //                                    
           {
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[k].enabled =
                 NV_CHANNEL_ENABLE;
              //                    
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[k].pwrLimit =
                 (tANI_S8) ((wiphy->bands[i]->channels[j].max_power)/100);
              if ((wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_NO_HT40) == 0)
              {
                 pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[n].enabled =
                    NV_CHANNEL_ENABLE;
                 //                                               
                 pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[n].pwrLimit =
                    (tANI_S8) (((wiphy->bands[i]->channels[j].max_power)/100)-3);
              }
           }
           /*                                                                    
                                                                 */
       }
   }
   if (k == 0)
       return -1;
   run_time_alpha2[0] = request->alpha2[0];
   run_time_alpha2[1] = request->alpha2[1];
   crda_regulatory_run_time_entry_valid = VOS_TRUE;
   crda_regulatory_entry_post_processing(wiphy, request, nBandCapability, NUM_REG_DOMAINS-2);
return 0;
}


v_BOOL_t is_crda_regulatory_entry_valid(void)
{
return crda_regulatory_entry_valid;
}

/*                                                                                       
                                                           
                                                       
                                                           
                                                       
*/
static int bw20_start_freq_to_channel_index(u32 freq_khz)
{
  int i;
  u32 center_freq = freq_khz + 10000;

  //                                         
  //                  
  for (i=RF_CHAN_1;i<=RF_CHAN_14;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                                                        
  for (i=RF_CHAN_240;i<=RF_CHAN_216;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_36;i<=RF_CHAN_64;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
#ifdef FEATURE_WLAN_CH144
  for (i=RF_CHAN_100;i<=RF_CHAN_144;i++)
#else
  for (i=RF_CHAN_100;i<=RF_CHAN_140;i++)
#endif /*                    */
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                     
  for (i=RF_CHAN_149;i<=RF_CHAN_165;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
return -1;
}

static int bw20_end_freq_to_channel_index(u32 freq_khz)
{
  int i;
  u32 center_freq = freq_khz - 10000;

  //                                         
  //                     
  for (i=RF_CHAN_165;i>=RF_CHAN_149;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
#ifdef FEATURE_WLAN_CH144
  for (i=RF_CHAN_144;i>=RF_CHAN_100;i--)
#else
  for (i=RF_CHAN_140;i>=RF_CHAN_100;i--)
#endif /*                    */
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_64;i>=RF_CHAN_36;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                                                        
  for (i=RF_CHAN_216;i>=RF_CHAN_240;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                  
  for (i=RF_CHAN_14;i>=RF_CHAN_1;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  return -1;
}

static int bw40_start_freq_to_channel_index(u32 freq_khz)
{
  int i;
  u32 center_freq = freq_khz + 20000;

  //                                         
  //                  
  for (i=RF_CHAN_BOND_3;i<=RF_CHAN_BOND_11;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
        return i;
  //                                              
  for (i=RF_CHAN_BOND_242;i<=RF_CHAN_BOND_214;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_BOND_38;i<=RF_CHAN_BOND_62;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
#ifdef FEATURE_WLAN_CH144
  for (i=RF_CHAN_BOND_102;i<=RF_CHAN_BOND_142;i++)
#else
  for (i=RF_CHAN_BOND_102;i<=RF_CHAN_BOND_138;i++)
#endif /*                  */
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                     
  for (i=RF_CHAN_BOND_151;i<=RF_CHAN_BOND_163;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
return -1;
}

static int bw40_end_freq_to_channel_index(u32 freq_khz)
{
  int i;
  u32 center_freq = freq_khz - 20000;

  //                                         
  //                     
  for (i=RF_CHAN_BOND_163;i>=RF_CHAN_BOND_151;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
#ifdef FEATURE_WLAN_CH144
  for (i=RF_CHAN_BOND_142;i>=RF_CHAN_BOND_102;i--)
#else
  for (i=RF_CHAN_BOND_138;i>=RF_CHAN_BOND_102;i--)
#endif /*                    */
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_BOND_62;i>=RF_CHAN_BOND_38;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                                              
  for (i=RF_CHAN_BOND_214;i>=RF_CHAN_BOND_242;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                  
  for (i=RF_CHAN_BOND_11;i>=RF_CHAN_BOND_3;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
return -1;
}

static v_BOOL_t channel_in_capable_band(int j, v_U8_t nBandCapability)
{
   switch (nBandCapability)
   {
      case eCSR_BAND_ALL:
           return VOS_TRUE;
      case eCSR_BAND_24:
           if (j >= RF_CHAN_1 && j <= RF_CHAN_14)
              return VOS_TRUE;
           if (j >= RF_CHAN_BOND_3 && j <= RF_CHAN_BOND_11)
              return VOS_TRUE; //                   
           break;
      case eCSR_BAND_5G:
           if (j >= RF_CHAN_240 && j <= RF_CHAN_165)
              return VOS_TRUE;
           if (j >= RF_CHAN_BOND_242 && j <= RF_CHAN_BOND_163)
              return VOS_TRUE; //                   
           break;
      default:
           break;
   }
   return VOS_FALSE;
}

/*                                                                          */
static int create_crda_regulatory_entry_from_regd(struct wiphy *wiphy,
                struct regulatory_request *request,
                v_U8_t nBandCapability)
{
   int i, j, n, domain_id;
   int bw20_start_channel_index, bw20_end_channel_index;
   int bw40_start_channel_index, bw40_end_channel_index;

   if (wiphy == NULL || wiphy->regd == NULL)
   {
      wiphy_dbg(wiphy, "error: wiphy->regd is NULL\n");
      return -1;
   }
   if (crda_regulatory_entry_valid == VOS_FALSE)
      domain_id = NUM_REG_DOMAINS-1; /*           */
   else
      domain_id = NUM_REG_DOMAINS-2; /*                      */
   for (n = 0; n < NUM_RF_CHANNELS; n++)
      pnvEFSTable->halnv.tables.regDomains[domain_id].channels[n].enabled = NV_CHANNEL_DISABLE;

   for (i=0;i<wiphy->regd->n_reg_rules;i++)
   {
      wiphy_dbg(wiphy, "info: crda rule %d --------------------------------------------\n", i);
      bw20_start_channel_index =
         bw20_start_freq_to_channel_index(wiphy->regd->reg_rules[i].freq_range.start_freq_khz);
      bw20_end_channel_index =
      bw20_end_freq_to_channel_index(wiphy->regd->reg_rules[i].freq_range.end_freq_khz);
      if (bw20_start_channel_index == -1 || bw20_end_channel_index == -1)
      {
         wiphy_dbg(wiphy, "error: crda freq not supported, start freq (KHz) %d end freq %d\n",
          wiphy->regd->reg_rules[i].freq_range.start_freq_khz,
             wiphy->regd->reg_rules[i].freq_range.end_freq_khz);
         continue; //                                          
      }
      wiphy_dbg(wiphy, "20MHz start freq (KHz) %d end freq %d start ch index %d end ch index %d\n",
         wiphy->regd->reg_rules[i].freq_range.start_freq_khz,
          wiphy->regd->reg_rules[i].freq_range.end_freq_khz,
              bw20_start_channel_index, bw20_end_channel_index);
      for (j=bw20_start_channel_index;j<=bw20_end_channel_index;j++)
      {
         if (channel_in_capable_band(j, nBandCapability) == VOS_FALSE)
         {
             wiphy_dbg(wiphy, "info: CH %d is not in capable band\n",
                 rfChannels[j].channelNum);
             continue; //                                     
         }
         if (wiphy->regd->reg_rules[i].flags & NL80211_RRF_DFS)
         {
             pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].enabled = NV_CHANNEL_DFS;
             wiphy_dbg(wiphy, "info: CH %d is DFS, max EIRP (mBm) is %d\n", rfChannels[j].channelNum,
                wiphy->regd->reg_rules[i].power_rule.max_eirp);
         }
         if (wiphy->regd->reg_rules[i].flags & NL80211_RRF_PASSIVE_SCAN)
         {
             pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].enabled = NV_CHANNEL_DFS;
             wiphy_dbg(wiphy, "info: CH %d is Passive, max EIRP (mBm) is %d\n", rfChannels[j].channelNum,
                wiphy->regd->reg_rules[i].power_rule.max_eirp);
         }
         else
         {
             pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].enabled = NV_CHANNEL_ENABLE;
             wiphy_dbg(wiphy, "info: CH %d is enabled, no DFS, max EIRP (mBm) is %d\n", rfChannels[j].channelNum,
                 wiphy->regd->reg_rules[i].power_rule.max_eirp);
         }
         /*                                       */
         pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].pwrLimit =
            (tANI_S8) ((wiphy->regd->reg_rules[i].power_rule.max_eirp)/100);
      }
      /*                                                                    
                                                               */
      if (wiphy->regd->reg_rules[i].freq_range.max_bandwidth_khz == 40000)
      {
         wiphy_dbg(wiphy, "info: 40MHz (channel bonding) is allowed\n");
         bw40_start_channel_index =
            bw40_start_freq_to_channel_index(wiphy->regd->reg_rules[i].freq_range.start_freq_khz);
         bw40_end_channel_index =
            bw40_end_freq_to_channel_index(wiphy->regd->reg_rules[i].freq_range.end_freq_khz);
         if (bw40_start_channel_index == -1 || bw40_end_channel_index == -1)
         {
            wiphy_dbg(wiphy, "error: crda freq not supported, start_freq_khz %d end_freq_khz %d\n",
                wiphy->regd->reg_rules[i].freq_range.start_freq_khz,
                   wiphy->regd->reg_rules[i].freq_range.end_freq_khz);
            continue; //                                          
         }
         wiphy_dbg(wiphy, "40MHz start freq (KHz) %d end freq %d start ch index %d end ch index %d\n",
            wiphy->regd->reg_rules[i].freq_range.start_freq_khz,
                wiphy->regd->reg_rules[i].freq_range.end_freq_khz,
                   bw40_start_channel_index, bw40_end_channel_index);
         for (j=bw40_start_channel_index;j<=bw40_end_channel_index;j++)
         {
            if (channel_in_capable_band(j, nBandCapability) == VOS_FALSE)
                continue; //                                     
            if (wiphy->regd->reg_rules[i].flags & NL80211_RRF_DFS)
            {
                pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].enabled = NV_CHANNEL_DFS;
                wiphy_dbg(wiphy, "info: 40MHz centered on CH %d is DFS\n", rfChannels[j].channelNum);
            }
            else
            {
                pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].enabled = NV_CHANNEL_ENABLE;
                wiphy_dbg(wiphy, "info: 40MHz centered on CH %d is enabled, no DFS\n", rfChannels[j].channelNum);
            }
            /*                                                   */
            pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].pwrLimit =
                (tANI_S8) (((wiphy->regd->reg_rules[i].power_rule.max_eirp)/100)-3);
         }
      }
  }
  /*                                                                
                                                                     
                                                */
  if (crda_regulatory_entry_valid == VOS_FALSE)
  { /*           */
     crda_alpha2[0] = request->alpha2[0];
     crda_alpha2[1] = request->alpha2[1];
     crda_regulatory_entry_valid = VOS_TRUE;
  }
  else
  { /*                      */
     run_time_alpha2[0] = request->alpha2[0];
     run_time_alpha2[1] = request->alpha2[1];
     crda_regulatory_run_time_entry_valid = VOS_TRUE;
  }
  crda_regulatory_entry_post_processing(wiphy, request, nBandCapability, domain_id);
  return 0;
}

/*                                                                         
                           
                                                           
                           
     
                                                                           */
v_U16_t vos_chan_to_freq(v_U8_t chanNum)
{
   int i;

   for (i = 0; i < NUM_RF_CHANNELS; i++)
   {
      if (rfChannels[i].channelNum == chanNum)
      {
         return rfChannels[i].targetFreq;
      }
   }

   return (0);
}

#ifdef CONFIG_ENABLE_LINUX_REG

/*                                                                         
                              
                                                         
                                           
                                                
                                         
     
                                                                           */
VOS_STATUS vos_nv_setRegDomain(void * clientCtxt, v_REGDOMAIN_t regId,
                                                v_BOOL_t sendRegHint)
{

    if (regId >= REGDOMAIN_COUNT)
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                  "VOS set reg domain, invalid REG domain ID %d", regId);
        return VOS_STATUS_E_INVAL;
    }

    /*                                                     */
    regChannels = pnvEFSTable->halnv.tables.regDomains[regId].channels;

    return VOS_STATUS_SUCCESS;
}

/*                                                                         
                                                                            
                                  
                                                                              
                                                                       
                          
                                                   
                                   
                                                 
                                                                               
                                                    
                                                          
                                                                          
     
                                                                           */
VOS_STATUS vos_nv_getRegDomainFromCountryCode( v_REGDOMAIN_t *pRegDomain,
      const v_COUNTRYCODE_t country_code, v_CountryInfoSource_t source)
{

    v_CONTEXT_t pVosContext = NULL;
    hdd_context_t *pHddCtx = NULL;
    struct wiphy *wiphy = NULL;
    int i;
    int wait_result;

    /*               */
    if (NULL == pRegDomain)
    {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   ("Invalid reg domain pointer") );
        return VOS_STATUS_E_FAULT;
    }

    *pRegDomain = REGDOMAIN_COUNT;

    if (NULL == country_code)
    {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   ("Country code array is NULL"));
        return VOS_STATUS_E_FAULT;
    }

    if (0 == countryInfoTable.countryCount)
    {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   ("Reg domain table is empty") );
        return VOS_STATUS_E_EMPTY;
    }


    pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);

    if (NULL != pVosContext)
        pHddCtx = vos_get_context(VOS_MODULE_ID_HDD, pVosContext);
    else
        return VOS_STATUS_E_EXISTS;

    if (NULL == pHddCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   ("Invalid pHddCtx pointer") );
        return VOS_STATUS_E_FAULT;
    }

    temp_reg_domain = REGDOMAIN_COUNT;
    /*                                          */
    for (i = 0; i < countryInfoTable.countryCount &&
             REGDOMAIN_COUNT == temp_reg_domain; i++)
    {
        if (memcmp(country_code, countryInfoTable.countryInfo[i].countryCode,
                   VOS_COUNTRY_CODE_LEN) == 0)
        {
            /*                       */
            /*                                                */
            temp_reg_domain = countryInfoTable.countryInfo[i].regDomain;
            break;
        }
    }

    if (REGDOMAIN_COUNT == temp_reg_domain) {

        /*                                                  */
        /*                                                  */

        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   ("Country does not map to any Regulatory domain"));

        temp_reg_domain = REGDOMAIN_WORLD;
    }

    if (COUNTRY_QUERY == source)
    {
        *pRegDomain = temp_reg_domain;
         return VOS_STATUS_SUCCESS;
    }

    wiphy = pHddCtx->wiphy;

    if (false == wiphy->registered)
    {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   ("wiphy is not yet registered with the kernel") );
        return VOS_STATUS_E_FAULT;
    }

    /*                                                              
                        */


    /*                                                                      
                                                                 */

    VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                   ("regdomain request"));

    if ((country_code[0] == linux_reg_cc[0]) &&
        (country_code[1] == linux_reg_cc[1])) {

        /*                             */

        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                   (" country code already exists"));

        *pRegDomain = cur_reg_domain;

        return VOS_STATUS_SUCCESS;
    }
    else {

        /*                                               
                    */

        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_WARN,
                   (" get country information from kernel db"));


        if (COUNTRY_NV == source)
        {
            INIT_COMPLETION(pHddCtx->linux_reg_req);
            regulatory_hint(wiphy, country_code);
            wait_result = wait_for_completion_interruptible_timeout(
                                                            &pHddCtx->linux_reg_req,
                                                            LINUX_REG_WAIT_TIME);

            /*                                                           
                                                            */

            if (wait_result >= 0) {

                /*                                                       
                                                                           
                                                                              
                                                                             
                          */

                VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                           ("runtime country code is found in kernel db"));

                *pRegDomain = temp_reg_domain;
                cur_reg_domain = temp_reg_domain;
                linux_reg_cc[0] = country_code[0];
                linux_reg_cc[1] = country_code[1];

                return VOS_STATUS_SUCCESS;
            }
            else {

                /*                                                         
                                            */

                VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_WARN,
                           ("runtime country code is not found in kernel db"));

                return VOS_STATUS_E_EXISTS;
            }
        }
        else if (COUNTRY_IE == source || COUNTRY_USER == source)
        {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
            regulatory_hint_user(country_code,NL80211_USER_REG_HINT_USER);
#else
            regulatory_hint_user(country_code);
#endif
            *pRegDomain = temp_reg_domain;
        }

   }

   return VOS_STATUS_SUCCESS;
}

/*                                                                          */
static int create_linux_regulatory_entry(struct wiphy *wiphy,
                struct regulatory_request *request,
                v_U8_t nBandCapability)
{
    int i, j, m;
    int k = 0, n = 0;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0))
    int err;
#endif
    const struct ieee80211_reg_rule *reg_rule;
    v_CONTEXT_t pVosContext = NULL;
    hdd_context_t *pHddCtx = NULL;

    pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);

    if (NULL != pVosContext)
    {
        pHddCtx = vos_get_context(VOS_MODULE_ID_HDD, pVosContext);
        if (NULL == pHddCtx)
        {
           VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                       ("Invalid pHddCtx pointer") );
        }
        else
        {
           pHddCtx->isVHT80Allowed = 0;
        }
    }
    else
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                  ("Invalid pVosContext pointer") );
    }

    /*                */
    if (nBandCapability == eCSR_BAND_24)
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                  "BandCapability is set to 2G only\n");

    for (i = 0, m = 0; i<IEEE80211_NUM_BANDS; i++)
    {
        /*         */
        if (i == IEEE80211_BAND_2GHZ && nBandCapability == eCSR_BAND_5G)
            continue;

        /*         */
        else if (i == IEEE80211_BAND_5GHZ && nBandCapability == eCSR_BAND_24)
            continue;

        if (wiphy->bands[i] == NULL)
        {

            VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                      "error: wiphy->bands is NULL, i = %d\n", i);
            return -1;
        }

        /*                                                                    
                                                              */

        if (i == 0)
            m = 0;
        else
            m = wiphy->bands[i-1]->n_channels + m;

        for (j = 0; j < wiphy->bands[i]->n_channels; j++)
        {
            /*                                                                
                                                                          */

            k = m + j;
            n = bw20_ch_index_to_bw40_ch_index(k);

            if (n == -1)
                return -1;

            /*                                                        
                                                                      
                               
             */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
            reg_rule = freq_reg_info(wiphy,
                                     MHZ_TO_KHZ(wiphy->bands[i]->channels[j].center_freq));
#else
            err = freq_reg_info(wiphy,
                                MHZ_TO_KHZ(wiphy->bands[i]->channels[j].center_freq),
                                0, &reg_rule);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
            if (!IS_ERR(reg_rule))
#else
            if (0 == err)
#endif
            {
                if (!(reg_rule->flags & NL80211_RRF_PASSIVE_SCAN))
                {
                    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                              "%s: Remove passive scan restriction for %u",
                              __func__, wiphy->bands[i]->channels[j].center_freq);
                    wiphy->bands[i]->channels[j].flags &= ~IEEE80211_CHAN_PASSIVE_SCAN;
                }
            }

            if (wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_DISABLED)
            {
                if (pnvEFSTable == NULL)
                {
                    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                              "error: pnvEFSTable is NULL, probably not parsed nv.bin yet\n");
                    return -1;
                }
                pnvEFSTable->halnv.tables.regDomains[temp_reg_domain].channels[k].enabled =
                    NV_CHANNEL_DISABLE;
                pnvEFSTable->halnv.tables.regDomains[temp_reg_domain].channels[n].enabled =
                    NV_CHANNEL_DISABLE;
            }
            /*                                                        */
            else if (wiphy->bands[i]->channels[j].flags &
                    (IEEE80211_CHAN_RADAR | IEEE80211_CHAN_PASSIVE_SCAN))
            {
                pnvEFSTable->halnv.tables.regDomains[temp_reg_domain].channels[k].enabled =
                    NV_CHANNEL_DFS;

                /*                                 */
                pnvEFSTable->halnv.tables.regDomains[temp_reg_domain].channels[k].pwrLimit =
                    (tANI_S8) ((wiphy->bands[i]->channels[j].max_power));
                if ((wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_NO_HT40) == 0)
                {
                    pnvEFSTable->halnv.tables.regDomains[temp_reg_domain].channels[n].enabled =
                        NV_CHANNEL_DFS;

                    /*                                                */
                    pnvEFSTable->halnv.tables.regDomains[temp_reg_domain].channels[n].pwrLimit =
                        (tANI_S8) (((wiphy->bands[i]->channels[j].max_power))-3);
                }
                if ((wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_NO_80MHZ) == 0)
                {
                   if (NULL == pHddCtx)
                   {
                      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                                  ("Invalid pHddCtx pointer") );
                   }
                   else
                   {
                      pHddCtx->isVHT80Allowed = 1;
                   }
                }
            }
            else /*                                     */
            {
                pnvEFSTable->halnv.tables.regDomains[temp_reg_domain].channels[k].enabled =
                    NV_CHANNEL_ENABLE;

                /*                     */
                pnvEFSTable->halnv.tables.regDomains[temp_reg_domain].channels[k].pwrLimit =
                    (tANI_S8) ((wiphy->bands[i]->channels[j].max_power));
                if ((wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_NO_HT40) == 0)
                {
                    pnvEFSTable->halnv.tables.regDomains[temp_reg_domain].channels[n].enabled =
                        NV_CHANNEL_ENABLE;
                    /*                                             */
                    pnvEFSTable->halnv.tables.regDomains[temp_reg_domain].channels[n].pwrLimit =
                        (tANI_S8) (((wiphy->bands[i]->channels[j].max_power))-3);
                }
                if ((wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_NO_80MHZ) == 0)
                {
                   if (NULL == pHddCtx)
                   {
                      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                                  ("Invalid pHddCtx pointer") );
                   }
                   else
                   {
                      pHddCtx->isVHT80Allowed = 1;
                   }
                }

            }



        }
    }

    if (k == 0)
       return -1;

    return 0;
}


/*
                                        
                                                                            
                                                                          
                                                                                 
                            
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
void wlan_hdd_linux_reg_notifier(struct wiphy *wiphy,
                struct regulatory_request *request)
#else
int wlan_hdd_linux_reg_notifier(struct wiphy *wiphy,
                struct regulatory_request *request)
#endif
{
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
    eCsrBand nBandCapability = eCSR_BAND_ALL;
    v_COUNTRYCODE_t country_code;
    int i;
    v_BOOL_t isVHT80Allowed;

    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
               ("cfg80211 reg notifier callback for country"));

    if (TRUE == isWDresetInProgress())
    {
        wiphy_dbg(wiphy, "info: %s: SSR is in progress", __func__);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
        return;
#else
        return 0;
#endif
    }

    if (NULL == pHddCtx)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   ("Invalid pHddCtx pointer") );
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
       return;
#else
       return 0;
#endif
    }

    if (pHddCtx->isLoadUnloadInProgress)
    {
        wiphy_dbg(wiphy, "info: %s: Unloading/Loading in Progress. Ignore!!!",
                  __func__);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
        return;
#else
        return 0;
#endif
    }


    sme_GetFreqBand(pHddCtx->hHal, &nBandCapability);
    /*                                                                    */

    if (request->initiator == NL80211_REGDOM_SET_BY_DRIVER)
    {

        isVHT80Allowed = pHddCtx->isVHT80Allowed;
        if (create_linux_regulatory_entry(wiphy, request, nBandCapability) == 0)
        {

            VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                       (" regulatory entry created"));
        }
        if (pHddCtx->isVHT80Allowed != isVHT80Allowed)
        {
           hdd_checkandupdate_phymode( pHddCtx);
        }

        complete(&pHddCtx->linux_reg_req);
    }

    else if (request->initiator == NL80211_REGDOM_SET_BY_USER ||
             request->initiator ==  NL80211_REGDOM_SET_BY_CORE)
    {

        /*                                                */

        country_code[0] = request->alpha2[0];
        country_code[1] = request->alpha2[1];

        temp_reg_domain = REGDOMAIN_COUNT;
        for (i = 0; i < countryInfoTable.countryCount &&
                 REGDOMAIN_COUNT == temp_reg_domain; i++)
        {
            if (memcmp(country_code, countryInfoTable.countryInfo[i].countryCode,
                       VOS_COUNTRY_CODE_LEN) == 0)
            {
                /*                       */
                /*                                                */
                temp_reg_domain = countryInfoTable.countryInfo[i].regDomain;
                break;
            }
        }

        if  (REGDOMAIN_COUNT == temp_reg_domain)
            temp_reg_domain = REGDOMAIN_WORLD;

        nBandCapability = pHddCtx->cfg_ini->nBandCapability;
        isVHT80Allowed = pHddCtx->isVHT80Allowed;
        if (create_linux_regulatory_entry(wiphy, request,
                                          nBandCapability) == 0)
        {
            VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                      (" regulatory entry created"));

        }
        if (pHddCtx->isVHT80Allowed != isVHT80Allowed)
        {
           hdd_checkandupdate_phymode( pHddCtx);
        }

        cur_reg_domain = temp_reg_domain;
        linux_reg_cc[0] = country_code[0];
        linux_reg_cc[1] = country_code[1];

        /*                                             */
        if (request->alpha2[0] == '0' && request->alpha2[1] == '0')
        {
           sme_GenericChangeCountryCode(pHddCtx->hHal, country_code,
                                        REGDOMAIN_COUNT);
        }
        else
        {
           sme_GenericChangeCountryCode(pHddCtx->hHal, country_code,
                                        temp_reg_domain);
        }

    }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
    return;
#else
    return 0;
#endif
}


/*                              */
VOS_STATUS vos_init_wiphy_from_nv_bin(void)
{
    int i, j, m;
    int k = 0;
    v_REGDOMAIN_t reg_domain;
    v_CONTEXT_t pVosContext = NULL;
    hdd_context_t *pHddCtx = NULL;
    struct wiphy *wiphy = NULL;

    pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);

    if (NULL != pVosContext)
        pHddCtx = vos_get_context(VOS_MODULE_ID_HDD, pVosContext);
    else
        return VOS_STATUS_E_EXISTS;

    if (NULL == pHddCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   ("Invalid pHddCtx pointer") );
        return VOS_STATUS_E_FAULT;
    }

    wiphy = pHddCtx->wiphy;

    if  (('0' == pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[0])
         &&
         ('0' == pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[1]))
    {
        /*                                  */

        reg_domain = REGDOMAIN_WORLD;
        wiphy->flags |= WIPHY_FLAG_CUSTOM_REGULATORY;
    }
    else if (REGDOMAIN_WORLD ==
	     pnvEFSTable->halnv.tables.defaultCountryTable.regDomain) {

        reg_domain = pnvEFSTable->halnv.tables.defaultCountryTable.regDomain;
        wiphy->flags |= WIPHY_FLAG_CUSTOM_REGULATORY;
    }
    else {

        reg_domain = pnvEFSTable->halnv.tables.defaultCountryTable.regDomain;
        wiphy->flags |= WIPHY_FLAG_STRICT_REGULATORY;
    }

    m = 0;
    for (i = 0; i < IEEE80211_NUM_BANDS; i++)
    {

        if (wiphy->bands[i] == NULL)
        {
            pr_info("error: wiphy->bands[i] is NULL, i = %d\n", i);
            continue;
        }

        /*                                                                    
                                                              */

        for (j = 0; j < wiphy->bands[i]->n_channels; j++)
        {
            /*                                               */
            k = m + j;

            if (pnvEFSTable->halnv.tables.regDomains[reg_domain].channels[k].enabled ==
                NV_CHANNEL_DISABLE)
                wiphy->bands[i]->channels[j].flags |= IEEE80211_CHAN_DISABLED;

            else if (pnvEFSTable->halnv.tables.regDomains[reg_domain].channels[k].enabled ==
                     NV_CHANNEL_DFS) {

                wiphy->bands[i]->channels[j].flags |= IEEE80211_CHAN_PASSIVE_SCAN;

                wiphy->bands[i]->channels[j].max_power =
                    (pnvEFSTable->halnv.tables.regDomains[reg_domain].channels[k].pwrLimit)*100;
            }

            else if (pnvEFSTable->halnv.tables.regDomains[reg_domain].channels[k].enabled ==
                     NV_CHANNEL_ENABLE) {

                wiphy->bands[i]->channels[j].max_power =
                    (pnvEFSTable->halnv.tables.regDomains[reg_domain].channels[k].pwrLimit)*100;
            }
        }

	m += wiphy->bands[i]->n_channels;
    }

    return VOS_STATUS_SUCCESS;
}


#else

/*                                                                         
                              
                                                         
                                           
                                                
                                         
     
                                                                           */
VOS_STATUS vos_nv_setRegDomain(void * clientCtxt, v_REGDOMAIN_t regId,
                                                  v_BOOL_t sendRegHint)
{
    v_CONTEXT_t pVosContext = NULL;
    hdd_context_t *pHddCtx = NULL;
    struct wiphy *wiphy = NULL;
   /*                                            */
   if (regId >= REGDOMAIN_COUNT)
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "VOS set reg domain, invalid REG domain ID %d", regId);
      return VOS_STATUS_E_INVAL;
   }

   pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
   if (NULL != pVosContext)
      pHddCtx = vos_get_context(VOS_MODULE_ID_HDD, pVosContext);
   else
      return VOS_STATUS_E_EXISTS;
   /*                                                     */
   regChannels = pnvEFSTable->halnv.tables.regDomains[regId].channels;

   /*                                                    
                                                              
                                          */
   if (NULL != pHddCtx && sendRegHint)
   {
      wiphy = pHddCtx->wiphy;
      regulatory_hint(wiphy, "00");
   }
   return VOS_STATUS_SUCCESS;
}


/*                                                                         
                                                                            
                                  
                                                                              
                                                                       
                          
                                                   
                                   
                                                 
                                                                               
                                                    
                                                          
                                                                          
     
                                                                           */
VOS_STATUS vos_nv_getRegDomainFromCountryCode( v_REGDOMAIN_t *pRegDomain,
      const v_COUNTRYCODE_t countryCode, v_CountryInfoSource_t source)
{
   int i;
   v_CONTEXT_t pVosContext = NULL;
   hdd_context_t *pHddCtx = NULL;
   struct wiphy *wiphy = NULL;
   int status;

   //              
   if (NULL == pRegDomain)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            ("Invalid reg domain pointer\n") );
      return VOS_STATUS_E_FAULT;
   }
   *pRegDomain = REGDOMAIN_COUNT;

   if (NULL == countryCode)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            ("Country code array is NULL\r\n") );
      return VOS_STATUS_E_FAULT;
   }
   if (0 == countryInfoTable.countryCount)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            ("Reg domain table is empty\r\n") );
      return VOS_STATUS_E_EMPTY;
   }
   /*                                                           
                                      
                                                                
                                              
                                                      */
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
          "vos_nv_getRegDomainFromCountryCode %c%c\n",
          countryCode[0], countryCode[1]);

   if (crda_regulatory_entry_valid == VOS_TRUE)
   {
       if (crda_alpha2[0]==countryCode[0] && crda_alpha2[1]==countryCode[1])
       {
          *pRegDomain = NUM_REG_DOMAINS-1;
              VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
              "vos_nv_getRegDomainFromCountryCode return crda init entry\n");
          return VOS_STATUS_SUCCESS;
       }
       if (run_time_alpha2[0]==countryCode[0] &&
           run_time_alpha2[1]==countryCode[1] &&
           crda_regulatory_run_time_entry_valid == VOS_TRUE)
       {
          *pRegDomain = NUM_REG_DOMAINS-2;
              VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
              "vos_nv_getRegDomainFromCountryCode return crda none-default country entry\n");
           return VOS_STATUS_SUCCESS;
       }
       else
       {
           crda_regulatory_run_time_entry_valid = VOS_FALSE;
           pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
           if (NULL != pVosContext)
               pHddCtx = vos_get_context(VOS_MODULE_ID_HDD, pVosContext);
           else
               return VOS_STATUS_E_EXISTS;
           if (NULL == pHddCtx)
           {
              VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    ("Invalid pHddCtx pointer\r\n") );
              return VOS_STATUS_E_FAULT;
           }

           wiphy = pHddCtx->wiphy;

           INIT_COMPLETION(pHddCtx->driver_crda_req);
           regulatory_hint(wiphy, countryCode);
           status = wait_for_completion_interruptible_timeout(
                   &pHddCtx->driver_crda_req,
                   msecs_to_jiffies(CRDA_WAIT_TIME));
           if (!status)
           {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                       "%s: Timeout waiting for CRDA REQ", __func__);
           }

           if (crda_regulatory_run_time_entry_valid == VOS_TRUE)
           {
              VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
                 "vos_nv_getRegDomainFromCountryCode return crda new none-default country entry\n");
               return VOS_STATUS_SUCCESS;
           }
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "vos_nv_getRegDomainFromCountryCode failed to get crda new none-default country entry\n");
           return VOS_STATUS_E_EXISTS;
       }
   }

   //                                                                      
   //         
   for (i = 0; i < countryInfoTable.countryCount &&
         REGDOMAIN_COUNT == *pRegDomain; i++)
   {
      if (memcmp(countryCode, countryInfoTable.countryInfo[i].countryCode,
               VOS_COUNTRY_CODE_LEN) == 0)
      {
         //                      
         *pRegDomain = countryInfoTable.countryInfo[i].regDomain;
      }
   }
   if (REGDOMAIN_COUNT != *pRegDomain)
   {
      return VOS_STATUS_SUCCESS;
   }
   else
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_WARN,
            ("country code is not found\r\n"));
      return VOS_STATUS_E_EXISTS;
   }
}
/*                                        
                                     
*/
void* vos_nv_change_country_code_cb(void *pAdapter)
{
   struct completion *change_code_cng = pAdapter;
   complete(change_code_cng);
   return NULL;
}

/*
                                       
                                                                            
                                                                          
                                                                                 
                            
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
void wlan_hdd_crda_reg_notifier(struct wiphy *wiphy,
                struct regulatory_request *request)
#else
int wlan_hdd_crda_reg_notifier(struct wiphy *wiphy,
                struct regulatory_request *request)
#endif
{
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
    v_REGDOMAIN_t domainIdCurrent;
    tANI_U8 ccode[WNI_CFG_COUNTRY_CODE_LEN];
    tANI_U8 uBufLen = WNI_CFG_COUNTRY_CODE_LEN;
    eCsrBand nBandCapability = eCSR_BAND_ALL;
    int i,j,k,m;

    wiphy_dbg(wiphy, "info: cfg80211 reg_notifier callback for country"
                     " %c%c\n", request->alpha2[0], request->alpha2[1]);

    /*                                                                    
                                                                     
                                  
     */
    if (NULL == pHddCtx)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   ("%s Invalid pHddCtx pointer"), __func__);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
       return;
#else
       return 0;
#endif
    }

//                                                         
#if 0
    if(pHddCtx->isLoadUnloadInProgress ||
        pHddCtx->isLogpInProgress)
#else
    if(pHddCtx->isLogpInProgress)
#endif
    {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   ("%s load/unload or SSR is in progress Ignore"), __func__ );
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
       return;
#else
       return 0;
#endif
    }

    if (request->initiator == NL80211_REGDOM_SET_BY_USER)
    {
       int status;
       wiphy_dbg(wiphy, "info: set by user\n");
       memset(ccode, 0, WNI_CFG_COUNTRY_CODE_LEN);
       memcpy(ccode, request->alpha2, 2);
       init_completion(&change_country_code);
       /*                                                      
                                                             
                                  
                                                                            
                                                                         
                                                                             
                                                                           
                                                     
                                                             
                                                         
                                        */
       status = sme_ChangeCountryCode(pHddCtx->hHal,
                                   (void *)(tSmeChangeCountryCallback)
                                   vos_nv_change_country_code_cb,
                                   ccode,
                                   &change_country_code,
                                   pHddCtx->pvosContext,
                                   eSIR_FALSE,
                                   eSIR_FALSE);
       if (eHAL_STATUS_SUCCESS == status)
       {
          status = wait_for_completion_interruptible_timeout(
                                       &change_country_code,
                                       800);
          if(status <= 0)
          {
             wiphy_dbg(wiphy, "info: set country timed out\n");
          }
       }
       else
       {
          wiphy_dbg(wiphy, "info: unable to set country by user\n");
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
          return;
#else
          return 0;
#endif
       }
       //     
       /*                                                                    */
       /*                                                                   
                                        */
       //                                          
       //                                                      
    }
    sme_GetFreqBand(pHddCtx->hHal, &nBandCapability);
    if (request->initiator == NL80211_REGDOM_SET_BY_COUNTRY_IE)
    {
       wiphy_dbg(wiphy, "info: set by country IE\n");
       if (create_crda_regulatory_entry(wiphy, request, nBandCapability) != 0)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
          return;
#else
          return 0;
#endif
       //     
       /*                                    */

       /*                                                                   */
       /*                                                                  
                                               */
       //                                           
       //                                                      
    }
    else if (request->initiator == NL80211_REGDOM_SET_BY_DRIVER ||
             (request->initiator == NL80211_REGDOM_SET_BY_CORE)||
                (request->initiator == NL80211_REGDOM_SET_BY_USER))
    {
         if ( eHAL_STATUS_SUCCESS !=  sme_GetCountryCode(pHddCtx->hHal, ccode, &uBufLen))
         {
            wiphy_dbg(wiphy, "info: set by driver CCODE ERROR\n");
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
            return;
#else
            return 0;
#endif
         }
         if (eHAL_STATUS_SUCCESS != sme_GetRegulatoryDomainForCountry (pHddCtx->hHal,
                                             ccode, (v_REGDOMAIN_t *) &domainIdCurrent))
         {
            wiphy_dbg(wiphy, "info: set by driver ERROR\n");
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
            return;
#else
            return 0;
#endif
         }

         wiphy_dbg(wiphy, "country: %c%c set by driver\n",ccode[0],ccode[1]);
         /*                                                             
                                                                             
                                                                            
                                                         */

         for (i = 0, m = 0; i < IEEE80211_NUM_BANDS; i++)
         {
             if (NULL == wiphy->bands[i])
             {
                 VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                           "error: wiphy->bands[i] is NULL, i = %d", i);
                 continue;
             }

             //                                                                    
             //                                                   
             if (0 == i)
             {
                 m = 0;
             }
             else
             {
                 m = wiphy->bands[i-1]?wiphy->bands[i-1]->n_channels + m:m;
             }

             for (j=0; j<wiphy->bands[i]->n_channels; j++)
             {
                 //                                                                
                 //                                                            
                 k = m + j;
                 if (IEEE80211_BAND_2GHZ == i && eCSR_BAND_5G == nBandCapability) //        
                 {
                     //                               
                     if ((2412 == wiphy->bands[i]->channels[j].center_freq ||
                          2437 == wiphy->bands[i]->channels[j].center_freq ||
                          2462 == wiphy->bands[i]->channels[j].center_freq ) &&
                         NV_CHANNEL_ENABLE == regChannels[k].enabled)
                     {
                         wiphy->bands[i]->channels[j].flags &= ~IEEE80211_CHAN_DISABLED;
                     }
                     else
                     {
                         wiphy->bands[i]->channels[j].flags |= IEEE80211_CHAN_DISABLED;
                     }
                     continue;
                 }
                 else if (IEEE80211_BAND_5GHZ == i && eCSR_BAND_24 == nBandCapability) //        
                 {
                     wiphy->bands[i]->channels[j].flags |= IEEE80211_CHAN_DISABLED;
                     continue;
                 }

                 if (NV_CHANNEL_DISABLE == regChannels[k].enabled ||
                     NV_CHANNEL_INVALID == regChannels[k].enabled)
                 {
                     wiphy->bands[i]->channels[j].flags |= IEEE80211_CHAN_DISABLED;
                 }
                 else if (NV_CHANNEL_DFS == regChannels[k].enabled)
                 {
                     wiphy->bands[i]->channels[j].flags &= ~(IEEE80211_CHAN_DISABLED
                                                             |IEEE80211_CHAN_RADAR);
                     wiphy->bands[i]->channels[j].flags |= IEEE80211_CHAN_PASSIVE_SCAN;
                 }
                 else
                 {
                     wiphy->bands[i]->channels[j].flags &= ~(IEEE80211_CHAN_DISABLED
                                                             |IEEE80211_CHAN_PASSIVE_SCAN
                                                             |IEEE80211_CHAN_NO_IBSS
                                                             |IEEE80211_CHAN_RADAR);
                 }
             }
         }

         /*                                                               
                                                                     */

         /*                                                    
                                                           
                                                                  
                                                                
                                            
                                                                
                                                                     */

         if ( pHddCtx->cfg_ini->gEnableStrictRegulatoryForFCC &&
              wiphy->bands[IEEE80211_BAND_5GHZ])
         {
             for (j=0; j<wiphy->bands[IEEE80211_BAND_5GHZ]->n_channels; j++)
             {
/*                                                                                     */
#if 0
                 //                                                         
                 if ((wiphy->bands[IEEE80211_BAND_5GHZ ]->channels[j].center_freq == 5180 ||
                      wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5200 ||
                      wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5220 ||
                      wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5240) &&
                     ((domainIdCurrent == REGDOMAIN_FCC) && pHddCtx->nEnableStrictRegulatoryForFCC))
                 {
                     wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].flags |= IEEE80211_CHAN_PASSIVE_SCAN;
                 }
                 else if ((wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5180 ||
                           wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5200 ||
                           wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5220 ||
                           wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5240) &&
                          ((domainIdCurrent != REGDOMAIN_FCC) || !pHddCtx->nEnableStrictRegulatoryForFCC))
                 {
                     wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].flags &= ~IEEE80211_CHAN_PASSIVE_SCAN;
                 }
#else
                if ((wiphy->bands[IEEE80211_BAND_5GHZ ]->channels[j].center_freq == 5180 ||
                               wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5200 ||
                               wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5220 ||
                               wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5240)
                               )
                {
                   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "freq:%u unset PASSIVE_SCAN by lge",
                               wiphy->bands[IEEE80211_BAND_5GHZ ]->channels[j].center_freq );
                   wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].flags &= ~IEEE80211_CHAN_PASSIVE_SCAN;
                }
#endif
             }
         }

         if (request->initiator == NL80211_REGDOM_SET_BY_CORE)
         {
             request->processed = 1;
         }
    }

    complete(&pHddCtx->wiphy_channel_update_event);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
    return;
#else
    return 0;
#endif
}

#endif
