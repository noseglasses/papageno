/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "detail/ppg_compression_detail.h"
#include "detail/ppg_context_detail.h"
#include "detail/ppg_token_detail.h"
#include "detail/ppg_note_detail.h"
#include "detail/ppg_chord_detail.h"
#include "detail/ppg_cluster_detail.h"

#include "assert.h"

static void ppg_compression_context_symbol_buffer_resize(
                           PPG_Compression_Symbol_Buffer *symbols,
                           PPG_Count new_size)
{
   if(new_size == 0) {
      new_size = 4;
   }
   
   PPG_Compression_Symbol *new_buffer
      = (PPG_Compression_Symbol *)calloc(1, 
                     new_size*sizeof(PPG_Compression_Symbol));
      
   if(!symbols->buffer) {
      symbols->buffer = new_buffer;
      return;
   }
   
   PPG_ASSERT(new_size > symbols->n_allocated);
   
   for(PPG_Count i = 0; i < symbols->n_stored; ++i) {
      
      new_buffer[i] = symbols->buffer[i];
   }
   
   free(symbols->buffer);
   
   symbols->buffer = new_buffer;
   
   symbols->n_allocated = new_size;
}
   
PPG_Compression_Context ppg_compression_init(void)
{
   PPG_Compression_Context__ *ccontext
      = (PPG_Compression_Context__ *)calloc(1, sizeof(PPG_Compression_Context__));
      
   ccontext->symbols_lookup.buffer = NULL;
   ccontext->symbols_lookup.n_allocated = 0;
   ccontext->symbols_lookup.n_stored = 0;
      
   ppg_compression_context_symbol_buffer_resize(&ccontext->symbols_lookup, 4);
   
   ccontext->target_storage = NULL;
   ccontext->symbols = NULL;
   ccontext->vptrs = NULL;
   ccontext->storage_size = 0;
   
   return (PPG_Compression_Context)ccontext;
}

void ppg_compression_finalize(PPG_Compression_Context ccontext)
{
   if(!ccontext) { return; }
   
   PPG_Compression_Context__ *ccontext__  
                  = (PPG_Compression_Context__ *)ccontext;
   
   if(ccontext__->symbols_lookup.buffer) {
      free(ccontext__->symbols_lookup.buffer);
      ccontext__->symbols_lookup.buffer = NULL;
   }
   
   if(ccontext__->target_storage) {
      free(ccontext__->target_storage);
   }
   
   free(ccontext__);
}

void ppg_compression_register_symbol(
                         PPG_Compression_Context ccontext,
                         void *symbol,
                         char *symbol_name)
{
   if(!symbol) { return; }
   
   printf("Registering symbol %s = %p\n", symbol_name, symbol);
   
   PPG_Compression_Context__ *ccontext__  
                  = (PPG_Compression_Context__ *)ccontext;
                  
   PPG_ASSERT(ccontext__);
   PPG_ASSERT(ccontext__->symbols_lookup.buffer);
   
   if(ccontext__->symbols_lookup.n_stored == ccontext__->symbols_lookup.n_allocated) {
      
      // Double the size with each allocation
      //
      ppg_compression_context_symbol_buffer_resize(   
                                             &ccontext__->symbols_lookup,
                                             2*ccontext__->symbols_lookup.n_allocated);
   }
   
   ccontext__->symbols_lookup.buffer[ccontext__->symbols_lookup.n_stored] =
      (PPG_Compression_Symbol) {
         .address = symbol,
         .name = symbol_name
      };
   
   ++ccontext__->symbols_lookup.n_stored;
}

typedef struct {
   size_t memory;
   size_t n_tokens;
} PPG_Compression_Size_Data;

static void ppg_compression_collect_token_size_requirement(
                                 PPG_Token__ *token, void *user_data)
{
   PPG_Compression_Size_Data *size_data = (PPG_Compression_Size_Data *)user_data;
   
   size_data->memory += PPG_CALL_VIRT_METHOD(token, dynamic_size);
   
   ++size_data->n_tokens;
}

static size_t ppg_compression_allocate_target_buffer(PPG_Compression_Context__ *ccontext)
{
   PPG_Compression_Size_Data size_data =
      (PPG_Compression_Size_Data){
         .memory = 0,
         .n_tokens = 0
      };
   
   size_data.memory += sizeof(PPG_Context);
   
   // Collect the size requirements of all tokens in the tree
   //
   ppg_token_traverse_tree(ppg_context->pattern_root,
                           (PPG_Token_Tree_Visitor)ppg_compression_collect_token_size_requirement,
                           NULL,
                           (void *)&size_data);
   
   PPG_ASSERT(!ccontext->target_storage);
   
   printf("Allocating target_storage size %lu\n", size_data.memory);
   
   ccontext->target_storage = (char*)calloc(size_data.memory, sizeof(char));
   
   printf("Allocating target_storage %p\n", ccontext->target_storage);
   
   ccontext->storage_size = size_data.memory;
   
   return size_data.n_tokens;
}

