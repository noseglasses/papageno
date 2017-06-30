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
 * @file 
 * 
 * @brief Papageno - Turn input devices into magic musical instruments
 * 
 * Define magic melodies that can consist of phrases (notes, chords or note clusters) and 
 * assign actions that are triggered when phrases or a whole melody is completed.
 * 
 * The idea of magic melodies is inspired by magic musical instruments from
 * opera and phantasy fiction. The library itself is named after Papageno one of 
 * the protagonists of Mozard's opera "The Magic Flute". With the help ot a magic glockenspiel is given as a present,
 * Papageno is able to cope with different sorts of obstacles and threads he encounters on his way. 
 * 
 * ## Basic Concept
 * 
 * Imagine a magic piano that does crazy things when certain
 * melodies are played. Melodies can consist of single notes, chords and note clusters
 * that must be played in a well defined order for magic to happen.
 * Although the naming and basic concepts are taken from the world of music, there is not necessarily 
 * any music involved at all.
 * However, we consider the use of such musical terms helpful as they already cover basic ideas that are a good start for abstraction.
 * Whether it is a key on a magic piano or a key on a computer keyboard, keys represent notes that
 * in turn form melodies.
 * 
 * ## Notes
 * 
 * Notes are the basic building blocks of melodies. They can be arranged to form single note
 * lines, chords or clusters.
 * A note can e.g. represent a key on an input device but it can also represent any
 * variable. Activation of a note is queried as a boolean result ot a callback function. Thus even non-boolean 
 * variables can be mapped to a boolean activation state.
 * 
 * ## Chords
 * 
 * Chords are very similar to musical chords. The common property is that all
 * keys that are associated with the chord have to be activated at the same time for the
 * chord to be considered complete.
 * 
 * ## Note Clusters
 * 
 * Clusters are sets of keys that can be activated in arbitrary order. It is only required
 * that every cluster member must have been active at least once for the cluster-phrase
 * to be accepted as complete.
 * 
 * ## Definitions of Melodies
 * 
 * The implementation allows for definitions of single note lines, isolated chords
 * or clusters as well as complex melodies that may consist of arbitrary combinations of
 * the afforementioned types of phrases. Also tap dances are provided that blend in well with the concept of magic melodies.
 * 
 * ## Tap Dances
 * 
 * Tap dances are a concept that emerged in the context of mechanical keyboard firmwares. A single key can trigger
 * different actions depending on how many times the key is consecutively activated. Papageno allows for gaps between tap definitions.
 * It is e.g. possible to trigger an action after 2 and another action after 4 key strokes. 
 * It depends thereby on the specified action flags what is going to happen after three keypresses. See the description of action fall back.
 * 
 * ## Melodies and Layers
 * 
 * Melodies are associated with layers. During key event processing, an integer identifier of the current layer must be passed
 * as a parameter to allow for a selection of matching melodies.
 * Only melodies that are associated with 
 * the current layer or those associated with layers whose layer id is lower than the id of the 
 * current layer are available. As a consequence, the same melody can be associated 
 * with a different action on a higher layer. Moreover, melodies can also be overidden with noops on higher layers.
 * 
 * ## Actions
 * 
 * Actions are defined via callback functions supplied with optional user data that is passed to
 * the callbacks when the action is triggered. The most common behavior is to assign 
 * an action only to the completion of a melody. However, it is also possible to assign 
 * actions to intermediate phrases which are triggered upon phrase completion. To obtain this behavior,
 * add the action flag PPG_Action_Immediate by means of a bitwise or operation to the provieded action flags.
 * 
 * ## Timeout
 * 
 * If a user defined time interval elapses since the last key event occured, timeout is detected.
 * If a melody was already in progress, it is aborted in this case.
 * On timeout the default behavior is that the action associated with the last phrase that completed is triggered, or no action if there was no action associated with the respective phrase. The latter is the typical case for single note 
 * lines where an action is assigned only to the last note.
 *
 * ## Action Fall Back
 * 
 * In case of timeout under certain circumstances, it may be desired to traverse the line of completed phrases back to the point where the next phrase is found that
 * is assigned an action. This behavior is obtained by adding the flag PPG_Action_Fall_Back to all intermediate notes that have not been
 * assigned any action.
 * Action fall back can be a desired behavior for defining tap dances, e.g. when the actions are assigned to three and to five keystrokes of a specific key and the three keystroke action is supposed to happen if only four keystrokes occur before timeout.
 * 
 * ## Action Fall Through
 * 
 * An extension to the fall back behavior is fall through. If a phrase that is assigned an action is also flagged PPG_Action_Fall_Through, 
 * the phrase that completed before is also checked for an action that is then triggered if assigned. It PPG_Action_Fall_Through
 * is also set for this previous phrase also the phrase that completed before is checked, and so on.
 *
 * ## Consumed Key Actions and Timeout/Abort
 * 
 * Papageno temporarily stores all key events that occured from the beginning of a melody. The default behavior is to ignore these key events once a melody 
 * is completed. The respective action is normally wanted as a replacement for the key events. However, it may be desired to actively process these key events even after melody completion. To enable this, a key processor
 * call back can be registered that is passed the the key events and a slot id that identifies at which point during melody processing the callback was called (see PPG_Slots). It is also possible to use the function ppg_flush_stored_key_events to pass all stored key events through a user defined key event processor callback at any time, e.g. from an action callback.
 * 
 * An example application in the context of programmable keyboards would be to define a character sequence, e.g. a sentence that always should automatically be uppercase. By assigning a user callback action to melody completion, it is possible to activate the shift key, then process the key sequence as if it was just typed and then remove the shift key. 
 * 
 * ## Implementation
 * 
 * Papageon efficiently deals with the task of finding matching melodies by using a search tree.
 * Tree nodes thereby represent 
 * phrases, i.e. notes, chords or clusters. Every newly defined melody is
 * automatically incorporated into the search tree of the current context.
 * Once a keystroke happens, the tree search algorithm tries to determine whether
 * the key is associated with a phrase of any melody. This is done by looking for matching phrases on the 
 * current level of the search tree that is associated with the current layer or any layer below. Key events are
 * passed to the child nodes of the current phrase/tree node to let the dedicated 
 * phrase implementation decide if the key is part of the phrase definition.
 * Child phrases signal completion or invalidation. The latter happens 
 * as soon as a key event arrives that is not part of the respective phrase.
 * If one or more suitable child phrases signal completion, the most
 * suitable one is selected with respect to the current layer id. It then replaces the current phrase 
 * and thus the current level of the search tree for the next key event to process. 
 * It may sometimes happen that no child phrases 
 * signal completion or invalidation e.g. if all child phrases are clusters or chords that require several key events until completion.
 * If the most suitable child phrase that just completed is a leaf node of the search tree, 
 * the current magic melody is considered as completed.
 * 
 * ## Papageno Context
 *
 * To allow for a flexible use of Papageno, its implementation is based on a
 * global context pointer. Any function of its programming interface operates 
 * on this pointer. This makes it very easy to use different instances of Papageno
 * in one program and to switch between these instances by activating different
 * contexts. 
 * 
 * Please note that due to this implementation detail, Papageno is not thread safe.
 * Although this is not thread safe, it provides great flexibility.
 * 
 * ## Choice of Language
 * 
 * To make it as flexible as possible, Papageno is written in C99. This makes it 
 * possible to integrate with other projects, a C99 compiler provided. This decision
 * was hard to take as it means to live without all those neat C++ features such as
 * type safefy and templates.
 *
 * In its interior Papageno uses object oriented C to implement a polymorphic phrase class hierarchy.
 * Phrase types that were initially implemented are notes, chords and clusters.
 * The phrase-class family is extensible, so new types of phrases are comparably easy to implement.
 * Just follow the example of the chord and cluster implementations.
 */

