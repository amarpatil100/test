#ifndef LINT
   static char rcsid[] = "$Header: /cctlshome/tls/cctls/cctls/bb/cellmafcom/RCS/cmg/src/ppcmg_AC.c,v 49.1.1.1 2016/12/06 05:04:52 cctls STL_2 $" ;
#endif
/*************************************************************\
* Name        : ppcmg_AC.c                                    *
*                                                             *
* Purpose     : Common generic Interface between application  *
*               and Audit and Control tables.                 *
*                                                             *
* Description : This routine performs the following functions:*
*                                                             *
* Called by   :                                               *
*                                                             *
* Calls       :                                               *
*                                                             *
* Supervisor  : Michael Gwirzman                              *
*                                                             *
* Written By  : Evgeny Wolfson                                *
*                                                             *
* Date        : 30/03/94                                      *
*                                                             *
* Change History                                              *
* Num     Date    By           Description                    *
* ---   --/--/--  ----------   ------------------------------ *
* $Log: ppcmg_AC.c,v $
* Revision 49.1.1.1  2016/12/06 05:04:52  cctls
* created by vercopy from revision 48.1.1.3
*
* Revision 49.1  2016/12/06 05:04:52  cctls
* Initial revision
*
* Revision 48.1.1.3  2016/10/24 10:05:30  jyotiha
* S2L-RBMS5
*
* Revision 48.1.1.2  2016/05/25 12:24:13  cctls
* Updating File on Linux
*
 * Revision 48.1.1.1  2016/05/09  11:11:44  ccts
 * created by vercopy from revision 47.1.1.1
 *
 * Revision 48.1  2016/05/09  11:11:44  ccts
 * Initial revision
 *
 * Revision 47.1.1.1  2016/01/19  05:54:42  ccts
 * created by vercopy from revision 46.1.1.1
 *
 * Revision 47.1  2016/01/19  05:54:42  ccts
 * Initial revision
 *
 * Revision 46.1.1.1  2015/11/24  06:57:21  ccts
 * created by vercopy from revision 45.1.1.1
 *
 * Revision 46.1  2015/11/24  06:57:21  ccts
 * Initial revision
 *
 * Revision 45.1.1.1  2015/08/17  16:58:36  ccts
 * created by vercopy from revision 44.1.1.1
 *
 * Revision 45.1  2015/08/17  16:58:36  ccts
 * Initial revision
 *
 * Revision 44.1.1.1  2015/03/11  14:10:06  ccts
 * created by vercopy from revision 43.1.1.1
 *
 * Revision 44.1  2015/03/11  14:10:06  ccts
 * Initial revision
 *
 * Revision 43.1.1.1  2014/12/10  10:58:14  ccts
 * created by vercopy from revision 42.1.1.1
 *
 * Revision 43.1  2014/12/10  10:58:14  ccts
 * Initial revision
 *
 * Revision 42.1.1.1  2014/07/18  11:48:01  ccts
 * created by vercopy from revision 41.1.1.1
 *
 * Revision 42.1  2014/07/18  11:48:00  ccts
 * Initial revision
 *
 * Revision 41.1.1.1  2014/02/17  04:41:47  ccts
 * created by vercopy from revision 40.1.1.1
 *
 * Revision 41.1  2014/02/17  04:41:47  ccts
 * Initial revision
 *
 * Revision 40.1.1.1  2013/10/01  12:06:59  ccts
 * created by vercopy from revision 39.1.1.1
 *
 * Revision 40.1  2013/10/01  12:06:59  ccts
 * Initial revision
 *
 * Revision 39.1.1.1  2013/07/30  07:32:51  ccts
 * created by vercopy from revision 38.1.1.1
 *
 * Revision 39.1  2013/07/30  07:32:51  ccts
 * Initial revision
 *
 * Revision 38.1.1.1  2013/05/07  14:13:39  ccts
 * created by vercopy from revision 37.1.1.1
 *
 * Revision 38.1  2013/05/07  14:13:39  ccts
 * Initial revision
 *
 * Revision 37.1.1.1  2013/01/31  16:37:49  ccts
 * created by vercopy from revision 35.1.1.1
 *
 * Revision 37.1  2013/01/31  16:37:49  ccts
 * Initial revision
 *
 * Revision 35.1.1.1  2012/08/14  18:24:04  ccts
 * created by vercopy from revision 34.1.1.1
 *
 * Revision 35.1  2012/08/14  18:24:04  ccts
 * Initial revision
 *
 * Revision 34.1.1.1  2012/04/25  16:25:50  ccts
 * created by vercopy from revision 33.1.1.1
 *
 * Revision 34.1  2012/04/25  16:25:50  ccts
 * Initial revision
 *
 * Revision 33.1.1.1  2011/12/01  17:32:16  ccts
 * created by vercopy from revision 32.1.1.1
 *
 * Revision 33.1  2011/12/01  17:32:16  ccts
 * Initial revision
 *
 * Revision 32.1.1.1  2011/08/03  15:58:42  ccts
 * created by vercopy from revision 31.1.1.1
 *
 * Revision 32.1  2011/08/03  15:58:42  ccts
 * Initial revision
 *
 * Revision 31.1.1.1  2010/12/08  18:48:08  ccts
 * created by vercopy from revision 30.1.1.1
 *
 * Revision 31.1  2010/12/08  18:48:08  ccts
 * Initial revision
 *
 * Revision 30.1.1.1  2010/09/08  17:53:31  ccts
 * created by vercopy from revision 29.1.1.1
 *
 * Revision 30.1  2010/09/08  17:53:31  ccts
 * Initial revision
 *
 * Revision 29.1.1.1  2010/06/03  14:34:48  ccts
 * created by vercopy from revision 28.1.1.1
 *
 * Revision 29.1  2010/06/03  14:34:48  ccts
 * Initial revision
 *
 * Revision 28.1.1.1  2010/03/10  17:57:14  ccts
 * created by vercopy from revision 27.1.1.1
 *
 * Revision 28.1  2010/03/10  17:57:14  ccts
 * Initial revision
 *
 * Revision 27.1.1.1  2009/11/05  14:53:55  ccts
 * created by vercopy from revision 26.1.1.1
 *
 * Revision 27.1  2009/11/05  14:53:55  ccts
 * Initial revision
 *
 * Revision 26.1.1.1  2009/04/29  14:29:42  ccts
 * created by vercopy from revision 25.1.1.1
 *
 * Revision 26.1  2009/04/29  14:29:42  ccts
 * Initial revision
 *
 * Revision 25.1.1.1  2009/01/27  16:38:06  ccts
 * created by vercopy from revision 24.1.1.1
 *
 * Revision 25.1  2009/01/27  16:38:06  ccts
 * Initial revision
 *
 * Revision 24.1.1.1  2008/11/07  15:07:53  ccts
 * created by vercopy from revision 23.1.1.1
 *
 * Revision 24.1  2008/11/07  15:07:53  ccts
 * Initial revision
 *
 * Revision 23.1.1.1  2008/09/17  18:25:54  ccts
 * created by vercopy from revision 22.1.1.1
 *
 * Revision 23.1  2008/09/17  18:25:54  ccts
 * Initial revision
 *
 * Revision 22.1.1.1  2008/06/30  17:33:26  ccts
 * created by vercopy from revision 21.1.1.1
 *
 * Revision 22.1  2008/06/30  17:33:26  ccts
 * Initial revision
 *
 * Revision 21.1.1.1  2008/06/04  20:20:19  ccts
 * created by vercopy from revision 20.1.1.1
 *
 * Revision 21.1  2008/06/04  20:20:19  ccts
 * Initial revision
 *
 * Revision 20.1.1.1  2008/01/10  22:23:40  ccts
 * created by vercopy from revision 19.1.1.1
 *
 * Revision 20.1  2008/01/10  22:23:40  ccts
 * Initial revision
 *
 * Revision 19.1.1.1  2007/10/10  14:52:33  ccts
 * created by vercopy from revision 18.1.1.1
 *
 * Revision 19.1  2007/10/10  14:52:33  ccts
 * Initial revision
 *
 * Revision 18.1.1.1  2007/07/18  14:47:15  ccts
 * created by vercopy from revision 17.1.1.1
 *
 * Revision 18.1  2007/07/18  14:47:15  ccts
 * Initial revision
 *
 * Revision 17.1.1.1  2007/02/06  16:25:21  ccts
 * created by vercopy from revision 16.1.1.1
 *
 * Revision 17.1  2007/02/06  16:25:21  ccts
 * Initial revision
 *
 * Revision 16.1.1.1  2006/12/11  19:29:25  ccts
 * created by vercopy from revision 15.1.1.1
 *
 * Revision 16.1  2006/12/11  19:29:25  ccts
 * Initial revision
 *
 * Revision 15.1.1.1  2006/10/24  17:18:25  ccts
 * created by vercopy from revision 13.1.1.1
 *
 * Revision 15.1  2006/10/24  17:18:25  ccts
 * Initial revision
 *
 * Revision 13.1.1.1  2006/07/12  18:51:51  ccts
 * created by vercopy from revision 9.1.1.1
 *
 * Revision 13.1  2006/07/12  18:51:51  ccts
 * Initial revision
 *
 * Revision 9.1.1.1  2006/02/28  14:55:24  ccts
 * created by vercopy from revision 7.1.1.1
 *
 * Revision 9.1  2006/02/28  14:55:24  ccts
 * Initial revision
 *
 * Revision 7.1.1.1  2005/09/22  17:27:48  ccts
 * created by vercopy from revision 6.1.1.1
 *
 * Revision 7.1  2005/09/22  17:27:48  ccts
 * Initial revision
 *
 * Revision 6.1.1.1  2005/04/11  18:05:25  ccts
 * created by vercopy from revision 5.1.1.1
 *
 * Revision 6.1  2005/04/11  18:05:25  ccts
 * Initial revision
 *
 * Revision 5.1.1.1  2004/12/13  19:07:52  ccts
 * created by vercopy from revision 4.1.1.1
 *
 * Revision 5.1  2004/12/13  19:07:52  ccts
 * Initial revision
 *
 * Revision 4.1.1.1  2004/08/09  20:08:55  ccts
 * created by vercopy from revision 3.1.1.1
 *
 * Revision 4.1  2004/08/09  20:08:55  ccts
 * Initial revision
 *
 * Revision 3.1.1.1  2004/05/13  22:30:47  ccts
 * created by vercopy from revision 2.1.1.1
 *
 * Revision 3.1  2004/05/13  22:30:47  ccts
 * Initial revision
 *
 * Revision 2.1.1.1  2002/03/18  22:08:33  ccts
 * initial revision
 *
 * Revision 2.1  2002/03/18  22:08:33  ccts
 * Initial revision
 *
 * Revision 1.1.1.1  2002/03/18  22:08:32  ccts
 *  Initial CC revision
 *
 * Revision 1.1  2002/03/18  22:08:32  ccts
 * Initial revision
 *
 * Revision 6.1.1.1  1997/12/15  10:22:56  ccmngr
 * created by vercopy from revision 3.1.1.3
 *
 * Revision 6.1  1997/12/15  10:22:56  ccmngr
 * Initial revision
 *
 * Revision 3.1.1.3  1997/08/28  14:00:28  eyal
 * *** empty log message ***
 *
 * Revision 3.1.1.2  1997/08/28  12:24:01  eyal
 * add media to cre out
 *
 * Revision 3.1.1.1  1997/03/02  15:02:25  ccmngr
 * A new version v07_0 created from v01_0
 *
 * Revision 3.1  1997/03/02  15:02:25  ccmngr
 * Initial major revision
 *
 * Revision 1.1.1.1  1996/11/05  14:21:18  ccmngr
 * Initial CC revision
 *
 * Revision 1.1  1996/11/05  14:21:18  ccmngr
 * Initial revision
 *
 * Revision 1.1.1.1  1996/11/03  13:26:35  ccmngr
 * Initial CC revision
 *
 * Revision 1.1  1996/11/03  13:26:35  ccmngr
 * Initial revision
 *
 * Revision 1.1  1996/10/31  18:09:47  ccmngr
 * Initial revision
 *
 * Revision 21.1.1.1  1996/10/17  14:36:56  ccsbms
 * A new version v1A_0 created from v19_0
 *
 * Revision 21.1  1996/10/17  14:36:55  ccsbms
 * Initial major revision
 *
 * Revision 18.1.1.1  1996/06/17  12:42:19  ccmngr
 * A new version v19_0 created from v18_0
 *
 * Revision 18.1  1996/06/17  12:42:18  ccmngr
 * Initial major revision
 *
 * Revision 16.1.1.1  1996/02/22  15:49:27  ccmngr
 * A new version v18_0 created from v17_0
 *
 * Revision 16.1  1996/02/22  15:49:26  ccmngr
 * Initial major revision
 *
 * Revision 15.1.1.1  1996/01/30  11:58:53  ccmngr
 * A new version v17_0 created from v16_0
 *
 * Revision 15.1  1996/01/30  11:58:51  ccmngr
 * Initial major revision
 *
 * Revision 13.1.1.1  1995/11/27  00:14:47  ccmngr
 * A new version v16_0 created from v15_0
 *
 * Revision 13.1  1995/11/27  00:14:46  ccmngr
 * Initial major revision
 *
 * Revision 10.1.1.2  1995/09/21  11:27:55  daria
 * Media processing.
 *
 * Revision 10.1.1.1  1995/09/05  03:40:58  ccmngr
 * A new version v15_0 created from v14_0
 *
 * Revision 10.1  1995/09/05  03:40:56  ccmngr
 * Initial major revision
 *
 * Revision 9.1.1.1  1995/08/25  08:33:06  ccmngr
 * A new version v14_0 created from old_140
 *
 * Revision 9.1  1995/08/25  08:33:04  ccmngr
 * Initial major revision
 *
 * Revision 7.1.1.1  1995/07/20  12:56:12  ccmngr
 * A new version v14_0 created from v13_0
 *
 * Revision 7.1  1995/07/20  12:56:11  ccmngr
 * Initial major revision
 *
 * Revision 6.1.1.1  1995/06/28  19:26:38  ccmngr
 * A new version v13_0 created from v12_0
 *
 * Revision 6.1  1995/06/28  19:26:37  ccmngr
 * Initial major revision
 *
 * Revision 5.1.1.1  1995/05/21  16:42:07  ccmngr
 * A new version v12_0 created from v11_2
 *
 * Revision 5.1  1995/05/21  16:42:05  ccmngr
 * Initial major revision
 *
 * Revision 4.1.1.1  1995/05/02  13:08:16  ccmngr
 * A new version v11_2 created from v05
 *
 * Revision 4.1  1995/05/02  13:08:12  ccmngr
 * Initial major revision
 *
 * Revision 3.1.1.8  1995/04/24  16:28:04  guyl
 * *** empty log message ***
 *
 * Revision 3.1.1.7  1995/04/18  08:45:34  genadi
 * *** empty log message ***
 *
 * Revision 3.1.1.6  1995/04/11  08:14:37  genadi
 * *** empty log message ***
 *
 * Revision 3.1.1.5  1995/04/09  08:14:40  itzik
 * Bug fixed.
 *
 * Revision 3.1.1.4  1995/04/06  12:21:54  itzik
 * Cosmetics.
 *
 * Revision 3.1.1.3  1995/04/05  17:16:47  itzik
 * Bug fixed.\
 *
 * Revision 3.1.1.2  1995/04/02  13:12:46  genadi
 * *** empty log message ***
 *
 * Revision 3.1.1.1  1995/03/06  11:16:39  ccupp
 * A new version v05 created from v04
 *
 * Revision 3.1  1995/03/06  11:16:38  ccupp
 * Initial major revision
 *
 * Revision 2.1.1.2  1995/02/19  14:51:11  itzik
 * Type of ACFlag changed to int.
 *
 * Revision 2.1.1.1  1995/02/09  19:25:25  ccupp
 * A new version v04 created from v03
 *
 * Revision 2.1  1995/02/09  19:25:24  ccupp
 * Initial major revision
 *
 * Revision 1.13  1995/02/07  09:58:12  nira
 * Skipping DUMMY CREATE_OUTPUT message when writing to Flat file
 *
 * Revision 1.12  1995/01/23  13:40:11  itzik
 * Bug fixed.
 *
 * Revision 1.11  1995/01/10  18:24:22  daria
 * getenv for ACFlag was changed to be called only once.
 *
 * Revision 1.10  1995/01/10  17:45:49  daria
 * *** empty log message ***
 *
 * Revision 1.9  1995/01/10  10:41:00  daria
 * *** empty log message ***
 *
 * Revision 1.8  1995/01/09  18:22:14  daria
 * for ACFlag other than N call to ppcmg_AC_realAC.
 *
 * Revision 1.7  1994/12/15  15:52:36  ccupp
 * A new version: v03
 *
 * Revision 1.6  1994/11/15  09:40:59  evgeny
 * CREATE_OUTPUT printing to "mdfiles" corrected
 *
 * Revision 1.5  1994/11/07  18:12:30  evgeny
 * Dummy A&C printing to output file was corrected
 *
 * Revision 1.4  1994/10/31  12:30:00  evgeny
 * The END_INPUT verb changed yo END_INPUT_UPDATE and END_INPUT_BALANCE
 *
 * Revision 1.3  1994/10/23  19:18:15  ccupp
 * A new version: v02
 *
 * Revision 1.2  1994/10/13  12:48:57  daria
 * Porting to CC SBMS.
 *
*   1    4/05/94  Inna S.      converting ppcmg_ACgetVerbName *
*                              and ppcmg_ACgetStatus to public*
*                              functions.                     *
* ----------------------------------------------------------- *
\*************************************************************/
/* Std library *\
\* ----------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Include Libraries *\
\* ----------------- */


