#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "des.h"


void des_init( des_context *ctx )
{
    memset( ctx, 0, sizeof( des_context ) );
}

void des_free( des_context *ctx )
{
    if( ctx == NULL )
        return;

    zeroize( ctx, sizeof( des_context ) );
}

void des3_init( des3_context *ctx )
{
    memset( ctx, 0, sizeof( des3_context ) );
}

void des3_free( des3_context *ctx )
{
    if( ctx == NULL )
        return;

    zeroize( ctx, sizeof( des3_context ) );
}



void des_key_set_parity( unsigned char key[DES_KEY_SIZE] )
{
    int i;

    for( i = 0; i < DES_KEY_SIZE; i++ )
        key[i] = odd_parity_table[key[i] / 2];
}

/*
 * Check the given key's parity, returns 1 on failure, 0 on SUCCESS
 */
int des_key_check_key_parity( const unsigned char key[DES_KEY_SIZE] )
{
    int i;

    for( i = 0; i < DES_KEY_SIZE; i++ )
        if( key[i] != odd_parity_table[key[i] / 2] )
            return( 1 );

    return( 0 );
}

/*
 * Table of weak and semi-weak keys
 *
 * Source: http://en.wikipedia.org/wiki/Weak_key
 *
 * Weak:
 * Alternating ones + zeros (0x0101010101010101)
 * Alternating 'F' + 'E' (0xFEFEFEFEFEFEFEFE)
 * '0xE0E0E0E0F1F1F1F1'
 * '0x1F1F1F1F0E0E0E0E'
 *
 * Semi-weak:
 * 0x011F011F010E010E and 0x1F011F010E010E01
 * 0x01E001E001F101F1 and 0xE001E001F101F101
 * 0x01FE01FE01FE01FE and 0xFE01FE01FE01FE01
 * 0x1FE01FE00EF10EF1 and 0xE01FE01FF10EF10E
 * 0x1FFE1FFE0EFE0EFE and 0xFE1FFE1FFE0EFE0E
 * 0xE0FEE0FEF1FEF1FE and 0xFEE0FEE0FEF1FEF1
 *
 */



int des_key_check_weak( const unsigned char key[DES_KEY_SIZE] )
{
    int i;

    for( i = 0; i < WEAK_KEY_COUNT; i++ )
        if( memcmp( weak_key_table[i], key, DES_KEY_SIZE) == 0 )
            return( 1 );

    return( 0 );
}


