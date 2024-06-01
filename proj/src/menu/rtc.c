#include "rtc.h"

int rtc_hook_id = 8;
rtc_date_t date;

int(rtc_subscribe_int)(uint16_t *bit_no)
{
    if (bit_no == NULL)
    {
        return 1;
    }

    *bit_no = BIT(rtc_hook_id);

    if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &rtc_hook_id) != 0)
    {
        return 1;
    }

    return 0;
}

int(rtc_unsubscribe_int)()
{
    if (sys_irqrmpolicy(&rtc_hook_id) != 0)
    {
        return 1;
    }
    return 0;
}

int(rtc_read_date)()
{
    while (1)
    {
        uint8_t wordRegA;
        if (rtc_read_reg(RTC_REG_A, &wordRegA) != 0)
        {
            printf("Error! Couldn't read 'wordRegA' from RTC_REG_A\n");
            return 1;
        }

        if (!(wordRegA & UIP_REG_A))
        {
            break;
        }
    }

    if (rtc_set_regB_bit() != 0)
    {
        printf("Error! Couldn't set 'SET_REG_B' in RTC_REG_B\n");
        return 1;
    }

    uint8_t day, month, year, hours, minutes, seconds;

    if (rtc_read_reg(RTC_YEAR_LOC, &year) != 0)
    {
        printf("Error! Couldn't read 'year' from RTC_DAY_REG\n");
        return 1;
    }

    if (rtc_read_reg(RTC_MONTH_LOC, &month) != 0)
    {
        printf("Error! Couldn't read 'month' from RTC_MONTH_REG\n");
        return 1;
    }

    if (rtc_read_reg(RTC_DAY_LOC, &day) != 0)
    {
        printf("Error! Couldn't read 'day' from RTC_YEAR_REG\n");
        return 1;
    }

    if (rtc_read_reg(RTC_HOURS_LOC, &hours) != 0)
    {
        printf("Error! Couldn't read 'hours' from RTC_HOURS_REG\n");
        return 1;
    }

    if (rtc_read_reg(RTC_MINUTES_LOC, &minutes) != 0)
    {
        printf("Error! Couldn't read 'minutes' from RTC_MINUTES_REG\n");
        return 1;
    }

    if (rtc_read_reg(RTC_SECONDS_LOC, &seconds) != 0)
    {
        printf("Error! Couldn't read 'seconds' from RTC_SECONDS_REG\n");
        return 1;
    }

    if (rtc_clear_regB_bit() != 0)
    {
        printf("Error! Couldn't clear 'SET_REG_B' in RTC_REG_B\n");
        return 1;
    }

    date.day = rtc_bcd_to_binary(day);
    date.month = rtc_bcd_to_binary(month);
    date.year = rtc_bcd_to_binary(year);
    date.hours = rtc_bcd_to_binary(hours);
    date.minutes = rtc_bcd_to_binary(minutes);
    date.seconds = rtc_bcd_to_binary(seconds);

    return 0;
}

int(rtc_read_reg)(uint8_t reg, uint8_t *data)
{
    if (sys_outb(RTC_ADDR_REG, reg) != 0)
    {
        printf("Error! Couldn't write 'reg' to RTC_ADDR_REG\n");
        return 1;
    }

    if (util_sys_inb(RTC_DATA_REG, data) != 0)
    {
        printf("Error! Couldn't read 'data' from RTC_DATA_REG\n");
        return 1;
    }
    return 0;
}

int(rtc_write_reg)(uint8_t reg, uint8_t data)
{
    if (sys_outb(RTC_ADDR_REG, reg) != 0)
    {
        printf("Error! Couldn't write 'reg' to RTC_ADDR_REG\n");
        return 1;
    }

    if (sys_outb(RTC_DATA_REG, data) != 0)
    {
        printf("Error! Couldn't write 'data' to RTC_DATA_REG\n");
        return 1;
    }
    return 0;
}

uint8_t(rtc_bcd_to_binary)(uint8_t bcd)
{
    return ((bcd & 0x0F) + ((bcd >> 4) * 10));
}

