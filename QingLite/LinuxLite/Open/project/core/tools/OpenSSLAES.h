#pragma once
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>

#include "BoostLog.h"
#include "../../../../../../Common/Tool/HexString.h"

const int KEY_BIT_SIZE = 128;



inline std::string AESEncrypt(const std::string &ClearText, const std::string &Key)
{
    if (static_cast<int>(Key.size()) > KEY_BIT_SIZE)
    {
        BoostLog::WriteError(BoostFormat("Key size is more than %d", KEY_BIT_SIZE));
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_encrypt_key((const unsigned char*)Key.c_str(), KEY_BIT_SIZE, &AESKey) < 0)
    {
        BoostLog::WriteError("Set encrypt key failed.");
        return std::string();
    }

    int PaddingCount = 0;
    if (ClearText.length() % AES_BLOCK_SIZE > 0)
    {
        PaddingCount = static_cast<int>(AES_BLOCK_SIZE - ClearText.length() % AES_BLOCK_SIZE);
        BoostLog::WriteDebug(BoostFormat("Padding count = %d", PaddingCount));
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
    BoostLog::WriteDebug(BoostFormat("AESEncrypt: Encrypt hex, size = %d, string = %s", EncryptString.size(), EncryptString.c_str()));

    return EncryptString;
}

inline std::string AESDecrypt(const std::string &CipherText, const std::string &Key)
{
    if (static_cast<int>(Key.size()) > KEY_BIT_SIZE)
    {
        BoostLog::WriteError(BoostFormat("Key size is more than %d", KEY_BIT_SIZE));
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_decrypt_key((const unsigned char*)Key.c_str(), KEY_BIT_SIZE, &AESKey) < 0)
    {
        BoostLog::WriteError("Set decrypt key failed.");
        return std::string();
    }

    BoostLog::WriteDebug(BoostFormat("AESDecrypt: Decrypt hex, size = %d, string = %s", CipherText.size(), CipherText.c_str()));

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

    BoostLog::WriteDebug(BoostFormat("AESDecrypt: Cipher text size = %d, Clear text size = %d", ASCIIChipherText.size(), ClearText.size()));
    std::string::size_type Index = ClearText.find_last_not_of('\0');
    if (Index != std::string::npos)
    {
        std::string::size_type EraseCount = ClearText.size() - (Index + 1);
        ClearText.erase(Index + 1, EraseCount);
        BoostLog::WriteDebug(BoostFormat("Erase count = %d\n", EraseCount));
    }

    BoostLog::WriteDebug(BoostFormat("AESDecrypt: Cipher text size = %d, Clear text size = %d", ASCIIChipherText.size(), ClearText.size()));
    return ClearText;
}

inline std::string AEScbcEncrypt(const std::string &ClearText, const std::string &Key)
{
    if (static_cast<int>(Key.size()) > KEY_BIT_SIZE)
    {
        BoostLog::WriteError(BoostFormat("Key size is more than %d", KEY_BIT_SIZE));
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_encrypt_key((const unsigned char*)Key.c_str(), KEY_BIT_SIZE, &AESKey) < 0)
    {
        BoostLog::WriteError("Set encrypt key failed.");
        return std::string();
    }

    unsigned int PaddingCount = 0;
    if (ClearText.length() % AES_BLOCK_SIZE > 0)
    {
        PaddingCount = static_cast<int>(AES_BLOCK_SIZE - ClearText.length() % AES_BLOCK_SIZE);
        BoostLog::WriteDebug(BoostFormat("AEScbcEncrypt: Padding count = %d\n", PaddingCount));
    }

    std::string ClearTextBackup(ClearText);
    ClearTextBackup.append(PaddingCount, '\0');
    std::vector<unsigned char> EncryptVector(ClearTextBackup.size(), 0);

    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 0, sizeof(ivec));
    AES_cbc_encrypt((const unsigned char*)ClearTextBackup.c_str(), &EncryptVector[0], ClearTextBackup.size(), &AESKey, ivec, AES_ENCRYPT);

    std::string CipherText(EncryptVector.begin(), EncryptVector.end());
    BoostLog::WriteDebug(BoostFormat("AEScbcEncrypt: Clear text size = %d, Cipher text size = %d", ClearText.size(), CipherText.size()));

    HexString MyHexString;
    const std::string &EncryptString =  MyHexString.ASCIIStringToHexString(CipherText.c_str(), CipherText.size(), false);
    BoostLog::WriteDebug(BoostFormat("AEScbcEncrypt: Encrypt hex, size = %d, string = %s", EncryptString.size(), EncryptString.c_str()));

    return EncryptString;
}

inline std::string AEScbcDecrypt(const std::string &CipherText, const std::string &Key)
{
    if (static_cast<int>(Key.size()) > KEY_BIT_SIZE)
    {
        BoostLog::WriteError(BoostFormat("ERROR: Key size is more than %d", KEY_BIT_SIZE));
        return std::string();
    }

    AES_KEY AESKey;
    if (AES_set_decrypt_key((const unsigned char*)Key.c_str(), KEY_BIT_SIZE, &AESKey) < 0)
    {
        BoostLog::WriteError("Set decrypt key failed.");
        return std::string();
    }

    BoostLog::WriteDebug(BoostFormat("AEScbcDecrypt: Decrypt hex, size = %d, string = %s", CipherText.size(), CipherText.c_str()));

    HexString MyHexString;
    std::string ASCIIChipherText;
    MyHexString.HexStringToASCIIString(CipherText, ASCIIChipherText);

    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 0, sizeof(ivec));

    std::vector<unsigned char> DecryptVector(ASCIIChipherText.size(), 0);
    AES_cbc_encrypt((const unsigned char*)ASCIIChipherText.c_str(), &DecryptVector[0], ASCIIChipherText.size(), &AESKey, ivec, AES_DECRYPT);

    std::string ClearText(DecryptVector.begin(), DecryptVector.end());
    BoostLog::WriteDebug(BoostFormat("AEScbcDecrypt: Cipher text size = %d, Clear text size = %d", ASCIIChipherText.size(), ClearText.size()));

    std::string::size_type Index = ClearText.find_last_not_of('\0');
    if (Index != std::string::npos)
    {
        std::string::size_type EraseCount = ClearText.size() - (Index + 1);
        ClearText.erase(Index + 1, EraseCount);
        BoostLog::WriteDebug(BoostFormat("AEScbcDecrypt: Erase count = %d", EraseCount));
    }

    BoostLog::WriteDebug(BoostFormat("AEScbcDecrypt: Cipher text size = %d, Clear text size = %d", ASCIIChipherText.size(), ClearText.size()));
    return ClearText;
}
