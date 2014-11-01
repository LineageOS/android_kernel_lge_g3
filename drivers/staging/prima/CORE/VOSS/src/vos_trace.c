/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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

/**=========================================================================

  \file  vos_trace.c

  \brief virtual Operating System Servies (vOS)

   Trace, logging, and debugging definitions and APIs

   Copyright 2008,2011 (c) Qualcomm, Incorporated.  All Rights Reserved.

   Qualcomm Confidential and Proprietary.

  ========================================================================*/

/*                                                                           

                                            


                                                                       
                                                                


                                   


                                     
                                                                             
                                                                              
                                                                            
                                                                         
                                   
                                                                           */

/*                                                                          
               
                                                                          */
#include <vos_trace.h>
/*                                                                          
                                        
                                                                          */

#define VOS_TRACE_BUFFER_SIZE ( 512 )

//                                               
#define VOS_TRACE_LEVEL_TO_MODULE_BITMASK( _level ) ( ( 1 << (_level) ) )

typedef struct
{
   //                                                               
   //                                                                  
   //                                                                 
   //                                                              
   //                                                                
   //                                  
   //
   //                                               
   v_U16_t moduleTraceLevel;

   //                                       
   unsigned char moduleNameStr[ 4 ];   //                      

} moduleTraceInfo;

#define VOS_DEFAULT_TRACE_LEVEL \
   ((1<<VOS_TRACE_LEVEL_FATAL)|(1<<VOS_TRACE_LEVEL_ERROR))

//                                                               
//                                                               
//                                                                 
moduleTraceInfo gVosTraceInfo[ VOS_MODULE_ID_MAX ] =
{
   [VOS_MODULE_ID_BAP]        = { VOS_DEFAULT_TRACE_LEVEL, "BAP" },
   [VOS_MODULE_ID_TL]         = { VOS_DEFAULT_TRACE_LEVEL, "TL " },
   [VOS_MODULE_ID_WDI]        = { VOS_DEFAULT_TRACE_LEVEL, "WDI"},
   [VOS_MODULE_ID_HDD]        = { VOS_DEFAULT_TRACE_LEVEL, "HDD" },
   [VOS_MODULE_ID_SME]        = { VOS_DEFAULT_TRACE_LEVEL, "SME" },
   [VOS_MODULE_ID_PE]         = { VOS_DEFAULT_TRACE_LEVEL, "PE " },
   [VOS_MODULE_ID_WDA]        = { VOS_DEFAULT_TRACE_LEVEL, "WDA" },
   [VOS_MODULE_ID_SYS]        = { VOS_DEFAULT_TRACE_LEVEL, "SYS" },
   [VOS_MODULE_ID_VOSS]       = { VOS_DEFAULT_TRACE_LEVEL, "VOS" },
   [VOS_MODULE_ID_SAP]        = { VOS_DEFAULT_TRACE_LEVEL, "SAP" },
   [VOS_MODULE_ID_HDD_SOFTAP] = { VOS_DEFAULT_TRACE_LEVEL, "HSP" },
   [VOS_MODULE_ID_PMC]        = { VOS_DEFAULT_TRACE_LEVEL, "PMC" },
};


/*                                                                         
           
                                                                          */
void vos_trace_setLevel( VOS_MODULE_ID module, VOS_TRACE_LEVEL level )
{
   //                                                  
   if ( level >= VOS_TRACE_LEVEL_MAX )
   {
      pr_err("%s: Invalid trace level %d passed in!\n", __func__, level);
      return;
   }

   //                                                             
   //                                                                       
   //                                                           
   if ( VOS_TRACE_LEVEL_NONE == level )
   {
      gVosTraceInfo[ module ].moduleTraceLevel = VOS_TRACE_LEVEL_NONE;
   }
   else
   {
      //                                                                
      gVosTraceInfo[ module ].moduleTraceLevel |= VOS_TRACE_LEVEL_TO_MODULE_BITMASK( level );
   }
}

