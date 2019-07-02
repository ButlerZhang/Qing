#pragma once
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include <zlib.h>
#include <string>
#include <stdint.h>
#include <arpa/inet.h>  // htonl, ntohl



//struct ProtobufTransportFormat __attribute__ ((__packed__))
//{
//  int      TotalLength;                                       //except itself
//  int      MessageNameLength;                                 //at least two characters
//  char     MessageName[MessageNameLength];                    //end with \0
//  char     ProtobufData[TotalLength - MessageNameLength -8];  //google::protobuf::Message
//  int      CRC32;                                             //crc32 of MessageNameLength, MessageName and ProtobufData
//}

inline std::string EncodeMessage(const google::protobuf::Message &ProtobufMessage)
{
    const int INTSize = sizeof(int);
    std::string EncodeString;

    //�ճ�λ�ã�������Ϣ����TotalLength
    EncodeString.resize(INTSize);

    //������Ϣ���Ƶĳ��Ⱥ���\0��β����Ϣ���ַ���
    const std::string& MessageName = ProtobufMessage.GetTypeName();
    int MessageNameLength = static_cast<int>(MessageName.size() + 1);
    int MessageNameLengthHostToNetwork = ::htonl(MessageNameLength);
    EncodeString.append(reinterpret_cast<char*>(&MessageNameLengthHostToNetwork), INTSize);
    EncodeString.append(MessageName.c_str(), MessageNameLength);
    printf("Encode: Message name = %s, name length = %d\n", MessageName.c_str(), MessageNameLength);

    //����protobuf����
    const std::string &ProtobufDataString = ProtobufMessage.SerializeAsString();
    EncodeString.append(ProtobufDataString.c_str(), ProtobufDataString.size());
    printf("Encode: Protobuf size = %d\n", ProtobufDataString.size());

    //��������ܳ������������ݵ�CRC32ֵ
    const char *CheckBegin = EncodeString.c_str() + INTSize;
    unsigned int CheckCharCount = static_cast<unsigned int>(EncodeString.size() - INTSize);
    int CRC32 = static_cast<int>(crc32(1, reinterpret_cast<const Bytef*>(CheckBegin), CheckCharCount));
    int CRC32HostToNetwork = ::htonl(CRC32);
    EncodeString.append(reinterpret_cast<char*>(&CRC32HostToNetwork), INTSize);
    printf("Encode: crc32 = %d\n", CRC32);

    //������Ϣ���ܳ���
    int TotalLength = ::htonl(static_cast<int>(EncodeString.size() - INTSize));
    std::copy(reinterpret_cast<char*>(&TotalLength), reinterpret_cast<char*>(&TotalLength) + sizeof(TotalLength), EncodeString.begin());
    printf("Encode: Total Length = %d\n", static_cast<int>(EncodeString.size() - INTSize));
    printf("Encode: encode string size = %d\n", EncodeString.size());

    return EncodeString;
}

inline google::protobuf::Message* CreateMessage(const std::string &MessageName)
{
    using namespace google::protobuf;

    const Descriptor *descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(MessageName);
    if (descriptor == NULL)
    {
        printf("descriptor is NULL\n");
        return NULL;
    }

    const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (prototype == NULL)
    {
        printf("prototype is NULL\n");
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

inline google::protobuf::Message* DecodeMessage(const std::string &EncodeBuffer)
{
    const int INTSize = sizeof(int);

    //��ⳤ��
    int EncodeBufferLength = static_cast<int>(EncodeBuffer.size());
    printf("Decode: encode buffer length = %d\n", EncodeBufferLength);
    if (EncodeBufferLength <= INTSize * 2) //TotalLength + CRC32
    {
        printf("Decode: encode buffer length is wrong\n");
        return NULL;
    }

    //У��CRC32ֵ
    int EpectedCheckSum = ConvertToINT(EncodeBuffer.c_str() + (EncodeBuffer.size() - INTSize));
    int CurrentCheckSum = static_cast<int>(crc32(1, reinterpret_cast<const Bytef*>(EncodeBuffer.c_str() + INTSize), EncodeBufferLength - INTSize * 2));
    printf("Decode: check sum, epected = %d, current = %d\n", EpectedCheckSum, CurrentCheckSum);
    if (CurrentCheckSum != EpectedCheckSum)
    {
        printf("Decode: check sum wrong, epected = %d, current = %d\n", EpectedCheckSum, CurrentCheckSum);
        return NULL;
    }

    //�����Ϣ������
    int MessageNameLength = ConvertToINT(EncodeBuffer.c_str() + INTSize);
    printf("Decode: Message name length = %d\n", MessageNameLength);
    if (MessageNameLength < 2 || MessageNameLength > EncodeBufferLength)
    {
        printf("Decode: invalid message name length = %d\n", MessageNameLength);
        return NULL;
    }

    //��ȡ��Ϣ��
    std::string MessageName(EncodeBuffer.begin() + INTSize * 2, EncodeBuffer.begin() + INTSize * 2 + MessageNameLength - 1);
    printf("Decode: Message name = %s\n", MessageName.c_str());

    //������Ϣ
    google::protobuf::Message* ProtobufMessage = CreateMessage(MessageName);
    if (ProtobufMessage == NULL)
    {
        printf("Decode: Can not create message\n");
        return NULL;
    }

    //������Ϣ
    const char* ProtobufData = EncodeBuffer.c_str() + INTSize * 2 + MessageNameLength;
    int ProtobufDataLength = EncodeBufferLength - INTSize * 3 - MessageNameLength;
    printf("Decode: Protobuf message length = %d\n", ProtobufDataLength);

    if (!ProtobufMessage->ParseFromArray(ProtobufData, ProtobufDataLength))
    {
        printf("Decode: Parse wrong\n");
        delete ProtobufMessage;
        return NULL;
    }

    return ProtobufMessage;
}
