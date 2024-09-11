#include "acurite_986.h"

#define TAG "subghz_protocolAcurite_986"

// PORTING E PICCOLE MODIFICHE A CURA DI HAISENTECK
/*
 * Help
 * https://github.com/merbanan/rtl_433/blob/5bef4e43133ac4c0e2d18d36f87c52b4f9458453/src/devices/acurite.c#L1644
 *
 *     0110 0100 | 1010 1011 | 0110 0010 | 0000 0000 | 0111 0110
 *     tttt tttt | IIII IIII | iiii iiii | nbuu uuuu | cccc cccc
 * - t: temperature in °F
 * - I: identification (high byte)
 * - i: identification (low byte)
 * - n: sensor number
 * - b: battery low flag to indicate low battery voltage
 * - u: unknown
 * - c: CRC (CRC-8 poly 0x07, little-endian)
 *
 *  bits are sent and shown above LSB first
 *  identification changes on battery switch
 */

static const SubGhzBlockConst subghz_protocol_acurite_986_const = {
    .te_short = 800,
    .te_long = 1750,
    .te_delta = 50,
    .min_count_bit_for_found = 40,
};

struct subghz_protocolDecoderAcurite_986 {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;
};

struct subghz_protocolEncoderAcurite_986 {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    Acurite_986DecoderStepReset = 0,
    Acurite_986DecoderStepSync1,
    Acurite_986DecoderStepSync2,
    Acurite_986DecoderStepSync3,
    Acurite_986DecoderStepSaveDuration,
    Acurite_986DecoderStepCheckDuration,
} Acurite_986DecoderStep;

