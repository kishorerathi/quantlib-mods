
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006 StatPro Italia srl
 Copyright (C) 2005 Dominic Thuillier

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

#ifndef quantlib_cash_flows_i
#define quantlib_cash_flows_i

%include date.i
%include types.i
%include calendars.i
%include daycounters.i
%include indexes.i
%include termstructures.i
%include scheduler.i
%include vectors.i
%include volatilities.i

%{
using QuantLib::CashFlow;
%}

%ignore CashFlow;
class CashFlow {
  public:
    Real amount() const;
    Date date() const;
};

%template(CashFlow) boost::shared_ptr<CashFlow>;
IsObservable(boost::shared_ptr<CashFlow>);


// implementations

%{
using QuantLib::SimpleCashFlow;
using QuantLib::FixedRateCoupon;
using QuantLib::ParCoupon;
using QuantLib::FloatingRateCoupon;
/*using QuantLib::IndexedCoupon;*/
using QuantLib::UpFrontIndexedCoupon;
using QuantLib::InArrearIndexedCoupon;

using QuantLib::IndexedCouponVector;

typedef boost::shared_ptr<CashFlow> SimpleCashFlowPtr;
typedef boost::shared_ptr<CashFlow> FixedRateCouponPtr;
typedef boost::shared_ptr<CashFlow> ParCouponPtr;
typedef boost::shared_ptr<CashFlow> FloatingRateCouponPtr;
typedef boost::shared_ptr<CashFlow> IndexedCouponPtr;
typedef boost::shared_ptr<CashFlow> UpFrontIndexedCouponPtr;
typedef boost::shared_ptr<CashFlow> InArrearIndexedCouponPtr;
%}

%rename(SimpleCashFlow) SimpleCashFlowPtr;
class SimpleCashFlowPtr : public boost::shared_ptr<CashFlow> {
  public:
    %extend {
        SimpleCashFlowPtr(Real amount, const Date& date) {
            return new SimpleCashFlowPtr(new SimpleCashFlow(amount,date));
        }
    }
};

%rename(FixedRateCoupon) FixedRateCouponPtr;
class FixedRateCouponPtr : public boost::shared_ptr<CashFlow> {
  public:
    %extend {
        FixedRateCouponPtr(Real nominal, const Date& paymentDate,
                           Rate rate, const DayCounter& dayCounter,
                           const Date& startDate, const Date& endDate,
                           const Date& refPeriodStart = Date(),
                           const Date& refPeriodEnd = Date()) {
            return new FixedRateCouponPtr(
                new FixedRateCoupon(nominal, paymentDate, rate,
                                    dayCounter, startDate, endDate,
                                    refPeriodStart, refPeriodEnd));
        }
    }
};

%rename(ParCoupon) ParCouponPtr;
class ParCouponPtr : public boost::shared_ptr<CashFlow> {
  public:
    %extend {
        ParCouponPtr(const Date& paymentDate, Real nominal,
                     const Date& startDate, const Date& endDate,
                     Integer fixingDays, const XiborPtr& index,
                     Real gearing = 1.0, Spread spread = 0.0,
                     const Date& refPeriodStart = Date(),
                     const Date& refPeriodEnd = Date(),
                     const DayCounter& dayCounter = DayCounter()) {
            boost::shared_ptr<Xibor> libor =
                boost::dynamic_pointer_cast<Xibor>(index);
            return new ParCouponPtr(
                new ParCoupon(paymentDate, nominal, startDate, endDate,
                              fixingDays, libor, gearing, spread,
                              refPeriodStart, refPeriodEnd, dayCounter));
        }
        Date accrualStartDate() {
            return boost::dynamic_pointer_cast<ParCoupon>(*self)
                 ->accrualStartDate();
        }
        Date accrualEndDate() {
            return boost::dynamic_pointer_cast<ParCoupon>(*self)
                 ->accrualEndDate();
        }
        Rate rate() {
            return boost::dynamic_pointer_cast<ParCoupon>(*self)->rate();
        }
        Rate indexFixing() {
            return boost::dynamic_pointer_cast<ParCoupon>(*self)
                ->indexFixing();
        }
        Real nominal() {
            return boost::dynamic_pointer_cast<ParCoupon>(*self)->nominal();
        }
    }
};