#define EXTERN extern
#include <ppreg_MEM_pub.h> /**/     /* Malloc function                  */
#include <ppcmc_pub.h> /**/         /* General information definitions  */
#include <ppreg_LIST_pub.h> /**/    /* Linked List  definitions         */
#include <ppcmg_MSGdef_pub.h> /**/  /* Display Message definitions      */
#include <ppcmg_IO_pub.h> /**/      /* IO definitions                   */
#include <ppcmg_AC_pub.h> /**/   
#include <ppcmc_AC_pub.h> /**/   
#undef EXTERN

#define EXTERN 
#include <ppcmg_ACstruct_pub.h> /**/ 
#include <ppcmg_AC_int.h> /**/ 
#include <ppcmc_AC_int.h> /**/   
#undef EXTERN

/* internal routines *\   
\* ----------------- */
static char *s_formatToType ( ppcmc_AC_typeAndFormat *Table_p , char *Type_p ); 
static int ppcmg_AC_realAC ( int ); 
static int ppcmg_AC_dummyAC ( int ); 
static int s_convertAcRc_applRc( int , int ) ; 
static int s_convertCustAcRc_applRc( int , int ) ; 

/* Function ppcmg_AC   *\
\* ------------------- */
/*************************************************************\
*                                                             *
* Name             : ppcmg_AC                                 *
*                                                             *
* Purpose          :                                          *
*                                                             *
*                                                             *
* Parameters       :                                          *
*                                                             *
* Returned Value   :                                          *
*                                                             *
* Return Points    :                                          *
* ----------------------------------------------------------- *
\*************************************************************/
#define __FN__ "ppcmg_AC"
int ppcmg_AC(int verb)
{

/* Local variables *\
\* --------------- */
    int          rc = 0;
    char        *ACFlag_value;
    static int   ACFlag = -1;

/* Code *\
\* ---- */

    /* First time read the environment */
    if (ACFlag == -1) {
        ACFlag = 1;
        if (NULL != (ACFlag_value = getenv("AC_FLAG")) &&
            (*ACFlag_value == 'N' || *ACFlag_value == 'n'))
        {
            ACFlag = 0;
        }
    }

    ppcmc_AC_print(1, verb);

    if (ACFlag) {
        rc = ppcmg_AC_realAC(verb);
    }
    else {
        rc = ppcmg_AC_dummyAC(verb);
    }

    ppcmc_AC_print(0, verb);

    return s_convertAcRc_applRc(rc, verb);
}    
#undef __FN__ 

