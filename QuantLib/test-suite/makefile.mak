
# makefile for test-suite.cpp under Borland C++

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
QL_INCLUDE_DIR     = "$(QL_DIR)"
QL_LIB_DIR         = "$(QL_DIR)\lib"
QL_FUN_INCLUDE_DIR = "$(QL_DIR)\functions"
EXE_DIR            = .\bin

# Object files
QL_TESTS = \
    "americanoption.obj$(_mt)$(_D)" \
    "array.obj$(_mt)$(_D)" \
    "asianoptions.obj$(_mt)$(_D)" \
    "barrieroption.obj$(_mt)$(_D)" \
    "basketoption.obj$(_mt)$(_D)" \
    "batesmodel.obj$(_mt)$(_D)" \
    "bermudanswaption.obj$(_mt)$(_D)" \
    "bonds.obj$(_mt)$(_D)" \
    "calendars.obj$(_mt)$(_D)" \
    "capfloor.obj$(_mt)$(_D)" \
    "cliquetoption.obj$(_mt)$(_D)" \
    "compoundforward.obj$(_mt)$(_D)" \
    "convertiblebonds.obj$(_mt)$(_D)" \
    "covariance.obj$(_mt)$(_D)" \
    "dates.obj$(_mt)$(_D)" \
    "daycounters.obj$(_mt)$(_D)" \
    "digitaloption.obj$(_mt)$(_D)" \
    "distributions.obj$(_mt)$(_D)" \
    "dividendoption.obj$(_mt)$(_D)" \
    "europeanoption.obj$(_mt)$(_D)" \
    "exchangerate.obj$(_mt)$(_D)" \
    "factorial.obj$(_mt)$(_D)" \
    "forwardoption.obj$(_mt)$(_D)" \
    "gaussianquadratures.obj$(_mt)$(_D)" \
    "hestonmodel.obj$(_mt)$(_D)" \
    "instruments.obj$(_mt)$(_D)" \
    "integrals.obj$(_mt)$(_D)" \
    "interestrates.obj$(_mt)$(_D)" \
    "interpolations.obj$(_mt)$(_D)" \
    "jumpdiffusion.obj$(_mt)$(_D)" \
    "libormarketmodel.obj$(_mt)$(_D)" \
    "libormarketmodelprocess.obj$(_mt)$(_D)" \
    "lowdiscrepancysequences.obj$(_mt)$(_D)" \
    "matrices.obj$(_mt)$(_D)" \
    "mersennetwister.obj$(_mt)$(_D)" \
    "money.obj$(_mt)$(_D)" \
    "old_pricers.obj$(_mt)$(_D)" \
    "operators.obj$(_mt)$(_D)" \
    "pathgenerator.obj$(_mt)$(_D)" \
    "piecewiseflatforward.obj$(_mt)$(_D)" \
    "piecewiseyieldcurve.obj$(_mt)$(_D)" \
    "quantlibtestsuite.obj$(_mt)$(_D)" \
    "quantooption.obj$(_mt)$(_D)" \
    "quotes.obj$(_mt)$(_D)" \
    "riskstats.obj$(_mt)$(_D)" \
    "rngtraits.obj$(_mt)$(_D)" \
    "rounding.obj$(_mt)$(_D)" \
    "sampledcurve.obj$(_mt)$(_D)" \
    "shortratemodels.obj$(_mt)$(_D)" \
    "solvers.obj$(_mt)$(_D)" \
    "stats.obj$(_mt)$(_D)" \
    "swap.obj$(_mt)$(_D)" \
    "swaption.obj$(_mt)$(_D)" \
    "termstructures.obj$(_mt)$(_D)" \
    "timeseries.obj$(_mt)$(_D)" \
    "tqreigendecomposition.obj$(_mt)$(_D)" \
    "tracing.obj$(_mt)$(_D)" \
    "transformedgrid.obj$(_mt)$(_D)" \
    "utilities.obj$(_mt)$(_D)" \
    "volatilitymodels.obj$(_mt)$(_D)"

# Tools to be used
CC        = bcc32
TLIB      = tlib

# Options
CC_OPTS = -vi- -I$(QL_INCLUDE_DIR) -I$(QL_FUN_INCLUDE_DIR) -w-8070 -w-8066 -w-8008

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

TLIB_OPTS    = /P256

QL_TEST_LIB = all-tests-bcb$(_mt)$(_D)-0_3_8.lib

# Generic rules
.cpp.obj:
    $(CC) -c -q $(CC_OPTS) $<
.cpp.obj$(_mt)$(_D):
    $(CC) -c -q $(CC_OPTS) -o$@ $<


# Primary target:
$(EXE_DIR)\QuantLib-test-suite-bcb$(_mt)$(_D)-$(VERSION_STRING).exe:: $(EXE_DIR) \
                                                          $(QL_TEST_LIB)
    if exist $(EXE_DIR)\QuantLib-test-suite-bcb$(_mt)$(_D)-$(VERSION_STRING).exe \
         del $(EXE_DIR)\QuantLib-test-suite-bcb$(_mt)$(_D)-$(VERSION_STRING).exe
    $(CC) $(CC_OPTS) -L$(QL_LIB_DIR) \
    -e"$(EXE_DIR)\QuantLib-test-suite-bcb$(_mt)$(_D)-$(VERSION_STRING).exe" \
    $(QL_TEST_LIB) libboost_unit_test_framework-bcb$(_mt)$(_D)-1_33_1.lib

$(EXE_DIR):
    if not exist .\bin md .\bin

$(QL_TEST_LIB): $(QL_TESTS)
    if exist $(QL_TEST_LIB)     del $(QL_TEST_LIB)
    $(TLIB) $(TLIB_OPTS) "$(QL_TEST_LIB)" /a $(QL_TESTS)


check: $(EXE_DIR)\QuantLib-test-suite-bcb$(_mt)$(_D)-$(VERSION_STRING).exe
    $(EXE_DIR)\QuantLib-test-suite-bcb$(_mt)$(_D)-$(VERSION_STRING).exe \
                  --log_level=message --build_info=yes --report_level=short
    cd ..


# Clean up
clean::
    if exist *.obj*                  del /q *.obj*
    if exist *.lib*                  del /q *.lib*
    if exist $(EXE_DIR)\*-bcb*.tds   del /q $(EXE_DIR)\*-bcb*.tds
    if exist $(EXE_DIR)\*-bcb*.exe   del /q $(EXE_DIR)\*-bcb*.exe
