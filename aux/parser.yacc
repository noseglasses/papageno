%{

#include "parser.h"

#include <stdio.h>
#include <search.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

#include "lex.yy.h"

int yydebug=1;

int yylex();
void yyerror(const char *s);

int cur_line = -1;

#define MAX_LIST_LENGTH 256
char id_list[MAX_LIST_LENGTH][MAX_ID_LENGTH];
int cur_id = -1;

#define MAX_TOKENS 256
int cur_token = -1;
char *token_definitions[MAX_TOKENS];

FILE *out_file = NULL;

int max_count_id = 0;

typedef struct {
   char *count_id;
   char *action_id;
} TapAction;

#define MAX_TAP_ACTIONS 100
TapAction tap_actions[MAX_TAP_ACTIONS];
int n_tap_actions = 0;

int token_start = -1;

static void *alias_tree_root = NULL;

static void finish_pattern(void);
static void add_action_to_token_n(int token_count, const char *action_id);
static void add_action_to_token(const char *token_count_id, const char *action_id);
static void add_action_to_current_token(const char *action_id);
static void generate_note(const char *input_id);
static void generate_cluster(void);
static void generate_chord(void);
static void repeat_token(const char *count_id);
// static void store_tap_action(const char *count_id, const char *action_id);
// static void finish_tap_dance(void);
static void set_current_layer(const char *layer_id);
static void store_alias(const char *key_id, const char *value);
static const char *subst_alias(const char *key_id);
static void mark_symbol(const char *symbol_id);

%}

%start lines

%union {
   char id[MAX_ID_LENGTH]; 
}

%token LAYER SYMBOL ALIAS ARROW
%token <id> ID QUOTED_STRING

%%                   /* beginning of rules section */

lines:  /*  empty  */
        |
        '\n'
        |
        line
        |      
        lines line
        ;
        
line:
        pattern '\n'
        |
        layer_def '\n'
        |
        alias_def '\n'
        |
        symbol_def '\n'
        |
        error '\n'
        {
          yyerrok;
        }
        ;

input_list:
         ID
         {
            ++cur_id;
            strncpy(id_list[cur_id], $1, MAX_ID_LENGTH);
         }
         |
         input_list ',' ID
         {
            ++cur_id;
            strncpy(id_list[cur_id], $3, MAX_ID_LENGTH);
         }
         ;
         
pattern_list: 
        action_token
        |
        pattern_list ARROW action_token
        ;
        
pattern:
        pattern_list ';'
        {
           finish_pattern();
        }
        ;
        
action_token:
        rep_token
        |
        rep_token ':' action_list
        ;
        
rep_token:
        token
        |
        token '*' ID
        {
           repeat_token($3);
        }
        ;
           
token:  note
        |
        cluster
        |
        chord
        ;
        
note:   '(' ID ')'
        {
           generate_note($2);
        }
        ;
        
cluster:
        '{' input_list '}'
        {
           generate_cluster();
        }
        ;
        
chord:
        '[' input_list ']'
        {
           generate_chord();
        }
        ;
        
action_def:
        ID
        {
           add_action_to_token_n(0, $1);
        }
        |
        ID '=' ID
        {
           add_action_to_token($1, $3);
        }
        ;
        
action_list:
        action_def
        |
        action_list ',' action_def
        ;
        
layer_def:
        LAYER ':' ID
        {
           printf("Setting layer %s\n", $3);
           set_current_layer($3);
        }
        ;
        
alias_def:
        ALIAS ':' ID '=' QUOTED_STRING
        {
           store_alias($3, $5);
        }
        ;
        
symbol_def:
        SYMBOL ':' ID
        {
           mark_symbol($3);
        }
        ;
%%

static int compare(const void *pa, const void *pb)
{
   return strcmp(pa, pb);
}

static void finish_pattern(void)
{
   fprintf(
      out_file, 
"ppg_pattern(\n"
"   current_layer,\n"
"   %d, /* number of tokens */\n"
"   PPG_TOKENS(\n",
      cur_token + 1);

   for(int i = 0; i <= cur_token; ++i) {
      fprintf(
         out_file,
"      %s",
         token_definitions[i]
      );
      
      free(token_definitions[i]);
      
      if(i < cur_token) {
         fprintf(out_file, ",");
      }
      
      fprintf(out_file, "\n");
   }
   
   fprintf(
      out_file, 
"   )\n"
   );     
   fprintf(
      out_file,
");\n\n"
   );
   
   cur_token = -1;
}

