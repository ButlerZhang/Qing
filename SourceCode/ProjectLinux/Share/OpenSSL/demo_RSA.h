#pragma once
#include <string>
#include <vector>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>



void WriteKeyToPemFile(const std::string &PublicKeyString, const std::string &PrivateKeyString)
{
    FILE *PublicFile = fopen("publickey.pem", "w");
    if (PublicFile == NULL)
    {
        printf("Open public key pem file failed.\n");
        return;
    }

    FILE *PrivateFile = fopen("privatekey.pem", "w");
    if (PrivateFile == NULL)
    {
        printf("Open private key pem file failed.\n");
        fclose(PublicFile);
        PublicFile = NULL;
        return;
    }

    fputs(PublicKeyString.c_str(), PublicFile);
    fputs(PrivateKeyString.c_str(), PrivateFile);

    fclose(PublicFile);
    fclose(PrivateFile);
}

//可以使用GenerateRSAKeyPair生成密钥对，也可以使用openssl命令行生成密钥对。
void GenerateRSAKeyPair(std::string &PublicKeyString, std::string &PrivateKeyString)
{
    //生成密钥对
    const int KEY_LENGTH = 2048;
    RSA *RSAKeyPair = RSA_generate_key(KEY_LENGTH, RSA_3, NULL, NULL);

    BIO *PrivateBIO = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(PrivateBIO, RSAKeyPair, NULL, NULL, 0, NULL, NULL);

    BIO *PublicBIO = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(PublicBIO, RSAKeyPair);

    //密钥对读取到字符串
    int PrivateBIOLength = BIO_pending(PrivateBIO);
    int PublicBIOLength = BIO_pending(PublicBIO);

    char *PrivateKey = (char *)malloc(PrivateBIOLength + 1);
    char *PublicKey = (char *)malloc(PublicBIOLength + 1);

    BIO_read(PrivateBIO, PrivateKey, PrivateBIOLength);
    BIO_read(PublicBIO, PublicKey, PublicBIOLength);

    PrivateKey[PrivateBIOLength] = '\0';
    PublicKey[PublicBIOLength] = '\0';

    //存储密钥对
    PublicKeyString = PublicKey;
    PrivateKeyString = PrivateKey;

    //内存释放
    RSA_free(RSAKeyPair);
    BIO_free_all(PublicBIO);
    BIO_free_all(PrivateBIO);

    free(PrivateKey);
    free(PublicKey);

    WriteKeyToPemFile(PublicKeyString, PrivateKeyString);
}

std::string RSAPublicEncrypt(const std::string &ClearText, const std::string &PublicKeyString)
{
    BIO *PublicKeyBIO = BIO_new_mem_buf((unsigned char *)PublicKeyString.c_str(), -1);

    RSA *Rsa = NULL;
    Rsa = PEM_read_bio_RSAPublicKey(PublicKeyBIO, &Rsa, NULL, NULL);

    int RSASize = RSA_size(Rsa);
    char *EncryptedText = (char *)malloc(RSASize + 1);
    memset(EncryptedText, 0, RSASize + 1);

    int EncryptSize = RSA_public_encrypt(
        static_cast<int>(ClearText.length()),
        (const unsigned char*)ClearText.c_str(),
        (unsigned char*)EncryptedText,
        Rsa,
        RSA_PKCS1_PADDING);

    std::string EncryptString;
    if (EncryptSize >= 0)
    {
        EncryptString = std::string(EncryptedText, EncryptSize);
    }

    free(EncryptedText);
    BIO_free_all(PublicKeyBIO);
    RSA_free(Rsa);

    return EncryptString;
}

std::string RSAPrivateDecrypt(const std::string &CipherText, const std::string &PrivateKey)
{
    BIO *PrivateKeyBIO = BIO_new_mem_buf((unsigned char *)PrivateKey.c_str(), -1);

    RSA *Rsa = RSA_new();
    Rsa = PEM_read_bio_RSAPrivateKey(PrivateKeyBIO, &Rsa, NULL, NULL);

    int RSASize = RSA_size(Rsa);
    char *DecryptedText = (char *)malloc(RSASize + 1);
    memset(DecryptedText, 0, RSASize + 1);

    int DecryptSize = RSA_private_decrypt(
        static_cast<int>(CipherText.length()),
        (const unsigned char*)CipherText.c_str(),
        (unsigned char*)DecryptedText,
        Rsa,
        RSA_PKCS1_PADDING);

    std::string DecryptString;
    if (DecryptSize >= 0)
    {
        DecryptString = std::string(DecryptedText, DecryptSize);
    }

    free(DecryptedText);
    BIO_free_all(PrivateKeyBIO);
    RSA_free(Rsa);

    return DecryptString;
}
