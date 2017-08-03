![Logo](logos/papageno_logo_240.png)

Papageno
========

[![Build Status](https://travis-ci.org/noseglasses/papageno.png?branch=master)](https://travis-ci.org/noseglasses/papageno)

[Current Testing Code Coverage](https://noseglasses.github.io/papageno/coverage/index.html)

An Advanced Pattern Matching Library
------------------------------------

Define patterns consisting of tokens (notes, chords or note clusters) and assign actions that are triggered when tokens or whole patterns match.

The idea of this special type of pattern matching is inspired by magic musical instruments as they appear in opera and fantasy fiction. Such instruments let all sorts of magic happen when certain melodies are played.

This is why the library is named after Papageno one of the protagonists of Mozart's opera [The Magic Flute](https://en.wikipedia.org/wiki/The_Magic_Flute). A magic glockenspiel that Papageno was given as a present, helps him to deal with trouble and mischief he encounters on his way through the acts.

Possible Fields of Applications
-------------------------------

-	input devices such as programmable keyboards, mouses, etc. (trigger actions through predefined sequences of keystrokes)
-	musical instruments supporting [Midi](https://en.wikipedia.org/wiki/MIDI) (e.g. toggle a specific sound effect once a specific melody was played)
-	computer games (trigger [special moves](https://en.wikipedia.org/wiki/Fighting_game#Special_attacks) of characters after specific sequences of controller input)
-	general multiple-input environments (trigger actions when specific events occur in a predefined order)
-	...

Do you remember the game [Simon](https://en.wikipedia.org/wiki/Simon_%28game%29) with four colored buttons. It played a tune that the player had to repeat. In the original version the tune consists of a series of four different single notes.

Why not intermix some chords to spice it a little? Papageno could easily do it.

Build requirements
------------------

The following tools are mandatory to build Papageno.

-	a [C99](https://en.wikipedia.org/wiki/C99)-compatible C compiler
-	[CMake](https://cmake.org) as build system

Some tools are optional and only required for special purposes.

-	[Doxygen](http://www.stack.nl/~dimitri/doxygen/) to create a documentation of the programming API (optional)
-	[Valgrind](http://valgrind.org/) to run the testbench using a memory debugger

How to Build
------------

```sh
# Clone the Papageno git repository
#
git clone https://github.com/noseglasses/papageno.git papageno.git

cd papageno.git

# Prefer out-of-source builds
#
mkdir -p build/release
cd build/release

# Configure the build system
#
cmake ../..

# Build
#
make
```

The following steps are optional.

```sh
# Run the testbench
#
ctest
```

For more information about how to build Papageno including cross-builds, see [here](doc/building.md).

Contributing
------------

If you want to contribute to Papageno, please start with reading our [general informations for contributers](CONTRIBUTING.md).

Examples
--------

Please see the test bench that resides in the `testing/char_strings` directory of Papageno's code repository for a code example.

This test bench tests Papageno's features by using characters to represent inputs just as they are used in the examples below. Some C experience provided it might be quite easy to adapt some of the tests to other applications.

Introduction
============

Use of Musical Terminology
--------------------------

Although the basic concept is inspired by the world of music, there is not necessarily any music involved at all. However, we consider the use of some musical terms advantageous helpful as they typically cover basic ideas that do not need to be re-explained and also are a good start for abstraction.

Basic Concepts
--------------

A musical melody is  
> a linear succession of musical tones that the listener perceives as a single entity > -- <cite>[Wikipedia](https://en.wikipedia.org/wiki/Melody)</cite>

Melodies or phrases are well defined recipes to create sound and may consist of single notes, chords and possibly note clusters.

We adopt these concepts and extend them to our needs. We take the concepts of melodies or phrases as a basis for the definition of the more general concept of patterns.

Notes, chords and clusters are tokens that may be chained to form patterns. The pitch of notes in a musical melody or phrase is hereby replaced by the boolean state of abstract variables that we call inputs.

Depending on the application context, the boolean states of inputs can represent the activation of switches of input devices, e.g. keyboards, sensor data that exceeds a defined threshold or general any variables that may be mapped to boolean state.

Notes represent the activation and/or deactivation of inputs. Chords and clusters are used to group activations and deactivations of inputs.

By defining a melody, or in general a pattern, we define an expected order of activations and deactivations of inputs. Papageno's task is to scan a continuous series of events, that represent activations or deactivations of inputs for the occurrence of user-defined patterns.

If a pattern is detected, a user-defined action is triggered and the scan is continued.

That's it.

Distinctive Features
--------------------

There already exist some widely used approaches to pattern matching, [regular expressions (regex)](https://en.wikipedia.org/wiki/Regular_expression) as the most prominent. We certainly do not want to compete with these approaches, neither do we want to re-invent the wheel.

As we did not find a suitable pattern matching approach that solves the problem of pattern matching in sequences of stated transitions of boolean variables, we implemented our own. If we overlooked an suitable existing approach, please [let us know](shinynoseglasses@gmail.com).

The question about the differences of Papageno's approach against regex is, nevertheless, justified.

-	Papageno is based on stateful boolean inputs instead of characters.
-	Actions can be assigned and triggered when specific tokens or patterns match.
-	Regex in general do not provide a similar mechanism to trigger actions as its task is to return matching sub-strings.
-	Papageno allows several inputs to be active at the same time (cf. chords). The concept of character strings does not allow several characters to share a common spot within a word.
-	There is no simple way to specify a regex pattern that defines that multiple characters can occur in arbitrary order, every one at least once (clusters).

Every one of the reasons stated above prevents using regular expression to solve the given task.

That's why Papageno is here.

Aknowledgements
---------------

Papageno uses Boost Preprocessor to simplify pattern specification.

Working with Patterns
=====================

Patterns are defined through Papageno's programming interface. Before you confront yourself with programming patterns it is important that you understand how Papageno patterns work.

Example Syntax
--------------

In the following examples we will use different types of brackets to distinguish between types of tokens. We will use alphabetic characters to denote the inputs that are affected by tokens.

The definition of a pattern looks as follows. It will soon be explained in detail.

```
(A) -> [B, C]
```

And this is how a event series is written.

```
A B a C c
```

Please note the absence of brackets and the use of upper- and lower-case letters in the event series.

Inputs
------

Inputs are considered as boolean variables that can change state (true/false). Papageno considers an input either as active (true) or inactive (false). Changes between the states of inputs are passed to Papageno in form of an event that provides information about the state transition. By default Papageno can deal with 256 different inputs. If more inputs are required, the library must be compiled with an increased value of the pre-processor macro `PPG_MAX_INPUTS`.

Papageno uses integer identifiers for inputs. Make sure to use the function `ppg_global_set_number_of_inputs` to define the number of inputs before you start pattern matching.

Event Series
------------

As mentioned before, events represent either an activation or a deactivation of an input. In the following examples we will use alphabetic letters to denote inputs. An upper case character means that an input is activated, a lower case character means the deactivation of an input.

Let's start with a simple example. Imagine e.g. three inputs A, B and C. The following could be a possible event series that toggles the state of all three inputs.

```
A B a C c b
```

In Detail this means: 1. Inputs A and B are activated, 2. Input A is deactivated, 3. Input C is activated and deactivated, 4. Input B is deactivated.

It is important to note that before and input can be deactivated, it must have been activated beforehand. Two consecutive activations or deactivations of inputs are illegal, even if other events are intermixed, e.g.

```
A B b C a c   # Legal

A B C B a b c # Illegal because B is activated twice

B A C a b a c # Illegal because A is deactivated twice
```

Patterns
--------

Patterns may consist of arbitrary combinations of tokens.

Tokens
------

A token can represent an activation/deactivation of one or more inputs.

### Notes

Notes are the most simple building blocks of patterns. They can e.g. be arranged to form single note lines. One note can thereby represent any input. By default, e.g. when used in single note lines, notes expect only the activation of an input to match. Deactivations of an input for that a previous activation had been registered are silently ignored and consumed if the overall pattern matches.

An immediate corresponding deactivation is only required if tokens are marked as pedantic.

Notes may also require an explicit activation or deactivation of an input. This can be forced by supplying a parameter `flag` to the `ppg_note_create` function.

The following is a note that consumes the activation (and deactivation, see above) of an input and requires the activation of an input to match.

```
(A)
```

A note that explicitly requires the activation of an input is written as follows.

```
(A
```

Notes can also explicitly require the deactivation of an input to match.

```
A)
```

### Chords

The concept of Chords is quite similar to that of musical chords. They share the common property that all associated inputs have to be activated simultaneously for the chord to be considered a match.

In the following examples a chord is symbolized by the set of characters that represents the associated inputs written in square brackets.

The order of activation of associated inputs of a chord is arbitrary.

No activations of other inputs than those associated with the chord must be intermixed to allow for a match.

What follows is a simple chord. It requires inputs A, B and C to be simultaneously active to match.

```
[A, B, C]
```

### Note Clusters

Clusters are sets of inputs that may be activated in arbitrary order. It is only required that every cluster member must have been active at least once for the cluster-token to be matched. This is less strict than the requirements of a chord where simultaneous activations are necessary.

In the following examples a note cluster is symbolized by the set of characters that represents the associated inputs written in square brackets.

The order of activation of associated inputs of a note cluster is arbitrary.

No activations of other inputs than those associated with the cluster must be intermixed to allow for a match.

The following is a note cluster. It requires inputs A, B and C to be activated in arbitrary order to match.

```
{A, B, C}
```

Defining Patterns
-----------------

Patterns are token sequences that may consist of notes, chords and note clusters.

The following is a simple token sequence where a note `(C)` is followed by the chord `[A, B]` that is followed by the cluster `{B, A}`.

```
(C) -> [A, B] -> {B, A}
```

It would be matched e.g. by the following event seriess.

```
C A B b B a A c
C A B b a B A c
```

Unique Matches
--------------

As we have seen, token sequences do not necessarily have unique matches. A single note line

```
(A) -> (B) -> (C)
```

can e.g. have the following matches.

```
A B C b a c
A B C b c a
```

And there are still more combinations possible that also match.

If we want to enforce unique matches of single note lines, we have to define the order of activations and deactivations of inputs explicitly.

The token sequence

```
(A (B A) B) (C C)
```

e.g., has the unique match

```
A B a b C c
```

Pedantic Tokens
---------------

By default, tokens match if all related inputs are activated as required by the respective token. However, tokens can also be marked as pedantic. If so, every token requires all related inputs first to be activated and then immediately deactivated.

In pedantic mode, the pattern

```
(C) -> [A, B] -> {B, A}
```

would require the event series

```
C c    A B a b   B b A a
```

for an (non-unique) overall match.

Please note that even in pedantic mode there is no unique token sequence that leads to a match. This is because the deactivations of inputs that are associated with chords and note clusters are accepted in arbitrary order.

The following event series are thus equivalent and all lead to an overall match of the pattern defined above.

```
C c    A B a b    B b A a
C c    A B b a    B b A a
C c    A B b a    B A b a
C c    A B b a    B A a b
```

Tap Dances
----------

Tap dances are a concept that emerged in the context of mechanical keyboard firmwares. A single input can trigger different actions depending on how many times the input is consecutively activated.

Papageno allows for gaps between tap definitions. It is e.g. possible to trigger an action after 2 consecutive activations and another action after 4 consecutive activations of an input. What is going to happen after three consecutive activations hereby depends on the specified action flags (please see the description of action fallback).

Patterns and Layers
-------------------

Papageno provides a layer system. When defined, patterns must be associated with layers. During event processing, the current layer affects the range of patterns that are considered when looking for a match.

Only patterns that are associated with the current layer or layers whose layer id is lower than that of the current layer are considered.

As a consequence, the same pattern can be associated with a different action on a higher layer. The same mechanism allows patterns to be overridden emptily on higher layers.

Actions
-------

Actions are in general associated with tokens rather than patterns. Whenever a pattern matches, the action that is associated with the final token is triggered. Thus the action of the final token can be interpreted as the action associated with the overall pattern.

Actions are defined as callback functions supplied with optional user data that is passed to the callbacks when the action is triggered.

Timeout
-------

If a user defined time interval elapses after the last event was registered, a timeout exception occurs. If pattern matching is in progress at this point, it is aborted. The default timeout behavior is to try to find a matching token with respect to the events in the queue. This means that any tokens that would require further events are ignored. If no match can be found, the action is triggered that is associated with the last token that matched, or no action if there was no action associated with the respective token.

Action Fallback
---------------

Under certain circumstances, it may be desired to traverse the line of already matched tokens back to the point where a token is found that was assigned an action. This mechanism is called action fallback.

It can be obtained by adding the flag `PPG_Action_Fall_Back` to all intermediate tokens that have not been assigned an action.

Action fallback is e.g. internally applied when processing tap dances, when e.g. actions were assigned to three and to five consecutive activations of a specific input and the action associated with three consecutive activations of the input is supposed to happen even if only four activations occurred before timeout. To achieve this the token that represents the fourth activation is flagged with `PPG_Action_Fall_Back`.

If a single note line

```
(A) -> (B) -> (C)
```

Would match and the notes `(B)` and `(C)` would be flagged with `PPG_Action_Fall_Back`, the actions associated with all three notes would be executed consecutively.

It is important to note that the order of the execution of actions is always along the pattern, i.e. in the above example the action associated with `(A)` would be triggered first, followed by the actions of `(B)` and `(C)`.

Signals
-------

During pattern matching there are certain conditions that might be useful to be handled programmatically. Therefore, Papageno allows to register a signal callback that is called when

-	a pattern matches
-	pattern matching is aborted
-	timeout occurs

Pattern matching can be aborted in two ways:

-	through a function call of the programming API
-	or because the input that is registered as special input for aborting is activated.

Events and Timeout/Abort
------------------------

Papageno temporarily stores all events that occurred from the beginning of current pattern matching. The default behavior is to clear the event queue once a pattern was entirely matched. The respective events are thus consumed by the match.

An example for a scenario where this behavior would be useful is the application of Pasimodo to recognize patterns in keystrokes that are entered on a keyboard. Specific key combinations (patterns) are usually expected to trigger an action rather than cause characters to output. Thus the related keystrokes are meant to be ignored after the action is triggered.

In other scenarios it may be desired to actively process the sequence of cached events even in case of pattern matches. To enable this, the event queue can be traversed by passing a callback function to a call to the function `ppg_event_buffer_iterate`. The callback is then passed every event that was encountered. Events that were considered can be recognized by the flag `PPG_Event_Considered`.

An example application of deliberate flushing of events emerges again from the world of programmable keyboards. One might want to define a character/key sequence that is supposed to be automatically transformed to uppercase. By assigning a user callback action to a single note line, it is possible to first activate the shift key, then process the event/keystroke sequence by calling `ppg_event_buffer_iterate` and then deactivate restore the previous state of the shift key. A character sequence that would be processed this way would appear to the host system as if it had originally been typed uppercase by the operator of the keyboard.

Context Switching
-----------------

Every function of Papageno's API operates on a data set that we call the context. The context stores everything that is related to pattern matching, patterns, settings and the state of the pattern matching engine. Instead of using one global context, multiple contexts can be used and activated as required. This is called context switching.

This flexible approach makes it possible to use different instances of Papageno in one program and to switch between these instances by activating different contexts. Context switching is implemented through a global context pointer.

Please note that due to this implementation detail, Papageno cannot be guaranteed to be thread safe.

The Library
===========

C Application Programming Interface
-----------------------------------

A Doxygen documentation of Papageno's API can be found [here](https://noseglasses.github.io/papageno/doc/doxygen/html/index.html).

Performance
-----------

The best case performance of pattern matching with Papageno scales linearly with both the length of patterns defined and the number of different inputs. However, under certain circumstances, the complexity can degrade to quadratic with respect to pattern length. See [more information about performance](doc/performance.md).

Implementation
--------------

### Search Tree

Papageon efficiently solves the task of finding matching patterns by means of using a search tree. Tokens such as notes, chords or clusters are the nodes of the search tree. Every newly defined pattern is automatically incorporated into the search tree of the currently active context.

Papageno is designed in a way that supports on-the-fly detection of patterns. This means that a pattern search is carried out or continued whenever a new event occurs. Therefore the pattern matching engine has an interior state.

When a new event occurs it is first added to the end of the event queue. Then, the engine checks if the new event allows for a match of one of the child tokens of the token that matched the last event. If a child nodes matches, it becomes the new current token and the algorithm waits for the next event to occur.

It is also possible that there are multiple candidates for a new current token which is the case when several child tokens match the latest event. In such a case the current token is marked as a furcation. Then a selection logic selects the most suitable child token that is then declared the new current token to be considered as basis for the next event to occur.

When a branch of a furcation does not match, the engine rewinds the search to the last registered furcation node. Such a condition occurs when an event arrives that cannot be matched by any child token of the current token. After rewinding, the search is continued with one of the remaining branches that were not yet tested. This requires a bookkeeping of the event that is associated with the furcation to determine the chain of events that is used for token matching of branches.

If no branch of a furcation matches, the engine rewinds again back to the previous furcation node if there is one and continues as described. If there is no furcation left, no pattern can be matched by the current chain of events stored in the event queue.

In this case the first of the events (the oldest) is removed from the event cache and the pattern matching is started over with the new first event and all the events that follow.

Events that are, thus, removed from the cache are passed through a globally registered event processing callback. This makes it possible for the user to decide what to do with such an event.

In the aforementioned keyboard example, such an event, i.e. a keystroke would be passed to the host system as if it would have been typed by the operator.

for this all to work efficiently, events are stored in a ring buffer.

If the current node/token has several child token that all do not match yet, e.g. if they are all chords or note clusters, the engine waits for the next event to occur.

During the search for suitable child tokens, some tokens are excluded:

-	Invalid tokens, i.e. those that do not match or branches that have been detected as not matching.
-	Tokens that still require further events to match (chords/clusters).
-	Tokens that are associated with a layer whose ID is greater than that of the current layer.

It is possible that after applying these sorting rules there are still several possible candidates, i.e. child tokens of the current token that match an event or the previous series of events. The children of such a furcation node are ordered by applying the following rules.

-	A token with a higher type precedence wins.
-	If two tokens have the same type precedence, the one with the higher assigned layer wins.
-	If several tokens are equal with respect to the rules above, the first candidate is selected.

The precedence order from highest to lowest is chord, note, cluster. This means that a chord

```
[A, B, C]
```

wins against a single note line,

```
(A) -> (B) -> (C)
```

that would in turn beat a cluster

```
{A, B, C}
```

Supported Platforms
-------------------

Papageno supports any platform that comes with a C99 conforming compiler. It is developed on an x86-64 system but also runs well on embedded systems with small memory, such as Atmel boards (Teensy/Arduino). Actually it is a major design criterion to keep the implementation as compact as possible to safe memory on platforms with restricted resources.

Build System
------------

Papageno uses CMake as its build system.

Debugging
---------

In general it is highly recommended to use proper debugging tools when developing Papageno. However, in some situation it might be helpful to fall back to plain old `printf` debugging. This is e.g. necessary on platforms with Atmel-processors where a debugger does not fit into program memory.

The programming API provides a pre-processor macro `PPG_LOG` that works the same as common `printf` but is only active when Papageno is compiled in debug mode.

To compile Papageno in debug mode, add the required CMake directive to the configuration command.

```sh
cmake -DCMAKE_BUILD_TYPE=Debug ../..
```

If you look at Papageno's code, you will find frequent use of abbreviations in the stings that are passed to `PPG_LOG`. This is because those strings consume precious memory.

**Important:** Papageno is designed to be used on platforms that provide only a small amount of program memory. Therefore, keep the number of `PPG_LOG` statements low and comment or remove them if possible, when they are no more necessary.

Documentation
-------------

The programming API is documented using Doxygen. It can be generated as part of the build process by enabling the CMake cache option `PAPAGENO_DOXYGEN`. This requires a Doxygen installation to be available. To build the Doxygen API-Documentation replace the configuration step with the following.

```sh
cmake -DPAPAGENO_DOXYGEN=TRUE ../..
```

Testing
-------

Papageno comes with a test bench that is based on CTest. The test bench is by default enabled but it can also be switched off which results in the test executables being excluded from the set of build targets. This is controlled by the CMake option `PAPAGENO_TESTING_ENABLED`.

To run the test bench, execute CTest after the build step.

```
.
.

# Build
#
make

# Execute the test bench
#
ctest
```

Choice of Language
------------------

To make it as flexible as possible, Papageno is written in C. This enables it to be integrated with other projects, a C99 compiler provided. This was a hard decision as it meant to live without the neat features C++ provides, such as type safety and templates.

To not sacrifice all benefits in its interior Papageno is implemented partially object oriented. Tokens are e.g. implemented as a polymorphic token class hierarchy. The token-class family is extensible. New types of tokens are thus quite simple to implement.

See the chord and cluster implementations for code examples.

Ideas and To Do
---------------

There are many ideas that are still waiting to be implemented. One reason is that the value of some ideas is not completely clear. Thus, some of the features listed below might be implemented when Papageno users regard them useful.

### Token Timeouts

-	[ ] token timeout: Every token has its own timeout. It must match before the timeout elapses.
-	[ ] token time-interval: A token must match before timeout and can only match if a given time already elapsed.

### Enhanced Token Types

-	[ ] Repetition: A pattern is required to be repeated N times to match.
-	[ ] Tremolo: Requires a note to be repeated N times. The same can be achieved by a Tap Dance or a single note line of N individual notes but a dedicated token that does the job would be by far more memory efficient in comparison to N individual nodes.
-	[ ] Random Chord: N random inputs must be active at the same time for a match.
-	[ ] Random Cluster: N different inputs must have been active for a match.
-	[ ] Random Note Sequence: N inputs must have been active (and inactive) for a match.
