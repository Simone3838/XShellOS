/*****************************************
		NanoShell Operating System
	   (C) 2021, 2022 iProgramInCpp

       NSScript Interpreter  module
******************************************/

#include <main.h>
#include <console.h>
#include <video.h>
#include <vfs.h>
#include <keyboard.h>
#include <string.h>
#include <memory.h>
#include <misc.h>
#include <cinterp.h>
#include <print.h>
#include <idt.h>

// c4.c - C in four functions
// char, int, and pointer types
// if, while, return, and expression statements
// just enough features to allow self-compilation and a bit more
// Written by Robert Swierczek, ported to NanoShell by iProgramInCpp
extern Console* g_currentConsole;

// tokens and classes (operators last and in precedence order)
enum
{
    Num = 128, Fun, Sys, Glo, Loc, Id, FunPro,
    Char, Else, Enum, If, Int, Return, Sizeof, While,
    Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

// opcodes
enum
{
    LEA, IMM, JMP, JSR, BZ, BNZ, ENT, ADJ, LEV, LI, LC, SI, SC, PSH,
    OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, MUL, DIV, MOD,
    OPEN, READ, CLOS, PRTF, PRTN, MALC, FREE, MSET, MCMP, RAND, DRPX, EXSC, RDCH, RDIN, CLSC,
    FLSC, FLRC, DRRC, SSCY, DRST, SPTF, MVCR, SLEP, EXIT
};

// types
enum { CHAR, INT, PTR };

// identifier offsets (since we can't create an ident struct)
enum { Tk, Hash, Name, Class, Type, Val, HClass, HType, HVal, Idsz };


#define exit(code) do {\
	LogMsg("Exit requested with code %d. We can't exit straight away so we will just let the application keep running, which might result in a crash or something.");\
	return;\
} while (0)

#define Except(mach, eno) do {\
	mach->g_pErrorExists = true;\
	mach->g_pErrorLine = __LINE__;\
    mach->m_halted = true;\
	LongJump(mach->m_jumpError, eno);\
} while (0)


void CcAdvanceOpc(CMachine* pMachine)
{
    ++pMachine->pText;
}
void CcAdvanceImm(CMachine* pMachine)
{
    ++pMachine->pText;
}
void CcSetOpCodeHere(CMachine* pMachine, int opc)
{
    *pMachine->pText = opc;
}
void CcSetImmHere   (CMachine* pMachine, int imm)
{
    *pMachine->pText = imm;
}

bool CcOpcodeRequiresOptional (int opc)
{
    return opc <= ADJ;
}

void CcPrintOpCode(int opc, int optional/*opc <= ADJ*/)
{
    LogMsgNoCr("%s",
               &"LEA \0IMM \0JMP \0JSR \0BZ  \0BNZ \0ENT \0ADJ \0LEV \0LI  \0LC  \0SI  \0SC  \0PSH \0"
               "OR  \0XOR \0AND \0EQ  \0NE  \0LT  \0GT  \0LE  \0GE  \0SHL \0SHR \0ADD \0SUB \0MUL \0DIV \0MOD \0"
               "OPEN\0READ\0CLOS\0PRTF\0PRTN\0MALC\0FREE\0MSET\0MCMP\0RAND\0DRPX\0EXSC\0RDCH\0RDIN\0CLSC\0"
               "FLSC\0FLRC\0DRRC\0SSCY\0DRST\0SPTF\0MVCR\0SLEP\0EXIT\0"[opc * 5]);
    if (CcOpcodeRequiresOptional (opc) && optional != (int)0xDDEEAAFF/*hack!!!*/)
    {
        LogMsg(" %d (%x)", optional, optional);
    }
    else LogMsg("");
}

// equivalent to *++e = opc;
void CcPushOpCode(CMachine* pMachine, int opc)
{
    CcAdvanceOpc(pMachine);
    CcSetOpCodeHere(pMachine, opc);
    //LogMsg("Pushed opcode: ");
    //CcPrintOpCode(opc, 0xDDEEAAFF);
}
// equivalent to *++e = imm;
void CcPushImm(CMachine* pMachine, int imm)
{
    CcAdvanceImm(pMachine);
    CcSetImmHere(pMachine, imm);
    //LogMsg("Pushed immediate: %d\n", imm);
}
int CcGetOpCodeHere(CMachine* pMachine)
{
    return *pMachine->pText;
}
int CcGetImmHere   (CMachine* pMachine)
{
    return *pMachine->pText;
}
void CcGoBackOpc   (CMachine* pMachine)
{
    pMachine->pText--;
}
void CcGoBackImm   (CMachine* pMachine)
{
    pMachine->pText--;
}