void des_setkey( uint32_t SK[32], const unsigned char key[DES_KEY_SIZE] )
{
    int i;
    uint32_t X, Y, T;

    GET_UINT32_BE( X, key, 0 );
    GET_UINT32_BE( Y, key, 4 );

    /*
     * Permuted Choice 1
     */
    T =  ((Y >>  4) ^ X) & 0x0F0F0F0F;  X ^= T; Y ^= (T <<  4);
    T =  ((Y      ) ^ X) & 0x10101010;  X ^= T; Y ^= (T      );

    X =   (LHs[ (X      ) & 0xF] << 3) | (LHs[ (X >>  8) & 0xF ] << 2)
        | (LHs[ (X >> 16) & 0xF] << 1) | (LHs[ (X >> 24) & 0xF ]     )
        | (LHs[ (X >>  5) & 0xF] << 7) | (LHs[ (X >> 13) & 0xF ] << 6)
        | (LHs[ (X >> 21) & 0xF] << 5) | (LHs[ (X >> 29) & 0xF ] << 4);

    Y =   (RHs[ (Y >>  1) & 0xF] << 3) | (RHs[ (Y >>  9) & 0xF ] << 2)
        | (RHs[ (Y >> 17) & 0xF] << 1) | (RHs[ (Y >> 25) & 0xF ]     )
        | (RHs[ (Y >>  4) & 0xF] << 7) | (RHs[ (Y >> 12) & 0xF ] << 6)
        | (RHs[ (Y >> 20) & 0xF] << 5) | (RHs[ (Y >> 28) & 0xF ] << 4);

    X &= 0x0FFFFFFF;
    Y &= 0x0FFFFFFF;

    /*
     * calculate subkeys
     */
    for( i = 0; i < 16; i++ )
    {
        if( i < 2 || i == 8 || i == 15 )
        {
            X = ((X <<  1) | (X >> 27)) & 0x0FFFFFFF;
            Y = ((Y <<  1) | (Y >> 27)) & 0x0FFFFFFF;
        }
        else
        {
            X = ((X <<  2) | (X >> 26)) & 0x0FFFFFFF;
            Y = ((Y <<  2) | (Y >> 26)) & 0x0FFFFFFF;
        }

        *SK++ =   ((X <<  4) & 0x24000000) | ((X << 28) & 0x10000000)
                | ((X << 14) & 0x08000000) | ((X << 18) & 0x02080000)
                | ((X <<  6) & 0x01000000) | ((X <<  9) & 0x00200000)
                | ((X >>  1) & 0x00100000) | ((X << 10) & 0x00040000)
                | ((X <<  2) & 0x00020000) | ((X >> 10) & 0x00010000)
                | ((Y >> 13) & 0x00002000) | ((Y >>  4) & 0x00001000)
                | ((Y <<  6) & 0x00000800) | ((Y >>  1) & 0x00000400)
                | ((Y >> 14) & 0x00000200) | ((Y      ) & 0x00000100)
                | ((Y >>  5) & 0x00000020) | ((Y >> 10) & 0x00000010)
                | ((Y >>  3) & 0x00000008) | ((Y >> 18) & 0x00000004)
                | ((Y >> 26) & 0x00000002) | ((Y >> 24) & 0x00000001);

        *SK++ =   ((X << 15) & 0x20000000) | ((X << 17) & 0x10000000)
                | ((X << 10) & 0x08000000) | ((X << 22) & 0x04000000)
                | ((X >>  2) & 0x02000000) | ((X <<  1) & 0x01000000)
                | ((X << 16) & 0x00200000) | ((X << 11) & 0x00100000)
                | ((X <<  3) & 0x00080000) | ((X >>  6) & 0x00040000)
                | ((X << 15) & 0x00020000) | ((X >>  4) & 0x00010000)
                | ((Y >>  2) & 0x00002000) | ((Y <<  8) & 0x00001000)
                | ((Y >> 14) & 0x00000808) | ((Y >>  9) & 0x00000400)
                | ((Y      ) & 0x00000200) | ((Y <<  7) & 0x00000100)
                | ((Y >>  7) & 0x00000020) | ((Y >>  3) & 0x00000011)
                | ((Y <<  2) & 0x00000004) | ((Y >> 21) & 0x00000002);
    }
}


/*
 * DES key schedule (56-bit, encryption)
 */
int des_setkey_enc( des_context *ctx, const unsigned char key[DES_KEY_SIZE] )
{
    des_setkey( ctx->sk, key );

    return( 0 );
}

/*
 * DES key schedule (56-bit, decryption)
 */
int des_setkey_dec( des_context *ctx, const unsigned char key[DES_KEY_SIZE] )
{
    int i;

    des_setkey( ctx->sk, key );

    for( i = 0; i < 16; i += 2 )
    {
        SWAP( ctx->sk[i    ], ctx->sk[30 - i] );
        SWAP( ctx->sk[i + 1], ctx->sk[31 - i] );
    }

    return( 0 );
}

static void des3_set2key( uint32_t esk[96],
                          uint32_t dsk[96],
                          const unsigned char key[DES_KEY_SIZE*2] )
{
    int i;

    des_setkey( esk, key );
    des_setkey( dsk + 32, key + 8 );

    for( i = 0; i < 32; i += 2 )
    {
        dsk[i     ] = esk[30 - i];
        dsk[i +  1] = esk[31 - i];

        esk[i + 32] = dsk[62 - i];
        esk[i + 33] = dsk[63 - i];

        esk[i + 64] = esk[i    ];
        esk[i + 65] = esk[i + 1];

        dsk[i + 64] = dsk[i    ];
        dsk[i + 65] = dsk[i + 1];
    }
}

/*
 * Triple-DES key schedule (112-bit, encryption)
 */
int des3_set2key_enc( des3_context *ctx,
                      const unsigned char key[DES_KEY_SIZE * 2] )
{
    uint32_t sk[96];

    des3_set2key( ctx->sk, sk, key );
    zeroize( sk,  sizeof( sk ) );

    return( 0 );
}

