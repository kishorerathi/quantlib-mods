
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#ifndef quantlib_volatilities_i
#define quantlib_volatilities_i

%include common.i
%include date.i
%include daycounters.i
%include types.i
%include currencies.i
%include observer.i
%include marketelements.i
%include interpolation.i

%{
using QuantLib::BlackVolTermStructure;
using QuantLib::LocalVolTermStructure;
using QuantLib::CapletVolatilityStructure;
%}

%ignore BlackVolTermStructure;
class BlackVolTermStructure : public Extrapolator {
    #if defined(SWIGMZSCHEME) || defined(SWIGGUILE)
    %rename("reference-date") referenceDate;
    %rename("day-counter")    dayCounter;
    %rename("max-date")       maxDate;
    %rename("max-time")       maxTime;
    %rename("min-strike")     minStrike;
    %rename("max-strike")     maxStrike;
    %rename("black-vol")      blackVol;
    %rename("black-variance") blackVariance;
    %rename("black-forward-vol")      blackVol;
    %rename("black-forward-variance") blackVariance;
    #endif
  public:
    Date referenceDate() const;
    DayCounter dayCounter() const;
    Calendar calendar() const;
    Date maxDate() const;
    Time maxTime() const;
    Real minStrike() const;
    Real maxStrike() const;
    Volatility blackVol(const Date&, Real strike,
                        bool extrapolate = false) const;
    Volatility blackVol(Time, Real strike,
                        bool extrapolate = false) const;
    Real blackVariance(const Date&, Real strike,
                       bool extrapolate = false) const;
    Real blackVariance(Time, Real strike,
                       bool extrapolate = false) const;
    Volatility blackForwardVol(const Date&, const Date&,
                               Real strike, bool extrapolate = false) const;
    Volatility blackForwardVol(Time, Time, Real strike,
                               bool extrapolate = false) const;
    Real blackForwardVariance(const Date&, const Date&,
                              Real strike, bool extrapolate = false) const;
    Real blackForwardVariance(Time, Time, Real strike,
                              bool extrapolate = false) const;
};

%template(BlackVolTermStructure) boost::shared_ptr<BlackVolTermStructure>;
IsObservable(boost::shared_ptr<BlackVolTermStructure>);

%template(BlackVolTermStructureHandle) Handle<BlackVolTermStructure>;
IsObservable(Handle<BlackVolTermStructure>);


%ignore LocalVolTermStructure;
class LocalVolTermStructure : public Extrapolator {
    #if defined(SWIGMZSCHEME) || defined(SWIGGUILE)
    %rename("reference-date") referenceDate;
    %rename("day-counter")    dayCounter;
    %rename("max-date")       maxDate;
    %rename("max-time")       maxTime;
    %rename("min-strike")     minStrike;
    %rename("max-strike")     maxStrike;
    %rename("local-vol")      localVol;
    #endif
  public:
    Date referenceDate() const;
    DayCounter dayCounter() const;
    Calendar calendar() const;
    Date maxDate() const;
    Time maxTime() const;
    Real minStrike() const;
    Real maxStrike() const;
    Volatility localVol(const Date&, Real u,
                        bool extrapolate = false) const;
    Volatility localVol(Time, Real u,
                        bool extrapolate = false) const;
};

%template(LocalVolTermStructure) boost::shared_ptr<LocalVolTermStructure>;
IsObservable(boost::shared_ptr<LocalVolTermStructure>);

%template(LocalVolTermStructureHandle) Handle<LocalVolTermStructure>;
IsObservable(Handle<LocalVolTermStructure>);

// capvolstructure

%ignore CapletVolatilityStructure;
class CapletVolatilityStructure : public Extrapolator {
    #if defined(SWIGMZSCHEME) || defined(SWIGGUILE)
    %rename("reference-date") referenceDate;
    %rename("day-counter")    dayCounter;
    %rename("max-date")       maxDate;
    %rename("max-time")       maxTime;
    %rename("min-strike")     minStrike;
    %rename("max-strike")     maxStrike;
    %rename("local-vol")      localVol;
    #endif
  public:
    Date referenceDate() const;
    DayCounter dayCounter() const;
    Calendar calendar() const;
    Date maxDate() const;
    Time maxTime() const;
    Real minStrike() const;
    Real maxStrike() const;
    Volatility volatility(const Date&, Real u,
                        bool extrapolate = false) const;
    Volatility volatility(Time, Real u,
                        bool extrapolate = false) const;
};

%template(CapletVolatilityStructure) boost::shared_ptr<CapletVolatilityStructure>;
IsObservable(boost::shared_ptr<CapletVolatilityStructure>);

%template(CapletVolatilityStructureHandle) Handle<CapletVolatilityStructure>;
IsObservable(Handle<CapletVolatilityStructure>);


// actual term structures below

// constant Black vol term structure
%{
using QuantLib::BlackConstantVol;
typedef boost::shared_ptr<BlackVolTermStructure> BlackConstantVolPtr;
%}

%rename(BlackConstantVol) BlackConstantVolPtr;
class BlackConstantVolPtr : public boost::shared_ptr<BlackVolTermStructure> {
  public:
    %extend {
        BlackConstantVolPtr(
                const Date& referenceDate, Volatility volatility,
                const DayCounter& dayCounter) {
            return new BlackConstantVolPtr(
                new BlackConstantVol(referenceDate, volatility, dayCounter));
        }
        BlackConstantVolPtr(
                const Date& referenceDate,
                const Handle<Quote>& volatility,
                const DayCounter& dayCounter) {
            return new BlackConstantVolPtr(
                new BlackConstantVol(referenceDate, volatility, dayCounter));
        }
        BlackConstantVolPtr(
                Integer settlementDays, const Calendar& calendar,
                Volatility volatility,
                const DayCounter& dayCounter) {
            return new BlackConstantVolPtr(
                new BlackConstantVol(settlementDays, calendar,
                                     volatility, dayCounter));
        }
        BlackConstantVolPtr(
                Integer settlementDays, const Calendar& calendar,
                const Handle<Quote>& volatility,
                const DayCounter& dayCounter) {
            return new BlackConstantVolPtr(
                new BlackConstantVol(settlementDays, calendar,
                                     volatility, dayCounter));
        }
    }
};

