![Logo](logos/papageno_logo_240.png)

Papageno
==============

A pattern matching library
--------------

Define patterns consisting of tokens (notes, chords or note clusters) and 
assign actions that are triggered when tokens or a whole pattern are matched.

The idea of this type of pattern matching was inspired by magic musical instruments from
opera and phantasy fiction that trigger all sorts of magic when certain melodies are played.
The library itself is named after Papageno one of 
the protagonists of Mozart's opera "The Magic Flute". With the help of a magic glockenspiel he was given as a present,
Papageno is able to cope with many sorts of obstacles and mischief he encounters on his way. 

# Introduction

## Basic Concepts

Imagine a magic piano that does crazy things when certain
melodies are played. Musical melodies can consist of single notes, chords and note clusters.
We take these basic concepts and extend them a bit. Instead of a definition of a well defined way to created sound, 
we interpret melodies as a well defined pattern of notes. In the present context notes
are generalized to the concept of pattern tokens that form the pattern. A token can conceptually still be seen as a note
but also a chord or a cluster. Abstract inputs replace the original notes and are grouped to define tokens.
Input can depending on the application represent keys on a keyboard or measurements that are assigned a threshold value.

Events represent activation or deactivation of inputs. 
Tokens are considered as matching if they match a given series of events.
It is e.g. necesarry that all inputs that are associted with a chord have been active at the same time for the chord to be matched.
Once a token matches, the series of arriving events is scanned for the next match. This is done until a whole sequence of tokens, a pattern, matches.

## Use of Musical Terminology

Although the concept is inspired by the word of music ane some basic concepts are more or less the same, there is not necessarily 
any music involved at all.
However, we consider the use of some musical terms helpful as they already cover basic ideas that do not need to be explained again or that are a good start for abstraction.

## Possible Fields of Applications

- input devices such as programmable keyboards, mouses, etc. (trigger actions through predefined key-combinations)
- musical instruments supporting Midi (e.g. toggle a specific sound effect once a specific melody has been played)
- computer games (trigger special moves after complicated motion of input devices or controller key combinations)
- general multiple-input environments (trigger actions when specific events occur in a predefined order)
- ...

## Distinctive Features

There are other approaches to pattern matching. We do not have an overview 
over all those approaches. Regular Expressions are probably one 
of the most common types of pattern matching. Therefore, we will point
out the major differences between regex and Papageno.

- Papageno is based on stateful inputs instead of characters.
- Actions can be assigned and triggered when specific tokens or patterns are matched.
- Events trigger the activation or deactivation of inputs.
- Several inputs can be active at the same time (e.g. chords). A regex character does have neither state, nor can characters share a common spot. 
- There is no simple way to specify a regex pattern that defines that multiple characters must occur in arbitrary order, every one at least once (clusters).

# Working with Patterns

## Notes

Notes are the most simple type of a token. They are the basic building blocks of patterns. They can e.g. be arranged to form single note
lines or tap dances.
A note can represent any
input. Activation of a note is queried through the boolean result of a callback function that is associated with the input. Thus, even non-boolean input 
variables can easily be mapped to a boolean activation state.

## Chords

Chords are quite similar to musical chords. They share the common property that all
inputs that are associated with the chord have to be activated at the same time for the
chord to be considered complete.

## Note Clusters

Clusters are sets of inputs that may be activated in arbitrary order. It is only required
that every cluster member must have been active at least once for the cluster-token
to be matched.

## Patterns

Patterns may consist of arbitrary combinations of
the afforementioned types of tokens, notes, chords and clusters.

## Tap Dances

Tap dances are a concept that emerged in the context of mechanical keyboard firmwares. A single input can trigger
different actions depending on how many times the input is consecutively activated. Papageno allows for gaps between tap definitions.
It is e.g. possible to trigger an action after 2 and another action after 4 consecutive activations of an input. 
It depends thereby on the specified action flags what is going to happen after three consecutive activations. See the description of action fall back.
Tap dances can be seen as a set of single note lines that chain the same token several times. Every tap definition hereby represents an individual pattern.

## Patterns and Layers

Papageno supports a layer system.
Patterns are associated with layers.
During event processing, the current layer affects the choice of patterns that can be matched.
Only patterns that are associated with 
the current layer or layers whose layer id is lower than that of the
current layer are accessible. As a consequence, the same pattern can be associated 
with a different action on a higher layer, thus overriding it. Patterns can of course also be overidden with noops on higher layers.

## Actions

Actions are defined as callback functions supplied with optional user data that is passed to
the callbacks when the action is triggered. The most common behavior is to assign 
an action that is triggered only when a pattern entirely matches. However, it is also possible to assign 
actions to intermediate tokens which are triggered upon token match. To obtain this behavior,
add the action flag PPG_Action_Immediate by means of a bitwise or operation to the provieded action flags.

## Timeout

If a user defined time interval elapses after the last event occured, a timeout is detected.
If pattern matching was already in progress, it is aborted when timeout occurs.
The default timeout behavior is to trigger the action that is associated with the last token that matched, or no action if there was no action associated with the respective token. 

## Action Fall Back

Under certain circumstances in case of timeout, it may be desired to traverse the line of already matched tokens back to the point where a token is found that
was assigned an action. This behavior can be obtained by adding the flag PPG_Action_Fall_Back to all intermediate tokens that have not been
assigned any action.
Action fall back is e.g. internally applied when processing tap dances, 
when e.g. actions were assigned to three and to five activations of a specific input and the action associated with three consecutive activations of the input is supposed to happen also if four activations occured before timeout.

