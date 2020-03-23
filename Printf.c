// Printf.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdarg.h>

int Printf(const char* sFormat, ...);
int PrintfV(const char* sFormat, va_list arg_list);

void outDec(int);       // znakowo wypisuje liczbe calk
void outChar(char);     // wypisuje znak  // putchar()
void outStr(char*);     // wypisuje zanakowo string
void outDouble(double); // wypisuje znakowow liczbe double    0.
void outNum(int);       // wypisuje znakowo liczbe int >0     rekurencyjnie

//----------------------------------
int main(int argc, char* argv[])
{
    int n = -0;
    char c = '$';
    double x = 12000000.3450000012;
    double y = -.12;
    double z = -0.5;
    char str[] = "to jest string";

    Printf("%s\n%f%c  n=%d \\ % /\ny=%f ` z=%f\n\n", str, x, c, n, y, z);
    return 0;
}
// implementacja uproszczonej funkcji "Printf": %c %d %f %s oraz
// zamienia `(~) na '(")
// do wypisywania mozna uzyc JEDYNIE putchar()

//-----------------------------------------------
int Printf(const char* sFormat, ...)
{
    va_list args; //inicjalizacja typu
    va_start(args, sFormat); //umozliwa dostep do argumentow za pomoca makra

    int res = PrintfV(sFormat, args);  //wywolanie funkcji przetwarzajacej PrintfV()

    va_end(args); // resetowanie argumentów zainicjalizowanych przez va_start

    return res;
}
//-----------------------------------------------
int PrintfV(const char* sFormat, va_list arg_list)
{
    char c;
    int i = 0;
    while ( c = sFormat[i++])
    {
        switch (c)
        {
            case '%':
               switch( c = sFormat[i++] )
               {
                 case 'd':
                     outDec(va_arg(arg_list, int ));
                     break;
                 case 'f':
                     outDouble(va_arg(arg_list, double) );
                     break;
                 case 's':
                     outStr(va_arg(arg_list, char*) );
                     break;
                 case 'c':
                     outChar(va_arg(arg_list, char) );
                     break;
                 default:
                     outChar('%');
                     outChar(c);
                     break;
               }
               break;
            case '`':
                c = '\'';
            default:
                outChar(c);
                break;
        }
    }
    return 0;
}
//-----------------------------------------------
void outChar(char c)
{
    putchar(c);
}
//-----------------------------------------------
void outStr(char* pStr)
{
    while (*pStr) outChar(*pStr++);
}
//-----------------------------------------------
void outDec(int x)
{
    if( x < 0 ) {
        x = -x;
        outChar('-');
    }
    else if( x == 0 ) {
        outChar('0');
        return;
    }
    outNum(x);
}
//-----------------------------------------------
void outDouble(double x)
{
    if( ( x > -1 ) && ( x < 0 ) )
        outChar('-');

    outDec( (int)x );
    outChar('.');

    if( x < 0 )
        x = -x;
    x = x - (int)x;
    for (int i = 0; i < 8 && ((x - (int)x) > 1e-6); i++) {
        outChar((int)(x *= 10) % 10 + '0');
    }
}
//-----------------------------------------------
void outNum( int x )
{
    if( x > 0 ) {
        outNum( x / 10 );
        outChar( x % 10 + '0' );
    }
}
