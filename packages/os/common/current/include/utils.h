#ifndef UTILS_H
#define UTILS_H

/*
 * need this for rtti and so on, you can remove it if you find
 * a better one or in another header file an implementation 
 * PS 
 */ 
/*
inline int strcmp(const char* a, const char* b)
{
    while (*a)
        if (*a++ != *b++) return 0;
    return 1;
}*/

inline unsigned int powertwo( unsigned int power )
{
    return ( unsigned int ) 1 << power;
}

inline char * bin( unsigned int i )
{
#define UTILS_BIN_RESULT_STRING_SIZE 41 /* 32 bits + 8 spaces (every 4 bits) + \0 = 41 */
    static char result[ UTILS_BIN_RESULT_STRING_SIZE ];     
    int bit, write_pos;
    write_pos = 0;
    result[ UTILS_BIN_RESULT_STRING_SIZE - 1 ] = '\0';

    for ( bit = 31; bit >= 0; bit-- )
    {
        result[ write_pos++ ] = ( i & powertwo( bit ) ? '1' : '0' );

        if ( ( bit % 4 ) == 0 )
            result[ write_pos++ ] = ' ';
    }

    return result;
}

/* returns true if bit "bitnr" is equal in value1 and value2 */
inline int bitIsEqual( unsigned long value1, unsigned long value2, unsigned int bitnr )
{
    unsigned int bitmask = powertwo( bitnr );
    return ( value1 & bitmask ) == ( value2 & bitmask );
}

#endif /* UTILS_H */
