/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PAPAGENO_H
#define PAPAGENO_H

/**
 * \brief Papageno - Turns input devices into magic instruments
 *
 * Object oriented C is used to implement a polymorphic phrase class hierarchy.
 * Phrases that were initially implemented are notes, chords and clusters.
 * The phrase-family is extensible, so new types of phrases are easy to implement
 * as polymorphic classes.
 * 
 * All this would have been easier and more concise in C++ but as qmk seems
 * to be mostly written in C, we adhere to this choice of language, although it
 * is hard to live without type safety and all those neat C++ features as templates...
 * 
 * The idea of magic melodies is inspired by magic musical instruments from
 * phantasy fiction. Imagine a magic piano that does crazy things when certain
 * melodies are played. Melodies can consist of single notes, chords and note clusters
 * that must be played it a well defined order for the magic to happen.
 * Clusters are sets of notes that can be played in arbitrary order. It is only necessary
 * that every cluster member must have been played at least once for the cluster-phrase
 * to be accepted.
 * Going over to computer keyboards means that there is no music at all.
 * However, the basic concepts are transferable. A key on the piano 
 * here is a key on the keyboard. So a melody can be a combination of keystrokes.
 * 
 * Actions such as tmk/qmk keycodes can be associated with magic melodies as well as
 * user function calls that are fed with user data.
 * 
 * The implementation allows for simple definition of single note lines, isolated chords
 * or clusters as well as complex melodies that may consist of arbitrary combinations of
 * the afforementioned types of phrases.
 * 
 * A melody is generally associated with a layer and melody recognition works 
 * basically the same as layer selection. Only melodies that are associated with 
 * the current layer or those below are available. This also means that.
 * The same melody can be associated with a different action on a higher layer and
 * melodies can also be overriden with noops on higher layers.
 * 
 * To allow all this to work, magic melodies are independent of keymaps and implemented
 * on top of them. Because of this, phrases consist of keystroke sequences instead of
 * keycode sequences. Every member of a phrase is defined according to the 
 * row/column index of the key in the keyboard matrix.
 * 
 * If a melody completes, the associated action is triggered and
 * all keystrokes that happend to this point are consumed, i.e. not passed through
 * the keymap system. Only if a set of keystrokes does not match any defined melody, 
 * all keystrokes (keydowns/keyups) are passed in the exact order to the keymap processing
 * system as if they just happend, thereby also conserving the temporal order and the
 * time interval between key events.
 * 
 * The implementation is based on a search tree. Tree nodes represent 
 * phrases, i.e. notes, chords or clusters. Every newly defined melody is
 * integrated into the search tree.
 * Once a keystroke happens, the tree search tries to determine whether
 * the key is associated with any melody by determining a matching phrase on the 
 * current level of the search tree. Therefore, keystrokes are
 * passed to the sucessors of the current phrase/tree node to let the 
 * dedicated phrase implementation decide if the key is part of their definition. 
 * Successor phrases signal completion or invalid state. The latter happens 
 * as soon as a keystroke happens that is not part of the respective successor phrase.
 * If one or more suitable successor phrases complete, the most
 * suitable one is selected with respect to the current layer 
 * and replaces the current phrase. I may also happen that no successor phrase 
 * signals completion or invalidation e.g. if all successors are clusters or chords
 * that are not yet copleted.
 * If the most suitable successor that just copleted is a leaf node of the search tree, 
 * the current magic melody is considered as completed and the action 
 * that is associated with the melody is triggered. 
 * 
 * \\TODO	PPG_Action_Fallthrough explain
 *
 */

#include <stdbool.h>
#include <inttypes.h>

#ifdef DEBUG_PAPAGENO
/** Function type of output functions that have the same signature as prinf 
 */
typedef void (*PPG_Printf_Fun)(const char *, ...);

/** 	\brief A function to set a custom printf function for debugging
 * 				output (only available if DEBUG_PAPAGENO is defined)
		\param printf_fun Pointer to the output function
  */
