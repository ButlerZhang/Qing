// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: user.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_user_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_user_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3008000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3008000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_user_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_user_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_user_2eproto;
namespace Open {
class User;
class UserDefaultTypeInternal;
extern UserDefaultTypeInternal _User_default_instance_;
}  // namespace Open
PROTOBUF_NAMESPACE_OPEN
template<> ::Open::User* Arena::CreateMaybeMessage<::Open::User>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace Open {

// ===================================================================

class User :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Open.User) */ {
 public:
  User();
  virtual ~User();

  User(const User& from);
  User(User&& from) noexcept
    : User() {
    *this = ::std::move(from);
  }

  inline User& operator=(const User& from) {
    CopyFrom(from);
    return *this;
  }
  inline User& operator=(User&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const User& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const User* internal_default_instance() {
    return reinterpret_cast<const User*>(
               &_User_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(User* other);
  friend void swap(User& a, User& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline User* New() const final {
    return CreateMaybeMessage<User>(nullptr);
  }

  User* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<User>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const User& from);
  void MergeFrom(const User& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  #else
  bool MergePartialFromCodedStream(
      ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(User* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Open.User";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_user_2eproto);
    return ::descriptor_table_user_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated string nickname = 5;
  int nickname_size() const;
  void clear_nickname();
  static const int kNicknameFieldNumber = 5;
  const std::string& nickname(int index) const;
  std::string* mutable_nickname(int index);
  void set_nickname(int index, const std::string& value);
  void set_nickname(int index, std::string&& value);
  void set_nickname(int index, const char* value);
  void set_nickname(int index, const char* value, size_t size);
  std::string* add_nickname();
  void add_nickname(const std::string& value);
  void add_nickname(std::string&& value);
  void add_nickname(const char* value);
  void add_nickname(const char* value, size_t size);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>& nickname() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>* mutable_nickname();

  // string name = 2;
  void clear_name();
  static const int kNameFieldNumber = 2;
  const std::string& name() const;
  void set_name(const std::string& value);
  void set_name(std::string&& value);
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  std::string* mutable_name();
  std::string* release_name();
  void set_allocated_name(std::string* name);

  // string password = 3;
  void clear_password();
  static const int kPasswordFieldNumber = 3;
  const std::string& password() const;
  void set_password(const std::string& value);
  void set_password(std::string&& value);
  void set_password(const char* value);
  void set_password(const char* value, size_t size);
  std::string* mutable_password();
  std::string* release_password();
  void set_allocated_password(std::string* password);

  // string authority = 4;
  void clear_authority();
  static const int kAuthorityFieldNumber = 4;
  const std::string& authority() const;
  void set_authority(const std::string& value);
  void set_authority(std::string&& value);
  void set_authority(const char* value);
  void set_authority(const char* value, size_t size);
  std::string* mutable_authority();
  std::string* release_authority();
  void set_allocated_authority(std::string* authority);

  // int32 id = 1;
  void clear_id();
  static const int kIdFieldNumber = 1;
  ::PROTOBUF_NAMESPACE_ID::int32 id() const;
  void set_id(::PROTOBUF_NAMESPACE_ID::int32 value);

  // @@protoc_insertion_point(class_scope:Open.User)
 private:
  class HasBitSetters;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string> nickname_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr name_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr password_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr authority_;
  ::PROTOBUF_NAMESPACE_ID::int32 id_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_user_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// User

// int32 id = 1;
inline void User::clear_id() {
  id_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 User::id() const {
  // @@protoc_insertion_point(field_get:Open.User.id)
  return id_;
}
inline void User::set_id(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  id_ = value;
  // @@protoc_insertion_point(field_set:Open.User.id)
}

// string name = 2;
inline void User::clear_name() {
  name_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& User::name() const {
  // @@protoc_insertion_point(field_get:Open.User.name)
  return name_.GetNoArena();
}
inline void User::set_name(const std::string& value) {
  
  name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Open.User.name)
}
inline void User::set_name(std::string&& value) {
  
  name_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:Open.User.name)
}
inline void User::set_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Open.User.name)
}
inline void User::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Open.User.name)
}
inline std::string* User::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:Open.User.name)
  return name_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* User::release_name() {
  // @@protoc_insertion_point(field_release:Open.User.name)
  
  return name_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void User::set_allocated_name(std::string* name) {
  if (name != nullptr) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:Open.User.name)
}

// string password = 3;
inline void User::clear_password() {
  password_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& User::password() const {
  // @@protoc_insertion_point(field_get:Open.User.password)
  return password_.GetNoArena();
}
inline void User::set_password(const std::string& value) {
  
  password_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Open.User.password)
}
inline void User::set_password(std::string&& value) {
  
  password_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:Open.User.password)
}
inline void User::set_password(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  password_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Open.User.password)
}
inline void User::set_password(const char* value, size_t size) {
  
  password_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Open.User.password)
}
inline std::string* User::mutable_password() {
  
  // @@protoc_insertion_point(field_mutable:Open.User.password)
  return password_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* User::release_password() {
  // @@protoc_insertion_point(field_release:Open.User.password)
  
  return password_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void User::set_allocated_password(std::string* password) {
  if (password != nullptr) {
    
  } else {
    
  }
  password_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), password);
  // @@protoc_insertion_point(field_set_allocated:Open.User.password)
}