void CcNextToken(CMachine* pMachine)
{
    char* pp;

    // While the current token is not blank:
    while ((pMachine->currentToken = *pMachine->pSource) != 0)
    {
        ++pMachine->pSource;
        // Hit new-line ?
        if (pMachine->currentToken == '\n')
        {
            // If there's a need to print asm-output
            if (pMachine->printAssembly)
            {
                char buffer[4096];
                int size = pMachine->pSource - pMachine->pLastSource;
                memcpy (buffer, pMachine->pLastSource, size);
                buffer[size] = 0;
                LogMsg("%d: %s", pMachine->lineNum, buffer);

                pMachine->pLastSource = pMachine->pSource;
                while (pMachine->pLastText < pMachine->pText)
                {
                    int optional = 0, opcode = *++pMachine->pLastText;

                    if (CcOpcodeRequiresOptional (opcode))
                        optional = *++pMachine->pLastText;

                    CcPrintOpCode(opcode, optional);
                }
            }
            ++pMachine->lineNum;
        }
        else if (pMachine->currentToken == '#')
        {
            while (*pMachine->pSource != 0 && *pMachine->pSource != '\n') ++pMachine->pSource;
        }
        else if ((pMachine->currentToken >= 'a' && pMachine->currentToken <= 'z') || (pMachine->currentToken >= 'A' && pMachine->currentToken <= 'Z') || pMachine->currentToken == '_')
        {
            pp = pMachine->pSource - 1;
            while ((*pMachine->pSource >= 'a' && *pMachine->pSource <= 'z') || (*pMachine->pSource >= 'A' && *pMachine->pSource <= 'Z') || (*pMachine->pSource >= '0' && *pMachine->pSource <= '9') || *pMachine->pSource == '_')
                pMachine->currentToken = pMachine->currentToken * 147 + *pMachine->pSource++;
            pMachine->currentToken = (pMachine->currentToken << 6) + (pMachine->pSource - pp);
            pMachine->pCurrIdentifier = pMachine->pCurrSymbol;
            while (pMachine->pCurrIdentifier[Tk])
            {
                if (pMachine->currentToken == pMachine->pCurrIdentifier[Hash] && !memcmp((char*)pMachine->pCurrIdentifier[Name], pp, pMachine->pSource - pp))
                {
                    pMachine->currentToken = pMachine->pCurrIdentifier[Tk];
                    return;
                }
                pMachine->pCurrIdentifier = pMachine->pCurrIdentifier + Idsz;
            }
            pMachine->pCurrIdentifier[Name] = (int)pp;
            pMachine->pCurrIdentifier[Hash] = pMachine->currentToken;
            pMachine->currentToken = pMachine->pCurrIdentifier[Tk] = Id;
            return;
        }
        else if (pMachine->currentToken >= '0' && pMachine->currentToken <= '9')
        {
            if ((pMachine->currentTokenValue = pMachine->currentToken - '0'))
            {
                while (*pMachine->pSource >= '0' && *pMachine->pSource <= '9') pMachine->currentTokenValue = pMachine->currentTokenValue * 10 + *pMachine->pSource++ - '0';
            }
            else if (*pMachine->pSource == 'x' || *pMachine->pSource == 'X')
            {
                while ((pMachine->currentToken = *++pMachine->pSource) && ((pMachine->currentToken >= '0' && pMachine->currentToken <= '9') || (pMachine->currentToken >= 'a' && pMachine->currentToken <= 'f') || (pMachine->currentToken >= 'A' && pMachine->currentToken <= 'F')))
                    pMachine->currentTokenValue = pMachine->currentTokenValue * 16 + (pMachine->currentToken & 15) + (pMachine->currentToken >= 'A' ? 9 : 0);
            }
            else
            {
                while (*pMachine->pSource >= '0' && *pMachine->pSource <= '7') pMachine->currentTokenValue = pMachine->currentTokenValue * 8 + *pMachine->pSource++ - '0';
            }
            pMachine->currentToken = Num;
            return;
        }
        else if (pMachine->currentToken == '/')
        {
            if (*pMachine->pSource == '/')
            {
                ++pMachine->pSource;
                while (*pMachine->pSource != 0 && *pMachine->pSource != '\n') ++pMachine->pSource;
            }
            else
            {
                pMachine->currentToken = Div;
                return;
            }
        }
        else if (pMachine->currentToken == '\'' || pMachine->currentToken == '"')
        {
            pp = pMachine->pData;
            while (*pMachine->pSource != 0 && *pMachine->pSource != pMachine->currentToken)
            {
                if ((pMachine->currentTokenValue = *pMachine->pSource++) == '\\')
                {
                    if ((pMachine->currentTokenValue = *pMachine->pSource++) == 'n') pMachine->currentTokenValue = '\n';
                }
                if (pMachine->currentToken == '"') *pMachine->pData++ = pMachine->currentTokenValue;
            }
            ++pMachine->pSource;
            if (pMachine->currentToken == '"') pMachine->currentTokenValue = (int)pp;
            else pMachine->currentToken = Num;
            return;
        }
        else if (pMachine->currentToken == '=')
        {
            if (*pMachine->pSource == '=')
            {
                ++pMachine->pSource;
                pMachine->currentToken = Eq;
            }
            else pMachine->currentToken = Assign;
            return;
        }
        else if (pMachine->currentToken == '+')
        {
            if (*pMachine->pSource == '+')
            {
                ++pMachine->pSource;
                pMachine->currentToken = Inc;
            }
            else pMachine->currentToken = Add;
            return;
        }
        else if (pMachine->currentToken == '-')
        {
            if (*pMachine->pSource == '-')
            {
                ++pMachine->pSource;
                pMachine->currentToken = Dec;
            }
            else pMachine->currentToken = Sub;
            return;
        }
        else if (pMachine->currentToken == '!')
        {
            if (*pMachine->pSource == '=')
            {
                ++pMachine->pSource;
                pMachine->currentToken = Ne;
            }
            return;
        }
        else if (pMachine->currentToken == '<')
        {
            if (*pMachine->pSource == '=')
            {
                ++pMachine->pSource;
                pMachine->currentToken = Le;
            }
            else if (*pMachine->pSource == '<')
            {
                ++pMachine->pSource;
                pMachine->currentToken = Shl;
            }
            else pMachine->currentToken = Lt;
            return;
        }
        else if (pMachine->currentToken == '>')
        {
            if (*pMachine->pSource == '=')
            {
                ++pMachine->pSource;
                pMachine->currentToken = Ge;
            }
            else if (*pMachine->pSource == '>')
            {
                ++pMachine->pSource;
                pMachine->currentToken = Shr;
            }
            else pMachine->currentToken = Gt;
            return;
        }
        else if (pMachine->currentToken == '|')
        {
            if (*pMachine->pSource == '|')
            {
                ++pMachine->pSource;
                pMachine->currentToken = Lor;
            }
            else pMachine->currentToken = Or;
            return;
        }
        else if (pMachine->currentToken == '&')
        {
            if (*pMachine->pSource == '&')
            {
                ++pMachine->pSource;
                pMachine->currentToken = Lan;
            }
            else pMachine->currentToken = And;
            return;
        }
        else if (pMachine->currentToken == '^')
        {
            pMachine->currentToken = Xor;
            return;
        }
        else if (pMachine->currentToken == '%')
        {
            pMachine->currentToken = Mod;
            return;
        }
        else if (pMachine->currentToken == '*')
        {
            pMachine->currentToken = Mul;
            return;
        }
        else if (pMachine->currentToken == '[')
        {
            pMachine->currentToken = Brak;
            return;
        }
        else if (pMachine->currentToken == '?')
        {
            pMachine->currentToken = Cond;
            return;
        }
        else if (pMachine->currentToken == '~' || pMachine->currentToken == ';' || pMachine->currentToken == '{' || pMachine->currentToken == '}' || pMachine->currentToken == '(' || pMachine->currentToken == ')' || pMachine->currentToken == ']' || pMachine->currentToken == ',' || pMachine->currentToken == ':') return;
    }
}
void CcOnExpression(CMachine* pMachine, int lev)
{
    int t, * d;

    if (!pMachine->currentToken)
    {
        LogMsg("%d: unexpected eof in expression", pMachine->lineNum);
        Except(pMachine, CCSTATUS_UNEXPECTED_EOF);
    }
    else if (pMachine->currentToken == Num)
    {
        CcPushOpCode(pMachine, IMM);
        CcPushImm(pMachine, pMachine->currentTokenValue);
        CcNextToken(pMachine);
        pMachine->currentExprType = INT;
    }
    else if (pMachine->currentToken == '"')
    {
        CcPushOpCode(pMachine, IMM);
        CcPushImm(pMachine, pMachine->currentTokenValue);
        CcNextToken(pMachine);
        while (pMachine->currentToken == '"') CcNextToken(pMachine);
        //FIXME
        pMachine->pData = (char*)((int)pMachine->pData + sizeof(int) & (unsigned int)(-(int)(sizeof(int))));
        pMachine->currentExprType = PTR;
    }
    else if (pMachine->currentToken == Sizeof)
    {
        CcNextToken(pMachine);
        if (pMachine->currentToken == '(') CcNextToken(pMachine);
        else
        {
            LogMsg("%d: open paren expected in sizeof", pMachine->lineNum);
            Except(pMachine, CCSTATUS_OPEN_PAREN_EXPECTED);
        }
        pMachine->currentExprType = INT;
        if (pMachine->currentToken == Int) CcNextToken(pMachine);
        else if (pMachine->currentToken == Char)
        {
            CcNextToken(pMachine);
            pMachine->currentExprType = CHAR;
        }
        while (pMachine->currentToken == Mul)
        {
            CcNextToken(pMachine);
            pMachine->currentExprType = pMachine->currentExprType + PTR;
        }
        if (pMachine->currentToken == ')') CcNextToken(pMachine);
        else
        {
            LogMsg("%d: close paren expected in sizeof", pMachine->lineNum);
            Except(pMachine, CCSTATUS_CLOSE_PAREN_EXPECTED);
        }
        CcPushOpCode(pMachine, IMM);
        CcPushImm(pMachine, (pMachine->currentExprType == CHAR) ? sizeof(char) : sizeof(int));
        pMachine->currentExprType = INT;
    }
    else if (pMachine->currentToken == Id)
    {
        d = pMachine->pCurrIdentifier;
        CcNextToken(pMachine);
        if (pMachine->currentToken == '(')
        {
            CcNextToken(pMachine);
            t = 0;
            while (pMachine->currentToken != ')')
            {
                CcOnExpression(pMachine, Assign);
                //ExitIfAnythingWrong(pMachine);
                CcPushOpCode(pMachine, PSH);
                ++t;
                if (pMachine->currentToken == ',')
                    CcNextToken(pMachine);
            }
            CcNextToken(pMachine);
            if (d[Class] == Sys) CcPushImm(pMachine, d[Val]);
            else if (d[Class] == Fun)
            {
                CcPushImm(pMachine, JSR);
                CcPushImm(pMachine, d[Val]);
            }
            else
            {
                LogMsg("%d: bad function call", pMachine->lineNum);
                Except(pMachine, CCSTATUS_BAD_FUNCTION_CALL);
            }
            if (t)
            {
                CcPushOpCode(pMachine, ADJ);
                CcPushImm(pMachine, t);
            }
            pMachine->currentExprType = d[Type];
        }
        else if (d[Class] == Num)
        {
            CcPushOpCode(pMachine, IMM);
            CcPushImm(pMachine, d[Val]);
            pMachine->currentExprType = INT;
        }
        else
        {
            if (d[Class] == Loc)
            {
                CcPushOpCode(pMachine, LEA);
                CcPushImm(pMachine, pMachine->localVariableOffset - d[Val]);
            }
            else if (d[Class] == Glo)
            {
                CcPushOpCode(pMachine, IMM);
                CcPushImm(pMachine, d[Val]);
            }
            else
            {
                LogMsg("%d: undefined variable", pMachine->lineNum);
                Except(pMachine, CCSTATUS_UNDEFINED_VARIABLE);
            }
            CcPushImm(pMachine, ((pMachine->currentExprType = d[Type]) == CHAR) ? LC : LI);
        }
    }
    else if (pMachine->currentToken == '(')
    {
        CcNextToken(pMachine);
        if (pMachine->currentToken == Int || pMachine->currentToken == Char)
        {
            t = (pMachine->currentToken == Int) ? INT : CHAR;
            CcNextToken(pMachine);
            while (pMachine->currentToken == Mul)
            {
                CcNextToken(pMachine);
                t = t + PTR;
            }
            if (pMachine->currentToken == ')')
			{
				CcNextToken(pMachine);
			}
            else
            {
                LogMsg("%d: bad cast", pMachine->lineNum);
                Except(pMachine, CCSTATUS_BAD_CAST);
            }
            CcOnExpression(pMachine, Inc);
            //ExitIfAnythingWrong(pMachine);
            pMachine->currentExprType = t;
        }
        else
        {
            CcOnExpression(pMachine, Assign);
            //ExitIfAnythingWrong(pMachine);
            if (pMachine->currentToken == ')')
			{
				CcNextToken(pMachine);
			}
            else
            {
                LogMsg("%d: close paren expected", pMachine->lineNum);
                Except(pMachine, CCSTATUS_CLOSE_PAREN_EXPECTED);
            }
        }
    }
    else if (pMachine->currentToken == Mul)
    {
        CcNextToken(pMachine);
        CcOnExpression(pMachine, Inc);
        //ExitIfAnythingWrong(pMachine);
        if (pMachine->currentExprType > INT) pMachine->currentExprType = pMachine->currentExprType - PTR;
        else
        {
            LogMsg("%d: bad dereference", pMachine->lineNum);
            Except(pMachine, CCSTATUS_BAD_DEREFERENCE);
        }
        CcPushOpCode(pMachine, (pMachine->currentExprType == CHAR) ? LC : LI);
    }
    else if (pMachine->currentToken == And)
    {
        CcNextToken(pMachine);
        CcOnExpression(pMachine, Inc);
        //ExitIfAnythingWrong(pMachine);
        if (CcGetOpCodeHere(pMachine) == LC || CcGetOpCodeHere(pMachine) == LI)
            CcGoBackOpc(pMachine);
        else
        {
            LogMsg("%d: bad address-of", pMachine->lineNum);
            Except(pMachine, CCSTATUS_BAD_REFERENCE);
        }
        pMachine->currentExprType = pMachine->currentExprType + PTR;
    }
    else if (pMachine->currentToken == '!')
    {
        CcNextToken(pMachine);
        CcOnExpression(pMachine, Inc);
        //ExitIfAnythingWrong(pMachine);
        CcPushOpCode(pMachine, PSH);
        CcPushOpCode(pMachine, IMM);
        CcPushImm(pMachine, 0);
        CcPushOpCode(pMachine, EQ);
        pMachine->currentExprType = INT;
    }
    else if (pMachine->currentToken == '~')
    {
        CcNextToken(pMachine);
        CcOnExpression(pMachine, Inc);
        //ExitIfAnythingWrong(pMachine);
        CcPushOpCode(pMachine, PSH);
        CcPushOpCode(pMachine, IMM);
        CcPushImm(pMachine, -1);
        CcPushOpCode(pMachine, XOR);
        pMachine->currentExprType = INT;
    }
    else if (pMachine->currentToken == Add)
    {
        CcNextToken(pMachine);
        CcOnExpression(pMachine, Inc);
        //ExitIfAnythingWrong(pMachine);
        pMachine->currentExprType = INT;
    }
    else if (pMachine->currentToken == Sub)
    {
        CcNextToken(pMachine);
        CcPushOpCode(pMachine, IMM);
        if (pMachine->currentToken == Num)
        {
            CcPushImm(pMachine, -pMachine->currentTokenValue);
            CcNextToken(pMachine);
        }
        else
        {
            CcPushImm(pMachine, -1);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Inc);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, MUL);
        }
        pMachine->currentExprType = INT;
    }
    else if (pMachine->currentToken == Inc || pMachine->currentToken == Dec)
    {
        t = pMachine->currentToken;
        CcNextToken(pMachine);
        CcOnExpression(pMachine, Inc);
        //ExitIfAnythingWrong(pMachine);
        if (CcGetOpCodeHere(pMachine) == LC)
        {
            CcSetOpCodeHere(pMachine, PSH);
            CcPushOpCode(pMachine, LC);
        }
        else if (CcGetOpCodeHere(pMachine) == LI)
        {
            CcSetOpCodeHere(pMachine, PSH);
            CcPushOpCode(pMachine, LI);
        }
        else
        {
            LogMsg("%d: bad lvalue in pre-increment", pMachine->lineNum);
            Except(pMachine, CCSTATUS_BAD_LVALUE);
        }
        CcPushOpCode(pMachine, PSH);
        CcPushOpCode(pMachine, IMM);
        CcPushImm(pMachine, (pMachine->currentExprType > PTR) ? sizeof(int) : sizeof(char));
        CcPushOpCode(pMachine, (t == Inc) ? ADD : SUB);
        CcPushOpCode(pMachine, (pMachine->currentExprType == CHAR) ? SC : SI);
    }
    else
    {
        LogMsg("%d: bad expression", pMachine->lineNum);
        Except(pMachine, CCSTATUS_BAD_EXPRESSION);
    }

    while (pMachine->currentToken >= lev)   // "precedence climbing" or "Top Down Operator Precedence" method
    {
        t = pMachine->currentExprType;
        if (pMachine->currentToken == Assign)
        {
            CcNextToken(pMachine);
            if (CcGetOpCodeHere(pMachine) == LC || CcGetOpCodeHere(pMachine) == LI) CcSetOpCodeHere(pMachine, PSH);
            else
            {
                LogMsg("%d: bad lvalue in assignment", pMachine->lineNum);
                Except(pMachine, CCSTATUS_BAD_LVALUE);
            }
            CcOnExpression(pMachine, Assign);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, ((pMachine->currentExprType = t) == CHAR) ? SC : SI);
        }
        else if (pMachine->currentToken == Cond)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, BZ);
            d = ++pMachine->pText;
            CcOnExpression(pMachine, Assign);
            //ExitIfAnythingWrong(pMachine);
            if (pMachine->currentToken == ':') CcNextToken(pMachine);
            else
            {
                LogMsg("%d: conditional missing colon", pMachine->lineNum);
                Except(pMachine, CCSTATUS_COND_MISSING_COLON);
            }
            *d = (int)(pMachine->pText + 3);
            CcPushOpCode(pMachine, JMP);
            d = ++pMachine->pText;
            CcOnExpression(pMachine, Cond);
            //ExitIfAnythingWrong(pMachine);
            *d = (int)(pMachine->pText + 1);
        }
        else if (pMachine->currentToken == Lor)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, BNZ);
            CcAdvanceOpc(pMachine);
            d = pMachine->pText;
            CcOnExpression(pMachine, Lan);
            //ExitIfAnythingWrong(pMachine);
            *d = (int)(pMachine->pText + 1);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Lan)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, BZ);
            CcAdvanceOpc(pMachine);
            d = pMachine->pText;
            CcOnExpression(pMachine, Or);
            //ExitIfAnythingWrong(pMachine);
            *d = (int)(pMachine->pText + 1);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Or)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Xor);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, OR);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Xor)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, And);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, XOR);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == And)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Eq);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, AND);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Eq)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Lt);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, EQ);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Ne)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Lt);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, NE);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Lt)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Shl);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, LT);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Gt)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Shl);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, GT);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Le)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Shl);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, LE);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Ge)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Shl);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, GE);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Shl)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Add);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, SHL);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Shr)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Add);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, SHR);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Add)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Mul);
            //ExitIfAnythingWrong(pMachine);
            if ((pMachine->currentExprType = t) > PTR)
            {
                CcPushOpCode(pMachine, PSH);
                CcPushOpCode(pMachine, IMM);
                CcPushImm(pMachine, sizeof(int));
                CcPushOpCode(pMachine, MUL);
            }
            CcPushOpCode(pMachine, ADD);
        }
        else if (pMachine->currentToken == Sub)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Mul);
            //ExitIfAnythingWrong(pMachine);
            if (t > PTR && t == pMachine->currentExprType)
            {
                CcPushOpCode(pMachine, SUB);
                CcPushOpCode(pMachine, PSH);
                CcPushOpCode(pMachine, IMM);
                CcPushImm(pMachine, sizeof(int));
                CcPushOpCode(pMachine, DIV);
                pMachine->currentExprType = INT;
            }
            else if ((pMachine->currentExprType = t) > PTR)
            {
                CcPushOpCode(pMachine, PSH);
                CcPushOpCode(pMachine, IMM);
                CcPushImm(pMachine, sizeof(int));
                CcPushOpCode(pMachine, MUL);
                CcPushOpCode(pMachine, SUB);
            }
            else CcPushOpCode(pMachine, SUB);
        }
        else if (pMachine->currentToken == Mul)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Inc);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, MUL);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Div)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Inc);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, DIV);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Mod)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Inc);
            //ExitIfAnythingWrong(pMachine);
            CcPushOpCode(pMachine, MOD);
            pMachine->currentExprType = INT;
        }
        else if (pMachine->currentToken == Inc || pMachine->currentToken == Dec)
        {
            if (CcGetOpCodeHere(pMachine) == LC)
            {
                CcSetOpCodeHere(pMachine, PSH);
                CcPushOpCode(pMachine, LC);
            }
            else if (CcGetOpCodeHere(pMachine) == LI)
            {
                CcSetOpCodeHere(pMachine, PSH);
                CcPushOpCode(pMachine, LI);
            }
            else
            {
                LogMsg("%d: bad lvalue in post-increment", pMachine->lineNum);
                Except(pMachine, CCSTATUS_BAD_LVALUE);
            }
            CcPushOpCode(pMachine, PSH);
            CcPushOpCode(pMachine, IMM);
            CcPushImm(pMachine, (pMachine->currentExprType > PTR) ? sizeof(int) : sizeof(char));
            CcPushOpCode(pMachine, (pMachine->currentToken == Inc) ? ADD : SUB);
            CcPushOpCode(pMachine, (pMachine->currentExprType == CHAR) ? SC : SI);
            CcPushOpCode(pMachine, PSH);
            CcPushOpCode(pMachine, IMM);
            CcPushImm(pMachine, (pMachine->currentExprType > PTR) ? sizeof(int) : sizeof(char));
            CcPushOpCode(pMachine, (pMachine->currentToken == Inc) ? SUB : ADD);
            CcNextToken(pMachine);
        }
        else if (pMachine->currentToken == Brak)
        {
            CcNextToken(pMachine);
            CcPushOpCode(pMachine, PSH);
            CcOnExpression(pMachine, Assign);
            //ExitIfAnythingWrong(pMachine);
            if (pMachine->currentToken == ']') CcNextToken(pMachine);
            else
            {
                LogMsg("%d: close bracket expected", pMachine->lineNum);
                Except(pMachine, CCSTATUS_CLOSE_BRACK_EXPECTED);
            }
            if (t > PTR)
            {
                CcPushOpCode(pMachine, PSH);
                CcPushOpCode(pMachine, IMM);
                CcPushImm(pMachine, sizeof(int));
                CcPushOpCode(pMachine, MUL);
            }
            else if (t < PTR)
            {
                LogMsg("%d: pointer type expected", pMachine->lineNum);
                Except(pMachine, CCSTATUS_POINTER_TYPE_EXPECTED);
            }
            CcPushOpCode(pMachine, ADD);
            CcPushImm(pMachine, ((pMachine->currentExprType = t - PTR) == CHAR) ? LC : LI);
        }
        else
        {
            LogMsg("%d: compiler error pMachine->currentToken=%d", pMachine->lineNum, pMachine->currentToken);
            Except(pMachine, CCSTATUS_COMPILER_ERROR);
        }
    }
}
void CcStatement(CMachine *pMachine)
{
    int* a, * b;

    if (pMachine->currentToken == If)
    {
        CcNextToken(pMachine);
        if (pMachine->currentToken == '(') CcNextToken(pMachine);
        else
        {
            LogMsg("%d: open paren expected", pMachine->lineNum);
            Except(pMachine, CCSTATUS_OPEN_PAREN_EXPECTED);
        }
        CcOnExpression(pMachine, Assign);
        //ExitIfAnythingWrong(pMachine);
        if (pMachine->currentToken == ')') CcNextToken(pMachine);
        else
        {
            LogMsg("%d: close paren expected", pMachine->lineNum);
            Except(pMachine, CCSTATUS_CLOSE_PAREN_EXPECTED);
        }
        CcPushOpCode(pMachine, BZ);
        CcAdvanceOpc(pMachine);
        b = pMachine->pText;
        CcStatement(pMachine);
        //ExitIfAnythingWrong(pMachine);
        if (pMachine->currentToken == Else)
        {
            *b = (int)(pMachine->pText + 3);
            CcPushOpCode(pMachine, JMP);
            CcAdvanceOpc(pMachine);
            b = pMachine->pText;
            CcNextToken(pMachine);
            CcStatement(pMachine);
            //ExitIfAnythingWrong(pMachine);
        }
        *b = (int)(pMachine->pText + 1);
    }
    else if (pMachine->currentToken == While)
    {
        CcNextToken(pMachine);
        a = pMachine->pText + 1;
        if (pMachine->currentToken == '(') CcNextToken(pMachine);
        else
        {
            LogMsg("%d: open paren expected", pMachine->lineNum);
            Except(pMachine, CCSTATUS_OPEN_PAREN_EXPECTED);
        }
        CcOnExpression(pMachine, Assign);
        //ExitIfAnythingWrong(pMachine);
        if (pMachine->currentToken == ')') CcNextToken(pMachine);
        else
        {
            LogMsg("%d: close paren expected", pMachine->lineNum);
            Except(pMachine, CCSTATUS_CLOSE_PAREN_EXPECTED);
        }
        CcPushOpCode(pMachine, BZ);
        CcAdvanceOpc(pMachine);
        b = pMachine->pText;
        CcStatement(pMachine);
        //ExitIfAnythingWrong(pMachine);
        CcPushOpCode(pMachine, JMP);
        CcPushImm(pMachine, (int)a);
        *b = (int)(pMachine->pText + 1);
    }
    else if (pMachine->currentToken == Return)
    {
        CcNextToken(pMachine);
        if (pMachine->currentToken != ';') CcOnExpression(pMachine, Assign);
        //ExitIfAnythingWrong(pMachine);
        CcPushOpCode(pMachine, LEV);
        if (pMachine->currentToken == ';') CcNextToken(pMachine);
        else
        {
            LogMsg("%d: semicolon expected", pMachine->lineNum);
            Except(pMachine, CCSTATUS_SEMICOLON_EXPECTED);
        }
    }
    else if (pMachine->currentToken == '{')
    {
        CcNextToken(pMachine);
        while (pMachine->currentToken != '}') CcStatement(pMachine);
        //ExitIfAnythingWrong(pMachine);
        CcNextToken(pMachine);
    }
    else if (pMachine->currentToken == ';')
    {
        CcNextToken(pMachine);
    }
    else
    {
        CcOnExpression(pMachine, Assign);
        //ExitIfAnythingWrong(pMachine);
        if (pMachine->currentToken == ';') CcNextToken(pMachine);
        else
        {
            LogMsg("%d: semicolon expected", pMachine->lineNum);
            Except(pMachine, CCSTATUS_SEMICOLON_EXPECTED);
        }
    }
}

