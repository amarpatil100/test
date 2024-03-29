#ifndef LINT
   static char rcsid[] = "$Header: /cctlshome/tls/cctls/cctls/bb/cellmafcom/RCS/cmg/src/ppcmg_IO.c,v 49.1.1.2 2017/02/22 08:34:43 patilpra STL_7 $";
#endif
/*************************************************************\
*                                                             *
* Name        : ppcmg_IO                                      *
*                                                             *
* Purpose     : A general routine which performs all the IO   *
*               operations required by the Preprocessor (UPS2)*
*               of the Message Processing System (MPS).       *
*                                                             *
* Description : ppcmg_IO performs input/output from/to        *
*               external files.                               *
*               The routine handles files residing on external*
*               storage media, e.g.: tape, cartridge, disk.   *
*               The routine performs the following functions: *
*               - opens an existing input file                *
*               - opens a new output file                     *
*               - reads a record from an input file           *
*               - writes a record to an output file           *
*               - closes an input/output file                 *
*               - deletes an existing file                    *
*               The routine handles only sequential files,    *
*               which contain fixed/variable length records.  *
*               In case of a request (via special parameter)  *
*               to perform a dynamic allocation, the routine  *
*               does it, using the dynamic allocation         *
*               parameters from an external table.            *
*               Main routine ppcmg_IO calls:                  *
*               IO_open, IO_read, IO_write, IO_close,IO_delete*
*               ppcmg_MSG_get, ERROR_STACK_ADD.               *
*                                                             *
* Parameters  : Pointer to I/O parameters structure, which is *
*               described in file JH07131A.H.                 *
*                                                             *
* Returns     : Status of I/O commands execution.             *
*                                                             *
* Supervisor  : Orna Leventer                                 *
*                                                             *
* Written By  : Isaac Pailis                                  *
*                                                             *
* Date        : 21/01/93                                      *
*                                                             *
\*************************************************************/

/****************** Change History ***************************\
* Num Date      By         Description                        *
* --- --/--/--  ---------- ---------------------------------- *
* 001 08/10/08  Amit       CR332 - Holborn                    *
*               Kumar      Added support for new line         *
*                          terminated records                 *
* 002 04/12/18  Meghna     CCOMCKB-374- Archival of collection*
*               Jain       File will be moved to archive      *
*                       directory after succesfull processing *
\*************************************************************/

/* Std library *\
\* ----------- */
/* Ch#U2L.STAT; stat/fstat/lstat functions populate the variables incorrectly with -align */
#ifdef __linux
	#pragma pack(push,4)
	#include <sys/stat.h>
	#pragma pack(pop)
#else
	#include <sys/stat.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <dirent.h>

/* Include Libraries *\
\* ----------------- */
#undef  EXTERN
#define EXTERN extern
#include <pptbc_tableAccess_pub.h>
#include <ppreg_MEM_pub.h>         /* Memory allocation             */
#include <ppreg_STR_pub.h>         /* String functions              */
#include <ppcmg_STATUSrep_pub.h>   /* DisplayMessage definitions    */
#include <ppcmg_MSGdef_pub.h> 
#include <ppcmc_pub.h>             /* General information           */
#include <ppcmg_IO_pub.h>          /* I/O parameters structure      */
#include <pptbg_FILES_OPERATION_pub.h>   /* Operation table's definitions */
#include <ppreg_LAYOUTshell_pub.h> 
#include <ppreg_LAYOUTshell_int.h> 
#include <pprbc_WMdef_pub.h>
#include <pprbc_EXP_pub.h>
#include <pprbc_EXPvec_pub.h>
#include <ppcmg_MSGdef_pub.h>
#include <pptbg_pub.h> 
#include <ppreg_FUNC_pub.h> 
#include <pptbg_tableAccess_pub.h> 
#include <ppmdg_REC_pub.h>
#include <errno.h>
#include <funcsts.h>
#include <fullpathfn.h>
#undef EXTERN

#undef  EXTERN

#define EXTERN
#include <ppcmg_IO_int.h> /**/         /* Miscitems for tape/cartridge  */
#undef  EXTERN

/* Statics *\
\* ------- */

/* Prototypes */
static void  IO_fileParams(ppcmg_IO_FCB *);
static char *IO_fillFileParams(ppcmg_IO_FCB *, files_operation_t *);
static void  IO_checkPath(char *);
static void  s_IO_read_record(int, char *, int *);
static void  s_IO_write_record(int, char *, int);
static void  s_IO_read_block(int);
static void  s_IO_write_block(int);

static void  s_IO_read_record_FB (ppcmg_IO_FCB *, char *, int *);
static void  s_IO_write_record_FB(ppcmg_IO_FCB *, char *, int);
static void  s_IO_read_block_FB  (ppcmg_IO_FCB *);
static void  s_IO_write_block_FB (ppcmg_IO_FCB *);

static void  s_IO_write_record_FTP(ppcmg_IO_FCB *, char *, int );
static void  s_IO_read_record_FTP (ppcmg_IO_FCB *, char *, int *);

/*  Start CH #001  */
static void s_IO_write_record_NL (ppcmg_IO_FCB *, char *, int);
static void s_IO_read_record_NL (ppcmg_IO_FCB *, char *, int *);
/*  End CH #001  */

static int   s_getFilesApplicationEntry(ppcmg_IO_FCB *,
                                       files_application_t *);
static int   s_IO_fillApplication(ppcmg_IO_FCB *);

/* Variables */
static char            currProgram[PGM_CODE_LEN + 1] = " ";
static ppcmg_IO_FCB   *IO_handle[PPCMG_IO_HANDLES_NUM] = { NULL };
static int             IO_BufferSize = 0;         
static char            temp_buff[PPCMG_IO_MSG_LEN + 1];  
static char           *disp_msg;                   
static int             status;
static int             error_code;
static int             curr_rec_len;
static int status;
static int error_code;
int        format_offset;
int        format_wmindex;

#undef  __FN__
#define __FN__ "ppcmg_IO"

int ppcmg_IO_setProgram(char *i_program)
{

/* Code *\
\* ---- */

    /* Check length of program's code */
    if (strlen(i_program) > PGM_CODE_LEN) {
        ppcmg_MSG_display(PPCMG_MSG_PROGRAM_NAME_EXCEEDS,
                          i_program,PGM_CODE_LEN); 
        return PPCMG_IO_INVALID_PARAMETER;
    }

    /* Set current program's code */
    strcpy(currProgram, i_program);

    return PPCMG_IO_NORMAL;

}

/* Function IO_open *\
\* ---------------- */

/*************************************************************\
*                                                             *
* Name             : IO_open                                  *
*                                                             *
* Purpose          : Opens external file using data from      *
*                    I/O parameters structure.                *
*                    Checks validity of the following input   *
*                    parameters:                              *
*                       IO_type                               *
*                       dyn_flag                              *
*                    If dynamic allocation is required, the   *
*                    routine prepares the necessary parameters*
*                    and allocates the input/output file      *
*                    resources (a DD card is not required for *
*                    file's parameters).                      *
*                    The routine then attemps to open the     *
*                    file.                                    *
*                    If open is successful, the routine sets  *
*                    pointer to file. Otherwise, if dynamic   *
*                    allocation had been made, the routine    *
*                    deallocates the resources.               *
*                    If deallocation fails, the routine prints*
*                    a local error message, and still returns *
*                    the open failure status.                 *
*                    Status is set :                          *
*                       INVALID PARAMETER                     *
*                       NORMAL if fopen successful            *
*                       OPEN_FILE_FAILURE if open fails       *
*                       DYNINIT_FAILURE if dyninit fails      *
*                    In cases of error detection,             *
*                    the routine returns.                     *
*                    Calls IO_dynAlloc,                       *
*                    ppcmg_MSG_get, ERROR_STACK_ADD.          *
*                                                             *
* Called by        : ppcmg_IO                                 *
*                                                             *
* Input Parameters : filename (data set name of external file)*
*                    IO_type (type of file input or output)   *
*                    dyn_flag (used to determine whether      *
*                              dynamic allocation routines are*
*                              to be performed)               *
*                    All the input parameters are part of     *
*                    I/O parameters structure.                *
*                                                             *
* Output Parameters: pfile (if status NORMAL)                 *
*                    status - NORMAL                          *
*                             INVALID PARAMETER               *
*                             DYNINIT FAILURE                 *
*                             INVALID DYNALLOC PARAMETER      *
*                             READ TABLE FAILURE              *
*                             RESOURCE ALLOCATION PROBLEM     *
*                             OPEN FILE FAILURE               *
*                    Pfile and status are part                *
*                    of I/O parameters structure.             *
*                                                             *
* Return Points    : invalid IO_typeg                         *
*                    dyninit failure                          *
*                    invalid dyn_flag                         *
*                    IO_dynAlloc failure                      *
*                    open file failure (end of routine)       *
*                    open file successful (end of routine)    *
*                                                             *
\*************************************************************/
#undef  __FN__
#define __FN__ "ppcmg_IO_open"