/* Function ppcmg_AC_real_AC *\
\* ---------------------------- */
/*************************************************************\
*                                                             *
* Name             : ppcmg_AC_realAC                          *
*                                                             *
* Purpose          : Gets input file name                     *
*                    from colfiles file.                      *
*                                                             *
* Description      :                                          *
*                                                             *
* Parameters       :                                          *
*                                                             *
* Returned Value   :                                          *
*                                                             *
* Return Points    :                                          *
* ----------------------------------------------------------- *
\*************************************************************/
void AcMain(ppcmg_AC_data *);
void AcCommit(int *i_flag, int *o_rc);
#define __FN__ "ppcmg_AC_real_AC"
static int ppcmg_AC_realAC(int i_verb)
{
/* Local variables *\
\* --------------- */
    int  rc = 0 ;
    int  commitFlag = 0 ;
    int i ;  

/* Code *\
\* ---- */
    ppcmg_ACarea.status = 0 ;
    ppcmg_ACarea.verb = i_verb ;
/* Call to Audit and Control */
    AcMain( &ppcmg_ACarea ) ;
/* Commit TABLE updates */
    if ( ppcmg_ACarea.status  !=  PPCMG_AC_ST_NORMAL_NO_COMMIT ) {
        commitFlag = ppcmc_AC_decideCommit(ppcmg_ACarea.status);
        if ( commitFlag != PPCMG_AC_SKIP_COMMIT ) {
            AcCommit( &commitFlag, &rc ) ;
            if ( rc > ppcmg_ACarea.status )
                ppcmg_ACarea.status = rc ;
        }
    }

  return ( ppcmg_ACarea.status ) ;
}
#undef __FN__

