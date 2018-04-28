/*
glockenspiel_begin

action: Single_Note_Line_1
action: Chord_1
action: Cluster_1

action: Chord_2
action: Cluster_2

action: Chord_3
action: Cluster_3
   
action: Single_Note_Line_4
action: Cluster_4

input: a = $'a'$
input: b = $'b'$
input: c = $'c'$
input: d = $'d'$
input: e = $'e'$
input: f = $'f'$
input: m = $'m'$
input: n = $'n'$
input: o = $'o'$
input: x = $'x'$
input: y = $'y'$
input: z = $'z'$

|a| -> |b| -> |c| : Single_Note_Line_1
[a, b, c] : Chord_1
{a, b, c} : Cluster_1

[x, y, z] : Chord_2
{x, y, z} : Cluster_2

|d| -> |e| -> |f| : Single_Note_Line_4
{d, e, f} : Cluster_4

[m, n, o] : Chord_3

layer: ppg_cs_layer_1
{m, n, o} : Cluster_3

glockenspiel_end
*/
