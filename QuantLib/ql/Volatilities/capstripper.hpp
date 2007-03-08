/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Fran�ois du Vignaud

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

/*! \file capstripper.hpp
    \brief caplet volatility stripper
*/

#ifndef quantlib_interpolated_caplet_variance_curve_hpp
#define quantlib_interpolated_caplet_variance_curve_hpp

#include <ql/Patterns/lazyobject.hpp>
#include <ql/capvolstructures.hpp>
#include <ql/Math/matrix.hpp>
#include <ql/CashFlows/cashflowvectors.hpp>
#include <ql/PricingEngines/CapFloor/blackcapfloorengine.hpp>
#include <ql/Indexes/iborindex.hpp>
#include <ql/Volatilities/capletvolatilitiesstructures.hpp>
#include <ql/schedule.hpp>
#include <ql/types.hpp>
#include <ql/daycounter.hpp>
#include <ql/quote.hpp>


namespace QuantLib {

    typedef std::vector<std::vector<boost::shared_ptr<CapFloor> > > CapMatrix;

    class CapsStripper : public CapletVolatilityStructure,
                         public LazyObject{
      public:
        CapsStripper(const std::vector<Period>& tenors,
         const std::vector<Rate>& strikes,
         const std::vector<std::vector<Handle<Quote> > >& vols,
         const boost::shared_ptr<IborIndex>& index,
         const Handle< YieldTermStructure > termStructure,
         const DayCounter& volatilityDayCounter = Actual365Fixed(),
         Real impliedVolatilityAccuracy = 1.0e-6,
         Size maxEvaluations = 100,
         const std::vector<boost::shared_ptr<SmileSection> >&
             smileSectionInterfaces = std::vector<boost::shared_ptr<SmileSection> >(),
         bool allowExtrapolation = true,
         bool decoupleInterpolation = false);

        //@}
        //! \name LazyObject interface
        //@{
        void performCalculations () const;
        void update() {
            TermStructure::update();
            LazyObject::update();
        };
        //@}
        //! \name TermStructure interface
        //@{
        Date maxDate() const;
        DayCounter dayCounter() const;
        //@}
        //! \name CapletVolatilityStructure interface
        //@{
        Real minStrike() const;
        Real maxStrike() const;
        //@}
        //! \ Inspectors
        //@{
        const std::vector<Period>& tenors() { return tenors_; }
        const std::vector<Rate>& strikes() { return strikes_; }
        const CapMatrix& marketDataCap() { return marketDataCap_; }
        Real impliedVolatilityAccuracy() {
            return impliedVolatilityAccuracy_; }
        boost::shared_ptr<ParametrizedCapletVolStructure>
            parametrizedCapletVolStructure() {
                return parametrizedCapletVolStructure_;}

      protected:
          Volatility volatilityImpl(Time t, Rate r) const;
      private:
        void createMarketData();
        CapMatrix marketDataCap_, calibCap_;
        DayCounter volatilityDayCounter_;
        std::vector<Period> tenors_;
        std::vector<Rate> strikes_;
        Real impliedVolatilityAccuracy_;
        Size maxEvaluations_;
        boost::shared_ptr<ParametrizedCapletVolStructure>
            parametrizedCapletVolStructure_;
    };

    inline DayCounter CapsStripper::dayCounter() const {
        return volatilityDayCounter_;
    }

    inline Date CapsStripper::maxDate() const {
        return parametrizedCapletVolStructure_->maxDate();
    }

    inline Rate CapsStripper::minStrike() const {
        return parametrizedCapletVolStructure_->minStrike();
    }

    inline Rate CapsStripper::maxStrike() const {
        return parametrizedCapletVolStructure_->maxStrike();
    }
}

#endif
