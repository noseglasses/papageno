![Logo](logos/papageno_logo_240.png)

Papageno
==============

A pattern matching library
--------------

Turn input devices into magic musical instruments...
Define magic melodies that can consist of phrases (notes, chords or note clusters) and 
assign actions that are triggered when phrases or a whole melody is completed.

The idea of magic melodies is inspired by magic musical instruments from
opera and phantasy fiction. The library itself is named after Papageno one of 
the protagonists of Mozard's opera "The Magic Flute". With the help ot a magic glockenspiel is given as a present,
Papageno is able to cope with different sorts of obstacles and threads he encounters on his way. 

## Basic Concepts

Imagine a magic piano that does crazy things when certain
melodies are played. Melodies can consist of single notes, chords and note clusters
that must be played in a well defined order for magic to happen.
Although the naming and basic concepts are taken from the world of music, there is not necessarily 
any music involved at all.
However, we consider the use of such musical terms helpful as they already cover basic ideas that are a good start for abstraction.
Whether it is a key on a magic piano or a key on a computer keyboard, keys represent notes that
in turn form melodies.

## Possible Fields of Applications

- general multi-input environments (handling of exceptions when specific events occur in a predefined order)
- input devices such as programmable keyboards, mouses, etc. (trigger actions through predefined key-combinations)
- musical instruments supporting Midi (e.g. toggle a specific sound effect once a certain melody has been played)
- computer games (trigger special moves after complicated motion of input devices or controller key combinations)
- ...

## Distinctive Features

There are other approaches to pattern matching, e.g. Regular Expressions. There are several differences
between the approach that Papageno takes and Regular Expressions.

- Papageno is based on events instead of characters.
- Actions can be assigned and triggered when specific tokens or patterns are encountered.
- Events can mean the activation or deactivation of an input.
- several inputs can be active at the same time (e.g. chords). A regex character does not have any state.
- There is no simple way to specify a regex pattern where multiple characters must occur in arbitrary order, every one at least once (clusters).

## Notes

Notes are the basic building blocks of melodies. They can be arranged to form single note
lines, chords or clusters.
A note can e.g. represent a key on an input device but it can also represent any
variable. Activation of a note is queried as a boolean result ot a callback function. Thus even non-boolean 
variables can be mapped to a boolean activation state.

## Chords

Chords are very similar to musical chords. The common property is that all
keys that are associated with the chord have to be activated at the same time for the
chord to be considered complete.

## Note Clusters

Clusters are sets of keys that can be activated in arbitrary order. It is only required
that every cluster member must have been active at least once for the cluster-phrase
to be accepted as complete.

## Definitions of Melodies

The implementation allows for definitions of single note lines, isolated chords
or clusters as well as complex melodies that may consist of arbitrary combinations of
the afforementioned types of phrases. Also tap dances are provided that blend in well with the concept of magic melodies.

## Tap Dances

Tap dances are a concept that emerged in the context of mechanical keyboard firmwares. A single key can trigger
different actions depending on how many times the key is consecutively activated. Papageno allows for gaps between tap definitions.
It is e.g. possible to trigger an action after 2 and another action after 4 key strokes. 
It depends thereby on the specified action flags what is going to happen after three keypresses. See the description of action fall back.

## Melodies and Layers

Melodies are associated with layers. During key event processing, an integer identifier of the current layer must be passed
as a parameter to allow for a selection of matching melodies.
Only melodies that are associated with 
the current layer or those associated with layers whose layer id is lower than the id of the 
current layer are available. As a consequence, the same melody can be associated 
with a different action on a higher layer. Moreover, melodies can also be overidden with noops on higher layers.

## Actions

Actions are defined via callback functions supplied with optional user data that is passed to
the callbacks when the action is triggered. The most common behavior is to assign 
an action only to the completion of a melody. However, it is also possible to assign 
actions to intermediate phrases which are triggered upon phrase completion. To obtain this behavior,
add the action flag PPG_Action_Immediate by means of a bitwise or operation to the provieded action flags.

