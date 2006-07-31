
.autodepend
#.silent

!ifdef _DEBUG
!ifndef _RTLDLL
    _D = -sd
!else
    _D = -d
!endif
!else
!ifndef _RTLDLL
    _D = -s
!endif
!endif

!ifdef __MT__
    _mt = -mt
!endif

# Directories
INCLUDE_DIR    = ..\..

# Object files
OBJS = \
    "beta.obj$(_mt)$(_D)" \
    "bivariatenormaldistribution.obj$(_mt)$(_D)" \
    "chisquaredistribution.obj$(_mt)$(_D)" \
    "choleskydecomposition.obj$(_mt)$(_D)" \
    "discrepancystatistics.obj$(_mt)$(_D)" \
    "errorfunction.obj$(_mt)$(_D)" \
    "factorial.obj$(_mt)$(_D)" \
    "gammadistribution.obj$(_mt)$(_D)" \
    "gaussianorthogonalpolynomial.obj$(_mt)$(_D)" \
    "gaussianquadratures.obj$(_mt)$(_D)" \
    "generalstatistics.obj$(_mt)$(_D)" \
    "incompletegamma.obj$(_mt)$(_D)" \
    "incrementalstatistics.obj$(_mt)$(_D)" \
    "linearleastsquaresregression.obj$(_mt)$(_D)" \
    "normaldistribution.obj$(_mt)$(_D)" \
    "primenumbers.obj$(_mt)$(_D)" \
    "pseudosqrt.obj$(_mt)$(_D)" \
    "rounding.obj$(_mt)$(_D)" \
    "sampledcurve.obj$(_mt)$(_D)" \
    "svd.obj$(_mt)$(_D)" \
    "symmetricschurdecomposition.obj$(_mt)$(_D)" \
    "tqreigendecomposition.obj$(_mt)$(_D)"

# Tools to be used
CC        = bcc32
TLIB      = tlib


# Options
CC_OPTS        = -vi- -q -c -I$(INCLUDE_DIR) -w-8070

!ifdef _DEBUG
    CC_OPTS = $(CC_OPTS) -v -D_DEBUG
!else
    CC_OPTS = $(CC_OPTS) -O2 -DNDEBUG
!endif

!ifdef _RTLDLL
    CC_OPTS = $(CC_OPTS) -D_RTLDLL
!endif

!ifdef __MT__
    CC_OPTS = $(CC_OPTS) -tWM
!endif

!ifdef SAFE
    CC_OPTS = $(CC_OPTS) -DQL_EXTRA_SAFETY_CHECKS
!endif

TLIB_OPTS    = /P128
!ifdef _DEBUG
TLIB_OPTS    = /P128
!endif

# Generic rules
.cpp.obj:
    $(CC) $(CC_OPTS) $<
.cpp.obj$(_mt)$(_D):
    $(CC) $(CC_OPTS) -o$@ $<

# Primary target:
# static library
Math$(_mt)$(_D).lib:: $(OBJS)
    if exist Math$(_mt)$(_D).lib     del Math$(_mt)$(_D).lib
    $(TLIB) $(TLIB_OPTS) "Math$(_mt)$(_D).lib" /a $(OBJS)





# Clean up
clean::
    if exist *.obj* del /q *.obj*
    if exist *.lib  del /q *.lib