int ppcmg_IO_open(char *i_filename, char *i_filetype, char *i_media,
                  char i_IO_type, int *o_handle)
{
/* Local variables *\
\* --------------- */

    int     index;                       /* Handle's number                */
    int     rc;                          /* Return code                    */
    char   *IO_type;                     /* Used as a parameter to fopen;  */
                                         /* designates whether the file to */
                                         /* be opened is input or output   */
                                         /* file, and also that the file   */
                                         /* is a record file               */
    char   *mapPrefix;
    char    envName[PPCMG_IO_ENV_NAME_LEN + FILETYPE_LEN + 1];
    char    mapFile[PPCMG_IO_MAP_PRFX_LEN + 1 + MAX_DSN_LEN + 1];
    FILE   *fp;

/* Code *\
\* ---- */

    /* First time set I/O buffer size */
    if (IO_BufferSize == 0) {
        disp_msg = getenv("IO_BUFFER_SIZE");
        if (disp_msg) {
            IO_BufferSize = atoi(disp_msg);
            if (IO_BufferSize < 1000) {
                IO_BufferSize = PPCMG_IO_BUF_SIZE;
            }
        }
        else {
            IO_BufferSize = PPCMG_IO_BUF_SIZE;
        }
    }

    *o_handle = -1;

    /* Search for free handle */
    for (index = 0;
         index < PPCMG_IO_HANDLES_NUM && IO_handle[index] != NULL;
         index++);
    if (index >= PPCMG_IO_HANDLES_NUM) {
        ppcmg_MSG_display(PPCMG_MSG_NO_FREE_HANDLE) ;    
        status = PPCMG_IO_RESOURCE_ALLOCATION_PROBLEM;
        return PPCMG_IO_ERROR;
    }

    /* Prepare I/O type */
    switch (i_IO_type) {
        case PPCMG_IO_INPUT:
            IO_type = "rb";
            strcpy(temp_buff, "input");
            break;

        case PPCMG_IO_OUTPUT:
            IO_type = "wb"; 
            strcpy(temp_buff, "output");
            break;

        default:
            status = PPCMG_IO_INVALID_PARAMETER;
            error_code = PPCMG_IO_PARAM_IO_TYPE;
            ppcmg_MSG_display(PPCMG_MSG_INVALID_IO_TYPE, i_IO_type);
            return PPCMG_IO_ERROR;
    }

    /* Allocate parameters structure */
    IO_handle[index] = (ppcmg_IO_FCB *)ppreg_MEM_malloc(sizeof(ppcmg_IO_FCB));
    if (IO_handle[index] == NULL) {
        status = PPCMG_IO_RESOURCE_ALLOCATION_PROBLEM;
        ppcmg_MSG_display(PPCMG_MSG_NO_FREE_MEMORY); 
        return PPCMG_IO_ERROR;
    }

    /* Clear file parameters */
    memset((char *)IO_handle[index], 0, sizeof(ppcmg_IO_FCB));

    /* Fill handle with call parameters */
    ppreg_STR_collapseN(IO_handle[index]->filename, i_filename, MAX_DSN_LEN);
    ppreg_STR_collapseN(IO_handle[index]->filetype, i_filetype, FILETYPE_LEN);
    ppreg_STR_collapseN(IO_handle[index]->media, i_media, MEDIA_LEN);
    IO_handle[index]->IO_type = i_IO_type;

    /* Process file parameters */
    IO_fileParams(IO_handle[index]); 
    if (status != PPCMG_IO_NORMAL) {

	if(IO_handle[index] != NULL)
	{
 	 free(IO_handle[index]);
	}

        IO_handle[index] = NULL;
        return PPCMG_IO_ERROR;
    }

    if (i_IO_type == PPCMG_IO_OUTPUT) {
        /* Order the path */
        IO_checkPath(IO_handle[index]->filename);
        if (status != PPCMG_IO_NORMAL) {

	if(IO_handle[index] != NULL)
	{
 	 free(IO_handle[index]);
	}

	IO_handle[index]=NULL;

        return PPCMG_IO_ERROR;
        }

        /* Check if mapping is required */
        strncpy(envName, PPCMG_IO_MAP_ENV_PRFX, PPCMG_IO_ENV_NAME_LEN);
        strcat(envName, IO_handle[index]->filetype);
        mapPrefix = getenv(envName);
        if (mapPrefix != NULL && strlen(mapPrefix) != 0) {
            /* Build map file name */
            sprintf(mapFile, "%.*s/%s", PPCMG_IO_MAP_PRFX_LEN,
            mapPrefix, IO_handle[index]->filename);

            /* Treatment of temporary file */
            if (IO_handle[index]->conddisp == PPCMG_IO_COND_DISP_DELETE) {
                strncat(IO_handle[index]->filename,
                        PPCMG_IO_TMP_SUFFIX, PPCMG_IO_TMP_SUFFIX_LEN);
            }

            /* Delete probable existing file */
            ppcmg_IO_delete(IO_handle[index]->filename);

            /* Build symbolic link to map file */
            rc = symlink(mapFile, IO_handle[index]->filename);
            if (rc) {
                status = PPCMG_IO_OPEN_FILE_FAILURE;
                error_code = errno;
                ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "symlink",
                                  strerror(errno));   
                ppcmg_MSG_display(PPCMG_MSG_LINK_FILE_FAILURE,   
                                  IO_handle[index]->filetype, mapFile);

	        if(IO_handle[index] != NULL)
           	{
 	            free(IO_handle[index]);
	        }

	        IO_handle[index]=NULL;
                return 0;
             }

             /* Order the path to map file */
             IO_checkPath(mapFile);
             if (status != PPCMG_IO_NORMAL) {

	       if(IO_handle[index] != NULL)
	       {
 	         free(IO_handle[index]);
	       }

	       IO_handle[index]=NULL;

               return 0;
             }
        }
        else {
            /* Treatment of temporary file */
            if (IO_handle[index]->conddisp == PPCMG_IO_COND_DISP_DELETE) {
                strncat(IO_handle[index]->filename,
                        PPCMG_IO_TMP_SUFFIX, PPCMG_IO_TMP_SUFFIX_LEN);
            }
        }

    }

    /* Open file */
    IO_handle[index]->fp = fopen(IO_handle[index]->filename, IO_type);

    /* Set status and error code */
    if (IO_handle[index]->fp == NULL) {      /* Open failure */

        status = PPCMG_IO_OPEN_FILE_FAILURE;
        error_code = errno;
        ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fopen", strerror(errno));   
        ppcmg_MSG_display(PPCMG_MSG_FOPEN_FAIL,
                          IO_handle[index]->filename,
                          IO_handle[index]->filetype);
        /* Deallocate file parameters structure */

	if(IO_handle[index] != NULL)
	{
 	 free(IO_handle[index]);
	}

	IO_handle[index]=NULL;

        return PPCMG_IO_ERROR;
    }

    /* Set buffer size */
    rc = setvbuf(IO_handle[index]->fp, NULL, _IOFBF, IO_BufferSize);
    if (rc) {
        ppcmg_MSG_display(PPCMG_MSG_BUFFER_ALLOC_PROBLEM); 
    }

    if (IO_handle[index]->recfm == PPCMG_IO_RECFM_VB) {
        if (!IO_handle[index]->blksize) {
            IO_handle[index]->blksize = MAX_BLKSIZE;
        }
   
        IO_handle[index]->block_buf =
                       (char *)ppreg_MEM_malloc(IO_handle[index]->blksize);
        if (IO_handle[index]->block_buf == NULL) {
            status = PPCMG_IO_RESOURCE_ALLOCATION_PROBLEM;
            ppcmg_MSG_display(PPCMG_MSG_NO_FREE_MEMORY); 

	if(IO_handle[index] != NULL)
	{
 	 free(IO_handle[index]);
	}

	IO_handle[index]=NULL;

         return PPCMG_IO_ERROR;
        }
      
        if (i_IO_type == PPCMG_IO_INPUT) {
            IO_handle[index]->curr_byte = IO_handle[index]->blksize;
        }
        else {
            IO_handle[index]->curr_byte = 4;
        }
    }
    switch ( IO_handle[index]->recfm ) {
    case PPCMG_IO_RECFM_F :
        break;
    case PPCMG_IO_RECFM_VB :
        break;
    case PPCMG_IO_RECFM_V :
        break;
    case PPCMG_IO_RECFM_UV :
        break;
    case PPCMG_IO_RECFM_FB :
        s_IO_fillApplication(IO_handle[index]);
        break;
    case PPCMG_IO_RECFM_RV :
        s_IO_fillApplication(IO_handle[index]);
        break;
    }
  
    /* Success */
    *o_handle = index;
    status = PPCMG_IO_NORMAL;
    return status;
}

/* Function IO_read *\
\* ---------------- */
/*************************************************************\
*                                                             *
* Name             : IO_read                                  *
*                                                             *
* Purpose          : Reads next record from pfile to rec_buf. *
*                    Pfile validity check is performed prior  *
*                    to fread operation (in case of error     *
*                    status is set to INVALID PARAMETER).     *
*                    Record length can be up to BUFFER_LEN    *
*                    bytes long.                              *
*                    Rec_len is set to actual number of bytes *
*                    read.                                    *
*                    Status is set according to fread result: *
*                      NORMAL if successful                   *
*                      END_OF_FILE if eof                     *
*                      READ_RECORD_FAILURE if error.          *
*                    In case of error detection the routine   *
*                    returns.                                 *
*                    Calls: ppcmg_MSG_get,                    *
*                           ERROR_STACK_ADD.                  *
*                                                             *
* Called by        : ppcmg_IO                                 *
*                                                             *
* Input Parameters : pfile (pointer to input file)            *
*                    rec_buf (to hold read record)            *
*                    Pfile and rec_buf are part of I/O        *
*                    parameters structure.                    *
*                                                             *
* Output Parameters: rec_buf (holds read record if status is  *
*                             NORMAL)                         *
*                    rec_len (length of record if status is   *
*                             NORMAL)                         *
*                    status - NORMAL                          *
*                             END OF FILE                     *
*                             READ RECORD FAILURE             *
*                             INVALID PARAMETER               *
*                    Rec_buf, rec_len and status are part of  *
*                    I/O parameters structure.                *
*                                                             *
* Return Points    : invalid pfile                            *
*                    end of routine                           *
*                                                             *
\*************************************************************/
#undef  __FN__
#define __FN__ "ppcmg_IO_read"

