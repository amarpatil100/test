#ifndef LINT
   static char rcsid[] = "$Header: /cctlshome/tls/cctls/cctls/bb/cellmafcom/RCS/cmg/src/ppcmg_MSG.c,v 49.1.1.2 2017/02/22 08:34:43 patilpra STL_3 $" ;
#endif
/*************************************************************\
*                                                             *
* Entries          : ppcmg_MSG_display                        *
*                    ppcmg_MSG_vdisplay                       *
*                    ppcmg_MSG_get                            *
*                    ppcmg_MSG_vget                           *
*                                                             *
* Purpose          : display message to console and/or print  *
*                    it to log                                *
*                                                             *
* Parameters       : char *messageCode - key to message table *
*                      0 - default destination                *
*                    ... - variable number of parameters      *
*                                                             *
*                                                             *
* Returned Value   : pointer to message or NULL               *
*                                                             *
*                                                             *
*                                                             *
* Static Variables :                                          *
*                                                             *
* Flow             :                                          *
*                                                             *
* Return Points    :                                          *
*                                                             *
* Change History
*-----------------------------------------------------------------
* No. | By           | Date     | Description
*-----+--------------+----------+---------------------------------
*  #1 | Shannon W.   | 08/17/01 | Added #ifndef __STDC__ statements 
*     |              |          | to include va_start function (in 
*     |              |          | stdarg.h) for porting to SUN.
*-----+--------------+----------+---------------------------------
\*************************************************************/

/* Ch#U2L.STAT; stat/fstat/lstat functions populate the variables incorrectly with -align */
#ifdef __linux
	#pragma pack(push,4)
	#include <sys/stat.h>
	#pragma pack(pop)
#else
	#include <sys/stat.h>
#endif

#include <stdio.h>
#include <ctype.h>
#ifndef __STDC__
#define __STDC__ 1
#endif
#include <stdarg.h>
#include <string.h>

#define EXTERN extern
#include <pptbc_tableAccess_pub.h> /**/
#include <pptbg_MAF_MESSAGE_pub.h> /**/  /* Message table access functions  */
#include <ppcmg_MSG_pub.h> /**/  /* Message definitions             */

static maf_message_t entry ;

#define XOR ^

char * ppcmg_MSG_vget( char * messageCode, va_list args )
{
/* Local variables *\
\* --------------- */
  int    rc ;
  static char message [ 512 ] ;
  char format [ 200 ];
  int  msglen ;


/* Code *\
\* ---- */
    if ( messageCode != NULL ) {
        memcpy( &entry.key, messageCode, sizeof( entry.key ) ) ;
        if ( entry_in_MAF_MESSAGE( &entry, DIRECT_KEY ) == ENTRY_FOUND ) {

            /* Determine message format length  */
                for ( msglen = sizeof( entry.msg_desc ) ;
                      entry.msg_desc [ msglen - 1 ]
                      == ' ' ; msglen -- ) ;

            /* Build full message format */
                sprintf( format, "%s%c  %.*s", messageCode,
                         entry.msg_severity,
                         msglen,
                         entry.msg_desc ) ;

            /* Prepare message to return it     */
                vsprintf( message, format, args ) ;
                return ( message ) ;
        }
    }

    return ( NULL ) ;
}


char * ppcmg_MSG_get( char *messageCode, ... )
{
  char    *message_p ;
  va_list  args ;

    va_start ( args, messageCode ) ;
    message_p = ppcmg_MSG_vget( messageCode, args ) ;
    va_end( args ) ;
    return ( message_p ) ;
}


char * ppcmg_MSG_vdisplay(
                     char  *messageCode, /* message key           */
                     va_list args          /* variable arguments    */
                             )

{
  char  *message_p;
  int    indent, width, length, offset;
  static FILE  *consol;
  static int    same;
  int i;

   if (!consol) {
       struct stat buff;
       ino_t       in_out, in_err;
       
       fstat(1, &buff);
       in_out = buff.st_ino;
       fstat(2, &buff);
       in_err = buff.st_ino;
       same = (in_out == in_err);
       consol = fopen("/dev/tty", "w");
   }

   if ( messageCode == NULL )
       return  NULL ;
/* If message exists in message table */
   if ((message_p = ppcmg_MSG_vget(messageCode, args)) != NULL) {

        for ( indent = 0, width = 80, length = strlen( message_p ),
              offset = 0 ;
              offset < length ;
              offset += width, indent = 10, width = 70 )
        {

/* 2/27/97 Daria - New Line Care */
            for ( i = 1; i < width ; i ++ ) {
               if ( message_p[offset + i] == '\n' ) {
                   message_p[offset + i] == ' ';
                   width = i;
               }
            }
/* 2/27/97 Daria - End */


        /* Adjust width not to split in the middle of a word */
            if ( offset + width < length )
                while (
                   ( isalnum( message_p [ offset + width ] ) &&
                     isalnum( message_p [ offset +width-1 ] ) )||
                     ispunct( message_p [ offset +width ] ) )
                    width-- ;

        /* Send message to log if specified */
            if (entry.target_dev[0] == 'Y') {
                if (!(isatty(1) && entry.target_dev[1] == 'Y')) {
                    printf("%*s%.*s\n",
                           indent, "", width, message_p + offset);
                }
            }

        /* Send message to console if specified */
            if (entry.target_dev[1] == 'Y') {
                if (consol != NULL) {
                    fprintf(consol, "%*s%.*s\n",
                            indent, "", width, message_p + offset);
                }
            }

        /* Send message to error if specified */
            if (entry.target_dev[2] == 'Y') {
                if (!(isatty(2) && entry.target_dev[1] == 'Y') &&
                    !(same && entry.target_dev[0] == 'Y'))
                {
                    fprintf(stderr, "%*s%.*s\n",
                            indent, "", width, message_p + offset);
                }
            }

        }
    }
/* ELSE issue 'unknown message' */
    else
        printf( "Message %s not found in message table !\n",
                messageCode ) ;

  return ( message_p ) ;
}


char * ppcmg_MSG_display (
             char  *messageCode, /* message key           */
             ...                 /* variable arguments    */
             )
{
  va_list args ;
  char * rc ;

      va_start( args, messageCode) ;
      rc = ppcmg_MSG_vdisplay( messageCode, args ) ;
      va_end( args ) ;

  return (rc) ;
}
