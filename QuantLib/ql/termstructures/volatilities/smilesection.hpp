/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Mario Pucci

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

/*! \file smilesection.hpp
    \brief Swaption volatility structure
*/

#ifndef quantlib_smile_section_hpp
#define quantlib_smile_section_hpp

#include <ql/patterns/observable.hpp>
#include <ql/time/date.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/utilities/null.hpp>
#include <vector>

namespace QuantLib {
    class SpreadedSmileSection;
    //! interest rate volatility smile section
    /*! This abstract class provides volatility smile section interface */
    class SmileSection : public virtual Observable {
      public:
        SmileSection(const Date& d,
                     const DayCounter& dc = Actual365Fixed(),
                     const Date& referenceDate = Date());
        SmileSection(Time exerciseTime,
                     const DayCounter& dc = Actual365Fixed());
        SmileSection() {}
        virtual ~SmileSection() {}

        virtual Real minStrike() const = 0;
        virtual Real maxStrike() const = 0;
        virtual Real variance(Rate strike = Null<Rate>()) const;
        virtual Volatility volatility(Rate strike = Null<Rate>()) const;
        virtual Real atmLevel() const = 0;
        virtual const Date& exerciseDate() const { return exerciseDate_; }
        virtual Time exerciseTime() const { return exerciseTime_; }
        virtual const DayCounter& dayCounter() const { return dc_; }
        friend class SpreadedSmileSection;
      protected:
        virtual Real varianceImpl(Rate strike) const = 0;
        virtual Volatility volatilityImpl(Rate strike) const = 0;
      private:
        Date exerciseDate_;
        DayCounter dc_;
        Time exerciseTime_;
    };

    inline Real SmileSection::variance(Rate strike) const {
        if(strike!=Null<Rate>())
            return varianceImpl(strike);
        else
            return varianceImpl(atmLevel());
    }

    inline Volatility SmileSection::volatility(Rate strike) const {
        if(strike!=Null<Rate>())
            return volatilityImpl(strike);
        else
            return volatilityImpl(atmLevel());
    }

    class FlatSmileSection : public SmileSection {
      public:
        FlatSmileSection(const Date& d,
                         Volatility vol,
                         const DayCounter& dc,
                         const Date& referenceDate = Date(),
                         Real atmLevel = Null<Rate>())
        : SmileSection(d, dc, referenceDate), 
          vol_(vol),
          atmLevel_(atmLevel){}

        FlatSmileSection(Time exerciseTime,
                         Volatility vol,
                         const DayCounter& dc = Actual365Fixed())
        : SmileSection(exerciseTime, dc), vol_(vol) {}

        Real minStrike () const { return 0.0; }
        Real maxStrike () const { return QL_MAX_REAL; }
        Real atmLevel() const { return atmLevel_; }
      protected:
        Real varianceImpl(Rate) const { return vol_*vol_* exerciseTime(); }
        Volatility volatilityImpl(Rate) const { return vol_; }
      private:
        Volatility vol_;
        Real atmLevel_;
    };

    class SabrSmileSection : public SmileSection {
      public:
        SabrSmileSection(Time timeToExpiry,
                         Rate forward,
                         const std::vector<Real>& sabrParameters);
        SabrSmileSection(const Date& d,
                         Rate forward,
                         const std::vector<Real>& sabrParameters,
                         const DayCounter& dc = Actual365Fixed());
        Real minStrike () const { return 0.0; }
        Real maxStrike () const { return QL_MAX_REAL; }
        Real atmLevel() const { return forward_; }
    protected:
        Real varianceImpl(Rate strike) const;
        Volatility volatilityImpl(Rate strike) const;
    private:
        Real alpha_, beta_, nu_, rho_, forward_;
    };

    class SpreadedSmileSection : public SmileSection,
                                 public Observer {
      public:
        SpreadedSmileSection(const boost::shared_ptr<SmileSection>& underlyingSection,
                         Spread spread =0)
        : underlyingSection_(underlyingSection), spread_(spread) {
            registerWith(underlyingSection_);
        }

        void update(){ notifyObservers(); }
      
        Real minStrike() const { return underlyingSection_->minStrike(); }
        Real maxStrike() const { return underlyingSection_->maxStrike(); }

        const Date& exerciseDate() const { return underlyingSection_->exerciseDate(); }
        Time exerciseTime() const { return underlyingSection_->exerciseTime(); }
        const DayCounter& dayCounter() const { return underlyingSection_->dayCounter(); }
        
        Real atmLevel() const { return underlyingSection_->atmLevel(); }
      protected:
        Real varianceImpl(Rate strike) const { 
            Volatility vol = volatilityImpl(strike);
            return vol*vol*exerciseTime(); 
        }
        Volatility volatilityImpl(Rate strike) const { 
            return underlyingSection_->volatilityImpl(strike)+spread_; 
        }
      private:
        const boost::shared_ptr<SmileSection> underlyingSection_;
        Spread spread_;
    };

}

#endif