/* The following macros influence the build

PAPAGENO_DISABLE_CONTEXT_SWITCHING 
		Disables the capability to switch contexts 

*/

#include <stdbool.h>
#include <inttypes.h>

/** @brief The phrase type
 */
typedef void * PPG_Phrase;

/** @brief Action flags are defined as power of two to be compined using bitwise operations
 */
enum PPG_Action_Flags {
	PPG_Action_Default = 0,
	PPG_Action_Fall_Through = 1 << 0,
	PPG_Action_Fall_Back = 1 << 2,
	PPG_Action_Immediate = 1 << 3
};

/** @brief Key processing slot identifiers
 * 
 * Slot identifiers are used during flushing key events to 
 * distinguish the different reasons for flushing
 */
enum PPG_Slots {
	PPG_On_Abort = 0,
	PPG_On_Timeout,
	PPG_On_Subphrase_Completed,
	PPG_On_Melody_Completed,
	PPG_On_User
};

/** @brief Function type of user callback functions
 * 
 *	 @param slot_id
 *  @param user_data Optional user data.
 */
typedef void (*PPG_User_Callback_Fun)(uint8_t slot_id, void *user_data);

/** @brief The PPG_User_Callback struct groups use callback information
 *  in an object oriented fashion (functor).
 */
typedef struct {
	PPG_User_Callback_Fun func; ///< The callback function
	void *	user_data; ///< Optional user data that is passed to the callback when called
} PPG_User_Callback;

