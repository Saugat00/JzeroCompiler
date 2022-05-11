/**
 * @file tac.c
 * @author Joseph Coston & Saugat Sharma
 * @brief Three Address Code - skeleton for CS 423 (course-provided code)
 * @version HW6
 * @date 2022-04-22
 */
#include "tac.h"

#define bruh break

stringRegion string_constant_region; // the global string constant region (.string)

char *regionNames[] = {
    "global",
    "loc",
    "class",
    "lab",
    "const",
    "name",
    "none",
    "string"};
char *regionName(int i) { return regionNames[i - R_GLOBAL]; }

// an array of offset counters corresponding to each icode region
int regionCounter[sizeof(regionNames) / sizeof(char *)] = {0};
/** @brief Get the current offset of the region corresponding to the passed region id
 *
 * @param i the region id
 * @return int the current offset of the region in words
 */
int getOffset(int i) { return regionCounter[i - R_GLOBAL]; }
/** @brief Advance the region offset counter corresponding to the passed region id, given the size s of the current address
 *
 * @param i the region id
 * @param s the size of the current address
 * @return int the next offset of the region in words
 */
int nextOffset(int i, int s) { return (regionCounter[i - R_GLOBAL] += s) - s; }
/** @brief Set the region offset counter crresponding to the passed region to the passed value
 *
 * @param i region id
 * @param o offset value
 */
void setOffset(int i, int o) { regionCounter[i - R_GLOBAL] = o; }
/** @brief resets the region offset counter corrresponding to the passed region id
 *
 * @param i region id
 */
void resetOffset(int i) { regionCounter[i - R_GLOBAL] = 0; }

char *opcodeNames[] = {
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    "NEG",
    "ASN",
    "ADDR",
    "LCONT",
    "SCONT",
    "GOTO",
    "BLT",
    "BLE",
    "BGT",
    "BGE",
    "BEQ",
    "BNE",
    "BIF",
    "BNIF",
    "PARM",
    "CALL",
    "RETURN"};
char *opcodeName(int i) { return opcodeNames[i - O_ADD]; }

char *pseudoNames[] = {
    "glob",
    "proc",
    "loc",
    "lab",
    "end",
    "prot"};
char *pseudoName(int i) { return pseudoNames[i - D_GLOB]; }

/**
 * @brief helper to declare a new address in the constant pseudo-region
 *
 * @param v the value of the constant
 * @return Addr* the new address struct pointer
 */
Addr *newConstAddr(int v)
{
    NEWMEM(a, Addr);
    a->region = R_CONST;
    a->u.offset = v;
    a->size = 0; // constants are used in immediate mode and are thus 0-width in memory
    PRINTDEBUG("\033[35mgenerated addr at %s:%d\n", regionName(R_CONST), a->u.offset);
    return a;
}

/**
 * @brief generates a new string address reference to the string constant region
 *
 * @param s the string to reference/add
 * @return Addr* the new address struct pointer
 */
Addr *newStrAddr(char *s)
{
    NEWMEM(a, Addr);
    a->region = R_STRING;
    a->size = strlen(s) + 1;
    a->u.offset = nextOffset(R_STRING, a->size); // add 1 for null byte
    string_constant_region.size += a->size;
    string_constant_region.contents = realloc(string_constant_region.contents, string_constant_region.size);
    strncat(string_constant_region.contents + (string_constant_region.size - a->size), s, a->size + 1);
    return a;
}

/**
 * @brief generates a new temporary variable address in the region corresponding to the passed region index
 *
 * @param r the region id
 * @param n size of the new address in WORDS
 * @return Addr* the new address
 */
Addr *newAddr(int r, int n)
{
    switch (r)
    {
    case R_CONST:
        return newConstAddr(n);
    default:
        break;
    }
    // switch case default behavior oustide of switch statement due to need for new address allocation
    NEWMEM(a, Addr);
    a->size = n;
    a->region = r;                  // select the region
    a->u.offset = nextOffset(r, n); // place localCounter at the start of the next available offset in the local region
    PRINTDEBUG("\033[35mgenerated addr at %s:%d\n", regionName(r), a->u.offset);
    return a;
}

// int localCounter = 0;
// /** DEPRECATED, USE newAddr(R_LOCAL,n) instead
//  * @brief generates a new temporary variable address in the local region (on the stack)
//  *
//  * @param n the size of the temporary variable in WORDS
//  * @return Addr* a pointer to the new address struct
//  */
// Addr *newtmp(int n)
// {
//     NEWMEM(a, Addr);
//     a->region = R_LOCAL;
//     a->u.offset = nextOffset(R_LOCAL, n); // place localCounter at the start of the next available offset in the local region
//     PRINTDEBUG("\033[35mgenerated a tmp in local %d\n", a->u.offset);
//     return a;
// }

// //**
//  * @brief
//  *
//  */
//  *
//  * @param a the address pointer to remove
//  */
// void remtmp(Addr *a)
// {
//     // maybe do something with the local counter here...
//     free(a);
//     return;
// }

