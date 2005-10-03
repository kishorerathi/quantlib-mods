
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
    cd AmericanOption
    $(MAKE)
    cd ..\BermudanSwaption
    $(MAKE)
    cd ..\DiscreteHedging
    $(MAKE)
    cd ..\EuropeanOption
    $(MAKE)
    cd ..\Swap
    $(MAKE)
    cd ..

# Clean up
clean::
    cd AmericanOption
    $(MAKE) clean
    cd ..\BermudanSwaption
    $(MAKE) clean
    cd ..\DiscreteHedging
    $(MAKE) clean
    cd ..\EuropeanOption
    $(MAKE) clean
    cd ..\Swap
    $(MAKE) clean
    cd ..
