%{

#include "compiler.h"

#include <stdio.h>
#include <search.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

#include "compiler.lex.h"

#include "cmdline.h"

struct gengetopt_args_info ai;

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

FILE *definitions_file = NULL;
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
      definitions_file, 
"ppg_pattern(\n"
"   current_layer,\n"
"   %d, /* number of tokens */\n"
"   PPG_TOKENS(\n",
      cur_token + 1);

   for(int i = 0; i <= cur_token; ++i) {
      fprintf(
         definitions_file,
"      %s",
         token_definitions[i]
      );
      
      free(token_definitions[i]);
      
      if(i < cur_token) {
         fprintf(definitions_file, ",");
      }
      
      fprintf(definitions_file, "\n");
   }
   
   fprintf(
      definitions_file, 
"   )\n"
   );     
   fprintf(
      definitions_file,
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
   char *buff_pos = buffer;
   
   char *old_token = new_token_definitions[token_pos];
   
   TypedEntity *tE = look_up_entity(action_id);
   if(!tE) {
      MY_ERROR("Action %s unregistered\n", action_id);
   }
   
   BUFF_PRINT("ppg_token_set_action(%s, PPG_ACTION_MAP_%s(%s",
         old_token, tE->tag, tE->name);
   if(tE->parameters) {
      BUFF_PRINT(", %s", tE->parameters);
   }
   BUFF_PRINT("))");
   
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
   
snprintf(buffer, BUFF_MAX, "ppg_note_create(PPG_INPUT_MAP_%s(%s, %s), %s)", tE->tag, tE->name, tE->parameters, flags);
   
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
   
      BUFF_PRINT("PPG_INPUT_MAP_%s(%s, %s)", tE->tag, tE->name, tE->parameters);
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
      definitions_file,
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
   fprintf(inputs_file, 
"#ifndef PPG_INPUT_MAP_%s\n", tag
   );
   fprintf(inputs_file,
"#define PPG_INPUT_MAP_%s(...) __VA_ARGS__\n", tag
   );
   fprintf(inputs_file,
"#endif\n"
"\n"
   );
   
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
   
   fprintf(actions_file, 
"#ifndef PPG_ACTION_MAP_%s\n", tag
   );
   fprintf(actions_file,
"#define PPG_ACTION_MAP_%s(...) __VA_ARGS__\n", tag
   );
   fprintf(actions_file,
"#endif\n"
"\n"
   );
   
   fprintf(actions_file, 
"#ifndef PPG_ACTION_INITIALIZATION_%s\n", tag
   );
   fprintf(actions_file,
"#define PPG_ACTION_INITIALIZATION_%s(...) __VA_ARGS__\n", tag
   );
   fprintf(actions_file,
"#endif\n"
"\n"
   );
   
   fprintf(actions_file, "#define PPG_ACTIONS___%s(OP) \\\n", tag);
   cur_tag = tag;
   
   iter_entity = 0;
   
   twalk(entity_tree_root, action_visitor);
      
   for(int i = 0; i < iter_entity; ++i) {
      const TypedEntity *tE = iter_entities[i];
      fprintf(actions_file, "   OP(%s", tE->name);
      if(tE->parameters) {
         fprintf(actions_file, ", %s", tE->parameters);
      }
      fprintf(actions_file, ")");
      if(i < iter_entity - 1) {
         fprintf(actions_file, " \\");
      }
      fprintf(actions_file, "\n");
   }
   
   fprintf(actions_file, "\n");
   
   fprintf(actions_file, 
"PPG_ACTIONS___%s(PPG_ACTION_INITIALIZATION_%s)\n\n", tag, tag);
}
   
static void flush_actions(void)
{
   twalk(action_tag_tree_root, action_tag_visitor);
}

static void prepare_symbols_file(void)
{
   fprintf(symbols_file, "#define PPG_FOR_EACH_SYMBOL___(OP) \\\n");
}

static void finish_symbols_file(void)
{
   fprintf(symbols_file, "\n");
}

static void append_file(FILE *source, FILE *target, const char *indent)
{
   rewind(source);
   
   char line[4096];
   
   while (fgets(line, sizeof(line), source)) {
      fprintf(target, "%s%s", indent, line);
   }
}

static void append_if_present(const char *source_filename, FILE *target, const char *indent) 
{
   if(!source_filename) { return; }
   
   FILE *source_file = fopen(source_filename, "r");
   fprintf(target, "// Read from %s\n\n", source_filename);
   append_file(source_file, target, indent);
   fprintf(target, "\n");
   fclose(source_file);
}

