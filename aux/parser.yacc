%{
#include <stdio.h>
#include <search.h>

#define MAX_ID_LENGTH 256

typedef struct {
   char id[MAX_ID_LENGTH];
} YylvalType;

#define MAX_LIST_LENGHT 256
char id_list[MAX_LIST_LENGHT][MAX_ID_LENGTH];
int cur_id = -1;

int cur_token = -1;

#define MAX_TOKENS 256
int cur_token = -1;
char *token_definitions[MAX_TOKENS];

FILE *out_file = NULL;

char *tap_key_id = NULL;
char *max_count_id = NULL;

typedef struct {
   char *count_id;
   char *action_id;
} TapAction;

#define MAX_TAP_ACTIONS 100
TapAction tap_actions[MAX_TAP_ACTIONS];
int n_tap_actions = 0;

static void *alias_tree_root = NULL;

static void finish_pattern(void);
static void add_action_to_current_token(const char *action_id);
static void generate_note(const char *input_id);
static void generate_cluster(void);
static void generate_chord(void);
static void init_tap_dance(const char *tap_key_id, const char *max_count_id);
static void store_tap_action(const char *count_id, const char *action_id);
static void finish_tap_dance(void);
static void set_current_layer(const char *layer_id);
static void store_alias(const char *key_id, const char *value);
static const char *subst_alias(const char *key_id);
static void mark_symbol(const char *symbol_id);

%}

%YylvalType

%start list

%token ID ALIAS

%%                   /* beginning of rules section */

input_list:
         ID
         {
            ++cur_id;
            strncpy(id_list[cur_id], $1.id, MAX_ID_LENGTH);
         }
         |
         input_list ',' ID
         {
            ++cur_id;
            strncpy(id_list[cur_id], $3.id, MAX_TOKEN_LENGTH);
         }
         ;
         
pattern: token
        |
        pattern "->" token
        ;
        
full_pattern:
        pattern ';'
        {
           finish_pattern();
        }
        ;
        
action_token:
        token ':' ID
        {
           add_action_to_current_token($1.id);
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
           generate_note($1.id);
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
        
tap_key:
        '(' ID '*' ID ')'
        {
           init_tap_dance($2.id, $4.id);
        }
        ;
        
tap_action_def:
        ID '=' ID
        {
           store_tap_action($1.id, $3.id);
        }
        ;
        
tap_action_list:
        tap_action_def
        |
        tap_action_list ',' tap_action_def
        ;

tap_dance:
        tap_key ':' tap_action_list ';'
        {
           finish_tap_dance();
        }
        ;
        
layer_def:
        "layer:" ID
        {
           set_current_layer($2.id);
        }
        ;
        
alias_def:
        "alias:" ID '=' ALIAS
        {
           store_alias($1.id, $2.id);
        }
        ;
        
symbol_def:
        "symbol:" ID
        {
           mark_symbol($1.id);
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
"   )\n\n"
   );        
   
   cur_token = -1;
}

#define SAFE_FREE(S) free(S); S = NULL;

#define BUFF_MAX 1024
#define BUFF_REST (BUFF_MAX - (buff_pos - buffer))

#define BUFF_PRINT(...) buff_pos += snprintf(buff_pos, BUFF_REST, __VA_ARGS__)

static void add_action_to_current_token(const char *action_id)
{
   char buffer[BUFF_MAX];
   
   char *old_token = token_definitions[cur_token];
   
   snprintf(buffer, BUFF_MAX, "ppg_token_set_action(%s, %s)", old_token, subst_alias(action_id));
   
   token_definitions[cur_token] = strdup(buffer);
   
   free(old_token);
}

static void generate_note(const char *input_id)
{
   char buffer[BUFF_MAX];
   
   snprintf(buffer, BUFF_MAX, "ppg_note_create_standard(%s)", subst_alias(input_id));
   
   token_definitions[cur_token] = strdup(buffer);
}

static void generate_aggregate(const char *aggr_type)
{
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

static void init_tap_dance(const char *tap_key_id__, const char *max_count_id__)
{
   tap_key_id = strdup(tap_key_id__);
   max_count_id = strdup(max_count_id__);
}

static void store_tap_action(const char *count_id, const char *action_id)
{
   tap_actions[n_tap_actions] = (TapAction){ .count_id = strdup(count_id), .action_id = strdup(action_id) };
}

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
"         %s\n
"      )"
         tap_actions[i].count_id,
         tap_actions[i].tap_key_id
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
      SAFE_FREE(tap_actions[i].tap_key_id);
   }
   
   n_tap_actions = 0;
}

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
#define PPG_START_DEFINITIONS_TOKEN "ppg_start_definitions"
#define PPG_END_DEFINITIONS_TOKEN "ppg_end_definitions"

void generate(const char *input_filename, const char *output_filename)
{
   FILE *in_file = fopen(input_filename, "r");
   out_file = fopen(output_filename, "w");
         
   char line[256];
   
   bool in_ppg = false;
   bool in_definitions = false;

   while (fgets(line, sizeof(line), in_file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
           
      if(strstr(line, PPG_START_TOKEN) != NULL) {
         in_ppg = true;
         continue;
      }
      
      if(strstr(line, PPG_END_TOKEN) != NULL) {
         break;
      }
      
      if(strstr(line, PPG_START_DEFINITIONS_TOKEN) != NULL) {
         in_definitions = true;
         continue;
      }
      
      if(strstr(line, PPG_START_DEFINITIONS_TOKEN) != NULL) {
         in_definitions = false;
         continue;
      }
      
      if(!in_ppg) {
         continue;
      }
      
      if(in_definitions) {
         process_definitions(line, out_file);
      }
      else {
         fprintf(out_file, line);
      }
   }
   
   fclose(in_file);
   fclose(out_file);
   
   out_file = NULL;
}

int main(int argc, char **argv) {
   
   if(argc < 3) {
     fprintf(stderr, "usage: %s <input_file> <output_file>\n");
     return 1;
   }
   
   generate(argv[1], argv[2]);
   
   return 0;
}

void yyerror(char *s)
{
  fprintf(stderr, "%s\n",s);
}

int yywrap(void)
{
  return 1;
}