## Timeout

If a user defined time interval elapses since the last key event occured, timeout is detected.
If a melody was already in progress, it is aborted in this case.
On timeout the default behavior is that the action associated with the last phrase that completed is triggered, or no action if there was no action associated with the respective phrase. The latter is the typical case for single note 
lines where an action is assigned only to the last note.

## Action Fall Back

In case of timeout under certain circumstances, it may be desired to traverse the line of completed phrases back to the point where the next phrase is found that
is assigned an action. This behavior is obtained by adding the flag PPG_Action_Fall_Back to all intermediate notes that have not been
assigned any action.
Action fall back can be a desired behavior for defining tap dances, e.g. when the actions are assigned to three and to five keystrokes of a specific key and the three keystroke action is supposed to happen if only four keystrokes occur before timeout.

## Action Fall Through

An extension to the fall back behavior is fall through. If a phrase that is assigned an action is also flagged PPG_Action_Fall_Through, 
the phrase that completed before is also checked for an action that is then triggered if assigned. It PPG_Action_Fall_Through
is also set for this previous phrase also the phrase that completed before is checked, and so on.

## Consumed Key Actions and Timeout/Abort

Papageno temporarily stores all key events that occured from the beginning of a melody. The default behavior is to ignore these key events once a melody 
is completed. The respective action is normally wanted as a replacement for the key events. However, it may be desired to actively process these key events even after melody completion. To enable this, a key processor
call back can be registered that is passed the the key events and a slot id that identifies at which point during melody processing the callback was called (see PPG_Slots). It is also possible to use the function ppg_flush_stored_key_events to pass all stored key events through a user defined key event processor callback at any time, e.g. from an action callback.

An example application in the context of programmable keyboards would be to define a character sequence, e.g. a sentence that always should automatically be uppercase. By assigning a user callback action to melody completion, it is possible to activate the shift key, then process the key sequence as if it was just typed and then remove the shift key. 

## Implementation

Papageon efficiently deals with the task of finding matching melodies by using a search tree.
Tree nodes thereby represent 
phrases, i.e. notes, chords or clusters. Every newly defined melody is
automatically incorporated into the search tree of the current context.
Once a keystroke happens, the tree search algorithm tries to determine whether
the key is associated with a phrase of any melody. This is done by looking for matching phrases on the 
current level of the search tree that is associated with the current layer or any layer below. Key events are
passed to the child nodes of the current phrase/tree node to let the dedicated 
phrase implementation decide if the key is part of the phrase definition.
Child phrases signal completion or invalidation. The latter happens 
as soon as a key event arrives that is not part of the respective phrase.
If one or more suitable child phrases signal completion, the most
suitable one is selected with respect to the current layer id. It then replaces the current phrase 
and thus the current level of the search tree for the next key event to process. 
It may sometimes happen that no child phrases 
signal completion or invalidation e.g. if all child phrases are clusters or chords that require several key events until completion.
If the most suitable child phrase that just completed is a leaf node of the search tree, 
the current magic melody is considered as completed.

## Papageno Context

To allow for a flexible use of Papageno, its implementation is based on a
global context pointer. Any function of its programming interface operates 
on this pointer. This makes it very easy to use different instances of Papageno
in one program and to switch between these instances by activating different
contexts. 

Please note that due to this implementation detail, Papageno is not thread safe.
Although this is not thread safe, it provides great flexibility.

## Choice of Language

To make it as flexible as possible, Papageno is written in C99. This makes it 
possible to integrate with other projects, a C99 compiler provided. This decision
was hard to take as it means to live without all those neat C++ features such as
type safefy and templates.

In its interior Papageno uses object oriented C to implement a polymorphic phrase class hierarchy.
Phrase types that were initially implemented are notes, chords and clusters.
The phrase-class family is extensible, so new types of phrases are comparably easy to implement.
Just follow the example of the chord and cluster implementations.