const SubGhzProtocolDecoder subghz_protocol_acurite_986_decoder = {
    .alloc = subghz_protocol_decoder_acurite_986_alloc,
    .free = subghz_protocol_decoder_acurite_986_free,

    .feed = subghz_protocol_decoder_acurite_986_feed,
    .reset = subghz_protocol_decoder_acurite_986_reset,

    .get_hash_data = subghz_protocol_decoder_acurite_986_get_hash_data,
    .serialize = subghz_protocol_decoder_acurite_986_serialize,
    .deserialize = subghz_protocol_decoder_acurite_986_deserialize,
    .get_string = subghz_protocol_decoder_acurite_986_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_acurite_986_encoder = {
    .alloc = NULL,
    .free = NULL,

    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol subghz_protocol_acurite_986 = {
    .name = subghz_protocol_ACURITE_986_NAME,
    .type = SubGhzProtocolWeatherStation,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 | SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save,

    .decoder = &subghz_protocol_acurite_986_decoder,
    .encoder = &subghz_protocol_acurite_986_encoder,
};

void* subghz_protocol_decoder_acurite_986_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    subghz_protocolDecoderAcurite_986* instance = malloc(sizeof(subghz_protocolDecoderAcurite_986));
    instance->base.protocol = &subghz_protocol_acurite_986;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void subghz_protocol_decoder_acurite_986_free(void* context) {
    furi_assert(context);
    subghz_protocolDecoderAcurite_986* instance = context;
    free(instance);
}

void subghz_protocol_decoder_acurite_986_reset(void* context) {
    furi_assert(context);
    subghz_protocolDecoderAcurite_986* instance = context;
    instance->decoder.parser_step = Acurite_986DecoderStepReset;
}

static bool subghz_protocol_acurite_986_check(subghz_protocolDecoderAcurite_986* instance) {
    if(!instance->decoder.decode_data) return false;
    uint8_t msg[] = {
    instance->decoder.decode_data >> 32,
    instance->decoder.decode_data >> 24,
    instance->decoder.decode_data >> 16,
    instance->decoder.decode_data >> 8 };

    uint8_t crc = subghz_protocol_blocks_crc8(msg, 4, 0x07, 0x00);
    return (crc == (instance->decoder.decode_data & 0xFF));
}

/**
 * Analysis of received data
 * @param instance Pointer to a SubGhzBlockGeneric* instance
 */
static void subghz_protocol_acurite_986_remote_controller(SubGhzBlockGeneric* instance) {
    int temp;

    instance->id = subghz_protocol_blocks_reverse_key(instance->data >> 24, 8);
    instance->id = (instance->id << 8) | subghz_protocol_blocks_reverse_key(instance->data >> 16, 8);
    instance->battery_low = (instance->data >> 14) & 1;
    instance->channel = ((instance->data >> 15) & 1) + 1;

    temp = subghz_protocol_blocks_reverse_key(instance->data >> 32, 8);
    if(temp & 0x80) {
        temp = -(temp & 0x7F);
    }
    instance->temp = locale_fahrenheit_to_celsius((float)temp);
    instance->btn = WS_NO_BTN;
    instance->humidity = WS_NO_HUMIDITY;
}

void subghz_protocol_decoder_acurite_986_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    subghz_protocolDecoderAcurite_986* instance = context;

    switch(instance->decoder.parser_step) {
    case Acurite_986DecoderStepReset:
        if((!level) && (DURATION_DIFF(duration, subghz_protocol_acurite_986_const.te_long) <
                        subghz_protocol_acurite_986_const.te_delta * 15)) {
            //Found 1st sync bit
            instance->decoder.parser_step = Acurite_986DecoderStepSync1;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
        }
        break;

    case Acurite_986DecoderStepSync1:
        if(DURATION_DIFF(duration, subghz_protocol_acurite_986_const.te_long) <
                         subghz_protocol_acurite_986_const.te_delta * 15) {
            if(!level) {
                instance->decoder.parser_step = Acurite_986DecoderStepSync2;
            }
        } else {
            instance->decoder.parser_step = Acurite_986DecoderStepReset;
        }
        break;

    case Acurite_986DecoderStepSync2:
        if(DURATION_DIFF(duration, subghz_protocol_acurite_986_const.te_long) <
                         subghz_protocol_acurite_986_const.te_delta * 15) {
            if(!level) {
                instance->decoder.parser_step = Acurite_986DecoderStepSync3;
            }
        } else {
            instance->decoder.parser_step = Acurite_986DecoderStepReset;
        }
        break;

    case Acurite_986DecoderStepSync3:
        if(DURATION_DIFF(duration, subghz_protocol_acurite_986_const.te_long) <
                         subghz_protocol_acurite_986_const.te_delta * 15) {
            if(!level) {
                instance->decoder.parser_step = Acurite_986DecoderStepSaveDuration;
            }
        } else {
            instance->decoder.parser_step = Acurite_986DecoderStepReset;
        }
        break;

    case Acurite_986DecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = Acurite_986DecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = Acurite_986DecoderStepReset;
        }
        break;

    case Acurite_986DecoderStepCheckDuration:
        if(!level) {
            if(DURATION_DIFF(duration, subghz_protocol_acurite_986_const.te_short) <
                             subghz_protocol_acurite_986_const.te_delta * 10) {
                if(duration < subghz_protocol_acurite_986_const.te_short) {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                    instance->decoder.parser_step = Acurite_986DecoderStepSaveDuration;
                } else {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                    instance->decoder.parser_step = Acurite_986DecoderStepSaveDuration;
                }
            } else {
                //Found syncPostfix
                instance->decoder.parser_step = Acurite_986DecoderStepReset;
                if((instance->decoder.decode_count_bit == subghz_protocol_acurite_986_const.min_count_bit_for_found) &&
                    subghz_protocol_acurite_986_check(instance)) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                    subghz_protocol_acurite_986_remote_controller(&instance->generic);
                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
            }
        } else {
            instance->decoder.parser_step = Acurite_986DecoderStepReset;
        }
        break;
    }
}

uint8_t subghz_protocol_decoder_acurite_986_get_hash_data(void* context) {
    furi_assert(context);
    subghz_protocolDecoderAcurite_986* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus subghz_protocol_decoder_acurite_986_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    subghz_protocolDecoderAcurite_986* instance = context;
    return subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    subghz_protocol_decoder_acurite_986_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    subghz_protocolDecoderAcurite_986* instance = context;
    return subghz_block_generic_deserialize_check_count_bit(
        &instance->generic,
        flipper_format,
        subghz_protocol_acurite_986_const.min_count_bit_for_found);
}

void subghz_protocol_decoder_acurite_986_get_string(void* context, FuriString* output) {
    furi_assert(context);
    subghz_protocolDecoderAcurite_986* instance = context;
    furi_string_printf(
        output,
        "%s %dbit\r\n"
        "Key:0x%lX%08lX\r\n"
        "Sn:0x%lX Ch:%d  Bat:%d\r\n"
        "Temp:%3.1f C Hum:%d%%",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        (uint32_t)(instance->generic.data >> 32),
        (uint32_t)(instance->generic.data),
        instance->generic.id,
        instance->generic.channel,
        instance->generic.battery_low,
        (double)instance->generic.temp,
        instance->generic.humidity);
}
