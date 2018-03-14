/*
papageno_start

action: Pattern_1
action: Pattern_2
action: Pattern_3
action: Single_Note_Line_1
action: 3_Taps
action: 5_Taps

input: a = $'a'$
input: b = $'b'$
input: c = $'c'$
input: d = $'d'$
input: m = $'m'$
input: n = $'n'$
input: r = $'r'$

|a| -> |b| -> |c| : Pattern_1
|a| -> |a| -> |c| : Pattern_2
|a| -> |b| -> |d| : Pattern_3
|r| -> |n| -> |m| : Single_Note_Line_1
|a|*5 : 3_Taps@3, 5_Taps@5

papageno_end
*/
