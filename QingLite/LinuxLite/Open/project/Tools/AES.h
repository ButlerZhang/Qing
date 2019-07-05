#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include "../../../../../Common/Tool/HexString.h"



inline std::string AESEncrypt(const std::string &ClearText, const std::string &Key)
{
    const int BIT_SIZE = 128;
    if (static_cast<int>(Key.size()) > BIT_SIZE)
    {
        printf("ERROR: Key length is more than %d\n", BIT_SIZE);
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_encrypt_key((const unsigned char*)Key.c_str(), BIT_SIZE, &AESKey) < 0)
    {
        printf("ERROR: Set encrypt key failed.\n");
        return std::string();
    }

    int PaddingCount = 0;
    if (ClearText.length() % AES_BLOCK_SIZE > 0)
    {
        PaddingCount = static_cast<int>(AES_BLOCK_SIZE - ClearText.length() % AES_BLOCK_SIZE);
    }

    std::string ClearTextBackup(ClearText);
    ClearTextBackup.append(PaddingCount, '\0');

    std::string EncodeString;
    unsigned char OutArray[AES_BLOCK_SIZE];
    int CycleCount = static_cast<int>(ClearTextBackup.length() / AES_BLOCK_SIZE);
    for (int Count = 0; Count < CycleCount; Count++)
    {
        ::memset(OutArray, 0, AES_BLOCK_SIZE);
        const std::string &SingleBlock = ClearTextBackup.substr(Count*AES_BLOCK_SIZE, AES_BLOCK_SIZE);

        AES_encrypt((const unsigned char*)SingleBlock.c_str(), OutArray, &AESKey);
        EncodeString.append((const char*)OutArray, AES_BLOCK_SIZE);
    }

    return EncodeString;
}

inline std::string AESDecrypt(const std::string &CipherText, const std::string &Key)
{
    const int BIT_SIZE = 128;
    if (static_cast<int>(Key.size()) > BIT_SIZE)
    {
        printf("ERROR: Key length is more than %d\n", BIT_SIZE);
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_decrypt_key((const unsigned char*)Key.c_str(), BIT_SIZE, &AESKey) < 0)
    {
        printf("ERROR: Set decrypt key failed.\n");
        return std::string();
    }

    std::string DecodeString;
    unsigned char OutArray[AES_BLOCK_SIZE];
    int CycleCount = static_cast<int>(CipherText.length() / AES_BLOCK_SIZE);
    for (int Count = 0; Count < CycleCount; Count++)
    {
        ::memset(OutArray, 0, AES_BLOCK_SIZE);
        const std::string &SingleBlock = CipherText.substr(Count*AES_BLOCK_SIZE, AES_BLOCK_SIZE);

        AES_decrypt((const unsigned char*)SingleBlock.c_str(), OutArray, &AESKey);
        DecodeString.append((const char*)OutArray, AES_BLOCK_SIZE);
    }

    return DecodeString;
}

inline std::string AEScbcEncrypt(const std::string &ClearText, const std::string &Key)
{
    const int BIT_SIZE = 128;
    if (static_cast<int>(Key.size()) > BIT_SIZE)
    {
        printf("ERROR: Key length is more than %d\n", BIT_SIZE);
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_encrypt_key((const unsigned char*)Key.c_str(), BIT_SIZE, &AESKey) < 0)
    {
        printf("ERROR: Set encrypt key failed.\n");
        return std::string();
    }

    unsigned int PaddingCount = 0;
    if (ClearText.length() % AES_BLOCK_SIZE > 0)
    {
        PaddingCount = static_cast<int>(AES_BLOCK_SIZE - ClearText.length() % AES_BLOCK_SIZE);
    }

    std::string ClearTextBackup(ClearText);
    ClearTextBackup.append(PaddingCount, '\0');

    //alloc encrypt_string
    unsigned char* EncryptString = (unsigned char*)calloc(ClearTextBackup.size(), sizeof(unsigned char));
    if (EncryptString == NULL)
    {
        printf("Unable to allocate memory for encrypt string.\n");
        return std::string();
    }

    //encrypt
    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 0, sizeof(ivec));
    AES_cbc_encrypt((const unsigned char*)ClearTextBackup.c_str(), EncryptString, ClearTextBackup.size(), &AESKey, ivec, AES_ENCRYPT);

    std::string ResultString(EncryptString, EncryptString + ClearTextBackup.size());
    free(EncryptString);
    EncryptString = NULL;

    return ResultString;
}

inline std::string AEScbcDecrypt(const std::string &CipherText, const std::string &Key)
{
    const int BIT_SIZE = 128;
    if (static_cast<int>(Key.size()) > BIT_SIZE)
    {
        printf("ERROR: Key length is more than %d\n", BIT_SIZE);
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_decrypt_key((const unsigned char*)Key.c_str(), BIT_SIZE, &AESKey) < 0)
    {
        printf("ERROR: Set decrypt key failed.\n");
        return std::string();
    }

    unsigned char* DecryptString = (unsigned char*)calloc(CipherText.size(), sizeof(unsigned char));
    if (DecryptString == NULL)
    {
        printf("Unable to allocate memory for decrypt string.\n");
        return std::string();
    }

    // decrypt
    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 0, sizeof(ivec));

    AES_cbc_encrypt((const unsigned char*)CipherText.c_str(), DecryptString, CipherText.size(), &AESKey, ivec, AES_DECRYPT);

    std::string ResultString(DecryptString, DecryptString + CipherText.size());
    free(DecryptString);
    DecryptString = NULL;

    return ResultString;
}