/*
 * Triple-DES key schedule (112-bit, decryption)
 */
int des3_set2key_dec( des3_context *ctx,
                      const unsigned char key[DES_KEY_SIZE * 2] )
{
    uint32_t sk[96];

    des3_set2key( sk, ctx->sk, key );
    zeroize( sk,  sizeof( sk ) );

    return( 0 );
}

static void des3_set3key( uint32_t esk[96],
                          uint32_t dsk[96],
                          const unsigned char key[24] )
{
    int i;

    des_setkey( esk, key );
    des_setkey( dsk + 32, key +  8 );
    des_setkey( esk + 64, key + 16 );

    for( i = 0; i < 32; i += 2 )
    {
        dsk[i     ] = esk[94 - i];
        dsk[i +  1] = esk[95 - i];

        esk[i + 32] = dsk[62 - i];
        esk[i + 33] = dsk[63 - i];

        dsk[i + 64] = esk[30 - i];
        dsk[i + 65] = esk[31 - i];
    }
}

/*
 * Triple-DES key schedule (168-bit, encryption)
 */
int des3_set3key_enc( des3_context *ctx,
                      const unsigned char key[DES_KEY_SIZE * 3] )
{
    uint32_t sk[96];

    des3_set3key( ctx->sk, sk, key );
    zeroize( sk,  sizeof( sk ) );

    return( 0 );
}

/*
 * Triple-DES key schedule (168-bit, decryption)
 */
int des3_set3key_dec( des3_context *ctx,
                      const unsigned char key[DES_KEY_SIZE * 3] )
{
    uint32_t sk[96];

    des3_set3key( sk, ctx->sk, key );
    zeroize( sk,  sizeof( sk ) );

    return( 0 );
}

/*
 * DES-ECB block encryption/decryption
 */

int des_crypt_ecb( des_context *ctx,
                    const unsigned char input[8],
                    unsigned char output[8] )
{
    int i;
    uint32_t X, Y, T, *SK;

    SK = ctx->sk;

    GET_UINT32_BE( X, input, 0 );
    GET_UINT32_BE( Y, input, 4 );

    DES_IP( X, Y );

    for( i = 0; i < 8; i++ )
    {
        DES_ROUND( Y, X );
        DES_ROUND( X, Y );
    }

    DES_FP( Y, X );

    PUT_UINT32_BE( Y, output, 0 );
    PUT_UINT32_BE( X, output, 4 );

    return( 0 );
}



/*
 * DES-CBC buffer encryption/decryption
 */
int des_crypt_cbc( des_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[8],
                    const unsigned char *input,
                    unsigned char *output )
{
    int i;
    unsigned char temp[8];

    if( length % 8 )
        return( -1 );

    if( mode == MBEDTLS_DES_ENCRYPT )
    {
        while( length > 0 )
        {
            for( i = 0; i < 8; i++ )
                output[i] = (unsigned char)( input[i] ^ iv[i] );

            des_crypt_ecb( ctx, output, output );
            memcpy( iv, output, 8 );

            input  += 8;
            output += 8;
            length -= 8;
        }
    }
    else /* DES_DECRYPT */
    {
        while( length > 0 )
        {
            memcpy( temp, input, 8 );
            des_crypt_ecb( ctx, input, output );

            for( i = 0; i < 8; i++ )
                output[i] = (unsigned char)( output[i] ^ iv[i] );

            memcpy( iv, temp, 8 );

            input  += 8;
            output += 8;
            length -= 8;
        }
    }

    return( 0 );
}


/*
 * 3DES-ECB block encryption/decryption
 */

int des3_crypt_ecb( des3_context *ctx,
                     const unsigned char input[8],
                     unsigned char output[8] )
{
    int i;
    uint32_t X, Y, T, *SK;

    SK = ctx->sk;

    GET_UINT32_BE( X, input, 0 );
    GET_UINT32_BE( Y, input, 4 );

    DES_IP( X, Y );

    for( i = 0; i < 8; i++ )
    {
        DES_ROUND( Y, X );
        DES_ROUND( X, Y );
    }

    for( i = 0; i < 8; i++ )
    {
        DES_ROUND( X, Y );
        DES_ROUND( Y, X );
    }

    for( i = 0; i < 8; i++ )
    {
        DES_ROUND( Y, X );
        DES_ROUND( X, Y );
    }

    DES_FP( Y, X );

    PUT_UINT32_BE( Y, output, 0 );
    PUT_UINT32_BE( X, output, 4 );

    return( 0 );
}



