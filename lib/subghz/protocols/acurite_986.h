#pragma once

#include <lib/subghz/protocols/base.h>

#include <lib/subghz/blocks/const.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/encoder.h>
#include <lib/subghz/blocks/generic.h>
#include <lib/subghz/blocks/math.h>

#define subghz_protocol_ACURITE_986_NAME "Acurite-986"

typedef struct subghz_protocolDecoderAcurite_986 subghz_protocolDecoderAcurite_986;
typedef struct subghz_protocolEncoderAcurite_986 subghz_protocolEncoderAcurite_986;

extern const SubGhzProtocolDecoder subghz_protocol_acurite_986_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_acurite_986_encoder;
extern const SubGhzProtocol subghz_protocol_acurite_986;

/**
 * Allocate subghz_protocolDecoderAcurite_986.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return subghz_protocolDecoderAcurite_986* pointer to a subghz_protocolDecoderAcurite_986 instance
 */
void* subghz_protocol_decoder_acurite_986_alloc(SubGhzEnvironment* environment);

/**
 * Free subghz_protocolDecoderAcurite_986.
 * @param context Pointer to a subghz_protocolDecoderAcurite_986 instance
 */
void subghz_protocol_decoder_acurite_986_free(void* context);

/**
 * Reset decoder subghz_protocolDecoderAcurite_986.
 * @param context Pointer to a subghz_protocolDecoderAcurite_986 instance
 */
void subghz_protocol_decoder_acurite_986_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a subghz_protocolDecoderAcurite_986 instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_acurite_986_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a subghz_protocolDecoderAcurite_986 instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_acurite_986_get_hash_data(void* context);

/**
 * Serialize data subghz_protocolDecoderAcurite_986.
 * @param context Pointer to a subghz_protocolDecoderAcurite_986 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_decoder_acurite_986_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data subghz_protocolDecoderAcurite_986.
 * @param context Pointer to a subghz_protocolDecoderAcurite_986 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus
    subghz_protocol_decoder_acurite_986_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a subghz_protocolDecoderAcurite_986 instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_acurite_986_get_string(void* context, FuriString* output);
