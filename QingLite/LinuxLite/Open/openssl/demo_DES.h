#pragma once
#include <string>
#include <vector>
#include <string.h>
#include <openssl/des.h>



std::string DESEncrypt(const std::string &ClearText, const std::string &Key)
{
    //初始化补齐后的密钥
    DES_cblock KeyEncrypt;
    memset(KeyEncrypt, 0, sizeof(KeyEncrypt));
    memcpy(KeyEncrypt, Key.c_str(), (Key.length() <= sizeof(KeyEncrypt) ? Key.length() : sizeof(KeyEncrypt)));

    //密钥置换
    DES_key_schedule KeySchedule;
    DES_set_key_unchecked(&KeyEncrypt, &KeySchedule);

    //循环加密，每8字节一次
    DES_cblock OutputText;
    const_DES_cblock InputText;
    std::vector<unsigned char> CipherTextVector;

    const int CycleByteSize = 8;
    unsigned char OutputBuffer[CycleByteSize];
    for (int InputIndex = 0; InputIndex < static_cast<int>(ClearText.length()) / CycleByteSize; InputIndex++)
    {
        memcpy(InputText, ClearText.c_str() + InputIndex * CycleByteSize, CycleByteSize);

        //加密函数
        DES_ecb_encrypt(&InputText, &OutputText, &KeySchedule, DES_ENCRYPT);

        memcpy(OutputBuffer, OutputText, CycleByteSize);
        for (int OutputIndex = 0; OutputIndex < CycleByteSize; OutputIndex++)
        {
            CipherTextVector.push_back(OutputBuffer[OutputIndex]);
        }
    }

    //不能被8整除的剩余的明文
    if (ClearText.length() % CycleByteSize != 0)
    {
        int tmp1 = static_cast<int>(ClearText.length()) / CycleByteSize * CycleByteSize;
        int tmp2 = static_cast<int>(ClearText.length()) - tmp1;

        memset(InputText, 0, CycleByteSize);
        memcpy(InputText, ClearText.c_str() + tmp1, tmp2);

        //加密函数
        DES_ecb_encrypt(&InputText, &OutputText, &KeySchedule, DES_ENCRYPT);
        memcpy(OutputBuffer, OutputText, CycleByteSize);
        for (int OutputIndex = 0; OutputIndex < CycleByteSize; OutputIndex++)
        {
            CipherTextVector.push_back(OutputBuffer[OutputIndex]);
        }
    }

    return std::string(CipherTextVector.begin(), CipherTextVector.end());
}

std::string DESDecrypt(const std::string &CipherText, const std::string &Key)
{
    //初始化补齐后的密钥
    DES_cblock KeyEncrypt;
    memset(KeyEncrypt, 0, sizeof(KeyEncrypt));
    memcpy(KeyEncrypt, Key.c_str(), (Key.length() <= sizeof(KeyEncrypt) ? Key.length() : sizeof(KeyEncrypt)));

    //密钥置换
    DES_key_schedule KeySchedule;
    DES_set_key_unchecked(&KeyEncrypt, &KeySchedule);

    //循环解密，每8字节一次
    DES_cblock OutputText;
    const_DES_cblock InputText;
    std::vector<unsigned char> ClearTextVector;

    const int CycleByteSize = 8;
    unsigned char OutputBuffer[CycleByteSize];
    for (int InputIndex = 0; InputIndex < static_cast<int>(CipherText.length()) / CycleByteSize; InputIndex++)
    {
        memcpy(InputText, CipherText.c_str() + InputIndex * CycleByteSize, CycleByteSize);

        //解密
        DES_ecb_encrypt(&InputText, &OutputText, &KeySchedule, DES_DECRYPT);

        memcpy(OutputBuffer, OutputText, CycleByteSize);
        for (int OutputIndex = 0; OutputIndex < CycleByteSize; OutputIndex++)
        {
            ClearTextVector.push_back(OutputBuffer[OutputIndex]);
        }
    }

    if (CipherText.length() % CycleByteSize != 0)
    {
        int tmp1 = static_cast<int>(CipherText.length()) / CycleByteSize * CycleByteSize;
        int tmp2 = static_cast<int>(CipherText.length()) - tmp1;

        memset(InputText, 0, CycleByteSize);
        memcpy(InputText, CipherText.c_str() + tmp1, tmp2);

        DES_ecb_encrypt(&InputText, &OutputText, &KeySchedule, DES_DECRYPT);

        memcpy(OutputBuffer, OutputText, CycleByteSize);
        for (int OutputIndex = 0; OutputIndex < CycleByteSize; OutputIndex++)
        {
            ClearTextVector.push_back(OutputBuffer[OutputIndex]);
        }
    }

    return std::string(ClearTextVector.begin(), ClearTextVector.end());
}
