/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Cristina Duminuco
 Copyright (C) 2007 Giorgio Facchinetti

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

#include <ql/cashflows/capflooredcoupon.hpp>
#include <ql/cashflows/digitalcoupon.hpp>
#include <ql/indexes/indexmanager.hpp>
#include <ql/indexes/interestrateindex.hpp>

namespace QuantLib {
        
    DigitalCoupon::DigitalCoupon(const boost::shared_ptr<FloatingRateCoupon>& underlying,
                      Rate callStrike,
                      Position::Type callPosition,
                      bool isCallATMIncluded,
                      Rate callDigitalPayoff,
                      Rate putStrike,
                      Position::Type putPosition,
                      bool isPutATMIncluded,
                      Rate putDigitalPayoff,
                      Replication::Type replication,
                      Real eps)
    : FloatingRateCoupon(underlying->date(),
                         underlying->nominal(),
                         underlying->accrualStartDate(),
                         underlying->accrualEndDate(),
                         underlying->fixingDays(),
                         underlying->index(),
                         underlying->gearing(),
                         underlying->spread(),
                         underlying->referencePeriodStart(),
                         underlying->referencePeriodEnd(),
                         underlying->dayCounter(),
                         underlying->isInArrears()),
      underlying_(underlying), callCsi_(0.), putCsi_(0.),
      hasCallStrike_(false), hasPutStrike_(false),
      putLeftEps_(eps/2.), putRightEps_(eps/2.),
      callLeftEps_(eps/2.), callRightEps_(eps/2.),
      isCallCashOrNothing_(false), isPutCashOrNothing_(false),
      replicationType_(replication),
      isCallATMIncluded_(isCallATMIncluded), isPutATMIncluded_(isPutATMIncluded){
      
        QL_REQUIRE(eps>0.0, "Non positive epsilon not allowed");

        if (putStrike == Null<Rate>())
            QL_REQUIRE(putDigitalPayoff == Null<Rate>(), "Put Cash rate non allowed if put strike is null");
        if (callStrike == Null<Rate>())
            QL_REQUIRE(callDigitalPayoff == Null<Rate>(), "Call Cash rate non allowed if call strike is null");

        if (callStrike != Null<Rate>()){
            QL_REQUIRE(callStrike >= 0., "negative call strike not allowed");
            hasCallStrike_ = true;
            callStrike_ = callStrike;
            QL_REQUIRE(callStrike_>=eps/2., "call strike < eps/2");
            switch (callPosition) {
                case Position::Long :
                    callCsi_ = 1.0;
                    break;
                case Position::Short :
                    callCsi_ = -1.0;
                    break;
                default:
                    QL_FAIL("unsupported position type");
            }
            if (callDigitalPayoff != Null<Rate>()){
                callDigitalPayoff_ = callDigitalPayoff;
                isCallCashOrNothing_ = true;
            }
        }
        if (putStrike != Null<Rate>()){
            QL_REQUIRE(putStrike >= 0., "negative put strike not allowed");
            hasPutStrike_ = true;
            putStrike_ = putStrike;
            switch (putPosition) {
                case Position::Long :
                    putCsi_ = 1.0;
                    break;
                case Position::Short :
                    putCsi_ = -1.0;
                    break;
                default:
                    QL_FAIL("unsupported position type");
            }
            if (putDigitalPayoff != Null<Rate>()){
                putDigitalPayoff_ = putDigitalPayoff;
                isPutCashOrNothing_ = true;
            }
        }

        switch (replication) {
          case Replication::Central :
            // do nothing
            break;
          case Replication::Sub :
            if (hasCallStrike_) {
                switch (callPosition) {
                    case Position::Long :
                        callLeftEps_ = 0.;
                        callRightEps_ = eps;
                        break;
                    case Position::Short :
                        callLeftEps_ = eps;
                        callRightEps_ = 0.;
                        break;
                    default:
                        QL_FAIL("unsupported position type");
                }
            }            
            if (hasPutStrike_) {
                switch (putPosition) {
                    case Position::Long :
                        putLeftEps_ = eps;
                        putRightEps_ = 0.;
                        break;
                    case Position::Short :
                        putLeftEps_ = 0.;
                        putRightEps_ = eps;
                        break;
                    default:
                        QL_FAIL("unsupported position type");
                }
            }
            break;
          case Replication::Super :
            if (hasCallStrike_) {
                switch (callPosition) {
                    case Position::Long :
                        callLeftEps_ = eps;
                        callRightEps_ = 0.;
                        break;
                    case Position::Short :
                        callLeftEps_ = 0.;
                        callRightEps_ = eps;
                        break;
                    default:
                        QL_FAIL("unsupported position type");
                }
            }
            if (hasPutStrike_) {
                switch (putPosition) {
                    case Position::Long :
                        putLeftEps_ = 0.;
                        putRightEps_ = eps;
                        break;
                    case Position::Short :
                        putLeftEps_ = eps;
                        putRightEps_ = 0.;
                        break;
                    default:
                        QL_FAIL("unsupported position type");
                }
            }
            break;
          default:
            QL_FAIL("unsupported replication type");
        }
        
        registerWith(underlying);
    }
    