/** @brief Action information
 */
typedef struct {
	PPG_User_Callback user_callback; ///< The user callback that represents that action
	uint8_t flags; ///< A bitfield that codes specific attributes associated with the action
} PPG_Action;

/** @brief The type used as key identifier.
 * 
 * This type is used as an identifier for keys.
 * Please supply a comparison callback via ppg_set_key_id_equal_function.
 * The default behavior is to compare key ids by value.
 */
typedef void * PPG_Key_Id;

/** @brief The type that represents key state.
 */
typedef void * PPG_Key_State;

/** @brief The key activation check callback function type
 */
typedef bool (*PPG_Key_Activation_Check_Fun)(PPG_Key_Id key_id,
															PPG_Key_State state);

/** @brief Definition of a key
 */
typedef struct {
	PPG_Key_Id	key_id; ///< The Key identifier
	PPG_Key_Activation_Check_Fun check_active; ///< A callback that provides checking if a key is activated
} PPG_Key;
	
/** @brief Function type of a callback function that compares user defined key ids
 * 
 * @param key_id1 The first key identifier
 * @param key_id2 The second key identifier
 */
typedef bool (*PPG_Key_Id_Equal_Fun)(PPG_Key_Id key_id1, PPG_Key_Id key_id2);

/** @brief Defines a custom key id comparison function.
 * 
 * The default behavior is to compare key ids by value.
 * 
 * @param fun The function callback to be registered
 */
void ppg_set_key_id_equal_function(PPG_Key_Id_Equal_Fun fun);

/** @brief Time identifier type.
 */
typedef void * PPG_Time;

/** @brief Function type of a callback that computes the current time
 * 
 * @param The pointer to the time value to receive current time
 */
typedef void (*PPG_Time_Fun)(PPG_Time *time);

/** @brief Defines a custom time retreival function
 * 
 * @param fun The function callback to be registered
 * @returns The callback previously active
 */
PPG_Time_Fun ppg_set_time_function(PPG_Time_Fun fun);

/** @brief Function type of a callback that computes differences between time values.
 * 
 * The result is expected as time2 - time1.
 * 
 * @param time1 The first time value
 * @param time2 The second time value
 * @param delta Pointer to the result of the difference computation
 */
typedef void (*PPG_Time_Difference_Fun)(PPG_Time time1, PPG_Time time2, PPG_Time *delta);