#define SAFE_FREE(S) free(S); S = NULL;

#define BUFF_MAX 1024
#define BUFF_REST (BUFF_MAX - (buff_pos - buffer))

#define BUFF_PRINT(...) buff_pos += snprintf(buff_pos, BUFF_REST, __VA_ARGS__)

#define MY_ERROR(FORMAT_STRING, ...) fprintf(stderr, "error: line %d: " FORMAT_STRING, cur_line,##__VA_ARGS__); exit(EXIT_FAILURE);

static char *strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);   // Space for length plus nul
    if (d == NULL) return NULL;          // No memory
    strcpy (d,s);                        // Copy the characters
    return d;                            // Return the new string
}

static long my_atol(const char *count)
{
   errno = 0;    /* To distinguish success/failure after call */
   char *dummy = NULL;
   long val = strtol(count, &dummy, 10);

   /* Check for various possible errors */

   if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0)) {
      MY_ERROR("Failed to convert tap count %s to integer\n", count);
   }
   
   return val;
}

static void add_action_to_token_n(int token_count, const char *action_id)
{
   int actual_token_count = token_start + token_count;

   char buffer[BUFF_MAX];
   
   char *old_token = token_definitions[actual_token_count];
   
   snprintf(buffer, BUFF_MAX, "ppg_token_set_action(%s, %s)", old_token, subst_alias(action_id));
   
   token_definitions[actual_token_count] = strdup(buffer);
   
   free(old_token);
}

static void add_action_to_token(const char *token_count_id, const char *action_id)
{
   int token_count = my_atol(token_count_id);
   
   add_action_to_token_n(token_count - 1, action_id);
}

static void add_action_to_current_token(const char *action_id)
{
   add_action_to_token_n(cur_token, action_id);
}

static void generate_note(const char *input_id)
{
   ++cur_token;
   
   token_start = cur_token;
   
   char buffer[BUFF_MAX];
   
   snprintf(buffer, BUFF_MAX, "ppg_note_create_standard(%s)", subst_alias(input_id));
   
   token_definitions[cur_token] = strdup(buffer);
}

static void generate_aggregate(const char *aggr_type)
{
   ++cur_token;
   
   token_start = cur_token;
   
   char buffer[BUFF_MAX];
   
   char *buff_pos = buffer;
   
   BUFF_PRINT("PPG_%s_CREATE(", aggr_type);
   
   for(int i = 0; i <= cur_id; ++i) {
      BUFF_PRINT("%s", subst_alias(id_list[i]));
      if(i < cur_id) {
         BUFF_PRINT(",");
      }
   }
   
   BUFF_PRINT(")");
   
   token_definitions[cur_token] = strdup(buffer);
   
   cur_id = -1;
}

static void generate_cluster(void)
{
   generate_aggregate("CLUSTER");
}

static void generate_chord(void)
{
   generate_aggregate("CHORD");
}

static void repeat_token(const char *count_id)
{
   max_count_id = my_atol(count_id);

   char *start_string = token_definitions[cur_token];
   for(long i = 1; i < max_count_id; ++i) {
      ++cur_token;
      token_definitions[cur_token] = strdup(start_string);
   }
}
/*
static void store_tap_action(const char *count_id, const char *action_id)
{
   int tap_count = my_atol(count_id);
   
   if((tap_count > max_count_id) || (tap_count < 1)) {
      MY_ERROR("Unable to store tap action %s for count %l. The provided tap count exceeds the range [1, %d]\n.",
         action_id, tap_count, max_count_id);
   }
   
   add_action_to_token_n(action_id, tap_count - 1);
}*/

