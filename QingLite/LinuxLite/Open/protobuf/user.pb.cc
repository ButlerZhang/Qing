// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: user.proto

#include "user.pb.h"

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
extern PROTOBUF_INTERNAL_EXPORT_base_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_MessageHeader_base_2eproto;
namespace Open {
class UserDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<User> _instance;
} _User_default_instance_;
}  // namespace Open
static void InitDefaultsscc_info_User_user_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::Open::_User_default_instance_;
    new (ptr) ::Open::User();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Open::User::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_User_user_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, InitDefaultsscc_info_User_user_2eproto}, {
      &scc_info_MessageHeader_base_2eproto.base,}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_user_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_user_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_user_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_user_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Open::User, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Open::User, header_),
  PROTOBUF_FIELD_OFFSET(::Open::User, id_),
  PROTOBUF_FIELD_OFFSET(::Open::User, name_),
  PROTOBUF_FIELD_OFFSET(::Open::User, password_),
  PROTOBUF_FIELD_OFFSET(::Open::User, authority_),
  PROTOBUF_FIELD_OFFSET(::Open::User, nickname_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Open::User)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::Open::_User_default_instance_),
};

const char descriptor_table_protodef_user_2eproto[] =
  "\n\nuser.proto\022\004Open\032\nbase.proto\"|\n\004User\022#"
  "\n\006header\030\001 \001(\0132\023.Open.MessageHeader\022\n\n\002I"
  "D\030\002 \001(\005\022\014\n\004Name\030\003 \001(\t\022\020\n\010Password\030\004 \001(\t\022"
  "\021\n\tAuthority\030\005 \001(\t\022\020\n\010Nickname\030\006 \003(\tb\006pr"
  "oto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_user_2eproto_deps[1] = {
  &::descriptor_table_base_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_user_2eproto_sccs[1] = {
  &scc_info_User_user_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_user_2eproto_once;
static bool descriptor_table_user_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_user_2eproto = {
  &descriptor_table_user_2eproto_initialized, descriptor_table_protodef_user_2eproto, "user.proto", 164,
  &descriptor_table_user_2eproto_once, descriptor_table_user_2eproto_sccs, descriptor_table_user_2eproto_deps, 1, 1,
  schemas, file_default_instances, TableStruct_user_2eproto::offsets,
  file_level_metadata_user_2eproto, 1, file_level_enum_descriptors_user_2eproto, file_level_service_descriptors_user_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_user_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_user_2eproto), true);
namespace Open {

// ===================================================================

void User::InitAsDefaultInstance() {
  ::Open::_User_default_instance_._instance.get_mutable()->header_ = const_cast< ::Open::MessageHeader*>(
      ::Open::MessageHeader::internal_default_instance());
}
class User::HasBitSetters {
 public:
  static const ::Open::MessageHeader& header(const User* msg);
};

const ::Open::MessageHeader&
User::HasBitSetters::header(const User* msg) {
  return *msg->header_;
}
void User::clear_header() {
  if (GetArenaNoVirtual() == nullptr && header_ != nullptr) {
    delete header_;
  }
  header_ = nullptr;
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int User::kHeaderFieldNumber;
const int User::kIDFieldNumber;
const int User::kNameFieldNumber;
const int User::kPasswordFieldNumber;
const int User::kAuthorityFieldNumber;
const int User::kNicknameFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

User::User()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Open.User)
}
User::User(const User& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      nickname_(from.nickname_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from.name().size() > 0) {
    name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  password_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from.password().size() > 0) {
    password_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.password_);
  }
  authority_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from.authority().size() > 0) {
    authority_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.authority_);
  }
  if (from.has_header()) {
    header_ = new ::Open::MessageHeader(*from.header_);
  } else {
    header_ = nullptr;
  }
  id_ = from.id_;
  // @@protoc_insertion_point(copy_constructor:Open.User)
}

void User::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_User_user_2eproto.base);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  password_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  authority_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&header_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&id_) -
      reinterpret_cast<char*>(&header_)) + sizeof(id_));
}

User::~User() {
  // @@protoc_insertion_point(destructor:Open.User)
  SharedDtor();
}

void User::SharedDtor() {
  name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  password_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  authority_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete header_;
}

void User::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const User& User::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_User_user_2eproto.base);
  return *internal_default_instance();
}


void User::Clear() {
// @@protoc_insertion_point(message_clear_start:Open.User)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  nickname_.Clear();
  name_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  password_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  authority_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (GetArenaNoVirtual() == nullptr && header_ != nullptr) {
    delete header_;
  }
  header_ = nullptr;
  id_ = 0;
  _internal_metadata_.Clear();
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
const char* User::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // .Open.MessageHeader header = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr = ctx->ParseMessage(mutable_header(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // int32 ID = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string Name = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParserUTF8(mutable_name(), ptr, ctx, "Open.User.Name");
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string Password = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParserUTF8(mutable_password(), ptr, ctx, "Open.User.Password");
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string Authority = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 42)) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParserUTF8(mutable_authority(), ptr, ctx, "Open.User.Authority");
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // repeated string Nickname = 6;
      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 50)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParserUTF8(add_nickname(), ptr, ctx, "Open.User.Nickname");
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint8>(ptr) == 50);
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
bool User::MergePartialFromCodedStream(
    ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
  ::PROTOBUF_NAMESPACE_ID::uint32 tag;
  // @@protoc_insertion_point(parse_start:Open.User)
  for (;;) {
    ::std::pair<::PROTOBUF_NAMESPACE_ID::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // .Open.MessageHeader header = 1;
      case 1: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (10 & 0xFF)) {
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadMessage(
               input, mutable_header()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // int32 ID = 2;
      case 2: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (16 & 0xFF)) {

          DO_((::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadPrimitive<
                   ::PROTOBUF_NAMESPACE_ID::int32, ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::TYPE_INT32>(
                 input, &id_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string Name = 3;
      case 3: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (26 & 0xFF)) {
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadString(
                input, this->mutable_name()));
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
            this->name().data(), static_cast<int>(this->name().length()),
            ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::PARSE,
            "Open.User.Name"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string Password = 4;
      case 4: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (34 & 0xFF)) {
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadString(
                input, this->mutable_password()));
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
            this->password().data(), static_cast<int>(this->password().length()),
            ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::PARSE,
            "Open.User.Password"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string Authority = 5;
      case 5: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (42 & 0xFF)) {
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadString(
                input, this->mutable_authority()));
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
            this->authority().data(), static_cast<int>(this->authority().length()),
            ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::PARSE,
            "Open.User.Authority"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // repeated string Nickname = 6;
      case 6: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (50 & 0xFF)) {
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadString(
                input, this->add_nickname()));
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
            this->nickname(this->nickname_size() - 1).data(),
            static_cast<int>(this->nickname(this->nickname_size() - 1).length()),
            ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::PARSE,
            "Open.User.Nickname"));
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
  // @@protoc_insertion_point(parse_success:Open.User)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Open.User)
  return false;
