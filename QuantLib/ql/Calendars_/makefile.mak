
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
    "argentina.obj$(_mt)$(_D)" \
    "australia.obj$(_mt)$(_D)" \
    "brazil.obj$(_mt)$(_D)" \
    "canada.obj$(_mt)$(_D)" \
    "china.obj$(_mt)$(_D)" \
    "czechrepublic.obj$(_mt)$(_D)" \
    "denmark.obj$(_mt)$(_D)" \
    "finland.obj$(_mt)$(_D)" \
    "germany.obj$(_mt)$(_D)" \
    "hongkong.obj$(_mt)$(_D)" \
    "hungary.obj$(_mt)$(_D)" \
    "iceland.obj$(_mt)$(_D)" \
    "india.obj$(_mt)$(_D)" \
    "indonesia.obj$(_mt)$(_D)" \
    "italy.obj$(_mt)$(_D)" \
    "japan.obj$(_mt)$(_D)" \
    "jointcalendar.obj$(_mt)$(_D)" \
    "mexico.obj$(_mt)$(_D)" \
    "newzealand.obj$(_mt)$(_D)" \
    "norway.obj$(_mt)$(_D)" \
    "poland.obj$(_mt)$(_D)" \
    "saudiarabia.obj$(_mt)$(_D)" \
    "singapore.obj$(_mt)$(_D)" \
    "slovakia.obj$(_mt)$(_D)" \
    "southafrica.obj$(_mt)$(_D)" \
    "southkorea.obj$(_mt)$(_D)" \
    "sweden.obj$(_mt)$(_D)" \
    "switzerland.obj$(_mt)$(_D)" \
    "taiwan.obj$(_mt)$(_D)" \
    "target.obj$(_mt)$(_D)" \
    "turkey.obj$(_mt)$(_D)" \
    "ukraine.obj$(_mt)$(_D)" \
    "unitedkingdom.obj$(_mt)$(_D)" \
    "unitedstates.obj$(_mt)$(_D)"


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
Calendars$(_mt)$(_D).lib:: $(OBJS)
    if exist Calendars$(_mt)$(_D).lib     del Calendars$(_mt)$(_D).lib
    $(TLIB) $(TLIB_OPTS) "Calendars$(_mt)$(_D).lib" /a $(OBJS)





# Clean up
clean::
    if exist *.obj* del /q *.obj*
    if exist *.lib  del /q *.lib
