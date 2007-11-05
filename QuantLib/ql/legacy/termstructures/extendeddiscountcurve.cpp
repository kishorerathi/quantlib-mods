/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2002, 2003 Decillion Pty(Ltd)

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/legacy/termstructures/extendeddiscountcurve.hpp>
#include <ql/legacy/termstructures/compoundforward.hpp>

namespace QuantLib {

    ExtendedDiscountCurve::ExtendedDiscountCurve(
                                 const std::vector<Date>& dates,
                                 const std::vector<DiscountFactor>& discounts,
                                 const Calendar& calendar,
                                 const BusinessDayConvention conv,
                                 const DayCounter& dayCounter)
    : DiscountCurve(dates, discounts, dayCounter, calendar),
      conv_(conv) {
        calibrateNodes();
    }

    void ExtendedDiscountCurve::calibrateNodes() const {
        Size i;
        Integer ci;

        std::vector<Date> dates = dates_;
        std::vector<Time> times = times_;
        std::vector<Rate> discounts = data_;

        for (i = 0, ci = 1; i < dates.size(); i++) {
            Date rateDate = dates[i];
            Date tmpDate = calendar().advance(referenceDate(),
                                             ci, Months, conv_);
            while (rateDate > tmpDate) {
                dates.insert(dates.begin() + i, tmpDate);
                Time t = dayCounter().yearFraction(referenceDate(),tmpDate);
                times.insert(times.begin() + i, t);
                discounts.insert(discounts.begin() + i,
                                 interpolation_(t,true));
                i++;
                tmpDate = calendar().advance(referenceDate(),
                                            ++ci, Months, conv_);
            }
            if (tmpDate == rateDate)
                ci++;
        }
        dates_ = dates;
        times_ = times;
        data_ = discounts;

        interpolation_ = LogLinearInterpolation(times_.begin(), times_.end(),
                                                data_.begin());
        interpolation_.update();
    }

    boost::shared_ptr<CompoundForward>
    ExtendedDiscountCurve::reversebootstrap(Integer compounding) const {
        std::vector<Rate> forwards;
        Date compoundDate = calendar().advance(referenceDate(),
                                              12/compounding,
                                              Months, conv_);
        Time compoundTime = dayCounter().yearFraction(referenceDate(),
                                                      compoundDate);
        Real qFactor = 0.0;
        Size i;
        Integer ci;
        // Ignore first entry (SPOT with df=1.0)
        for (i = 1, ci = 1; i < dates_.size(); i++) {
            Rate fwd;
            Date rateDate = dates_[i];
            Time t = dayCounter().yearFraction(referenceDate(),rateDate);
            DiscountFactor df = discount(t);
            if (t <= compoundTime) {
                fwd = ((1.0/df)-1.0)/t;
                qFactor = df*t;
            } else {
                Date tmpDate = calendar().advance(referenceDate(),
                                                 (12/compounding) * (ci+1),
                                                 Months, conv_);
                Time tt = dayCounter().yearFraction(compoundDate, rateDate);
                fwd = (1.0-df)/(qFactor+df*tt);
                // Rates on non-compounding boundaries?
                if (rateDate >= tmpDate) {
                    ci++;
                    qFactor += df*tt;
                    compoundDate = tmpDate;
                }
            }
            forwards.push_back(fwd);
        }
        forwards.insert(forwards.begin(),forwards[0]);

        return boost::shared_ptr<CompoundForward>(new
            CompoundForward(referenceDate(),
                            dates_, forwards, calendar(), conv_,
                            compounding,
                            dayCounter()));
    }

    Rate ExtendedDiscountCurve::compoundForwardImpl(Time t,
                                                    Integer f) const {
        if (f == 0) {
            Rate zy = zeroYieldImpl(t);
            if (f == 0)
                return zy;
            if (t <= 1.0/f)
                return (std::exp(zy*t)-1.0)/t;
            return (std::exp(zy*(1.0/f))-1.0)*f;
        }
        return forwardCurve(f)->compoundForward(t,f);
    }

    Rate ExtendedDiscountCurve::zeroYieldImpl(Time t) const {
        DiscountFactor df;
        if (t==0.0) {
            Time dt = 0.001;
            df = discountImpl(dt);
            return Rate(-std::log(df)/dt);
        } else {
            df = discountImpl(t);
            return Rate(-std::log(df)/t);
        }
    }

    boost::shared_ptr<CompoundForward>
    ExtendedDiscountCurve::forwardCurve(Integer compounding) const {
        if (forwardCurveMap_.find(compounding) == forwardCurveMap_.end())
            forwardCurveMap_[compounding] = reversebootstrap(compounding);
        return forwardCurveMap_[compounding];
    }

}