/* Function ppcmg_AC_dummyAC *\
\* ------------------------- */
/*************************************************************\
*                                                             *
* Name             : s_AC_dummyAC                             *
*                                                             *
* Purpose          : Gets input file name                     *
*                    from colfiles file.                      *
*                                                             *
* Description      :                                          *
*                                                             *
* Parameters       :                                          *
*                                                             *
* Returned Value   :                                          *
*                                                             *
* Return Points    :                                          *
* ----------------------------------------------------------- *
\*************************************************************/

#define __FN__ "ppcmg_dummy_AC"
static int ppcmg_AC_dummyAC(int  i_verb)
{

/* Local variables *\
\* --------------- */
    static FILE       *fp_in;
    static FILE       *fp_out;   
    int                rc;
    char               *s;   
    struct help_st {
        char filePath[PPCMC_AC_FILE_PATH_LEN + 1] ;
        char fileName[PPCMG_AC_FILE_NAME_LEN + 1] ;
        char fileFormat[PPCMG_AC_FILE_FORMAT_LEN + 1] ; 
        char fileType[PPCMG_AC_FILE_TYPE_LEN + 1] ;   
        char recordsNumber[PPCMG_AC_RECORD_NUM_LEN + 1] ;   
        char groupName[PPCMC_AC_GROUP_NAME_LEN + 1];
        char balanceDate[PPCMC_AC_BALANCE_DATE_LEN + 1];
    } help;
    static char         procAccBuf[BUFFER_LEN];
    char                buffer[BUFFER_LEN];
    int filePath_length ; 

/* Code *\
\* ---- */

  switch (i_verb) {

      case PPCMG_AC_DEFINE_LOGICAL :
      case PPCMG_AC_END_LOGICAL :
          ppcmg_ACarea.status = PPCMG_AC_ST_NORMAL_MIN;
          break;
      case PPCMG_AC_CREATE_OUTPUT :
          sprintf( buffer,
                   "%.*s %.*s %.*s %.*s %c %d %.*s %.*s\n",
                   PPCMG_AC_FILE_TYPE_LEN,
                   PPCMG_AC_CRT_OUT.i_fileType,
                   PPCMC_AC_FILE_PATH_LEN,
                   PPCMG_AC_CRT_OUT.CustIn.i_filePath,
                   PPCMG_AC_FILE_NAME_LEN,
                   PPCMG_AC_CRT_OUT.i_fileName,
                   PPCMG_AC_FILE_FORMAT_LEN,
                   s_formatToType( ppcmc_AC_TypeAndFormatTable,
                                   PPCMG_AC_CRT_OUT.i_fileType ),
                   (PPCMG_AC_CRT_OUT.i_media == ' ' ? '-' : PPCMG_AC_CRT_OUT.i_media),
                   PPCMG_AC_CRT_OUT.i_recordsQuant,
                   PPCMC_AC_GROUP_NAME_LEN,
                   strncmp(PPCMG_AC_CRT_OUT.CustIn.i_groupName," ", 1)? 
                           PPCMG_AC_CRT_OUT.CustIn.i_groupName : "-",
                   PPCMC_AC_BALANCE_DATE_LEN,
                   strncmp(PPCMG_AC_CRT_OUT.CustIn.i_balanceDate," ", 1)? 
                           PPCMG_AC_CRT_OUT.CustIn.i_balanceDate : "-");
          if ( fp_out &&
	       memcmp(PPCMG_DUMMY_FILE_TYPE, PPCMG_AC_CRT_OUT.i_fileType,
		      strlen(PPCMG_DUMMY_FILE_TYPE)) ) {
              fputs(buffer, fp_out);
          }
          else {
              strcpy(procAccBuf, buffer);
          }
          ppcmg_ACarea.status = PPCMG_AC_ST_NORMAL_MIN;
          break;

      case PPCMG_AC_GET_NEXT_APPEND :
	  ppcmg_ACarea.status = PPCMG_AC_ST_NORMAL_MIN;
      	  do
          {
              fgets(buffer, sizeof(buffer), fp_in);
              if (feof(fp_in)) {
	          rewind(fp_in);
	          ppcmg_ACarea.status = PPCMG_AC_ST_NO_MORE_FILES;
                  break;
              }
              if (*buffer == '*') {
                  continue;
              }

              memset(&help, 0, sizeof(help));
	      sscanf(buffer,
	             "%s%s%s%s%s%s%s",
	             help.fileType, 
	             help.filePath, 
	             help.fileName,
	             help.fileFormat,
	             help.recordsNumber,
                     help.groupName,
                     help.balanceDate) ;    
	      ppcmg_AC_clear(PPCMG_AC_GT_NXT_AP.CustOut.o_filePath);
	      ppcmg_AC_copy(PPCMG_AC_GT_NXT_AP.CustOut.o_filePath,
                            help.filePath); 
 	      ppcmg_AC_clear(PPCMG_AC_GT_NXT_AP.o_fileName); 
	      ppcmg_AC_copy(PPCMG_AC_GT_NXT_AP.o_fileName, help.fileName); 
	      ppcmg_AC_clear(PPCMG_AC_GT_NXT_AP.o_fileFormat); 
	      ppcmg_AC_copy(PPCMG_AC_GT_NXT_AP.o_fileFormat, help.fileFormat); 
	      ppcmg_AC_clear(PPCMG_AC_GT_NXT_AP.o_fileType); 
	      ppcmg_AC_copy(PPCMG_AC_GT_NXT_AP.o_fileType, help.fileType); 
	      PPCMG_AC_GT_NXT_AP.o_recordsQuant = atoi(help.recordsNumber);   
	      ppcmg_AC_clear(PPCMG_AC_GT_NXT_AP.CustOut.o_groupName);
              if (strncmp(help.groupName, "-", 1)) {
                  ppcmg_AC_copy(PPCMG_AC_GT_NXT_AP.CustOut.o_groupName,
                                help.groupName);
              }
              ppcmg_AC_clear(PPCMG_AC_GT_NXT_AP.CustOut.o_balanceDate);
              if (strncmp(help.balanceDate, "-", 1)) {
                  ppcmg_AC_copy(PPCMG_AC_GT_NXT_AP.CustOut.o_balanceDate,
                                help.balanceDate);
              }
          } while (memcmp(PPCMG_AC_GT_NXT_AP.i_fileType," ",1) &&
		   memcmp(PPCMG_AC_GT_NXT_AP.i_fileType,
		          PPCMG_AC_GT_NXT_AP.o_fileType,
		          PPCMG_AC_FILE_TYPE_LEN));
          break;

      case PPCMG_AC_GET_NEXT :
          if (!memcmp(PPCMG_AC_GT_NXT.i_fileType, PPCMG_DUMMY_FILE_TYPE,
                     strlen(PPCMG_DUMMY_FILE_TYPE)))
          {
              if (procAccBuf[0] == '\0') {
                  ppcmg_ACarea.status = PPCMG_AC_ST_NO_MORE_FILES;
              }
              else {
                  rc = s_GN_scan(procAccBuf);
                  if (rc != 0) {
                      ppcmg_ACarea.status = PPCMG_AC_ST_NO_MORE_FILES;
                  }
                  else {
                      procAccBuf[0] = '\0';
                      ppcmg_ACarea.status = PPCMG_AC_ST_NORMAL_MIN;
                  }
              }
              break; 
          }
          else {
	      ppcmg_ACarea.status = PPCMG_AC_ST_NORMAL_MIN;
      	      do
              {
                  fgets(buffer, sizeof(buffer), fp_in);
                  if (feof(fp_in)) {
	              rewind(fp_in);
	              ppcmg_ACarea.status = PPCMG_AC_ST_NO_MORE_FILES;
                      break;
                  }
                  if (*buffer == '*') {
                      continue;
                  }
                  rc = s_GN_scan(buffer);
              } while (rc);
          }
          break;

      case PPCMG_AC_END_INPUT_UPDATE :
      case PPCMG_AC_END_INPUT_BALANCE :
      case PPCMG_AC_DEFINE_PHYSICAL :
      case PPCMG_AC_END_PHYSICAL :
	   ppcmg_ACarea.status = PPCMG_AC_ST_NORMAL_MIN;
          break;

      case PPCMG_AC_START_PROGRAM :
	   ppcmg_ACarea.status = PPCMG_AC_ST_NORMAL_MIN;
              s = (char *) getenv("AC_OUTPUT");
              fp_out = fopen(s, "w");
              if (fp_out == NULL) {
		  ppcmg_MSG_display(PPCMG_MSG_AC_OUTFILE_OPEN_ERR,s) ;   
	          ppcmg_ACarea.status = PPCMG_AC_ST_FATAL_MIN;
              }

              s = (char *) getenv("AC_INPUT");
              fp_in = fopen(s, "r");
              if (fp_in == NULL) {
		  ppcmg_MSG_display(PPCMG_MSG_AC_INFILE_OPEN_ERR,s)  ; 
	          ppcmg_ACarea.status = PPCMG_AC_ST_FATAL_MIN;
              }
	      break;

      case PPCMG_AC_CHECK_INPUT_EXIST :     
          PPCMG_AC_CHK_INP.i_filesNumber = 1;
	  ppcmg_ACarea.status = PPCMG_AC_ST_NORMAL_MIN;
          break;

      case PPCMG_AC_END_PROGRAM :
          if (fp_out) { 
              fclose(fp_out);
          }
          if (fp_in) { 
              fclose(fp_in);
          }
	  ppcmg_ACarea.status = PPCMG_AC_ST_NORMAL_MIN;
	  break ;  
      case PPCMG_AC_NEW_PHYSICAL :
          if ( fp_out ) { 
	      /* Length calculation */     
	      if ( PPCMG_AC_NEW_PHY.i_filePath[PPCMG_AC_FILE_PATH_LEN-1] != ' ')
		  filePath_length = PPCMG_AC_FILE_PATH_LEN ;    
              else 
		  filePath_length = strchr(PPCMG_AC_NEW_PHY.i_filePath,' ') - 
                                                  PPCMG_AC_NEW_PHY.i_filePath ; 
              fprintf( fp_out, 
		       "%.*s %.*s%.*s %.*s 0 \n",
		       PPCMG_AC_FILE_TYPE_LEN,
		       PPCMG_AC_NEW_PHY.i_fileType, 
		       filePath_length, 
		       PPCMG_AC_NEW_PHY.i_filePath, 
                       PPCMG_AC_FILE_NAME_LEN,
		       PPCMG_AC_NEW_PHY.i_fileName,
                       PPCMG_AC_FILE_FORMAT_LEN,
                       s_formatToType( ppcmc_AC_TypeAndFormatTable,
                                       PPCMG_AC_NEW_PHY.i_fileType ));
          }  
          ppcmg_ACarea.status = PPCMG_AC_ST_NORMAL_MIN;
          break;

      default:
          ppcmg_MSG_display(PPCMG_MSG_AC_INVALID_VERB);     
	  ppcmg_ACarea.status = PPCMG_AC_ST_ERROR_MIN;
          break;
    }
    rc=ppcmg_ACarea.status;
    return rc;
}
#undef __FN__

