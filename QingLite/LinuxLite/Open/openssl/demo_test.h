#pragma once
#include "demo_MD5.h"
#include "demo_SHA256.h"
#include "demo_DES.h"
#include "demo_RSA.h"

#include <iostream>



void OpenSSL_EncryptAndDecryptTest()
{
    std::string ClearText("Tomorrow is better.");
    std::string DESKey("butler");

    std::cout << "=====MD5 test: " << std::endl;
    std::cout << "encrypt ascii: " << GetMD5String(ClearText) << std::endl;
    std::cout << "encrypt   hex: " << GetMD5HexString(ClearText) << std::endl << std::endl;

    std::cout << "=====SHA256 test: " << std::endl;
    std::string SHAASCII = GetSHA256String(ClearText);
    std::string SHAHEX = GetSHA256HexString(ClearText, true);

    std::cout << "encrypt ascii: " << SHAASCII << std::endl;
    std::cout << "ascii size = " << SHAASCII.size() << std::endl;
    std::cout << "encrypt   hex: " << SHAHEX << std::endl;
    std::cout << "Hex   size = " << SHAHEX.size() << std::endl << std::endl;

    std::cout << "=====DES test: " << std::endl;
    std::string DESEncryptString = DESEncrypt(ClearText, DESKey);
    std::cout << "DES encrypt: " << DESEncryptString << std::endl;
    std::string DESDecryptString = DESDecrypt(DESEncryptString, DESKey);
    std::cout << "DES decrypt: " << DESDecryptString << std::endl << std::endl;

    std::cout << "=====RAS test: " << std::endl;
    std::string PublicKeyString, PrivateKeyString;
    GenerateRSAKeyPair(PublicKeyString, PrivateKeyString);
    std::string RASEncryptString = RSAPublicEncrypt(ClearText, PublicKeyString);
    std::cout << "RAS encrypt: " << RASEncryptString << std::endl;
    std::string RASDecryptString = RSAPrivateDecrypt(RASEncryptString, PrivateKeyString);
    std::cout << "RAS decrypt: " << RASDecryptString << std::endl << std::endl;
}