    Rate DigitalCoupon::callOptionRate() const {

        Rate callOptionRate = Rate(0.);
        if(hasCallStrike_) {      
            callOptionRate = isCallCashOrNothing_ ? callDigitalPayoff_ : underlying_->rate();
            CappedFlooredCoupon next(underlying_, callStrike_ + callRightEps_);
            CappedFlooredCoupon previous(underlying_, callStrike_ - callLeftEps_);
            callOptionRate *= (next.rate() - previous.rate()) / (callLeftEps_ + callRightEps_);
        }
        return callOptionRate;
    }

    Rate DigitalCoupon::putOptionRate() const {

        Rate putOptionRate = Rate(0.);
        if(hasPutStrike_) {      
            putOptionRate = isPutCashOrNothing_ ? putDigitalPayoff_ : underlying_->rate();
            CappedFlooredCoupon next(underlying_, Null<Rate>(), putStrike_ + putRightEps_);
            CappedFlooredCoupon previous(underlying_, Null<Rate>(), putStrike_ - putLeftEps_);
            putOptionRate *= (next.rate() - previous.rate()) / (putLeftEps_ + putRightEps_);
        }
        return putOptionRate;
    }

    Rate DigitalCoupon::rate() const {

        QL_REQUIRE(underlying_->pricer(), "pricer not set");
        
        Date fixingDate = underlying_->fixingDate();
        Date today = Settings::instance().evaluationDate();
        bool enforceTodaysHistoricFixings = 
            Settings::instance().enforceTodaysHistoricFixings();
        Rate underlyingRate = underlying_->rate();
        if (fixingDate < today ||
            ((fixingDate == today) && enforceTodaysHistoricFixings)) {
            // must have been fixed
            return underlyingRate + callCsi_ * callPayoff() + putCsi_  * putPayoff();
        }
        if (fixingDate == today) {
            // might have been fixed
            Rate pastFixing =
                IndexManager::instance().getHistory((underlying_->index())->name())[fixingDate];
            if (pastFixing != Null<Real>()) {
                return underlyingRate + callCsi_ * callPayoff() + putCsi_  * putPayoff();
            } else
                return underlyingRate + callCsi_ * callOptionRate() + putCsi_ * putOptionRate();
        }
        return underlyingRate + callCsi_ * callOptionRate() + putCsi_ * putOptionRate();
    }

    Rate DigitalCoupon::convexityAdjustment() const {
        return underlying_->convexityAdjustment();
    }

    Rate DigitalCoupon::callStrike() const {
        if (hasCall())
            return callStrike_;
        else
            return Null<Rate>();
   }

    Rate DigitalCoupon::putStrike() const {
        if (hasPut())
            return putStrike_;
        else
            return Null<Rate>();
    }
    
    Rate DigitalCoupon::callDigitalPayoff() const {
        if (isCallCashOrNothing_)
            return callDigitalPayoff_;
        else
            return Null<Rate>();
    }

    Rate DigitalCoupon::putDigitalPayoff() const {
        if (isPutCashOrNothing_)
            return putDigitalPayoff_;
        else
            return Null<Rate>();
    }

    void DigitalCoupon::update() {
        notifyObservers();
    }

    void DigitalCoupon::accept(AcyclicVisitor& v) {
        typedef FloatingRateCoupon super;
        Visitor<DigitalCoupon>* v1 =
            dynamic_cast<Visitor<DigitalCoupon>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            super::accept(v);
    }

    void DigitalIborCoupon::accept(AcyclicVisitor& v) {
        typedef DigitalCoupon super;
        Visitor<DigitalIborCoupon>* v1 =
            dynamic_cast<Visitor<DigitalIborCoupon>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            super::accept(v);
    }

    void DigitalCmsCoupon::accept(AcyclicVisitor& v) {
        typedef DigitalCoupon super;
        Visitor<DigitalCmsCoupon>* v1 =
            dynamic_cast<Visitor<DigitalCmsCoupon>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            super::accept(v);
    }

    Rate DigitalCoupon::callPayoff() const {
        // to use only if index has fixed
        Rate payoff(0.);
        if(hasCallStrike_) {
            Rate underlyingRate = underlying_->rate();
            if ( (underlyingRate - callStrike_) > 1.e-16 ) {
                payoff = isCallCashOrNothing_ ? callDigitalPayoff_ : underlyingRate;
            } else { 
                if (isCallATMIncluded_) {
                    if ( std::abs(callStrike_ - underlyingRate) <= 1.e-16 )
                        payoff = isCallCashOrNothing_ ? callDigitalPayoff_ : underlyingRate;
                }            
            }
        }
        return payoff;
    }

    Rate DigitalCoupon::putPayoff() const {
        // to use only if index has fixed
        Rate payoff(0.);
        if(hasPutStrike_) {
            Rate underlyingRate = underlying_->rate();
            if ( (putStrike_ - underlyingRate) > 1.e-16 ) {
                payoff = isPutCashOrNothing_ ? putDigitalPayoff_ : underlyingRate;
            } else {
                // putStrike_ <= underlyingRate
                if (isPutATMIncluded_) {
                    if ( std::abs(putStrike_ - underlyingRate) <= 1.e-16 ) 
                        payoff = isPutCashOrNothing_ ? putDigitalPayoff_ : underlyingRate;
                }            
            }
        }
        return payoff;
    }

}

