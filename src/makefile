O = obj
EXENAME=patcher



PLATFORM = djgpp

STUB = $(O)\stub.exe

#UPXOPT=--ultra-brute --lzma
#For older versions of UPX or less powerful PCs
UPXOPT=-9

ifeq ($(REDIR),1)
REDIR_CMD=redir -oa error.log -eo 
else
REDIR_CMD=
endif

# compiler
CC=$(REDIR_CMD) gcc
        
# the exe file name -sf
EXE=$(O)/$(EXENAME).exe
        
# options common to all builds
CFLAGS_COMMON=-I.
LDFLAGS_COMMON=-s
       
# libraries to link in
LIBS=-lstdcxx
        
# this selects flags based on debug and release tagets
CFLAGS=$(CFLAGS_COMMON)  
LDFLAGS=$(LDFLAGS_COMMON) -Wl,-Map=$(O)/$(EXENAME).map

COMMONOBJS =                  \
	$(O)/backup.o       \
	$(O)/file.o     \
	$(O)/io.o      \
	$(O)/toolbox.o      \

SCUMMRPOBJS = \
	$(O)/block.o       \
	$(O)/toc.o     \
	$(O)/scummrp.o      \

PATCHEROBJS = \
	$(O)/samnmax.o       \
	$(O)/patch.o

# object files
OBJS=$(COMMONOBJS) $(SCUMMRPOBJS) $(PATCHEROBJS)


all : $(O) $(EXE) 

$(EXE): $(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@ $(LIBS)
ifneq ($(wildcard $(STUB)),)
	exe2coff $(O)\$(EXENAME).EXE
	del $(O)\$(EXENAME).EXE
	copy /B $(STUB) + $(O)\$(EXENAME) $(O)\$(EXENAME).EXE
	del $(O)\$(EXENAME)
endif
ifneq ($(wildcard $(addsuffix /upx.exe,$(subst ;, ,$(PATH)))),)
	upx $(UPXOPT) $(O)\$(EXENAME).EXE
else
ifneq ($(wildcard $(addsuffix /djp.exe,$(subst ;, ,$(PATH)))),)
	djp $(O)\$(EXENAME).EXE
endif
endif

test : $(O) $(O)/test.exe

$(O)/test.exe : $(O)/patch.o $(O)/test.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(O)/patch.o $(O)/test.o -o $@ 


clean:
	del $(O)\*.o
	del $(O)\*.exe

$(O):
	mkdir $(O)

$(O)/%.o:   COMMON/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(O)/%.o:   SCUMMRP/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(O)/%.o:   PATCHER/%.cpp
	$(CC) -Iscummrp $(CFLAGS) -c $< -o $@

$(O)/%.o:   PATCHER/%.c
	$(CC) $(CFLAGS) -c $< -o $@

