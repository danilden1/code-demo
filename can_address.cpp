#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <iomanip>
#include <stdint.h>

//TODO: проверить функции установки двух байт. можкт быть не везде нужно байти вестами менять
//TODO: написать генератор массивов для вставки его в проект Дмитрия для STM
//TODO: написать сортировщик для распределения адрисов по циклу отправки
//TODO: написать сортировщик для распределения адрисов по циклу увиличения таймера

#define NUMBER_OF_ADDRESS 30

#define ad0x1 0
#define ad0x2 1
#define ad0x3 2
#define ad0x4 3
#define ad0x5 4
#define ad0x6 5
#define ad0x7 6
#define ad0x8 7
#define ad0x9 8
#define ad0x10 9
#define ad0x11 10
#define ad0x12 11
#define ad0x13 12
#define ad0x14 13
#define ad0x15 14
#define ad0x16 15
#define ad0x17 16
#define ad0x18 17
#define ad0x19 18
#define ad0x20 19 
#define ad0x21 20
#define ad0x22 21
#define ad0x23 22
#define ad0x24 23
#define ad0x25 24
#define ad0x26 25
#define ad0x27 26
#define ad0x28 27
#define ad0x29 28
#define ad0x30 29


#define BYTE_0 0
#define BYTE_1 1
#define BYTE_2 2
#define BYTE_3 3
#define BYTE_4 4
#define BYTE_5 5
#define BYTE_6 6
#define BYTE_7 7

#define BIT_0 0 
#define BIT_1 1 
#define BIT_2 2 
#define BIT_3 3 
#define BIT_4 4 
#define BIT_5 5 
#define BIT_6 6 
#define BIT_7 6 

#define CYC_TIMES 9

int cycles_arr[CYC_TIMES] = {
    0,
    10,
    20,
    50,
    100,
    250,
    500,
    1000,
    3000
};


typedef struct {
    uint32_t address;
    uint8_t message[8];
    uint32_t cycle; // время повторения посылки
    uint32_t time_cycle; // время обновления таймера, если 0, то аймера нет
    uint32_t timer_pos; //позиция первого бита в таймере. предполагается, что таймеры 4х битные.
    std::string comment; //содержание сообщения
}address_t;

typedef struct {
    uint32_t address;
    uint8_t message[8];
}ad_to_gen_t;

ad_to_gen_t ad_to_gen[NUMBER_OF_ADDRESS];

typedef struct {
    uint8_t byte;
    uint8_t bit;
}pair_bits_t;
//массив для применения в процедурах выставления одного бита. Нулевой бит самый правый.
uint8_t bits_arr[8] = {
    0b00000001,
    0b00000010,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b10000000
};

typedef uint32_t mes_t[8];
std::vector <address_t> m;

pair_bits_t  bitToByte(const uint32_t input_bit);
int setLower4Bites(std::vector<address_t>& a, uint32_t addr, uint32_t byte_pos, uint8_t bits_value);
int setPairOf12Bit(std::vector<address_t>& a, const uint32_t addr, const uint32_t byte_pos, const uint32_t byte_value_1, const uint32_t byte_value_2);

