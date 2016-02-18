/*                      Copyright(c) 2009 Teknovus, Inc.                      */

////////////////////////////////////////////////////////////////////////////////
/// \file TkCLi.c
/// \brief Command Line Interface
///
/// The CLI module provides a text-based command shell for debug
/// interaction.  Commands are organized in a tree of directories,
/// much like a typical file system.  The CLI parses user input,
/// matching it against the available tree of commands, and the executes
/// the corresponding command.
///
/// Commands are added to directories and into the tree by other modules.
/// A command is a function with a particular interface.  A directory
/// is a table of pointers to commands and/or other directories.  These
/// tables are statically linked to produce the tree of available commands.
///
////////////////////////////////////////////////////////////////////////////////

//lint --esym(640,*)

#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include "TkPlatform.h"
#include "TkCli.h"
//#include "TkOs.h"
#include "xtty.h"
#include <sys/prctl.h>

#define TkCliStackSize 8192

static U32 cliStack[TkCliStackSize/4];

#define TkMaxInputLineLen    996
#define TkHistoryDepth       4
#define TkMaxArgs            1000
#define TkCmdDirNestDepth    4

#define TK_ESC               0x1b
#define TK_BKSPC             0x08
#define TK_DEL               0x7f
#define TK_CR                0x0d //lint !e767
#define TK_LF                0x0a //lint !e767
#define TK_WHITESPACE        " \t"
#define TK_COMMENT           '#'

// user input
static char  input[TkHistoryDepth][TkMaxInputLineLen]; //< user input line
static U8  curInput;
static U8  inputPos;              //< next place to use in input[curInput]
static U8  maxPos;                //< strlen(input[curInput])

// command function arguments
static char  parsedInput[TkMaxInputLineLen];
static U8  argc;                  //< count of args parsed so far
static char *  argv[TkMaxArgs];   //< pointers to arguments, in input[]

/// TkPwd: stores present working directory info
typedef struct
    {
    const TkCliDirEntry * path[TkCmdDirNestDepth];     //< to each dir in path
    U8 curDepth;                  //< current depth in path stack
    } TkPwd;

static TkPwd pwd; //< present working directory

#define TkUartPutByte(c) putchar(c)
#define TkUartWaitForByte() getchar()
#define TkUartGetByte() getchar()

int cliReady = 0;
////////////////////////////////////////////////////////////////////////////////
/// PrintPwd:  prints pwd as string to stdout
///
/// Walks the TkPwd data structure and outputs that directory
///
///    \param None
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void PrintPwd (void)
    {
    const TkCliDirEntry *  dir;
    U8  i;

    putchar ('/');
    for (i = 1; i <= pwd.curDepth; ++i)
        {
        dir = pwd.path[i-1];
        while ((dir->type) && (dir->ptr != pwd.path[i]))
            {
            dir++;
            }
        printf ("%s/", dir->name);
        }

    } // PrintPwd

////////////////////////////////////////////////////////////////////////////////
/// Prompt:  Prompts for user input
///
/// Supplies the CLI command string out the UART
///
/// \param None
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void Prompt (void)
    {
    printf("\n\rTekHost");
    PrintPwd();
    putchar('>');
    } // Prompt


////////////////////////////////////////////////////////////////////////////////
/// ParseInput:  Parses user input into argv array
///
/// \param None
///
/// \return number of tokens found
////////////////////////////////////////////////////////////////////////////////
static
int ParseInput (void)
    {
    char *s = NULL;

    strncpy (parsedInput, input[curInput],TkMaxInputLineLen-1);

    argc = 0;
    s = parsedInput;
    while ((argc < TkMaxArgs) && s && (*s))
        {
        // skip leading whitespace
        s += strspn (s, TK_WHITESPACE);

        if (*s)
            { // non-whitespace found
            argv[argc++] = s;  // point argv at non-whitespace

            // skip non-whitespace (find whitespace)
            s = strpbrk (s, TK_WHITESPACE);
            if (s)
                { // whitespace found
                if (argc < TkMaxArgs)
                    {
                    *s++ = 0; // null out first whitespace
                    }
                // else don't null the last param, to pass it on
                // command to parse themselves
                }
            }
        }

    return argc;
    } // ParseInput


////////////////////////////////////////////////////////////////////////////////
/// MatchOneDir:  attempts to match current argv[0] to current dir
///
///
///    \param None
///
/// \return pointer to matching dir entry (or final null entry)
////////////////////////////////////////////////////////////////////////////////
static
const TkCliDirEntry * MatchOneDir(const TkCliDirEntry * dir)
    {

    while (dir->type)
        {
           if (stricmp (dir->name, argv[0]) == 0)
            { // a match
            break;
            }
        dir++;
        }

    return dir;
    } // MatchOneDir


