
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006 StatPro Italia srl

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

#ifndef quantlib_indexes_i
#define quantlib_indexes_i

%include date.i
%include calendars.i
%include daycounters.i
%include currencies.i
%include types.i
%include termstructures.i
%include timeseries.i
%include vectors.i

%{
using QuantLib::IndexManager;
%}

class IndexManager {
    #if defined(SWIGRUBY)
    %rename("hasHistory?")  hasHistory;
    #elif defined(SWIGMZSCHEME) || defined(SWIGGUILE)
    %rename("has-history?") hasHistory;
    %rename("history-get")  getHistory;
    %rename("history-set!") setHistory;
    #endif
  private:
    IndexManager();
  public:
    static IndexManager& instance();
    void setHistory(const std::string& name, const TimeSeries<Real>& fixings);
    const TimeSeries<Real>& getHistory(const std::string& name) const;
    bool hasHistory(const std::string& name) const;
    std::vector<std::string> histories() const;
};


// base index class

%{
using QuantLib::Index;
%}

%ignore Index;
class Index {
    #if defined(SWIGRUBY)
    %rename("addFixing!") addFixing;
    #elif defined(SWIGMZSCHEME) || defined(SWIGGUILE)
    %rename("add-fixing") addFixing;
    #endif
  public:
    Rate fixing(const Date& fixingDate) const;
    std::string name() const;
    void addFixing(const Date& fixingDate, Rate fixing);
};

%template(Index) boost::shared_ptr<Index>;
%extend boost::shared_ptr<Index> {
    void addFixings(const std::vector<Date>& fixingDates,
                    const std::vector<Rate>& fixings) {
        (*self)->addFixings(fixingDates.begin(),fixingDates.end(),
                            fixings.begin());
    }
    std::string __str__() {
        return (*self)->name()+" index";
    }
}
IsObservable(boost::shared_ptr<Index>);

// interest-rate indexes
%{
using QuantLib::InterestRateIndex;
typedef boost::shared_ptr<Index> InterestRateIndexPtr;
%}

%rename(InterestRateIndex) InterestRateIndexPtr;
class InterestRateIndexPtr : public boost::shared_ptr<Index> {
    #if defined(SWIGMZSCHEME) || defined(SWIGGUILE)
    %rename("family-name")     familyName;
    %rename("settlement-days") settlementDays;
    %rename("day-counter")     dayCounter;
    #endif
  protected:
    InterestRateIndexPtr();
  public:
    %extend {
        std::string familyName() {
            return boost::dynamic_pointer_cast<InterestRateIndex>(*self)
                ->familyName();
        }
        Period tenor() {
            return boost::dynamic_pointer_cast<InterestRateIndex>(*self)
                ->tenor();
        }
        Integer settlementDays() {
            return boost::dynamic_pointer_cast<InterestRateIndex>(*self)
                ->settlementDays();
        }
        Currency currency() {
            return boost::dynamic_pointer_cast<InterestRateIndex>(*self)
                ->currency();
        }
        Calendar calendar() {
            return boost::dynamic_pointer_cast<InterestRateIndex>(*self)
                ->calendar();
        }
        DayCounter dayCounter() {
            return boost::dynamic_pointer_cast<InterestRateIndex>(*self)
                ->dayCounter();
        }
    }
};


// Xibor indexes
%{
using QuantLib::Xibor;
typedef boost::shared_ptr<Index> XiborPtr;
%}

%rename(Xibor) XiborPtr;
class XiborPtr : public InterestRateIndexPtr {
    #if defined(SWIGRUBY)
    %rename("isAdjusted?") isAdjusted;
    #elif defined(SWIGMZSCHEME) || defined(SWIGGUILE)
    %rename("is-adjusted?")            isAdjusted;
    %rename("business-day-convention") businessDayConvention;
    #endif
  public:
    %extend {
        XiborPtr(const std::string& familyName,
                 const Period& tenor,
                 Integer settlementDays,
                 const Currency& currency,
                 const Calendar& calendar,
                 BusinessDayConvention convention,
                 const DayCounter& dayCounter,
                 const Handle<YieldTermStructure>& h =
                                    Handle<YieldTermStructure>()) {
            return new XiborPtr(new Xibor(familyName, tenor, settlementDays,
                                          currency, calendar, convention,
                                          dayCounter, h));
        }
        bool isAdjusted() {
            return boost::dynamic_pointer_cast<Xibor>(*self)->isAdjusted();
        }
        BusinessDayConvention businessDayConvention() {
            return boost::dynamic_pointer_cast<Xibor>(*self)
                 ->businessDayConvention();
        }
    }
};

