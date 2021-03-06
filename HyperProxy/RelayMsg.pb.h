// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: RelayMsg.proto

#ifndef PROTOBUF_RelayMsg_2eproto__INCLUDED
#define PROTOBUF_RelayMsg_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_RelayMsg_2eproto();
void protobuf_AssignDesc_RelayMsg_2eproto();
void protobuf_ShutdownFile_RelayMsg_2eproto();

class RelayMsg;

// ===================================================================

class RelayMsg : public ::google::protobuf::Message {
 public:
  RelayMsg();
  virtual ~RelayMsg();
  
  RelayMsg(const RelayMsg& from);
  
  inline RelayMsg& operator=(const RelayMsg& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const RelayMsg& default_instance();
  
  void Swap(RelayMsg* other);
  
  // implements Message ----------------------------------------------
  
  RelayMsg* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RelayMsg& from);
  void MergeFrom(const RelayMsg& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required string connName = 1;
  inline bool has_connname() const;
  inline void clear_connname();
  static const int kConnNameFieldNumber = 1;
  inline const ::std::string& connname() const;
  inline void set_connname(const ::std::string& value);
  inline void set_connname(const char* value);
  inline void set_connname(const char* value, size_t size);
  inline ::std::string* mutable_connname();
  inline ::std::string* release_connname();
  
  // optional string msg = 2;
  inline bool has_msg() const;
  inline void clear_msg();
  static const int kMsgFieldNumber = 2;
  inline const ::std::string& msg() const;
  inline void set_msg(const ::std::string& value);
  inline void set_msg(const char* value);
  inline void set_msg(const char* value, size_t size);
  inline ::std::string* mutable_msg();
  inline ::std::string* release_msg();
  
  // @@protoc_insertion_point(class_scope:RelayMsg)
 private:
  inline void set_has_connname();
  inline void clear_has_connname();
  inline void set_has_msg();
  inline void clear_has_msg();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* connname_;
  ::std::string* msg_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  friend void  protobuf_AddDesc_RelayMsg_2eproto();
  friend void protobuf_AssignDesc_RelayMsg_2eproto();
  friend void protobuf_ShutdownFile_RelayMsg_2eproto();
  
  void InitAsDefaultInstance();
  static RelayMsg* default_instance_;
};
// ===================================================================


// ===================================================================

// RelayMsg

// required string connName = 1;
inline bool RelayMsg::has_connname() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RelayMsg::set_has_connname() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RelayMsg::clear_has_connname() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RelayMsg::clear_connname() {
  if (connname_ != &::google::protobuf::internal::kEmptyString) {
    connname_->clear();
  }
  clear_has_connname();
}
inline const ::std::string& RelayMsg::connname() const {
  return *connname_;
}
inline void RelayMsg::set_connname(const ::std::string& value) {
  set_has_connname();
  if (connname_ == &::google::protobuf::internal::kEmptyString) {
    connname_ = new ::std::string;
  }
  connname_->assign(value);
}
inline void RelayMsg::set_connname(const char* value) {
  set_has_connname();
  if (connname_ == &::google::protobuf::internal::kEmptyString) {
    connname_ = new ::std::string;
  }
  connname_->assign(value);
}
inline void RelayMsg::set_connname(const char* value, size_t size) {
  set_has_connname();
  if (connname_ == &::google::protobuf::internal::kEmptyString) {
    connname_ = new ::std::string;
  }
  connname_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RelayMsg::mutable_connname() {
  set_has_connname();
  if (connname_ == &::google::protobuf::internal::kEmptyString) {
    connname_ = new ::std::string;
  }
  return connname_;
}
inline ::std::string* RelayMsg::release_connname() {
  clear_has_connname();
  if (connname_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = connname_;
    connname_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string msg = 2;
inline bool RelayMsg::has_msg() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void RelayMsg::set_has_msg() {
  _has_bits_[0] |= 0x00000002u;
}
inline void RelayMsg::clear_has_msg() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void RelayMsg::clear_msg() {
  if (msg_ != &::google::protobuf::internal::kEmptyString) {
    msg_->clear();
  }
  clear_has_msg();
}
inline const ::std::string& RelayMsg::msg() const {
  return *msg_;
}
inline void RelayMsg::set_msg(const ::std::string& value) {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::kEmptyString) {
    msg_ = new ::std::string;
  }
  msg_->assign(value);
}
inline void RelayMsg::set_msg(const char* value) {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::kEmptyString) {
    msg_ = new ::std::string;
  }
  msg_->assign(value);
}
inline void RelayMsg::set_msg(const char* value, size_t size) {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::kEmptyString) {
    msg_ = new ::std::string;
  }
  msg_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RelayMsg::mutable_msg() {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::kEmptyString) {
    msg_ = new ::std::string;
  }
  return msg_;
}
inline ::std::string* RelayMsg::release_msg() {
  clear_has_msg();
  if (msg_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = msg_;
    msg_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_RelayMsg_2eproto__INCLUDED