int printAdress(std::vector<address_t> a, uint32_t addr) {
    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            //printMessage(a[i]);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Печатает массив в адекватнм виде для 
 * использования его в проекте stm
 * @param a ссылка на вектор со всеми адресами
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int printArraysForStm(
    const std::vector<address_t> a) {

    std::cout << "--------------------- ARRAYS FOR STM ---------------------" << std::endl;

    for (int i = 0; i < a.size(); i++) {
        std::cout << "static uint8_t data0x" << std::hex << std::setfill('0') << std::setw(8) << a[i].address;
        std::cout << "[8] = {";
        for (int j = 0; j < 7; j++) {
            std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(a[i].message[j]) << ", ";
        }
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(a[i].message[7]);
        std::cout << "};";
        std::cout << "//cycle: " << std::dec << a[i].cycle << ", " << a[i].comment;
        std::cout << std::endl;
    }
    std::cout << std::endl;
    return 0;
}


int printArraysByCycle(
    const std::vector<address_t> a) {

    std::cout << "--------------------- SORT ADDRESS BY CYCLES TIME ---------------------" << std::endl;
    
    for (int i = 0; i < CYC_TIMES; i++) {
        std::cout << "_______________CYCLE_________________: " << std::dec << cycles_arr[i] << std::endl;
        for (int j = 0; j < a.size(); j++) {
            if (a[j].cycle == cycles_arr[i]) {
                std::cout << "#define ____0x" << std::hex << std::setfill('0') << std::setw(8) << a[j].address << std::endl;
                std::cout << "//" << a[j].comment << std::endl;
                std::cout << "      msg.Id = 0x" << std::hex << std::setfill('0') << std::setw(8) << a[j].address << ";" << std::endl;
                std::cout << "      msg.bStdId = 0;" << std::endl;
                std::cout << "      msg.Dlc = 8;" << std::endl;
                std::cout << "      memcpy(msg.TxData, data0x" << std::hex << std::setfill('0') << std::setw(8) << a[j].address << ", 8);" << std::endl;
                std::cout << "      osMessageQueuePut (myCanTxQueueHandle, &msg, NULL, 2);" << std::endl;
                std::cout << std::endl;

            }
        }
        std::cout << std::endl;
    }

    
    std::cout << std::endl;
    return 0;
}

/**
 * @brief Печатает сообщение в бинарном виде.
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int printMessageBin(
    const std::vector<address_t> a, 
    const uint32_t addr) {
    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            std::cout << "addr 0x" << std::hex << addr << ": ";
            for (int j = 0; j < 8; j++) {
                std::cout << "0b" << std::bitset<8>(a[i].message[j]) << ", ";
            }
            std::cout << std::endl;
            return 1;
        }
    }
    std::cout << "!!! error to print message in bin by addr = 0x" << std::hex << addr << std::endl;
    return 0;
}

/**
 * @brief Печатает сообщение в шестнадцетеричном виде.
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int printMessageHex(
    const std::vector<address_t> a, 
    const uint32_t addr) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            std::cout << "addr 0x" << std::hex << addr << ": ";
            for (int j = 0; j < 8; j++) {
                std::cout << "0x"<< std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(a[i].message[j]) << ", ";
            }
            std::cout << std::endl;
            return 1;
        }
    }
    std::cout << "!!! error to print message in hex by addr = 0x" << std::hex << addr << std::endl;
    return 0;
}
/**
 * @brief Устанавливает все 8 байт в сообщение.
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param mes[8] массив со всеми значениями сообщений
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setMessage(
    std::vector<address_t> &a, 
    uint32_t addr, 
    uint32_t mes[8]) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            for (int j = 0; j < 8; j++) {
                a[i].message[j] = mes[j];
            }
            return 0;
        }
    }
    std::cout << "!!!error set msd on addr " << addr;
    return 1;
}

/**
 * @brief Устанавливает байт в сообщение.
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param byte_pos позиция устанавливаемого байта в сообщение (0-7)
 * @param byte_value значение устанавливаемого байта
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setByte(
    std::vector<address_t>& a, 
    const uint32_t addr, 
    const uint32_t byte_pos, 
    const uint32_t byte_value) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            a[i].message[byte_pos] = byte_value;
            return 0;
        }
    }
    std::cout << "!!!error set bit on addr " << addr;
    return 1;
}

/**
 * @brief Устанавливает подряд 12 бит. может приметяться только 
 * с последуюшей установкой еще 4х бит в старшие разряды второго байта
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param bit_pos позиция младшего бита для установки. Предполагается, что данный
 * параметр всегда кратен 8.
 * @param byte_value значение устанавливаемого значенния
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */

int set12Bits(
    std::vector<address_t>& a, 
    const uint32_t addr, 
    const uint32_t bit_pos, 
    const uint32_t byte_value) {

    if (bit_pos > 50 || byte_value > 0xFFF) {
        std::cout << "!!!error set12Bits on addr " << addr;
        return 1;
    }
    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            uint32_t loc_byte_pos = bitToByte(bit_pos).byte;
            uint32_t high_half_byte = byte_value >> 8;
            uint32_t low_byte = byte_value % 0x100;
            setByte(a, addr, loc_byte_pos, low_byte);
            setLower4Bites(a, addr, loc_byte_pos + 1, high_half_byte);
            return 0;
        }
    }


}