int ppcmg_IO_read(int i_index, void *i_rec_buf, int *o_rec_len)
{
/* Local variables *\
\* --------------- */

    int     rc;          /* Used to determine read error occurrence */
    int     n;       
    long    remain_len;
    char    word[4];     /* For word swap */

/* Code *\
\* ---- */

    rc = 0;

    /* Pfile validity check */
    if (i_index < 0 ) {
        status = PPCMG_IO_INVALID_PARAMETER;
        error_code = PPCMG_IO_PARAM_PFILE;
       	ppcmg_MSG_display(PPCMG_MSG_WRONG_FILE_HANDLE, i_index);   
        return PPCMG_IO_ERROR;
    }
    if (!IO_handle[i_index]->fp) {
        status = PPCMG_IO_INVALID_PARAMETER;
        error_code = PPCMG_IO_PARAM_PFILE;
        ppcmg_MSG_display(PPCMG_MSG_FILE_POINTER_IS_NULL);   
        return PPCMG_IO_ERROR;
    }

    switch ( IO_handle[i_index]->recfm ) {
    case PPCMG_IO_RECFM_F :
        break;
    case PPCMG_IO_RECFM_VB :
        s_IO_read_record_FTP(IO_handle[i_index], i_rec_buf, o_rec_len);
        return status;  
        break;
    case PPCMG_IO_RECFM_V :
        break;
    case PPCMG_IO_RECFM_UV :
        break;
    case PPCMG_IO_RECFM_FB :
        s_IO_read_record_FB(IO_handle[i_index], i_rec_buf, o_rec_len);
        return status;
        break;
    case PPCMG_IO_RECFM_RV :
        s_IO_read_record_FTP(IO_handle[i_index], i_rec_buf, o_rec_len);
        return status;
        break;
/*  Start CH #001  */
    case PPCMG_IO_RECFM_NL :
        s_IO_read_record_NL(IO_handle[i_index], i_rec_buf, o_rec_len);
        return status;
        break;
/*  End CH #001  */
    default:
	return(status=ppcmc_IO_read(IO_handle[i_index] , i_rec_buf, o_rec_len));
	break;
    }

    /* Set readable length of record */
    if (IO_handle[i_index]->recfm == PPCMG_IO_RECFM_F) {
        n = IO_handle[i_index]->lrecl;
    }
    else {
        rc = fread((char *)&n, 1, sizeof(int), IO_handle[i_index]->fp);
        switch (rc)
        {
            case sizeof(int): rc = 0; break;
            case 0:           rc = 1; n = sizeof(int); break;
            default:          rc = 2; n = sizeof(int); break;
        }
    }

    if (rc == 0) 
    {
        /* Set readable length of record  for V format*/
        if (IO_handle[i_index]->recfm == PPCMG_IO_RECFM_V) {
            memset(word, 0, 2);
            memcpy(word + 2, (char *)&n , 2);
            memcpy((char *)&n, word, 4);
            n -= 4;
        }

        /* Correct the length */
        if (n > BUFFER_LEN) {
            remain_len = n - BUFFER_LEN;
            n = BUFFER_LEN;
        }
        else if (n < 0) {
                return PPCMG_IO_READ_RECORD_FAILURE;
        }
        else {
            remain_len = 0;
        }

        /* Execute read command */
        *o_rec_len = fread((char *)i_rec_buf, 1, n, IO_handle[i_index]->fp);
        if (*o_rec_len == n) 
        {
            /* Pass remainder */
            if (remain_len) {
               rc = fseek(IO_handle[i_index]->fp, remain_len, 1);
            }
            if (rc == 0) {
                status = PPCMG_IO_NORMAL;
                return status;
            }
        }
        else 
        {
            if (*o_rec_len == 0 &&
                IO_handle[i_index]->recfm == PPCMG_IO_RECFM_F)
            {
                rc = 1;
            } 
            else
            {
                rc = 2;
            }
        }
    }
    
    if (feof(IO_handle[i_index]->fp)) {
        if (rc == 1) { 
            status = PPCMG_IO_END_OF_FILE;
        }
        else 
        {
            /* Set status */
            status = PPCMG_IO_READ_RECORD_FAILURE;
            ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fread",
                              strerror(errno));   
            ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "fread", n); 
            return PPCMG_IO_ERROR;
        }
    }
    else {
        /* Set status and error code */
        ferror(IO_handle[i_index]->fp);
        error_code = errno;
        status = PPCMG_IO_READ_RECORD_FAILURE;
        ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fread", strerror(errno));   
        ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "fread", n); 
        return PPCMG_IO_ERROR;
    }
    return status;
}

/* Function IO_write *\
\* ----------------- */
/*************************************************************\
*                                                             *
* Name             : IO_write                                 *
*                                                             *
* Purpose          : Writes a record from rec_buf to pfile.   *
*                    Pfile validity check is performed prior  *
*                    to fwrite operation (in case of error    *
*                    status is set to INVALID PARAMETER).     *
*                    Rec_len is the number of bytes to be     *
*                    written and is set by the colling        *
*                    application. It cannot be less then one  *
*                    bytes long.                              *
*                    After write operation rec_len is set by  *
*                    IO routine to actual number of bytes     *
*                    written.                                 *
*                    Status is set according to fwrite result:*
*                       INVALID_PARAMETER if rec_len is less  *
*                                         than 1 (fwrite      *
*                                         attempt is not even *
*                                         tried)              *
*                       NORMAL is successful                  *
*                       WRITE_ERROR_FAILURE if error          *
*                       WRITE_RECORD_TRANCATED if record is   *
*                                              too long       *
*                    In case of error detection, the routine  *
*                    returns.                                 *
*                    Calls: ppcmg_MSG_get,                    *
*                           ERROR_STACK_ADD.                  *
*                                                             *
* Called by        : ppcmg_IO                                 *
*                                                             *
* Input Parameters : pfile (pointer to output file)           *
*                    rec_buf (holds record to be written)     *
*                    rec_len (number of bytes to be written)  *
*                    Pfile, rec_len and status are part of    *
*                    I/O parameters structure.                *
*                                                             *
* Output Parameters: rec_len (number of bytes actually        *
*                             written)                        *
*                    status - NORMAL                          *
*                             INVALID PARAMETER               *
*                             WRITE RECORD FAILURE            *
*                             WRITE RECORD TRANCATED          *
*                    Rec_len and status are part of I/O       *
*                    parameters structure.                    *
*                                                             *
* Return Points    : invalid pfile parameter                  *
*                    invalid buffer length                    *
*                    successful write                         *
*                    write error                              *
*                    write record truncated                   *
*                                                             *
\*************************************************************/
#undef  __FN__
#define __FN__ "ppcmg_IO_write"

int ppcmg_IO_write(int i_index, void *i_rec_buf, int i_rec_len, int i_pad_mode)
{
/* Local variables *\
\* --------------- */

    int     n;                     /* Number of bytes to be written */
    int     m;                   
    char   *buf;
    char    pad_buf[BUFFER_LEN];
    char    word[4];               /* For word swap */

/* Code *\
\* ---- */

    /* Pfile validity check */
    if (i_index < 0 ) {
        status = PPCMG_IO_INVALID_PARAMETER;
        error_code = PPCMG_IO_PARAM_PFILE;
        ppcmg_MSG_display(PPCMG_MSG_WRONG_FILE_HANDLE, i_index) ;   
        return PPCMG_IO_ERROR;
    }
    if (!IO_handle[i_index]->fp) {
        status = PPCMG_IO_INVALID_PARAMETER;
        error_code = PPCMG_IO_PARAM_PFILE;
        ppcmg_MSG_display(PPCMG_MSG_FILE_POINTER_IS_NULL);   
        return PPCMG_IO_ERROR;
    }

    switch ( IO_handle[i_index]->recfm ) {
    case PPCMG_IO_RECFM_F :
        break;
    case PPCMG_IO_RECFM_VB :
        s_IO_write_record(i_index, i_rec_buf, i_rec_len) ;
        return status;
        break;
    case PPCMG_IO_RECFM_V :
        break;
    case PPCMG_IO_RECFM_UV :
        break;
    case PPCMG_IO_RECFM_FB :
        s_IO_write_record_FB(IO_handle[i_index], i_rec_buf, i_rec_len);
        return status;
        break;
    case PPCMG_IO_RECFM_RV :
        s_IO_write_record_FTP(IO_handle[i_index], i_rec_buf, i_rec_len);
        return status;
        break;
/*  Start CH #001  */
    case PPCMG_IO_RECFM_NL :
        s_IO_write_record_NL(IO_handle[i_index], i_rec_buf, i_rec_len);
        return status;
        break;
/*  End CH #001  */
    default:
	return(status=ppcmc_IO_write(IO_handle[i_index] , i_rec_buf, i_rec_len));
	break;
    }

    /* Check record length value */
    n = i_rec_len;
    buf = (char *)i_rec_buf;

    if (i_pad_mode == PPCMG_IO_PAD_BLANKS && 
        IO_handle[i_index]->recfm == PPCMG_IO_RECFM_F &&
        n > 0 &&
        n < IO_handle[i_index]->lrecl)
    {
        memcpy(pad_buf, buf, n);
        memset(pad_buf + n, ' ', IO_handle[i_index]->lrecl - n);
        buf = pad_buf;
        n = IO_handle[i_index]->lrecl;
    }

    if ((n < 1) ||
        (n > IO_handle[i_index]->lrecl) ||
        (IO_handle[i_index]->recfm == PPCMG_IO_RECFM_F &&
         n < IO_handle[i_index]->lrecl)) 
    {
        status = PPCMG_IO_INVALID_PARAMETER;
        error_code = PPCMG_IO_PARAM_REC_LEN;
        ppcmg_MSG_display(PPCMG_MSG_INVALID_OUTREC_LEN, n) ;  
        return PPCMG_IO_ERROR;
    }

    /* Execute write command for length prefix */
    if ((IO_handle[i_index]->recfm == PPCMG_IO_RECFM_UV) ||
        (IO_handle[i_index]->recfm == PPCMG_IO_RECFM_V))
    {
        /* Set readable length of record  for V format*/
        m = n;
        if (IO_handle[i_index]->recfm == PPCMG_IO_RECFM_V) {
            m += 4;
            memcpy(word, (char *)&m + 2, 2);
            memcpy(word + 2, (char *)&m , 2);
            memcpy((char *)&m, word, 4);
        }

        if (1 != fwrite((char *)&m, sizeof(int), 1, IO_handle[i_index]->fp)) {
            /* Set error code */
            ferror(IO_handle[i_index]->fp);
            status = PPCMG_IO_WRITE_RECORD_FAILURE;
            error_code = errno;
            ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fwrite",
                              strerror(errno));   
            ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "write",
                              sizeof(int));   
            return PPCMG_IO_ERROR;
        }
    }

    /* Execute write command for record */
    curr_rec_len = fwrite(buf, 1, n, IO_handle[i_index]->fp);

    /* Set status */
    if (curr_rec_len == n) {
        status = PPCMG_IO_NORMAL;
        return PPCMG_IO_NORMAL;
    }

    /* Set error code */
    ferror(IO_handle[i_index]->fp);
    status = PPCMG_IO_WRITE_RECORD_FAILURE;
    error_code = errno;
    ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fwrite", strerror(errno));   
    ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "fwrite", n) ;   

    return PPCMG_IO_ERROR;
}

