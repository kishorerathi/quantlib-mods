/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006, 2007 Ferdinando Ametrano
 Copyright (C) 2006 Katiuscia Manzoni
 Copyright (C) 2006 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/Instruments/makevanillaswap.hpp>
#include <ql/DayCounters/thirty360.hpp>

namespace QuantLib {

    MakeVanillaSwap::MakeVanillaSwap(const Period& swapTenor,
                                     const boost::shared_ptr<IborIndex>& index,
                                     Rate fixedRate,
                                     const Period& forwardStart)
    : forwardStart_(forwardStart), swapTenor_(swapTenor),
      index_(index), fixedRate_(fixedRate),
      effectiveDate_(Date()),
      fixedCalendar_(index->calendar()), floatCalendar_(index->calendar()),
      discountingTermStructure_(index->termStructure()),
      type_(VanillaSwap::Payer), nominal_(1.0),
      fixedTenor_(Period(1, Years)), floatTenor_(index->tenor()),
      fixedConvention_(ModifiedFollowing),
      fixedTerminationDateConvention_(ModifiedFollowing),
      floatConvention_(ModifiedFollowing),
      floatTerminationDateConvention_(ModifiedFollowing),
      fixedBackward_(true), floatBackward_(true),
      fixedEndOfMonth_(false), floatEndOfMonth_(false),
      fixedFirstDate_(Date()), fixedNextToLastDate_(Date()),
      floatFirstDate_(Date()), floatNextToLastDate_(Date()),
      floatSpread_(0.0),
      fixedDayCount_(Thirty360()), floatDayCount_(index->dayCounter()) {}

    MakeVanillaSwap::operator VanillaSwap() const {

        Date startDate;
        if (effectiveDate_ != Date())
            startDate=effectiveDate_;
        else {
          Natural fixingDays = index_->fixingDays();
          Date referenceDate = Settings::instance().evaluationDate();
          Date spotDate = floatCalendar_.advance(referenceDate,
                                                 fixingDays*Days);
          startDate = spotDate+forwardStart_;
        }

        Date terminationDate = startDate+swapTenor_;

        Schedule fixedSchedule(startDate, terminationDate,
                               fixedTenor_, fixedCalendar_,
                               fixedConvention_,
                               fixedTerminationDateConvention_,
                               fixedBackward_, fixedEndOfMonth_,
                               fixedFirstDate_, fixedNextToLastDate_);

        Schedule floatSchedule(startDate, terminationDate,
                               floatTenor_, floatCalendar_,
                               floatConvention_,
                               floatTerminationDateConvention_,
                               floatBackward_, floatEndOfMonth_,
                               floatFirstDate_, floatNextToLastDate_);

        Rate usedFixedRate = fixedRate_;
        if (fixedRate_ == Null<Rate>())
            usedFixedRate = VanillaSwap(type_, nominal_,
                       fixedSchedule, 0.0, fixedDayCount_,
                       floatSchedule, index_, floatSpread_, floatDayCount_,
                       discountingTermStructure_).fairRate();

        return VanillaSwap(type_, nominal_,
                       fixedSchedule, usedFixedRate, fixedDayCount_,
                       floatSchedule, index_, floatSpread_, floatDayCount_,
                       discountingTermStructure_);
    }

    MakeVanillaSwap::operator boost::shared_ptr<VanillaSwap>() const {

        Date startDate;
        if (effectiveDate_ != Date())
            startDate=effectiveDate_;
        else {
          Natural fixingDays = index_->fixingDays();
          Date referenceDate = Settings::instance().evaluationDate();
          Date spotDate = floatCalendar_.advance(referenceDate, fixingDays*Days);
          startDate = spotDate+forwardStart_;
        }

        Date terminationDate = startDate+swapTenor_;

        Schedule fixedSchedule(startDate, terminationDate,
                               fixedTenor_, fixedCalendar_,
                               fixedConvention_,
                               fixedTerminationDateConvention_,
                               fixedBackward_, fixedEndOfMonth_,
                               fixedFirstDate_, fixedNextToLastDate_);

        Schedule floatSchedule(startDate, terminationDate,
                               floatTenor_, floatCalendar_,
                               floatConvention_,
                               floatTerminationDateConvention_,
                               floatBackward_, floatEndOfMonth_,
                               floatFirstDate_, floatNextToLastDate_);

        Rate usedFixedRate = fixedRate_;
        if (fixedRate_ == Null<Rate>())
            usedFixedRate = VanillaSwap(type_, nominal_,
                       fixedSchedule, 0.0, fixedDayCount_,
                       floatSchedule, index_, floatSpread_, floatDayCount_,
                       discountingTermStructure_).fairRate();

        return boost::shared_ptr<VanillaSwap>(new
            VanillaSwap(type_, nominal_,
                   fixedSchedule, usedFixedRate, fixedDayCount_,
                   floatSchedule, index_, floatSpread_, floatDayCount_,
                   discountingTermStructure_));
    }

    MakeVanillaSwap& MakeVanillaSwap::receiveFixed(bool flag) {
        type_ = flag ? VanillaSwap::Receiver : VanillaSwap::Payer ;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withType(VanillaSwap::Type type) {
        type_ = type;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withNominal(Real n) {
        nominal_ = n;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withEffectiveDate(const Date& effectiveDate) {
        effectiveDate_ = effectiveDate;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withDiscountingTermStructure(
                const Handle<YieldTermStructure>& discountingTermStructure) {
        discountingTermStructure_ = discountingTermStructure;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withFixedLegTenor(const Period& t) {
        fixedTenor_ = t;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFixedLegCalendar(const Calendar& cal) {
        fixedCalendar_ = cal;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFixedLegConvention(BusinessDayConvention bdc) {
        fixedConvention_ = bdc;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFixedLegTerminationDateConvention(BusinessDayConvention bdc) {
        fixedTerminationDateConvention_ = bdc;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withFixedLegForward(bool flag) {
        fixedBackward_ = !flag;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withFixedLegEndOfMonth(bool flag) {
        fixedEndOfMonth_ = flag;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withFixedLegFirstDate(const Date& d) {
        fixedFirstDate_ = d;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFixedLegNextToLastDate(const Date& d) {
        fixedNextToLastDate_ = d;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFixedLegDayCount(const DayCounter& dc) {
        fixedDayCount_ = dc;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withFloatingLegTenor(const Period& t) {
        floatTenor_ = t;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFloatingLegCalendar(const Calendar& cal) {
        floatCalendar_ = cal;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFloatingLegConvention(BusinessDayConvention bdc) {
        floatConvention_ = bdc;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFloatingLegTerminationDateConvention(BusinessDayConvention bdc) {
        floatTerminationDateConvention_ = bdc;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withFloatingLegForward(bool flag) {
        floatBackward_ = !flag;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withFloatingLegEndOfMonth(bool flag) {
        floatEndOfMonth_ = flag;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFloatingLegFirstDate(const Date& d) {
        floatFirstDate_ = d;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFloatingLegNextToLastDate(const Date& d) {
        floatNextToLastDate_ = d;
        return *this;
    }

    MakeVanillaSwap&
    MakeVanillaSwap::withFloatingLegDayCount(const DayCounter& dc) {
        floatDayCount_ = dc;
        return *this;
    }

    MakeVanillaSwap& MakeVanillaSwap::withFloatingLegSpread(Spread sp) {
        floatSpread_ = sp;
        return *this;
    }

}
