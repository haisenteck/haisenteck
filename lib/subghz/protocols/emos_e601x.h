#pragma once

#include "base.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#define SUBGHZ_PROTOCOL_EMOSE601X_NAME "EMOS E601x"

typedef struct subghz_protocol_DecoderEmosE601x subghz_protocol_DecoderEmosE601x;
typedef struct subghz_protocol_EncoderEmosE601x subghz_protocol_EncoderEmosE601x;

extern const SubGhzProtocolDecoder subghz_protocol_emose601x_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_emose601x_encoder;
extern const SubGhzProtocol subghz_protocol_emose601x;

/**
 * Allocate subghz_protocol_DecoderEmosE601x.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return subghz_protocol_DecoderEmosE601x* pointer to a subghz_protocol_DecoderEmosE601x instance
 */
void* subghz_protocol_decoder_emose601x_alloc(SubGhzEnvironment* environment);

/**
 * Free subghz_protocol_DecoderEmosE601x.
 * @param context Pointer to a subghz_protocol_DecoderEmosE601x instance
 */
void subghz_protocol_decoder_emose601x_free(void* context);

/**
 * Reset decoder subghz_protocol_DecoderEmosE601x.
 * @param context Pointer to a subghz_protocol_DecoderEmosE601x instance
 */
void subghz_protocol_decoder_emose601x_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a subghz_protocol_DecoderEmosE601x instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_emose601x_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a subghz_protocol_DecoderEmosE601x instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_emose601x_get_hash_data(void* context);

/**
 * Serialize data subghz_protocol_DecoderEmosE601x.
 * @param context Pointer to a subghz_protocol_DecoderEmosE601x instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_decoder_emose601x_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data subghz_protocol_DecoderEmosE601x.
 * @param context Pointer to a subghz_protocol_DecoderEmosE601x instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_decoder_emose601x_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a subghz_protocol_DecoderEmosE601x instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_emose601x_get_string(void* context, FuriString* output);