void vos_trace_setValue( VOS_MODULE_ID module, VOS_TRACE_LEVEL level, v_U8_t on)
{
   //                                                  
   if ( level < 0  || level >= VOS_TRACE_LEVEL_MAX )
   {
      pr_err("%s: Invalid trace level %d passed in!\n", __func__, level);
      return;
   }

   //                                                   
   if ( module < 0 || module >= VOS_MODULE_ID_MAX )
   {
      pr_err("%s: Invalid module id %d passed in!\n", __func__, module);
      return;
   }

   //                                                              
   //                                                       
   if ( VOS_TRACE_LEVEL_NONE == level )
   {
      gVosTraceInfo[ module ].moduleTraceLevel = VOS_TRACE_LEVEL_NONE;
   }
   //                                                           
   //                                                      
   else if ( VOS_TRACE_LEVEL_ALL == level )
   {
      gVosTraceInfo[ module ].moduleTraceLevel = 0xFFFF;
   }

   else
   {
      if (on)
         //                                                                
         gVosTraceInfo[ module ].moduleTraceLevel |= VOS_TRACE_LEVEL_TO_MODULE_BITMASK( level );
      else
         //                                                                  
         gVosTraceInfo[ module ].moduleTraceLevel &= ~(VOS_TRACE_LEVEL_TO_MODULE_BITMASK( level ));
   }
}


v_BOOL_t vos_trace_getLevel( VOS_MODULE_ID module, VOS_TRACE_LEVEL level )
{
   v_BOOL_t traceOn = VOS_FALSE;

   if ( ( VOS_TRACE_LEVEL_NONE == level ) ||
        ( VOS_TRACE_LEVEL_ALL  == level ) ||
        ( level >= VOS_TRACE_LEVEL_MAX  )    )
   {
      traceOn = VOS_FALSE;
   }
   else
   {
      traceOn = ( level & gVosTraceInfo[ module ].moduleTraceLevel ) ? VOS_TRUE : VOS_FALSE;
   }

   return( traceOn );
}

void vos_snprintf(char *strBuffer, unsigned  int size, char *strFormat, ...)
{
    va_list val;

    va_start( val, strFormat );
    snprintf (strBuffer, size, strFormat, val);
    va_end( val );
}

#ifdef WCONN_TRACE_KMSG_LOG_BUFF

/*                                    
                                           */
#define KMSG_WCONN_TRACE_LOG_MAX    65536

static char wconnStrLogBuff[KMSG_WCONN_TRACE_LOG_MAX];
static unsigned int wconnstrContBuffIdx;
static void kmsgwconnstrlogchar(char c)
{
   wconnStrLogBuff[wconnstrContBuffIdx & (KMSG_WCONN_TRACE_LOG_MAX-1)] = c;
   wconnstrContBuffIdx++;
}

/*                                                                             
                                  
                                                                       
                                                                       
                                                                            
               
                                                           
  
                                                                          
                                                                        
                         
                                                                               */

void kmsgwconnBuffWrite(const char *wconnlogstrRead)
{
   const char *pwconnlogstr = wconnlogstrRead;
   static const char num[16] = {'0','1','2','3','4','5','6','7','8','9','A',
                                'B','C','D','E','F'};
   unsigned int timetick;
   int bits; /*                                      */

   timetick = ( jiffies_to_msecs(jiffies) / 10 );
   bits = sizeof(timetick) * 8/*                        */;

   kmsgwconnstrlogchar('[');

   for ( ; bits > 0; bits -= 4 )
      kmsgwconnstrlogchar( num[((timetick & (0xF << (bits-4)))>>(bits-4))] );

   kmsgwconnstrlogchar(']');

   for ( ; *pwconnlogstr; pwconnlogstr++)
   {
      kmsgwconnstrlogchar(*pwconnlogstr);
   }
   kmsgwconnstrlogchar('\n');/*      */
}

spinlock_t gVosSpinLock;

void vos_wconn_trace_init(void)
{
    spin_lock_init(&gVosSpinLock);
}

void vos_wconn_trace_exit(void)
{
    /*              */
}

#endif

#ifdef VOS_ENABLE_TRACING

/*                                                                            

                                                           

                                                                        

                                                                    
                                                                          

                                                               
                                                                         
                                                                      
                             

                                                                           
                                                                         
                                                      

                  

     

                                                                            */
