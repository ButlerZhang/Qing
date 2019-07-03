#pragma once
#include <zlib.h>
#include <string>
#include <stdint.h>
#include <arpa/inet.h>  // htonl, ntohl
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>



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

    //�ճ�λ�ã�������Ϣ����TotalLength
    EncodeString.resize(INT_SIZE);

    //������Ϣ���ͣ�����
    int MessageTypeHostToNetwork = ::htonl(MessageType);
    EncodeString.append(reinterpret_cast<char*>(&MessageTypeHostToNetwork), INT_SIZE);
    printf("Encode: Message type = %d\n", MessageType);

    //������Ϣ���Ƶĳ��Ⱥ���\0��β����Ϣ���ַ���
    const std::string& MessageName = ProtobufMessage.GetTypeName();
    int MessageNameLength = static_cast<int>(MessageName.size() + 1);
    int MessageNameLengthHostToNetwork = ::htonl(MessageNameLength);
    EncodeString.append(reinterpret_cast<char*>(&MessageNameLengthHostToNetwork), INT_SIZE);
    EncodeString.append(MessageName.c_str(), MessageNameLength);
    printf("Encode: Message name = %s, name length = %d\n", MessageName.c_str(), MessageNameLength);

    //����protobuf����
    const std::string &ProtobufDataString = ProtobufMessage.SerializeAsString();
    EncodeString.append(ProtobufDataString.c_str(), ProtobufDataString.size());
    printf("Encode: Protobuf size = %d\n", ProtobufDataString.size());

    //��������ܳ������������ݵ�CRC32ֵ
    const char *CheckBegin = EncodeString.c_str() + INT_SIZE;
    unsigned int CheckCharCount = static_cast<unsigned int>(EncodeString.size() - INT_SIZE);
    int CRC32 = static_cast<int>(crc32(1, reinterpret_cast<const Bytef*>(CheckBegin), CheckCharCount));
    int CRC32HostToNetwork = ::htonl(CRC32);
    EncodeString.append(reinterpret_cast<char*>(&CRC32HostToNetwork), INT_SIZE);
    printf("Encode: crc32 = %d\n", CRC32);

    //������Ϣ���ܳ���
    int TotalLength = static_cast<int>(EncodeString.size() - INT_SIZE);
    int TotalLengthHostToNetwork = ::htonl(TotalLength);
    char *CopyAddress = reinterpret_cast<char*>(&TotalLengthHostToNetwork);
    std::copy(CopyAddress, CopyAddress + INT_SIZE, EncodeString.begin());
    printf("Encode: Total Length = %d, encode string size = %d\n", TotalLength, EncodeString.size());

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

inline int DecodeMessage(std::string &EncodeBuffer)
{
    const int INT_SIZE = sizeof(int);

    //��ⳤ��
    int EncodeBufferLength = static_cast<int>(EncodeBuffer.size());
    printf("Decode: encode buffer length = %d\n", EncodeBufferLength);
    if (EncodeBufferLength <= INT_SIZE * 3) //message type + message name length + CRC32
    {
        printf("Decode: encode buffer length is wrong\n");
        return 0;
    }

    //У��CRC32ֵ
    int EpectedCheckSum = ConvertToINT(EncodeBuffer.c_str() + (EncodeBuffer.size() - INT_SIZE));
    int CurrentCheckSum = static_cast<int>(crc32(1, reinterpret_cast<const Bytef*>(EncodeBuffer.c_str() + INT_SIZE), EncodeBufferLength - INT_SIZE * 2));
    printf("Decode: check sum, epected = %d, current = %d\n", EpectedCheckSum, CurrentCheckSum);
    if (CurrentCheckSum != EpectedCheckSum)
    {
        printf("Decode: check sum wrong, epected = %d, current = %d\n", EpectedCheckSum, CurrentCheckSum);
        return 0;
    }

    //��ȡ��Ϣ����
    int MessageType = ConvertToINT(EncodeBuffer.c_str() + INT_SIZE);
    printf("Decode: Message type = %d\n", MessageType);

    //�����Ϣ������
    int MessageNameLength = ConvertToINT(EncodeBuffer.c_str() + INT_SIZE * 2);
    printf("Decode: Message name length = %d\n", MessageNameLength);
    if (MessageNameLength < 2 || MessageNameLength > EncodeBufferLength)
    {
        printf("Decode: invalid message name length = %d\n", MessageNameLength);
        return 0;
    }

    //��ȡProtobuf����
    const char* ProtobufData = EncodeBuffer.c_str() + INT_SIZE * 3 + MessageNameLength;
    int ProtobufDataLength = EncodeBufferLength - INT_SIZE * 4 - MessageNameLength;
    printf("Decode: Protobuf message length = %d\n", ProtobufDataLength);

    std::string ProtobufString(ProtobufData, ProtobufData + ProtobufDataLength);
    EncodeBuffer.swap(ProtobufString);

    return MessageType;
}

inline google::protobuf::Message* DecodeMessage(const std::string &EncodeBuffer)
{
    const int INT_SIZE = sizeof(int);

    //��ⳤ��
    int EncodeBufferLength = static_cast<int>(EncodeBuffer.size());
    printf("Decode: encode buffer length = %d\n", EncodeBufferLength);
    if (EncodeBufferLength <= INT_SIZE * 3) //message type + message name length + CRC32
    {
        printf("Decode: encode buffer length is wrong\n");
        return NULL;
    }

    //У��CRC32ֵ
    int EpectedCheckSum = ConvertToINT(EncodeBuffer.c_str() + (EncodeBuffer.size() - INT_SIZE));
    int CurrentCheckSum = static_cast<int>(crc32(1, reinterpret_cast<const Bytef*>(EncodeBuffer.c_str() + INT_SIZE), EncodeBufferLength - INT_SIZE * 2));
    printf("Decode: check sum, epected = %d, current = %d\n", EpectedCheckSum, CurrentCheckSum);
    if (CurrentCheckSum != EpectedCheckSum)
    {
        printf("Decode: check sum wrong, epected = %d, current = %d\n", EpectedCheckSum, CurrentCheckSum);
        return NULL;
    }

    //�����Ϣ������
    int MessageNameLength = ConvertToINT(EncodeBuffer.c_str() + INT_SIZE * 2);
    printf("Decode: Message name length = %d\n", MessageNameLength);
    if (MessageNameLength < 2 || MessageNameLength > EncodeBufferLength)
    {
        printf("Decode: invalid message name length = %d\n", MessageNameLength);
        return NULL;
    }

    //��ȡ��Ϣ��
    std::string MessageName(EncodeBuffer.begin() + INT_SIZE * 3, EncodeBuffer.begin() + INT_SIZE * 3 + MessageNameLength - 1);
    printf("Decode: Message name = %s\n", MessageName.c_str());

    //������Ϣ
    google::protobuf::Message* ProtobufMessage = CreateMessage(MessageName);
    if (ProtobufMessage == NULL)
    {
        printf("Decode: Can not create message\n");
        return NULL;
    }

    //������Ϣ
    const char* ProtobufData = EncodeBuffer.c_str() + INT_SIZE * 3 + MessageNameLength;
    int ProtobufDataLength = EncodeBufferLength - INT_SIZE * 4 - MessageNameLength;
    printf("Decode: Protobuf message length = %d\n", ProtobufDataLength);

    if (!ProtobufMessage->ParseFromArray(ProtobufData, ProtobufDataLength))
    {
        printf("Decode: Parse wrong\n");
        delete ProtobufMessage;
        return NULL;
    }

    return ProtobufMessage;
}