/*************************************************************\
*                                                             *
* Name             : s_convertAcRc_applRc                     *
*                                                             *
* Purpose          : Checks return status from A&C and        *
*                    converts it to application status        *
*                                                             *   
*                                                             *
* Parameters       : int status                               *
*                    int verb                                 *
* Returned Value   : rc                                       *
*                                                             *
* Return Points    :                                          *
* ----------------------------------------------------------- *
\*************************************************************/
#define __FN__ "s_convertAcRc_applRc"
static int s_convertAcRc_applRc( int status, int verb )
{

/* Local variables *\
\* --------------- */
    int    rc ;
    char  *command_p , *status_p ;

 /* Code *\
 \* ---- */

 if ( status >= PPCMG_AC_ST_NORMAL_MIN  &&
      status <= PPCMG_AC_ST_WARNING_MAX    ) {
     rc = PPCMG_AC_NORMAL;
     if ( status == PPCMG_AC_ST_NO_MORE_FILES )
         rc = PPCMG_AC_NORMAL_NO_MORE_FILES;
 } else {
      if ( status >= PPCMG_AC_ST_ERROR_MIN  &&
           status <= PPCMG_AC_ST_ERROR_MAX     ) 
          rc = PPCMG_AC_ABORT_FILE;
      else 
          rc = s_convertCustAcRc_applRc( status , verb ) ; 
 }
 if ( status >= PPCMG_AC_ST_WARNING_MIN  &&
      status <= PPCMG_AC_ST_FATAL_MAX        ) {
     command_p = ppcmg_ACgetVerbName( verb ) ;
     status_p = ppcmg_ACgetStatusName( status ) ;
     ppcmg_MSG_display( PPCMG_MSG_AC_WARNING , command_p , status_p ) ;
 }
 return (rc) ;
}   
#undef __FN__ 

