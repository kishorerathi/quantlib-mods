/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Ferdinando Ametrano
 Copyright (C) 2006 Fran�ois du Vignaud
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb
 Copyright (C) 2006, 2007 StatPro Italia srl

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

/*! \file capfloor.hpp
    \brief Cap and Floor class
*/

#ifndef quantlib_instruments_capfloor_hpp
#define quantlib_instruments_capfloor_hpp

#include <ql/numericalmethod.hpp>
#include <ql/instrument.hpp>
#include <ql/cashflow.hpp>
#include <ql/CashFlows/iborcoupon.hpp>
#include <ql/yieldtermstructure.hpp>
#include <ql/Quotes/simplequote.hpp>

namespace QuantLib {

    //! Base class for cap-like instruments
    /*! \ingroup instruments

        \test
        - the correctness of the returned value is tested by checking
          that the price of a cap (resp. floor) decreases
          (resp. increases) with the strike rate.
        - the relationship between the values of caps, floors and the
          resulting collars is checked.
        - the put-call parity between the values of caps, floors and
          swaps is checked.
        - the correctness of the returned implied volatility is tested
          by using it for reproducing the target value.
        - the correctness of the returned value is tested by checking
          it against a known good value.
    */
    class CapFloor : public Instrument {
      public:
        enum Type { Cap, Floor, Collar };
        class arguments;
        class engine;
        CapFloor(Type type,
                 const Leg& floatingLeg,
                 const std::vector<Rate>& capRates,
                 const std::vector<Rate>& floorRates,
                 const Handle<YieldTermStructure>& termStructure,
                 const boost::shared_ptr<PricingEngine>& engine);
        CapFloor(Type type,
                 const Leg& floatingLeg,
                 const std::vector<Rate>& strikes,
                 const Handle<YieldTermStructure>& termStructure,
                 const boost::shared_ptr<PricingEngine>& engine);
        //! \name Instrument interface
        //@{
        bool isExpired() const;
        void setupArguments(PricingEngine::arguments*) const;
        //@}
        //! \name Inspectors
        //@{
        Type type() const { return type_; }
        const Leg& leg() const {
            return floatingLeg_;
        }
        const std::vector<Rate>& capRates() const {
            return capRates_;
        }
        const std::vector<Rate>& floorRates() const {
            return floorRates_;
        }
        const Leg& floatingLeg() const {
            return floatingLeg_;
        }
        Rate atmRate() const;
        Date startDate() const;
        Date maturityDate() const;
        Date lastFixingDate() const;
        //@}
        //! implied term volatility
        Volatility impliedVolatility(Real price,
                                     Real accuracy = 1.0e-4,
                                     Size maxEvaluations = 100,
                                     Volatility minVol = 1.0e-7,
                                     Volatility maxVol = 4.0) const;
      private:
        Type type_;
        Leg floatingLeg_;
        std::vector<Rate> capRates_;
        std::vector<Rate> floorRates_;
        Handle<YieldTermStructure> termStructure_;
        // helper class for implied volatility calculation
        class ImpliedVolHelper {
          public:
            ImpliedVolHelper(const CapFloor&,
                             const Handle<YieldTermStructure>&,
                             Real targetValue);
            Real operator()(Volatility x) const;
            Real derivative(Volatility x) const;
          private:
            boost::shared_ptr<PricingEngine> engine_;
            Handle<YieldTermStructure> termStructure_;
            Real targetValue_;
            boost::shared_ptr<SimpleQuote> vol_;
            const Instrument::results* results_;
        };
    };

    //! Concrete cap class
    /*! \ingroup instruments */
    class Cap : public CapFloor {
      public:
        Cap(const Leg& floatingLeg,
            const std::vector<Rate>& exerciseRates,
            const Handle<YieldTermStructure>& termStructure,
            const boost::shared_ptr<PricingEngine>& engine)
        : CapFloor(CapFloor::Cap, floatingLeg,
                   exerciseRates, std::vector<Rate>(),
                   termStructure, engine) {}
    };

    //! Concrete floor class
    /*! \ingroup instruments */
    class Floor : public CapFloor {
      public:
        Floor(const Leg& floatingLeg,
              const std::vector<Rate>& exerciseRates,
              const Handle<YieldTermStructure>& termStructure,
              const boost::shared_ptr<PricingEngine>& engine)
        : CapFloor(CapFloor::Floor, floatingLeg,
                   std::vector<Rate>(), exerciseRates,
                   termStructure, engine) {}
    };

    //! Concrete collar class
    /*! \ingroup instruments */
    class Collar : public CapFloor {
      public:
        Collar(const Leg& floatingLeg,
               const std::vector<Rate>& capRates,
               const std::vector<Rate>& floorRates,
               const Handle<YieldTermStructure>& termStructure,
               const boost::shared_ptr<PricingEngine>& engine)
        : CapFloor(CapFloor::Collar, floatingLeg, capRates, floorRates,
                   termStructure, engine) {}
    };


    //! %Arguments for cap/floor calculation
    class CapFloor::arguments : public virtual PricingEngine::arguments {
      public:
        arguments() : type(CapFloor::Type(-1)) {}
        CapFloor::Type type;
        std::vector<Time> startTimes;
        std::vector<Date> fixingDates;
        std::vector<Time> fixingTimes;
        std::vector<Time> endTimes;
        std::vector<Time> accrualTimes;
        std::vector<Rate> capRates;
        std::vector<Rate> floorRates;
        std::vector<Rate> forwards;
        std::vector<Real> gearings;
        std::vector<Real> spreads;
        std::vector<DiscountFactor> discounts;
        std::vector<Real> nominals;
        void validate() const;
    };

    //! base class for cap/floor engines
    class CapFloor::engine
        : public GenericEngine<CapFloor::arguments, CapFloor::results> {};

    std::ostream& operator<<(std::ostream&, CapFloor::Type);

}

#endif