/** @brief Defines a custom function to compute time differences
 * 
 * @param fun The function callback to be registered
 * @returns The callback previously active
 */
PPG_Time_Difference_Fun ppg_set_time_difference_function(PPG_Time_Difference_Fun fun);

/** @brief Function type for time comparisons.
 * 
 * The function is expected to return a positive value when time1 is greater time2, 
 * a negative value it time 1 is less time 2 and zero if both are equal.
 * 
 * @param time1 The first time value
 * @param time2 The second time value
 */
typedef int8_t (*PPG_Time_Comparison_Fun)(PPG_Time time1, PPG_Time time2);

/** @brief Defines a custom function to compare time values
 * 
 * @param fun The function callback to be registered
 * @returns The callback previously active
 */
PPG_Time_Comparison_Fun ppg_set_time_comparison_function(PPG_Time_Comparison_Fun fun);

/** @brief Specification of a key event
 */
typedef struct {
	PPG_Key_Id key_id; ///< The key identifier associated with the key that is pressed or released
	PPG_Time time; ///< The time at which the key event occured
	PPG_Key_State state; ///< The state of the key when the event occured
} PPG_Key_Event;

#define PPG_PHRASES(...) \
	sizeof((PPG_Phrase[]) { __VA_ARGS__ })/sizeof(PPG_Phrase), \
	(PPG_Phrase[]) { __VA_ARGS__ }

/** @brief Defines a stand alone magic melody that consists of single notes.
 * 
 * @param layer The layer the melody is associated with
 * @param action The action that is supposed to be carried out if the melody is completed
 * @param keys Keypositions that represent the notes of the single note line.
 * @returns The constructed phrase
 */
PPG_Phrase ppg_single_note_line(
							uint8_t layer, 
							PPG_Action action, 
							uint8_t n_keys,
							PPG_Key keys[]);

/** @brief Defines a stand alone magic chord. 
 * 
 * All members must be activated/pressed simultaneously
 * for a chord to be considered as completed.
 * 
 * @param layer The layer the melody is associated with
 * @param action The action that is supposed to be carried out if the melody is completed
 * @param n_keys The number of keys
 * @param keys A pointer to an array of key definitions.
 * @param n_members The number of chord members that must match the size of the keys array.
 * @returns The constructed phrase
 */
PPG_Phrase ppg_chord(		
							uint8_t layer,
							PPG_Action action,
							uint8_t n_keys,
							PPG_Key keys[]);

/** @brief Defines a stand alone magic note cluster. 
 * 
 * All members must be activated/pressed at 
 * least once for	the cluster to be considered as completed.
 * 
 * @param layer The layer the melody is associated with
 * @param action The action that is supposed to be carried out if the melody is completed
 * @param n_keys The number of keys
 * @param keys A pointer to an array of key definitions.
 * @param n_members The number of cluster members that must match the size of the keys array.
 * @returns The constructed phrase
 */
PPG_Phrase ppg_cluster(		
							uint8_t layer, 
							PPG_Action action,
							uint8_t n_keys, 
							PPG_Key keys[]);

/** @brief A tap specification
 * 
 * Defines an action for a given number of taps
 */
typedef struct {
	PPG_Action action; ///< The action associated with the given number of taps
	uint8_t tap_count; ///< The number of taps necessary to trigger the action
} PPG_Tap_Definition;

/** @brief Auxiliary macro to simplify passing tap definitions
 * 
 * @param ... The array members
 */
#define PPG_TAP_DEFINITIONS(...) \
	sizeof((PPG_Tap_Definition[]){ __VA_ARGS__ })/sizeof(PPG_Tap_Definition), \
	(PPG_Tap_Definition[]){ __VA_ARGS__ }

#define PPG_TAP(TAP_COUNT, ACTION) \
	(PPG_Tap_Definition) { .action = ACTION, .tap_count = TAP_COUNT }