/* Function IO_close *\
\* ----------------- */
/*************************************************************\
*                                                             *
* Name             : IO_close                                 *
*                                                             *
* Purpose          : Closes pfile.                            *
*                    Pfile validity check is performed prior  *
*                    to fclose operation (in case of error    *
*                    status is set to INVALID PARAMETER).     *
*                    Pfile is set to NULL if fclose succeeds. *
*                    Status is set according to fclose result:*
*                       NORMAL if success                     *
*                       CLOSE_FILE_FAILURE if error           *
*                    In case of error detection, the routine  *
*                    returns.                                 *
*                    calls: ppcmg_MSG_get,                    *
*                           ERROR_STACK_ADD.                  *
*                                                             *
* Called by        : ppcmg_IO                                 *
*                                                             *
* Input Parameters : pfile (pointer to input/output file)     *
*                    Pfile is part of I/O parameters          *
*                    structure.                               *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             INVALID PARAMETER               *
*                             CLOSE FILE FAILURE              *
*                    pfile (set to NULL if fclose successful) *
*                    Status and pfile are part of I/O         *
*                    parameters structure.                    *
*                                                             *
*                                                             *
* Return Points    : invalid pfile parameter                  *
*                    successful close                         *
*                    close error                              *
*                                                             *
\*************************************************************/
#undef  __FN__
#define __FN__ "ppcmg_IO_close"

int ppcmg_IO_close(int i_index)
{

/* Local variables *\
\* --------------- */

    char     w_str[MAX_DSN_LEN + 1];     /* Working buffer for filename */

/* Code *\
\* ---- */

    /* Pfile validity check */
    if (i_index < 0 ) {
        status = PPCMG_IO_INVALID_PARAMETER;
        error_code = PPCMG_IO_PARAM_PFILE;
        ppcmg_MSG_display(PPCMG_MSG_WRONG_FILE_HANDLE, i_index) ;   
        return PPCMG_IO_ERROR;
    }
    if (!IO_handle[i_index]->fp) {
        status = PPCMG_IO_INVALID_PARAMETER;
        error_code = PPCMG_IO_PARAM_PFILE;
        ppcmg_MSG_display(PPCMG_MSG_FILE_POINTER_IS_NULL);   
        return PPCMG_IO_ERROR;
    }

    /* call ppcmc_IO_preclose in case of custom recfm */
    /* NOTICE: a conseption that custom recfm is greater than 50 assumed */
    if ( IO_handle[i_index]->recfm > 50 )
	   status=ppcmc_IO_preclose(IO_handle[i_index]);


    if (IO_handle[i_index]->recfm == PPCMG_IO_RECFM_VB &&
         IO_handle[i_index]->IO_type == PPCMG_IO_OUTPUT) {
        s_IO_write_block(i_index);
        if (status != PPCMG_IO_NORMAL) {
            ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "write last block",
                                sizeof(int));
        }
    }
    if( IO_handle[i_index]->recfm == PPCMC_IO_RECFM_FB &&
	IO_handle[i_index]->IO_type == PPCMG_IO_OUTPUT) 
    {
        s_IO_write_block_FB(IO_handle[i_index]);
    }

   
    /* Execute close command, set status and error code */
    if (!fclose(IO_handle[i_index]->fp)) {
        /* Treatment of temporary file */
        if (IO_handle[i_index]->IO_type == PPCMG_IO_OUTPUT &&
            IO_handle[i_index]->conddisp == PPCMG_IO_COND_DISP_DELETE)
        {
            strcpy(w_str, IO_handle[i_index]->filename);
            w_str[strlen(w_str) - PPCMG_IO_TMP_SUFFIX_LEN] = '\0';

            /* Delete probable existing file */
            ppcmg_IO_delete(w_str);

            rename(IO_handle[i_index]->filename, w_str);
        }
        status = PPCMG_IO_NORMAL;
    }
    else {
        status = PPCMG_IO_CLOSE_FILE_FAILURE;
        error_code = errno;
        ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fclose",
                          strerror(errno));   
        ppcmg_MSG_display(PPCMG_MSG_CLOSE_FILE_FAILURE);   
    }

    /* Deallocate file parameters structure */
    if (IO_handle[i_index]->block_buf != NULL) {

 	 free(IO_handle[i_index]->block_buf);

	IO_handle[i_index]->block_buf=NULL;

    }
  

	if(IO_handle[i_index] != NULL)
	{
 	 free(IO_handle[i_index]);
	}

	IO_handle[i_index]=NULL;


    IO_handle[i_index] = NULL;

    if (status == PPCMG_IO_NORMAL) {
        return status;
    }
    else {
        ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "free", strerror(errno));   
        return PPCMG_IO_ERROR;
    }

}


/* Function IO_delete *\
\* ------------------ */
/*************************************************************\
*                                                             *
* Name             : IO_delete                                *
*                                                             *
* Purpose          : delete file.                             *
*                                                             *
* Input Parameters : fileName                                 *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             ERROR                           *
\*************************************************************/

#undef  __FN__
#define __FN__ "ppcmg_IO_delete"

int ppcmg_IO_delete(char *fileName)
{

    /* Local variables *\
    \* --------------- */

    int     status;
    char    mapFile[PPCMG_IO_MAP_PRFX_LEN + 1 + MAX_DSN_LEN + 1];

    /* Code *\
    \* ---- */

    /* If file is symbolic link first remove the file link points to */
    error_code = readlink(fileName, mapFile, sizeof(mapFile) - 1);
    if (error_code > 0) {
        /* Terminate file name with zero */
        mapFile[error_code] = 0;

        /* Delete the file link points to */
        error_code = unlink(mapFile); 
        if (error_code) {
            status = PPCMG_IO_DELETE_FILE_FAILURE;
            error_code = errno;
            ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "unlink",
                              strerror(errno));   
            ppcmg_MSG_display(PPCMG_MSG_DELETE_FILE_FAILURE, mapFile);   
            return PPCMG_IO_ERROR;
        }
    }
    else {
        if (error_code == -1 && errno == ENOENT) {
            status = PPCMG_IO_DELETE_FILE_FAILURE;
            error_code = errno;
            return PPCMG_IO_ERROR;
        }
        if (error_code == -1 && errno != EINVAL) {
            status = PPCMG_IO_DELETE_FILE_FAILURE;
            error_code = errno;
            ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "readlink",
                              strerror(errno));   
            ppcmg_MSG_display(PPCMG_MSG_DELETE_FILE_FAILURE, fileName);   
            return PPCMG_IO_ERROR;
        }
    }

    error_code = unlink(fileName);

    if (error_code != 0) {
        status = PPCMG_IO_DELETE_FILE_FAILURE;
        error_code = errno;
        ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "unlink", strerror(errno));   
        ppcmg_MSG_display(PPCMG_MSG_DELETE_FILE_FAILURE, fileName);   
        return PPCMG_IO_ERROR;
    }
    
    status = PPCMG_IO_NORMAL;
    return status;
}

/*  Start CH #002  */
/* Function IO_move *\
 *   * \* ------------------ */
/*************************************************************\
 *                                                             *
 * Name             : IO_move                                  *
 *                                                             *
 * Purpose          : move file.                               *
 *                                                             *
 * Input Parameters : filePath, fileName                                 *
 *                                                             *
 * Output Parameters: status - NORMAL                          *
 *                             ERROR                           *
 * ************************************************************/

#undef  __FN__
#define __FN__ "ppcmg_IO_move"

int ppcmg_IO_move(char *filePath, char *fileName)
{

    int     status;
    int routines_sts;
    char *outputFilePath;
    if ((getenv("TLG_MPS_PHY_ARCHIVE")) == NULL) {
        printf("\nERROR in ppclg_20PHYS_endPhys - TLG_MPS_PHY_ARCHIVE doesn't exist \n");
        status = PPCMG_IO_MOVE_FILE_FAILURE;
        error_code = errno;
        ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "archive path not found", strerror(errno));
        ppcmg_MSG_display(PPCMG_MSG_MOVE_FILE_FAILURE, fileName);
        return PPCMG_IO_ERROR;
    }
    else{
        outputFilePath = getenv("TLG_MPS_PHY_ARCHIVE");
    }
    char archiveDirName[PPCMG_AC_START_DATE_LEN+1];
    char datFileName[PPCMG_AC_FILE_NAME_LEN+1];
    char archiveDirFullPath[FULLPATHFN_C_SZ+1];
    strcpy(datFileName, fileName);
    char *temp = datFileName, *token = strtok(temp,"_"), *prev = token;
    while (token != 0) {
        prev = token;
        token = strtok(0, "_");
    }
    char fileDateToken[10], archiveDirNameToken[10];
    strcpy(fileDateToken, prev);
    int c=0;
    while (c < 8) {
        archiveDirNameToken[c] = fileDateToken[2+c-1];
        c++;
    }
    archiveDirNameToken[c] = '\0';
    snprintf(archiveDirName, PPCMG_AC_START_DATE_LEN + 1 ,"%s", archiveDirNameToken);
    printf("\n dirname in ppclg_20PHYS_endPhys: %s\n",archiveDirName);
    sprintf(archiveDirFullPath, "%s%s%s", outputFilePath, "/", archiveDirName);

    if (routines_sts = ppcmg_IO_create_directory(archiveDirFullPath) == -1){
        printf("\nERROR in ppclg_20PHYS_endPhys -ppclg_20PHYS_creiate_directory failed \n");
        status = PPCMG_IO_MOVE_FILE_FAILURE;
        error_code = errno;
        ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "ppcmg IO create directory", strerror(errno));
        ppcmg_MSG_display(PPCMG_MSG_MOVE_FILE_FAILURE, fileName);
        return PPCMG_IO_ERROR;
    }

    chdir(archiveDirFullPath);
    printf("\n value of ppclg_p.fileName: %s", fileName);
    char moveFiletoArchive[1000];
    sprintf(moveFiletoArchive,"%s%s%s%s%s", "mv ", filePath, "/", fileName, " .");
    system(moveFiletoArchive);

    status = PPCMG_IO_NORMAL;
    return status;
}