#undef DO_
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

void User::SerializeWithCachedSizes(
    ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Open.User)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .Open.MessageHeader header = 1;
  if (this->has_header()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, HasBitSetters::header(this), output);
  }

  // int32 ID = 2;
  if (this->id() != 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32(2, this->id(), output);
  }

  // string Name = 3;
  if (this->name().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->name().data(), static_cast<int>(this->name().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Open.User.Name");
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->name(), output);
  }

  // string Password = 4;
  if (this->password().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->password().data(), static_cast<int>(this->password().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Open.User.Password");
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringMaybeAliased(
      4, this->password(), output);
  }

  // string Authority = 5;
  if (this->authority().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->authority().data(), static_cast<int>(this->authority().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Open.User.Authority");
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringMaybeAliased(
      5, this->authority(), output);
  }

  // repeated string Nickname = 6;
  for (int i = 0, n = this->nickname_size(); i < n; i++) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->nickname(i).data(), static_cast<int>(this->nickname(i).length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Open.User.Nickname");
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteString(
      6, this->nickname(i), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:Open.User)
}

::PROTOBUF_NAMESPACE_ID::uint8* User::InternalSerializeWithCachedSizesToArray(
    ::PROTOBUF_NAMESPACE_ID::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:Open.User)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .Open.MessageHeader header = 1;
  if (this->has_header()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessageToArray(
        1, HasBitSetters::header(this), target);
  }

  // int32 ID = 2;
  if (this->id() != 0) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(2, this->id(), target);
  }

  // string Name = 3;
  if (this->name().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->name().data(), static_cast<int>(this->name().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Open.User.Name");
    target =
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringToArray(
        3, this->name(), target);
  }

  // string Password = 4;
  if (this->password().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->password().data(), static_cast<int>(this->password().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Open.User.Password");
    target =
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringToArray(
        4, this->password(), target);
  }

  // string Authority = 5;
  if (this->authority().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->authority().data(), static_cast<int>(this->authority().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Open.User.Authority");
    target =
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringToArray(
        5, this->authority(), target);
  }

  // repeated string Nickname = 6;
  for (int i = 0, n = this->nickname_size(); i < n; i++) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->nickname(i).data(), static_cast<int>(this->nickname(i).length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "Open.User.Nickname");
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      WriteStringToArray(6, this->nickname(i), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Open.User)
  return target;
}

size_t User::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Open.User)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated string Nickname = 6;
  total_size += 1 *
      ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->nickname_size());
  for (int i = 0, n = this->nickname_size(); i < n; i++) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
      this->nickname(i));
  }

  // string Name = 3;
  if (this->name().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->name());
  }

  // string Password = 4;
  if (this->password().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->password());
  }

  // string Authority = 5;
  if (this->authority().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->authority());
  }

  // .Open.MessageHeader header = 1;
  if (this->has_header()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *header_);
  }

  // int32 ID = 2;
  if (this->id() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->id());
  }

  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void User::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Open.User)
  GOOGLE_DCHECK_NE(&from, this);
  const User* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<User>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Open.User)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Open.User)
    MergeFrom(*source);
  }
}

void User::MergeFrom(const User& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Open.User)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  nickname_.MergeFrom(from.nickname_);
  if (from.name().size() > 0) {

    name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  if (from.password().size() > 0) {

    password_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.password_);
  }
  if (from.authority().size() > 0) {

    authority_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.authority_);
  }
  if (from.has_header()) {
    mutable_header()->::Open::MessageHeader::MergeFrom(from.header());
  }
  if (from.id() != 0) {
    set_id(from.id());
  }
}

void User::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Open.User)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void User::CopyFrom(const User& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Open.User)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool User::IsInitialized() const {
  return true;
}

void User::Swap(User* other) {
  if (other == this) return;
  InternalSwap(other);
}
void User::InternalSwap(User* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  nickname_.InternalSwap(CastToBase(&other->nickname_));
  name_.Swap(&other->name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  password_.Swap(&other->password_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  authority_.Swap(&other->authority_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(header_, other->header_);
  swap(id_, other->id_);
}

::PROTOBUF_NAMESPACE_ID::Metadata User::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace Open
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Open::User* Arena::CreateMaybeMessage< ::Open::User >(Arena* arena) {
  return Arena::CreateInternal< ::Open::User >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