/*
static void finish_tap_dance(void)
{
   fprintf(
      out_file,
"ppg_tap_dance(\n"
"   current_layer,\n"
"   %s,\n"
"   PPG_TAP_DEFINITIONS(\n",
      subst_alias(tap_key_id)
   );
   
   for(int i = 0; i < n_tap_actions; ++i) {
      fprintf(
         out_file,
"      PPG_TAP(\n"
"         %s,\n"
"         %s\n"
"      )",
         tap_actions[i].count_id,
         tap_actions[i].action_id
      );
      
      if(i < (n_tap_actions - 1)) {
         fprintf(out_file, ",");
      }
      fprintf(out_file, "\n");
   }
   
   fprintf(
      out_file,
"   )\n"
");\n\n"
   );
   
   SAFE_FREE(tap_key_id);
   SAFE_FREE(max_count_id);
   
   for(int i = 0; i < n_tap_actions; ++i) {
      SAFE_FREE(tap_actions[i].count_id);
      SAFE_FREE(tap_actions[i].action_id);
   }
   
   n_tap_actions = 0;
}
*/

static void set_current_layer(const char *layer_id)
{
   fprintf(
      out_file,
"current_layer = %s;\n\n",
      subst_alias(layer_id)
   );
}

static void store_alias(const char *key_id, const char *value)
{
   void *val = tsearch((void *) key_id, &alias_tree_root, compare);

   *((void **)val) = (void*)strdup(value);
}

static const char *subst_alias(const char *key_id)
{
   void *val = tfind((void *)key_id, &alias_tree_root, compare);
   
   if(!val) {
      return key_id;
   }
   
   return (const char *)*((void**)val);
}

static void mark_symbol(const char *symbol_id)
{
   fprintf(
      out_file,
"PPG_COMPRESSION_REGISTER_SYMBOL(ccontext, %s)\n\n", 
      symbol_id
   );
}

struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;

static void process_definitions(const char *line)
{
   // add the second NULL terminator
   int len = strlen(line);
   char *temp = (char*)malloc(len + 2);
   strcpy( temp, line );
   temp[ len + 1 ] = 0; // The first NULL terminator is added by strcpy

   YY_BUFFER_STATE my_string_buffer = yy_scan_string(temp); 
   yy_switch_to_buffer( my_string_buffer ); // switch flex to the buffer we just created
   yyparse(); 
   yy_delete_buffer(my_string_buffer );
   free(temp);
}

#define PPG_START_TOKEN "ppg_start"
#define PPG_END_TOKEN "ppg_end"
#define PPG_START_DEFINITIONS_TOKEN "ppg_definitions_start"
#define PPG_END_DEFINITIONS_TOKEN "ppg_definitions_end"

void generate(const char *input_filename, const char *output_filename)
{
   FILE *in_file = fopen(input_filename, "r");
   out_file = fopen(output_filename, "w");
         
   char line[4096];
   
   bool in_ppg = false;
   bool in_definitions = false;

   cur_line = 0;
   
   while (fgets(line, sizeof(line), in_file)) {
   
      ++cur_line;
      
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
           
      if(strstr(line, PPG_START_TOKEN) != NULL) {
         printf("line %d: in ppg\n", cur_line);
         in_ppg = true;
         continue;
      }
      
      if(strstr(line, PPG_END_TOKEN) != NULL) {
         printf("line %d: end of ppg\n", cur_line);
         break;
      }
      
      if(strstr(line, PPG_START_DEFINITIONS_TOKEN) != NULL) {
         printf("line %d: in definitions\n", cur_line);
         in_definitions = true;
         continue;
      }
      
      if(strstr(line, PPG_END_DEFINITIONS_TOKEN) != NULL) {
         printf("line %d: end of definitions\n", cur_line);
         in_definitions = false;
         continue;
      }
      
      if(!in_ppg) {
         continue;
      }
      
      if(in_definitions) {
         printf("Processing line \'%s\'\n", line);
         process_definitions(line);
      }
      else {
         fprintf(out_file, "%s", line);
      }
   }
   
   fclose(in_file);
   fclose(out_file);
   
   out_file = NULL;
}

int main(int argc, char **argv) {
   
   if(argc < 3) {
     fprintf(stderr, "usage: %s <input_file> <output_file>\n", argv[0]);
     return 1;
   }
   
   generate(argv[1], argv[2]);
   
   return 0;
}

void yyerror(const char *s)
{
  MY_ERROR("yacc error: %s\n", s);
}

int yywrap(void)
{
  return 1;
}