// string authority = 4;
inline void User::clear_authority() {
  authority_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& User::authority() const {
  // @@protoc_insertion_point(field_get:Open.User.authority)
  return authority_.GetNoArena();
}
inline void User::set_authority(const std::string& value) {
  
  authority_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Open.User.authority)
}
inline void User::set_authority(std::string&& value) {
  
  authority_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:Open.User.authority)
}
inline void User::set_authority(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  authority_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Open.User.authority)
}
inline void User::set_authority(const char* value, size_t size) {
  
  authority_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Open.User.authority)
}
inline std::string* User::mutable_authority() {
  
  // @@protoc_insertion_point(field_mutable:Open.User.authority)
  return authority_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* User::release_authority() {
  // @@protoc_insertion_point(field_release:Open.User.authority)
  
  return authority_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void User::set_allocated_authority(std::string* authority) {
  if (authority != nullptr) {
    
  } else {
    
  }
  authority_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), authority);
  // @@protoc_insertion_point(field_set_allocated:Open.User.authority)
}

// repeated string nickname = 5;
inline int User::nickname_size() const {
  return nickname_.size();
}
inline void User::clear_nickname() {
  nickname_.Clear();
}
inline const std::string& User::nickname(int index) const {
  // @@protoc_insertion_point(field_get:Open.User.nickname)
  return nickname_.Get(index);
}
inline std::string* User::mutable_nickname(int index) {
  // @@protoc_insertion_point(field_mutable:Open.User.nickname)
  return nickname_.Mutable(index);
}
inline void User::set_nickname(int index, const std::string& value) {
  // @@protoc_insertion_point(field_set:Open.User.nickname)
  nickname_.Mutable(index)->assign(value);
}
inline void User::set_nickname(int index, std::string&& value) {
  // @@protoc_insertion_point(field_set:Open.User.nickname)
  nickname_.Mutable(index)->assign(std::move(value));
}
inline void User::set_nickname(int index, const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  nickname_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:Open.User.nickname)
}
inline void User::set_nickname(int index, const char* value, size_t size) {
  nickname_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Open.User.nickname)
}
inline std::string* User::add_nickname() {
  // @@protoc_insertion_point(field_add_mutable:Open.User.nickname)
  return nickname_.Add();
}
inline void User::add_nickname(const std::string& value) {
  nickname_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:Open.User.nickname)
}
inline void User::add_nickname(std::string&& value) {
  nickname_.Add(std::move(value));
  // @@protoc_insertion_point(field_add:Open.User.nickname)
}
inline void User::add_nickname(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  nickname_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:Open.User.nickname)
}
inline void User::add_nickname(const char* value, size_t size) {
  nickname_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:Open.User.nickname)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>&
User::nickname() const {
  // @@protoc_insertion_point(field_list:Open.User.nickname)
  return nickname_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>*
User::mutable_nickname() {
  // @@protoc_insertion_point(field_mutable_list:Open.User.nickname)
  return &nickname_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace Open

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_user_2eproto