/*
 * 3DES-CBC buffer encryption/decryption
 */
int des3_crypt_cbc( des3_context *ctx,
                     int mode,
                     size_t length,
                     unsigned char iv[8],
                     const unsigned char *input,
                     unsigned char *output )
{
    int i;
    unsigned char temp[8];

    if( length % 8 )
        return( -1 );

    if( mode == MBEDTLS_DES_ENCRYPT )
    {
        while( length > 0 )
        {
            for( i = 0; i < 8; i++ )
                output[i] = (unsigned char)( input[i] ^ iv[i] );

            des3_crypt_ecb( ctx, output, output );
            memcpy( iv, output, 8 );

            input  += 8;
            output += 8;
            length -= 8;
        }
    }
    else /* DES_DECRYPT */
    {
        while( length > 0 )
        {
            memcpy( temp, input, 8 );
            des3_crypt_ecb( ctx, input, output );

            for( i = 0; i < 8; i++ )
                output[i] = (unsigned char)( output[i] ^ iv[i] );

            memcpy( iv, temp, 8 );

            input  += 8;
            output += 8;
            length -= 8;
        }
    }

    return( 0 );
}



/*
 * DES-ECB buffer encryption API
 */
unsigned int des_ecb_encrypt(unsigned char *pout,
                             unsigned char *pdata,
                             unsigned int nlen,
                             unsigned char *pkey)
{
    unsigned char *tmp;
    unsigned int len,i;
    unsigned char ch = '\0';
    des_context ctx;

    des_setkey_enc( &ctx, pkey );

    len = (nlen / 8 + (nlen % 8 ? 1: 0)) * 8;

    //ch = 8 - nlen % 8;
    for(i = 0;i < nlen;i += 8)
    {
        des_crypt_ecb( &ctx, (pdata + i), (pout + i) );
    }
    if(len > nlen)
    {
        tmp = (unsigned char *)malloc(len);
        i -= 8;
        memcpy(tmp,pdata + i,nlen - i);
        memset(tmp + nlen % 8, ch, (8 - nlen % 8) % 8);
        des_crypt_ecb( &ctx, tmp, (pout + i));
        free(tmp);
    }

    des_free( &ctx );
    return len;


}
/*
 * DES-ECB buffer decryption API
 */
unsigned int des_ecb_decrypt(unsigned char *pout,
                             unsigned char *pdata,
                             unsigned int nlen,
                             unsigned char *pkey)
{

    unsigned int i;
    des_context ctx;

    if(nlen % 8)
        return 1;

    des_setkey_dec( &ctx, pkey );


    for(i = 0;i < nlen;i += 8)
    {
        des_crypt_ecb(&ctx, (pdata + i), (pout + i));
    }
    des_free( &ctx );
    return 0;

}

/*
 * DES-CBC buffer encryption API
 */
unsigned int des_cbc_encrypt(unsigned char *pout,
                             unsigned char *pdata,
                             unsigned int nlen,
                             unsigned char *pkey,
                             unsigned char *piv)
{
    des_context ctx;
    unsigned char iv[8] = {0};
    unsigned char *pivb;

    if(piv == NULL)
        pivb = iv;
    else
        pivb = piv;

    des_setkey_enc( &ctx, pkey );

    des_crypt_cbc( &ctx, 1, nlen, pivb, pdata, (pout));

    des_free( &ctx );

    return nlen;


}
/*
 * DES-CBC buffer decryption API
 */
unsigned int des_cbc_decrypt(unsigned char *pout,
                             unsigned char *pdata,
                             unsigned int nlen,
                             unsigned char *pkey,
                             unsigned char *piv)
{

    des_context ctx;
    unsigned char iv[8] = {0};
    unsigned char *pivb;

    if(piv == NULL)
        pivb = iv;
    else
        pivb = piv;

    des_setkey_dec( &ctx, pkey );

    des_crypt_cbc( &ctx, 0, nlen, pivb, pdata, (pout));

    des_free( &ctx );

    return 0;

}
/*
 * 3DES-ECB buffer encryption API
 */