int ppcmg_IO_create_directory(char *NewPlace)
{
    char* op_mode;
    int FinalCoMode;
    op_mode = getenv ("OP_MODE");
    FinalCoMode = CoMode;
    if (chdir(NewPlace) < 0){
        if (errno == EACCES) {
            ERRVAL ("(errno %d : %s )",errno,strerror(errno));
            ERRVAL("directory %s",NewPlace);
            printf("ERROR in ppclg_20PHYS_create_directory : change directory failed  , ");
            return(-1);
        }
        umask(CoUmask);
        if ((strcmp (op_mode, "P")) && (strcmp (op_mode,"p"))){
            FinalCoMode = 511;
            umask (0);
        }
        if ((mkdir(NewPlace,FinalCoMode) < 0) && (errno != EEXIST)) {
            ERRVAL ("(errno %d : %s )",errno,strerror(errno));
            ERRVAL("ERROR directory = %s",NewPlace);
            printf("ERROR in ppclg_20PHYS_create_directory : mkdir failed. ");
            return(-1);
        }
    }
    return(0);
}
/*  End CH #002  */

/* Function IO_fileParams *\
\* ---------------------- */
/*************************************************************\
*                                                             *
* Name             : IO_fileParams                            *
*                                                             *
* Purpose          : Opens dynamic allocation table DYNALL.   *
*                    Assembles search key from filetype  and  *
*                    file_mode and searchs it                 *
*                    in the table to find corresponding entry.*
*                    In case of success routine fills the     *
*                    dynamic allocation parameters structure. *
*                    Closes dynamic allocation table DYNALL.  *
*                    Status is set according to execution     *
*                    result:                                  *
*                       NORMAL if success                     *
*                       READ_TABLE_FAILURE if key not found   *
*                    In case of error detection, the routine  *
*                    returns.                                 *
*                    Calls IO_fillFileParams,                 *
*                          JH05231A_getMessgae,               *
*                          ERROR_STACK_ADD.                   *
*                                                             *
* Called by        : IO_dynAlloc                              *
*                                                             *
* Input Parameters : filetype                                 *
*                    IO_media                                 *
*                    file_mode                                *
*                    All of them are parts of I/O parameters  *
*                    structure.                               *
*                    pointer to dynamic allocation parameters.*
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             READ TABLE FAILURE              *
*                             INVALID PARAMETERS              *
*                             INVALID DYNALLOC PARAMETERS     *
*                    Status is part of I/O parameters         *
*                    structure.                               *
*                                                             *
* Return Points    : table not found                          *
*                    key not found                            *
*                    IO_fillFileParams fails                  *
*                    success                                  *
*                                                             *
\*************************************************************/
#undef  __FN__
#define __FN__ "ppcmg_IO_fileParams"

static void IO_fileParams(ppcmg_IO_FCB *io_fcb)
{
/* Local variables *\
\* --------------- */

    int                        rc;  /* Return value                       */
    files_operation_t  buff;  /* For input table entry              */
    char              *field_name;  /* Name of invalid dynalloc parameter */

/* Code *\
\* ---- */

    /* Clear key */
    memset(&buff.key, ' ', sizeof(buff.key));

    /* Put filetype to key field */
    memcpy(buff.key.file_type, io_fcb->filetype,
           MIN(strlen(io_fcb->filetype), sizeof(buff.key.file_type)));

    /* Put current program code to key field */
    if (io_fcb->IO_type == PPCMG_IO_INPUT) {
        memcpy(buff.key.receive_prog, currProgram, strlen(currProgram));
    }
    else {
        memcpy(buff.key.origin_prog, currProgram, strlen(currProgram));
    }

    /* Put media to key field */
    memcpy(buff.key.media, io_fcb->media,
           MIN(strlen(io_fcb->media),sizeof(buff.key.media)));

    /* Search for the key in Files Table */
    rc = entry_in_FILES_OPERATION(&buff, DIRECT_KEY); 

    if (rc != ENTRY_FOUND) {
        error_code = (short)rc;

        /* Key not found in the Files Table */
        status = PPCMG_IO_READ_TABLE_FAILURE;
        ppcmg_MSG_display(PPCMG_MSG_READ_TABLE_FAILURE, 
	                  "FilesOperation_table",
                          sizeof(buff.key), &buff.key);     
        return;
    }

    /* Convert dynamic allocation parameters */
    /* from buff to ppcmg_IO_FCB structure   */
    field_name = IO_fillFileParams(io_fcb, &buff);

    if (field_name != NULL) {
        if (status == PPCMG_IO_INVALID_DYNALLOC_PARAMETER) {
            ppcmg_MSG_display(PPCMG_MSG_INVALID_PARAM, field_name);
        }
    }
    return;
}

/* Function IO_fillFileParams *\
\* -------------------------- */
/*************************************************************\
*                                                             *
* Name             : IO_fillFileParams                        *
*                                                             *
* Purpose          : Converts each parameter in buff to       *
*                    internal SVC 99 form.                    *
*                    Sets converted parameter to __dyn_t      *
*                    structure.                               *
*                    Status is set according to conversion    *
*                    result:                                  *
*                       NORMAL if success                     *
*                       INVALID_PARAMETER  if error in input  *
*                                          data               *
*                       INVALID_DYNALLOC_PARAMETER  if error  *
*                                      in input dynalloc data *
*                    In case of error detection, the routine  *
*                    returns.                                 *
*                    Calls: ppcmg_MSG_get,                    *
*                           ERROR_STACK_ADD.                  *
*                                                             *
* Called by        : IO_dynStruct                             *
*                                                             *
* Input Parameters : pointer to I/O parameters structure      *
*                    pointer to dynamic allocation parameters *
*                    pointer to buffer with data from DYNALL  *
*                    table                                    *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             INVALID PARAMETER               *
*                             INVALID DYNALLOC PARAMETER      *
*                    Status is part of I/O parameters         *
*                    structure.                               *
*                                                             *
* Return Points    : conversion fails                         *
*                    success                                  *
*                                                             *
\*************************************************************/
#undef  __FN__
#define __FN__ "ppcmg_IO_fillFileParams"

#define FLD_CPY(p,f)   ppreg_STR_collapseN(p, buffer->f, sizeof(buffer->f));

static char *IO_fillFileParams(ppcmg_IO_FCB *io_fcb,
                               files_operation_t *buffer)
{
/* Local variables *\
\* --------------- */

    short    number;         /* For saving numeric data    */
    char     w_str[12];      /* Working buffer for string  */

/* Code *\
\* ---- */

    /* Check RECFM and set it */
	    FLD_CPY(w_str, recfm);
	    io_fcb->recfm = 0;
	    if (strlen(w_str) != 0) {
		if (!strcmp(w_str, PPCMG_IO_RECFM_F_STR)) {
		    io_fcb->recfm = PPCMG_IO_RECFM_F;
		}
		if (!strcmp(w_str, PPCMG_IO_RECFM_VB_STR)) {
		    io_fcb->recfm = PPCMG_IO_RECFM_VB;
		}
		if (!strcmp(w_str, PPCMG_IO_RECFM_V_STR)) {
		    io_fcb->recfm = PPCMG_IO_RECFM_V;
		}
		if (!strcmp(w_str, PPCMG_IO_RECFM_UV_STR)) {
		    io_fcb->recfm = PPCMG_IO_RECFM_UV;
		}
		if (!strcmp(w_str, PPCMC_IO_RECFM_RV_STR)) {
		    io_fcb->recfm = PPCMC_IO_RECFM_RV;
		}
/*  Start CH #001  */
                if (!strcmp(w_str, PPCMG_IO_RECFM_NL_STR)) {
                    io_fcb->recfm = PPCMG_IO_RECFM_NL;
                }
/*  End CH #001  */
		if (!strcmp(w_str, PPCMC_IO_RECFM_FB_STR)) {
		    io_fcb->recfm = PPCMC_IO_RECFM_FB;
		    if (!io_fcb->blksize) {
			io_fcb->blksize = MAX_BLKSIZE;
		    }
	      
		    io_fcb->block_buf =
				   (char *)ppreg_MEM_malloc(io_fcb->blksize);
		    if (io_fcb->block_buf == NULL) {
			status = PPCMG_IO_RESOURCE_ALLOCATION_PROBLEM;
			ppcmg_MSG_display(PPCMG_MSG_NO_FREE_MEMORY);

	if(io_fcb != NULL)
	{
 	 free(io_fcb);
	}

	io_fcb=NULL;


			error_code = PPCMG_IO_ERROR;
		    }
		    if (io_fcb->IO_type == PPCMG_IO_INPUT) {
			io_fcb->curr_byte = io_fcb->blksize;
		    }
		    else {
			io_fcb->curr_byte = 0;
		    }
		}
	    }
	    /* call cmc func in case custom recfm is used */
	    if (!io_fcb->recfm) {
		     status=ppcmc_IO_fillFileParams(io_fcb,buffer,w_str);
	    } 

	    /* if recfm has not yet determined it means error */
	    if (!io_fcb->recfm) {
		status = PPCMG_IO_INVALID_DYNALLOC_PARAMETER;
		error_code = PPCMG_IO_DYN_PARAM_RECFM;
		return ("Recfm");
	    }
	    /* Check BLKSIZE and set it */
	    FLD_CPY(w_str, blksize);
    if (strlen(w_str) != 0) {
        if ((number = (short)atoi(w_str)) < 0) {
            status = PPCMG_IO_INVALID_DYNALLOC_PARAMETER;
            error_code = PPCMG_IO_DYN_PARAM_BLKSIZE;
            return ("Blksize");
        }
        io_fcb->blksize = number;
    }

    /* Check LRECL and set it */
    FLD_CPY(w_str, lrecl);
    if (strlen(w_str) != 0) {
        if ((number = (short)atoi(w_str)) < 0 ||
            number > BUFFER_LEN)
        {
            status = PPCMG_IO_INVALID_DYNALLOC_PARAMETER;
            error_code = PPCMG_IO_DYN_PARAM_LRECL;
            return ("Lrecl");
        }
        io_fcb->lrecl = number;
    }

    /* Check CONDDISP and set it */
    FLD_CPY(w_str, cond_disp);
    io_fcb->conddisp = PPCMG_IO_COND_DISP_KEEP;
    if (strlen(w_str) != 0) {
        if (!strcmp(w_str, PPCMG_IO_COND_DISP_DELETE_STR)) {
            io_fcb->conddisp = PPCMG_IO_COND_DISP_DELETE;
        }
    }

    /* Check LABEL for FIXED BLOCK */
    FLD_CPY(w_str, label0);
    io_fcb->label = (unsigned char)(strtol(w_str,NULL,16));

    status = PPCMG_IO_NORMAL;

    return NULL;
}