/* Copy the type over so that R can put it in the R wrapper code */
#if defined(SWIGR)
enum VolExtrapolationType { ConstantExtrapolation,
			    InterpolatorDefaultExtrapolation };
#endif

// Black smiled surface
%{
using QuantLib::BlackVarianceSurface;

typedef BlackVarianceSurface::Extrapolation VolExtrapolationType;
typedef boost::shared_ptr<BlackVolTermStructure> BlackVarianceSurfacePtr;

VolExtrapolationType volExTypeFromString(std::string s) {
    s = QuantLib::lowercase(s);
    if (s == "const" || s == "constant")
        return BlackVarianceSurface::ConstantExtrapolation;
    else if (s == "" || s == "default")
        return BlackVarianceSurface::InterpolatorDefaultExtrapolation;
    else
        QL_FAIL("unknown extrapolation type: "+s);
}

std::string volExTypeToString(VolExtrapolationType t) {
    switch (t) {
      case BlackVarianceSurface::ConstantExtrapolation:
        return "constant";
      case BlackVarianceSurface::InterpolatorDefaultExtrapolation:
        return "default";
      default:
        QL_FAIL("unknown extrapolation type");
    }
}
%}

MapToString(VolExtrapolationType,volExTypeFromString,volExTypeToString);

%rename(BlackVarianceSurface) BlackVarianceSurfacePtr;
class BlackVarianceSurfacePtr
    : public boost::shared_ptr<BlackVolTermStructure> {
  public:
    %extend {
        BlackVarianceSurfacePtr(
                const Date& referenceDate,
                const std::vector<Date>& dates,
                const std::vector<Real>& strikes,
                const Matrix& blackVols,
                const DayCounter& dayCounter,
                VolExtrapolationType lower =
                    BlackVarianceSurface::InterpolatorDefaultExtrapolation,
                VolExtrapolationType upper =
                    BlackVarianceSurface::InterpolatorDefaultExtrapolation) {
            return new BlackVarianceSurfacePtr(
                new BlackVarianceSurface(referenceDate,dates,strikes,
                                         blackVols,dayCounter,lower,upper));
        }
    }
};



// constant local vol term structure
%{
using QuantLib::LocalConstantVol;
typedef boost::shared_ptr<LocalVolTermStructure> LocalConstantVolPtr;
%}

%rename(LocalConstantVol) LocalConstantVolPtr;
class LocalConstantVolPtr : public boost::shared_ptr<LocalVolTermStructure> {
  public:
    %extend {
        LocalConstantVolPtr(
                const Date& referenceDate, Volatility volatility,
                const DayCounter& dayCounter) {
            return new LocalConstantVolPtr(
                new LocalConstantVol(referenceDate, volatility, dayCounter));
        }
        LocalConstantVolPtr(
                const Date& referenceDate,
                const Handle<Quote>& volatility,
                const DayCounter& dayCounter) {
            return new LocalConstantVolPtr(
                new LocalConstantVol(referenceDate, volatility, dayCounter));
        }
        LocalConstantVolPtr(
                Integer settlementDays, const Calendar& calendar,
                Volatility volatility,
                const DayCounter& dayCounter) {
            return new LocalConstantVolPtr(
                new LocalConstantVol(settlementDays, calendar,
                                     volatility, dayCounter));
        }
        LocalConstantVolPtr(
                Integer settlementDays, const Calendar& calendar,
                const Handle<Quote>& volatility,
                const DayCounter& dayCounter) {
            return new LocalConstantVolPtr(
                new LocalConstantVol(settlementDays, calendar,
                                     volatility, dayCounter));
        }
    }
};


// constant caplet constant term structure
%{
using QuantLib::CapletConstantVolatility;
typedef boost::shared_ptr<CapletVolatilityStructure> CapletConstantVolatilityPtr;
%}

%rename(CapletConstantVolatility) CapletConstantVolatilityPtr;
class CapletConstantVolatilityPtr : public boost::shared_ptr<CapletVolatilityStructure> {
  public:
    %extend {
        CapletConstantVolatilityPtr(
                const Date& referenceDate, Volatility volatility,
                const DayCounter& dayCounter) {
            return new CapletConstantVolatilityPtr(
                new CapletConstantVolatility(referenceDate, volatility, dayCounter));
        }
        CapletConstantVolatilityPtr(
                const Date& referenceDate,
                const Handle<Quote>& volatility,
                const DayCounter& dayCounter) {
            return new CapletConstantVolatilityPtr(
                new CapletConstantVolatility(referenceDate, volatility, dayCounter));
        }
        CapletConstantVolatilityPtr(
                Integer settlementDays, const Calendar& calendar,
                Volatility volatility,
                const DayCounter& dayCounter) {
            return new CapletConstantVolatilityPtr(
                new CapletConstantVolatility(settlementDays, calendar,
                                     volatility, dayCounter));
        }
        CapletConstantVolatilityPtr(
                Integer settlementDays, const Calendar& calendar,
                const Handle<Quote>& volatility,
                const DayCounter& dayCounter) {
            return new CapletConstantVolatilityPtr(
                new CapletConstantVolatility(settlementDays, calendar,
                                     volatility, dayCounter));
        }
    }
};




#endif