/*************************************************************\
*                                                             *
* Name             : s_convertCustAcRc_applRc                 *
*                                                             *
* Purpose          : Checks return status from A&C for        *
*                    customer status and converts it to       *
*                    application status                       *
*                    the customer defines for MIN/MAX values  *   
*                    are in ppcmc_AC_pub.h                    *   
*                    each customer can define its own ranges  *   
*                    but must declare both end of each range  *   
*                    (MIN + MAX)                              *   
*                                                             *
* Parameters       : int status                               *
*                    int verb                                 *
*                                                             *
* Returned Value   : rc                                       *
*                                                             *
* Return Points    :                                          *
* ----------------------------------------------------------- *
\*************************************************************/
#define __FN__ "s_convertCustAcRc_applRc"
static int s_convertCustAcRc_applRc( int status, int verb )
{

/* Local variables *\
\* --------------- */
    int    rc , displayMsg = 0 ;
    char  *command_p , *status_p ;

 /* Code *\
 \* ---- */

 rc = PPCMG_AC_ABORT_EXECUTION;

#if (defined (PPCMC_AC_ST_NORMAL_MIN) && defined(PPCMC_AC_ST_NORMAL_MAX) )
 if ( status >= PPCMC_AC_ST_NORMAL_MIN  &&
      status <= PPCMC_AC_ST_NORMAL_MAX     )
     rc = PPCMG_AC_NORMAL;
#endif

#if (defined (PPCMC_AC_ST_WARNING_MIN) && defined(PPCMC_AC_ST_WARNING_MAX) )
 if ( status >= PPCMC_AC_ST_WARNING_MIN  &&
      status <= PPCMC_AC_ST_WARNING_MAX     ) {
     rc = PPCMG_AC_NORMAL;
     displayMsg = 1;
 }
#endif

#if (defined (PPCMC_AC_ST_ERROR_MIN) && defined(PPCMC_AC_ST_ERROR_MAX) )
 if ( status >= PPCMC_AC_ST_ERROR_MIN  &&
      status <= PPCMC_AC_ST_ERROR_MAX     ) {
     rc = PPCMG_AC_ABORT_FILE;
     displayMsg = 1;
 }
#endif

#if (defined (PPCMC_AC_ST_FATAL_MIN) && defined(PPCMC_AC_ST_FATAL_MAX) )
 if ( status >= PPCMC_AC_ST_FATAL_MIN  &&
      status <= PPCMC_AC_ST_FATAL_MAX     ) {
     rc = PPCMG_AC_ABORT_EXECUTION;
     displayMsg = 1;
 }
#endif

 if ( displayMsg ) {
     command_p = ppcmg_ACgetVerbName( verb ) ;
     status_p = ppcmg_ACgetStatusName( status ) ;
     ppcmg_MSG_display( PPCMG_MSG_AC_WARNING , command_p , status_p ) ;
 } 
 return (rc) ;
}   

