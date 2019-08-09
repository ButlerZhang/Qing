#pragma once
#include <zlib.h>
#include <string>
#include <stdint.h>
#include <arpa/inet.h>  // htonl, ntohl
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include "../../../../Common/Boost/BoostLog.h"
#include "../core/tools/OpenSSLAES.h"



//struct ProtobufTransportFormat __attribute__ ((__packed__))
//{
//  int      TotalLength;                                       //except itself
//  int      MessageType;                                       //message type
//  int      MessageNameLength;                                 //at least two characters
//  char     MessageName[MessageNameLength];                    //end with \0
//  char     ProtobufData[TotalLength - MessageNameLength -8];  //google::protobuf::Message
//  int      CRC32;                                             //crc32 of MessageNameLength, MessageName and ProtobufData
//}

inline std::string EncodeMessage(const google::protobuf::Message &ProtobufMessage, int MessageType = 0)
{
    const int INT_SIZE = sizeof(int);
    std::string EncodeString;

    //空出位置，保存消息长度TotalLength
    EncodeString.resize(INT_SIZE);

    //保存消息类型，备用
    int MessageTypeHostToNetwork = ::htonl(MessageType);
    EncodeString.append(reinterpret_cast<char*>(&MessageTypeHostToNetwork), INT_SIZE);
    g_Log.WriteDebug(BoostFormat("Encode: Message type = %d", MessageType));

    //保存消息名称的长度和以\0结尾的消息名字符串
    const std::string& MessageName = ProtobufMessage.GetTypeName();
    int MessageNameLength = static_cast<int>(MessageName.size() + 1);
    int MessageNameLengthHostToNetwork = ::htonl(MessageNameLength);
    EncodeString.append(reinterpret_cast<char*>(&MessageNameLengthHostToNetwork), INT_SIZE);
    EncodeString.append(MessageName.c_str(), MessageNameLength);
    g_Log.WriteDebug(BoostFormat("Encode: Message name = %s, name length = %d", MessageName.c_str(), MessageNameLength));

    //保存protobuf内容
    const std::string &ProtobufDataString = ProtobufMessage.SerializeAsString();
    //EncodeString.append(ProtobufDataString.c_str(), ProtobufDataString.size());
    //g_Log.WriteDebug(BoostFormat("Encode: Protobuf size = %d", ProtobufDataString.size()));

    //加密
    const std::string &EncryptString = AEScbcEncrypt(ProtobufDataString, MY_AES_KEY);
    EncodeString.append(EncryptString.c_str(), EncryptString.size());
    g_Log.WriteDebug(BoostFormat("Encode: Protobuf size = %d", EncryptString.size()));

    //计算除了总长度外其余数据的CRC32值
    const char *CheckBegin = EncodeString.c_str() + INT_SIZE;
    unsigned int CheckCharCount = static_cast<unsigned int>(EncodeString.size() - INT_SIZE);
    int CRC32 = static_cast<int>(crc32(1, reinterpret_cast<const Bytef*>(CheckBegin), CheckCharCount));
    int CRC32HostToNetwork = ::htonl(CRC32);
    EncodeString.append(reinterpret_cast<char*>(&CRC32HostToNetwork), INT_SIZE);
    g_Log.WriteDebug(BoostFormat("Encode: crc32 = %d", CRC32));

    //保存消息的总长度
    int TotalLength = static_cast<int>(EncodeString.size() - INT_SIZE);
    int TotalLengthHostToNetwork = ::htonl(TotalLength);
    char *CopyAddress = reinterpret_cast<char*>(&TotalLengthHostToNetwork);
    std::copy(CopyAddress, CopyAddress + INT_SIZE, EncodeString.begin());
    g_Log.WriteDebug(BoostFormat("Encode: Total Length = %d, encode string size = %d", TotalLength, EncodeString.size()));

    return EncodeString;
}

inline google::protobuf::Message* CreateMessage(const std::string &MessageName)
{
    using namespace google::protobuf;

    const Descriptor *descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(MessageName);
    if (descriptor == NULL)
    {
        g_Log.WriteError("descriptor is NULL");
        return NULL;
    }

    const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (prototype == NULL)
    {
        g_Log.WriteError("prototype is NULL");
        return NULL;
    }

    return prototype->New();
}

inline int ConvertToINT(const char *buf)
{
    int value = 0;
    ::memcpy(&value, buf, sizeof(int));
    return ::ntohl(value);
}

