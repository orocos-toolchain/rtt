const int alpha=0x01;
const int upper=0x02;
const int lower=0x04;
const int digit=0x08;
const int space=0x10;
const int cntrl=0x20;
const int punct=0x40;
const int print=0x80;
const int xdigit=0x100;

int values[ 256 ];

class values_init
{

    public:
        values_init()
        {
            for ( int i = 'A'; i <= 'Z'; i++ )
                values[ i ] = alpha + upper + print;

            for ( int i = 'a'; i <= 'z'; i++ )
                values[ i ] = alpha + lower + print;

            for ( int i = '0'; i <= '9'; i++ )
                values[ i ] = digit + print;

            values[ int(',') ] = punct + print;

            values[ int(';') ] = punct + print;

            values[ int('.') ] = punct + print;

            values[ int(':') ] = punct + print;

            values[ int('!') ] = punct + print;

            values[ int('/') ] = punct + print;

            values[ int('?') ] = punct + print;

            values[ int('\'') ] = punct + print;

            values[ int('"') ] = punct + print;

            values[ int('-') ] = punct + print;

            values[ int('+') ] = punct + print;

            values[ int('(') ] = punct + print;

            values[ int(')') ] = punct + print;
        }
};

values_init values_init_instance; // call boem's constructor on init

inline int isalpha( int c )
{
    return values[ c ] & alpha;
}

inline int isupper( int c )
{
    return values[ c ] & upper;
}

inline int islower( int c )
{
    return values[ c ] & lower;
}

inline int isdigit( int c )
{
    return values[ c ] & digit;
}

inline int isxdigit ( int c )
{
    return values[ c ] & digit;
} // FIXME

inline int isspace ( int c )
{
    return values[ c ] & space;
}

inline int iscntrl ( int c )
{
    return values[ c ] & cntrl;
}

inline int ispunct ( int c )
{
    return values[ c ] & punct;
}

inline int isalnum ( int c )
{
    return isalpha( c ) || isdigit( c );
}

inline int isprint ( int c )
{
    return values[ c ] & print;
}

inline int isgraph ( int c )
{
    return true;
} // FIXME

char toupper( int c )
{
    if ( isalpha( c ) && islower( c ) )
        return ( c + 'A' - 'a' );
    else
        return c;
}

char tolower( int c )
{
    if ( isalpha( c ) && isupper( c ) )
        return ( c - ( 'A' - 'a' ) );
    else
        return c;
}
