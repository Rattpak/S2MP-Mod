# S2MP-Mod | Call of Duty: WWII Client Modification

Discord Server: https://discord.gg/Wbb5YMZrHG


## Features
- Internal Developer Console
- External Developer Console
- Reimplemented Stripped Commands / DVars
- Restored hundreds of Dvars (See DvarInterface.cpp)
- Error String Mapping
- Some asset dumping

## Reimplemented Commands / DVars
- `noclip`
- `god`
- `r_fog`
- `cg_drawgun`
- `map_restart`
- `fast_restart`

## Custom Commands
- `luidbg` - Shows LUI debug info
- `entdbg` - Shows g_spawn entity debug info
- `intcondbg` - Shows internal console debugger
- `listcmd` - Lists all commands loaded in engine

## Custom DVARs
- `cg_drawlui` \<1 or 0\> - Enable drawing of LUI elements
- `cg_hudblood` \<1 or 0\> - Enable drawing of on-screen damage blood
- `g_dumpLui` \<1 or 0\> - Dump LUI files on map load
- `g_dumpStringTables` \<1 or 0\> - Dump StringTables when they are loaded
- `g_dumpRawfiles` \<1 or 0\> - Dump RawFiles when they are loaded
- `printWorldInfo` \<1 or 0\> - Prints GfxWorld build info on load
  
## Notes
- Extract the contents into the root folder of your WW2 installation (steam only). Run the launcher and choose Multiplayer or Zombies.

## build
- clone the repository and run the generate.bat to clone the dependencies and use the s2mp-mod.sln to open it in vs2022 and build.

## Disclaimer
This project is for educational and personal use only. I am not responsible for any bans, penalties, or any other consequences that may result from using this client. Use it at your own risk.