/** @brief Defines a tap dance. 
 * 
 * Tap dances are a sequence of key presses of the same key. With each press 
 * an action can be associated. As an enhancement to the original tap dance idea.
 * It is also possible to fall back to the last action if e.g. an action
 * is defined for three key presses and five key presses. If default_action_flags
 * contains PPG_Action_Fall_Back the action associated with the next lower 
 * amount of key presses is triggered, in this example the action associated with
 * three presses.
 * 
 * @param layer The layer the melody is associated with
 * @param key The key the tap dance is associated with
 * @param default_action_flags The default action that is supposed to be 
 *               associated with the notes of the tap dance.
 * @param n_tap_definitions The number of tap definitions
 * @param tap_definitions A tap definitions array.
 * @returns The constructed phrase
 */
PPG_Phrase ppg_tap_dance(	
							uint8_t layer,
							PPG_Key key,
							uint8_t default_action_flags,
							uint8_t n_tap_definitions,
							PPG_Tap_Definition tap_definitions[]);

/** @brief Generates a note subphrase.
 *
 * Use this function to generate subphrases that are passed to the ppg_melody function
 * to generate complex magic melodies. 
 * 
 * @note Notes that are generated by this function must be passed to ppg_melody
 * 		to be effective
 * @note Use setter functions that operate on phrases to change attributes of the generated phrase 
 * 
 * @param key The key that is represented by the note
 * @returns The constructed subphrase
 */
PPG_Phrase ppg_create_note(PPG_Key key);

/** @brief Generates a magic chord subphrase.
 * 
 * Use this function to generate subphrases that are be passed to the ppg_melody function
 * to generate complex magic melodies. All notes of a chord must have been pressed
 * at the same time time for the magic chord to be completed.
 * 
 * @note Chords that are generated by this function must be passed to ppg_melody
 * 		to be effective
 * @note Use setter functions that operate on phrases to change attributes of the generated phrase 
 * 
 * @param n_keys The number of keys that are associated with the chord 
 * @param keys An array of keys that represent the notes of the magic chord
 * @returns The constructed subphrase
 */
PPG_Phrase ppg_create_chord(	
							uint8_t n_keys,
							PPG_Key keys[]
							);

/** @brief Auxiliary macro to create a chord based on a set of key specifications
 * 
 * @param ... A list of keys
 */
#define PPG_CREATE_CHORD(...) \
	ppg_create_chord(PPG_KEYS(__VA_ARGS__))

/** @brief Generates a magic cluster subphrase.
 *
 * Use this function to generate subphrases that are passed to the ppg_melody function
 * to generate complex magic melodies. Every member of the magic cluster must have been
 * activated at least once for the cluster to be considered as completed.
 * 
 * @note Clusters that are generated by this function must be passed to ppg_melody
 * 		to be effective
 * @note Use setter functions that operate on phrases to change attributes of the generated phrase 
 * 
 * @param n_keys The number of keys that are associated with the cluster 
 * @param keys An array of key ids that represent the notes of the magic cluster
 * @returns The constructed subphrase
 */
PPG_Phrase ppg_create_cluster(
							uint8_t n_keys,
							PPG_Key keys[]);

/** @brief Auxiliary macro to create a cluster based on a set of key specifications
 * 
 * @param ... A list of keys
 */
#define PPG_CREATE_CLUSTER(...) \
	ppg_create_cluster(PPG_KEYS(__VA_ARGS__))
	
/** @brief Auxiliary macro to simplify passing key arrays to functions such as
 * ppg_cluster or ppg_chord
 * 
 * @param ... The array members
 */
#define PPG_KEYS(...) \
	sizeof((PPG_Key[]){ __VA_ARGS__ })/sizeof(PPG_Key), \
	(PPG_Key[]){ __VA_ARGS__ }