static void generate_output_file(void)
{
   FILE  *output_file = fopen(ai.output_filename_arg, "w");
   
   append_if_present(ai.preamble_filename_arg, output_file, "");
   
   fprintf(output_file,
"#include \"ppg_note.h\"\n"
"#include \"ppg_cluster.h\"\n"
"#include \"ppg_chord.h\"\n"
"#include \"ppg_pattern.h\"\n"
"#include \"ppg_action.h\"\n"
"\n"
   );
   append_file(symbols_file, output_file, "");
   append_file(inputs_file, output_file, "");
   append_file(actions_file, output_file, "");
   
   append_if_present(ai.initialization_filename_arg, output_file, "");
   
   fprintf(output_file,
"\n"
"void generate_tree(void *context)\n"
"{\n"
"   int current_layer = 0;\n\n"
   );
   
   append_if_present(ai.init_tree_generation_filename_arg, output_file, "   ");
   append_file(definitions_file, output_file, "   ");
   append_if_present(ai.finish_tree_generation_filename_arg, output_file, "   ");
   
   fprintf(output_file,
"\n"
"   ppg_global_compile();\n"
"}\n"
"\n"
   );
   
   fprintf(output_file,
"#define DEFINE_DUMMY_SYMBOL(S) \\\n"
"   void S(void) {}\n"
"\n"
"PPG_FOR_EACH_SYMBOL___(DEFINE_DUMMY_SYMBOL)\n"
"\n"
"void compress_tree(void *context, const char *output_filename)\n"
"{\n"
"#define REGISTER_DUMMY_SYMBOL(S) \\\n"
"   PPG_COMPRESSION_REGISTER_SYMBOL(context, S)\n"
"\n"
"   PPG_FOR_EACH_SYMBOL___(REGISTER_DUMMY_SYMBOL)\n"
"\n"
   );
   
   append_if_present(ai.init_compression_filename_arg, output_file, "");
   
   fprintf(output_file,
"   ppg_compression_run(context, \"%s\", output_filename);\n", (ai.project_name_arg) ? ai.project_name_arg : "generic"
   );
   fprintf(output_file,
"   ppg_compression_finalize(context);\n"
"}\n"
"\n"
   );

   fprintf(output_file,
"#ifdef PAPAGENO_HAVE_MAIN\n"
"\n"
"int main(int argc, char **argv)\n"
"{\n"
"   if(argc < 2) {\n"
"      fprintf(stderr, \"Usage: %%s <output_filename>\\n\", argv[0]);\n"
"      exit(EXIT_FAILURE);\n"
"   }\n"
"\n"
"   void *context = ppg_context_create();\n"
"   ppg_global_set_current_context(context);\n"
"\n"
"   generate_tree(context);\n"
"\n"
"   compress_tree(context, argv[1]);\n"
"\n"
"   ppg_context_destroy(context);\n"
"}\n"
"\n"
"#endif\n"
   );
   
   fclose(output_file);
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

static void write_header(FILE *definitions_file)
{
}

#define PPG_START_TOKEN "papageno_start"
#define PPG_END_TOKEN "papageno_end"
#define PPG_START_INLINE_TOKEN "inline_start"
#define PPG_END_INLINE_TOKEN "inline_end"

void generate()
{
   FILE *source_file = fopen(ai.source_filename_arg, "r");
   definitions_file = tmpfile(); //fopen(ai.output_filename, "w");
   inputs_file = tmpfile();//fopen(inputs_filename, "w");
   actions_file = tmpfile();//fopen(actions_filename, "w");
   symbols_file = tmpfile();//fopen(symbols_filename, "w");
   
   prepare_symbols_file();
   
   write_header(definitions_file);
         
   char line[4096];
   
   bool in_ppg = false;
   bool was_in_ppg = false;
   bool in_inline = false;

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
      
      if(!in_ppg) {
         if(was_in_ppg) { break; }
         
         continue;
      }
      
      if(strstr(line, PPG_END_TOKEN) != NULL) {
         printf("line %d: end of ppg\n", cur_line);
         break;
      }
      
      if(strstr(line, PPG_START_INLINE_TOKEN) != NULL) {
         printf("line %d: in inline\n", cur_line);
         in_inline = true;
         continue;
      }
      
      if(strstr(line, PPG_END_INLINE_TOKEN) != NULL) {
         printf("line %d: end of inline\n", cur_line);
         in_inline = false;
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
      
      if(in_inline) {
         fprintf(definitions_file, "%s", line);
      }
      else {
         printf("Processing line \'%s\'\n", line);
         process_definitions(line);
      }
   }
   
   flush_inputs();
   flush_actions();
   
   finish_symbols_file();
   
   generate_output_file();
   
   fclose(source_file);
   fclose(definitions_file);
   fclose(inputs_file);
   fclose(actions_file);
   fclose(symbols_file);
   
   definitions_file = NULL;
   inputs_file = NULL;
   actions_file = NULL;
   symbols_file = NULL;
   
   if(!was_in_ppg) {
      fprintf(stderr, "No papageno definitions encountered\n");
      exit(EXIT_FAILURE);
   }
}

int main(int argc, char **argv)
{
   if(cmdline_compiler(argc, argv, &ai) != 0) {
      exit(1);
   }
   
   generate();
   
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
