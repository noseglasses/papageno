%{

#include "parser.h"

#include <stdio.h>
#include <search.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

#include "parser.lex.h"

int yydebug=1;

int yylex();
void yyerror(const char *s);

int cur_line = -1;

#define MAX_INPUT_KEYWORDS 256
char inputs[MAX_INPUT_KEYWORDS][MAX_ID_LENGTH];
int cur_id = -1;

#define MAX_TOKENS 256
int cur_token = -1;
char *token_definitions[MAX_TOKENS];

int cur_new_token = -1;
char *new_token_definitions[MAX_TOKENS];

FILE *out_file = NULL;
FILE *inputs_file = NULL;
FILE *actions_file = NULL;
FILE *symbols_file = NULL;

int max_count_id = 0;

int token_start = -1;

typedef struct {
   const char *name;
   char **members;
   int n_members;
} Phrase;

enum EnityType {
   EntityTypeNone = 0,
   EntityTypeAction = 1,
   EntityTypeInput = 2,
   EntityTypePattern = 3
};

typedef struct {
   const char *name;
   const char *tag;
   const char *parameters;
   int type;
} TypedEntity;

TypedEntity cur_entity = (TypedEntity){ .name = NULL, .tag = NULL, .parameters = NULL, .type = EntityTypeNone };

static void *pattern_tree_root = NULL;
static void *entity_tree_root = NULL;
static void *input_tag_tree_root = NULL;
static void *action_tag_tree_root = NULL;

static void finish_pattern(void);
static void save_phrase(const char *name);
static Phrase *lookup_phrase(const char *name);
static void push_phrase(const char *name);
static void flush_new_tokens(void);
static void add_action_to_token_relative(int rel_count, const char *action_id);
static void add_action_to_token_n(int token_count, const char *action_id);
static void add_action_to_token(const char *token_count_id, const char *action_id);
static void generate_note(const char *input_id, const char *flags);
static void generate_note_full(const char *input_id);
static void generate_note_activation(const char *input_id);
static void generate_note_deactivation(const char *input_id);
static void generate_cluster(void);
static void generate_chord(void);
static void repeat_token(const char *count_id);
static void set_current_layer(const char *layer_id);
static void mark_symbol(const char *symbol_id);
static void store_cur_typed_entity(const char *type_id, const char *id);
static void store_typed_entity_parameters(const char *quoted_parameters);
static void store_current_entity(void);
static TypedEntity *look_up_entity(const char *name);
static void store_entity(TypedEntity *entity);
static void flush_inputs(void);
static void store_input_tag(const char *tag);
static void store_action_tag(const char *tag);

#define MY_ERROR(FORMAT_STRING, ...) fprintf(stderr, "error: line %d: " FORMAT_STRING, cur_line,##__VA_ARGS__); exit(EXIT_FAILURE);

%}

%start lines

%union {
   char id[MAX_ID_LENGTH]; 
}

%token LAYER_KEYWORD SYMBOL_KEYWORD ARROW ACTION_KEYWORD INPUT_KEYWORD PHRASE_KEYWORD
%token <id> ID RAW_CODE

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
        phrase '\n'
        {
           finish_pattern();
        }
        |
        PHRASE_KEYWORD ':' ID '=' phrase '\n'
        {
           save_phrase($3);
        }
        |
        layer_def '\n'
        |
        symbol_def '\n'
        |
        input_def '\n'
        |
        action_def '\n'
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
            if(cur_id >= MAX_INPUT_KEYWORDS) {
               MY_ERROR("Maximum amount of ids (%d) exceeded\n", MAX_INPUT_KEYWORDS);
            }
            strncpy(inputs[cur_id], $1, MAX_ID_LENGTH);
         }
         |
         input_list ',' ID
         {
            ++cur_id;
            if(cur_id >= MAX_INPUT_KEYWORDS) {
               MY_ERROR("Maximum amount of ids (%d) exceeded\n", MAX_INPUT_KEYWORDS);
            }
            strncpy(inputs[cur_id], $3, MAX_ID_LENGTH);
         }
         ;
         
phrase: 
        action_token
        {
           flush_new_tokens();
        } 
        |
        phrase ARROW action_token
        {
           flush_new_tokens();
        }
        ;
        
