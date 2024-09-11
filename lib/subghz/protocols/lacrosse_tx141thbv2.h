#pragma once

#include "base.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#define subghz_protocol_LACROSSE_TX141THBV2_NAME "TX141THBv2"

typedef struct subghz_protocol_decoder_lacrosse_tx141thbv2 subghz_protocol_decoder_lacrosse_tx141thbv2;
typedef struct subghz_protocol_encoder_lacrosse_tx141thbv2 subghz_protocol_encoder_lacrosse_tx141thbv2;

extern const SubGhzProtocolDecoder subghz_protocol_lacrosse_tx141thbv2_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_lacrosse_tx141thbv2_encoder;
extern const SubGhzProtocol subghz_protocol_lacrosse_tx141thbv2;

/**
 * Allocate subghz_protocol_decoder_lacrosse_tx141thbv2.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return subghz_protocol_decoder_lacrosse_tx141thbv2* pointer to a subghz_protocol_decoder_lacrosse_tx141thbv2 instance
 */
void* subghz_protocol_decoder_lacrosse_tx141thbv2_alloc(SubGhzEnvironment* environment);

/**
 * Free subghz_protocol_decoder_lacrosse_tx141thbv2.
 * @param context Pointer to a subghz_protocol_decoder_lacrosse_tx141thbv2 instance
 */
void subghz_protocol_decoder_lacrosse_tx141thbv2_free(void* context);

/**
 * Reset decoder subghz_protocol_decoder_lacrosse_tx141thbv2.
 * @param context Pointer to a subghz_protocol_decoder_lacrosse_tx141thbv2 instance
 */
void subghz_protocol_decoder_lacrosse_tx141thbv2_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a subghz_protocol_decoder_lacrosse_tx141thbv2 instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_lacrosse_tx141thbv2_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a subghz_protocol_decoder_lacrosse_tx141thbv2 instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_lacrosse_tx141thbv2_get_hash_data(void* context);

/**
 * Serialize data subghz_protocol_decoder_lacrosse_tx141thbv2.
 * @param context Pointer to a subghz_protocol_decoder_lacrosse_tx141thbv2 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_decoder_lacrosse_tx141thbv2_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data subghz_protocol_decoder_lacrosse_tx141thbv2.
 * @param context Pointer to a subghz_protocol_decoder_lacrosse_tx141thbv2 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_decoder_lacrosse_tx141thbv2_deserialize(
    void* context,
    FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a subghz_protocol_decoder_lacrosse_tx141thbv2 instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_lacrosse_tx141thbv2_get_string(void* context, FuriString* output);


void subghz_protocol_encoder_lacrosse_tx141thbv2_stop(void* context);

SubGhzProtocolStatus subghz_protocol_encoder_lacrosse_tx141thbv2_deserialize(void* context, FlipperFormat* flipper_format);

void subghz_protocol_encoder_lacrosse_tx141thbv2_free(void* context);

LevelDuration subghz_protocol_encoder_lacrosse_tx141thbv2_yield(void* context);

void* subghz_protocol_encoder_lacrosse_tx141thbv2_alloc(SubGhzEnvironment* environment);