inline int DecodeMessage(std::string &EncodeBuffer)
{
    const int INT_SIZE = sizeof(int);

    //检测长度
    int EncodeBufferLength = static_cast<int>(EncodeBuffer.size());
    g_Log.WriteDebug(BoostFormat("Decode: encode buffer length = %d", EncodeBufferLength));
    if (EncodeBufferLength <= INT_SIZE * 3) //message type + message name length + CRC32
    {
        g_Log.WriteError("Decode: encode buffer length is wrong");
        return 0;
    }

    //校验CRC32值
    int EpectedCheckSum = ConvertToINT(EncodeBuffer.c_str() + (EncodeBuffer.size() - INT_SIZE));
    int CurrentCheckSum = static_cast<int>(crc32(1, reinterpret_cast<const Bytef*>(EncodeBuffer.c_str() + INT_SIZE), EncodeBufferLength - INT_SIZE * 2));
    g_Log.WriteDebug(BoostFormat("Decode: check sum, epected = %d, current = %d", EpectedCheckSum, CurrentCheckSum));
    if (CurrentCheckSum != EpectedCheckSum)
    {
        g_Log.WriteError(BoostFormat("Decode: check sum wrong, epected = %d, current = %d", EpectedCheckSum, CurrentCheckSum));
        return 0;
    }

    //获取消息类型
    int MessageType = ConvertToINT(EncodeBuffer.c_str() + INT_SIZE);
    g_Log.WriteDebug(BoostFormat("Decode: Message type = %d", MessageType));

    //检查消息名长度
    int MessageNameLength = ConvertToINT(EncodeBuffer.c_str() + INT_SIZE * 2);
    g_Log.WriteDebug(BoostFormat("Decode: Message name length = %d", MessageNameLength));
    if (MessageNameLength < 2 || MessageNameLength > EncodeBufferLength)
    {
        g_Log.WriteError(BoostFormat("Decode: invalid message name length = %d", MessageNameLength));
        return 0;
    }

    //获取Protobuf数据
    const char* ProtobufData = EncodeBuffer.c_str() + INT_SIZE * 3 + MessageNameLength;
    int ProtobufDataLength = EncodeBufferLength - INT_SIZE * 4 - MessageNameLength;
    g_Log.WriteDebug(BoostFormat("Decode: Protobuf message length = %d", ProtobufDataLength));

    //解密
    std::string ProtobufString(ProtobufData, ProtobufData + ProtobufDataLength);
    std::string DecryptString = AEScbcDecrypt(ProtobufString, MY_AES_KEY);
    EncodeBuffer.swap(DecryptString);

    return MessageType;
}

inline google::protobuf::Message* DecodeMessage(const std::string &EncodeBuffer)
{
    const int INT_SIZE = sizeof(int);

    //检测长度
    int EncodeBufferLength = static_cast<int>(EncodeBuffer.size());
    g_Log.WriteDebug(BoostFormat("Decode: encode buffer length = %d", EncodeBufferLength));
    if (EncodeBufferLength <= INT_SIZE * 3) //message type + message name length + CRC32
    {
        g_Log.WriteError("Decode: encode buffer length is wrong");
        return NULL;
    }

    //校验CRC32值
    int EpectedCheckSum = ConvertToINT(EncodeBuffer.c_str() + (EncodeBuffer.size() - INT_SIZE));
    int CurrentCheckSum = static_cast<int>(crc32(1, reinterpret_cast<const Bytef*>(EncodeBuffer.c_str() + INT_SIZE), EncodeBufferLength - INT_SIZE * 2));
    g_Log.WriteDebug(BoostFormat("Decode: check sum, epected = %d, current = %d", EpectedCheckSum, CurrentCheckSum));
    if (CurrentCheckSum != EpectedCheckSum)
    {
        g_Log.WriteError(BoostFormat("Decode: check sum wrong, epected = %d, current = %d", EpectedCheckSum, CurrentCheckSum));
        return NULL;
    }

    //检查消息名长度
    int MessageNameLength = ConvertToINT(EncodeBuffer.c_str() + INT_SIZE * 2);
    g_Log.WriteDebug(BoostFormat("Decode: Message name length = %d", MessageNameLength));
    if (MessageNameLength < 2 || MessageNameLength > EncodeBufferLength)
    {
        g_Log.WriteError(BoostFormat("Decode: invalid message name length = %d", MessageNameLength));
        return NULL;
    }

    //获取消息名
    std::string MessageName(EncodeBuffer.begin() + INT_SIZE * 3, EncodeBuffer.begin() + INT_SIZE * 3 + MessageNameLength - 1);
    g_Log.WriteDebug(BoostFormat("Decode: Message name = %s", MessageName.c_str()));

    //创建消息
    google::protobuf::Message* ProtobufMessage = CreateMessage(MessageName);
    if (ProtobufMessage == NULL)
    {
        g_Log.WriteError("Decode: Can not create message");
        return NULL;
    }

    //解析消息
    const char* ProtobufData = EncodeBuffer.c_str() + INT_SIZE * 3 + MessageNameLength;
    int ProtobufDataLength = EncodeBufferLength - INT_SIZE * 4 - MessageNameLength;
    g_Log.WriteDebug(BoostFormat("Decode: Protobuf message length = %d", ProtobufDataLength));

    if (!ProtobufMessage->ParseFromArray(ProtobufData, ProtobufDataLength))
    {
        g_Log.WriteError("Decode: Parse wrong");
        delete ProtobufMessage;
        return NULL;
    }

    return ProtobufMessage;
}