void vos_trace_msg( VOS_MODULE_ID module, VOS_TRACE_LEVEL level, char *strFormat, ... )
{
   char strBuffer[VOS_TRACE_BUFFER_SIZE];
   int n;
   unsigned long irq_flag;

   //                                                                        
   //                   
   if ( gVosTraceInfo[ module ].moduleTraceLevel & VOS_TRACE_LEVEL_TO_MODULE_BITMASK( level ) )
   {
      //                                                                          
      //                                                                        
      //                                                                        
      //                        
      //                                                                     
      static const char * TRACE_LEVEL_STR[] = { "  ", "F ", "E ", "W ", "I ", "IH", "IM", "IL", "D" };
      va_list val;
      va_start(val, strFormat);

      //                                                  
      n = snprintf(strBuffer, VOS_TRACE_BUFFER_SIZE, "wlan: [%d:%2s:%3s] ",
                   in_interrupt() ? 0 : current->pid,
                   (char *) TRACE_LEVEL_STR[ level ],
                   (char *) gVosTraceInfo[ module ].moduleNameStr );

      //                                                         
      if ((n >= 0) && (n < VOS_TRACE_BUFFER_SIZE))
      {
         vsnprintf(strBuffer + n, VOS_TRACE_BUFFER_SIZE - n, strFormat, val );

#ifdef WCONN_TRACE_KMSG_LOG_BUFF
         spin_lock_irqsave (&gVosSpinLock, irq_flag);
         kmsgwconnBuffWrite(strBuffer);
         spin_unlock_irqrestore (&gVosSpinLock, irq_flag);
#endif
         pr_err("%s\n", strBuffer);
      }
     va_end(val);
   }
}

void vos_trace_display(void)
{
   VOS_MODULE_ID moduleId;

   pr_err("     1)FATAL  2)ERROR  3)WARN  4)INFO  5)INFO_H  6)INFO_M  7)INFO_L 8)DEBUG\n");
   for (moduleId = 0; moduleId < VOS_MODULE_ID_MAX; ++moduleId)
   {
      pr_err("%2d)%s    %s        %s       %s       %s        %s         %s         %s        %s\n",
             (int)moduleId,
             gVosTraceInfo[moduleId].moduleNameStr,
             (gVosTraceInfo[moduleId].moduleTraceLevel & (1 << VOS_TRACE_LEVEL_FATAL)) ? "X":" ",
             (gVosTraceInfo[moduleId].moduleTraceLevel & (1 << VOS_TRACE_LEVEL_ERROR)) ? "X":" ",
             (gVosTraceInfo[moduleId].moduleTraceLevel & (1 << VOS_TRACE_LEVEL_WARN)) ? "X":" ",
             (gVosTraceInfo[moduleId].moduleTraceLevel & (1 << VOS_TRACE_LEVEL_INFO)) ? "X":" ",
             (gVosTraceInfo[moduleId].moduleTraceLevel & (1 << VOS_TRACE_LEVEL_INFO_HIGH)) ? "X":" ",
             (gVosTraceInfo[moduleId].moduleTraceLevel & (1 << VOS_TRACE_LEVEL_INFO_MED)) ? "X":" ",
             (gVosTraceInfo[moduleId].moduleTraceLevel & (1 << VOS_TRACE_LEVEL_INFO_LOW)) ? "X":" ",
             (gVosTraceInfo[moduleId].moduleTraceLevel & (1 << VOS_TRACE_LEVEL_DEBUG)) ? "X":" "
         );
   }
}

/*                                                                            

                                                                   

                                                                        

                                                                    
                                                                          

                                                               
                                                                         
                                                                      
                             

                                                               

                                                          

                  

     
                                                                            */
void vos_trace_hex_dump( VOS_MODULE_ID module, VOS_TRACE_LEVEL level,
                                void *data, int buf_len )
{
    char *buf = (char *)data;
    int i;
    for (i=0; (i+7)<buf_len; i+=8)
    {
        vos_trace_msg( module, level,
                 "%02x %02x %02x %02x %02x %02x %02x %02x \n",
                 buf[i],
                 buf[i+1],
                 buf[i+2],
                 buf[i+3],
                 buf[i+4],
                 buf[i+5],
                 buf[i+6],
                 buf[i+7]);
    }

    //                                
    for (; i < buf_len; i++)
    {
        vos_trace_msg( module, level, "%02x ", buf[i]);
        if ((i+1) == buf_len)
            vos_trace_msg( module, level, "\n");
    }

}

#endif