bool CcBreakCheck(void)
{
    if (g_currentConsole == &g_debugConsole)
    {
        return(KbGetKeyState(KEY_CONTROL) == KEY_PRESSED && KbGetKeyState(KEY_C) == KEY_PRESSED);
    }

    return 0;
}

void CcOnAllocateSomething(CMachine* pMachine, void* pJustAllocated)
{
    // find a free spot in the current allocations
    for (int i = 0; i < pMachine->g_memoryAllocCount; i++)
    {
        //! NOTE: Is this really necessary? g_memoryAllocCount would be 0 anyway :P
        if (pMachine->g_memoryAllocatedPointers[i] == NULL)
        {
            pMachine->g_memoryAllocatedPointers[i] = pJustAllocated;
            return;
        }
    }
    pMachine->g_memoryAllocatedPointers[pMachine->g_memoryAllocCount++] = pJustAllocated;
}
void CcOnDeallocateSomething(CMachine* pMachine, void* pJustAllocated)
{
    for (int i = 0; i < pMachine->g_memoryAllocCount; i++)
    {
        //! NOTE: Is this really necessary? g_memoryAllocCount would be 0 anyway :P
        if (pMachine->g_memoryAllocatedPointers[i] == pJustAllocated)
        {
            pMachine->g_memoryAllocatedPointers[i] = NULL;
            if (i == pMachine->g_memoryAllocCount - 1) // is this the last element that we've got?
                pMachine->g_memoryAllocCount--;
            return;
        }
    }
}

