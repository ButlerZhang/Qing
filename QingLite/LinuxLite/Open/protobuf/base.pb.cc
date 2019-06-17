// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: base.proto

#include "base.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
namespace Open {
class MessageHeaderDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<MessageHeader> _instance;
} _MessageHeader_default_instance_;
}  // namespace Open
static void InitDefaultsscc_info_MessageHeader_base_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::Open::_MessageHeader_default_instance_;
    new (ptr) ::Open::MessageHeader();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Open::MessageHeader::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_MessageHeader_base_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsscc_info_MessageHeader_base_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_base_2eproto[1];
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_base_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_base_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_base_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Open::MessageHeader, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Open::MessageHeader, type_),
  PROTOBUF_FIELD_OFFSET(::Open::MessageHeader, transmissionid_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Open::MessageHeader)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::Open::_MessageHeader_default_instance_),
};

const char descriptor_table_protodef_base_2eproto[] =
  "\n\nbase.proto\022\004Open\"H\n\rMessageHeader\022\037\n\004T"
  "ype\030\001 \001(\0162\021.Open.MessageType\022\026\n\016Transmis"
  "sionID\030\002 \001(\t*;\n\013MessageType\022\r\n\tMT_UNKNOW"
  "\020\000\022\r\n\010MT_LOGIN\020\350\007\022\016\n\tMT_LOGOUT\020\351\007b\006proto"
  "3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_base_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_base_2eproto_sccs[1] = {
  &scc_info_MessageHeader_base_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_base_2eproto_once;
static bool descriptor_table_base_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_base_2eproto = {
  &descriptor_table_base_2eproto_initialized, descriptor_table_protodef_base_2eproto, "base.proto", 161,
  &descriptor_table_base_2eproto_once, descriptor_table_base_2eproto_sccs, descriptor_table_base_2eproto_deps, 1, 0,
  schemas, file_default_instances, TableStruct_base_2eproto::offsets,
  file_level_metadata_base_2eproto, 1, file_level_enum_descriptors_base_2eproto, file_level_service_descriptors_base_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_base_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_base_2eproto), true);
namespace Open {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MessageType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_base_2eproto);
  return file_level_enum_descriptors_base_2eproto[0];
}
bool MessageType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1000:
    case 1001:
      return true;
    default:
      return false;
  }
}


// ===================================================================

void MessageHeader::InitAsDefaultInstance() {
}
class MessageHeader::HasBitSetters {
 public:
};

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int MessageHeader::kTypeFieldNumber;
const int MessageHeader::kTransmissionIDFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

MessageHeader::MessageHeader()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Open.MessageHeader)
}
MessageHeader::MessageHeader(const MessageHeader& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  transmissionid_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from.transmissionid().size() > 0) {
    transmissionid_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.transmissionid_);
  }
  type_ = from.type_;
  // @@protoc_insertion_point(copy_constructor:Open.MessageHeader)
}

void MessageHeader::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_MessageHeader_base_2eproto.base);
  transmissionid_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  type_ = 0;
}

MessageHeader::~MessageHeader() {
  // @@protoc_insertion_point(destructor:Open.MessageHeader)
  SharedDtor();
}

void MessageHeader::SharedDtor() {
  transmissionid_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void MessageHeader::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const MessageHeader& MessageHeader::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_MessageHeader_base_2eproto.base);
  return *internal_default_instance();
}


void MessageHeader::Clear() {
// @@protoc_insertion_point(message_clear_start:Open.MessageHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  transmissionid_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  type_ = 0;
  _internal_metadata_.Clear();
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
const char* MessageHeader::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // .Open.MessageType Type = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
          set_type(static_cast<::Open::MessageType>(val));
        } else goto handle_unusual;
        continue;
      // string TransmissionID = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParserUTF8(mutable_transmissionid(), ptr, ctx, "Open.MessageHeader.TransmissionID");
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
bool MessageHeader::MergePartialFromCodedStream(
    ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
  ::PROTOBUF_NAMESPACE_ID::uint32 tag;
  // @@protoc_insertion_point(parse_start:Open.MessageHeader)
  for (;;) {
    ::std::pair<::PROTOBUF_NAMESPACE_ID::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // .Open.MessageType Type = 1;
      case 1: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (8 & 0xFF)) {
          int value = 0;
          DO_((::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadPrimitive<
                   int, ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          set_type(static_cast< ::Open::MessageType >(value));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string TransmissionID = 2;
      case 2: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (18 & 0xFF)) {
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadString(
                input, this->mutable_transmissionid()));
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
            this->transmissionid().data(), static_cast<int>(this->transmissionid().length()),
            ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::PARSE,
            "Open.MessageHeader.TransmissionID"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:Open.MessageHeader)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Open.MessageHeader)
  return false;
#undef DO_
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

void MessageHeader::SerializeWithCachedSizes(
    ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Open.MessageHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .Open.MessageType Type = 1;
  if (this->type() != 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnum(
      1, this->type(), output);
  }

  // string TransmissionID = 2;
  if (this->transmissionid().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->transmissionid().data(), static_cast<int>(this->transmissionid().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Open.MessageHeader.TransmissionID");
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->transmissionid(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:Open.MessageHeader)
}

::PROTOBUF_NAMESPACE_ID::uint8* MessageHeader::InternalSerializeWithCachedSizesToArray(
    ::PROTOBUF_NAMESPACE_ID::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:Open.MessageHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .Open.MessageType Type = 1;
  if (this->type() != 0) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      1, this->type(), target);
  }

  // string TransmissionID = 2;
  if (this->transmissionid().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->transmissionid().data(), static_cast<int>(this->transmissionid().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Open.MessageHeader.TransmissionID");
    target =
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringToArray(
        2, this->transmissionid(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Open.MessageHeader)
  return target;
}

size_t MessageHeader::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Open.MessageHeader)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string TransmissionID = 2;
  if (this->transmissionid().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->transmissionid());
  }

  // .Open.MessageType Type = 1;
  if (this->type() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->type());
  }

  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void MessageHeader::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Open.MessageHeader)
  GOOGLE_DCHECK_NE(&from, this);
  const MessageHeader* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<MessageHeader>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Open.MessageHeader)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Open.MessageHeader)
    MergeFrom(*source);
  }
}

void MessageHeader::MergeFrom(const MessageHeader& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Open.MessageHeader)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.transmissionid().size() > 0) {

    transmissionid_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.transmissionid_);
  }
  if (from.type() != 0) {
    set_type(from.type());
  }
}

void MessageHeader::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Open.MessageHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void MessageHeader::CopyFrom(const MessageHeader& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Open.MessageHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool MessageHeader::IsInitialized() const {
  return true;
}

void MessageHeader::Swap(MessageHeader* other) {
  if (other == this) return;
  InternalSwap(other);
}
void MessageHeader::InternalSwap(MessageHeader* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  transmissionid_.Swap(&other->transmissionid_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(type_, other->type_);
}

::PROTOBUF_NAMESPACE_ID::Metadata MessageHeader::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace Open
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Open::MessageHeader* Arena::CreateMaybeMessage< ::Open::MessageHeader >(Arena* arena) {
  return Arena::CreateInternal< ::Open::MessageHeader >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