/* Function IO_checkPath *\
\* --------------------- */
/*************************************************************\
*                                                             *
* Name             : IO_checkPath                             *
*                                                             *
* Purpose          : Check and creates subdirectories of path.*
*                                                             *
* Called by        : IO_open                                  *
*                                                             *
* Input Parameters : pointer to filename.                     *
*                                                             *
* Output Parameters: NORMAL                                   *
*                    PATH CHECK PROBLEM                       *
*                                                             *
*                                                             *
\*************************************************************/
#undef  __FN__
#define __FN__ "ppcmg_IO_checkPath"

static void IO_checkPath(char *i_filename)
{
/* Local variables *\
\* --------------- */

    int      rc;
    int      n;
    char    *dir;                     /* Path's current component      */
    char     w_str[PPCMG_IO_MAP_PRFX_LEN + 1 + MAX_DSN_LEN + 1];
    char     cwd[MAXNAMLEN + 1];      /* Working buffer for curr. dir. */

/* Code *\
\* ---- */

    if (!getcwd(cwd, sizeof(cwd)-1)) { 
        ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "getcwd", strerror(errno));   
        status = PPCMG_IO_PATH_CHECK_PROBLEM;
        return;
    }
    
    if (i_filename[0] == '/') {
        if (chdir("/")) {
            ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "chdir",
                              strerror(errno));   
            ppcmg_MSG_display(PPCMG_MSG_CHECK_DIR_FAILURE, "/", i_filename);   
            status = PPCMG_IO_PATH_CHECK_PROBLEM;
            return;
        }
        strcpy(w_str, i_filename+1);
    }
    else {
        strcpy(w_str, i_filename);
    }

    dir = strrchr(w_str, '/');
    if (dir == NULL) {
        chdir(cwd);
        status = PPCMG_IO_NORMAL;
        return;
    }

    *dir = '\0';

    dir = strtok(w_str, "/");
    if (dir == NULL) {
        status = PPCMG_IO_INVALID_PARAMETER;
        error_code = PPCMG_IO_PARAM_FILENAME;
        ppcmg_MSG_display(PPCMG_MSG_INVALID_FILE_NAME, i_filename);
        chdir(cwd);
        return;
    }

    do
    {
        rc = chdir(dir);
        if (rc) {
            if (errno != ENOTDIR && errno != ENOENT) {
                ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "chdir",
                                  strerror(errno));   
                ppcmg_MSG_display(PPCMG_MSG_CHECK_DIR_FAILURE, dir,
                                  i_filename);   
                status = PPCMG_IO_PATH_CHECK_PROBLEM;
                error_code = errno;
                chdir(cwd);
                return;
            }
            rc = mkdir(dir, PPCMG_IO_MKDIR_MODE);
            if (rc) {
                ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "mkdir",
                                  strerror(errno));   
                ppcmg_MSG_display(PPCMG_MSG_CHECK_DIR_FAILURE, dir, i_filename);
                status = PPCMG_IO_PATH_CHECK_PROBLEM;
                error_code = errno;
                chdir(cwd);
                return;
            }
            rc = chdir(dir);
            if (rc) {
                ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "chdir",
                                  strerror(errno));   
                ppcmg_MSG_display(PPCMG_MSG_CHECK_DIR_FAILURE, dir,
                                  i_filename);   
                status = PPCMG_IO_PATH_CHECK_PROBLEM;
                error_code = errno;
                chdir(cwd);
                return;
            }
        }
        dir = strtok(NULL, "/");
    }
    while (dir != NULL);

    chdir(cwd);
    status = PPCMG_IO_NORMAL;
    return;
}

/* Function s_IO_read_record *\
\* ------------------------- */
/*************************************************************\
*                                                             *
* Name             : s_IO_read_record                         *
*                                                             *
* Purpose          : read record VB.                          *
*                                                             *
* Input Parameters : index of file.                           *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             ERROR                           *
\*************************************************************/
#undef __FN__
#define __FN__ "s_IO_read_record"

static void s_IO_read_record(int i_index, char *i_rec_buf, int *o_rec_len)
{
    int            rc;
    unsigned int   n;
    unsigned int   m;
    char          *s;

    /* If current block has been processed, read the next one */
    if (IO_handle[i_index]->curr_byte + 5 > IO_handle[i_index]->block_len) {
        s_IO_read_block(i_index);
        if (status == PPCMG_IO_END_OF_FILE) {
            return;
        }
        if (status != PPCMG_IO_NORMAL) {
            ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "read record", n);
            return;
        }
    }

    /* Set current record's address in block buffer */
    s = IO_handle[i_index]->block_buf + IO_handle[i_index]->curr_byte;

    /* Scan length of next record in block buffer */
    memcpy((char *)&n + 2, s, 2);
    memcpy((char *)&n, s + 2, 2);

    /* Check record's length */
    if (n < 5 ||
        IO_handle[i_index]->curr_byte + n > IO_handle[i_index]->block_len)
    {
        status = PPCMG_IO_READ_RECORD_FAILURE;
        ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "wrong record's length", n);
        return;
    }

    /* Correct the length */
    m = n - 4 > BUFFER_LEN ? BUFFER_LEN : n - 4;

    /* Get record */
    memcpy(i_rec_buf, s + 4, m);
    *o_rec_len = m;

    /* Shift byte pointer */
    IO_handle[i_index]->curr_byte += n;

    status = PPCMG_IO_NORMAL;
    return;

}


/* Function s_IO_read_block *\
\* ------------------------ */
/*************************************************************\
*                                                             *
* Name             : s_IO_read_block                          *
*                                                             *
* Purpose          : read block  VB.                          *
*                                                             *
* Input Parameters : index of file.                           *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             ERROR                           *
\*************************************************************/
#undef  __FN__ 
#define __FN__ "s_IO_read_block"

static void s_IO_read_block(int i_index)
{
    int            rc;
    unsigned int   n;
    char           s[4];

    /* Execute read command for block length */
    rc = fread(s, 1, 4, IO_handle[i_index]->fp);

    if (rc == 0 && feof(IO_handle[i_index]->fp)) {
        status = PPCMG_IO_END_OF_FILE;
        return;
    }

    if (rc == 4)
    {
        memcpy((char *)&n + 2, s, 2);
        memcpy((char *)&n, s + 2, 2);
        if (n < 9 || n > IO_handle[i_index]->blksize) {
            status = PPCMG_IO_READ_RECORD_FAILURE;
            ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE,
                                     "wrong record's length", n);
            return;
        }
        /* Correct block length */
        n -= 4;

        /* Execute read command */
        rc = fread(IO_handle[i_index]->block_buf, 1, n, IO_handle[i_index]->fp);
        if (rc == n) 
        {
            IO_handle[i_index]->block_len = n;
            IO_handle[i_index]->curr_byte = 0;
            status = PPCMG_IO_NORMAL;
            return;
        }
    }

    /* Set status and error code */
    ferror(IO_handle[i_index]->fp);
    error_code = errno;
    status = PPCMG_IO_READ_RECORD_FAILURE;
    ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fread", strerror(errno));
    ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "fread", n);
    return;
}

/* Function s_IO_write_record *\
\* -------------------------- */
/*************************************************************\
*                                                             *
* Name             : s_IO_write_record                        *
*                                                             *
* Purpose          : write record VB.                         *
*                                                             *
* Input Parameters : index of file.                           *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             ERROR                           *
\*************************************************************/
#undef  __FN__ 
#define __FN__ "s_IO_write_record"