%rename(FloatingRateCoupon) FloatingRateCouponPtr;
class FloatingRateCouponPtr : public boost::shared_ptr<CashFlow> {
  public:
    %extend {
        Rate rate() {
            return boost::dynamic_pointer_cast<FloatingRateCoupon>(*self)->rate();
        }
    Integer fixingDays() {
            return boost::dynamic_pointer_cast<FloatingRateCoupon>(*self)
                ->fixingDays();
    }
        Rate spread() {
            return boost::dynamic_pointer_cast<FloatingRateCoupon>(*self)
                ->spread();
        }
        Rate indexFixing() {
            return boost::dynamic_pointer_cast<FloatingRateCoupon>(*self)
                ->indexFixing();
        }
        Date fixingDate() {
            return boost::dynamic_pointer_cast<FloatingRateCoupon>(*self)
                ->fixingDate();
        }
    }
};


/* %rename(IndexedCoupon) IndexedCouponPtr;
class IndexedCouponPtr : public FloatingRateCouponPtr {}; */

%rename(UpFrontIndexedCoupon) UpFrontIndexedCouponPtr;
class UpFrontIndexedCouponPtr : public FloatingRateCouponPtr {
  public:
    %extend {
        UpFrontIndexedCouponPtr(const Date& paymentDate, Real nominal,
                                const Date& startDate, const Date& endDate,
                                Integer fixingDays, const XiborPtr& index,
                                Real gearing = 1.0, Spread spread = 0.0,
                                const Date& refPeriodStart = Date(),
                                const Date& refPeriodEnd = Date(),
                                const DayCounter &dayCounter = DayCounter()) {
            boost::shared_ptr<Xibor> libor =
                boost::dynamic_pointer_cast<Xibor>(index);
            return new UpFrontIndexedCouponPtr(
                new UpFrontIndexedCoupon(paymentDate, nominal,
                                         startDate, endDate,
                                         fixingDays, libor,
                                         gearing, spread,
                                         refPeriodStart, refPeriodEnd,
                                         dayCounter));
        }
    }
};

%rename(InArrearIndexedCoupon) InArrearIndexedCouponPtr;
class InArrearIndexedCouponPtr : public FloatingRateCouponPtr {
  public:
    %extend {
        InArrearIndexedCouponPtr(const Date& paymentDate, Real nominal,
                                 const Date& startDate, const Date& endDate,
                                 Integer fixingDays, const XiborPtr& index,
                                 Real gearing = 1.0, Spread spread = 0.0,
                                 const Date& refPeriodStart = Date(),
                                 const Date& refPeriodEnd = Date(),
                                 const DayCounter &dayCounter = DayCounter()) {
            boost::shared_ptr<Xibor> libor =
                boost::dynamic_pointer_cast<Xibor>(index);
            return new InArrearIndexedCouponPtr(
                new InArrearIndexedCoupon(paymentDate, nominal,
                                          startDate, endDate,
                                          fixingDays, libor,
                                          gearing, spread,
                                          refPeriodStart, refPeriodEnd,
                                          dayCounter));
        }

        void setCapletVolatility(const Handle<CapletVolatilityStructure>&c) {
            boost::dynamic_pointer_cast<InArrearIndexedCoupon>(*self)
                ->setCapletVolatility(c);
        }
    }
};

#if defined(SWIGCSHARP)
SWIG_STD_VECTOR_SPECIALIZE( CashFlow, boost::shared_ptr<CashFlow> )
#endif
namespace std {
    %template(CashFlowVector) vector<boost::shared_ptr<CashFlow> >;
}

// cash flow vector builders

%{
using QuantLib::FixedRateCouponVector;
%}