/** @brief Defines a magic melody that consists of subphrases
 * 
 * @note Use setter functions that operate on phrases to change attributes of the phrases
 *       that make up the magic melody. This is e.g. required to assign an action to a phrase,
 * 		or to modify fall through behavior.
 * 
 * @param layer The layer the melody is associated with
 * @param n_phrases The number of phrases combined to form the melody
 * @param phrases The phrases that make up the melody
 * @returns The constructed phrase
 */
PPG_Phrase ppg_melody(		
							uint8_t layer,
							uint8_t n_phrases,
							PPG_Phrase phrases[]);

/** @brief Assigns an action to a subphrase.
 * 
 * @param phrase The phrase to be modified
 * @param action The action definition to associate with the subphrase
 * @returns Returns the value of parameter phrase to allow for setter-chaining
 */
PPG_Phrase ppg_phrase_set_action(
									PPG_Phrase phrase,
									PPG_Action action);

/** @brief Returns the action that is associated with a subphrase
 * 
 * @param phrase The subphrase whose action is supposed to be retreived
 * @returns The associated action
 */
PPG_Action ppg_phrase_get_action(PPG_Phrase phrase);

/** @brief Assigns action flags to a subphrase. 
 * 
 * To modify the current state of 
 * the action flags of a subphrase, use the function ppg_get_action_flags, 
 * modify its return value by means of applying bit wise operations and pass it 
 * to ppg_set_action_flags.
 * 
 * @param phrase The subphrase to be modified
 * @param action_flags The new value of action flags
 * @returns Returns the value of parameter phrase to allow for setter-chaining
 */
PPG_Phrase ppg_phrase_set_action_flags(
									PPG_Phrase phrase,
									uint8_t action_flags);

/** @brief Returns the action flags associated with a subphrase
 * 
 * @param phrase The subphrase whose action flags are supposed to be retreived
 * @returns The current value of the action flags associated with the subphrase.
 */
uint8_t ppg_phrase_get_action_flags(PPG_Phrase phrase);

/** @brief Defines a key that aborts current melody processing. 
 * 
 * If melody processing is
 * aborted the actions of subphrases that already completed are triggered.
 * 
 * @note By default there is no abort key defined
 * 
 * @param key The abort key
 * @returns The abort key id that was previously set
 */
PPG_Key ppg_set_abort_key(PPG_Key key);

/** @brief Retreives the current abort key. 
 * 
 * See ppg_set_abort_key for further information.
 */
PPG_Key ppg_get_abort_key(void);

/** @brief Aborts processing of the current magic melody
 */
void ppg_abort_magic_melody(void);

/** @brief Defines whether actions are supposed to be processed along the melody
 * chain, based on the last phrase completed. 
 * 
 * The default setting
 * is to process no action in case of an abortion of melody processing.
 * 
 * @param state The new value to be set
 * 
 * @returns The previous setting
 */
bool ppg_set_process_actions_if_aborted(bool state);

/** @brief Retreives the current settings of the process action if aborted.
 * 
 * See the documentation of ppg_set_process_actions_if_aborted
 * 
 * @returns The current boolean value
 */
bool ppg_get_process_actions_if_aborted(void);

/** @brief Set the current timeout for melody processing. 
 * 
 * If no key events are encountered
 * within the timeout interval after the last key event, melody processing is 
 * aborted and actions are processed recursively starting with the last phrase completed.
 * 
 * @param timeout The timeout time value
 * @returns The previous setting
 */
PPG_Time ppg_set_timeout(PPG_Time timeout);

/** @brief Returns the current timeout value
 * 
 * @returns The current timeout value
 */
PPG_Time ppg_get_timeout(void);

/** @brief A callback function that is used during processing of stored key events
 * 
 * @param key_event A pointer to a PPG_Key_Event struct that provides information 
 * 						about the key event
 * @param slot_id The slot identifier informs about the slot that triggered key event processing
 * @param user_data Optional user data
 * @returns A boolean value that decides about whether key event processing is continued (true) or aborted (false).
 */