static void s_IO_write_record(int i_index, char *i_rec_buf, int i_rec_len)
{
    unsigned int    n;   
    char           *s;
 
    n = i_rec_len + 4;

    if (IO_handle[i_index]->recfm == PPCMG_IO_RECFM_V && 
                   (n < 5 || n > IO_handle[i_index]->lrecl)) {
        status = PPCMG_IO_INVALID_PARAMETER;
        error_code = PPCMG_IO_PARAM_REC_LEN;
        ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE,
                                     "wrong record's length", n);
        return;

    }

    /* If current block is full flush it */
    if (IO_handle[i_index]->blksize - IO_handle[i_index]->curr_byte < n) {
        s_IO_write_block(i_index);
        if (status != PPCMG_IO_NORMAL) {
            ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "write record", n);
            status = PPCMG_IO_WRITE_RECORD_FAILURE;
            return;
        }
    }

    /* Set current record's address in block buffer */
    s = IO_handle[i_index]->block_buf + IO_handle[i_index]->curr_byte;

    /* Swap record length and put it into block buffer */
    memcpy(s, (char *)&n + 2, 2);
    memcpy(s + 2, (char *)&n, 2);
    s += 4;

    /* Put record into block buffer */
    memcpy(s, i_rec_buf, i_rec_len);

    /* Increase block length */
    IO_handle[i_index]->curr_byte += n;

    status = PPCMG_IO_NORMAL;
    return;

}

/* function s_IO_write_block *\
\* ------------------------- */
/*************************************************************\
*                                                             *
* Name             : s_IO_write_block                         *
*                                                             *
* Purpose          : write block  VB.                         *
*                                                             *
* Input Parameters : index of file.                           *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             ERROR                           *
\*************************************************************/
#undef  __FN__ 
#define __FN__ "s_IO_write_block"

static void s_IO_write_block(int i_index)
{

    unsigned int    n;
    char           *s;

    n = IO_handle[i_index]->curr_byte;
    s = IO_handle[i_index]->block_buf;

    /* Ignore empty block output */
    if (n < 5) {
        status = PPCMG_IO_NORMAL;
        return;
    }

    /* Swap block length and put it into block buffer */
    memcpy(s, (char *)&n + 2, 2);
    memcpy(s + 2, (char *)&n, 2);

    /* Execute write command for block */
    n = fwrite(s, 1, n, IO_handle[i_index]->fp);

    /* Set status */
    if (IO_handle[i_index]->curr_byte == n) {
        /* Init block */
        IO_handle[i_index]->curr_byte = 4;
        status = PPCMG_IO_NORMAL;
        return;
    }

    /* Set error code */
    ferror(IO_handle[i_index]->fp);
    status = PPCMG_IO_WRITE_RECORD_FAILURE;
    error_code = errno;
    ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fwrite", strerror(errno));
    ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "fwrite", n);
    return;

}

/****************************************************\
* Name          :     s_IO_fillApplication           *
*                                                    *
* Purpose       :     find record structure code len *
*                                                    *
* Called by     :     ppcmc_IO_open                  * 
*                                                    *
* Input         :     pfile (pointer to input file)  *
*                                                    *
* Output        :                                    *
*                                                    *
\****************************************************/
static int s_IO_fillApplication(ppcmg_IO_FCB *i_IO_handle)
{
files_application_t filesApplication;
int  rc;

    rc = s_getFilesApplicationEntry(i_IO_handle,
                                    &filesApplication);
    if (rc != ENTRY_FOUND) {
       return -1;
    }
    strcpy(i_IO_handle->body_format,filesApplication.body_format);
    strcpy(i_IO_handle->body_recid,filesApplication.body_recid);

    return 0;
}

/****************************************************\
* Name          :     s_readKeyField                 *
*                                                    *
* Purpose       :     find record structure code len *
*                                                    *
* Called by     :     ppcmc_IO_read                  * 
*                                                    *
* Input         :     pfile (pointer to input file)  *
*                                                    *
* Output        :     structure code length          *
*                                                    *
\****************************************************/
int s_KeyFieldLen(ppcmg_IO_FCB *i_IO_handle)
{
format_table_t *format;
files_application_t filesApplication;
int  rc;

    rc = s_getFilesApplicationEntry(i_IO_handle,
                                    &filesApplication);
    if (rc != ENTRY_FOUND) {
       return -1;
    }
    format = ppreg_FUNC_defineFormat(filesApplication.body_format);
    if ( format == NULL ) {
	return -1;
    }
    return ( wmInfo[format->expVecIndex].len);
}

int s_memccmp(char *buff,char c,int n)
{

char *p = buff;
int   i = 0;

    while ( *p++ == c && i++ <= n );
    return (( i >= n ) ? 0 : 1 );
}

/* Function s_IO_read_record_FB *\
\* ----------------------------- */
/*************************************************************\
*                                                             *
* Name             : s_IO_read_record_FB                     *
*                                                             *
* Purpose          : read record FB.                         *
*                                                             *
* Input Parameters : index of file.                           *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             ERROR                           *
\*************************************************************/
#undef __FN__
#define __FN__ "s_IO_read_record_FB"

static void s_IO_read_record_FB(ppcmg_IO_FCB *i_IO_handle, char *i_rec_buf, int *o_rec_len)
{
    int            rc,i;
    int            externalRecLen;
    int            expandRecLen;
    char          *recBuf;
    int            minimum_record_len;
    char          *recordId;
 
    minimum_record_len = ppreg_FUNC_getFormatKeyBytes(i_IO_handle->body_format);
    if ( minimum_record_len < 0 ) {
        status = PPCMG_IO_READ_RECORD_FAILURE;
        return;
    }

    /* If current block has been processed, read the next one */
    if (i_IO_handle->curr_byte + minimum_record_len >= i_IO_handle->block_len 
        || !(s_memccmp(i_IO_handle->block_buf + i_IO_handle->curr_byte,
                       i_IO_handle->label,minimum_record_len))) {

        s_IO_read_block_FB(i_IO_handle);
        if (status == PPCMG_IO_END_OF_FILE) {
            return;
        }
        if (status != PPCMG_IO_NORMAL) {
            ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "read record", i_IO_handle->block_len);
            return;
        }
    }
    /* Set current record's address in block buffer */
    recBuf = i_IO_handle->block_buf + i_IO_handle->curr_byte;

    /* Scan length of next record in block buffer */
    recordId = ppreg_FUNC_readKeyFieldValue(i_IO_handle->body_format,
                                            recBuf);
    expandRecLen = ppreg_FUNC_getLayoutLength(i_IO_handle->body_format,
                                              i_IO_handle->body_recid,
                                              recordId ) ;
    if ( expandRecLen < 0 ) {
	status = PPCMG_IO_READ_RECORD_FAILURE;
        /*   Datail Message */
	return;
    }

    /* Get record */
    if ( ppreg_FUNC_halfByteFormat(i_IO_handle->body_format)) {
        externalRecLen = expandRecLen / 2;
        recBuf = ppreg_halfByte_expand(recBuf, expandRecLen);
        if (recBuf == NULL) {
            status = PPCMG_IO_READ_RECORD_FAILURE;
            return;
        }
    } else {
        externalRecLen = expandRecLen;
    }

    memcpy(i_rec_buf ,recBuf, expandRecLen);
    *o_rec_len = expandRecLen;

    /* Shift byte pointer */
    i_IO_handle->curr_byte += externalRecLen;

    status = PPCMG_IO_NORMAL;
    return;
}


/* Function s_IO_read_block_FB *\
\* --- ------------------------ */
/*************************************************************\
*                                                             *
* Name             : s_IO_read_block_FB                      *
*                                                             *
* Purpose          : read block  VB.                          *
*                                                             *
* Input Parameters : index of file.                           *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             ERROR                           *
\*************************************************************/
#undef  __FN__ 
#define __FN__ "s_IO_read_block_FB"

static void s_IO_read_block_FB(ppcmg_IO_FCB *i_IO_handle)
{
    int            rc;
    unsigned int   n;

    /* Execute read command for block length */
    n = i_IO_handle->blksize;

    /* Execute read command */
    rc = fread(i_IO_handle->block_buf, 1, n, i_IO_handle->fp);

    if (rc == 0 && feof(i_IO_handle->fp)) {
        status = PPCMG_IO_END_OF_FILE;
        return;
    }

    if (rc == n) 
    {
        i_IO_handle->block_len = n;
        i_IO_handle->curr_byte = 0;
        status = PPCMG_IO_NORMAL;
        return;
    }

    /* Set status and error code */
    ferror(i_IO_handle->fp);
    error_code = errno;
    status = PPCMG_IO_READ_RECORD_FAILURE;
    ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fread", strerror(errno));
    ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "fread", n);
    return;
}

/* Function s_IO_write_record_FB *\
\* ------------------------------ */
/*************************************************************\
*                                                             *
* Name             : s_IO_write_record_FB                     *
*                                                             *
* Purpose          : write record _FB                         *
*                                                             *
* Input Parameters : index of file.                           *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             ERROR                           *
\*************************************************************/
#undef  __FN__ 
#define __FN__ "s_IO_write_record_FB"

static void s_IO_write_record_FB(ppcmg_IO_FCB *i_IO_handle, char *i_rec_buf, int i_rec_len)
{
    unsigned int   n;   
    char          *s;
    char          *recBuf;
 
    if ( ppreg_FUNC_halfByteFormat(i_IO_handle->body_format)) {
        recBuf = ppreg_halfByte_compress(i_rec_buf, i_rec_len);
        if (recBuf == NULL) {
           status = PPCMG_IO_READ_RECORD_FAILURE;
           return;
        }
        i_rec_len /=2;
    } else {
        recBuf = i_rec_buf;
    }

    /* If current block is full flush it */
    if (i_IO_handle->blksize - i_IO_handle->curr_byte < i_rec_len) {
        s_IO_write_block_FB(i_IO_handle);
        if (status != PPCMG_IO_NORMAL) {
            ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "write record", n);
            status = PPCMG_IO_WRITE_RECORD_FAILURE;
            return;
        }
    }

    /* Set current record's address in block buffer */
    s = i_IO_handle->block_buf + i_IO_handle->curr_byte;

    /* Put record into block buffer */
    memcpy(s, recBuf, i_rec_len);

    /* Increase block length */
    i_IO_handle->curr_byte += i_rec_len;

    status = PPCMG_IO_NORMAL;
    return;

}

