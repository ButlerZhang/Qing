#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>

const int KEY_BIT_SIZE = 128;



inline std::string AESEncrypt(const std::string &ClearText, const std::string &Key)
{
    if (static_cast<int>(Key.size()) > KEY_BIT_SIZE)
    {
        printf("ERROR: Key size is more than %d\n", KEY_BIT_SIZE);
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_encrypt_key((const unsigned char*)Key.c_str(), KEY_BIT_SIZE, &AESKey) < 0)
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
        const std::string &SingleBlock = ClearTextBackup.substr(Count*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        AES_encrypt((const unsigned char*)SingleBlock.c_str(), OutArray, &AESKey);
        EncodeString.append((const char*)OutArray, AES_BLOCK_SIZE);
    }

    return EncodeString;
}

inline std::string AESDecrypt(const std::string &CipherText, const std::string &Key)
{
    if (static_cast<int>(Key.size()) > KEY_BIT_SIZE)
    {
        printf("ERROR: Key size is more than %d\n", KEY_BIT_SIZE);
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_decrypt_key((const unsigned char*)Key.c_str(), KEY_BIT_SIZE, &AESKey) < 0)
    {
        printf("ERROR: Set decrypt key failed.\n");
        return std::string();
    }

    std::string DecodeString;
    unsigned char OutArray[AES_BLOCK_SIZE];
    int CycleCount = static_cast<int>(CipherText.length() / AES_BLOCK_SIZE);
    for (int Count = 0; Count < CycleCount; Count++)
    {
        const std::string &SingleBlock = CipherText.substr(Count * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        AES_decrypt((const unsigned char*)SingleBlock.c_str(), OutArray, &AESKey);
        DecodeString.append((const char*)OutArray, AES_BLOCK_SIZE);
    }

    return DecodeString;
}

inline std::string AEScbcEncrypt(const std::string &ClearText, const std::string &Key)
{
    if (static_cast<int>(Key.size()) > KEY_BIT_SIZE)
    {
        printf("ERROR: Key size is more than %d\n", KEY_BIT_SIZE);
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_encrypt_key((const unsigned char*)Key.c_str(), KEY_BIT_SIZE, &AESKey) < 0)
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
    std::vector<unsigned char> EncryptVector(ClearTextBackup.size(), 0);

    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 0, sizeof(ivec));
    AES_cbc_encrypt((const unsigned char*)ClearTextBackup.c_str(), &EncryptVector[0], ClearTextBackup.size(), &AESKey, ivec, AES_ENCRYPT);

    return std::string(EncryptVector.begin(), EncryptVector.end());
}

inline std::string AEScbcDecrypt(const std::string &CipherText, const std::string &Key)
{
    if (static_cast<int>(Key.size()) > KEY_BIT_SIZE)
    {
        printf("ERROR: Key size is more than %d\n", KEY_BIT_SIZE);
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_decrypt_key((const unsigned char*)Key.c_str(), KEY_BIT_SIZE, &AESKey) < 0)
    {
        printf("ERROR: Set decrypt key failed.\n");
        return std::string();
    }

    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 0, sizeof(ivec));

    std::vector<unsigned char> DecryptVector(CipherText.size(), 0);
    AES_cbc_encrypt((const unsigned char*)CipherText.c_str(), &DecryptVector[0], CipherText.size(), &AESKey, ivec, AES_DECRYPT);

    return std::string(DecryptVector.begin(), DecryptVector.end());
}
