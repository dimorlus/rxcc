#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <stdarg.h>
#include "RegExpClass.h"

#pragma warning(disable: 4996) // Disable deprecated function warnings


#define BUFSIZE         4096
#define ARGSIZE         128
#define MAXSTRLEN      1024

//---------------------------------------------------------------------------
//https://bbs.archlinux.org/viewtopic.php?id=31087
//scanf
//int main() {
//  char* m = (char*) malloc(sizeof(int)*2 + sizeof(char*)); /* prepare enough memory*/
//  void* bm = m; /* copies the pointer */
//  char* string = "I am a string!!"; /* an example string */
//
//  (*(int*)m) = 10; /*puts the first value */
//  m += sizeof(int); /* move forward the pointer to the next element */
//
//  (*(char**)m) = string; /* puts the next value */
//  m += sizeof(char*); /* move forward again*/
//
//  (*(int*)m) = 20; /* puts the third element */
//  m += sizeof(int); /* unneeded, but here for clarity. */
//
//  vprintf("%d %s %d\n", bm); /* the deep magic starts here...*/
//  free(bm);
//}
//---------------------------------------------------------------------------

#define _DYNAMIC_
//sscanf test utility function 
int ssctst (char *dst, const char *sfmt, const char *str)
{
 char c = '\0';
 char cc = '\0';
 int ii = 0;
 int n = 0;
 int pfsize = 0;
 bool percent, open;
 enum ftypes
 {
  tNone, tChar, tShort, tInt, tLong, ti64, tFloat, tDouble, tExt, tString
 } fmt;

#ifdef _DYNAMIC_
  void **arglist = NULL;
  char *typelist = NULL;
  char *fmtlist = NULL;
  char** pstr = NULL;
#else //_DYNAMIC_
 void* arglist[ARGSIZE] = { NULL };
 char typelist[ARGSIZE] = { 0 };
 char fmtlist[ARGSIZE] = { 0 };
 char* pstr[ARGSIZE] = { NULL };
 memset(arglist, 0, sizeof(arglist));
 memset(typelist, 0, sizeof(typelist));
 memset(fmtlist, 0, sizeof(fmtlist));
 memset(pstr, 0, sizeof(pstr));
#endif //_DYNAMIC_

#ifdef _DYNAMIC_
 //make here 2 pass for count args and allocate memory
 for(int pass=0; pass<2; pass++)
#endif //_DYNAMIC_
 {
#ifdef _DYNAMIC_
  if (pass == 1)
   {
       arglist = (void**)malloc((n + 1) * sizeof(void*));
       typelist = (char*)malloc((n + 1) * sizeof(char));
       fmtlist = (char*)malloc((n + 1) * sizeof(char));
       pstr = (char**)malloc((n + 1) * sizeof(char*));
       if (arglist) memset(arglist, 0, (n + 1) * sizeof(void*));
       else return -1;
       if (typelist) memset(typelist, 0, (n + 1) * sizeof(char));
       else return -1;
       if (fmtlist) memset(fmtlist, 0, (n + 1) * sizeof(char));
       else return -1;
       if (pstr) memset(pstr, 0, (n + 1) * sizeof(char*));
       else return -1;
       c = cc = '\0';
       ii = n = pfsize = 0;
   }
#endif //_DYNAMIC_
 do
  {
   open = false;
   percent = false;
   fmt = tNone;
   do
	{
	 cc = c;
	 c = sfmt[ii++];
	 if (open)
	  {
	   if ((c == ']'))
		{
		 fmt = tString;
		 c = 's';
		 open = false;
		 break;
		}
	   else continue;
	  }
	 if (percent)
	  {
	   if (c == '%')
		{
		 percent = false;
		 continue;
		}
	   else
	   if (c == '*')
		{
		 fmt = tNone;
		 percent = false;
		 continue;
		}
	   else
	   if ((c == 'f') || (c == 'e') || (c == 'E') || (c == 'g') || (c == 'G'))
		{
		 fmt = tFloat;
		 if (cc == 'l') fmt = tDouble;
		 else
		 if (cc == 'L') fmt = tExt;
		 break;
		}
	   else
	   if ((c == 'd') || (c == 'i') || (c == 'u') ||
	       (c == 'x') || (c == 'X') || (c == 'o'))
		{
		 fmt = tInt;
		 if (cc == 'L') fmt = ti64;
		 else 
                 if (cc == 'l')  fmt = tLong;
		 else 
                 if (cc == 'h')  fmt = tShort;
		 break;
		}
	   else
	   if ((c == 'D') || (c == 'I') || (c == 'U') || (c == 'O'))
		{
		 fmt = tLong;
		 c = tolower(c);
		 break;
		}
	   else
	   if (c == 'c')
		{
		 //fmt = tChar;
		 fmt = tString;
		 break;
		}
	   else
	   if (c == 's')
		{
		 fmt = tString;
		 break;
		}
	   else
	   if (c == '[')
		{
		 open = true;
		 continue;
		}
	  }
	 else
	 if (c == '%')
	  {
	   percent = true;
	   continue;
	  }
	}
   while (c);
   if (c && percent && fmt)
   {
#ifdef _DYNAMIC_
	       if (pass == 1)
#endif //_DYNAMIC_
           {
	   typelist[n] = fmt;
	   fmtlist[n] = c;
               pstr[n] = NULL;
	   switch (fmt)
	   {
	   case tChar:
		   arglist[n] = (void*)malloc(sizeof(char));
		   *(char*)arglist[n] = '\0';
		   pfsize += sizeof(char);
		   break;
	   case tShort:
		   arglist[n] = (void*)malloc(sizeof(short));
		   *(short*)arglist[n] = 0;
		   pfsize += sizeof(short);
		   break;
	   case tInt:
		   arglist[n] = (void*)malloc(sizeof(int));
		   *(int*)arglist[n] = 0;
		   pfsize += sizeof(int);
		   break;
	   case tLong:
		   arglist[n] = (void*)malloc(sizeof(long));
		   *(long*)arglist[n] = 0;
		   pfsize += sizeof(long);
		   break;
	   case ti64:
		   arglist[n] = (void*)malloc(sizeof(__int64));
		   *(__int64*)arglist[n] = 0;
		   pfsize += sizeof(__int64);
		   break;
	   case tFloat:
		   arglist[n] = (void*)malloc(sizeof(float));
		   *(float*)arglist[n] = 0.0;
		   pfsize += sizeof(float);
		   break;
	   case tDouble:
		   arglist[n] = (void*)malloc(sizeof(double));
		   *(double*)arglist[n] = 0.0;
		   pfsize += sizeof(double);
		   break;
	   case tExt:
		   arglist[n] = (void*)malloc(sizeof(long double));
		   *(long double*)arglist[n] = 0.0;
		   pfsize += sizeof(long double);
		   break;
	   case tString:
                   char* cp = (char*)malloc(BUFSIZE);
		   arglist[n] = (void*)malloc(sizeof(char*));
                   *(char**)arglist[n] = cp;
                   pstr[n] = cp;
		   memset(*(char**)arglist[n], 0, BUFSIZE);
		   pfsize += sizeof(char*);
		   break;
	   }
           }
	   n++;
      }
    } while (c && (n < BUFSIZE));
   }//end for pass

    arglist[n] = (void*)malloc(sizeof(void*));

    int nn = vsscanf(str, sfmt, (char*)arglist);

    char* fdest = dst;
    for (int i = 0; (i < n) && arglist[i]; i++)
    {
        switch (typelist[i])
        {
        case tChar:
        {
            char ci = *(char*)arglist[i];
            char fmt[] = { "%c, \0" };
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, ci);
        }
        break;
        case tShort:
        {
            short si = *(short*)arglist[i];
            char fmt[] = { "%hd, \0" };
            fmt[2] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, si);
        }
        break;
        case tInt:
        {
            int ii = *(int*)arglist[i];
            char fmt[] = { "%d, \0" };
            fmt[1] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, ii);
        }
        break;
        case tLong:
        {
            long li = *(long*)arglist[i];
            char fmt[] = { "%ld, \0" };
            fmt[2] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, li);
        }
        break;
        case ti64:
        {
            __int64 Li = *(__int64*)arglist[i];
            char fmt[] = { "%Ld, \0" };
            fmt[2] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, Li);
        }
        break;
        case tFloat:
        {
            float ff = *(float*)arglist[i];
            char fmt[] = { "%f, \0" };
            fmt[1] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, ff);
        }
        break;
        case tDouble:
        {
            double df = *(double*)arglist[i];
            char fmt[] = { "%lf, \0" };
            fmt[2] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, df);
        }
        break;
        case tExt:
        {
            long double lf = *(long double*)arglist[i];
            char fmt[] = { "%Lf, \0" };
            fmt[2] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, lf);
        }
        break;
        case tString:
        {
            char* ss = (char*)arglist[i];
            char fmt[] = { "%s, \0" };
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, ss);
        }
        break;
        }
    }
    if (fdest > dst + 2) fdest[-2] = '\0'; //supress last ", "