action_token:
        rep_token
        |
        rep_token ':' action_list
        |
        '#' ID
        {
           push_phrase($2);
        }
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
        
note:   '|' ID '|'
        {
           generate_note_full($2);
        }
        |
        '|' ID
        {
           generate_note_activation($2);
        }
        |
        ID '|'
        {
           generate_note_deactivation($1);
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
           add_action_to_token_n(cur_new_token, $1);
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
        LAYER_KEYWORD ':' ID
        {
           printf("Setting layer %s\n", $3);
           set_current_layer($3);
        }
        ;
        
symbol_def:
        SYMBOL_KEYWORD ':' ID
        {
           mark_symbol($3);
        }
        ;
        
input_def:
        INPUT_KEYWORD ':' typed_id parameters
        {
           store_input_tag(cur_entity.tag);
           cur_entity.type = EntityTypeInput;
           store_current_entity();
        }
        ;
        
action_def:
        ACTION_KEYWORD ':' typed_id parameters
        {
           store_action_tag(cur_entity.tag);
           cur_entity.type = EntityTypeAction;
           store_current_entity();
        }
        ;
        
typed_id:
        ID
        {
           store_cur_typed_entity("", $1);
        }
        |
        ID '<' ID '>'
        {
           store_cur_typed_entity($3, $1);
        }
        ;
        
parameters:
        /* possibly empty */
        |
        RAW_CODE
        {
           store_typed_entity_parameters($1);
        }
        ;
%%

static char *strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);   // Space for length plus nul
    if (d == NULL) return NULL;          // No memory
    strcpy (d,s);                        // Copy the characters
    return d;                            // Return the new string
}

static int lexical_compare(const void *pa, const void *pb)
{
   return strcmp(pa, pb);
}

static int entity_compare(const void *pa, const void *pb)
{
   TypedEntity *ea = (TypedEntity*)pa;
   TypedEntity *eb = (TypedEntity*)pb;
   return strcmp(ea->name, eb->name);
}

