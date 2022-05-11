/**
 * @file termargs.c
 * @author Joseph Coston & Saugat Sharma
 * @brief Contains logic for parsing command line arguments more gracefully
 * @version HW6
 * @date 2022-04-06
 *
 */
#include "termargs.h"

// features the project version name
const char *argp_program_version = "j0 HW4";
// describes non-option command line arguments accepted.
static char args_doc[] = "J0FILE1 [... J0FILEn]";
// describes the program itself
static char doc[] = "j0 is a compiler built for a subset of the Java programming language. \n(written by J. Coston & S. Sharma)\n";

/**
 * @brief Struct to communicate the possible options to argp
 * Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
 */
static struct argp_option options[] =
    {
        {"tree", 't', 0, 0, "Print the parse tree"},
        {"debug", 'd', 0, 0, "Run with debugging features active"},
        {"symtab", 's', 0, 0, "Print the symbol tables"},
        {"verbose", 'v', 0, 0, "Generate more verbose output"},
        {0}
    };

/**
 * @brief the command line argument parser
 */
error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
    arguments *a = state->input;
    // set appropriate flags when detected
    switch (key)
    {
    case 't': // flag to print the parse tree
        a->tree = 1;
        break;
    case 's': // flag to print the symbol tables
        a->symtab = 1;
        break;
    case 'v': // flag for more verbose output
        a->verbose = 1;
        break;
    case 'd': // flag for debugging
        debug = 1;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num > 10)
        {
            argp_usage(state);
        }
        if (a->firstfilename == 0)
            a->firstfilename = state->next - 1;
        a->filecount++;
        break;
    case ARGP_KEY_END:
        if (state->arg_num < 1)
        {
            argp_usage(state);
        }
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

arguments get_args(int argc, char *argv[])
{
    // create a new arguments type and populate its fields with defaults
    arguments a;
    a.filenames = NULL;
    a.firstfilename = 0;
    a.filecount = 0;
    a.tree = 0;
    a.symtab = 0;
    a.verbose = 0;

    // parse the arguments from terminal input
    argp_parse(&argp, argc, argv, ARGP_LONG_ONLY, 0, &a);

    // set the filename array correctly
    a.filenames = argv + a.firstfilename;
    return a;
}