#ifdef _DYNAMIC_
    for (int i = 0; i < n; i++)
#else //_DYNAMIC_
    for (int i = 0; i < ARGSIZE; i++)
#endif //_DYNAMIC_
    {
        if (pstr[i])
    {
            free(pstr[i]);
            pstr[i] = NULL;
        }
        if (arglist[i])
        {
            free(arglist[i]);
            arglist[i] = NULL;
        }
    }
#ifdef _DYNAMIC_
	 if (arglist) free(arglist);
     if (typelist) free(typelist);
     if (fmtlist) free(fmtlist);
     if (pstr) free(pstr);
#endif //_DYNAMIC_   
    return nn;
}
//---------------------------------------------------------------------------
//sprintf test utility function
void sprtst(char* dst, char* sfmt, char* str)
{
    char pfmt[BUFSIZE];
    char pstr[BUFSIZE];
    enum ftypes {
        tNone, tShort, tInt, tLong, tInt64,
        tFloat, tDouble, tLongDouble, tString, tPtr
    } fmt;
    char c = '\0', cc = '\0';
    int i;
    bool flag;

    char* pdst = dst; // Новый указатель для отслеживания позиции в dst

    do
    {
        i = 0; 
        flag = false; 
        fmt = tNone;
        do
        {
            cc = c;
            c = pfmt[i++] = *sfmt++;
            pfmt[i] = '\0';
            if (c == '\0')  break;
            if (flag)
            {
                if (c == '%') 
                 { 
                    flag = false; 
                    continue; 
                 }
                else
                if ((c == 'f') || (c == 'e') || (c == 'E') || (c == 'g') || (c == 'G'))
                {
                    if (cc == 'l') fmt = tDouble;
                    else
                    if (cc == 'L') fmt = tLongDouble;
                    else fmt = tFloat;
                    break;
                }
                else
                if ((c == 'd') || (c == 'i') || (c == 'u') ||
                    (c == 'x') || (c == 'X') || (c == 'o') || (c == 'c'))
                {
                    if (cc == 'l') fmt = tLong;
                    else
                    if (cc == 'h') fmt = tShort;
                    else
                    if (cc == 'L') fmt = tInt64;
                    else fmt = tInt;
                    break;
                }
                else
                if ((c == 'n') || (c == 'p')) 
                 { 
                    fmt = tPtr; 
                    break; 
                 }
                else
                if (c == 's') 
                 { 
                    fmt = tString; 
                    break; 
                 }
                else continue;
            }
            else
            if (c == '%') 
             { 
                flag = true; 
                continue; 
             }
        } while (c && (i < BUFSIZE));
        if (!c) sfmt--; //poit to '\0'
        i = 0; flag = false;
        do
        {
            c = *str++;
            if (!flag && (c == '"')) 
            { 
                flag = true; 
                continue; 
            }
            if (flag && (c == '"')) 
            { 
                flag = false; 
                continue; 
            }
            if (!flag && ((c == ',') || (c == '\0'))) //add asterix
            {
                i = pstr[i] = '\0';
                switch (fmt)
                {
                case tNone:
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt);
                    //pdst += sprintf(pdst, pfmt);
                    break;
                case tPtr:
                    strcpy_s(pdst, BUFSIZE - (pdst - dst), pfmt);
                    pdst += strlen(pfmt);
                    break;
                case tInt:
                {
                    int l = atol(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, l);
                }
                break;
                case tLong:
                {
                    long l = atol(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, l);
                }
                break;
                case tInt64:
                {
                    __int64 l = _atoi64(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, l);
                }
                break;
                case tShort:
                {
                    short l = (short)atol(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, l);
                }
                break;
                case tFloat:
                {
                    float d = (float)atof(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, d);
                }
                break;
                case tDouble:
                {
                    double d = (double)atof(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, d);
                }
                break;
                case tLongDouble:
                {
                    long double d = (long double)atof(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, d);
                }
                break;
                case tString:
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, pstr);
                    break;
                }
                break;
            }
            else pstr[i++] = c;
        } while (c && (i < BUFSIZE));
    } while (*sfmt && (i < BUFSIZE));
}