/**
 * @brief Устанавливает бит в сообщении
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param bit_pos позиция младшего бита для установки. Предполагается, что данный
 * параметр всегда кратен 8.
 * @param byte_value значение устанавливаемого значенния
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setBit(
    std::vector<address_t>& a, uint32_t addr, uint32_t bit_pos, uint32_t byte_pos) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            a[i].message[byte_pos] |= bits_arr[bit_pos];
            return 0;
        }
    }
    std::cout << "!!!error set bit on addr " << addr;
    return 1;
}

/**
 * @brief Устанавливает младшие 4 бита в указанный байт
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param byte_pos байта для установки (0-7).
 * @param bits_value значение устанавливаемого значенния
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setLower4Bites(
    std::vector<address_t>& a, 
    const uint32_t addr,  
    const uint32_t byte_pos, 
    const uint8_t bits_value) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            a[i].message[byte_pos] |= bits_value;
            return 0;
        }
    }
    std::cout << "!!!error setLower4Bites on addr " << addr;
    return 1;
}

/**
 * @brief Устанавливает старшие 4 бита в указанный байт
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param byte_pos байта для установки (0-7).
 * @param bits_value значение устанавливаемого значенния
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setUpper4Bites(
    std::vector<address_t>& a, 
    const uint32_t addr, 
    const uint32_t byte_pos, 
    const uint8_t bits_value) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            a[i].message[byte_pos] |= (bits_value * 0x10);
            return 0;
        }
    }
    std::cout << "!!!error setUpper4Bites on addr " << addr;
    return 1;
}


/**
 * @brief Устанавливает старшие 2 бита подряд начиная с указанного бита
 * нужно для быстрого выполнения двухбитны команд
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param bite_pos начиная с какого бита следует установить (0 - 61).
 * @param bits_value значение устанавливаемого значенния
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int set2Bites(
    std::vector<address_t>& a, 
    const uint32_t addr, 
    const uint32_t bite_pos, 
    const uint8_t bits_value) {

    if (bits_value > 3 || bits_value > 62){
        std::cout << "!!!error set2Bites by input data on addr " << addr << std::endl;
    }
    uint32_t byte_pos = bite_pos / 8;
    uint32_t bit_start = bite_pos % 8;
    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            switch (bit_start){
            case 0: {
                a[i].message[byte_pos] |= bits_value;
                break;
            }
            case 2: {
                a[i].message[byte_pos] |= (bits_value << 2);
                break;
            }
            case 4: {
                a[i].message[byte_pos] |= (bits_value << 4);
                break;
            }
            case 6: {
                a[i].message[byte_pos] |= (bits_value << 6);
                break;
            }
            }
            return 0;
        }
    }
    std::cout << "!!!error set2Bites on addr " << addr;
    return 1;
}


/**
 * @brief Меняет старший и младший бит в в 16 битной переменной местами
 * нужно для 16 битных посылок
 * @param input значение для 16 битной посылки
 * @return пара перевернутых значений младший байт, старший байт
 */

