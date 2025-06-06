# Examples

This directory contains example DBPF files defining ordinance exemplars.    
For instructions on creating new ordinances using these files as a template, see [Tutorial for Creating New Ordinances](../docs/Tutorial_For_Creating_New_Ordinances.md).

- [CityLotteryOrdinance.dat](#citylotteryordinance.dat)
  - [Cost](#cost)
  - [Effects](#effects)
  - [Requirements](#requirements)
- [HumaneSocietyOrdinance.dat](#humanesocietyordinance.dat)
  - [Cost](#cost-1)
  - [Effects](#effects-1)
  - [Requirements](#requirements-1)
- [JewelryTax.dat](#jewelrytax.dat)
  - [Cost](#cost-2)
  - [Effects](#effects-2)
  - [Requirements](#requirements-2)
- [SpaceExplorationProgramOrdinance.dat](#spaceexplorationprogramordinance.dat)
  - [Cost](#cost-3)
  - [Effects](#effects-3)
  - [Requirements](#requirements-3)
- [LuaFunctionOrdinance.dat](#luafunctionordinance.dat)
  - [Cost](#cost-4)
  - [Effects](#effects-4)
  - [Requirements](#requirements-4)

## CityLotteryOrdinance.dat

This DAT file defines a City Lottery income ordinance.

### Cost

Enactment: § -10    
Retracment: § -10    
Monthly: § 500 + (&lt;R§ population&gt; x 0.05) + (&lt;R§§ population&gt; x 0.03) + (&lt;R§§§ population&gt; x 0.01)

### Effects

Crime: +10%    
Cs$ demand: +1%    
Mayor Rating: +5    
School EQ Boost: +2%

### Requirements

None.

## HumaneSocietyOrdinance.dat

This DAT file defines a Humane Society expense ordinance.

### Cost

Enactment: § -10    
Retracment: § -10    
Monthly: § -50 + (&lt;hospital count&gt; x -5)

### Effects

Crime: -5%    
Mayor Rating: +5

### Requirements

At least one hospital building.

## JewelryTax.dat

This DAT file defines a Jewelry Tax income ordinance.

### Cost

Enactment: § 0    
Retracment: § 0    
Monthly: § 20 + (&lt;R§§§ population&gt; x 0.1)

### Effects

Crime: +2%    
R$$$ demand: -5%

### Requirements

None.

## SpaceExplorationProgramOrdinance.dat

This DAT file defines a Space Exploration Program expense ordinance.

### Cost

Enactment: § -10    
Retracment: § -10    
Monthly: § -100 + (&lt;IHT population&gt; x -0.01)

### Effects

IM demand: +0.33%    
IHT demand: +0.83%

### Requirements

Minimum IHT population of 1500.

## LuaFunctionOrdinance.dat

This DAT file tests the Lua-based availability condition (property id 0x6B23D830) and monthly income (0x6B23D930) properties.
The Lua functions are defined in TGI 0xCA63E2A3, 0x4A5E8EF6, 0x179B832B, a copy of that file is also located [here](lua_functions.lua).
The Lua script is checked in separately from the DBPF file to take advantage of GitHub's syntax highlighting.

### Cost

Enactment: § -10    
Retracment: § -10    
Monthly: § -50 + (&lt;RCI population&gt; x -0.001)

### Effects

None.

### Requirements

None.