## Action Fall Through

Fall through takes the idea of fall back even a bit further.
If a token that is assigned an action is also flagged PPG_Action_Fall_Through, 
the token that matched just before is also checked for an action that is triggered if assigned. It PPG_Action_Fall_Through
is also set for this previous token also its predecessor is checked, and so on.

## Events and Timeout/Abort

Papageno temporarily stores all events that occured from the beginning of current pattern matching. The default behavior is to forget these events once a pattern was completely matched. When Papageno is e.g. used to recognice previously defined key combinations that are entered on a programmable keyboard, the action that is associated with the key combination often triggers configuration changes or outputs special characters. In most cases the character sequence that triggered the action is supposed to be ignored, i.e. not to be passed to the host system.
However, it may be desired to actively process these cached events even after a whole pattern matches. They could e.g. be passed on to another part of a program to interpret them. To enable this, an event processing
callback may be registered that is passed the series of stored events. It is also possible to use the function ppg_event_buffer_flush to pass all currently stored events through a user defined event processing callback at any time, e.g. from an action callback.

An example application of deliberate flushing of events emerges again from the world of programmable keyboards: One might want to define a character/key sequence that is supposed to be automatically output uppercase. By assigning a user callback action to a single note line, it is possible to activate the shift key, then process the key sequence  by calling ppg_event_buffer_flush and then deactivate the shift key. The character sequence would then appear to the host system as if it had originally been typed uppercase. 

## Slots

To distinguish where flushing of cached events occured, a slot id is 
passed to the respective processing callback.

## Context Switching

To allow for a flexible use of Papageno, its implementation supports context switches.
Any function of its programming interface operates 
on the currently active context. This makes it very easy to use different instances of Papageno
in one program and to switch between these instances by activating different
contexts. 
Context switching is implemented through a global context pointer. Please note that due to this implementation detail, Papageno is not quaranteed to be thread safe.

# The Library

## Implementation

### Search Tree

Papageon efficiently solves the task of finding matching patterns by using a search tree.
With this approach nodes of the search tree represent 
tokens, i.e. notes, chords or clusters. Every newly defined pattern is
automatically incorporated into the search tree of the current context.

Papageno is designed in a way that supports on-the-fly detection of patterns.
This means that a pattern search is carried out or continued whenever a 
new event is registered.

There are two possible states of the pattern matching automaton. When a pattern
was detected or detection failed, it falls back to initialize state. Else it
is waiting for the next event to occur.

When a new event occurs it is added to an event cache whose purpose is described
below. After that, the algorithm checks it the new event allows for a
match of one of the child (successor) tokens of the current token, i.e. the 
token that matched the last event to arrive previously. 

If one of the child nodes matches, it becomes the new current token and the algorithm waits for
the next token to arrive. If there were multiple candidates for a new current token,
the current token is marked as current furcation. The marked furcation will be 
required if a branch of the search tree is detected as not matching. Marking
furcations speeds up tree traversal.

If no child tokens are left that could match the current event. The 
the algorithm continues with one of the 
candidates of the most recently marked furcation.

If a furcation has no further child nodes, that were not yet tried for 
matchesr, the search continues with
the second latest furcation.
Thus, if necessary all partially matching parts of the search tree are traversed
when searching for a matching pattern.

If no pattern does match after traversing all possible branches of the search tree, 
the oldest of the cached events is dropped and the pattern matching is resumed
starting from the second oldest cached event. 

Events are cached using a a ring buffer.

If the current token has several candidates that all do not match yet, the algorithm waits
for the next event to arrive and then resumes matching based on the new event 
and all the recent events that are still in the event buffer.

During the search for suitable child tokens to continue event matching, 
the following tokens are excluded:

- invalid tokens (those that do not match)
- tokens that still require further events to match
- tokens with an assigned layers that is greater than the current layer

When after applying this rule several children of a current token match, 
several rules decide about which child token to try first:

- the token with the highest type precedence wins
- if two tokens have same type precedence, the one with the higher assigned layer wins

The precedence order from highest to lowest is thereby note, chord, cluster.

## Supported Platforms

The libray supports any platform that comes with a C99 capable compiler.
It is developed on an x86-64 system but also runs well on embedded 
systems with small memory, such as Arduino or Teensy boards. 
Actually it
is a major design criterion to keep the implementation as compact as possible 
to safe memory on platforms with restricted resources.

## Choice of Language

To make it as flexible as possible, Papageno is written in C. This makes it 
possible to integrate with other projects, a C99 compiler provided. This decision
was hard to take as it means to live without many neat C++ features such as
type safefy and templates.

In its interior Papageno uses object oriented C to implement a polymorphic token class hierarchy.
Token types that were initially implemented are notes, chords and clusters.
The token-class family is extensible. New types of tokens are quite easy to implement.
See the chord and cluster implementations as code example.

## Ideas and TODO

### Token Timeouts

- token timeout: Every token has its own timeout. It must match before the timeout elapses
- token time-interval: A token must match before timeout and can only match if a given time already elapsed

### Enhanced Token Types

- Repetition: Requires a pattern to be repeated N times
- Tremolo: Requires a note to be repeated N times (the same can be achieved by a Tap Dance or a single note line of N individual notes but a token that does the job would be by far more memory efficient)
- Random Chord: N random inputs must be active at the same time
- Random Cluster: N different inputs must have been active
- Random Sequence: N inputs must have been active (and inactive)