//---------------------------------------------------------------------------



// Split slashes ('/') divided string to <num> fields. Return fields count.
int nsplit(int num, char* str, ...)
{
 char c, cc;
 int i = 0;
 int n = 0;
 char **ostr = &str; //ostr[1] point to first <...> argument
 do
  {
   c = str[i++];
   if (c) cc = str[i];  // next character
   if ((c == '\n') || (c == '\r')) // ignore
    {
     if (cc) i++;
     continue;
    }
   if (c == '\t') c = ' '; // change
   if (c == '/')
    {
     if (cc == '/') i++; // screaning
     else
      {
       n++;
       continue;
      }
    }
   if (n && ostr[n])
    {
     *ostr[n]++ = c;
     *ostr[n] = '\0';
    }
  }
 while ((c)&&(n<=num));
 for(i=n; i <= num; i++) if (ostr[n]) *ostr[n] = '\0';
 return n;
}

const char help1[] = {
    "This utility performs search and replace operations on text strings\n"
    "using regular expressions.\n"
    "\n"
    "Usage: rxcc [-r] /source string/regsearch/regreplace\n"
    "OR\n"
    "rxcc[-p] /format string/data\n"
    "OR\n"
    "rxcc[-s] /format string/data\n"
	"\n"
    "-p  printf test.\n"
    "-s  scanf test.\n"
    "-r  regular expression test.\n"
	"-d  enable debug output.\n"
	"-h  display this help and exit.\n"
    "\n"
};

