/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: response.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "response.pb-c.h"
void   navirice__proto__response__init
                     (Navirice__Proto__Response         *message)
{
  static const Navirice__Proto__Response init_value = NAVIRICE__PROTO__RESPONSE__INIT;
  *message = init_value;
}
size_t navirice__proto__response__get_packed_size
                     (const Navirice__Proto__Response *message)
{
  assert(message->base.descriptor == &navirice__proto__response__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t navirice__proto__response__pack
                     (const Navirice__Proto__Response *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &navirice__proto__response__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t navirice__proto__response__pack_to_buffer
                     (const Navirice__Proto__Response *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &navirice__proto__response__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Navirice__Proto__Response *
       navirice__proto__response__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Navirice__Proto__Response *)
     protobuf_c_message_unpack (&navirice__proto__response__descriptor,
                                allocator, len, data);
}
void   navirice__proto__response__free_unpacked
                     (Navirice__Proto__Response *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &navirice__proto__response__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCEnumValue navirice__proto__response__status__enum_values_by_number[3] =
{
  { "SUCCESS", "NAVIRICE__PROTO__RESPONSE__STATUS__SUCCESS", 0 },
  { "BAD_REQUEST", "NAVIRICE__PROTO__RESPONSE__STATUS__BAD_REQUEST", 1 },
  { "NOT_IMPLEMENTED", "NAVIRICE__PROTO__RESPONSE__STATUS__NOT_IMPLEMENTED", 2 },
};
static const ProtobufCIntRange navirice__proto__response__status__value_ranges[] = {
{0, 0},{0, 3}
};
static const ProtobufCEnumValueIndex navirice__proto__response__status__enum_values_by_name[3] =
{
  { "BAD_REQUEST", 1 },
  { "NOT_IMPLEMENTED", 2 },
  { "SUCCESS", 0 },
};
const ProtobufCEnumDescriptor navirice__proto__response__status__descriptor =
{
  PROTOBUF_C__ENUM_DESCRIPTOR_MAGIC,
  "navirice.proto.Response.Status",
  "Status",
  "Navirice__Proto__Response__Status",
  "navirice.proto",
  3,
  navirice__proto__response__status__enum_values_by_number,
  3,
  navirice__proto__response__status__enum_values_by_name,
  1,
  navirice__proto__response__status__value_ranges,
  NULL,NULL,NULL,NULL   /* reserved[1234] */
};
static const ProtobufCFieldDescriptor navirice__proto__response__field_descriptors[1] =
{
  {
    "status",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_ENUM,
    0,   /* quantifier_offset */
    offsetof(Navirice__Proto__Response, status),
    &navirice__proto__response__status__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned navirice__proto__response__field_indices_by_name[] = {
  0,   /* field[0] = status */
};
static const ProtobufCIntRange navirice__proto__response__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor navirice__proto__response__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "navirice.proto.Response",
  "Response",
  "Navirice__Proto__Response",
  "navirice.proto",
  sizeof(Navirice__Proto__Response),
  1,
  navirice__proto__response__field_descriptors,
  navirice__proto__response__field_indices_by_name,
  1,  navirice__proto__response__number_ranges,
  (ProtobufCMessageInit) navirice__proto__response__init,
  NULL,NULL,NULL    /* reserved[123] */
};