%define export_xibor_instance(Name)
%{
using QuantLib::Name;
typedef boost::shared_ptr<Index> Name##Ptr;
%}
%rename(Name) Name##Ptr;
class Name##Ptr : public XiborPtr {
  public:
    %extend {
      Name##Ptr(const Period& tenor,
                const Handle<YieldTermStructure>& h =
                                    Handle<YieldTermStructure>()) {
          return new Name##Ptr(new Name(tenor,h));
      }
    }
};
%enddef

%define export_quoted_xibor_instance(Name,Base)
%{
using QuantLib::Name;
typedef boost::shared_ptr<Index> Name##Ptr;
%}
%rename(Name) Name##Ptr;
class Name##Ptr : public Base##Ptr {
  public:
    %extend {
      Name##Ptr(const Handle<YieldTermStructure>& h =
                                    Handle<YieldTermStructure>()) {
          return new Name##Ptr(new Name(h));
      }
    }
};
%enddef



export_xibor_instance(AUDLibor);
export_xibor_instance(CADLibor);
export_xibor_instance(Cdor);
export_xibor_instance(CHFLibor);
export_xibor_instance(DKKLibor);

export_xibor_instance(Euribor);
export_quoted_xibor_instance(EuriborSW,Euribor);
export_quoted_xibor_instance(Euribor2W,Euribor);
export_quoted_xibor_instance(Euribor3W,Euribor);
export_quoted_xibor_instance(Euribor1M,Euribor);
export_quoted_xibor_instance(Euribor2M,Euribor);
export_quoted_xibor_instance(Euribor3M,Euribor);
export_quoted_xibor_instance(Euribor4M,Euribor);
export_quoted_xibor_instance(Euribor5M,Euribor);
export_quoted_xibor_instance(Euribor6M,Euribor);
export_quoted_xibor_instance(Euribor7M,Euribor);
export_quoted_xibor_instance(Euribor8M,Euribor);
export_quoted_xibor_instance(Euribor9M,Euribor);
export_quoted_xibor_instance(Euribor10M,Euribor);
export_quoted_xibor_instance(Euribor11M,Euribor);
export_quoted_xibor_instance(Euribor1Y,Euribor);

export_xibor_instance(Euribor365);
export_quoted_xibor_instance(Euribor365_SW,Euribor365);
export_quoted_xibor_instance(Euribor365_2W,Euribor365);
export_quoted_xibor_instance(Euribor365_3W,Euribor365);
export_quoted_xibor_instance(Euribor365_1M,Euribor365);
export_quoted_xibor_instance(Euribor365_2M,Euribor365);
export_quoted_xibor_instance(Euribor365_3M,Euribor365);
export_quoted_xibor_instance(Euribor365_4M,Euribor365);
export_quoted_xibor_instance(Euribor365_5M,Euribor365);
export_quoted_xibor_instance(Euribor365_6M,Euribor365);
export_quoted_xibor_instance(Euribor365_7M,Euribor365);
export_quoted_xibor_instance(Euribor365_8M,Euribor365);
export_quoted_xibor_instance(Euribor365_9M,Euribor365);
export_quoted_xibor_instance(Euribor365_10M,Euribor365);
export_quoted_xibor_instance(Euribor365_11M,Euribor365);
export_quoted_xibor_instance(Euribor365_1Y,Euribor365);

export_xibor_instance(EURLibor);
export_quoted_xibor_instance(EURLiborSW,EURLibor);
export_quoted_xibor_instance(EURLibor2W,EURLibor);
export_quoted_xibor_instance(EURLibor1M,EURLibor);
export_quoted_xibor_instance(EURLibor2M,EURLibor);
export_quoted_xibor_instance(EURLibor3M,EURLibor);
export_quoted_xibor_instance(EURLibor4M,EURLibor);
export_quoted_xibor_instance(EURLibor5M,EURLibor);
export_quoted_xibor_instance(EURLibor6M,EURLibor);
export_quoted_xibor_instance(EURLibor7M,EURLibor);
export_quoted_xibor_instance(EURLibor8M,EURLibor);
export_quoted_xibor_instance(EURLibor9M,EURLibor);
export_quoted_xibor_instance(EURLibor10M,EURLibor);
export_quoted_xibor_instance(EURLibor11M,EURLibor);
export_quoted_xibor_instance(EURLibor1Y,EURLibor);

export_xibor_instance(GBPLibor);
export_xibor_instance(Jibar);
export_xibor_instance(JPYLibor);
export_xibor_instance(NZDLibor);
export_xibor_instance(Tibor);
export_xibor_instance(TRLibor);
export_xibor_instance(USDLibor);
export_xibor_instance(Zibor);


#endif