static int pattern_compare(const void *pa, const void *pb)
{
   Phrase *ea = (Phrase*)pa;
   Phrase *eb = (Phrase*)pb;
   return strcmp(ea->name, eb->name);
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

static void save_phrase(const char *name) {

   TypedEntity *new_entity = (TypedEntity*)malloc(sizeof(TypedEntity));
   
   new_entity->name = strdup(name);
   new_entity->type = EntityTypePattern;
   
   store_entity(new_entity);

   Phrase *new_pattern = (Phrase*)malloc(sizeof(Phrase));
   
   new_pattern->members = (char**)malloc(sizeof(char*)*(cur_token + 1));
   
   for(int i = 0; i <= cur_token; ++i) {
      new_pattern->members[i] = token_definitions[i];
   }
   
   new_pattern->n_members = cur_token + 1;
   new_pattern->name = strdup(name);
   
   cur_token = -1;
   
   tsearch((void *)new_pattern, &pattern_tree_root, pattern_compare);
}

static Phrase *lookup_phrase(const char *name) {

   Phrase tmp = { .name = name };

   void *val = tfind((void *)&tmp, &pattern_tree_root, pattern_compare);
   
   if(!val) {
      MY_ERROR("Unable to find phrase \'%s\'\n", name);
   }
   
   return *(Phrase**)val;
}

static void push_phrase(const char *name) {

   Phrase *phrase = lookup_phrase(name);
   
   for(int i = 0; i < phrase->n_members; ++i) {
      ++cur_token;
      token_definitions[cur_token] = phrase->members[i];
   }
}

static void flush_new_tokens(void)
{
   for(int i = 0; i <= cur_new_token; ++i) {
      ++cur_token;
      token_definitions[cur_token] = new_token_definitions[i];
   }
   
   cur_new_token = -1;
}

#define SAFE_FREE(S) free(S); S = NULL;

#define BUFF_MAX 1024
#define BUFF_REST (BUFF_MAX - (buff_pos - buffer))

#define BUFF_PRINT(...) buff_pos += snprintf(buff_pos, BUFF_REST, __VA_ARGS__)


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

static void add_action_to_token_relative(int rel_count, const char *action_id)
{
   add_action_to_token_n(token_start + rel_count, action_id);
}

static void add_action_to_token_n(int token_pos, const char *action_id)
{
   if(token_pos > cur_new_token) {
      MY_ERROR("Unable to access undefined token %d\n", token_pos);
   }

   char buffer[BUFF_MAX];
   
   char *old_token = new_token_definitions[token_pos];
   
   TypedEntity *tE = look_up_entity(action_id);
   if(!tE) {
      MY_ERROR("Action %s unregistered\n", action_id);
   }
   
   snprintf(buffer, BUFF_MAX, "ppg_token_set_action(%s, PPG_ACTION_MAP_%s(%s, %s))", old_token, tE->tag, tE->name, tE->parameters);
   
   new_token_definitions[token_pos] = strdup(buffer);
   
   free(old_token);
}

static void add_action_to_token(const char *token_count_id, const char *action_id)
{
   int token_count = my_atol(token_count_id);
   
   add_action_to_token_relative(token_count - 1, action_id);
}

static void generate_note(const char *input_id, const char *flags)
{
   ++cur_new_token;
   
   if(cur_new_token >= MAX_TOKENS) {
      MY_ERROR("Maximum number of tokens %d exceeded\n", cur_new_token);
   }
   
   token_start = cur_new_token;
   
   char buffer[BUFF_MAX];
   
   TypedEntity *tE = look_up_entity(input_id);
   if(!tE) {
      MY_ERROR("Input %s unregistered\n", input_id);
   }
   
   snprintf(buffer, BUFF_MAX, "ppg_note_create(PPG_INPUT_KEYWORD_MAPPING_%s(%s, %s), %s)", tE->tag, tE->name, tE->parameters, flags);
   
   new_token_definitions[cur_new_token] = strdup(buffer);
}

static void generate_note_activation(const char *input_id)
{
   generate_note(input_id, "PPG_Note_Flag_Match_Activation");
}

static void generate_note_deactivation(const char *input_id)
{
   generate_note(input_id, "PPG_Note_Flag_Match_Deactivation");
}

static void generate_note_full(const char *input_id)
{
   generate_note(input_id, "PPG_Note_Flags_A_N_D");
}

static void generate_aggregate(const char *aggr_type)
{
   ++cur_new_token;
   
   if(cur_new_token >= MAX_TOKENS) {
      MY_ERROR("Maximum number of tokens %d exceeded\n", cur_new_token);
   }
   
   token_start = cur_new_token;
   
   char buffer[BUFF_MAX];
   
   char *buff_pos = buffer;
   
   BUFF_PRINT("PPG_%s_CREATE(", aggr_type);
   
   for(int i = 0; i <= cur_id; ++i) {
   
      TypedEntity *tE = look_up_entity(inputs[i]);
      if(!tE) {
         MY_ERROR("Input %s unregistered\n", inputs[i]);
      }
   
      BUFF_PRINT("PPG_INPUT_KEYWORD_MAPPING_%s(%s, %s)", tE->tag, tE->name, tE->parameters);
      if(i < cur_id) {
         BUFF_PRINT(",");
      }
   }
   
   BUFF_PRINT(")");
   
   new_token_definitions[cur_new_token] = strdup(buffer);
   
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

   char *start_string = new_token_definitions[cur_new_token];
   for(long i = 1; i < max_count_id; ++i) {
      ++cur_new_token;
         
      if(cur_new_token >= MAX_TOKENS) {
         MY_ERROR("Maximum number of tokens %d exceeded\n", cur_new_token);
      }
      
      new_token_definitions[cur_new_token] = strdup(start_string);
   }
}

static void set_current_layer(const char *layer_id)
{
   fprintf(
      out_file,
"current_layer = %s;\n\n",
      layer_id
   );
}

static void mark_symbol(const char *symbol_id)
{
   fprintf(
      symbols_file,
"   OP(%s) \\\n", 
      symbol_id
   );
}

static void store_current_entity(void)
{
   if(!cur_entity.parameters) {
      cur_entity.parameters = strdup("");
   }   
   
   TypedEntity *entity_copy = (TypedEntity*)malloc(sizeof(TypedEntity));
   *entity_copy = cur_entity;
   
   cur_entity.name = NULL;
   cur_entity.tag = NULL;
   cur_entity.parameters = NULL;
   cur_entity.type = EntityTypeNone;
   
   store_entity(entity_copy);
}

static void store_entity(TypedEntity *entity)
{
   void *val = tfind((void *)entity, &entity_tree_root, entity_compare);
   
   if(val) {
      MY_ERROR("Entity \'%s\' ambiguously defined\n", cur_entity.name);
   }
   
   val = tsearch((void *)entity, &entity_tree_root, entity_compare);
}

static TypedEntity *look_up_entity(const char *name)
{
   TypedEntity tmp;
      tmp.name = name;
      
   void *val = tfind((void *)&tmp, &entity_tree_root, entity_compare);
   
   if(!val) {
      return NULL;
   }
   
   return (TypedEntity *)*((void**)val);
}

static void store_cur_typed_entity(const char *tag_id, const char *name)
{
   cur_entity.tag = strdup(tag_id);
   cur_entity.name = strdup(name);
}

static void store_typed_entity_parameters(const char *raw_code)
{
   // Copy parameters and remove leading ===
   
   cur_entity.parameters = strdup(raw_code + 3);
}

static void store_input_tag(const char *tag)
{
   void *val = tfind((void *)tag, &input_tag_tree_root, lexical_compare);
   if(val) { return; }
   
   val = tsearch((void *) strdup(tag), &input_tag_tree_root, lexical_compare);
}

static void store_action_tag(const char *tag)
{
   void *val = tfind((void *)tag, &action_tag_tree_root, lexical_compare);
   if(val) { return; }
   
   val = tsearch((void *) strdup(tag), &action_tag_tree_root, lexical_compare);
}

const char *cur_tag = NULL;

#define MAX_ITER_ENTITIES 4096
const TypedEntity *iter_entities[MAX_ITER_ENTITIES];
int iter_entity = -1;

static void input_visitor(const void *node, VISIT visit, int dummy) {
   if((visit == preorder) || (visit == postorder)) { return; }
   
   const TypedEntity *tE = *((const TypedEntity**)node);
   if(tE->type != EntityTypeInput) { return; }
   if(strcmp(cur_tag, tE->tag)) { return; }
   
   iter_entities[iter_entity] = tE;
   ++iter_entity;
}

static void input_tag_visitor(const void *node, VISIT visit, int dummy) {
   if((visit == preorder) || (visit == postorder)) { return; }

   const char *tag = *((const char**)node);

   fprintf(inputs_file, "#define PPG_INPUTS___%s(OP) \\\n", tag);
   cur_tag = tag;
   
   iter_entity = 0;
   
   twalk(entity_tree_root, input_visitor);
   
   for(int i = 0; i < iter_entity; ++i) {
      const TypedEntity *tE = iter_entities[i];
      fprintf(inputs_file, "   OP(%s, %s)", tE->name, tE->parameters);
      if(i < iter_entity - 1) {
         fprintf(inputs_file, " \\");
      }
      fprintf(inputs_file, "\n");
   }
   
   fprintf(inputs_file, "\n");
}
   
static void flush_inputs(void)
{
   twalk(input_tag_tree_root, input_tag_visitor);
}

static void action_visitor(const void *node, VISIT visit, int dummy) {
   if((visit == preorder) || (visit == postorder)) { return; }
   
   const TypedEntity *tE = *((const TypedEntity**)node);
   if(tE->type != EntityTypeAction) { return; }
   if(strcmp(cur_tag, tE->tag)) { return; }
   
   iter_entities[iter_entity] = tE;
   ++iter_entity;
}

static void action_tag_visitor(const void *node, VISIT visit, int dummy) {
   if((visit == preorder) || (visit == postorder)) { return; }
   
   const char *tag = *((const char**)node);
   
   fprintf(actions_file, "#define PPG_ACTIONS___%s(OP) \\\n", tag);
   cur_tag = tag;
   
   iter_entity = 0;
   
   twalk(entity_tree_root, action_visitor);
      
   for(int i = 0; i < iter_entity; ++i) {
      const TypedEntity *tE = iter_entities[i];
      fprintf(actions_file, "   OP(%s, %s)", tE->name, tE->parameters);
      if(i < iter_entity - 1) {
         fprintf(actions_file, " \\");
      }
      fprintf(actions_file, "\n");
   }
   
   fprintf(actions_file, "\n");
}
   
static void flush_actions(void)
{
   twalk(action_tag_tree_root, action_tag_visitor);
}

static void prepare_symbols_file(void)
{
   fprintf(symbols_file, "#define PPG_SYMBOLS___(OP)\n");
}

static void finish_symbols_file(void)
{
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

static void write_header(FILE *out_file)
{
   fprintf(out_file, 
"#include \"ppg_note.h\"\n"
"#include \"ppg_cluster.h\"\n"
"#include \"ppg_chord.h\"\n"
"#include \"ppg_pattern.h\"\n"
"#include \"ppg_action.h\"\n"
"\n"
"#ifndef PPG_INPUT_KEYWORD_MAPPING\n"
"#define PPG_INPUT_KEYWORD_MAPPING(S) S\n"
"#endif\n"
"\n"
   );
}

#define PPG_START_TOKEN "ppg_start"
#define PPG_END_TOKEN "ppg_end"
#define PPG_START_DEFINITIONS_TOKEN "ppg_definitions_start"
#define PPG_END_DEFINITIONS_TOKEN "ppg_definitions_end"

void generate(const char *source_filename, 
              const char *output_filename,
              const char *inputs_filename,
              const char *actions_filename,
              const char *symbols_filename)
{
   FILE *source_file = fopen(source_filename, "r");
   out_file = fopen(output_filename, "w");
   inputs_file = fopen(inputs_filename, "w");
   actions_file = fopen(actions_filename, "w");
   symbols_file = fopen(symbols_filename, "w");
   
   prepare_symbols_file();
   
   write_header(out_file);
         
   char line[4096];
   
   bool in_ppg = false;
   bool was_in_ppg = false;
   bool in_definitions = false;
   bool was_in_definitions = false;

   cur_line = 0;
   
   while (fgets(line, sizeof(line), source_file)) {
   
      ++cur_line;
      
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
           
      if(strstr(line, PPG_START_TOKEN) != NULL) {
         printf("line %d: in ppg\n", cur_line);
         in_ppg = true;
         was_in_ppg = true;
         continue;
      }
      
      if(strstr(line, PPG_END_TOKEN) != NULL) {
         printf("line %d: end of ppg\n", cur_line);
         break;
      }
      
      if(strstr(line, PPG_START_DEFINITIONS_TOKEN) != NULL) {
         printf("line %d: in definitions\n", cur_line);
         in_definitions = true;
         was_in_definitions = true;
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
      
      // Remove comments
      //
      for(char *c = line; *c != '\0'; ++c) {
         if(*c == '%') {
            *c = '\n';
            ++c;
            *c = '\0';
            break;
         }
      }
      
      if(in_definitions) {
         printf("Processing line \'%s\'\n", line);
         process_definitions(line);
      }
      else {
         fprintf(out_file, "%s", line);
      }
   }
   
   flush_inputs();
   flush_actions();
   
   finish_symbols_file();
   
   fclose(source_file);
   fclose(out_file);
   fclose(inputs_file);
   fclose(actions_file);
   fclose(symbols_file);
   
   out_file = NULL;
   inputs_file = NULL;
   actions_file = NULL;
   symbols_file = NULL;
   
   if(!was_in_ppg) {
      fprintf(stderr, "No tag \'" PPG_START_TOKEN "\' encountered\n");
      exit(EXIT_FAILURE);
   }
   
   if(!was_in_definitions) {
      fprintf(stderr, "No tag \'" PPG_END_DEFINITIONS_TOKEN "\' encountered\n");
      exit(EXIT_FAILURE);
   }
}

int main(int argc, char **argv) {
   
   if(argc < 6) {
     fprintf(stderr, "usage: %s <input_file> <output_file> <inputs_file> <actions_file> <symbols_file>\n", argv[0]);
     return 1;
   }
   
   generate(argv[1], argv[2], argv[3], argv[4], argv[5]);
   
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