#undef __FN__ 
/* Function ppcmg_ACgetVerbName *\
\* -----------------------------*/
/*************************************************************\
*                                                             *
* Name             : ppcmg_ACgetVerbName                      *
*                                                             *
* Purpose          :                                          *
*                                                             *
*                                                             *
* Parameters       :                                          *
*                                                             *
* Returned Value   :                                          *
*                                                             *
* Return Points    :                                          *
* ----------------------------------------------------------- *
\*************************************************************/

#define __FN__ "ppcmg_ACgetVerbName"
char *ppcmg_ACgetVerbName ( int i_verb ) {

/* Local variables *\
\* --------------- */
static  char  verbName[50];

/* Code *\
\* ---- */

  strcpy(verbName, "") ;
	 
  if ( i_verb <= ARRAY_MAX_INDEX(ppcmg_AC_verbName) )
     strcpy(verbName, ppcmg_AC_verbName[i_verb]);
  
  #ifdef  PPCMC_AC_VERBNAME
  if ( !strcmp(verbName, "")  &&
       i_verb <= ARRAY_MAX_INDEX(ppcmc_AC_verbName) )
     strcpy(verbName, ppcmc_AC_verbName[i_verb]);
  #endif
  
  if ( !strcmp(verbName, "") )  
     strcpy(verbName, "A&C unknown verb");

  return (verbName);
  }