std::pair<uint8_t, uint8_t>  swapByte(const uint16_t input) {
    uint32_t high_value = 0;
    uint32_t low_value = 0;
    std::pair<uint8_t, uint8_t> ret;
    high_value = input >> 8;//сдвигаем на 8 байт вправо и тем самым получаем старший байт
    low_value = input;
    low_value &= ~(0xff00);//отризаем старший байт и тем самым получаем младший байт
    ret.first = (uint8_t)(low_value);
    ret.second =  (uint8_t)(high_value);
    return ret;

}
/**
 * @brief устанавливает 2 байта переворачивая в подряд идущие ячейки сообщения. 
 * нужно для 16 битных сообщений
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param byte_pos начиная с какого байта следует установить (0 - 6).
 * @param byte_value значение устанавливаемого значенния
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setTwoByte(
    std::vector<address_t>& a,
    const uint32_t addr,
    const uint8_t byte_pos,
    const uint16_t byte_value) {
    
    if (byte_pos > 6 || byte_value > 0xFFFF){
        std::cout << "!!!error setTwoByte by input value on addr " << addr;
        return 1;
    }

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            a[i].message[byte_pos] = swapByte(byte_value).first;
            a[i].message[byte_pos + 1] = swapByte(byte_value).second;
            return 0;
        }
    }
    std::cout << "!!!error set bit on addr " << addr;
    return 1;
}

/**
 * @brief устанавливает 2 байта в не переворачивая их подряд идущие ячейки сообщения.
 * нужно для 16 битных сообщений
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param byte_pos начиная с какого байта следует установить (0 - 6).
 * @param byte_value значение устанавливаемого значенния
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setTwoByteNoChange(
    std::vector<address_t>& a, 
    const uint32_t addr, 
    const uint8_t byte_pos, 
    const uint16_t byte_value) {

    if (byte_pos > 6 || byte_value > 0xFFFF) {
        std::cout << "!!!error setTwoByteNoChange by input value on addr " << addr;
        return 1;
    }

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            a[i].message[byte_pos] = swapByte(byte_value).second;
            a[i].message[byte_pos + 1] = swapByte(byte_value).first;
            return 0;
        }
    }
    std::cout << "!!!error set bit on addr " << addr;
    return 1;
}
/**
 * @brief устанавливает время цикла отправки сообщения в мс
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param cyc время в мс
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setCycle(
    std::vector<address_t>& a, 
    const uint32_t addr, 
    const uint32_t cyc) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            a[i].cycle = cyc;
            return 0;
        }
    }
    std::cout << "!!!error set cyc on addr " << addr;
    return 1;
}
/**
 * @brief устанавливает время цикла обновления счетчика в мс.
 * при отсутствии счетчика оставить 0
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param time_cycle время в мс
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setTimeCycle(
    std::vector<address_t>& a, 
    const uint32_t addr, 
    const uint32_t time_cycle) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            a[i].time_cycle = time_cycle;
            return 0;
        }
    }
    std::cout << "!!!error set time cycle on addr " << addr;
    return 1;
}


/**
 * @brief устанавливает комментарий к адресу
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param time_cycle время в мс
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setComment(
    std::vector<address_t>& a, 
    const uint32_t addr, 
    const std::string comment) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            a[i].comment = comment;
            return 0;
        }
    }
    std::cout << "!!!error set time cycle on addr " << addr;
    return 1;
}

/**
 * @brief устанавливает позицию младщего бита таймера
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param pos позиция
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setTimerPos(
    std::vector<address_t>& a, 
    const uint32_t addr, 
    const uint32_t pos) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            a[i].timer_pos = pos;
            return 0;
        }
    }
    std::cout << "!!!error set time cycle on addr " << addr;
    return 1;
}


/**
 * @brief устанавливает пару из 12 битных значения
 * @param a ссылка на вектор со всеми адресами
 * @param addr адрес интересующего нас сообщения
 * @param byte_pos первого байта в паре сообщений (0 - 5)
 * @param byte_value_1 значение первого 12 битного поля из пары
 * @param byte_value_2 значение второго 12 битного поля из пары
 * @return возвращает 0 при выполнении функции, при ошибках 1.
 */
int setPairOf12Bit(
    std::vector<address_t>& a, 
    const uint32_t addr, 
    const uint32_t byte_pos, 
    const uint32_t byte_value_1, 
    const uint32_t byte_value_2) {

    if (byte_value_1 > 0xFFF || byte_value_2 > 0xFFF || byte_pos > 5) {
        std::cout << "!!!error setPairOf12Bit by input value on addr " << addr;
        return 1;
    }
    for (int i = 0; i < a.size(); i++) {
        if (a[i].address == addr) {
            uint32_t high_half_byte_1 = byte_value_1 >> 8;
            uint32_t low_byte_1 = byte_value_1 % 0x100;
            uint32_t low_half_byte_2 = byte_value_2 % 0x10;
            uint32_t high_byte_2 = byte_value_2 >> 4;
            setByte(a, addr, byte_pos, low_byte_1);
            setLower4Bites(a, addr, byte_pos + 1, high_half_byte_1);
            setByte(a, addr, byte_pos + 2, high_byte_2);
            setUpper4Bites(a, addr, byte_pos + 1, low_half_byte_2);
            return 0;
        }
    }
    std::cout << "!!!error setPairOf12Bit by wrong address on addr " << addr;
    return 1;
}


