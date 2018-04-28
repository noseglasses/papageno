/*
glockenspiel_begin

action: Action_1
action: Action_2
action: Action_3
action: Action_4

input: a = $'a'$
input: b = $'b'$
input: c = $'c'$
input: d = $'d'$

layer: ppg_cs_layer_0

|a| -> |b| -> |c| : Action_1
|a| -> |a| -> |c| : Action_2
|a| -> |b| -> |d| : Action_3

layer: ppg_cs_layer_1
|a| -> |b| -> |c| : Action_4

layer: ppg_cs_layer_2
|a| -> |b| -> |c| % empty override

glockenspiel_end
*/