std::vector<boost::shared_ptr<CashFlow> >
FixedRateCouponVector(const Schedule& schedule,
                      BusinessDayConvention paymentAdjustment,
                      const std::vector<Real>& nominals,
                      const std::vector<Rate>& couponRates,
                      const DayCounter& dayCount,
                      const DayCounter& firstPeriodDayCount = DayCounter());

%inline %{
std::vector<boost::shared_ptr<CashFlow> >
FloatingRateCouponVector(const Schedule& schedule,
                         BusinessDayConvention paymentAdjustment,
                         const std::vector<Real>& nominals,
                         Integer fixingDays, const XiborPtr& index,
                         const std::vector<Real>& gearings =
                             std::vector<Real>(),
                         const std::vector<Spread>& spreads =
                             std::vector<Spread>(),
                         const DayCounter& dayCount = DayCounter()) {
    boost::shared_ptr<Xibor> libor =
        boost::dynamic_pointer_cast<Xibor>(index);
    return QuantLib::FloatingRateCouponVector(schedule,paymentAdjustment,
                                              nominals,fixingDays,libor,
                                              gearings,spreads,dayCount);
}

std::vector<boost::shared_ptr<CashFlow> >
ParCouponVector(const Schedule& schedule,
                BusinessDayConvention paymentAdjustment,
                const std::vector<Real>& nominals,
                Integer fixingDays, const XiborPtr& index,
                const std::vector<Real>& gearings = std::vector<Real>(),
                const std::vector<Spread>& spreads = std::vector<Spread>(),
                const DayCounter& dayCount = DayCounter()) {
    boost::shared_ptr<Xibor> libor =
        boost::dynamic_pointer_cast<Xibor>(index);
    return QuantLib::IndexedCouponVector<ParCoupon>(schedule,paymentAdjustment,
                                                    nominals,fixingDays,libor,
                                                    gearings,spreads,dayCount);
}

std::vector<boost::shared_ptr<CashFlow> >
InArrearIndexedCouponVector(const Schedule& schedule,
                            BusinessDayConvention paymentAdjustment,
                            const std::vector<Real>& nominals,
                            Integer fixingDays, const XiborPtr& index,
                            const std::vector<Real>& gearings =
                                                      std::vector<Real>(),
                            const std::vector<Spread>& spreads =
                                                      std::vector<Spread>(),
                            const DayCounter& dayCount = DayCounter()) {
    boost::shared_ptr<Xibor> libor =
        boost::dynamic_pointer_cast<Xibor>(index);
    return QuantLib::IndexedCouponVector<InArrearIndexedCoupon>(
                                                  schedule,paymentAdjustment,
                                                  nominals,fixingDays,libor,
                                                  gearings,spreads,dayCount);
}
%}



// cash-flow analysis

%{
using QuantLib::Cashflows;
using QuantLib::Duration;
%}

struct Duration {
    enum Type { Simple, Macaulay, Modified };
};

class Cashflows {
  private:
    Cashflows();
    Cashflows(const Cashflows&);
  public:
    static Real npv(const std::vector<boost::shared_ptr<CashFlow> >&,
                    const Handle<YieldTermStructure>&);
    static Real npv(const std::vector<boost::shared_ptr<CashFlow> >&,
                    const InterestRate&,
                    Date settlementDate = Date());
    static Rate irr(const std::vector<boost::shared_ptr<CashFlow> >&,
                    Real marketPrice,
                    const DayCounter& dayCounter,
                    Compounding compounding,
                    Frequency frequency = NoFrequency,
                    Date settlementDate = Date(),
                    Real tolerance = 1.0e-10,
                    Size maxIterations = 10000,
                    Rate guess = 0.05);
    static Time duration(const std::vector<boost::shared_ptr<CashFlow> >&,
                         const InterestRate&,
                         Duration::Type type = Duration::Modified,
                         Date settlementDate = Date());
    static Real convexity(const std::vector<boost::shared_ptr<CashFlow> >&,
                          const InterestRate&,
                          Date settlementDate = Date());
};


#endif