const char help2[] = {
    "Regular expressions are characters that customize a search string.\n"
    "The product recognizes these regular expressions:\n"
    "\n"
    "Search expression:\n"
    "\n"
    "^       A circumflex at the start of the string matches the start of a line.\n"
    "$       A dollar sign at the end of the expression matches the end of a line.\n"
    ".       A period matches any character.\n"
    "*       An asterisk after a string matches any number of occurrences of that\n"
    "        string followed by any characters, including zero characters.\n"
    "        For example, bo* matches bot, bo and boo but not b.\n"
    "+       A plus sign after a string matches any number of occurrences of that\n"
    "        string followed by any characters except zero characters.\n"
    "        For example, bo+ matches boo, and booo, but not bo or be.\n"
    "?       A question mark makes the preceding item optional. Greedy, so the\n"
    "        optional item is included in the match if possible.\n"
    "        For example, abc? matches ab or abc.\n"
    "[ ]     Characters in brackets match any one character that appears in the\n"
    "        brackets, but no others. For example [bot] matches b, o, or t.\n"
    "[^]     A circumflex at the start of the string in brackets means NOT.\n"
    "        Hence, [^bot] matches any characters except b, o, or t.\n"
    "[-]     A hyphen within the brackets signifies a range of characters.\n"
    "        For example, [b-o] matches any character from b through o.\n"
    "        For example, [a-z,A-Z,!,$,%%,_,#,0-9]+\n"
    "( )     Braces group characters or expressions. Groups can be nested, with\n"
    "        a maximum number of 16 groups in a single pattern. For the Replace\n"
    "        operation, the groups are referred to by a backslash and a number\n"
    "        according to the position in the \"Text to find\" expression, beginning\n"
    "        with 0.\n"
    "        For example, given the text to find and replacement strings,\n"
    "        Find: ([0-9])([a-c]*), Replace: NUM\\1, the string 3abcabc is changed\n"
    "        to NUMabcabc.\n"
    "|       A vertical bar matches either expression on either side of the\n"
    "        vertical bar.\n"
    "        For example, bar|car will match either bar or car.\n"
    "\\       A backslash before a wildcard character tells the Code editor to\n"
    "        treat that\n"
    "        character literally, not as a wildcard. For example, \\^ matches ^ and\n"
    "        does not look for the start of a line.\n"
    "\\esc    Escape characters.\n"
    "\n"
    "Replace expression:\n"
    "\n"
    "\\0..\\9  Recalls stored substring from matched pattern ()'s.\n"
    "\\A..\\F  Recalls stored substring from matched pattern ()'s from 10 to 16.\n"
    "\\esc    Escape characters.\n"
    "&       Recalls entire matched pattern at replace.\n"
    "\n"
    "Escape Characters:\n"
    "\n"
    "Sequence        Description             DEC     HEX\n"
    "\\b              Backspace               8       0x08\n"
    "\\t              (Horizontal) Tab        9       0x09\n"
    "\\n              New Line Feed (LF)      10      0x0a\n"
    "\\v              Vertical Tab            11      0x0b\n"
    "\\f              Form Feed               12      0x0c\n"
    "\\r              Carriage Return (CR)    13      0x0d\n"
    "\\e              Escape                  27      0x1b\n"
};

