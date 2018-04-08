/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: requestHeader.proto */

#ifndef PROTOBUF_C_requestHeader_2eproto__INCLUDED
#define PROTOBUF_C_requestHeader_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003000 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Navirice__Proto__RequestHeader Navirice__Proto__RequestHeader;


/* --- enums --- */

typedef enum _Navirice__Proto__RequestHeader__Type {
  NAVIRICE__PROTO__REQUEST_HEADER__TYPE__CURRENT_STEP = 0,
  NAVIRICE__PROTO__REQUEST_HEADER__TYPE__CURRENT_LOCATION = 1,
  NAVIRICE__PROTO__REQUEST_HEADER__TYPE__CURRENT_ACCELERATION_FORCE = 2,
  NAVIRICE__PROTO__REQUEST_HEADER__TYPE__CURRENT_ROTATION_RATE = 3,
  NAVIRICE__PROTO__REQUEST_HEADER__TYPE__ON_BUTTON_CLICK = 4
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(NAVIRICE__PROTO__REQUEST_HEADER__TYPE)
} Navirice__Proto__RequestHeader__Type;

/* --- messages --- */

struct  _Navirice__Proto__RequestHeader
{
  ProtobufCMessage base;
  Navirice__Proto__RequestHeader__Type type;
  uint32_t length;
};
#define NAVIRICE__PROTO__REQUEST_HEADER__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&navirice__proto__request_header__descriptor) \
    , NAVIRICE__PROTO__REQUEST_HEADER__TYPE__CURRENT_STEP, 0 }


/* Navirice__Proto__RequestHeader methods */
void   navirice__proto__request_header__init
                     (Navirice__Proto__RequestHeader         *message);
size_t navirice__proto__request_header__get_packed_size
                     (const Navirice__Proto__RequestHeader   *message);
size_t navirice__proto__request_header__pack
                     (const Navirice__Proto__RequestHeader   *message,
                      uint8_t             *out);
size_t navirice__proto__request_header__pack_to_buffer
                     (const Navirice__Proto__RequestHeader   *message,
                      ProtobufCBuffer     *buffer);
Navirice__Proto__RequestHeader *
       navirice__proto__request_header__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   navirice__proto__request_header__free_unpacked
                     (Navirice__Proto__RequestHeader *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Navirice__Proto__RequestHeader_Closure)
                 (const Navirice__Proto__RequestHeader *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor navirice__proto__request_header__descriptor;
extern const ProtobufCEnumDescriptor    navirice__proto__request_header__type__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_requestHeader_2eproto__INCLUDED */
