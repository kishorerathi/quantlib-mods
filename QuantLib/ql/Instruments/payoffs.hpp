/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Ferdinando Ametrano
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

/*! \file payoffs.hpp
    \brief Payoffs for various options
*/

#ifndef quantlib_payoffs_hpp
#define quantlib_payoffs_hpp

#include <ql/option.hpp>

namespace QuantLib {

    //! Intermediate class for call/put/straddle payoffs
    class TypePayoff : public Payoff {
      public:
        TypePayoff(Option::Type type)
        : type_(type) {}
        Option::Type optionType() const { return type_; };
      protected:
        Option::Type type_;
    };

    //! Intermediate class for payoffs based on a fixed strike
    class StrikedTypePayoff : public TypePayoff {
      public:
        StrikedTypePayoff(Option::Type type,
                          Real strike)
        : TypePayoff(type), strike_(strike) {
            QL_REQUIRE(strike >= 0.0,
                       "negative strike given");
        }
        Real strike() const { return strike_; };
      protected:
        Real strike_;
    };


    //! Plain-vanilla payoff
    class PlainVanillaPayoff : public StrikedTypePayoff {
      public:
        PlainVanillaPayoff(Option::Type type,
                           Real strike)
        : StrikedTypePayoff(type, strike) {}
        Real operator()(Real price) const;
        virtual void accept(AcyclicVisitor&);
    };

    inline Real PlainVanillaPayoff::operator()(Real price) const {
        switch (type_) {
          case Option::Call:
            return std::max<Real>(price-strike_,0.0);
          case Option::Put:
            return std::max<Real>(strike_-price,0.0);
          default:
            QL_FAIL("unknown/illegal option type");
        }
    }

    inline void PlainVanillaPayoff::accept(AcyclicVisitor& v) {
        Visitor<PlainVanillaPayoff>* v1 =
            dynamic_cast<Visitor<PlainVanillaPayoff>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            Payoff::accept(v);
    }


    //! %Payoff with strike expressed as percentage
    class PercentageStrikePayoff : public StrikedTypePayoff {
      public:
        PercentageStrikePayoff(Option::Type type,
                               Real moneyness)
        : StrikedTypePayoff(type, moneyness) {
            QL_REQUIRE(moneyness>=0.0,
                       "negative moneyness not allowed");
        }
        Real operator()(Real price) const;
        virtual void accept(AcyclicVisitor&);
    };

    inline Real PercentageStrikePayoff::operator()(Real price) const {
        switch (type_) {
          case Option::Call:
            return price*std::max<Real>(Real(1.0)-strike_,0.0);
          case Option::Put:
            return price*std::max<Real>(strike_-Real(1.0),0.0);
          default:
            QL_FAIL("unknown/illegal option type");
        }
    }

    inline void PercentageStrikePayoff::accept(AcyclicVisitor& v) {
        Visitor<PercentageStrikePayoff>* v1 =
            dynamic_cast<Visitor<PercentageStrikePayoff>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            Payoff::accept(v);
    }


    //! Binary cash-or-nothing payoff
    class CashOrNothingPayoff : public StrikedTypePayoff {
      public:
        CashOrNothingPayoff(Option::Type type,
                            Real strike,
                            Real cashPayoff)
        : StrikedTypePayoff(type, strike), cashPayoff_(cashPayoff) {}
        Real operator()(Real price) const;
        Real cashPayoff() const { return cashPayoff_;}
        virtual void accept(AcyclicVisitor&);
      private:
        Real cashPayoff_;
    };

    inline Real CashOrNothingPayoff::operator()(Real price) const {
        switch (type_) {
          case Option::Call:
            return (price-strike_ > 0.0 ? cashPayoff_ : 0.0);
          case Option::Put:
            return (strike_-price > 0.0 ? cashPayoff_ : 0.0);
          default:
            QL_FAIL("unknown/illegal option type");
        }
    }

    inline void CashOrNothingPayoff::accept(AcyclicVisitor& v) {
        Visitor<CashOrNothingPayoff>* v1 =
            dynamic_cast<Visitor<CashOrNothingPayoff>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            Payoff::accept(v);
    }


    //! Binary asset-or-nothing payoff
    class AssetOrNothingPayoff : public StrikedTypePayoff {
      public:
        AssetOrNothingPayoff(Option::Type type,
                             Real strike)
        : StrikedTypePayoff(type, strike) {}
        Real operator()(Real price) const;
        virtual void accept(AcyclicVisitor&);
    };

    inline Real AssetOrNothingPayoff::operator()(Real price) const {
        switch (type_) {
          case Option::Call:
            return (price-strike_ > 0.0 ? price : 0.0);
          case Option::Put:
            return (strike_-price > 0.0 ? price : 0.0);
          default:
            QL_FAIL("unknown/illegal option type");
        }
    }

    inline void AssetOrNothingPayoff::accept(AcyclicVisitor& v) {
        Visitor<AssetOrNothingPayoff>* v1 =
            dynamic_cast<Visitor<AssetOrNothingPayoff>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            Payoff::accept(v);
    }


    //! Binary gap payoff
    class GapPayoff : public StrikedTypePayoff {
      public:
        GapPayoff(Option::Type type,
                  Real strike,
                  Real strikePayoff)
        : StrikedTypePayoff(type, strike), strikePayoff_(strikePayoff) {}
        Real operator()(Real price) const;
        Real strikePayoff() const { return strikePayoff_;}
        virtual void accept(AcyclicVisitor&);
      private:
        Real strikePayoff_;
    };

    inline Real GapPayoff::operator()(Real price) const {
        switch (type_) {
          case Option::Call:
            return (price-strike_ > 0.0 ? price-strikePayoff_ : 0.0);
          case Option::Put:
            return (strike_-price > 0.0 ? strikePayoff_-price : 0.0);
          default:
            QL_FAIL("unknown/illegal option type");
        }
    }

    inline void GapPayoff::accept(AcyclicVisitor& v) {
        Visitor<GapPayoff>* v1 =
            dynamic_cast<Visitor<GapPayoff>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            Payoff::accept(v);
    }

    //! Binary supershare payoff
    class SuperSharePayoff : public StrikedTypePayoff {
      public:
        SuperSharePayoff(Option::Type type,
                         Real strike,
                         Real strikeIncrement)
        : StrikedTypePayoff(type, strike), strikeIncrement_(strikeIncrement) {}
        Real operator()(Real price) const;
        Real strikeIncrement() const { return strikeIncrement_;}
        virtual void accept(AcyclicVisitor&);
      private:
        Real strikeIncrement_;
    };

    inline Real SuperSharePayoff::operator()(Real price) const {
        switch (type_) {
          case Option::Call:
            return ((price-strike_                  > 0.0 ? 1.0 : 0.0)
                   -(price-strike_-strikeIncrement_ > 0.0 ? 1.0 : 0.0))
                / strikeIncrement_;
          case Option::Put:
            return ((strike_                 -price > 0.0 ? 1.0 : 0.0)
                   -(strike_+strikeIncrement_-price > 0.0 ? 1.0 : 0.0))
                / strikeIncrement_;
          default:
            QL_FAIL("unknown/illegal option type");
        }
    }

    inline void SuperSharePayoff::accept(AcyclicVisitor& v) {
        Visitor<SuperSharePayoff>* v1 =
            dynamic_cast<Visitor<SuperSharePayoff>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            Payoff::accept(v);
    }

}


#endif