#undef __FN__

/* Function ppcmg_ACgetStatusName *\
\* ------------------------------ */
/*************************************************************\
*                                                             *
* Name             : ppcmg_ACgetStatusName                    *
*                                                             *
* Purpose          :                                          *
*                                                             *
*                                                             *
* Parameters       :                                          *
*                                                             *
* Returned Value   :                                          *
*                                                             *
* Return Points    :                                          *
* ----------------------------------------------------------- *
\*************************************************************/
#define __FN__ "ppcmg_ACgetStatusName"
char *ppcmg_ACgetStatusName ( int i_status ) {

/* Local variables *\
\* --------------- */
char tempStatusName [160];
static char statusName [160];

/* Code *\
\* ---- */

  strcpy(tempStatusName, "") ;
	 
  if ( i_status <= ARRAY_MAX_INDEX(ppcmg_AC_statusName) )
     strcpy(tempStatusName, ppcmg_AC_statusName[i_status]);
  
  #ifdef  PPCMC_AC_STATUSNAME
  if ( !strcmp(tempStatusName, "")  &&
       i_status <= ARRAY_MAX_INDEX(ppcmc_AC_statusName) )
     strcpy(tempStatusName, ppcmc_AC_statusName[i_status]);
  #endif
  
  if ( !strcmp(tempStatusName, "") )  
     strcpy(tempStatusName, "A&C unknown status");

  sprintf(statusName, "(%d) %s", i_status, tempStatusName) ;

  return (statusName);
}
#undef  __FN__

/* Function ppcmg_AC_formatToType *\
\* ------------------------------ */
/*************************************************************\
*                                                             *
* Name             : s_formatToType                           *
*                                                             *
* Purpose          : Finds the format according to the type . *
*                                                             *
*                                                             *
* Parameters       :                                          *
*                                                             *
* Returned Value   :                                          *
*                                                             *
* Return Points    :                                          *
* ----------------------------------------------------------- *
\*************************************************************/
#define __FN__ "s_formatToType"
static char *s_formatToType ( ppcmc_AC_typeAndFormat *Table_p , char *Type_p ) {

/* Local variables *\
\*-----------------*/

ppcmc_AC_typeAndFormat *p;

/*     Code        *\
\*-----------------*/

   p = Table_p;
   while ( p->fileType && memcmp(p->fileType,Type_p,strlen(p->fileType)) ) 
           p++;
   return p->fileFormat;
}

#undef __FN__

#define __FN__ "s_GN_scan"
int s_GN_scan(char *buffer) {

    struct help_st {
        char filePath[PPCMC_AC_FILE_PATH_LEN + 1] ;
        char fileName[PPCMG_AC_FILE_NAME_LEN + 1] ;
        char fileFormat[PPCMG_AC_FILE_FORMAT_LEN + 1] ;
        char fileType[PPCMG_AC_FILE_TYPE_LEN + 1] ;
        char fileMedia[1 + 1] ;
        char recordsNumber[PPCMG_AC_RECORD_NUM_LEN + 1] ;
    } help;

    memset(&help, 0, sizeof(help));

    sscanf(buffer,
           "%s%s%s%s%s%s",
           help.fileType,
           help.filePath,
           help.fileName,
           help.fileFormat,
           help.fileMedia,
           help.recordsNumber);
    ppcmg_AC_clear(PPCMG_AC_GT_NXT.CustOut.o_filePath);
    ppcmg_AC_copy(PPCMG_AC_GT_NXT.CustOut.o_filePath,help.filePath);
    ppcmg_AC_clear(PPCMG_AC_GT_NXT.o_fileName);
    ppcmg_AC_copy(PPCMG_AC_GT_NXT.o_fileName,help.fileName);
    ppcmg_AC_clear(PPCMG_AC_GT_NXT.o_fileFormat) ;
    ppcmg_AC_copy(PPCMG_AC_GT_NXT.o_fileFormat,help.fileFormat) ;
    ppcmg_AC_clear(PPCMG_AC_GT_NXT.o_fileType) ;
    ppcmg_AC_copy(PPCMG_AC_GT_NXT.o_fileType,help.fileType) ;
    PPCMG_AC_GT_NXT.o_recordsQuant = atoi(help.recordsNumber);
    PPCMG_AC_GT_NXT.o_media = *help.fileMedia;

    if (memcmp(PPCMG_AC_GT_NXT.i_fileType," ",1) &&
        memcmp(PPCMG_AC_GT_NXT.i_fileType, PPCMG_AC_GT_NXT.o_fileType,
               PPCMG_AC_FILE_TYPE_LEN) ||
        memcmp(PPCMG_AC_GT_NXT.i_fileName," ",1) &&
        memcmp(PPCMG_AC_GT_NXT.i_fileName, PPCMG_AC_GT_NXT.o_fileName,
               PPCMG_AC_FILE_NAME_LEN) ||
        memcmp(PPCMG_AC_GT_NXT.i_filePath," ",1) &&
        memcmp(PPCMG_AC_GT_NXT.i_filePath, PPCMG_AC_GT_NXT.o_filePath,
        PPCMC_AC_FILE_PATH_LEN)) 
    {
        return 1;
    }

    return 0;
}