void ppg_set_printf(PPG_Printf_Fun printf_fun);
#endif

typedef void * PPG_Phrase;

/** Action flags are defined as power of two to be compined using bitwise operations
 */
enum PPG_Action_Flags {
	PPG_Action_Undefined = 0,
	PPG_Action_Fallthrough = 1 << 0,
	PPG_Action_Noop_Fallthrough = 1 << 2,
	PPG_Action_Immediate = 1 << 3
};

/** Key flush identifiers are used during flushing key events to 
 * distinguish the different reasons for flushing
 */
enum PPG_Slots {
	PPG_On_Abort = 0,
	PPG_On_Timeout,
	PPG_On_Subphrase_Completed,
	PPG_On_Melody_Completed,
	PPG_On_User
};

/** \brief Function type of user callback functions
 * 
 *	 \param slot_id
 *  \param user_data Optional user data.
 */
typedef void (*PPG_User_Callback_Fun)(uint8_t slot_id, void *user_data);

/** The PPG_User_Callback struct groups use callback information
 *  in an object oriented fashion (functor).
 */
typedef struct {
	PPG_User_Callback_Fun func;
	void *	user_data;
} PPG_User_Callback;

/** Action information
 */
typedef struct {
	PPG_User_Callback user_callback;
	uint8_t flags;
} PPG_Action;

/** The type of key id data. This type is used as an identifier for keys.
 * Please supply a comparison callback via ppg_set_key_id_equal_function.
 * The default behavior is to compare key ids by value.
 */
typedef void * PPG_Key_Id;

/** Function type of a callback function that compares user defined key ids
 * 
 * \param key_id1 The first key identifier
 * \param key_id2 The second key identifier
 */
typedef bool (*PPG_Key_Id_Equal_Fun)(PPG_Key_Id key_id1, PPG_Key_Id key_id2);

/** Defines a custom key id comparison function.
 * The default behavior is to compare key ids by value.
 * 
 * \param fun The function callback to be registered
 */
void ppg_set_key_id_equal_function(PPG_Key_Id_Equal_Fun fun);

/** Time identifier type.
 */
typedef void * PPG_Time;

/** Function type of a callback that computes the current time
 * 
 * \param The pointer to the time value to receive current time
 */
typedef void (*PPG_Time_Fun)(PPG_Time *time);

/** Defines a custom time retreival function
 * 
 * \param fun The function callback to be registered
 * \returns The callback previously active
 */
PPG_Time_Fun ppg_set_time_function(PPG_Time_Fun fun);

/** Function type of a callback that compues differences between time values.
 * The result is expected as time2 - time1.
 * 
 * \param time1 The first time value
 * \param time2 The second time value
 * \param delta Pointer to the result of the difference computation
 */
typedef void (*PPG_Time_Difference_Fun)(PPG_Time time1, PPG_Time time2, PPG_Time *delta);

/** Defines a custom function to compute time differences
 * 
 * \param fun The function callback to be registered
 * \returns The callback previously active
 */
PPG_Time_Difference_Fun ppg_set_time_difference_function(PPG_Time_Difference_Fun fun);

/** Function type of a functionReturns a positive value when time1 is greater time2, 
 * a negative value it time 1 is less time 2 and zero if both are equal.
 * 
 * \param time1 The first time value
 * \param time2 The second time value
 */
typedef int8_t (*PPG_Time_Comparison_Fun)(PPG_Time time1, PPG_Time time2);

/** Defines a custom function to compare time values
 * 
 * \param fun The function callback to be registered
 * \returns The callback previously active
 */
PPG_Time_Comparison_Fun ppg_set_time_comparison_function(PPG_Time_Comparison_Fun fun);

/** Specification of a key event
 * 
 * \param key_id The key identifier associated with the key that is pressed or released
 * \param time The time at which the key event occured
 * \param pressed A boolean value that signals that a key was pressed (true) or released (false)
 */