void(rtc_ih)()
{
    uint8_t wordRegC;
    if (rtc_read_reg(RTC_REG_C, &wordRegC) != 0)
    {
        printf("Error! Couldn't read 'wordRegC' from RTC_REG_C\n");
        return;
    }

    if (wordRegC & AF_REG_C)
    {
    }

    if (wordRegC & UF_REG_C)
    {
        if (rtc_read_date() != 0)
        {
            printf("Error! Couldn't read date\n");
            return;
        }
    }
}

int(rtc_enable_alarm_int)()
{
    uint8_t wordRegB;
    if (rtc_read_reg(RTC_REG_B, &wordRegB) != 0)
    {
        printf("Error! Couldn't read 'wordRegB' from RTC_REG_B\n");
        return 1;
    }

    wordRegB |= AIE_REG_B;

    if (rtc_write_reg(RTC_REG_B, wordRegB) != 0)
    {
        printf("Error! Couldn't write 'wordRegB' to RTC_REG_B\n");
        return 1;
    }

    return 0;
}

int(rtc_disable_int)()
{
    uint8_t wordRegB;
    if (rtc_read_reg(RTC_REG_B, &wordRegB) != 0)
    {
        printf("Error! Couldn't read 'wordRegB' from RTC_REG_B\n");
        return 1;
    }

    wordRegB &= ~PIE_REG_B;
    wordRegB &= ~UIE_REG_B;
    wordRegB &= ~AIE_REG_B;

    if (rtc_write_reg(RTC_REG_B, wordRegB) != 0)
    {
        printf("Error! Couldn't write 'wordRegB' to RTC_REG_B\n");
        return 1;
    }

    return 0;
}

rtc_date_t(rtc_get_date)()
{
    return date;
}

int(rtc_set_regB_bit)()
{
    uint8_t wordRegB;
    if (rtc_read_reg(RTC_REG_B, &wordRegB) != 0)
    {
        printf("Error! Couldn't read 'wordRegB' from RTC_REG_B\n");
        return 1;
    }
    wordRegB |= SET_REG_B;
    if (rtc_write_reg(RTC_REG_B, wordRegB) != 0)
    {
        printf("Error! Couldn't write 'wordRegB' to RTC_REG_B\n");
        return 1;
    }
    return 0;
}

int(rtc_clear_regB_bit)()
{
    uint8_t wordRegB;
    if (rtc_read_reg(RTC_REG_B, &wordRegB) != 0)
    {
        printf("Error! Couldn't read 'wordRegB' from RTC_REG_B\n");
        return 1;
    }
    wordRegB &= ~SET_REG_B;
    if (rtc_write_reg(RTC_REG_B, wordRegB) != 0)
    {
        printf("Error! Couldn't write 'wordRegB' to RTC_REG_B\n");
        return 1;
    }
    return 0;
}

int(rtc_set_alarm)()
{
    while (1)
    {
        uint8_t wordRegA;
        if (rtc_read_reg(RTC_REG_A, &wordRegA) != 0)
        {
            printf("Error! Couldn't read 'wordRegA' from RTC_REG_A\n");
            return 1;
        }

        if (!(wordRegA & UIP_REG_A))
        {
            break;
        }

        tickdelay(micros_to_ticks(20000));
    }

    if (rtc_set_regB_bit() != 0)
    {
        printf("Error! Couldn't set 'SET_REG_B' in RTC_REG_B\n");
        return 1;
    }

    if (rtc_write_reg(RTC_SECONDS_ALARM_LOC, RTC_DONT_CARE) != 0)
    {
        printf("Error! Couldn't write '0' to RTC_SECONDS_LOC\n");
        return 1;
    }

    if (rtc_write_reg(RTC_MINUTES_ALARM_LOC, RTC_DONT_CARE) != 0)
    {
        printf("Error! Couldn't write '0' to RTC_MINUTES_LOC\n");
        return 1;
    }

    if (rtc_write_reg(RTC_HOURS_ALARM_LOC, RTC_DONT_CARE) != 0)
    {
        printf("Error! Couldn't write '0' to RTC_HOURS_LOC\n");
        return 1;
    }

    if (rtc_clear_regB_bit() != 0)
    {
        printf("Error! Couldn't clear 'SET_REG_B' in RTC_REG_B\n");
        return 1;
    }
    return 0;
}
