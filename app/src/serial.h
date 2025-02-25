#pragma once
/***************************************************************************
 * Global print functions for debugging.
 * 
 * Printable parameters either have to be strings, integers or doubles.
 * 
 * In case of integers an optional second parameter can be set,
 * which converts the number into its hexadecimal form.
 * 
 * In case of doubles an optional second parameter can be set,
 * which controls the number of decimal places.
 * 
 * println() appends a '\n' character at the end of the print.
 ***************************************************************************/
#include "usart.h"
#include <string>
#include <stdio.h>

namespace uart_debug{

    void print(std::string str){
        uint8_t buffer[256];
        uint8_t size = sprintf((char*)buffer,  "%s", str.c_str());
        HAL_UART_Transmit(&huart2, buffer, size, HAL_MAX_DELAY);
    }

    void print(int i, bool hexa = false){
        uint8_t buffer[256];
        if(hexa){
            uint8_t size = sprintf((char*)buffer,  "0x%X", i);
            HAL_UART_Transmit(&huart2, buffer, size, HAL_MAX_DELAY);
            return;
        }
        uint8_t size = sprintf((char*)buffer,  "%d", i);
        HAL_UART_Transmit(&huart2, buffer, size, HAL_MAX_DELAY);
    }

    void print(double d, uint8_t precision = 2){
        uint8_t buffer[256];
        uint8_t size = sprintf((char*)buffer,  "%.*f", precision, d);
        HAL_UART_Transmit(&huart2, buffer, size, HAL_MAX_DELAY);
    }

    void println(std::string str){
        uint8_t buffer[256];
        uint8_t size = sprintf((char*)buffer,  "%s\n", str.c_str());
        HAL_UART_Transmit(&huart2, buffer, size, HAL_MAX_DELAY);
    }

    void println(int i, bool hexa = false){
        uint8_t buffer[256];
        if(hexa){
            uint8_t size = sprintf((char*)buffer,  "0x%X\n", i);
            HAL_UART_Transmit(&huart2, buffer, size, HAL_MAX_DELAY);
            return;
        }
        uint8_t size = sprintf((char*)buffer,  "%d\n", i);
        HAL_UART_Transmit(&huart2, buffer, size, HAL_MAX_DELAY);
    }

    void println(double d, uint8_t precision = 2){
        uint8_t buffer[256];
        uint8_t size = sprintf((char*)buffer,  "%.*f\n", precision, d);
        HAL_UART_Transmit(&huart2, buffer, size, HAL_MAX_DELAY);
    }

    void println(void){
        HAL_UART_Transmit(&huart2, (uint8_t*)"\n", 1, HAL_MAX_DELAY);
    }
}