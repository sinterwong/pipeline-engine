// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: basicMessage.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_basicMessage_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_basicMessage_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021001 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_basicMessage_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_basicMessage_2eproto {
  static const uint32_t offsets[];
};
namespace tutorial {
class BasicMessage;
struct BasicMessageDefaultTypeInternal;
extern BasicMessageDefaultTypeInternal _BasicMessage_default_instance_;
}  // namespace tutorial
PROTOBUF_NAMESPACE_OPEN
template<> ::tutorial::BasicMessage* Arena::CreateMaybeMessage<::tutorial::BasicMessage>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace tutorial {

// ===================================================================

class BasicMessage final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:tutorial.BasicMessage) */ {
 public:
  inline BasicMessage() : BasicMessage(nullptr) {}
  ~BasicMessage() override;
  explicit PROTOBUF_CONSTEXPR BasicMessage(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  BasicMessage(const BasicMessage& from);
  BasicMessage(BasicMessage&& from) noexcept
    : BasicMessage() {
    *this = ::std::move(from);
  }

  inline BasicMessage& operator=(const BasicMessage& from) {
    CopyFrom(from);
    return *this;
  }
  inline BasicMessage& operator=(BasicMessage&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const BasicMessage& default_instance() {
    return *internal_default_instance();
  }
  static inline const BasicMessage* internal_default_instance() {
    return reinterpret_cast<const BasicMessage*>(
               &_BasicMessage_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(BasicMessage& a, BasicMessage& b) {
    a.Swap(&b);
  }
  inline void Swap(BasicMessage* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(BasicMessage* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  BasicMessage* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<BasicMessage>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const BasicMessage& from);
  void MergeFrom(const BasicMessage& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(BasicMessage* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "tutorial.BasicMessage";
  }
  protected:
  explicit BasicMessage(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kSendFieldNumber = 1,
    kRecvFieldNumber = 2,
    kTypeFieldNumber = 3,
    kMessageFieldNumber = 4,
  };
  // string send = 1;
  void clear_send();
  const std::string& send() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_send(ArgT0&& arg0, ArgT... args);
  std::string* mutable_send();
  PROTOBUF_NODISCARD std::string* release_send();
  void set_allocated_send(std::string* send);
  private:
  const std::string& _internal_send() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_send(const std::string& value);
  std::string* _internal_mutable_send();
  public:

  // string recv = 2;
  void clear_recv();
  const std::string& recv() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_recv(ArgT0&& arg0, ArgT... args);
  std::string* mutable_recv();
  PROTOBUF_NODISCARD std::string* release_recv();
  void set_allocated_recv(std::string* recv);
  private:
  const std::string& _internal_recv() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_recv(const std::string& value);
  std::string* _internal_mutable_recv();
  public:

  // string type = 3;
  void clear_type();
  const std::string& type() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_type(ArgT0&& arg0, ArgT... args);
  std::string* mutable_type();
  PROTOBUF_NODISCARD std::string* release_type();
  void set_allocated_type(std::string* type);
  private:
  const std::string& _internal_type() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_type(const std::string& value);
  std::string* _internal_mutable_type();
  public:

  // bytes message = 4;
  void clear_message();
  const std::string& message() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_message(ArgT0&& arg0, ArgT... args);
  std::string* mutable_message();
  PROTOBUF_NODISCARD std::string* release_message();
  void set_allocated_message(std::string* message);
  private:
  const std::string& _internal_message() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_message(const std::string& value);
  std::string* _internal_mutable_message();
  public:

  // @@protoc_insertion_point(class_scope:tutorial.BasicMessage)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr send_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr recv_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr type_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr message_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_basicMessage_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// BasicMessage

// string send = 1;
inline void BasicMessage::clear_send() {
  _impl_.send_.ClearToEmpty();
}
inline const std::string& BasicMessage::send() const {
  // @@protoc_insertion_point(field_get:tutorial.BasicMessage.send)
  return _internal_send();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void BasicMessage::set_send(ArgT0&& arg0, ArgT... args) {
 
 _impl_.send_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:tutorial.BasicMessage.send)
}
inline std::string* BasicMessage::mutable_send() {
  std::string* _s = _internal_mutable_send();
  // @@protoc_insertion_point(field_mutable:tutorial.BasicMessage.send)
  return _s;
}
inline const std::string& BasicMessage::_internal_send() const {
  return _impl_.send_.Get();
}
inline void BasicMessage::_internal_set_send(const std::string& value) {
  
  _impl_.send_.Set(value, GetArenaForAllocation());
}
inline std::string* BasicMessage::_internal_mutable_send() {
  
  return _impl_.send_.Mutable(GetArenaForAllocation());
}
inline std::string* BasicMessage::release_send() {
  // @@protoc_insertion_point(field_release:tutorial.BasicMessage.send)
  return _impl_.send_.Release();
}
inline void BasicMessage::set_allocated_send(std::string* send) {
  if (send != nullptr) {
    
  } else {
    
  }
  _impl_.send_.SetAllocated(send, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.send_.IsDefault()) {
    _impl_.send_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:tutorial.BasicMessage.send)
}

// string recv = 2;
inline void BasicMessage::clear_recv() {
  _impl_.recv_.ClearToEmpty();
}
inline const std::string& BasicMessage::recv() const {
  // @@protoc_insertion_point(field_get:tutorial.BasicMessage.recv)
  return _internal_recv();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void BasicMessage::set_recv(ArgT0&& arg0, ArgT... args) {
 
 _impl_.recv_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:tutorial.BasicMessage.recv)
}
inline std::string* BasicMessage::mutable_recv() {
  std::string* _s = _internal_mutable_recv();
  // @@protoc_insertion_point(field_mutable:tutorial.BasicMessage.recv)
  return _s;
}
inline const std::string& BasicMessage::_internal_recv() const {
  return _impl_.recv_.Get();
}
inline void BasicMessage::_internal_set_recv(const std::string& value) {
  
  _impl_.recv_.Set(value, GetArenaForAllocation());
}
inline std::string* BasicMessage::_internal_mutable_recv() {
  
  return _impl_.recv_.Mutable(GetArenaForAllocation());
}
inline std::string* BasicMessage::release_recv() {
  // @@protoc_insertion_point(field_release:tutorial.BasicMessage.recv)
  return _impl_.recv_.Release();
}
inline void BasicMessage::set_allocated_recv(std::string* recv) {
  if (recv != nullptr) {
    
  } else {
    
  }
  _impl_.recv_.SetAllocated(recv, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.recv_.IsDefault()) {
    _impl_.recv_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:tutorial.BasicMessage.recv)
}

// string type = 3;
inline void BasicMessage::clear_type() {
  _impl_.type_.ClearToEmpty();
}
inline const std::string& BasicMessage::type() const {
  // @@protoc_insertion_point(field_get:tutorial.BasicMessage.type)
  return _internal_type();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void BasicMessage::set_type(ArgT0&& arg0, ArgT... args) {
 
 _impl_.type_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:tutorial.BasicMessage.type)
}
inline std::string* BasicMessage::mutable_type() {
  std::string* _s = _internal_mutable_type();
  // @@protoc_insertion_point(field_mutable:tutorial.BasicMessage.type)
  return _s;
}
inline const std::string& BasicMessage::_internal_type() const {
  return _impl_.type_.Get();
}
inline void BasicMessage::_internal_set_type(const std::string& value) {
  
  _impl_.type_.Set(value, GetArenaForAllocation());
}
inline std::string* BasicMessage::_internal_mutable_type() {
  
  return _impl_.type_.Mutable(GetArenaForAllocation());
}
inline std::string* BasicMessage::release_type() {
  // @@protoc_insertion_point(field_release:tutorial.BasicMessage.type)
  return _impl_.type_.Release();
}
inline void BasicMessage::set_allocated_type(std::string* type) {
  if (type != nullptr) {
    
  } else {
    
  }
  _impl_.type_.SetAllocated(type, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.type_.IsDefault()) {
    _impl_.type_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:tutorial.BasicMessage.type)
}

// bytes message = 4;
inline void BasicMessage::clear_message() {
  _impl_.message_.ClearToEmpty();
}
inline const std::string& BasicMessage::message() const {
  // @@protoc_insertion_point(field_get:tutorial.BasicMessage.message)
  return _internal_message();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void BasicMessage::set_message(ArgT0&& arg0, ArgT... args) {
 
 _impl_.message_.SetBytes(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:tutorial.BasicMessage.message)
}
inline std::string* BasicMessage::mutable_message() {
  std::string* _s = _internal_mutable_message();
  // @@protoc_insertion_point(field_mutable:tutorial.BasicMessage.message)
  return _s;
}
inline const std::string& BasicMessage::_internal_message() const {
  return _impl_.message_.Get();
}
inline void BasicMessage::_internal_set_message(const std::string& value) {
  
  _impl_.message_.Set(value, GetArenaForAllocation());
}
inline std::string* BasicMessage::_internal_mutable_message() {
  
  return _impl_.message_.Mutable(GetArenaForAllocation());
}
inline std::string* BasicMessage::release_message() {
  // @@protoc_insertion_point(field_release:tutorial.BasicMessage.message)
  return _impl_.message_.Release();
}
inline void BasicMessage::set_allocated_message(std::string* message) {
  if (message != nullptr) {
    
  } else {
    
  }
  _impl_.message_.SetAllocated(message, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.message_.IsDefault()) {
    _impl_.message_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:tutorial.BasicMessage.message)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace tutorial

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_basicMessage_2eproto