CCSTATUS CcInitMachine(CMachine* pMachine)
{
    pMachine->g_memoryAllocCount = 0;

    pMachine->printAssembly = 0;
    pMachine->printCycles = 0;

    pMachine->g_pErrorExists = false;

    pMachine->main_poolSize = 256 * 1024; // arbitrary size
    if (!(pMachine->pStartOfSymbols = pMachine->pCurrSymbol = (int*)MmAllocateK(pMachine->main_poolSize)))
    {
        LogMsg("could not MmAllocateK(%d) symbol area", pMachine->main_poolSize);
        return CCSTATUS_NO_MALLOC_POOL_AREA;
    }
    if (!(pMachine->pTextStart = pMachine->pLastText = pMachine->pText = (int*)MmAllocateK(pMachine->main_poolSize)))
    {
        LogMsg("could not MmAllocateK(%d) text area", pMachine->main_poolSize);
        return CCSTATUS_NO_MALLOC_TEXT_AREA;
    }
    if (!(pMachine->pDataStart = pMachine->pData = (char*)MmAllocateK(pMachine->main_poolSize)))
    {
        LogMsg("could not MmAllocateK(%d) data area", pMachine->main_poolSize);
        return CCSTATUS_NO_MALLOC_DATA_AREA;
    }
    if (!(pMachine->main_stackStart = pMachine->main_stackPtr = (int*)MmAllocateK(pMachine->main_poolSize)))
    {
        LogMsg("could not MmAllocateK(%d) stack area", pMachine->main_poolSize);
        return CCSTATUS_NO_MALLOC_STACK_AREA;
    }

    memset(pMachine->pCurrSymbol, 0, pMachine->main_poolSize);
    memset(pMachine->pText, 0, pMachine->main_poolSize);
    memset(pMachine->pData, 0, pMachine->main_poolSize);

    pMachine->pSource =
        "char else enum if int return sizeof while "//keywords

        //available function list
        "open read close LogMsg LogMsgNoCr malloc free memset memcmp Random DrawPixel ExecuteScript ReadChar ReadInt "
        "ClearScreen FillScreen FillRectangle DrawRectangle ShiftScreenY DrawString sprintf MoveCursor Sleep exit "

        //other keywords
        "void main"
        ;

    pMachine->main_tempI = Char;
    while (pMachine->main_tempI <= While)
    {
        CcNextToken(pMachine);
        pMachine->pCurrIdentifier[Tk] = pMachine->main_tempI++;
    } // add keywords to symbol table
    pMachine->main_tempI = OPEN;
    while (pMachine->main_tempI <= EXIT)
    {
        CcNextToken(pMachine);
        pMachine->pCurrIdentifier[Class] = Sys;
        pMachine->pCurrIdentifier[Type] = INT;
        pMachine->pCurrIdentifier[Val] = pMachine->main_tempI++;
    } // add library to symbol table
    CcNextToken(pMachine);
    pMachine->pCurrIdentifier[Tk] = Char; // handle void type
    CcNextToken(pMachine);
    pMachine->main_idMain = pMachine->pCurrIdentifier; // keep track of main

    return 0;
}
CCSTATUS CcCompileCode(CMachine* pMachine, const char* pCode, int length)
{
    pMachine->main_tempI = length;
    if (!length) pMachine->main_tempI = strlen(pCode);
    if (!(pMachine->pLastSource = pMachine->pSource = pMachine->pSourceStart = (char*)MmAllocateK(pMachine->main_tempI+30)))
    {
        LogMsg("could not MmAllocateK(%d) source area", pMachine->main_poolSize);
        return CCSTATUS_NO_MALLOC_SRC_AREA;
    }
    memcpy(pMachine->pSource, pCode, pMachine->main_tempI);
    pMachine->pSource[pMachine->main_tempI] = 0;
    pMachine->pSource[pMachine->main_tempI+1] = 0;
    pMachine->pSource[pMachine->main_tempI+2] = 0;
	
	// Error Handling
	int error = SetJump (pMachine->m_jumpError);
	if (error != 0)
	{
		LogMsg("Got ERROR during compilation: %x", error);
		return error;
	}

    // parse declarations
    pMachine->lineNum = 1;
    CcNextToken(pMachine);
    while (pMachine->currentToken)
    {
        pMachine->main_baseType = INT; // basetype
        if (pMachine->currentToken == Int) CcNextToken(pMachine);
        else if (pMachine->currentToken == Char)
        {
            CcNextToken(pMachine);
            pMachine->main_baseType = CHAR;
        }
        else if (pMachine->currentToken == Enum)
        {
            CcNextToken(pMachine);
            if (pMachine->currentToken != '{') CcNextToken(pMachine);
            if (pMachine->currentToken == '{')
            {
                CcNextToken(pMachine);
                pMachine->main_tempI = 0;
                while (pMachine->currentToken != '}')
                {
                    if (pMachine->currentToken != Id)
                    {
                        LogMsg("%d: bad enum identifier %d", pMachine->lineNum, pMachine->currentToken);
                        LongJump (pMachine->m_jumpError, CCSTATUS_BAD_ENUM_IDENTIFIER);
                    }
                    CcNextToken(pMachine);
                    if (pMachine->currentToken == Assign)
                    {
                        CcNextToken(pMachine);
                        if (pMachine->currentToken != Num)
                        {
                            LogMsg("%d: bad enum initializer", pMachine->lineNum);
                            LongJump(pMachine->m_jumpError, CCSTATUS_BAD_ENUM_INITIALIZER);
                        }
                        pMachine->main_tempI = pMachine->currentTokenValue;
                        CcNextToken(pMachine);
                    }
					
                    pMachine->pCurrIdentifier[Class] = Num;
                    pMachine->pCurrIdentifier[Type] = INT;
                    pMachine->pCurrIdentifier[Val] = pMachine->main_tempI++;
					
                    if (pMachine->currentToken == ',')
						CcNextToken(pMachine);
                }
                CcNextToken(pMachine);
            }
        }
        while (pMachine->currentToken != ';' && pMachine->currentToken != '}')
        {
            pMachine->main_tokenType = pMachine->main_baseType;
            while (pMachine->currentToken == Mul)
            {
                CcNextToken(pMachine);
                pMachine->main_tokenType = pMachine->main_tokenType + PTR;
            }
            if (pMachine->currentToken != Id)
            {
                LogMsg("%d: bad global declaration", pMachine->lineNum);
                LongJump(pMachine->m_jumpError, CCSTATUS_BAD_GLOBAL_DECLARATION);
            }
            if (pMachine->pCurrIdentifier[Class])
            {
                LogMsg("%d: duplicate global definition", pMachine->lineNum);
                LongJump(pMachine->m_jumpError, CCSTATUS_DUP_GLOBAL_DECLARATION);
            }
            CcNextToken(pMachine);
            pMachine->pCurrIdentifier[Type] = pMachine->main_tokenType;
            if (pMachine->currentToken == '(')   // function
            {
                pMachine->pCurrIdentifier[Class] = Fun;
                pMachine->pCurrIdentifier[Val] = (int)(pMachine->pText + 1);
                CcNextToken(pMachine);
                pMachine->main_tempI = 0;
                while (pMachine->currentToken != ')')
                {
                    pMachine->main_tokenType = INT;
                    if (pMachine->currentToken == Int) CcNextToken(pMachine);
                    else if (pMachine->currentToken == Char)
                    {
                        CcNextToken(pMachine);
                        pMachine->main_tokenType = CHAR;
                    }
                    while (pMachine->currentToken == Mul)
                    {
                        CcNextToken(pMachine);
                        pMachine->main_tokenType = pMachine->main_tokenType + PTR;
                    }
                    if (pMachine->currentToken != Id)
                    {
                        LogMsg("%d: bad parameter declaration", pMachine->lineNum);
                        LongJump(pMachine->m_jumpError, CCSTATUS_BAD_PARM_DECLARATION);
                    }
                    if (pMachine->pCurrIdentifier[Class] == Loc)
                    {
                        LogMsg("%d: duplicate parameter definition", pMachine->lineNum);
                        LongJump(pMachine->m_jumpError, CCSTATUS_DUP_PARM_DECLARATION);
                    }
                    pMachine->pCurrIdentifier[HClass] = pMachine->pCurrIdentifier[Class];
                    pMachine->pCurrIdentifier[Class] = Loc;
                    pMachine->pCurrIdentifier[HType] = pMachine->pCurrIdentifier[Type];
                    pMachine->pCurrIdentifier[Type] = pMachine->main_tokenType;
                    pMachine->pCurrIdentifier[HVal] = pMachine->pCurrIdentifier[Val];
                    pMachine->pCurrIdentifier[Val] = pMachine->main_tempI++;
                    CcNextToken(pMachine);
                    if (pMachine->currentToken == ',') CcNextToken(pMachine);
                }
                CcNextToken(pMachine);
                if (pMachine->currentToken != '{')
                {
                    LogMsg("%d: bad function definition", pMachine->lineNum);
                    LongJump(pMachine->m_jumpError, CCSTATUS_BAD_FUNC_DECLARATION);
                }
                pMachine->localVariableOffset = ++pMachine->main_tempI;
                CcNextToken(pMachine);
                while (pMachine->currentToken == Int || pMachine->currentToken == Char)
                {
                    pMachine->main_baseType = (pMachine->currentToken == Int) ? INT : CHAR;
                    CcNextToken(pMachine);
                    while (pMachine->currentToken != ';')
                    {
                        pMachine->main_tokenType = pMachine->main_baseType;
                        while (pMachine->currentToken == Mul)
                        {
                            CcNextToken(pMachine);
                            pMachine->main_tokenType = pMachine->main_tokenType + PTR;
                        }
                        if (pMachine->currentToken != Id)
                        {
                            LogMsg("%d: bad local declaration", pMachine->lineNum);
                            LongJump(pMachine->m_jumpError, CCSTATUS_BAD_LOCAL_DECLARATION);
                        }
                        if (pMachine->pCurrIdentifier[Class] == Loc)
                        {
                            LogMsg("%d: duplicate local definition", pMachine->lineNum);
                            LongJump(pMachine->m_jumpError, CCSTATUS_DUP_LOCAL_DECLARATION);
                        }
                        pMachine->pCurrIdentifier[HClass] = pMachine->pCurrIdentifier[Class];
                        pMachine->pCurrIdentifier[Class] = Loc;
                        pMachine->pCurrIdentifier[HType] = pMachine->pCurrIdentifier[Type];
                        pMachine->pCurrIdentifier[Type] = pMachine->main_tokenType;
                        pMachine->pCurrIdentifier[HVal] = pMachine->pCurrIdentifier[Val];
                        pMachine->pCurrIdentifier[Val] = ++pMachine->main_tempI;
                        CcNextToken(pMachine);
                        if (pMachine->currentToken == ',') CcNextToken(pMachine);
                    }
                    CcNextToken(pMachine);
                }
                CcPushOpCode(pMachine, ENT);
                CcPushImm(pMachine, pMachine->main_tempI - pMachine->localVariableOffset);
                while (pMachine->currentToken != '}')
                {
                    CcStatement(pMachine);

                    if (pMachine->g_pErrorExists)
                    {
                        pMachine->m_halted = true;// goto _cleanup;
                        LongJump(pMachine->m_jumpError, CCSTATUS_ERROR_FOUND);
                    }
                }
                CcPushOpCode(pMachine, LEV);
                pMachine->pCurrIdentifier = pMachine->pCurrSymbol; // unwind symbol table locals
                while (pMachine->pCurrIdentifier[Tk])
                {
                    if (pMachine->pCurrIdentifier[Class] == Loc)
                    {
                        pMachine->pCurrIdentifier[Class] = pMachine->pCurrIdentifier[HClass];
                        pMachine->pCurrIdentifier[Type] = pMachine->pCurrIdentifier[HType];
                        pMachine->pCurrIdentifier[Val] = pMachine->pCurrIdentifier[HVal];
                    }
                    pMachine->pCurrIdentifier = pMachine->pCurrIdentifier + Idsz;
                }
            }
            else
            {
                pMachine->pCurrIdentifier[Class] = Glo;
                pMachine->pCurrIdentifier[Val] = (int)pMachine->pData;
                pMachine->pData += sizeof(int);
            }
            if (pMachine->currentToken == ',') CcNextToken(pMachine);
        }
        CcNextToken(pMachine);
    }
    if (!(pMachine->main_instPtr = (int*)pMachine->main_idMain[Val]))
    {
        LogMsg("Error: main() not defined");
        LongJump(pMachine->m_jumpError, CCSTATUS_MAIN_NOT_DEFINED);
    }
    //if (src) return 0;

    // setup stack
    pMachine->main_basePtr = pMachine->main_stackPtr = (int*)((int)pMachine->main_stackPtr + pMachine->main_poolSize);
    *--pMachine->main_stackPtr = EXIT; // call exit if main returns
    *--pMachine->main_stackPtr = PSH;
    pMachine->main_tempT = pMachine->main_stackPtr;
    *--pMachine->main_stackPtr = 0;
    *--pMachine->main_stackPtr = (int)NULL;
    *--pMachine->main_stackPtr = (int)pMachine->main_tempT;

    pMachine->retnVal = 0;

    // run...
    pMachine->main_cycle = 0;
    return CCSTATUS_SUCCESS;
}
void CcRunMachine(CMachine* pMachine, int cycs_per_run)
{
    while (cycs_per_run != 0)
    {
        cycs_per_run--;
        if (CcBreakCheck())
        {
            LogMsg("Ctrl-C, exit at cycle %d", pMachine->main_cycle);
            pMachine->retnVal = CCSTATUS_CTRL_C;
            pMachine->m_halted = 1;
            return;
            //goto _cleanup;
        }

        pMachine->main_tempI = *pMachine->main_instPtr++;
        ++pMachine->main_cycle;
        if (pMachine->printCycles)
        {
            LogMsgNoCr("%d> %s", pMachine->main_cycle,
                       &"LEA \0IMM \0JMP \0JSR \0BZ  \0BNZ \0ENT \0ADJ \0LEV \0LI  \0LC  \0SI  \0SC  \0PSH \0"
                       "OR  \0XOR \0AND \0EQ  \0NE  \0LT  \0GT  \0LE  \0GE  \0SHL \0SHR \0ADD \0SUB \0MUL \0DIV \0MOD \0"
                       "OPEN\0READ\0CLOS\0PRTF\0PRTN\0MALC\0FREE\0MSET\0MCMP\0RAND\0DRPX\0EXSC\0RDCH\0RDIN\0CLSC\0"
                       "FLSC\0FLRC\0DRRC\0SSCY\0DRST\0SPTF\0MVCR\0SLEP\0EXIT\0"[pMachine->main_tempI * 5]);
            if (pMachine->main_tempI <= ADJ)
            {
                void* ptr = (void*)(*pMachine->main_instPtr);
                LogMsg(" %#x", (int)ptr);
            }
            else LogMsg("");
        }
        if (pMachine->main_tempI == LEA) pMachine->main_theAReg = (int)(pMachine->main_basePtr + *pMachine->main_instPtr++);                             // load local address
        else if (pMachine->main_tempI == IMM) pMachine->main_theAReg = *pMachine->main_instPtr++;                                         // load global address or immediate
        else if (pMachine->main_tempI == JMP) pMachine->main_instPtr = (int*)*pMachine->main_instPtr;                                   // jump
        else if (pMachine->main_tempI == JSR)
        {
            *--pMachine->main_stackPtr = (int)(pMachine->main_instPtr + 1);    // jump to subroutine
            pMachine->main_instPtr = (int*)*pMachine->main_instPtr;
        }
        else if (pMachine->main_tempI == BZ)  pMachine->main_instPtr = pMachine->main_theAReg ? pMachine->main_instPtr + 1 : (int*)*pMachine->main_instPtr;                      // branch if zero
        else if (pMachine->main_tempI == BNZ) pMachine->main_instPtr = pMachine->main_theAReg ? (int*)*pMachine->main_instPtr : pMachine->main_instPtr + 1;                      // branch if not zero
        else if (pMachine->main_tempI == ENT)
        {
            *--pMachine->main_stackPtr = (int)pMachine->main_basePtr;    // enter subroutine
            pMachine->main_basePtr = pMachine->main_stackPtr;
            pMachine->main_stackPtr = pMachine->main_stackPtr - *pMachine->main_instPtr++;
        }
        else if (pMachine->main_tempI == ADJ) pMachine->main_stackPtr = pMachine->main_stackPtr + *pMachine->main_instPtr++;                                   // stack adjust
        else if (pMachine->main_tempI == LEV)
        {
            pMachine->main_stackPtr = pMachine->main_basePtr;    // leave subroutine
            pMachine->main_basePtr = (int*)*pMachine->main_stackPtr++;
            pMachine->main_instPtr = (int*)*pMachine->main_stackPtr++;
        }
        else if (pMachine->main_tempI == LI)  pMachine->main_theAReg = *(int*)pMachine->main_theAReg;                                     // load int
        else if (pMachine->main_tempI == LC)  pMachine->main_theAReg = *(char*)pMachine->main_theAReg;                                    // load char
        else if (pMachine->main_tempI == SI)  *(int*)*pMachine->main_stackPtr++ = pMachine->main_theAReg;                                 // store int
        else if (pMachine->main_tempI == SC)  pMachine->main_theAReg = *(char*)*pMachine->main_stackPtr++ = pMachine->main_theAReg;                            // store char
        else if (pMachine->main_tempI == PSH) *--pMachine->main_stackPtr = pMachine->main_theAReg;                                         // push

        else if (pMachine->main_tempI == OR)  pMachine->main_theAReg = *pMachine->main_stackPtr++ | pMachine->main_theAReg;
        else if (pMachine->main_tempI == XOR) pMachine->main_theAReg = *pMachine->main_stackPtr++ ^ pMachine->main_theAReg;
        else if (pMachine->main_tempI == AND) pMachine->main_theAReg = *pMachine->main_stackPtr++ & pMachine->main_theAReg;
        else if (pMachine->main_tempI == EQ)  pMachine->main_theAReg = *pMachine->main_stackPtr++ == pMachine->main_theAReg;
        else if (pMachine->main_tempI == NE)  pMachine->main_theAReg = *pMachine->main_stackPtr++ != pMachine->main_theAReg;
        else if (pMachine->main_tempI == LT)  pMachine->main_theAReg = *pMachine->main_stackPtr++ < pMachine->main_theAReg;
        else if (pMachine->main_tempI == GT)  pMachine->main_theAReg = *pMachine->main_stackPtr++ > pMachine->main_theAReg;
        else if (pMachine->main_tempI == LE)  pMachine->main_theAReg = *pMachine->main_stackPtr++ <= pMachine->main_theAReg;
        else if (pMachine->main_tempI == GE)  pMachine->main_theAReg = *pMachine->main_stackPtr++ >= pMachine->main_theAReg;
        else if (pMachine->main_tempI == SHL) pMachine->main_theAReg = *pMachine->main_stackPtr++ << pMachine->main_theAReg;
        else if (pMachine->main_tempI == SHR) pMachine->main_theAReg = *pMachine->main_stackPtr++ >> pMachine->main_theAReg;
        else if (pMachine->main_tempI == ADD) pMachine->main_theAReg = *pMachine->main_stackPtr++ + pMachine->main_theAReg;
        else if (pMachine->main_tempI == SUB) pMachine->main_theAReg = *pMachine->main_stackPtr++ - pMachine->main_theAReg;
        else if (pMachine->main_tempI == MUL) pMachine->main_theAReg = *pMachine->main_stackPtr++ * pMachine->main_theAReg;
        else if (pMachine->main_tempI == DIV) pMachine->main_theAReg = *pMachine->main_stackPtr++ / pMachine->main_theAReg;
        else if (pMachine->main_tempI == MOD) pMachine->main_theAReg = *pMachine->main_stackPtr++ % pMachine->main_theAReg;

        else if (pMachine->main_tempI == OPEN) LogMsg("Not supported!");
        else if (pMachine->main_tempI == READ) LogMsg("Not supported!");
        else if (pMachine->main_tempI == CLOS) LogMsg("Not supported!");
        else if (pMachine->main_tempI == PRTF)
        {
            pMachine->main_tempT = pMachine->main_stackPtr + pMachine->main_instPtr[1];
            LogMsg    ((char*)pMachine->main_tempT[-1], pMachine->main_tempT[-2], pMachine->main_tempT[-3], pMachine->main_tempT[-4], pMachine->main_tempT[-5], pMachine->main_tempT[-6]);
        }
        else if (pMachine->main_tempI == PRTN)
        {
            pMachine->main_tempT = pMachine->main_stackPtr + pMachine->main_instPtr[1];
            LogMsgNoCr((char*)pMachine->main_tempT[-1], pMachine->main_tempT[-2], pMachine->main_tempT[-3], pMachine->main_tempT[-4], pMachine->main_tempT[-5], pMachine->main_tempT[-6]);
        }
        else if (pMachine->main_tempI == MALC)
        {
            if (pMachine->g_memoryAllocCount < MAX_ALLOCS - 2)
            {
                pMachine->main_theAReg = (int)MmAllocateK(*pMachine->main_stackPtr);

                if (pMachine->main_theAReg)
                    CcOnAllocateSomething(pMachine, (void*)pMachine->main_theAReg);
            }
            else
            {
                LogMsg("Out of memory");
                pMachine->main_theAReg = (int)NULL;
            }
        }
        else if (pMachine->main_tempI == FREE)
        {
            CcOnDeallocateSomething(pMachine, (void*)*pMachine->main_stackPtr);
            MmFree((void*)*pMachine->main_stackPtr);
        }
        else if (pMachine->main_tempI == MSET) pMachine->main_theAReg = (int)memset((char*)pMachine->main_stackPtr[2], pMachine->main_stackPtr[1], *pMachine->main_stackPtr);
        else if (pMachine->main_tempI == MCMP) pMachine->main_theAReg = memcmp((char*)pMachine->main_stackPtr[2], (char*)pMachine->main_stackPtr[1], *pMachine->main_stackPtr);
        else if (pMachine->main_tempI == RAND)
        {
            int x = *pMachine->main_stackPtr;
            int rng = GetRandom();
            rng = rng & 0xFFFF;//make sure it is unsigned
            if (x) pMachine->main_theAReg = rng % x;
            else pMachine->main_theAReg = rng;
            //LogMsg("[DEBUG] Random(%d) returned %d", x,a);
        }
        else if (pMachine->main_tempI == RDCH)
        {
            while(CoInputBufferEmpty()) hlt;
            pMachine->main_theAReg = CoGetChar();
        }
        else if (pMachine->main_tempI == RDIN)
        {
            /*if (pMachine->m_bHookedConsole)  //Did we hook the console?
            	pMachine->main_theAReg = KeReadIntDebug();
            else
            	pMachine->main_theAReg = -1;*/

            char buffer [11];
            LogMsgNoCr("?");
            CoGetString(buffer, 11);

            int num = atoi (buffer);

            pMachine->main_theAReg = num;
        }
        else if (pMachine->main_tempI == CLSC)
        {
#ifndef _WIN32
            CoClearScreen(g_currentConsole);
            g_currentConsole->curX = 0;
            g_currentConsole->curY = 0;
#endif
        }
        else if (pMachine->main_tempI == MVCR)
        {
#ifndef _WIN32
            g_currentConsole->curX = (int)pMachine->main_stackPtr[1];
            g_currentConsole->curY = (int)pMachine->main_stackPtr[0];
#endif
        }
        else if (pMachine->main_tempI == SLEP)
        {
#ifndef _WIN32
            WaitMS((int)pMachine->main_stackPtr[0]);
#else
            Sleep ((int)pMachine->main_stackPtr[0]);
#endif
        }
        else if (pMachine->main_tempI == DRPX) VidPlotPixel((int)pMachine->main_stackPtr[2], (int)pMachine->main_stackPtr[1], (int)*pMachine->main_stackPtr);
        else if (pMachine->main_tempI == FLRC)
        {
#ifndef _WIN32
            Rectangle r;
            r.left = (int)pMachine->main_stackPtr[4];
            r.top = (int)pMachine->main_stackPtr[3];
            r.right = (int)pMachine->main_stackPtr[2], r.bottom = (int)pMachine->main_stackPtr[1];
            uint32_t color = (uint32_t)pMachine->main_stackPtr[0];
            VidFillRectangle(color, r);
#endif
        }
        else if (pMachine->main_tempI == DRRC)
        {
#ifndef _WIN32
            Rectangle r;
            r.left = (int)pMachine->main_stackPtr[4];
            r.top = (int)pMachine->main_stackPtr[3];
            r.right = (int)pMachine->main_stackPtr[2], r.bottom = (int)pMachine->main_stackPtr[1];
            uint32_t color = (uint32_t)pMachine->main_stackPtr[0];
            VidDrawRectangle(color, r);
#endif
        }
        else if (pMachine->main_tempI == FLSC)
        {
            uint32_t color = (int)pMachine->main_stackPtr[0];
            VidFillScreen(color);
        }
        else if (pMachine->main_tempI == SSCY)
        {
            //int howMuch = (int)pMachine->main_stackPtr[0];
            //VidShiftScreen(howMuch);
        }
        else if (pMachine->main_tempI == DRST)
        {
            int color = (int)pMachine->main_stackPtr[3], px = (int)pMachine->main_stackPtr[2], py = (int)pMachine->main_stackPtr[1];
            const char* pText = (char*)pMachine->main_stackPtr[0];
            VidTextOut(pText, px, py, color, TRANSPARENT);
        }
        else if (pMachine->main_tempI == SPTF)
        {
            pMachine->main_tempT = pMachine->main_stackPtr + pMachine->main_instPtr[1];
            char* pBuf = (char*)pMachine->main_tempT[-1], * pFormat = (char*)pMachine->main_tempT[-2];

            sprintf(pBuf, pFormat, pMachine->main_tempT[-3], pMachine->main_tempT[-4], pMachine->main_tempT[-5], pMachine->main_tempT[-6]);
        }
        else if (pMachine->main_tempI == EXSC)
        {
            /*const char* pFileName = (const char*)*sp;

            uint8_t* data = NULL; int size = 0;
            int errorCode = FileLoad(g_curDir, pFileName, &size, &data);
            if (errorCode != Success) {
                LogMsg("%d, file operation failed: %s", cycle, GetErrorString(errorCode));
            } else {
                //CcRunCCode((const char*)data);
            }
            if (data) FreeMem(data);*/
            LogMsg("Not supported yet!");
        }
        else if (pMachine->main_tempI == EXIT)
        {
            if (pMachine->printCycles)
                LogMsg("exit(%d) cycle = %d", *pMachine->main_stackPtr, pMachine->main_cycle);

            pMachine->retnVal = *pMachine->main_stackPtr;
            pMachine->m_halted = 1;
            return;
            //goto _cleanup;
        }
        else
        {
            LogMsg("ERROR: Unknown instruction %d! cycle = %d", pMachine->main_tempI, pMachine->main_cycle);
            pMachine->retnVal = CCSTATUS_UNKNOWN_INSTRUCTION;
            pMachine->m_halted = 1;
            return;//goto _cleanup;
        }
    }
}
void CcKillMachine(CMachine* pMachine)
{
    bool printedMemleakAlert = true;
    int memLeakCount = 0;
    for (int i = 0; i < pMachine->g_memoryAllocCount; i++)
    {
        if (pMachine->g_memoryAllocatedPointers[i] != NULL)
        {
            if (printedMemleakAlert)
            {
                LogMsg("WARNING: Memory leak detected, automatically freed.");
                printedMemleakAlert = false;
            }
            memLeakCount++;
            MmFree(pMachine->g_memoryAllocatedPointers[i]);
            pMachine->g_memoryAllocatedPointers[i] = NULL;
        }
    }
    if (memLeakCount)
        LogMsg("Unfreed block count: %d", memLeakCount);

    if (pMachine->pTextStart)          MmFree(pMachine->pTextStart);
    if (pMachine->pDataStart)          MmFree(pMachine->pDataStart);
    if (pMachine->pSourceStart)        MmFree(pMachine->pSourceStart);
    if (pMachine->main_stackStart)     MmFree(pMachine->main_stackStart);
    if (pMachine->pStartOfIdentifiers) MmFree(pMachine->pStartOfIdentifiers);
    if (pMachine->pStartOfSymbols)     MmFree(pMachine->pStartOfSymbols);
}