/**
 * @brief печатает одно сообшение по указанному адресу
 * я не использую эту функцию. уже не помню что она делает
 * @param a ссылка на вектор со всеми адресами
 */
void printMessage(address_t a) {
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(8) << a.address << " | ";
    for (int i = 0; i < 7; i++) {
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(a.message[i]) << ", ";
    }
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(a.message[7]) << " | ";
    std::cout << std::dec << std::setfill('0') << std::setw(4) << a.time_cycle << " | ";
    std::cout << std::dec << std::setfill('0') << std::setw(4) << a.cycle << " | ";
    std::cout << std::dec << std::setfill('0') << std::setw(4) << a.timer_pos << " | ";
    std::cout << a.comment;
}

/**
 * @brief печатает полностью весь вектор адресов со всеми полями
 * @param addr адрес интересующего нас сообщения
 */
void printAllData(std::vector<address_t> a) {
    std::cout << "address |                         message                         |time_cycle|cycle|timer_pos|comment" << std::endl;
    for (int i = 0; i < a.size(); i++) {
        printMessage(a[i]);
        std::cout << std::endl;
    }
}

/**
 * @brief переводит бит (0-63) в пару байт (0-7) и бит(0-7)
 * 43  -> (5, 3). Нужно для использование битового адреса в байтовыз функциях
 * bitToByte(43).byte = 5
 * bitToByte(43).bit = 3
 * @param a ссылка на вектор со всеми адресами
 */
pair_bits_t  bitToByte(const uint32_t input_bit) {
    pair_bits_t ret;
    ret.byte = input_bit / 8;
    ret.bit = input_bit % 8;
    return ret;
}

