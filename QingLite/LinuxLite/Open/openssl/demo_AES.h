#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include "../../../../Common/Tool/HexString.h"



std::string AESEncrypt(const std::string &ClearText, const std::string &Key)
{
    //set key
    unsigned char TempKey[AES_BLOCK_SIZE];
    memset(TempKey, 0, sizeof(TempKey));
    memcpy(TempKey, Key.c_str(), (Key.size() > AES_BLOCK_SIZE ? AES_BLOCK_SIZE : Key.size()));

    AES_KEY AESKey;
    if (AES_set_encrypt_key(TempKey, 128, &AESKey) < 0)
    {
        printf("Set encrypt key failed.\n");
        return std::string();
    }

    //calculate encrypt length
    ssize_t EncryptLength = ClearText.size() + 1;
    if (EncryptLength % AES_BLOCK_SIZE != 0)
    {
        EncryptLength = ((ClearText.size() + 1) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    }

    //alloc encrypt_string
    unsigned char* EncryptString = (unsigned char*)calloc(EncryptLength, sizeof(unsigned char));
    if (EncryptString == NULL)
    {
        printf("Unable to allocate memory for encrypt string.\n");
        return std::string();
    }

    //encrypt
    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 0, sizeof(ivec));
    AES_cbc_encrypt((const unsigned char*)ClearText.c_str(), EncryptString, EncryptLength, &AESKey, ivec, AES_ENCRYPT);

    HexString MyHexString;
    const std::string &ResultString = MyHexString.ASCIIStringToHexString((const char*)EncryptString, EncryptLength, true);

    free(EncryptString);
    EncryptString = NULL;

    return ResultString;
}

std::string AESDecrypt(const std::string &CipherText, const std::string &Key)
{
    //set key
    unsigned char TempKey[AES_BLOCK_SIZE];
    memset(TempKey, 0, sizeof(TempKey));
    memcpy(TempKey, Key.c_str(), (Key.size() > AES_BLOCK_SIZE ? AES_BLOCK_SIZE : Key.size()));

    AES_KEY AESKey;
    if (AES_set_decrypt_key(TempKey, 128, &AESKey) < 0)
    {
        printf("Set encrypt key failed.\n");
        return std::string();
    }

    //get ascii string
    HexString MyHexString;
    std::string ASCIIString;
    MyHexString.HexStringToASCIIString(CipherText, ASCIIString);

    // alloc decrypt_string
    unsigned char* DecryptString = (unsigned char*)calloc(ASCIIString.size(), sizeof(unsigned char));
    if (DecryptString == NULL)
    {
        printf("Unable to allocate memory for decrypt string.\n");
        return std::string();
    }

    // decrypt
    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 0, sizeof(ivec));
    AES_cbc_encrypt((const unsigned char*)ASCIIString.c_str(), DecryptString, ASCIIString.size(), &AESKey, ivec, AES_DECRYPT);

    std::string ResultString((const char*)DecryptString);
    free(DecryptString);
    DecryptString = NULL;

    return ResultString;
}