typedef struct {
	PPG_Key_Id key_id;
	PPG_Time time;
	bool pressed;
} PPG_Key_Event;

/** Defines a stand alone magic melody that consists of single notes.
 * 
 * \param layer The layer the melody is associated with
 * \param action The action that is supposed to be carried out if the melody is completed
 * \param count The number of notes that follow as variadic arguments
 * \param ... Keypositions that represent the notes of the single note line.
 * \return The constructed phrase
 */
PPG_Phrase ppg_single_note_line(
							uint8_t layer, 
							PPG_Action action, 
							int count, 
							...);

/** Defines a stand alone magic chord. All members must be activated/pressed simultaneously
 * for the chord to be considered as completed.
 * 
 * \param layer The layer the melody is associated with
 * \param action The action that is supposed to be carried out if the melody is completed
 * \param key_ids A pointer to an array of key identifiers.
 * \param n_members The number of chord members that must match the size of the key_ids array.
 * \return The constructed phrase
 */
PPG_Phrase ppg_chord(		
							uint8_t layer,
							PPG_Action action,
							PPG_Key_Id *key_ids,
							uint8_t n_members);

/** Defines a stand alone magic note cluster. All members must be activated/pressed at 
 * least once for	the cluster to be considered as completed.
 * 
 * \param layer The layer the melody is associated with
 * \param action The action that is supposed to be carried out if the melody is completed
 * \param key_ids A pointer to an array of key identifiers.
 * \param n_members The number of cluster members that must match the size of the key_ids array.
 * \return The constructed phrase
 */
PPG_Phrase ppg_cluster(		
							uint8_t layer, 
							PPG_Action action, 
							PPG_Key_Id *key_id,
							uint8_t n_members);

/** \brief Defines a tap dance. 
 * 
 * Tap dances are a sequence of key presses of the same key. With each press 
 * an action can be associated. As an enhancement to the original tap dance idea.
 * It is also possible to fall back to the last action if e.g. an action
 * is defined for three key presses and five key presses. If default_action_flags
 * contains PPG_Action_Noop_Fallthrough the action associated with the next lower 
 * amount of key presses is triggered, in this example the action associated with
 * three presses.
 * 
 * \param layer The layer the melody is associated with
 * \param key_id The key the tap dance is associated with
 * \param default_action_flags The default action that is supposed to be 
 *               associated with the notes of the tap dance.
 * \param n_tap_definition_varargs The number of tap definition variadic arguments (this is actually a multitude of the number of tap dance specifications. Please use the macro PPG_N_TAPS 
 * supplied with the number of tap specifications to define correct values.
 * \param ... Pairs of tap counts and action specifications. If you want to define,
 *          tap dance actions for three, five and seven taps, you would specify three pairs.
 * \return The constructed phrase
 */
PPG_Phrase ppg_tap_dance(	
							uint8_t layer,
							PPG_Key_Id key_id,
							uint8_t default_action_flags,
							uint8_t n_tap_definition_varargs,
							...);

/** Auxiliary macro to compute the correct number of variadic arguments for the
 * ppg_tap_dance function. Use it to produce the function parameter n_tap_definition_varargs.
 */
#define PPG_N_TAPS(S) (2*S)

/** Generates a note subphrase that can be passed to the ppg_melody function
 * to generate complex magic melodies. 
 * 
 * \note Notes that are generated by this function must be passed to ppg_melody
 * 		to be effective
 * \note Use setter functions that operate on phrases to change attributes of the generated phrase 
 * 
 * \param key_id The key that is represented by the note
 * \return The constructed subphrase
 */
PPG_Phrase ppg_create_note(PPG_Key_Id key_id);

/** Generates a magic chord subphrase that can be passed to the ppg_melody function
 * to generate complex magic melodies. All notes of a chord must have been pressed
 * at the same time time for the magic chord to be completed.
 * 
 * \note Chords that are generated by this function must be passed to ppg_melody
 * 		to be effective
 * \note Use setter functions that operate on phrases to change attributes of the generated phrase 
 * 
 * \param key_ids An array of key ids that represent the notes of the magic chord
 * \param n_members The number of notes that make up the chord. This value must match the
 * 						number of array members of key_ids
 * \return The constructed subphrase
 */