////////////////////////////////////////////////////////////////////////////////
/// HandleInput:  Matches input against command tree
///
///
///
/// \param None
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void HandleInput (void)
    {
    const TkCliDirEntry *  dir;
    char *  s;
    TkPwd  tempPath;
    Bool  setDir;

    if (ParseInput())
        {
        if ((stricmp ("help", argv[0]) == 0)||(stricmp ("?", argv[0]) == 0))
            { // display current directory
            printf ("Directory: ");
            PrintPwd();
            putchar('\n');putchar('\r');

            printf ("..%-32cUp one directory\n\r", '/');
            dir = pwd.path[pwd.curDepth];
            while (dir->type)
                {
                printf ("%s%-*c%s\n\r",
                    dir->name,
                    (35- strlen(dir->name)) - 1,
                    (dir->type == TkDirEntryDir) ? '/' : ' ',
                    dir->helpText
                    );
                dir++;
                }
            dir = NULL; // no further action required
            }
        else
            { // try to match pathname
            setDir = FALSE;
            memcpy(&tempPath, &pwd, sizeof(TkPwd));
            if (argv[0][0] == '/')
                { // starting at the root
                tempPath.curDepth = 0;
                argv[0]++; // bump argv to first component of pathname
                setDir = TRUE; // if no more input
                }

            s = argv[0];
            while (s && *s)
                {
                // isolate first component of pathname
                s = strchr (argv[0], '/');
                if (s)
                    {
                    *s++ = 0; // null out the slash, move to next component
                    }

                if (strcmp(argv[0], "..") == 0)
                    { // arg is ".."
                    if (tempPath.curDepth > 0)
                        {
                        tempPath.curDepth--;
                        }
                    if (s)
                        { // more to go
                        // bump to next component of pathname
                        argv[0] = s;
                        }
                    else
                        { // all out of path; change here
                        setDir = TRUE;
                        }
                    }
                else
                    {
                    dir = MatchOneDir(tempPath.path[tempPath.curDepth]);

                    switch (dir->type)
                        {
                        case TkDirEntryTerminator : // couldn't find
                            printf ("Can't find %s\n\r", argv[0]);
                            s = NULL;
                            setDir = FALSE;
                            break;

                        case TkDirEntryDir : // found dir; change there
                            if (tempPath.curDepth == (TkCmdDirNestDepth -1))
                                {
                                printf ("Max dir depth is %d\n\r",
                                    (int)TkCmdDirNestDepth); //lint !e961
                                }
                            else
                                {
                                tempPath.path[++tempPath.curDepth] =
                                    (const TkCliDirEntry *)(dir->ptr);
                                }

                            if (s)
                                { // more to go
                                // bump to next component of pathname
                                argv[0] = s;
                                }
                            else
                                { // all out of path; change here
                                setDir = TRUE;
                                }
                            break;

                        case TkDirEntryCmd :
                            if (s)
                                { // extra stuff after match
                                printf ("%s is a command, not a directory\n\r",
                                    argv[0]);
                                }
                            else
                                { // call the command
                             //   extern void setDbgPrintFlag(U16 enableFlag);
                                /*if((0!=strcmp(dir->name,"dbgSet"))
                                    &&(0!=strcmp(dir->name,"DbgSwitch"))
                                    setDbgPrintFlag(1);*/
                                ((TkCliCmd)(dir->ptr))(argc,
                                    (const char * const *)argv);
                                /*if((0!=strcmp(dir->name,"dbgSet"))
                                    &&(0!=strcmp(dir->name,"DbgSwitch"))
                                    setDbgPrintFlag(0);*/
                                }
                            setDir = FALSE;
                            break;

                        default:
                            break;
                        } // switch dir->type

                    } // if ..

                } // while *s

            if (setDir)
                {
                memcpy (&pwd, &tempPath, sizeof(TkPwd));
                }

            } // else match pathname

        } // if ParseInput

    } // HandleInput



////////////////////////////////////////////////////////////////////////////////
/// CursorLeft:  one-line description
///
/// Detailed description
///
 // Parameters:
///    \param paramName Description
///
/// \return
/// Description of return value
////////////////////////////////////////////////////////////////////////////////
static
void CursorLeft (U8 num)
    {
    if (num)
        {
        printf ("\033[%dD", (int)num);
        }
    } // CursorLeft



