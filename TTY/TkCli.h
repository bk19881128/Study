/*                      Copyright(c) 2009 Teknovus, Inc.                      */

#if !defined(TkCli_h)
#define TkCli_h
////////////////////////////////////////////////////////////////////////////////
/// \file TkCLi.h
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

//#include "TkPlatform.h"
#define stricmp strcasecmp

#if !defined(TRUE)
#define TRUE 1
#endif

#if !defined(FALSE)
#define FALSE 0
#endif

#if !defined(NULL)
#define NULL ((void*)0)
#endif

typedef unsigned char       U8;
typedef unsigned short      U16;
typedef unsigned int       U32;
typedef unsigned long long  U64;
typedef signed char         S8;
typedef signed short        S16;
typedef signed int         S32;
typedef signed long long    S64;
typedef unsigned char       Bool;

extern int LocalBusPrint(const char *format,...);
/// TkCliCmd: prototype for CLI command function
typedef void (* const TkCliCmd) (U8 argc, const char * const argv[]);

/// TkDirEntryType: sorts of things pointers can point to
typedef enum
    {
    TkDirEntryTerminator,
    TkDirEntryDir,
    TkDirEntryCmd
    } TkDirEntryType;

static void print_test(U8 argc, const char *const argv[])
    {   
    (void)argc;
    (void)argv;
    LocalBusPrint("\n\r*** print from cmd line ***\n\r"); 
    }
/// TkCliDirEntry: a single directory entry in a table
typedef struct
    {
    const char * name;         //< dir/command name string
    // To save space on the ONUs the help string has been removed from all of
    // the commands.  Rather than leave the string empty if saves even more
    // space if the field is completely removed from this structure.
    const char * helpText;     //< brief description of the command
    void * ptr;          //< pointer to directory, or command function
    TkDirEntryType type;       //< type of entry (directory or command)
    } TkCliDirEntry;

/// cliRootDir: root directory of command tree
//extern const TkCliDirEntry TkCliRootDir[];
const TkCliDirEntry TkCliRootDir[] =
    {
    { "test_jacob", "test_jacob", print_test, TkDirEntryCmd },
//    { "connect", "Connect to OLT", TkCliConnectOlt, TkDirEntryCmd },
//    { "disconnect", "Disconnect from OLT", TkCliDisconnectOlt, TkDirEntryCmd },
//    { "userIp", "Get/Set User IP Address", TkCliSetUserIp, TkDirEntryCmd },
//    { "userPort", "Get/Set User UDP Port", TkCliSetUserPort, TkDirEntryCmd },
//    { "oltIp", "Get/Set OLT IP Address", TkCliSetOltIp, TkDirEntryCmd },
//    { "oltPort", "Get/Set OLT UDP Port", TkCliSetOltPort, TkDirEntryCmd },
//    { "host", "Host messages (categorized)", TkCliHostCategories, TkDirEntryDir },
//    { "oam", "(ASB)OAM stack messages (categorized)", ASBCliOamStackCategories, TkDirEntryDir },
//    { "msg", "Host messages (by number)", TkCliHostNumbers, TkDirEntryDir },
//#if defined(TK_CLI_UPGRADE_INCLUDED)
//    { "upgrade", "Upgrade OLT/ONU firmware", TkCliUpgrade, TkDirEntryDir },
//#endif
//    { "msgdump", "Enable/Disable message dump", TkCliMsgDebug, TkDirEntryCmd },
//    { "exit", "Quit the program", TkCliExit, TkDirEntryCmd },
//    { "hmiAdapt", "Hmi adapt cmd", hmiAdaptCli, TkDirEntryDir },
//    { "auth", "dpoe auth cmd", auth_debug_cli_entry, TkDirEntryDir },
    { NULL, NULL, NULL, TkDirEntryTerminator }
    };


////////////////////////////////////////////////////////////////////////////////
/// TkCliTaskInit:  Initializes CLI task
///
 // Parameters:
///    \param None
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
//void TkCliTaskInit(void);


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
void TkCliInputComplaint(void);


////////////////////////////////////////////////////////////////////////////////
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
void TkCliPrintBanner(void);


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
void TkCliTask (void);


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
void CliPoll (void);

#endif // TkCLi.h
