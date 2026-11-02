#include "pxt.h"

#if MICROBIT_CODAL
#include "nrf.h"
#endif

namespace guitarchord {

#if MICROBIT_CODAL
    static uint16_t seq0[1];
    static uint16_t seq1[1];
    static uint16_t seq2[1];

    // パルス幅を 1/16 単位で保持します。初期値 8/16 = 50% (square)。
    static uint8_t pulseWidth16 = 8;

    // フェード用の疑似音量。GPIOの振幅は変えられないため、
    // 各音のパルス幅を割合で狭めて実効エネルギーを下げます。
    static uint8_t volumePercent = 100;

    static uint32_t top0 = 0;
    static uint32_t top1 = 0;
    static uint32_t top2 = 0;
    static uint8_t width0 = 8;
    static uint8_t width1 = 8;
    static uint8_t width2 = 8;
    static bool active0 = false;
    static bool active1 = false;
    static bool active2 = false;

    static uint16_t makeCompare(uint32_t top, uint8_t width) {
        uint32_t numerator =
            top * (uint32_t)width * (uint32_t)volumePercent;

        uint32_t compare =
            (numerator + 800UL) / 1600UL; // / (16 * 100)

        if (compare < 1)
            compare = 1;
        if (compare >= top)
            compare = top - 1;

        return (uint16_t)compare;
    }

    static void updateActiveSequences() {
        if (active0 && top0 >= 2)
            seq0[0] = makeCompare(top0, width0);
        if (active1 && top1 >= 2)
            seq1[0] = makeCompare(top1, width1);
        if (active2 && top2 >= 2)
            seq2[0] = makeCompare(top2, width2);
    }

    static void startPwm(
        NRF_PWM_Type *pwm,
        NRF_GPIO_Type *gpio,
        uint32_t port,
        uint32_t pin,
        uint16_t *seq,
        uint32_t *storedTop,
        uint8_t *storedWidth,
        bool *active,
        int frequency
    ) {
        if (frequency < 31)
            frequency = 31;
        if (frequency > 10000)
            frequency = 10000;

        pwm->TASKS_STOP = 1;
        pwm->ENABLE = 0;

        gpio->DIRSET = (1UL << pin);
        gpio->OUTCLR = (1UL << pin);

        pwm->PSEL.OUT[0] =
            (pin << PWM_PSEL_OUT_PIN_Pos) |
            (port << PWM_PSEL_OUT_PORT_Pos) |
            (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);

        pwm->PSEL.OUT[1] = 0xFFFFFFFF;
        pwm->PSEL.OUT[2] = 0xFFFFFFFF;
        pwm->PSEL.OUT[3] = 0xFFFFFFFF;

        pwm->MODE =
            (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);

        pwm->PRESCALER =
            (PWM_PRESCALER_PRESCALER_DIV_16 << PWM_PRESCALER_PRESCALER_Pos);

        uint32_t top =
            (1000000UL + (uint32_t)frequency / 2) / (uint32_t)frequency;

        if (top < 2)
            top = 2;
        if (top > 32767)
            top = 32767;

        pwm->COUNTERTOP =
            (top << PWM_COUNTERTOP_COUNTERTOP_Pos);

        pwm->DECODER =
            (PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) |
            (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);

        *storedTop = top;
        *storedWidth = pulseWidth16;
        *active = true;
        seq[0] = makeCompare(top, *storedWidth);

        pwm->SEQ[0].PTR = (uint32_t)seq;
        pwm->SEQ[0].CNT = 1;
        pwm->SEQ[0].REFRESH = 0;
        pwm->SEQ[0].ENDDELAY = 0;

        pwm->LOOP = 1;
        pwm->SHORTS = PWM_SHORTS_LOOPSDONE_SEQSTART0_Msk;

        pwm->ENABLE =
            (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);

        pwm->TASKS_SEQSTART[0] = 1;
    }

    static void stopPwm(
        NRF_PWM_Type *pwm,
        NRF_GPIO_Type *gpio,
        uint32_t pin,
        bool *active
    ) {
        pwm->TASKS_STOP = 1;
        pwm->SHORTS = 0;
        pwm->ENABLE = 0;
        gpio->OUTCLR = (1UL << pin);
        *active = false;
    }
#endif

    //%
    void setTimbreNative(int timbre) {
#if MICROBIT_CODAL
        switch (timbre) {
            case 0: pulseWidth16 = 8; break; // Square 50%
            case 1: pulseWidth16 = 6; break; // Pulse 37.5%
            case 2: pulseWidth16 = 4; break; // Pulse 25%
            case 3: pulseWidth16 = 2; break; // Pulse 12.5%
            case 4: pulseWidth16 = 1; break; // Pulse 6.25%
            default: pulseWidth16 = 8; break;
        }
#endif
    }

    //%
    void setVolumeNative(int volume) {
#if MICROBIT_CODAL
        if (volume < 1)
            volume = 1;
        if (volume > 100)
            volume = 100;

        volumePercent = (uint8_t)volume;
        updateActiveSequences();
#endif
    }

    //%
    void tone1(int frequency) {
#if MICROBIT_CODAL
        startPwm(NRF_PWM0, NRF_P0, 0, 12, seq0, &top0, &width0, &active0, frequency);
#endif
    }

    //%
    void tone2(int frequency) {
#if MICROBIT_CODAL
        startPwm(NRF_PWM1, NRF_P0, 0, 1, seq1, &top1, &width1, &active1, frequency);
#endif
    }

    //%
    void tone3(int frequency) {
#if MICROBIT_CODAL
        startPwm(NRF_PWM2, NRF_P1, 1, 2, seq2, &top2, &width2, &active2, frequency);
#endif
    }

    //%
    void stop1() {
#if MICROBIT_CODAL
        stopPwm(NRF_PWM0, NRF_P0, 12, &active0);
#endif
    }

    //%
    void stop2() {
#if MICROBIT_CODAL
        stopPwm(NRF_PWM1, NRF_P0, 1, &active1);
#endif
    }

    //%
    void stop3() {
#if MICROBIT_CODAL
        stopPwm(NRF_PWM2, NRF_P1, 2, &active2);
#endif
    }

    //%
    void stopAll() {
        stop1();
        stop2();
        stop3();
    }
}