////////////////////////////////////////////////////////////////////////////////
/// ClearToEol:  one-line description
///
/// Detailed description
///
 // Parameters:
///    \param paramName Description
///
/// \return
/// Description of return value
////////////////////////////////////////////////////////////////////////////////
static
void ClearToEol (void)
    {
    printf ("\033[K");
    } // ClearToEol



////////////////////////////////////////////////////////////////////////////////
/// SetInsertMode:  one-line description
///
/// Detailed description
///
 // Parameters:
///    \param paramName Description
///
/// \return
/// Description of return value
////////////////////////////////////////////////////////////////////////////////
static
void SetInsertMode (void)
    {
    printf("\033[4h");
    } // SetInsertMode




////////////////////////////////////////////////////////////////////////////////
/// SetOverwriteMode:  one-line description
///
/// Detailed description
///
 // Parameters:
///    \param paramName Description
///
/// \return
/// Description of return value
////////////////////////////////////////////////////////////////////////////////
static
void SetOverwriteMode (void)
    {
    printf("\033[4l");
    } // SetOverwriteMode




////////////////////////////////////////////////////////////////////////////////
/// GetInput:  Gets one line of user input
///
/// Allows user to enter and edit a line of text
///
 // Parameters:
///    \param c                Current character to process
/// \param acceptingInput    pointer to state of comment/noncomment characters
///
/// \return
/// TRUE if more input needed for line
////////////////////////////////////////////////////////////////////////////////
static
Bool GetInput (char c, Bool * acceptingInput)
    {
    switch (c)
        {
        case TK_CR : // end of line
        case TK_LF :
            input[curInput][maxPos] = 0;
            //TkUartPutByte(TK_CR);
            //TkUartPutByte(TK_LF);

            c = TK_LF;
            break;

        case TK_BKSPC : // delete previous char
            if (inputPos > 0)
                {
                CursorLeft(1);
                ClearToEol();
                inputPos--;
                maxPos--;

                strncpy (input[curInput] + inputPos,
                    input[curInput] + inputPos + 1,TkMaxInputLineLen-inputPos);
                printf(input[curInput] + inputPos);
                CursorLeft (maxPos - inputPos);
                }
            c = 0;
            break;

        case TK_DEL :
            if (inputPos < maxPos)
                {
                ClearToEol();
                strncpy (input[curInput] + inputPos,
                    input[curInput] + inputPos + 1,TkMaxInputLineLen-inputPos);
                printf(input[curInput] + inputPos);
                maxPos--;
                CursorLeft(maxPos - inputPos);
                }
            c = 0;
            break;

        case TK_COMMENT : // ignore until EOL
            *acceptingInput = FALSE;
            break;

        case TK_ESC :    // control sequence
            if (TkUartWaitForByte() == '[')
                {
                c = 0;
                switch (TkUartWaitForByte())
                    {
                    case 'A' : // up
                        CursorLeft(inputPos);
                        ClearToEol();
                        curInput = ((curInput + TkHistoryDepth) - 1) % TkHistoryDepth;
                        inputPos = (U8)printf(input[curInput]);
                        maxPos = inputPos;
                        break;

                    case 'B' : // down
                        CursorLeft(inputPos);
                        ClearToEol();
                        curInput = (curInput + 1) % TkHistoryDepth;
                        inputPos = (U8)printf(input[curInput]);
                        maxPos = inputPos;
                        break;

                    case 'C' : // right
                        SetOverwriteMode();
                        if (inputPos < maxPos)
                            {
                            c = input[curInput][inputPos];
                            inputPos++;
                            }
                        break;

                    case 'D' : // left
                        if (inputPos > 0)
                            {
                            c = TK_BKSPC;
                            inputPos--;
                            }
                        break;

                    default :
                        break;
                    }
                }
            break;

        default : // save the input for parsing
            if (*acceptingInput && isprint((int)c))
                {
                if (inputPos < (TkMaxInputLineLen - 1))
                    {
                    if (inputPos < maxPos)
                        {
                        SetInsertMode();
                        memmove (input[curInput] + inputPos + 1,
                            input[curInput] + inputPos, maxPos - inputPos);
                        }
                    input[curInput][inputPos] = c;
                    inputPos++;
                    maxPos++;
                    input[curInput][maxPos] = 0;
                    }
                }
            break;

        } // switch c

    //TkUartPutByte((U8)c);

    return (c != TK_LF);
    } // GetInput