PPG_Phrase ppg_create_chord(	
							PPG_Key_Id *key_ids,
							uint8_t n_members);

/** Generates a magic cluster subphrase that can be passed to the ppg_melody function
 * to generate complex magic melodies. Every member of the magic cluster must have been
 * activated at least once for the cluster to be considered as completed.
 * 
 * \note Clusters that are generated by this function must be passed to ppg_melody
 * 		to be effective
 * \note Use setter functions that operate on phrases to change attributes of the generated phrase 
 * 
 * \param key_ids An array of key ids that represent the notes of the magic cluster
 * \param n_members The number of notes that make up the cluster. This value must match the
 * 						number of array members of key_ids
 * \return The constructed subphrase
 */
PPG_Phrase ppg_create_cluster(
							PPG_Key_Id *key_id,
							uint8_t n_members);

/** Defines a magic melody that consists of subphrases
 * 
 * \note Use setter functions that operate on phrases to change attributes of the phrases
 *       that make up the magic melody. This is e.g. required to assign an action to a phrase,
 * 		or to modify fall through behavior.
 * 
 * \param layer The layer the melody is associated with
 * \param count The number of subphrases that make up the magic melody
 * \param ... Pointers to phrases that make up the melody.
 * \note The number of variaradic arguments, the phrase pointers must match the value of parameter count
 * \return The constructed phrase
 */
PPG_Phrase ppg_melody(		
							uint8_t layer,  
							int count,
							...);

/** Assigns an action to a subphrase.
 * 
 * \param phrase The phrase to be modified
 * \param action The action definition to associate with the subphrase
 * \return Returns the value of parameter phrase to allow for setter-chaining
 */
PPG_Phrase ppg_phrase_set_action(
									PPG_Phrase phrase,
									PPG_Action action);

/** Returns the action that is associated with a subphrase
 * 
 * \param phrase The subphrase whose action is supposed to be retreived
 * \return The associated action
 */
PPG_Action ppg_phrase_get_action(PPG_Phrase phrase);

/** Assigns the action flags to a subphrase. To modify the current state of 
 * the action flags of a subphrase, use the function ppg_get_action_flags, 
 * modify its return value by means of applying bit wise operations and pass it 
 * to ppg_set_action_flags.
 * \param phrase The subphrase to be modified
 * \param action_flags The new value of action flags
 * \return Returns the value of parameter phrase to allow for setter-chaining
 */
PPG_Phrase ppg_phrase_set_action_flags(
									PPG_Phrase phrase,
									uint8_t action_flags);

/** Returns the action flags associated with a subphrase
 * 
 * \param phrase The subphrase whose action flags are supposed to be retreived
 * \return The current value of the action flags associated with the subphrase.
 */
uint8_t ppg_phrase_get_action_flags(PPG_Phrase phrase);

/** Defines a key that aborts current melody processing. If melody processing is
 * aborted the actions of subphrases that already completed are triggered.
 * 
 * \note By default there is no abort key defined
 * 
 * \param key_id A key id
 * \returns The abort key id that was previously set
 */
PPG_Key_Id ppg_set_abort_key_id(PPG_Key_Id key_id);

/** Retreives the current abort key. See ppg_set_abort_key_id for further information.
 */
PPG_Key_Id ppg_get_abort_key_id(void);

/** Defines whether actions are supposed to be processed along the melody
 * chain, based on the last phrase completed. The default setting
 * is to process no action on abort.
 * 
 * \param state The new value to be set
 * 
 * \returns The previous setting
 */
bool ppg_set_process_actions_if_aborted(bool state);

/** Retreives the current settings of the process action if aborted.
 * See the documentation of ppg_set_process_actions_if_aborted
 * 
 * \returns The current boolean value
 */