/* function s_IO_write_block_FB *\
\* ----------------------------- */
/*************************************************************\
*                                                             *
* Name             : s_IO_write_block_FB                      *
*                                                             *
* Purpose          : write block FB.                          *
*                                                             *
* Input Parameters : index of file.                           *
*                                                             *
* Output Parameters: status - NORMAL                          *
*                             ERROR                           *
\*************************************************************/
#undef  __FN__ 
#define __FN__ "s_IO_write_block_FB"

static void s_IO_write_block_FB(ppcmg_IO_FCB *i_IO_handle)
{

    unsigned int    n;
    char           *s;

    s = i_IO_handle->block_buf;

    /* fill space left in block with fillers */
    while (i_IO_handle->curr_byte < i_IO_handle->blksize) 
    {
        *(i_IO_handle->block_buf + i_IO_handle->curr_byte) = i_IO_handle->label;
	(i_IO_handle->curr_byte)++;
    }

    /* Execute write command for block */
    n = fwrite(s, 1, i_IO_handle->curr_byte, i_IO_handle->fp);

    /* Set status */
    if (i_IO_handle->curr_byte == n) {
        /* Init block */
        i_IO_handle->curr_byte = 0;
        status = PPCMG_IO_NORMAL;
        return;
    }

    /* Set error code */
    ferror(i_IO_handle->fp);
    status = PPCMG_IO_WRITE_RECORD_FAILURE;
    error_code = errno;
    ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fwrite", strerror(errno));
    ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "fwrite", n);
    return;

}
#undef  __FN__


/****************************************************\
* Name          :     s_IO_read_record_FTP           *
*                                                    *
* Purpose       :     Read record RV                 *
*                                                    *
* Called by     :     ppcmg_IO_read                  * 
*                                                    *
* Input         :     pfile (pointer to input file)  *
*                     rec_buf (to hold read record)  *
*                     rec_len ( to hold len )        *
*                                                    *
* Output        :     0 - success                    *
*                     1 -failed                      *
*                                                    *
\****************************************************/
static void s_IO_read_record_FTP(ppcmg_IO_FCB *i_IO_handle,
                                 char *i_rec_buf, 
                                 int *o_rec_len) 
{
/* Local variables *\
\* --------------- */

int                       rec_len;
int                       bytesRead;
int                       rc = 0;
char                     *recordId;

    memset(i_rec_buf,0,1000);
    
    bytesRead = ppreg_FUNC_getFormatKeyBytes(i_IO_handle->body_format);
    if ( bytesRead <= 0 ) {
        status = PPCMG_IO_READ_RECORD_FAILURE;
        return;
    }

    /* Reaching for the "key" field in the record */
    fread( i_rec_buf, sizeof(char), bytesRead, i_IO_handle->fp );
    if (feof(i_IO_handle->fp)) {
        status = PPCMG_IO_END_OF_FILE;
        return;
    }
    recordId = ppreg_FUNC_readKeyFieldValue(i_IO_handle->body_format,
                                            i_rec_buf);
    rec_len = ppreg_FUNC_getLayoutLength(i_IO_handle->body_format,
                                         i_IO_handle->body_recid,
                                         recordId ) ;
    if ( rec_len < 0 ) {
	status = PPCMG_IO_READ_RECORD_FAILURE;
        /*   Detail Message */
	return;
    }

    if( rec_len-bytesRead > 0){
        fread( (char *)i_rec_buf+bytesRead,
               sizeof(char),
               rec_len-bytesRead,
               i_IO_handle->fp );
    }

    if (feof(i_IO_handle->fp)) {
        status = PPCMG_IO_END_OF_FILE;
        return;
    }

    *o_rec_len = rec_len;
    status = PPCMG_IO_NORMAL;
    return;
}

/****************************************************\
* Name          :     s_IO_write_record_FTP          *
*                                                    *
* Purpose       :     write record into output       *
*                     file in FTP structure;         *
*                     without any RDW                *
*                                                    *
* Input         :                                    *
*                                                    *
* Output        :                                    *
*                                                    *
\****************************************************/
#define __FN__ "s_IO_write_record_FTP" 

static void s_IO_write_record_FTP(ppcmg_IO_FCB *i_IO_handle,
                                  char *i_rec_buf,
                                  int o_rec_len)
{
  int n ;

  n = fwrite( i_rec_buf,1, o_rec_len,i_IO_handle->fp);

    /* Set status */
    if (o_rec_len == n) { /** normal **/
        status = PPCMG_IO_NORMAL;
        return;
    }

    /* Set error code */
    ferror(i_IO_handle->fp);
    status = PPCMG_IO_WRITE_RECORD_FAILURE;
    error_code = errno;
    ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fwrite", strerror(errno));
    ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "fwrite", n);
    return;
}
#undef __FN__



/*  Start CH #001  */
/***********************************************************************\
* Name             : s_IO_read_record_NL                                *
*                                                                       *
* Purpose          : Read record with new line                          *
*                                                                       *
* Called by        : ppcmg_IO_read ( through the table )                *
*                                                                       *
* Input Parameters : IOhandle index ( checked in the calling function ) *
*                                                                       *
* Output Parameters: rec_buf ( for read record )                        *
*                    rec_len ( for read record length )                 *
*                                                                       *
* Return Points    : fgets failed                                       *
*                    (record length is greater than BUFFER SIZE )       *
*                    unexected end of file                              *
*                                                                       *
* Return Code      : NORMAL                                             *
*                    END OF FILE                                        *
*                    READ_RECORD_FAILURE                                *
\***********************************************************************/

#define __FN__ "s_IO_read_record_NL"

static void s_IO_read_record_NL(ppcmg_IO_FCB *i_IO_handle, char *o_rec_buf, int *o_rec_len)
{
    char   *buff;


    buff = fgets(o_rec_buf, INPUT_RECORD_LENGTH, i_IO_handle->fp);

    *o_rec_len = strlen(o_rec_buf) - 1;

    /* handle case for \r before \n  */
    if ( ( *o_rec_len > 0 ) && (*(o_rec_buf + (*o_rec_len) - 1) == '\r') ) {
       *o_rec_len = *o_rec_len - 1;
    }


    if (buff == NULL && feof(i_IO_handle->fp) ) {

            status = PPCMG_IO_END_OF_FILE;
            return;
    }

    if (buff == NULL && ! feof(i_IO_handle->fp) ) {

    ferror(i_IO_handle->fp);
    error_code = errno;
    ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fgets", strerror(errno));
    status = PPCMG_IO_READ_RECORD_FAILURE;
    return;
    }

    /* Go to the end of line */

    status = PPCMG_IO_NORMAL;
    return;
}

#undef __FN__


/***********************************************************************\
* Name             : s_IO_write_record_NL                               *
*                                                                       *
* Purpose          : Write record with new line                         *
*                                                                       *
*                                                                       *
* Called by        : ppcmg_IO_write ( through the table )               *
*                                                                       *
* Input Parameters : IOhandle index ( checked in the calling function ) *
*                    rec_buf                                            *
*                    rec_len                                            *
*                                                                       *
* Return Points    : invalid record length                              *
*                    fwrite failed                                      *
*                                                                       *
* Return Code      : NORMAL                                             *
*                    INVALID_PARAMETER                                  *
*                    WRITE_RECORD_FAILURE                               *
\***********************************************************************/

#define __FN__ "s_IO_write_record_NL"

static void s_IO_write_record_NL(ppcmg_IO_FCB *i_IO_handle,
                             char *i_rec_buf,
                             int i_rec_len)
{
int     curr_rec_len;
char    pad_buf[OUTPUT_RECORD_LENGTH];


    /* Check record length value */

    if ( i_rec_len > OUTPUT_RECORD_LENGTH ) {
        status = PPCMG_IO_INVALID_PARAMETER;
        return;
    }

    memcpy(pad_buf,i_rec_buf,i_rec_len);

    if (pad_buf[i_rec_len - 1] != '\n') {
        pad_buf[i_rec_len] = '\n';
        i_rec_len++;
    }


    /* Execute write command for record */
    curr_rec_len =  fwrite(pad_buf, 1, i_rec_len, i_IO_handle->fp);

    if (curr_rec_len != i_rec_len) {
        ferror(i_IO_handle->fp);
        error_code = errno;
        ppcmg_MSG_display(PPCMG_MSG_SYSTEM_ERROR, "fwrite", strerror(errno));
        ppcmg_MSG_display(PPCMG_MSG_RECORD_FAILURE, "fwrite", i_rec_len + 1);
        status = PPCMG_IO_WRITE_RECORD_FAILURE;
        return;
    }

    status = PPCMG_IO_NORMAL;
    return;
}

#undef __FN__
/*  End CH #001  */




/****************************************************\
* Name          :     s_getFilesApplicationEntry     *
*                                                    *
* Purpose       :     Find entry in                  *
*                     files_applicataion table       *
*                     with a key <Type>              *
*                                                    *
* Input         :     fileType                       *
*                                                    *
* Output        :     rc                             *
*                                                    *
\****************************************************/
#define __FN__ "ppcmg_IO_getFilesApplicationEntry"
static int s_getFilesApplicationEntry(ppcmg_IO_FCB *i_IO_handle,
                          files_application_t *o_filesApplication)
{
int rc;

    switch(i_IO_handle->IO_type) {
    case('I'):
          rc = ppcmc_UTILgetFilesApplicationEntry(i_IO_handle->filetype,
                                                  NULL,
                                                  currProgram,
                                                  o_filesApplication);
          break; 
    case('O'):
          rc = ppcmc_UTILgetFilesApplicationEntry(i_IO_handle->filetype,
                                                  currProgram,
                                                  NULL,
                                                  o_filesApplication);
          break; 
    }
    return rc;
}

#undef __FN__