unsigned int des3_ecb_encrypt(unsigned char *pout,
                              unsigned char *pdata,
                              unsigned int nlen,
                              unsigned char *pkey,
                              unsigned int klen)
{
    unsigned char *tmp;
    unsigned int len,i;
    unsigned char ch = '\0';
    des3_context ctx3;

    if(klen == DES3_KEY2_SIZE)//16字节
        des3_set2key_enc( &ctx3, pkey );//根据长度设置key
    else if(klen == DES3_KEY3_SIZE)//24字节
        des3_set3key_enc( &ctx3, pkey );

    len = (nlen / 8 + (nlen % 8 ? 1: 0)) * 8;

    //ch = 8 - nlen % 8;//可以设置补齐内容，常用0或0xFF
    for(i = 0;i < nlen;i += 8)
    {
        des3_crypt_ecb( &ctx3, (pdata + i), (pout + i) );
    }
    if(len > nlen)//不足8字节补齐
    {
        tmp = (unsigned char *)malloc(len);
        i -= 8;
        memcpy(tmp,pdata + i,nlen - i);
        memset(tmp + nlen % 8, ch, (8 - nlen % 8) % 8);
        des3_crypt_ecb( &ctx3, tmp, (pout + i));
        free(tmp);
    }

    des3_free( &ctx3 );
    return len;


}
/*
 * 3DES-ECB buffer decryption API
 */
unsigned int des3_ecb_decrypt(unsigned char *pout,
                              unsigned char *pdata,
                              unsigned int nlen,
                              unsigned char *pkey,
                              unsigned int klen)
{

    unsigned int i;
    des3_context ctx3;

    if(nlen % 8)
        return 1;

    if(klen == DES3_KEY2_SIZE)
        des3_set2key_dec( &ctx3, pkey );
    else if(klen == DES3_KEY3_SIZE)
        des3_set3key_dec( &ctx3, pkey );


    for(i = 0;i < nlen;i += 8)
    {
        des3_crypt_ecb(&ctx3, (pdata + i), (pout + i));
    }
    des3_free( &ctx3 );
    return 0;

}
/*
 * 3DES-CBC buffer encryption API
 */
unsigned int des3_cbc_encrypt(unsigned char *pout,
                              unsigned char *pdata,
                              unsigned int nlen,
                              unsigned char *pkey,
                              unsigned int klen,
                              unsigned char *piv)
{
    des3_context ctx;
    unsigned char iv[8] = {0};
    unsigned char *pivb;
    unsigned int len;

    if(piv == NULL)
        pivb = iv;
    else
        pivb = piv;

    if(klen == DES3_KEY2_SIZE)
        des3_set2key_enc( &ctx, pkey );
    else if(klen == DES3_KEY3_SIZE)
        des3_set3key_enc( &ctx, pkey );

    if(nlen % 8)
    {
        len = nlen + 8 - nlen % 8;
        unsigned char * tmp = (unsigned char *)calloc(1, len);
        memcpy(tmp, pdata, nlen);
        des3_crypt_cbc( &ctx, 1, len, pivb, tmp, (pout));
        free(tmp);
    }
    else
    {
        des3_crypt_cbc( &ctx, 1, nlen, pivb, pdata, (pout));
    }

    des3_free( &ctx );

    return nlen;


}
/*
 * 3DES-CBC buffer decryption API
 */
unsigned int des3_cbc_decrypt(unsigned char *pout,
                              unsigned char *pdata,
                              unsigned int nlen,
                              unsigned char *pkey,
                              unsigned int klen,
                              unsigned char *piv)
{

    des3_context ctx;
    unsigned char iv[8] = {0};
    unsigned char *pivb;

    if(nlen % 8)
        return 1;

    if(piv == NULL)
        pivb = iv;
    else
        pivb = piv;


    if(klen == DES3_KEY2_SIZE)
        des3_set2key_dec( &ctx, pkey );
    else if(klen == DES3_KEY3_SIZE)
        des3_set3key_dec( &ctx, pkey );

    des3_crypt_cbc( &ctx, 0, nlen, pivb, pdata, (pout));

    des3_free( &ctx );

    return 0;

}

