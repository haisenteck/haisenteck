#include "kedsum_th.h"

#define TAG "subghz_protocolKedsumTH"

/*
 * Help
 * https://github.com/merbanan/rtl_433/blob/master/src/devices/kedsum.c
 *
 *  Frame structure:
 * 
 *     Byte:      0        1        2        3        4
 *     Nibble:    1   2    3   4    5   6    7   8    9   10
 *     Type:   00 IIIIIIII BBCC++++ ttttTTTT hhhhHHHH FFFFXXXX
 * 
 * - I: unique id. changes on powercycle
 * - B: Battery state 10 = Ok, 01 = weak, 00 = bad
 * - C: channel, 00 = ch1, 10=ch3
 * - + low temp nibble
 * - t: med temp nibble
 * - T: high temp nibble
 * - h: humidity low nibble
 * - H: humidity high nibble
 * - F: flags
 * - X: CRC-4 poly 0x3 init 0x0 xor last 4 bits
 */

static const SubGhzBlockConst subghz_protocol_kedsum_th_const = {
    .te_short = 500,
    .te_long = 2000,
    .te_delta = 150,
    .min_count_bit_for_found = 42,
};

struct subghz_protocol_DecoderKedsumTH {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    uint16_t header_count;
};

struct subghz_protocol_EncoderKedsumTH {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    KedsumTHDecoderStepReset = 0,
    KedsumTHDecoderStepCheckPreambule,
    KedsumTHDecoderStepSaveDuration,
    KedsumTHDecoderStepCheckDuration,
} KedsumTHDecoderStep;