static void ppg_compression_copy_token(
                                 PPG_Token__ *token, void *user_data)
{
//    printf("Original:\n");
//    PPG_CALL_VIRT_METHOD(token, print_self, 0, false);
   
   char **target = (char **)user_data;
   
   PPG_Token__ *new_token = (PPG_Token__ *)*target;
   
   *target = PPG_CALL_VIRT_METHOD(token, placement_clone, *target);
   
//    printf("Clone raw:\n");
//    PPG_CALL_VIRT_METHOD(new_token, print_self, 0, false);
   
   // Abuse the old tree's temporarily to
   // store child ptr. Node parent pointers are thereby
   // used to store node copies. 
   // This assumes that the
   // parent ptrs are restored afterwards through
   // a tree traversal.
   //
   if(token->parent) {
      
      if(token->parent->parent) {
      
         // Note: Here token->parent->parent is actually the clone of token->parent
         //
         new_token->parent = token->parent->parent;
      }
      
      // Get the storage index of the current node
      //
      PPG_Count child_id = 0;
      for(; child_id < token->parent->n_children; ++child_id) {
         
         if(token->parent->children[child_id] == token) {
            break;
         }
      }
      
//       printf("Resetting child %d\n", child_id);
      new_token->parent->children[child_id] = new_token;
   }
   
   token->parent = new_token;
   
//    printf("Original after:\n");
//    PPG_CALL_VIRT_METHOD(token, print_self, 0, false);
//    
//    printf("\nClone:\n");
//    PPG_CALL_VIRT_METHOD(new_token, print_self, 0, false);
}

static void ppg_compression_generate_relative_addresses(
                                 PPG_Token__ *token, void *begin_of_buffer)
{
   for(PPG_Count i = 0; i < token->n_children; ++i) {
      token->children[i] = (PPG_Token__ *)((char*)token->children[i] - (char*)begin_of_buffer);
   }
   
   token->children = (PPG_Token__ **)((char*)token->children - (char*)begin_of_buffer);
}

static void ppg_compression_generate_absolute_addresses(
                                 PPG_Token__ *token, void *begin_of_buffer)
{
   token->children = (PPG_Token__ **)((char*)begin_of_buffer + (uintptr_t)token->children);
   
   for(PPG_Count i = 0; i < token->n_children; ++i) {
      token->children[i] = (PPG_Token__ *)((char*)begin_of_buffer + (uintptr_t)token->children[i]);
   }
}

static void ppg_compression_restore_tree_relations(PPG_Token__ *token)
{
   for(PPG_Count i = 0; i < token->n_children; ++i) {
      token->children[i]->parent = token;
      
      ppg_compression_restore_tree_relations(token->children[i]);
   }
}

static void ppg_compression_copy_context(PPG_Compression_Context__ *ccontext)
{
   char *target = ccontext->target_storage;
   
   PPG_Context *target_context = (PPG_Context*)target;
   
   *target_context = *ppg_context;
   
   target += sizeof(PPG_Context);
   
   target_context->pattern_root = (PPG_Token__ *)target;
   
   ppg_token_traverse_tree(ppg_context->pattern_root,
                           (PPG_Token_Tree_Visitor)ppg_compression_copy_token,
                           NULL,
                           (void *)&target);
   
   // We destroyed parent-child relation during the previous tree traverse.
   // Now we restore it.
   //
   ppg_context->pattern_root->parent = NULL;
   ppg_compression_restore_tree_relations(ppg_context->pattern_root);
}

static void ppg_compression_convert_all_addresses_to_relative(PPG_Compression_Context__ *ccontext)
{
   char *target = ccontext->target_storage;
   
   PPG_Context *target_context = (PPG_Context*)target;
   
   // Convert pointers to relative addresses
   //
   ppg_token_traverse_tree(target_context->pattern_root,
                           NULL,
                           (PPG_Token_Tree_Visitor)ppg_compression_generate_relative_addresses,
                           (void *)&target);
   
   target_context->pattern_root = (PPG_Token__ *)((char*)target_context->pattern_root 
                                          - target);
}

