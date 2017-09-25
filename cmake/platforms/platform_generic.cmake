set(__PPG_PLATFORM generic)

set(__PPG_MAX_EVENTS 100)

set(__PPG_MAX_INPUTS 255)

set(__PPG_MAX_ACTIVE_TOKENS 255)

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

set(__PPG_TIME_IDENTIFIER_TYPE "long unsigned")
