/*
 * =====================================================================================
 *       Filename:  flames_pub.c
 *    Description:  Decodes SUNBURST requests and prints the results out. 
 *                  Reads names from stdin
 *        Created:  16.12.2020 22:07:30
 *         Author:  Igor Kuznetsov (igosha), 2igosha@gmail.com
 * =====================================================================================
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

// Inverse for OrionImprovementBusinessLayer.Base64Encode
bool Base32Decode(char* str, int len, uint8_t** strOut, int* lenOut){
    const char text[] = "ph2eifo3n5utg1j8d94qrvbmk0sal76c";
    if ( strlen(str) < len ) {
        return false;
    }

    *lenOut = len*5/8;
    *strOut = malloc(*lenOut + 1);
    memset(*strOut, 0, *lenOut + 1);
    if (*strOut == NULL){
        return false;
    }
    for (int i = 0; i < len; i++){
        char* ptr = strchr(text, str[i]);
        if ( ptr == NULL ) {
            free(*strOut);
            return false;
        }
        uint8_t b = ptr - text;
        for (int j = 0; j < 5; j++){
            int bit_idx = i*5 + j;
            uint8_t bit = (b >> j) & 1;
            (*strOut)[bit_idx / 8] |= (bit << ( bit_idx % 8 ) );
        }
    }
    return true;
}

// Inverse for OrionImprovementBusinessLayer.Base64Decode
char* decode(char* str){
    const char text[] = "rq3gsalt6u1iyfzop572d49bnx8cvmkewhj";
    const char text2[] = "0_-.";
    
    if ( str[0] == '0' && str[1] == '0' ) {
        uint8_t* result;
        int resultLen;
        // Special case for non-ascii names
        if ( !Base32Decode(str + 2, strlen(str) - 2, &result, &resultLen) ) {
            return false;
        }
        return result;
    }
    int outCnt = 0;
    char* result = strdup(str);

    for (char* ptr = str ; *ptr != 0; ptr++){
        const char* found = strchr(text2, *ptr);
        if ( found ) {
            ptr++;
            if ( *ptr == 0 ) {
                free(result);
                return NULL;
            }
            found = strchr(text, *ptr);
            if ( found == NULL ) {
                free(result);
                return NULL;
            }
            result[outCnt++] = text2[(found-text)%4];
        } else {
            found = strchr(text, *ptr);
            if ( found == NULL ) {
                free(result);
                return NULL;
            }
            int idx = found - text;
            result[outCnt++] = text[(sizeof(text)-1+idx-4)%((int)sizeof(text)-1)];
        }
    }
    result[outCnt]=0;
    return result;
}

bool DecodeSecureString(char* str, int len, uint8_t** strOut, int* lenOut){
    if ( !Base32Decode(str, len, strOut, lenOut) ) {
        return false;
    }
    for (int i = 0; i < *lenOut-1; i++){
        (*strOut)[i+1] ^= (*strOut)[0];
    }
    *lenOut -= 1;
    memmove(*strOut, *strOut+1, *lenOut);
    return true;
}

// GetCurrentString:
// GUID - 8 bytes, encrypted into 9, encoded by base32 into 15 bytes
// + one char (seq num)
// + encrypted dnstr
//
// GetPreviousString:
// GUID - 8 bytes, encrypted into 9, encoded by base32 into 15 bytes
// + one char (seq num)
// + part of dnstr
//
bool DecodeCurrentString(char* str){
    const char text[] = "ph2eifo3n5utg1j8d94qrvbmk0sal76c";
    if ( strlen(str) <= 16 ) {
        return false;
    }
    uint8_t* uidBuf;
    int uidBufLen;
    if (!DecodeSecureString(str, 15, &uidBuf, &uidBufLen) ) {
        return false;
    }
    uint64_t uid = *(uint64_t*)(uidBuf);
    free(uidBuf);
    uidBuf = NULL;

    char c = str[15];
    int val = 0;
    if ( c >= '0' && c <= '9' ) {
        val = c - '0';
    } else if ( c >= 'a' && c <= 'z' ) {
        val = c - 87;
    } else {
        return false;
    }
    if ( val > 36 ) {
        printf("invalid val!\n");
        return false;
    }
    int posval = -1;
    for (int i = 0; i < 36; i++){
        if ( val == ( i + str[0] ) % 36 ) {
            if ( posval != -1 ) {
                return -1;
            }
            posval = i;
        }
    }
    if ( posval == -1 ) {
        return false;
    }
    val = posval;
    printf("domain name part UID (0x%" PRIX64 ") offset %i = ", uid, val);
    char* dn = decode(str+16);
    if ( dn == NULL ) {
        return false;
    }
    for (char* ptr = dn; *ptr; ptr++){
        printf("%c", *ptr);
    }
    printf("\n");
    return true; 
}

// GetNextString/Ex:
// encrypted base32 of guid + string hash (3), encrypted on xor
bool DecodeNextString(char* str){
    uint8_t* decoded;
    int decodedLen;
    if ( !DecodeSecureString(str, strlen(str), &decoded, &decodedLen) ) {
        return false;
    }
    if ( decodedLen < 11 ) { 
        printf("decoded len mismatch, need at least 11\n");
        free(decoded);
        return false;
    }
    for (int i = 0; i < 8; i++){
        decoded[i] ^= decoded[8 + 2 - ( i % 2 )];
    }
    uint64_t uid = *(uint64_t*)decoded;
    printf("NextString UID = 0x%" PRIX64 "\n", uid);
    free(decoded);
    return true;
}

int main(){
    char buf[256];
    while (fgets(buf, sizeof(buf), stdin)) {
        char* ptr;
        if ((ptr = strchr(buf, '\n')) ){
            *ptr = 0;
        }
        if ((ptr = strchr(buf, '\r') )){
            *ptr = 0;
        }
        if ((ptr = strchr(buf, '.') )){
            *ptr = 0;
        }
        if ( !DecodeCurrentString(buf) ) {
            printf("bad str: %s\n", buf);
        }
        DecodeNextString(buf);
    }
}


