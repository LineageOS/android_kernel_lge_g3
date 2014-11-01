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
  
  \file  wlan_qct_pal_trace.c
  
  \brief Implementation trace/logging APIs PAL exports. wpt = (Wlan Pal Type) wpal = (Wlan PAL)
               
   Definitions for Linux/Android platform
  
   Copyright 2010-2011 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

#include "wlan_qct_pal_trace.h"
#include "i_vos_types.h"

#ifdef WLAN_DEBUG


/*                                                                          
                                        
                                                                          */

#define WPAL_TRACE_BUFFER_SIZE ( 512 )

//                                                
#define WPAL_TRACE_LEVEL_TO_MODULE_BITMASK( _level ) ( ( 1 << (_level) ) )

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
   wpt_uint16 moduleTraceLevel;

   //                                       
   wpt_uint8 moduleNameStr[ 4 ];   //                      

} moduleTraceInfo;


//                                                               
//                                                               
//                                                                 
moduleTraceInfo gTraceInfo[ eWLAN_MODULE_COUNT ] =
{
   { (1<<eWLAN_PAL_TRACE_LEVEL_FATAL)|(1<<eWLAN_PAL_TRACE_LEVEL_ERROR), "DAL" }, 
   { (1<<eWLAN_PAL_TRACE_LEVEL_FATAL)|(1<<eWLAN_PAL_TRACE_LEVEL_ERROR), "CTL" },
   { (1<<eWLAN_PAL_TRACE_LEVEL_FATAL)|(1<<eWLAN_PAL_TRACE_LEVEL_ERROR), "DAT" }, 
   { (1<<eWLAN_PAL_TRACE_LEVEL_FATAL)|(1<<eWLAN_PAL_TRACE_LEVEL_ERROR), "PAL" }, 
};


//                                                                          
//                                                                       
//                                                                        
//                    
//                                                              
static const char * TRACE_LEVEL_STR[] = {
   "  ", "F ", "E ", "W ", "I ", "IH", "IM", "IL" };


/*                                                                         
           
                                                                          */
static void wpalOutput(wpt_tracelevel level, char *strBuffer)
{
   switch(level)
   {
   default:
      printk(KERN_CRIT "%s: Unknown trace level passed in!\n", __func__); 
      //                        

   case eWLAN_PAL_TRACE_LEVEL_FATAL:
      printk(KERN_CRIT "%s\n", strBuffer);
      break;

   case eWLAN_PAL_TRACE_LEVEL_ERROR:
      printk(KERN_ERR "%s\n", strBuffer);
      break;

   case eWLAN_PAL_TRACE_LEVEL_WARN:
      printk(KERN_WARNING "%s\n", strBuffer);
      break;

   case eWLAN_PAL_TRACE_LEVEL_INFO:
      printk(KERN_INFO "%s\n", strBuffer);
      break;

   case eWLAN_PAL_TRACE_LEVEL_INFO_HIGH:
      printk(KERN_NOTICE "%s\n", strBuffer);
      break;

   case eWLAN_PAL_TRACE_LEVEL_INFO_MED:
      printk(KERN_NOTICE "%s\n", strBuffer);
      break;

   case eWLAN_PAL_TRACE_LEVEL_INFO_LOW:
      printk(KERN_INFO "%s\n", strBuffer);
      break;
   }
}

void wpalTraceSetLevel( wpt_moduleid module, wpt_tracelevel level,
                        wpt_boolean on )
{
   //                                                             
   if ( (eWLAN_PAL_TRACE_LEVEL_COUNT <= level) ||
        (eWLAN_MODULE_COUNT <= module) )
   {
      return;
   }

   if ( eWLAN_PAL_TRACE_LEVEL_NONE == level )
   {
      //                                                              
      //                                                       
      gTraceInfo[ module ].moduleTraceLevel = 0;
   }
   else if ( eWLAN_PAL_TRACE_LEVEL_ALL == level )
   {
      //                                                           
      //                                                      
      gTraceInfo[ module ].moduleTraceLevel = 0xFFFF;
   }
   else
   {
      //                                                  
      if (on)
      {
         //                                                                
         gTraceInfo[ module ].moduleTraceLevel |=
            WPAL_TRACE_LEVEL_TO_MODULE_BITMASK( level );
      }
      else
      {
         //                                                                  
         gTraceInfo[ module ].moduleTraceLevel &=
            ~(WPAL_TRACE_LEVEL_TO_MODULE_BITMASK( level ));
      }
   }
}

wpt_boolean wpalTraceCheckLevel( wpt_moduleid module, wpt_tracelevel level )
{
   wpt_boolean traceOn = eWLAN_PAL_FALSE;

   if ( ( eWLAN_PAL_TRACE_LEVEL_NONE == level ) ||
        ( level >= eWLAN_PAL_TRACE_LEVEL_COUNT ))
   {
      traceOn = eWLAN_PAL_FALSE;
   }
   else
   {
      traceOn = ( level & gTraceInfo[ module ].moduleTraceLevel ) ? eWLAN_PAL_TRUE : eWLAN_PAL_FALSE;
   }

   return( traceOn );
}

