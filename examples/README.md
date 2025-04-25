# Examples

This directory contains a DBPF file I used for testing and the Lua condition script.
The Lua script is checked in separately from the DBPF file to take advantage of GitHub's syntax highlighting.

## Ordinance - Exemplar test 1

This exemplar uses the TGI 0x6534284A, 0xFD01A30F, 0xB5BAC3F9.     
It tests the Monthly Income Factor property and the LTEXT key loading.

## Ordinance - Exemplar test 2

This exemplar uses the TGI 0x6534284A, 0xFD01A30F, 0xB5BAC3FA.     
It tests the Year Available (property id 0x28EBBC35), Police Station Count Availability Condition (property id 0x6B23D823),
and Police Station Count Income Factor (property id 0x6B23D923) properties.

## Ordinance - Exemplar test 3

This exemplar uses the TGI 0x6534284A, 0xFD01A30F, 0xB5BAC3FB.     
It tests the  R§ Population Availability Condition (property id 0x6B23D800), and R§ Population Income Factor (property id 0x6B23D900) properties.

## Ordinance - Exemplar test 4

This exemplar uses the TGI 0x6534284A, 0xFD01A30F, 0xB5BAC3FC.     
It tests the Lua-based availability contention (property id 0x6B23D830) and monthly income (0x6B23D930) properties.

The Lua functions are defined in TGI 0xCA63E2A3, 0x4A5E8EF6, 0x179B832B, a copy of that file is also located [here](lua_functioons.lua).

