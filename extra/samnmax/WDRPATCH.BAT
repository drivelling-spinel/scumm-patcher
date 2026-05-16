@ECHO OFF
SET EXEFILE=SBPRO.WDR
IF NOT EXIST FPATCH.COM GOTO NOFPAT
IF NOT EXIST %EXEFILE% GOTO NOFILE
ECHO This patch will update multiple game files. 
ECHO Please always keep backup copy of the original files!
ECHO To abort press Ctrl+C now.
ECHO What changes:
ECHO    - SB, SB Pro and SB16 sampling frequency overridden for 
ECHO      correct sound playback with Sam'n'Max CD-ROM version
PAUSE
ECHO Patching...

FPATCH -s268B4402484874 -p+6 -rEB SB16.WDR
FPATCH -s268B4402484874 -p+6 -rEB SBCLONE.WDR
FPATCH -s268B4402484874 -p+6 -rEB SBPRO.WDR
GOTO DONE

:NOFPAT
ECHO ERROR: Please put FPATCH.COM side by side with this batch file for it to work.
GOTO DONE

:NOFILE
ECHO ERROR: Files to patch - (e.g. %EXEFILE%, ...) - not found.
GOTO DONE

:DONE
ECHO Batch file created in 2026 by Ludicrous_peridot