// for backwards compatibility with CcRunCCode
CCSTATUS CcRunCCode(const char* pCode, int length)
{
    CMachine* pMachine;
    pMachine = (CMachine*)MmAllocateK(sizeof(CMachine));
    if (!pMachine)
    {
        LogMsg("Wtf?! (16)");
        return 1;
    }
    memset(pMachine, 0, sizeof(CMachine));

    pMachine->m_bHookedConsole = true;

    //int c = CcRunCCode(pMachine, lol);
    int state;
    state = CcInitMachine(pMachine);
    if (state != 0)
    {
        CcKillMachine(pMachine);
        return state;
    }
    state = CcCompileCode(pMachine, pCode, length);
    if (state != 0)
    {
        CcKillMachine(pMachine);
        return state;
    }

    while (!pMachine->m_halted)
        CcRunMachine(pMachine, 1000);

    int rv = pMachine->retnVal;

    CcKillMachine(pMachine);
    MmFree(pMachine);
    return rv;
}

int random_seed_thing[] = {
	-1244442060, -1485266709, 234119953, 
	-201730870, -1913591016, -1591339077, 
	2041649642, -886342341, 711555730, 
	40192431, 1517024340, -576781024, 
	-891284503, 1587790946, 1566147323, 
	2010441970, 1320486880, 66264233, 
	737611192, -1119832783, -865761768, 
	-420460857, 1682411333, -477284012, 
	-1116287873, -1211736837, 963092786,
	1730979436, 1505625485, 873340739, 
	1627747955, 1683315894, -1257054734, 
	1161594573, -1427794708, 648449375, 
	1030832345, -1089406083, 1545559989, 
	1407925421, -905406156, -1280911754, 
	-2001308758, 227385715, 520740534, 
	-1282033083, 1284654531, 1238828013, 
	148302191, -1656360560, -1139028545, 
	-704504193, 1367565640, -1213605929, 
	289692537, 2057687908, 684752482, 
	1301104665, 1933084912, 1134552268, 
	-1865611277, -757445071, -827902808, 
	-439546422, 1978388539, -997231600, 
	-1912768246, -922198660, 467538317, 
	-395047754, 146974967, 225791951, 
	-1521041638, 784853764, -586969394, 
	-465017921, 1258064203, 1500716775, 
	-250934705, -1117364667, -293202430, 
	-595719623, 376260241, -535874599, 
	-1939247052, 839033960, 1069014255, 
	-282157071, -22946408, -1178535916, 
	-537160644, -105327143, -316195415, 
	-571571219, 353753037, 1463803685, 
	1161010901, -404201378, 391199413, 
	405473472, 742889183, 452611239, 
	-1790628764, -1572764945, 917344354, 
	-1045176342, 1083902565, 1454280688, 
	-1995648000, 1037588263, 2145497403, 
	-1112202532, -1856081951, 938534213, 
	-1691760356, 2109781738, 1820109821, 
	1988028837, -2101599530, -926145485, 
	1852560250, 1917576092, -1377736232, 
	216878403, -1405586386, -544290439, 
	-382424683, -1477587186, -1488023165, 
	-1360589093, 25592369, 1695536505, 
	1821713859, -690132140, 1967963236, 
	1833534930, 74127397, -1987912089, 
	-586108586, -868646236, 1034085250, 
	527296915, 1954505506, 2083589286, 
	1608417972, 1461209721, 1669506543, 
	-140128717, 2089251682, -476684220, 
	1361481586, -543753628, 1954572638, 
	-1308070260, 1671930579, -922485963, 
	2047578920, -456758938, 1635678287, 
	-92864401, -457923115, 1647288373, 
	-852311725, 1513449752, 1503502780, 
	-98945231, -1537511900, -79182046, 
	635382674, 1144074041, -1850919743, 
	-2087056151, -1780458811, -582321540, 
	-1488230638, 2032974544, -1888292665, 
	205171085, 1986540608, -1362867570, 
	-358549401, -432582257, 2083465592, 
	440402956, 1274505014, -212283123, 
	865327044, -2051882447, -1521574964, 
	219506962, 2117666163, -45436631, 
	1947688981, -1094014751, -1712545352, 
	17866106, -1716024722, 1073364778, 
	625435084, -974600754, 505280162, 
	397970076, 643236003, 1046854828, 
	-944971290, -1297255861, -683254942, 
	721663995, 323535860, 1313747580, 
	-1802955617, -537520271, 1933763889, 
	1463929564, 268181342, -999074919, 
	-1399420127, 523583817, -844122414, 
	-1128805346, -1243791916, 593274684, 
	-82140258, -130641455, 2026672404, 
	841707801, -1597038831, 1654379730, 
	-1514952243, 400358679, 673293266, 
	839530185, 1371387967, 1469106392, 
	1646045314, 2112649705, -1727683438, 
	-555809424, 205081272, 748992652, 
	858137072, 1873953998, -884024544, 
	1336521232, -1480354168, 1238715379, 
	2009642630, 284719651, 1292482073, 
	1424572383, 971818364, -2069392610, 
	-1551855738, -1381069134, 1521291482, 
	-195336867, 
}, random_seed_thing2;

// basic garbage rand():
int GetRandom()
{
	//read the tsc:
	uint32_t hi, lo;
	GetTimeStampCounter(&hi, &lo);
	//combine the high and low numbers:
	
	hi ^= lo;
	
	//then mask it out so it wont look obvious:
	hi ^= 0xe671c4b4;
	
	//more masking
	hi ^= random_seed_thing[random_seed_thing2++];
	if (random_seed_thing2 >= (int)ARRAY_COUNT(random_seed_thing))
		random_seed_thing2 = 0;
	
	//then make it positive:
	hi &= 2147483647;
	
	//lastly, return.
	return (int) hi;
}