void wpalTraceDisplay(void)
{
   wpt_moduleid moduleId;

   printk(KERN_CRIT
          "     1)FATAL  2)ERROR  3)WARN  4)INFO  "
          "5)INFO_H  6)INFO_M  7)INFO_L\n"); 
   for (moduleId = 0; moduleId < eWLAN_MODULE_COUNT; ++moduleId)
   {
      printk(KERN_CRIT
             "%2d)%s    %s        %s       %s       "
             "%s        %s         %s         %s\n",
             (int)moduleId,
             gTraceInfo[moduleId].moduleNameStr,
             (gTraceInfo[moduleId].moduleTraceLevel &
              (1 << eWLAN_PAL_TRACE_LEVEL_FATAL)) ? "X":" ",
             (gTraceInfo[moduleId].moduleTraceLevel &
              (1 << eWLAN_PAL_TRACE_LEVEL_ERROR)) ? "X":" ",
             (gTraceInfo[moduleId].moduleTraceLevel &
              (1 << eWLAN_PAL_TRACE_LEVEL_WARN)) ? "X":" ",
             (gTraceInfo[moduleId].moduleTraceLevel &
              (1 << eWLAN_PAL_TRACE_LEVEL_INFO)) ? "X":" ",
             (gTraceInfo[moduleId].moduleTraceLevel &
              (1 << eWLAN_PAL_TRACE_LEVEL_INFO_HIGH)) ? "X":" ",
             (gTraceInfo[moduleId].moduleTraceLevel &
              (1 << eWLAN_PAL_TRACE_LEVEL_INFO_MED)) ? "X":" ",
             (gTraceInfo[moduleId].moduleTraceLevel &
              (1 << eWLAN_PAL_TRACE_LEVEL_INFO_LOW)) ? "X":" "
         );
   }

}

/*                                                                            

                                                       

                                                                        

                                                                   
                                                                          

                                                              
                                                                         
                                                                      
                             

                                                                           
                                                                         
                                                      

                  

     

                                                                            */
void wpalTrace( wpt_moduleid module, wpt_tracelevel level, char *strFormat, ... )
{
   wpt_uint8 strBuffer[ WPAL_TRACE_BUFFER_SIZE ];
   int n;

   //                                                                        
   //                   
   if ( gTraceInfo[ module ].moduleTraceLevel & WPAL_TRACE_LEVEL_TO_MODULE_BITMASK( level ) )
   {
      va_list val;
      va_start(val, strFormat);

      //                                                  
      n = snprintf(strBuffer, WPAL_TRACE_BUFFER_SIZE, "wlan: [%d:%2s:%3s] ",
                   in_interrupt() ? 0 : current->pid,
                   (char *) TRACE_LEVEL_STR[ level ],
                   (char *) gTraceInfo[ module ].moduleNameStr);


      //                                                         
      //                                              
      if ((n >= 0) && (n < WPAL_TRACE_BUFFER_SIZE))
      {
         vsnprintf(strBuffer + n, WPAL_TRACE_BUFFER_SIZE - n - 1, strFormat, val);
         wpalOutput(level, strBuffer);
      }
      va_end(val);
   }
}

/*                                                                             
  
                                                      
   
                                                                    
                                                                    
                    
  
                                                                  
                                                                           
         
                                                              
                                                                                
                                                                       
                                    
         
                                                               

                                                               
  
                   
    
                                                                            */
//                                     
#define BYTES_PER_LINE 16

//                                                                  
#define CHARS_PER_LINE ((BYTES_PER_LINE * 3) + 1)

void wpalDump( wpt_moduleid module, wpt_tracelevel level,
               wpt_uint8 *pMemory, wpt_uint32 length)
{
   char strBuffer[CHARS_PER_LINE];
   int n, num, offset;

   //                                                                
   //                   
   if ( gTraceInfo[ module ].moduleTraceLevel & WPAL_TRACE_LEVEL_TO_MODULE_BITMASK( level ) )
   {
      num = 0;
      offset = 0;
      while (length > 0)
      {
         n = snprintf(strBuffer + offset, CHARS_PER_LINE - offset - 1,
                      "%02X ", *pMemory);
         offset += n;
         num++;
         length--;
         pMemory++;
         if (BYTES_PER_LINE == num)
         {
            wpalOutput(level, strBuffer);
            num = 0;
            offset = 0;
         }
      }

      if (offset > 0)
      {
         //                     
         wpalOutput(level, strBuffer);
      }
   }
}
#endif //          
