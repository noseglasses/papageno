set(__PPG_MAX_EVENTS 100)

set(__PPG_MAX_INPUTS 256)

set(__PPG_MAX_ACTIVE_TOKENS 16)

set(__PPG_SMALL_UNSIGNED_INT_TYPE uint8_t)

set(__PPG_SMALL_SIGNED_INT_TYPE int8_t)

set(__PPG_MEDIUM_SIGNED_INT_TYPE int16_t)
   
if(PAPAGENO_PEDANTIC_TOKENS)
   set(__PPG_PEDANTIC_TOKENS 1)
else()
   set(__PPG_PEDANTIC_TOKENS 0)
endif()

set(__PPG_COUNT_TYPE ${__PPG_SMALL_UNSIGNED_INT_TYPE})

set(__PPG_ID_TYPE ${__PPG_MEDIUM_SIGNED_INT_TYPE})

set(__PPG_ACTION_FLAGS_TYPE ${__PPG_SMALL_UNSIGNED_INT_TYPE})

set(__PPG_LAYER_TYPE ${__PPG_SMALL_SIGNED_INT_TYPE})

set(__PPG_SLOT_ID_TYPE ${__PPG_SMALL_UNSIGNED_INT_TYPE})

set(__PPG_TIME_COMPARISON_RESULT_TYPE ${__PPG_SMALL_SIGNED_INT_TYPE})

set(__PPG_EVENT_BUFFER_INDEX_TYPE ${__PPG_SMALL_SIGNED_INT_TYPE})

set(__PPG_PROCESSING_STATE_TYPE ${__PPG_SMALL_UNSIGNED_INT_TYPE})

set(__PPG_TIME_IDENTIFIER_TYPE uintptr_t)

# Add some platform specific compiler flags

set(PAPAGENO_ATMEL_ARCHITECTURE "atmega1280")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmcu=${PAPAGENO_ATMEL_ARCHITECTURE} -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -fno-inline-small-functions -fno-strict-aliasing -gdwarf-2  -Os -Wall -Wstrict-prototypes -Werror -D__AVR__")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mmcu=${PAPAGENO_ATMEL_ARCHITECTURE}")