const SubGhzProtocolDecoder subghz_protocol_kedsum_th_decoder = {
    .alloc = subghz_protocol_decoder_kedsum_th_alloc,
    .free = subghz_protocol_decoder_kedsum_th_free,

    .feed = subghz_protocol_decoder_kedsum_th_feed,
    .reset = subghz_protocol_decoder_kedsum_th_reset,

    .get_hash_data = subghz_protocol_decoder_kedsum_th_get_hash_data,
    .serialize = subghz_protocol_decoder_kedsum_th_serialize,
    .deserialize = subghz_protocol_decoder_kedsum_th_deserialize,
    .get_string = subghz_protocol_decoder_kedsum_th_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_kedsum_th_encoder = {
    .alloc = NULL,
    .free = NULL,

    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol subghz_protocol_kedsum_th = {
    .name = SUBGHZ_PROTOCOL_KEDSUM_TH_NAME,
    .type = SubGhzProtocolWeatherStation,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 | SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save,

    .decoder = &subghz_protocol_kedsum_th_decoder,
    .encoder = &subghz_protocol_kedsum_th_encoder,
};

void* subghz_protocol_decoder_kedsum_th_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    subghz_protocol_DecoderKedsumTH* instance = malloc(sizeof(subghz_protocol_DecoderKedsumTH));
    instance->base.protocol = &subghz_protocol_kedsum_th;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void subghz_protocol_decoder_kedsum_th_free(void* context) {
    furi_assert(context);
    subghz_protocol_DecoderKedsumTH* instance = context;
    free(instance);
}

void subghz_protocol_decoder_kedsum_th_reset(void* context) {
    furi_assert(context);
    subghz_protocol_DecoderKedsumTH* instance = context;
    instance->decoder.parser_step = KedsumTHDecoderStepReset;
}

static bool subghz_protocol_kedsum_th_check_crc(subghz_protocol_DecoderKedsumTH* instance) {
    uint8_t msg[] = {
        instance->decoder.decode_data >> 32,
        instance->decoder.decode_data >> 24,
        instance->decoder.decode_data >> 16,
        instance->decoder.decode_data >> 8,
        instance->decoder.decode_data};

    uint8_t crc =
        subghz_protocol_blocks_crc4(msg, 4, 0x03, 0); // CRC-4 poly 0x3 init 0x0 xor last 4 bits
    crc ^= msg[4] >> 4; // last nibble is only XORed
    return (crc == (msg[4] & 0x0F));
}

/**
 * Analysis of received data
 * @param instance Pointer to a SubGhzBlockGeneric* instance
 */
static void subghz_protocol_kedsum_th_remote_controller(SubGhzBlockGeneric* instance) {
    instance->id = instance->data >> 32;
    if((instance->data >> 30) & 0x3) {
        instance->battery_low = 0;
    } else {
        instance->battery_low = 1;
    }
    instance->channel = ((instance->data >> 28) & 0x3) + 1;
    instance->btn = WS_NO_BTN;
    uint16_t temp_raw = ((instance->data >> 16) & 0x0f) << 8 |
                        ((instance->data >> 20) & 0x0f) << 4 | ((instance->data >> 24) & 0x0f);
    instance->temp = locale_fahrenheit_to_celsius(((float)temp_raw - 900.0f) / 10.0f);
    instance->humidity = ((instance->data >> 8) & 0x0f) << 4 | ((instance->data >> 12) & 0x0f);
}

void subghz_protocol_decoder_kedsum_th_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    subghz_protocol_DecoderKedsumTH* instance = context;

    switch(instance->decoder.parser_step) {
    case KedsumTHDecoderStepReset:
        if((level) && (DURATION_DIFF(duration, subghz_protocol_kedsum_th_const.te_short) <
                       subghz_protocol_kedsum_th_const.te_delta)) {
            instance->decoder.parser_step = KedsumTHDecoderStepCheckPreambule;
            instance->decoder.te_last = duration;
            instance->header_count = 0;
        }
        break;

    case KedsumTHDecoderStepCheckPreambule:
        if(level) {
            instance->decoder.te_last = duration;
        } else {
            if((DURATION_DIFF(instance->decoder.te_last, subghz_protocol_kedsum_th_const.te_short) <
                subghz_protocol_kedsum_th_const.te_delta) &&
               (DURATION_DIFF(duration, subghz_protocol_kedsum_th_const.te_long * 4) <
                subghz_protocol_kedsum_th_const.te_delta * 4)) {
                //Found preambule
                instance->header_count++;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, subghz_protocol_kedsum_th_const.te_short) <
                 subghz_protocol_kedsum_th_const.te_delta) &&
                (duration < (subghz_protocol_kedsum_th_const.te_long * 2 +
                             subghz_protocol_kedsum_th_const.te_delta * 2))) {
                //Found syncPrefix
                if(instance->header_count > 0) {
                    instance->decoder.parser_step = KedsumTHDecoderStepSaveDuration;
                    instance->decoder.decode_data = 0;
                    instance->decoder.decode_count_bit = 0;
                    if((DURATION_DIFF(
                            instance->decoder.te_last, subghz_protocol_kedsum_th_const.te_short) <
                        subghz_protocol_kedsum_th_const.te_delta) &&
                       (DURATION_DIFF(duration, subghz_protocol_kedsum_th_const.te_long) <
                        subghz_protocol_kedsum_th_const.te_delta * 2)) {
                        subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                        instance->decoder.parser_step = KedsumTHDecoderStepSaveDuration;
                    } else if(
                        (DURATION_DIFF(
                             instance->decoder.te_last, subghz_protocol_kedsum_th_const.te_short) <
                         subghz_protocol_kedsum_th_const.te_delta) &&
                        (DURATION_DIFF(duration, subghz_protocol_kedsum_th_const.te_long * 2) <
                         subghz_protocol_kedsum_th_const.te_delta * 4)) {
                        subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                        instance->decoder.parser_step = KedsumTHDecoderStepSaveDuration;
                    } else {
                        instance->decoder.parser_step = KedsumTHDecoderStepReset;
                    }
                }
            } else {
                instance->decoder.parser_step = KedsumTHDecoderStepReset;
            }
        }
        break;

    case KedsumTHDecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = KedsumTHDecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = KedsumTHDecoderStepReset;
        }
        break;

    case KedsumTHDecoderStepCheckDuration:
        if(!level) {
            if(DURATION_DIFF(duration, subghz_protocol_kedsum_th_const.te_long * 4) <
               subghz_protocol_kedsum_th_const.te_delta * 4) {
                //Found syncPostfix
                if((instance->decoder.decode_count_bit ==
                    subghz_protocol_kedsum_th_const.min_count_bit_for_found) &&
                   subghz_protocol_kedsum_th_check_crc(instance)) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                    subghz_protocol_kedsum_th_remote_controller(&instance->generic);
                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.parser_step = KedsumTHDecoderStepReset;
                break;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, subghz_protocol_kedsum_th_const.te_short) <
                 subghz_protocol_kedsum_th_const.te_delta) &&
                (DURATION_DIFF(duration, subghz_protocol_kedsum_th_const.te_long) <
                 subghz_protocol_kedsum_th_const.te_delta * 2)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                instance->decoder.parser_step = KedsumTHDecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, subghz_protocol_kedsum_th_const.te_short) <
                 subghz_protocol_kedsum_th_const.te_delta) &&
                (DURATION_DIFF(duration, subghz_protocol_kedsum_th_const.te_long * 2) <
                 subghz_protocol_kedsum_th_const.te_delta * 4)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                instance->decoder.parser_step = KedsumTHDecoderStepSaveDuration;
            } else {
                instance->decoder.parser_step = KedsumTHDecoderStepReset;
            }
        } else {
            instance->decoder.parser_step = KedsumTHDecoderStepReset;
        }
        break;
    }
}

uint8_t subghz_protocol_decoder_kedsum_th_get_hash_data(void* context) {
    furi_assert(context);
    subghz_protocol_DecoderKedsumTH* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus subghz_protocol_decoder_kedsum_th_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    subghz_protocol_DecoderKedsumTH* instance = context;
    return subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    subghz_protocol_decoder_kedsum_th_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    subghz_protocol_DecoderKedsumTH* instance = context;
    return subghz_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, subghz_protocol_kedsum_th_const.min_count_bit_for_found);
}

void subghz_protocol_decoder_kedsum_th_get_string(void* context, FuriString* output) {
    furi_assert(context);
    subghz_protocol_DecoderKedsumTH* instance = context;
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