const char* const ttls[] =
{ "Res:", "&:", "\\0:", "\\1:", "\\2:", "\\3:",
 "\\4:", "\\5:", "\\6:", "\\7:", "\\8:", "\\9:",
 "\\A:", "\\B:", "\\C:", "\\D:", "\\E:", "\\F:" };

const char* const rrpls[] =
{ "\0", "&", "\\0", "\\1", "\\2", "\\3",
 "\\4", "\\5", "\\6", "\\7", "\\8", "\\9",
 "\\A", "\\B", "\\C", "\\D", "\\E", "\\F" };

#define _RLINES_ sizeof(ttls)/sizeof(ttls[0])
int main(int argc, char* argv[])
{
	bool pflag = false;
	bool sflag = false;
	bool rxflag = false;
	bool helpflag = false;
	bool dbgflag = false;

    char sstr[MAXSTRLEN] = { '\0' };
    char rxstr[MAXSTRLEN] = { '\0' };
	char repstr[MAXSTRLEN] = { '\0' };

    //Read search string from command line
    char* cmd = GetCommandLineA();

    nsplit(3, cmd, sstr, rxstr, repstr);
	if (strstr(cmd, "-p ")) pflag = true;
	else
	if (strstr(cmd, "-s ")) sflag = true;
	else
	if (strstr(cmd, "-r ")) rxflag = true;
    if (strstr(cmd, "-d ")) dbgflag = true;
	if (strstr(cmd, "-h ")) helpflag = true;
    if (!(pflag || sflag || rxflag))
    {
        printf(help1);
        if (helpflag) printf(help2);
        return 0;
	}
    if (dbgflag)
    {
        printf("Options : -d=%d, -p=%d, -s=%d, -r=%d, -h=%d\n\n",
            dbgflag, pflag, sflag, rxflag, helpflag);
	}
    if (sflag)
    {
        char dst[BUFSIZE];
        int n = ssctst(dst, sstr, rxstr);
        printf("Format string: \"%s\"\n", sstr);
        printf("Data string: \"%s\"\n\n", rxstr);
        printf("Scanned %d fields: %s\n", n, dst);
    }
	else
    if (pflag)
    {
        char dst[BUFSIZE];
        sprtst(dst, sstr, rxstr);
        printf("Format string: \"%s\"\n", sstr);
        printf("Data string: \"%s\"\n\n", rxstr);
        printf("Printed string: %s\n", dst);
    }
    else
    if (rxflag)
    {
        TRegExp* RegExp = new TRegExp(rxstr);
        bool match = false;

        printf("Source string: \"%s\"\n", sstr);
        printf("RegEx search: \"%s\"\n", rxstr);
        printf("RegEx replace: \"%s\"\n\n", repstr);
        if (RegExp)
        {
            if ((RegExp->RegError() == RE_OK))
            {
                if (RegExp->Match(sstr))
                {
                    for (int i = 0; i < _RLINES_; i++)
                    {
                        char str[BUFSIZE] = { "\0" };
                        if (rrpls[i][0] == '\0') RegExp->Replace(repstr, str, BUFSIZE);
                        else RegExp->Replace(rrpls[i], str, BUFSIZE);
						printf("% 4s %s\n", ttls[i], str);
                    }
                }
            }
            else printf("Res: %s", RegExp->regerr());
        }
	}
    return 0;
}
