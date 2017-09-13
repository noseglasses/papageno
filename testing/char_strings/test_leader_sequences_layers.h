enum {
   ppg_cs_layer_0 = 0
};

#define PPG_CS_INPUT_FROM_CHAR(CHAR) \
   (CHAR - 'a')

#define PPG_CS_ALPH_CASE(LOWER_CASE_QUOTED, UPPER_CASE_QUOTED, ID) \
   case LOWER_CASE_QUOTED: \
   case UPPER_CASE_QUOTED: \
      return LOWER_CASE_QUOTED;
   
#ifndef PPG_CS_READ_COMPRESSED_CONTEXT
      
static PPG_Input_Id ppg_cs_input_from_alphabetic_character(char c) {   
   
   switch(c) {
      PPG_CS_ALPH_CASE('a', 'A', A)
      PPG_CS_ALPH_CASE('b', 'B', B)
      PPG_CS_ALPH_CASE('c', 'C', C)
      PPG_CS_ALPH_CASE('d', 'D', D)
      PPG_CS_ALPH_CASE('e', 'E', E)
      PPG_CS_ALPH_CASE('f', 'F', F)
      PPG_CS_ALPH_CASE('g', 'G', G)
      PPG_CS_ALPH_CASE('h', 'H', H)
      PPG_CS_ALPH_CASE('i', 'I', I)
      PPG_CS_ALPH_CASE('j', 'J', J)
      PPG_CS_ALPH_CASE('k', 'K', K)
      PPG_CS_ALPH_CASE('l', 'L', L)
      PPG_CS_ALPH_CASE('m', 'M', M)
      PPG_CS_ALPH_CASE('n', 'N', N)
      PPG_CS_ALPH_CASE('o', 'O', O)
      PPG_CS_ALPH_CASE('p', 'P', P)
      PPG_CS_ALPH_CASE('q', 'Q', Q)
      PPG_CS_ALPH_CASE('r', 'R', R)
      PPG_CS_ALPH_CASE('s', 'S', S)
      PPG_CS_ALPH_CASE('t', 'T', T)
      PPG_CS_ALPH_CASE('u', 'U', U)
      PPG_CS_ALPH_CASE('v', 'V', V)
      PPG_CS_ALPH_CASE('w', 'W', W)
      PPG_CS_ALPH_CASE('x', 'X', X)
      PPG_CS_ALPH_CASE('y', 'Y', Y)
      PPG_CS_ALPH_CASE('z', 'Z', Z)
   }
   
   return 0;
}

static void ppg_cs_retreive_string(uint8_t sequence_id,
                              char *buffer, 
                              uint8_t max_chars)
{
   switch(sequence_id) {
      case 0:
         strcpy(buffer, "foo");
         break;
      case 1:
         strcpy(buffer, "bar");
         break;
      case 2:
         strcpy(buffer, "Klaatubaradan");
         break;
   }
}

static PPG_Action ppg_cs_retreive_action(uint8_t sequence_id)
{
   return (PPG_Action) {
      .callback = (PPG_Action_Callback) {
         .func = (PPG_Action_Callback_Fun)ppg_cs_process_action,
         
         // The first three actions are reserved
         //
         .user_data = (void*)(uintptr_t)(sequence_id + 3)
      }
   };
}

#endif // #ifndef PPG_CS_READ_COMPRESSED_CONTEXT