int labelCounter = 0;
/**
 * @brief generates a new address in the label pseudo-region
 *
 * @return Addr* a pointer to the new address struct
 */
Addr *genlabel()
{
    NEWMEM(a, Addr);
    a->region = R_LABEL;
    a->u.offset = labelCounter++;
    a->size = 1;
    printf("generated a label %d\n", a->u.offset);
    return a;
}

/**
 * @brief generate a new three address code instruction
 *
 * @param op the operation opcode
 * @param d0 the destination address
 * @param s1 the first source address
 * @param s2 the second source address
 * @return Instr* a pointer to the new instruction struct
 */
Instr *gen(int op, Addr *d0, Addr *s1, Addr *s2)
{
    NEWMEM(rv, Instr);
    rv->opcode = op;
    rv->dest = d0;
    rv->src1 = s1;
    rv->src2 = s2;
    rv->next = NULL;
    return rv;
}

/**
 * @brief recursively copies the instruction list referenced by the passed pointer
 *
 * @param l pointer to the instruction list to copy
 * @return Instr* a pointer to the newly copied instruction list
 */
Instr *copylist(Instr *l)
{
    if (l == NULL)
        return NULL;
    Instr *lcopy = gen(l->opcode, l->dest, l->src1, l->src2);
    lcopy->next = copylist(l->next); // recursive call
    return lcopy;
}

/**
 * @brief joins the instruction list referenced by l2 to the end of the instruction list referenced by l1 (operation performed in-place)
 *
 * @param l1 pointer to the instruction list to append l2 to
 * @param l2 pointer to the instruction list to append to l1
 * @return Instr* a pointer to the conjoined instruction list
 */
Instr *append(Instr *l1, Instr *l2)
{
    if (l1 == NULL)
        return l2;
    Instr *ltmp = l1;
    while (ltmp->next != NULL)
        ltmp = ltmp->next;
    ltmp->next = l2;
    return l1;
}

/**
 * @brief creates a reference to a new instruction list consisting of l1 appended to l2
 *
 * @param l1 pointer to the instruction list to concatenate l2 to
 * @param l2 pointer to the instruction list to concatenate to l1
 * @return Instr* a ponter to the newly created concatenated instruction list
 */
Instr *concat(Instr *l1, Instr *l2)
{
    return append(copylist(l1), l2);
}

/**
 * @brief recursively print a list of intructions
 *
 * @param i the i of an instruction pointer linked list
 */
void printInstr(Instr *i)
{
    if (i == NULL)
        return; // do noting on null address pointer

    if (i->opcode >= O_ADD && i->opcode < D_GLOB)
        PRINTOFILE("\t%s\t", opcodeName(i->opcode));

    if (i->dest)
    {
        switch (i->opcode)
        { // special treatment for certain opcodes and declarations
        case O_CALL:
            PRINTOFILE("%s,%d,", i->dest->u.name, i->src1->u.offset);
            printAddr(i->src2);
            break;

        case D_PROC:
            PRINTOFILE("%s\t%s,%d,%d", pseudoName(i->opcode), i->dest->u.name, i->src1->u.offset, i->src2->u.offset);
            bruh;

        case D_LOCAL:
            PRINTOFILE("\t%s\t", pseudoName(i->opcode));
            printAddr(i->dest);
            PRINTOFILE(",");
            printAddr(i->src1);
            break;

        default:
            printAddr(i->dest);
            if (i->src1)
            {
                PRINTOFILE(",");
                printAddr(i->src1);
            }
            if (i->src2)
            {
                PRINTOFILE(",");
                printAddr(i->src2);
            }
            break;
        }
    }
    else
        PRINTERR("WARNING: no destination address in instruction.\n");

    PRINTOFILE("\n");
    if (i->next)
        printInstr(i->next);
}

/**
 * @brief helper printer function to print the address
 *
 * @param region as region of address
 *
 * #return void
 */
void printAddr(Addr *a)
{
    if (a->region - R_GLOBAL > sizeof(regionNames) / sizeof(char *) || a->region - R_GLOBAL < 0)
    {
        PRINTERR("ERROR region index %d out of bounds.\n", a->region);
        return;
        // exit();   //not sure about exit# here
    }
    else
    {
        switch (a->region)
        { // printing behavior is region dependant
        case R_NAME:
        case R_CLASS:
            PRINTOFILE("%s:%s", regionName(a->region), a->u.name);
            break;
        default:
            PRINTOFILE("%s:%d", regionName(a->region), a->u.offset);
            break;
        }
    }
}

void printStringRegion(void)
{
    PRINTOFILE("\n.string %d", string_constant_region.size);
    int i = 0;
    for (char c = string_constant_region.contents[i]; i < string_constant_region.size; c = string_constant_region.contents[++i])
    {
        if (!(i % 16)) // return every 16 chars
            PRINTOFILE("\n\t");

        if (c >= 0 && c <= ' ')
        { // if char is a non-printable, print its octal code
            PRINTOFILE("\\%03o", (int)c);
        }
        else
        {
            PRINTOFILE("%c", c);
        }
    }
}