int main()
{
    //test comment
    //test comment 2
    setlocale(LC_ALL, "Russian");
    
    address_t my; // вектор всех адресов
    my.address = 0x0;
    for (int i = 0; i < 8; i++) {
        my.message[i] = 0x0; 
    }
    my.cycle = 0;
    my.time_cycle = 0;
    my.timer_pos = 0;
    my.comment = "comment";
    for (int i = 0; i < 30; i++) {
        m.push_back(my);
    }
    // delete addreses for remove NDA info
    m[ad0x1].address  = 0x0;
    m[ad0x2].address  = 0x0;
    m[ad0x3].address  = 0x0;
    m[ad0x4].address  = 0x0;
    m[ad0x5].address  = 0x0;
    m[ad0x6].address  = 0x0;
    m[ad0x7].address  = 0x0;
    m[ad0x8].address  = 0x0;
    m[ad0x9].address  = 0x0;
    m[ad0x10].address = 0x0;
    m[ad0x11].address = 0x0;
    m[ad0x12].address = 0x0;
    m[ad0x13].address = 0x0;
    m[ad0x14].address = 0x0;
    m[ad0x15].address = 0x0;
    m[ad0x16].address = 0x0;
    m[ad0x17].address = 0x0;
    m[ad0x18].address = 0x0;
    m[ad0x19].address = 0x0;
    m[ad0x20].address = 0x0;
    m[ad0x21].address = 0x0;
    m[ad0x22].address = 0x0;
    m[ad0x23].address = 0x0;
    m[ad0x24].address = 0x0;
    m[ad0x25].address = 0x0;
    m[ad0x26].address = 0x0;
    m[ad0x27].address = 0x0;
    m[ad0x28].address = 0x0;
    m[ad0x29].address = 0x0;
    m[ad0x30].address = 0x0;
    std::cout << "size: " << m.size() << std::endl;



    // начало битовой магии
    
    /*------------------------------------------------------------0x0-------------------------------------*/

    setCycle(m, 0x0, 20);   // цикл 20 мс
    setTimeCycle(m, 0x0, 100);  // таймера отличен от периуда 
    setTimerPos(m, 0x0, 60);
    setComment(m, 0x0, "информация о батареи и ошибки батареи");

    //биты 0-3 режим батареи нижние биты
    //0 = инициализировать [Соответствует 0, 1 в режиме работы от батареи]
    //1 = Отключить главный контактор(высоковольтный выключатель [Соответствует 2 или 5 в режиме работы от батареи]
    //2 = процесс предварительной зарядки [Соответствует режиму работы от батареи 2]
    //+++3 = замкнуть главный контактор(высокое напряжение замкнуто) 3 соответствует режиму работы от батаре
    //4 = Главный контактор отключен из - за неисправности, Безвозвратно[5 соответствует режиму работы от батареи]
    //5 ~ 15 = зарезервирован
    setLower4Bites(m, 0x0, BYTE_0, 3);

    //биты 4-7 режим  работы от  батареи верхние биты
    //0 = загрузка
    //1 = Самодиагностика
    //2 = ждать высокого давления
    //+++3 = разряд
    //4 = Зарезервировано
    //5 = режим проверки
    //6 = режим зарядки постоянным током
    //7 = режим зарядки переменного тока
    //8 = штекерный пистолет постоянного тока не заряжен
    //9 = вилка переменного тока не заряжена
    //10 ~15 = зарезервирован
    setUpper4Bites(m, 0x0, BYTE_0, 0);
   

    //биты 8-11 нижние биты первого байта ошибки BMS
    //ошибок нет, все нули
    setLower4Bites(m, 0x0, BYTE_1, 0);

    //биты 12-13 или биты 4 и 5 первого байта
    //состояние баланчировки
    //0 = отключить эквализацию
    //+++1 = включить выравнивание
    //2 = зарезервировано
    //3 = зарезервировано
    set2Bites(m, 0x0, 12, 1);

    //биты 16-23 или второй байт
    //SOC
    float soc = 98.56; // значение в процентах
    soc /= 0.5;
    setByte(m, 0x0, BYTE_2, (uint32_t)(soc));

    //биты 24-31 или третий байт
    //SOH здрорвье аккумулятора у нас аккум новый и он полон сил, поэтому 100%
    float soh = 100.00;
    soh /= 0.5;
    setByte(m, 0x0, BYTE_3, (uint32_t)(soh));



    //биты 32-47 байты 4 и 5
    //сопративление батареи
    float battary_resistance = 0.005; //сопративление батареи 5 мОм
    battary_resistance /= 0.001;
    setTwoByte(m, 0x0, bitToByte(32).byte, (uint32_t)(battary_resistance));

    //биты 48-59 
    //остаточная емкость
    float residual_capacity = 2.2; //ампер часа
    residual_capacity /= 0.1;
    set12Bits(m, 0x0, 48, (uint32_t)(residual_capacity));
    

    //биты 60-63
    //счетчик каждые 100 мс
    //!!!!!!!!!!!!!!!-----------------------СЧЕТЧИК 0-14, 15 ЗАРЕЗЕРВИРОВАНО
    setUpper4Bites(m, 0x0, bitToByte(60).byte, 0xe);


    /*---------------------------------------------------------0x0-------------------------------------*/

    setCycle(m, 0x0, 20);
    setComment(m, 0x0, "напряжение, ток, мощность АКБ");
    //подряд 2 посылки 0-11 и 12-23
    //напряжение АКБ, выходное напряжение в вольтах
    const float battary_voltage = 1.8;
    //battary_voltage /= 0.2;
    const float output_voltage = 316.9;
    //output_voltage /= 0.2;
    setPairOf12Bit(m, 0x0, BYTE_0, (uint32_t)(battary_voltage / 0.2), (uint32_t)(output_voltage / 0.2));


    //биты 24-39 или байты 3 и 4
    //ток батареи в амперах
    const float battary_current = 486.32;
    //battary_current = (battary_current + 500.0) / 0.02;
    setTwoByte(m, 0x0, BYTE_3, (uint32_t)((battary_current + 500.0) / 0.02));


    //биты 40-55 или байты 5 и 6
    //мощьность батареи в кВт
    const float battary_power = (output_voltage * battary_current) / 1000.0;
    setTwoByte(m, 0x0, BYTE_5, (uint32_t)((battary_power + 325.0) / 0.01));


    /*---------------------------------------------------------0x0-------------------------------------*/
    
    setCycle(m, 0x0, 1000);
    setComment(m, 0x0, "онлайн статусы, количество АКБ ящиков, номер ведомого, версия ПО");
    
    //биты 0-31 или байты 0-3 
    //задаются номера онлайн устройств. Списано с логов 
    setByte(m, 0x0, BYTE_0, 0x0f); // списано с логов. может быть 0x00
    setByte(m, 0x0, BYTE_1, 0x00); // списано с логов.
    setByte(m, 0x0, BYTE_2, 0x00); // списано с логов.
    setByte(m, 0x0, BYTE_3, 0x00); // списано с логов.

    //биты 32-39 или байт 4
    //количество аккумуляторнх ящиков списано с логов значения в диапазоне 1-16
    setByte(m, 0x0, BYTE_4, 0x02); // списано с логов. 2 ящика

    //биты 40-48 или байт 5
    //номер ведомого списано с логов значения в диапазоне 1-32
    setByte(m, 0x0, BYTE_5, 0x04); // списано с логов. ведовый четвертый

    //бит 48 статус программного обеспечения
    //биты 49-63 номер версии ПО
    //чтобы не плодить функции типа setNBits напишим все руками

    //0 представляет статус образца
    //1 представляет статус партии
    bool software_status = 0; //статус образца
    uint32_t software_version = 1; //версия 1;
    setTwoByte(m, 0x0, BYTE_6, (uint32_t)(software_status + (software_version << 1)));

    /*---------------------------------------------------------0x0-------------------------------------*/
    
    setCycle(m, 0x0, 100);
    setComment(m, 0x0, "описание ячеек АКБ");

    //биты 0-15 байты 0 и 1
    //сумма напряжения ячеек
    const float battary_cell_voltage_summ = output_voltage;
    setTwoByte(m, 0x0, BYTE_0, (uint32_t)(battary_cell_voltage_summ / 0.01));
    
    //биты 16-27 и биты 28-39 байты 2-4
    //максимальное и минимальное напряжение ячеек в вольтах
    const float min_voltage_per_cell = 2.2;
    const float max_voltage_per_cell = 2.2;
    setPairOf12Bit(m, 0x0, BYTE_2, (uint32_t)(max_voltage_per_cell / 0.0015),
        (uint32_t)(min_voltage_per_cell / 0.0015));

    //биты 40-47 ячейка с максимальным напряжением
    //биты 48-55 ячейка с минимальным напряжением
    setByte(m, 0x0, bitToByte(40).byte, 1); //у первой ячейки максимальное напряжение
    setByte(m, 0x0, bitToByte(48).byte, 2); //у второй ячейки минимальное напряжение

    //биты 56-59 системный номер ячейки с самым высоким напряжением
    setLower4Bites(m, 0x0, bitToByte(56).byte, 1);//первыя ячейка, как в логах

    //биты 60-63 системный номер ячейки с самым низким напряжением
    setUpper4Bites(m, 0x0, bitToByte(60).byte, 3);//третья ячейка, как в логах

    /*---------------------------------------------------------0x0-------------------------------------*/

    setCycle(m, 0x0, 100);
    setComment(m, 0x0, "температура");

    //биты 0-15 или байты 0-1
    //средняя температура мономера в градусах цельсия
    const float monomer_mean_temperature = 40.0;
    setTwoByte(m, 0x0, BYTE_0, (uint32_t)((monomer_mean_temperature + 50.0) / 0.2));

    //биты 16-27 и биты 28-39 байты 2-4
    //максимальное и минимальное значение температуры
    const float max_temperature = 30.1;
    const float min_temperature = 30.0;
    setPairOf12Bit(m, 0x0, bitToByte(16).byte, (uint32_t)((max_temperature + 50.0) / 0.2),
        (uint32_t)((min_temperature + 50.0) / 0.2));

    //биты 40-47 ячейка с максимальной температурой
    //биты 48-55 ячейка с минимальной температурой
    setByte(m, 0x0, bitToByte(40).byte, 1); //у первой ячейки максимальная температура
    setByte(m, 0x0, bitToByte(48).byte, 2); //у второй ячейки минимальная температура

    //биты 56-59 Номер ящика, где находится самая высокая температура
    setLower4Bites(m, 0x0, bitToByte(56).byte, 1);//первый ящик, как в логах

    //биты 60-63 Номер ящика, где находится самая низкая температура
    setUpper4Bites(m, 0x0, bitToByte(60).byte, 3);//третий ящик, как в логах


    /*---------------------------------------------------------0x0-------------------------------------*/

    setCycle(m, 0x0, 250);
    setComment(m, 0x0, "пределы тока и напряжения");
    
    //биты 0-15 и биты 16-23 байты 0-2
    //Пределы максимального и минимального напряжений блока в вольтах
    const float max_limit_voltage_per_block = 725.4; // очень странные значения 
    const float min_limit_voltage_per_block = 2.4; // очень странные значения
    setPairOf12Bit(m, 0x0, bitToByte(0).byte, (uint32_t)(max_limit_voltage_per_block / 0.2),
        (uint32_t)(min_limit_voltage_per_block / 0.2));


    //биты 24-35 и биты 36-47 байты 3-5
    // Пределы максимального и минимального напряжений ячейки в вольтах
    const float max_limit_voltage_per_cell = 1.686; // очень странные значения
    const float min_limit_voltage_per_cell = 0.0195; // очень странные значения
    setPairOf12Bit(m, 0x0, bitToByte(24).byte, (uint32_t)(max_limit_voltage_per_cell / 0.0015),
        (uint32_t)(min_limit_voltage_per_cell / 0.0015));

    //биты 48-55 и биты 56-63 байты 6 и 7
    // Максимальный и максимальный пределы тока разряда в амперах. Очень странно
    //И масштаб очень странный
    const float max1_limit_charge_current = 39.2; 
    const float max2_limit_charge_current = 16;
    setByte(m, 0x0, bitToByte(48).byte, (uint32_t)(max1_limit_charge_current / 2.0));
    setByte(m, 0x0, bitToByte(56).byte, (uint32_t)(max2_limit_charge_current / 2.0));

    /*---------------------------------------------------------0x0-------------------------------------*/

    setCycle(m, 0x0, 50);
    setComment(m, 0x0, "пределы soc, мощьность заряда 10с, температуры ячеек");

    //биты 0-7 и биты 8-15 байты 0-1
    //Максимальный и минимальный пределы SOC в процентах
    const float max_limit_soc = 100; // из логов, что логично
    const float min_limit_soc = 5; // 
    setByte(m, 0x0, bitToByte(0).byte, (uint32_t)(max_limit_soc / 0.5));
    setByte(m, 0x0, bitToByte(8).byte, (uint32_t)(min_limit_soc / 0.5));

    //биты 16-23 и биты 24-31 байты 2-3
    //Максимальная и минимальная мощность разряда 10 с в кВт
    //в документе максимальная и максимальная, что странно
    //!!!!!!!!!!!!!!!!!!------------------------------------------------ НЕ СТОЯТ ЗНАЧЕНИЯ. В ЛОГАХ НУЛИ
    const float max_limit_charge_power_10_second = 0;
    const float min_limit_charge_power_10_second = 0; // 
    setByte(m, 0x0, bitToByte(16).byte, (uint32_t)(max_limit_charge_power_10_second / 1));
    setByte(m, 0x0, bitToByte(24).byte, (uint32_t)(min_limit_charge_power_10_second / 1));

    //биты 32-39 и биты 40-47 байты 4-5
    //Максимальная и минимальная температуры ячеек в градусах
    const float max_limit_temperature_per_cell = 60.0; //значение из логов
    const float min_limit_temperature_per_cell = -19.0; //значение из логов
    setByte(m, 0x0, bitToByte(32).byte, (uint32_t)(max_limit_temperature_per_cell + 50.0));
    setByte(m, 0x0, bitToByte(40).byte, (uint32_t)(min_limit_temperature_per_cell + 50.0));

    printArraysForStm(m);

    //printAllData(m);

    //printArraysByCycle(m);


}