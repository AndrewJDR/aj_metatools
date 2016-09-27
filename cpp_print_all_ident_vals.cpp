/*
By Andrew Johnson

Takes a line of c++ code on stdin then re-outputs it, followed by a cout line
that prints all of the idenifiers from that line, and their values.

This can be useful if you see a line and you just want to "print all the stuff"
on that line without needing to do tedious typing or whip out the debugger.

=== Example ===
Input (you want to printf a, b and c, here!):
int a = b + c;
Output:
int a = b + c; std::cout << "int a = b + c; //" << " a:" << a << " b:" << b << " c:" << c << "\n";
The output from the cout statement would then look something like:
int a = b + c; // a:10 b:6 c:4
It also tries to handle more complex cases as best it can, such as (input):
int i = someArr[nestedArr[k + 1]];
Output:
int i = someArr[nestedArr[k + 1]]; std::cout << "int i = someArr[nestedArr[k + 1]]; //" << " i:" << i << " k:" << k << " nestedArr[k+1]:" << nestedArr[k+1] << " someArr[nestedArr[k+1]]:" << someArr[nestedArr[k+1]] << "\n";
===============

I then bind this in vim as follows:
function! InsertPrintIdents()
    let linelen = strlen(getline(".")) + 1
    exec ".!~/bin/cpp_print_all_ident_vals"
    if strlen(getline(".")) > linelen
        call cursor( line('.'), linelen)
    endif
endfunction
map ,t :silent :call InsertPrintIdents()<Return>

Hitting ,t on a line that already has "std::cout <<" will remove the printout code.

I don't usually use iostream, but because this just uses the lexer and I don't
know the datatypes, I'm not left with much of an option that I know of, short
of parsing the whole file to figure out data types. I may actually do that some
day, but this will be sufficient for now.
*/

#include "4cpp/4cpp_lexer.h"
#include "4cpp/4coder_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>

std::map <std::string, bool> printedIdents;  // I know, I know... but this is quick-n-dirty.

// Prints the identifier if it hasn't yet been printed
int printIdent(String *identStr, String *srcLine, int identAccumStartPos)
{
    if(identStr->size == 0)
    {
        return 0;
    }

    // Only print the identifier if this is our first encounter with it.
    if(printedIdents.find(identStr->str) == printedIdents.end())
    {
        printf(" << \" %.*s:\" << %.*s", identStr->size, identStr->str, identStr->size, identStr->str);
        printedIdents[identStr->str] = true;
    }
}

int findAndPrintIdentifiers(String srcLine, Cpp_Token_Array *tokens, int32_t startTokenIdx, Cpp_Token_Type endTokenType)
{
    Cpp_Token_Type lookForClosing = 0;
    int32_t identAccumStartPos = 0;
    char tmpdata[1024];
    String identAccumulator = make_string_cap(tmpdata, 0, sizeof(tmpdata));
    int32_t subExprCount = 0;

    for (int i = startTokenIdx; i < tokens->count; ++i)
    {
        Cpp_Token token = tokens->tokens[i];

        String thisToken = substr(srcLine, token.start, token.size);
        switch(token.type)
        {
            case CPP_TOKEN_IDENTIFIER:
                if(identAccumulator.size == 0)
                {
                    identAccumStartPos = token.start;
                }
                append_checked_ss(&identAccumulator, thisToken);
                break;

            case CPP_TOKEN_PARENTHESE_OPEN:
                identAccumulator.size = 0;
                break;

            case CPP_TOKEN_BRACKET_OPEN:
                if(identAccumulator.size) append_checked_ss(&identAccumulator, thisToken);
                if(subExprCount == 0)
                {
                    lookForClosing = CPP_TOKEN_BRACKET_CLOSE;
                    findAndPrintIdentifiers(srcLine, tokens, i+1, lookForClosing);
                }

                if(lookForClosing == CPP_TOKEN_BRACKET_CLOSE)
                {
                    subExprCount++;
                }
                break;

            case CPP_TOKEN_BRACKET_CLOSE:
                if(lookForClosing == CPP_TOKEN_BRACKET_CLOSE)
                {
                    if(identAccumulator.size) append_checked_ss(&identAccumulator, thisToken);

                    subExprCount--;
                    if(subExprCount == 0)
                    {
                        lookForClosing = 0;
                    }
                }
                else if(endTokenType == CPP_TOKEN_BRACKET_CLOSE)
                {
                    if(identAccumulator.size>0)
                    {
                        printIdent(&identAccumulator, &srcLine, identAccumStartPos);
                        identAccumulator.size = 0;
                    }
                    return 0;
                }
                break;

            case CPP_TOKEN_ARROW:
            case CPP_TOKEN_DOT:
            case CPP_TOKEN_SCOPE:
                if(identAccumulator.size) append_checked_ss(&identAccumulator, thisToken);
                break;

            default:
                if(lookForClosing != 0)
                {
                    if(identAccumulator.size) append_checked_ss(&identAccumulator, thisToken);
                }
                else if(identAccumulator.size>0)
                {
                    printIdent(&identAccumulator, &srcLine, identAccumStartPos);
                    identAccumulator.size = 0;
                }
                break;
        }
        //printf("type: %s token: %.*s\n", Cpp_Token_TypeName[token.type], thisToken.size, thisToken.str);
    }
}

int main(int argc, char **argv)
{
    char data[32 * 1024];
    int size = sizeof(data);
    fread(data, 1, size, stdin);
    String srcLine = make_string_slowly(data);

    Cpp_Token_Array tokens = cpp_make_token_array(100);

    cpp_lex_file(data, size, &tokens);

    char * stdOutChar = strstr(data, " std::cout");
    if(stdOutChar)
    {
        *stdOutChar = 0;
        printf("%s\n", data);
        return 0;
    }

    String choppedLine = chop_whitespace(srcLine);
    String choppedTrimmedLine = skip_whitespace(choppedLine);

    printf("%.*s std::cout << \"%.*s //\"",
           choppedLine.size,
           choppedLine.str,
           choppedTrimmedLine.size,
           choppedTrimmedLine.str);

    findAndPrintIdentifiers(srcLine, &tokens, 0, 0);

    printf(" << \"\\n\";");

    cpp_free_token_array(tokens);

    return 0;
}