typedef bool (*PPG_Key_Event_Processor_Fun)(PPG_Key_Event *key_event,
														 uint8_t slot_id, 
														 void *user_data);

/** @brief Defines the default key processor callback. 
 * 
 * Some functions, e.g. ppg_flush_stored_key_events
 * can be supplied with a custom key processor. If non is specified the default one is used
 * if defined.
 * 
 * @param fun The default key processor callback
 * @returns The callback that was active before resetting
 */
PPG_Key_Event_Processor_Fun ppg_set_default_key_processor(PPG_Key_Event_Processor_Fun fun);

/** @brief Call this function to actively flush any key events that occured since the last flush.
 * 
 * This function is called internally when melodies complete, on abort and on timeout.
 * 
 * @param slot_id The slot where this method was called. Pass PPG_On_User if you call this 
 * 					method from user code.
 * @param key_processor A custom key processor callback. If NULL the default processor registered by
 *                ppg_set_default_key_processor is used.
 * @param user_data Optional user data is passed on to the key processor callback
 */
void ppg_flush_stored_key_events(
								uint8_t slot_id, 
								PPG_Key_Event_Processor_Fun key_processor,
								void *user_data);

/** @brief Use this function to temporarily disable/enable magic melody processing. 
 * 
 * Processing is enabled by default.
 * 
 * @param state The new state. Enables if true, disables if false.
 * @returns The previous state
 */
bool ppg_set_enabled(bool state);

/** @brief Initialize Papageno
 */
void ppg_init(void);

/** @brief Finalizes Papageno, i.e. clears all magic melodies and frees all allocated memory.
 * 
 * Please not that this operation only operates on the current context. It you have created
 * several contexts, set and finalize them in a loop.
 */
void ppg_finalize(void);

/** @brief Resets papageno to initial state
 * 
 * This creates and initializes a new context. Other contexts are not affected. 
 * The currently active context is finalized before the reset operation takes place.
 */
void ppg_reset(void);

/** @brief This is the main entry function for key event processing.
 * 
 * @param key_event A pointer to a key event to process by papageno
 * @param cur_layer The current layer
 * @returns If further key event processing by other key event processors is desired
 */
bool ppg_process_key_event(PPG_Key_Event *key_event,
								  uint8_t cur_layer);

/** @brief The timeout check function
 * 
 * This timeout check function should be called frequently in a loop, e.g. when polling key presses
 * 
 * @returns True if timeout happend, i.e. the time elapsed since the last keypress event 
 * 			exceeded the user defined timeout threshold.
 */
bool ppg_check_timeout(void);

/** @brief Creates a new papageno context
 * 
 * @returns The newly created context
 */
void* ppg_create_context(void);

/** @brief Destroys a papageno context
 * 
 * Make sure to unset a context before 
 * destroying it.
 * 
 * @param context The context to destroy
 */
void ppg_destroy_context(void *context);

/** @brief Sets a new current context
 * 
 * @param context The context to be activated
 * @returns The previously active context
 */
void* ppg_set_context(void *context);

/** @brief Retreives the current context
 * 
 * @returns The current context
 */
void* ppg_get_current_context(void);

/** Use this macro to determine the number of key ids
 * of an array of key ids
 */
#define PPG_NUM_KEYS(S) \
	(sizeof(S)/sizeof(PPG_Key))
	
/** @brief Use this macro to simplify specification of action callbacks
 * @param FUNC The callback function pointer
 * @param USER_DATA A pointer to user data or NULL if none is required
 */
#define PPG_ACTION_USER_CALLBACK(FUNC, USER_DATA) \
	(PPG_Action) { \
		.flags = PPG_Action_Default, \
		.user_callback = (PPG_User_Callback) { \
			.func = (PPG_User_Callback_Fun)FUNC, \
			.user_data = USER_DATA \
		} \
	}
	
/** @brief Use this macro to specify an action that does nothing
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