static void ppg_compression_register_pointers(
                                 PPG_Token__ *token, void *user_data)
{
   PPG_Compression_Context__ *ccontext
      = (PPG_Compression_Context__ *)user_data;
      
   PPG_CALL_VIRT_METHOD(token, register_ptrs_for_compression, ccontext);
}

void ppg_compression_generate_dynamic_assignment_information(
                                          PPG_Compression_Context__ *ccontext)
{
   // The context resides at the front of the target storage
   //
   PPG_Context *context = (PPG_Context *)ccontext->target_storage;
   
   // Register dynamic symbols of the context
   
   if(context->event_processor) {
      ppg_compression_context_register_symbol((void**)&context->event_processor, ccontext);
   }
   
   if(context->signal_callback.func) {
      ppg_compression_context_register_symbol((void**)&context->signal_callback.func, ccontext);
   }
   
   if(context->signal_callback.user_data) {
      ppg_compression_context_register_symbol((void**)&context->signal_callback.user_data, ccontext);
   }
   
   if(context->time_manager.time) {
      ppg_compression_context_register_symbol((void**)&context->time_manager.time, ccontext);
   }
   
   if(context->time_manager.time_difference) {
      ppg_compression_context_register_symbol((void**)&context->time_manager.time_difference, ccontext);
   }
   
   if(context->time_manager.compare_times) {
      ppg_compression_context_register_symbol((void**)&context->time_manager.compare_times, ccontext);
   }
   
   // Traverse the search tree and collect any dynamic symbols of tokens
   //
   ppg_token_traverse_tree(context->pattern_root,
                           (PPG_Token_Tree_Visitor)ppg_compression_register_pointers,
                           NULL,
                           (void *)ccontext);
}

void ppg_compression_write_c_char_array(char *array_name,
                                        char *array,
                                        size_t size)
{
   printf("char %s[] = {\n", array_name);
    
//    printf("array size: %lu\n", size);
//    printf("array: %p\n", array);
   
   #define ROW_LENGTH 16
   
   size_t n_rows = size / ROW_LENGTH;
   size_t n_remaining = size % ROW_LENGTH;
   
//    printf("n_rows: %lu\n", n_rows);
//    printf("n_remaining: %lu\n", n_remaining);
   
   for(size_t row = 0; row < n_rows; ++row) {
      
      printf("   ");
      
      for(size_t col = 0; col < ROW_LENGTH; ++col) {
         printf("0x%02x, ", (int)(array[row*ROW_LENGTH + col] & 0xff));
      }
      
      printf("\n");
   }
   
   if(n_remaining) {
      
      for(size_t col = 0; col < n_remaining; ++col) {
         printf("0x%02x, ", (int)(array[n_rows*ROW_LENGTH + col] & 0xff));
      }
      
      printf("\n");
   }
   
   printf("};\n\n");
}

void ppg_compression_register_aux_array(void *context, 
                                        void *aux_array, 
                                        size_t aux_array_size)
{
   size_t n_vptrs = aux_array_size/sizeof(PPG_Compression_VPtr_Info);
   
   PPG_Compression_VPtr_Info *vptr_info 
      = (PPG_Compression_VPtr_Info *)aux_array;
      
   char *context_c = (char*)context;
   
   for(size_t i = 0; i < n_vptrs; ++i) {
      
      uintptr_t *vptr_pos = (uintptr_t *)&context_c[vptr_info[i].offset];
      
      switch(vptr_info[i].token_type_id) {
         case 1:
            *vptr_pos = (uintptr_t)&ppg_token_vtable;
            break;
         case 2:
            *vptr_pos = (uintptr_t)&ppg_note_vtable;
            break;
         case 3:
            *vptr_pos = (uintptr_t)&ppg_chord_vtable;
            break;
         case 4:
            *vptr_pos = (uintptr_t)&ppg_cluster_vtable;
            break;
         default:
            assert(0);
            break;
      }
   }
   
   PPG_Context *the_context = (PPG_Context *)context;
   
   // Convert relative addresses to absolute addresses
   //
   the_context->pattern_root = (PPG_Token__*)((char*)context_c 
                                          + (uintptr_t)the_context->pattern_root);
   
   ppg_token_traverse_tree(the_context->pattern_root,
                           (PPG_Token_Tree_Visitor)ppg_compression_generate_absolute_addresses,
                           NULL,
                           (void *)context);
   
}

