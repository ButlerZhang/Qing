#pragma once
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include "../../../../Common/Tool/HexString.h"

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
        printf("Padding count = %d\n", PaddingCount);
    }

    std::string ClearTextBackup(ClearText);
    ClearTextBackup.append(PaddingCount, '\0');

    std::string CipherText;
    unsigned char OutArray[AES_BLOCK_SIZE];
    int CycleCount = static_cast<int>(ClearTextBackup.length() / AES_BLOCK_SIZE);
    for (int Count = 0; Count < CycleCount; Count++)
    {
        const std::string &SingleBlock = ClearTextBackup.substr(Count*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        AES_encrypt((const unsigned char*)SingleBlock.c_str(), OutArray, &AESKey);
        CipherText.append((const char*)OutArray, AES_BLOCK_SIZE);
    }

    HexString MyHexString;
    const std::string &EncryptString = MyHexString.ASCIIStringToHexString(CipherText.c_str(), CipherText.size(), false);
    printf("AESEncrypt: Encrypt hex, size = %d, string = %s\n", EncryptString.size(), EncryptString.c_str());

    return EncryptString;
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

    printf("AESDecrypt: Decrypt hex, size = %d, string = %s\n", CipherText.size(), CipherText.c_str());

    HexString MyHexString;
    std::string ASCIIChipherText;
    MyHexString.HexStringToASCIIString(CipherText, ASCIIChipherText);

    std::string ClearText;
    unsigned char OutArray[AES_BLOCK_SIZE];
    int CycleCount = static_cast<int>(ASCIIChipherText.length() / AES_BLOCK_SIZE);
    for (int Count = 0; Count < CycleCount; Count++)
    {
        const std::string &SingleBlock = ASCIIChipherText.substr(Count * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        AES_decrypt((const unsigned char*)SingleBlock.c_str(), OutArray, &AESKey);
        ClearText.append((const char*)OutArray, AES_BLOCK_SIZE);
    }

    printf("AESDecrypt: Cipher text size = %d, Clear text size = %d\n", ASCIIChipherText.size(), ClearText.size());
    std::string::size_type Index = ClearText.find_last_not_of('\0');
    if (Index != std::string::npos)
    {
        std::string::size_type EraseCount = ClearText.size() - (Index + 1);
        ClearText.erase(Index + 1, EraseCount);
        printf("Erase count = %d\n", EraseCount);
    }

    printf("AESDecrypt: Cipher text size = %d, Clear text size = %d\n", ASCIIChipherText.size(), ClearText.size());
    return ClearText;
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
        printf("AEScbcEncrypt: Padding count = %d\n", PaddingCount);
    }

    std::string ClearTextBackup(ClearText);
    ClearTextBackup.append(PaddingCount, '\0');
    std::vector<unsigned char> EncryptVector(ClearTextBackup.size(), 0);

    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 0, sizeof(ivec));
    AES_cbc_encrypt((const unsigned char*)ClearTextBackup.c_str(), &EncryptVector[0], ClearTextBackup.size(), &AESKey, ivec, AES_ENCRYPT);

    std::string CipherText(EncryptVector.begin(), EncryptVector.end());
    printf("AEScbcEncrypt: Clear text size = %d, Cipher text size = %d\n", ClearText.size(), CipherText.size());

    HexString MyHexString;
    const std::string &EncryptString = MyHexString.ASCIIStringToHexString(CipherText.c_str(), CipherText.size(), false);
    printf("AEScbcEncrypt: Encrypt hex, size = %d, string = %s\n", EncryptString.size(), EncryptString.c_str());

    return EncryptString;
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

    printf("AEScbcDecrypt: Decrypt hex, size = %d, string = %s\n", CipherText.size(), CipherText.c_str());

    HexString MyHexString;
    std::string ASCIIChipherText;
    MyHexString.HexStringToASCIIString(CipherText, ASCIIChipherText);

    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 0, sizeof(ivec));

    std::vector<unsigned char> DecryptVector(ASCIIChipherText.size(), 0);
    AES_cbc_encrypt((const unsigned char*)ASCIIChipherText.c_str(), &DecryptVector[0], ASCIIChipherText.size(), &AESKey, ivec, AES_DECRYPT);

    std::string ClearText(DecryptVector.begin(), DecryptVector.end());
    printf("AEScbcDecrypt: Cipher text size = %d, Clear text size = %d\n", ASCIIChipherText.size(), ClearText.size());

    std::string::size_type Index = ClearText.find_last_not_of('\0');
    if (Index != std::string::npos)
    {
        std::string::size_type EraseCount = ClearText.size() - (Index + 1);
        ClearText.erase(Index + 1, EraseCount);
        printf("AEScbcDecrypt: Erase count = %d\n", EraseCount);
    }

    printf("AEScbcDecrypt: Cipher text size = %d, Clear text size = %d\n", ASCIIChipherText.size(), ClearText.size());
    return ClearText;
}
