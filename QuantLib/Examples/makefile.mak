
# makefile for QuantLib Examples under Borland C++

.autodepend
#.silent

# MAKE Options

MAKE = $(MAKE) -fmakefile.mak

!ifdef __MT__
    MAKE = $(MAKE) -D__MT__
!endif
!ifdef _RTLDLL
    MAKE = $(MAKE) -D_RTLDLL
!endif
!ifdef _DEBUG
    MAKE = $(MAKE) -D_DEBUG
!endif
!ifdef SAFE
    MAKE = $(MAKE) -DSAFE
!endif
!ifdef QL_DIR
    MAKE = $(MAKE) -DQL_DIR="$(QL_DIR)"
!endif

# Primary target:
# QuantLib Examples
examples::
    cd ConvertibleBonds
    $(MAKE)
    cd ..\DiscreteHedging
    $(MAKE)
    cd ..\EquityOption
    $(MAKE)
    cd ..\FRA
    $(MAKE)
    cd ..\Replication
    $(MAKE)
    cd ..\Repo
    $(MAKE)
    cd ..\Swap
    $(MAKE)
    cd ..

# Clean up
clean::
    cd ConvertibleBonds
    $(MAKE) clean
    cd ..\DiscreteHedging
    $(MAKE) clean
    cd ..\EquityOption
    $(MAKE) clean
    cd ..\FRA
    $(MAKE) clean
    cd ..\Replication
    $(MAKE) clean
    cd ..\Repo
    $(MAKE) clean
    cd ..\Swap
    $(MAKE) clean
    cd ..