void ppg_compression_write_c_output(PPG_Compression_Context__ *ccontext,
                                    char *name_tag)
{
   // Start with writing the raw data
   //
   char context_name[100], aux_name[100];
   
   sprintf(context_name, "%s_context", name_tag);
   sprintf(aux_name, "%s_context_aux", name_tag);
   
   ppg_compression_write_c_char_array(context_name,
                                      ccontext->target_storage,
                                      ccontext->storage_size);
   
   // Prepare an array with information about vptrs
   //
   PPG_Compression_VPtr_Info vptr_info[ccontext->n_vptrs];
   
   for(size_t i = 0; i < ccontext->n_vptrs; ++i) {
      
      vptr_info[i].offset = (char*)ccontext->vptrs[i] 
                  - ccontext->target_storage;
      
      if(*ccontext->vptrs[i] == &ppg_token_vtable) {
         vptr_info[i].token_type_id = 1;
      }
      else if(*ccontext->vptrs[i] == &ppg_note_vtable) {
         vptr_info[i].token_type_id = 2;
      }
      else if(*ccontext->vptrs[i] == &ppg_chord_vtable) {
         vptr_info[i].token_type_id = 3;
      }
      else if(*ccontext->vptrs[i] == &ppg_cluster_vtable) {
         vptr_info[i].token_type_id = 4;
      }
   }
   
   ppg_compression_write_c_char_array(
            aux_name,
            (char*)vptr_info,
            ccontext->n_vptrs*sizeof(PPG_Compression_VPtr_Info));
   
   printf("#define PPG_INITIALIZE_CONTEXT_%s \\\n", name_tag);
   printf("   \\\n");
   
//    printf("   ccontext->target_storage = %p\n", ccontext->target_storage);
      
   for(size_t i = 0; i < ccontext->n_symbols; ++i) {
      
//       printf("   ccontext->symbols[i] = %p\n", ccontext->symbols[i]);
      
      size_t offset = (char*)ccontext->symbols[i] - ccontext->target_storage;
      
      // Lookup the symbol name.
      // 
      // TODO: Turn this inefficient linear search into something more 
      //       efficient by, e.g. sorting the buffer first
      //       and then perform a binary search.
      //
      size_t s = 0;
      for(; s < ccontext->symbols_lookup.n_stored; ++s) {
         
//          printf("Checking symbol %s\n", ccontext->symbols_lookup.buffer[s].name);
//          printf("   Stored symbol %p\n", ccontext->symbols_lookup.buffer[s].address);
//          printf("   Checking symbol %p\n", *ccontext->symbols[i]);
         
         if(ccontext->symbols_lookup.buffer[s].address == *ccontext->symbols[i]) {
            break;
         }
      }
      
       PPG_ASSERT(s < ccontext->symbols_lookup.n_stored);
      
      if(!(s < ccontext->symbols_lookup.n_stored)) {
         printf("Unable to find symbol %s\n", ccontext->symbols_lookup.buffer[s].name);
      }
      
      printf("   *((uintptr_t*)&%s[%lu]) = (uintptr_t)&%s; \\\n", context_name, offset, 
         ccontext->symbols_lookup.buffer[s].name
      );
   }
   
   printf("   \\\n");
   
   printf("   ppg_compression_register_aux_array(%s, %s, sizeof(%s)); \\\n",
          context_name, aux_name, aux_name);
   
   printf("   \\\n");
   
   printf("   ppg_set_context((PPG_Context)%s);\n", context_name);
}
         
void ppg_compression_run(PPG_Compression_Context ccontext,
                        char *name_tag)
{
   PPG_Compression_Context__ *ccontext__  
                  = (PPG_Compression_Context__ *)ccontext;
   
   size_t n_tokens = ppg_compression_allocate_target_buffer(ccontext__);
   
   ppg_compression_copy_context(ccontext__);
   
   // Use stack memory to hold symbol and vptr data
   ///
   void **symbols[2*n_tokens];
   
   ccontext__->symbols = symbols;
   ccontext__->n_symbols = 0;
   ccontext__->n_symbols_space = sizeof(symbols);
   
   void **vptrs[n_tokens];
   ccontext__->vptrs = vptrs;
   ccontext__->n_vptrs = 0;
   ccontext__->n_vptrs_space = sizeof(vptrs);

   ppg_compression_generate_dynamic_assignment_information(ccontext__);
   
   // Important: This must be done last, as it invalidates any pointers
   //            of the token tree. Thus no tree traversal is possible afterwards.
   //
   ppg_compression_convert_all_addresses_to_relative(ccontext__);
   
   // Now the compression context is complete
   
   ppg_compression_write_c_output(ccontext__, name_tag);
}