bool ppg_get_process_actions_if_aborted(void);

/** Set the current timeout for melody processing. If no key events are registered
 * within the timeout interval after the last key event, melody processing is 
 * aborted and actions are processed recursively starting with the last phrase completed.
 * 
 * \param timeout The timeout time value
 * \returns The previous setting
 */
PPG_Time ppg_set_timeout(PPG_Time timeout);

/** Returns the current timeout value
 * 
 * \returns The current timeout value
 */
PPG_Time ppg_get_timeout(void);

/** A callback function that is used during processing of stored key events
 * 
 * \param key_event A pointer to a PPG_Key_Event struct that provides information 
 * 						about the key event
 * \param slot_id The slot identifier informs about the slot that triggered key event processing
 * \param user_data Optional user data
 * \returns A boolean value that decides about whether key event processing is continued (true) or aborted (false).
 */
typedef bool (*PPG_Key_Event_Processor_Fun)(PPG_Key_Event *key_event,
														 uint8_t slot_id, 
														 void *user_data);

/** Defines the default key processor callback. Some functions, e.g. ppg_flush_stored_key_events
 * can be supplied with a custom key processor. If non is specified the default one is used
 * if defined.
 * 
 * \param fun The default key processor callback
 * \returns The callback that was active before resetting
 */
PPG_Key_Event_Processor_Fun ppg_set_default_key_processor(PPG_Key_Event_Processor_Fun fun);

/** Call this function to deliberately flush any key events that occured since the last flush.
 * This function is called internally when melodies complete, on abort and on timeout.
 * 
 * \param slot_id The slot where this method was called. Pass PPG_On_User if you call this 
 * 					method from user code.
 * \param key_processor A custom key processor callback. If NULL the default processor registered by
 *                ppg_set_default_key_processor is used.
 * \param user_data Optional user data is passed on to the key processor callback
 */
void ppg_flush_stored_key_events(
								uint8_t slot_id, 
								PPG_Key_Event_Processor_Fun key_processor,
								void *user_data);

/** Use this function to temporarily disable/enable magic melody processing. Processing
 * is enabled by default.
 * 
 * \param state The new state. Enables if true, disables if false.
 * \returns The previous state
 */
bool ppg_set_enabled(bool state);

/** Finalizes, i.e. clears all magic melodies and frees all allocated memory
 */
void ppg_finalize(void);

/** Resets papageno to initial state
 */
void ppg_reset(void);

/** This is the main entry function for key event processing.
 * 
 * \param key_event A pointer to a key event to process by papageno
 * \param cur_layer The current layer
 * \returns If further key event processing by other key event processors is desired
 */
bool ppg_process_key_event(PPG_Key_Event *key_event,
								  uint8_t cur_layer);

/** This timeout check function should be called frequently, e.g. when polling key presses
 * 
 * \returns True if timeout happend, i.e. the time elapsed since the last keypress event 
 * 			exceeded the user defined timeout threshold.
 */
bool ppg_check_timeout(void);

/** Use this macro in favor of the sizeof operator to determine the number of key ids
 * of an array of key ids
 */
#define PPG_NUM_KEYS(S) \
	(sizeof(S)/sizeof(PPG_Key_Id))
	
/** Use this macro to simplify specification of action callbacks
 * \param FUNC The callback function pointer
 * \param USER_DATA A pointer to user data or NULL if none is required
 */
#define PPG_ACTION_USER_CALLBACK(FUNC, USER_DATA) \
	(PPG_Action) { \
		.flags = PPG_Action_Undefined, \
		.user_callback = (PPG_User_Callback) { \
			.func = FUNC, \
			.user_data = USER_DATA \
		} \
	}
	
/** Use this macro to specify an action that does nothing
 */
#define PPG_ACTION_NOOP \
	(PPG_Action) { \
		.flags = PPG_Action_None, \
		.user_callback = (PPG_User_Callback) { \
			.func = NULL, \
			.user_data = NULL \
		} \
	}

#endif