////////////////////////////////////////////////////////////////////////////////
/// TkCliTask:  body of CLI shell task
///
/// This function is expected to be the body of a task which handles
/// user input, parsing it, locating matching commands in the given
/// table, and executing them.
///
/// \param None
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//lint -esym(765,TkCliTask)
extern struct xtty_info *test_xtty;
static void SetCmdPrompt(void)
{
    char PromptBuf[128] = {0};
    char tempBuf[128] = {0};
    const TkCliDirEntry *  dir;
    U8  i;

    snprintf(PromptBuf,7,"%s","TekHost");

    strncat(PromptBuf,"/",1);
    for (i = 1; i <= pwd.curDepth; ++i)
    {
        dir = pwd.path[i-1];
        while ((dir->type) && (dir->ptr != pwd.path[i]))
        {
            dir++;
        }
		
        snprintf (tempBuf,strlen(dir->name)+1,"%s/", dir->name);
        strncat(PromptBuf,tempBuf,strlen(dir->name)+1);		
		memset(tempBuf,0,128);
    }
    strncat(PromptBuf,">",1);
    xtty_set_prefix(test_xtty,PromptBuf);
    PrintPwd();
}

extern void TkCliTask (void)
    {
    Bool acceptingInput;
    U16  readlen = 0;
    char buff[1024] = {0};
    U16  loopIndex = 0;

    pwd.curDepth = 0;
    pwd.path[0] = TkCliRootDir;

    maxPos = 0;
    inputPos = 0;
    curInput = 0;
    input[curInput][inputPos] = 0;

    prctl(PR_SET_NAME, (unsigned int) "CliTask");

    TkCliPrintBanner();

    for (;;)
        {
        //if(cliReady == 1)/**/
        {
            //Prompt();
            SetCmdPrompt();

            maxPos = 0;
            inputPos = 0;
            memset(buff,0,sizeof(buff));
            readlen = xtty_read_line(test_xtty,buff,512);
    		if(readlen > 0)
            {
            acceptingInput = TRUE;
            //while (GetInput((char)TkUartWaitForByte(), &acceptingInput))
            loopIndex = 0;
            while(GetInput(buff[loopIndex++],&acceptingInput))
                {
                    if(loopIndex >= readlen)
                        break;
                // Do nothing
                }

            if (maxPos > 0)
                {
                HandleInput();
                curInput = (curInput + 1) % TkHistoryDepth;
                input[curInput][0] = 0;
                }
            }
        }
        /*else
        {
            usleep(1000*1000);
        }*/
    } // TkCliTask

}

////////////////////////////////////////////////////////////////////////////////
/// CliPoll:  body of CLI shell polling check
///
/// This function is expected to be called periodically to check for
/// user input, parsing it, locating matching commands in the given
/// table, and executing them.
///
/// \param None
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//lint -esym(765,CliPoll)
//lint -esym(714,CliPoll)
extern void CliPoll (void)
    {
    static Bool  acceptingInput;
    static Bool  prompted;
    char  c;

    if (!prompted) //lint !e727
        {
        Prompt();
        acceptingInput = TRUE;
        prompted = TRUE;
        }

    c = (char)TkUartGetByte();
    if (c)
        {
        if (!GetInput(c, &acceptingInput))
            { // got a complete line
            if (maxPos > 0)
                {
                HandleInput();
                curInput = (curInput + 1) % TkHistoryDepth;
                input[curInput][0] = 0;
                }

            maxPos = 0;
            inputPos = 0;
            prompted = FALSE;
            }
        }

    } // CliPoll



////////////////////////////////////////////////////////////////////////////////
/// TkCliInputComplaint:  complain about bad input
///
/// This routine prints a generic complaint about bad input.  It's meant
/// just to take up minimal space on the 8051, and so it's not nearly
/// as friendly as adding command-specific code could be.
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void TkCliInputComplaint (void)
    {
    printf ("Bad params\n\r");
    } // TkCliInputComplaint



///////////////////////////////////////////////////////////////////////////////
/// TkCliPrintBanner:  print boot banner
///
/// The banner displays the name, version, etc., from SysInfo
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void TkCliPrintBanner (void)
    {
    puts("TekHost utility, (c) Broadcom 2013");
    } // TkCliPrintBanner



////////////////////////////////////////////////////////////////////////////////
/// TkCliTaskInit:  Initializes CLI task
///
 // Parameters:
///    \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
/*
void TkCliTaskInit (void)
    {
    (void)TkOsThreadCreate (6, TkCliTask, cliStack, TkCliStackSize,
        (const U8 *)"CLI");
    } // TkCliTaskInit
*/

