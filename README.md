## DOS patcher for SCUMM engine games based on ScummTr

ScummTr is the "collective name" of Thomas Combeleran's fan translation tools for most [SCUMM engine](https://en.wikipedia.org/wiki/SCUMM) games.


### Sam and Max Hit the Road CD-ROM patch

The patch removes annoying "Heap Status: Unplayable" message which interpreter v6.5.0 shows every time it is started with CD-ROM version of the game.

Copy `PATCHER.EXE` to the directory containing _"downgraded"_ game and run: 
```PATCHER -g samnmaxalt``` 
or run 
```PATCHER -p X:\PATH-TO-GAME -g samnmaxalt``` 

_Note:_ DOS long filenames support is required. In FreeDOS this can be activated e.g. by loading `DOSLFN`.

#### Sound driver patch

`WDRPATCH.BAT` is provided which _"fixes"_ issues that Sound Blaster drivers from v6.5.0 interpreter have with CD-ROM version of the game,
by forcing specific sampling rate. Patch requires `FPATCH.COM`. It has only been tested with game binaries listed as "German"
under "LucasArts Demo Files" section of [International House of Mojo](ttps://mixnmojo.com).


### Building

Building directly in DOS via DJGPP is supported. 
_Note:_ DOS long filenames support is required. In FreeDOS this can be activated e.g. by loading `DOSLFN`.


### Disclaimer

This project is not affiliated in any way with LucasArts Entertainment Company, or LucasFilm Ltd.

