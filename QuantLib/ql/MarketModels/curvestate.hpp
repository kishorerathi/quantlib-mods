/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Ferdinando Ametrano
 Copyright (C) 2006 Mark Joshi

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


#ifndef quantlib_curvestate_hpp
#define quantlib_curvestate_hpp

#include <ql/Math/array.hpp>
#include <vector>

namespace QuantLib {

    /*! This class stores the state of the yield curve associated to the
        fixed calendar times within the simulation.
        This is the workhorse discounting object associated to the rate times
        of the simulation. It's important to pass the rates via an object like
        this to the product rather than directly to make it easier to switch
        to other engines such as a coterminal swap rate engine.
        Many products will not need expired rates and others will only require
        the first rate.
    */
    class CurveState {
    /* There will n+1 rate times expressing payment and reset times
        of forward rates.

                |-----|-----|-----|-----|-----|      (size = 6)
                t0    t1    t2    t3    t4    t5     rateTimes
                f0    f1    f2    f3    f4           forwardRates
                d0    d1    d2    d3    d4    d5     discountBonds
                d0/d0 d1/d0 d2/d0 d3/d0 d4/d0 d5/d0  discountRatios
                sr0   sr1   sr2   sr3   sr4          cotSwaps
    */
      public:
        template <class ForwardIterator>
        CurveState(ForwardIterator begin, ForwardIterator end)
        : rateTimes_(begin, end), nRates_(rateTimes_.size()-1),
          first_(nRates_), firstCotSwap_(nRates_),
          taus_(nRates_),
          forwardRates_(nRates_), discRatios_(nRates_+1, 1.0),
          cotSwaps_(nRates_), cotAnnuities_(nRates_)
        {
            for (Size i=0; i<nRates_; i++)
                taus_[i] = rateTimes_[i+1] - rateTimes_[i];
        }

        //! \name Modifiers
        //@{
        template <class ForwardIterator>
        void setOnForwardRates(ForwardIterator begin,
                               ForwardIterator end,
                               Size firstValidIndex = 0) {
            QL_REQUIRE(Size(end-begin)==nRates_,
                       "too many forward rates: " <<
                       nRates_ << " required, " <<
                       end-begin << " provided");
            QL_REQUIRE(firstValidIndex<nRates_,
                       "first valid index must be less than " <<
                       nRates_ << ": " <<
                       firstValidIndex << " not allowed");

            // forwards
            first_ = firstValidIndex;
            std::copy(begin+first_, end, forwardRates_.begin()+first_);

            // discount ratios
            discRatios_[first_] = 1.0;
            for (Size i=first_; i<nRates_; ++i)
                discRatios_[i+1] = discRatios_[i] /
                                    (1.0+forwardRates_[i]*taus_[i]);

            // lazy evaluation of coterminal swap rates and annuities
            firstCotSwap_ = nRates_;
        }

        template <class ForwardIterator>
        void setOnDiscountRatios(ForwardIterator begin,
                                 ForwardIterator end,
                                 Size firstValidIndex = 0) {
            QL_REQUIRE(end-begin==nRates_+1,
                       "too many discount ratios: " <<
                       nRates_+1 << " required, " <<
                       end-begin << " provided");
            QL_REQUIRE(firstValidIndex<nRates_,
                       "first valid index must be less than " <<
                       nRates_+1 << ": " <<
                       firstValidIndex << " not allowed");

            // discount ratios
            first_ = firstValidIndex;
            std::copy(begin+first_, end, discRatios_.begin()+first_);

            // fwd rates
            for (Size i=first_; i<nRates_; ++i)
                forwardRates_[i] = (discRatios_[i]/discRatios_[i+1]-1.0) /
                                                                    taus_[i];

            // lazy evaluation of coterminal swap rates and annuities
            firstCotSwap_ = nRates_;
        }

        template <class ForwardIterator>
        void setOnCoterminalSwapRates(ForwardIterator begin,
                                      ForwardIterator end,
                                      Size firstValidIndex = 0) {
            QL_REQUIRE(end-begin==nRates_,
                       "too many coterminal swap rates: " <<
                       nRates_ << " required, " <<
                       end-begin << " provided");
            QL_REQUIRE(firstValidIndex<nRates_,
                       "first valid index must be less than " <<
                       nRates_ << ": " <<
                       firstValidIndex << " not allowed");

            // coterminal swap rates
            first_ = firstCotSwap_ = firstValidIndex;
            std::copy(begin+first_, end, cotSwaps_.begin()+first_);

            // discount ratios, annuities, and fwd
            Real accumulatedAnnuities = 0.0;
            for (Size i=nRates_; i>first_; --i) {
                accumulatedAnnuities += taus_[i-1]*discRatios_[i];
                cotAnnuities_[i-1] = accumulatedAnnuities;
                discRatios_[i-1] = discRatios_[nRates_] +
                    cotSwaps_[i-1]*cotAnnuities_[i-1];
                forwardRates_[i-1]=(discRatios_[i-1]/discRatios_[i]-1.0) /
                    taus_[i-1];
            }
        }
        //@}

        //! \name Inspectors
        //@{
        Size numberOfRates() const { return nRates_; }

        const std::vector<Time>& rateTimes() const;
        const std::vector<Time>& rateTaus() const;

        const std::vector<Rate>& forwardRates() const;
        const std::vector<DiscountFactor>& discountRatios() const;
        const std::vector<Real>& coterminalSwapAnnuities() const;
        const std::vector<Rate>& coterminalSwapRates() const;

        Rate forwardRate(Size i) const;
        Real discountRatio(Size i, Size j) const;
        Rate coterminalSwapAnnuity(Size i) const;
        Rate coterminalSwapRate(Size i) const;
        //@}
      private:
        void computeCoterminalSwap(Size firstIndex) const;
        std::vector<Time> rateTimes_;
        Size nRates_;
        Size first_;
        mutable Size firstCotSwap_;
        std::vector<Time> taus_;
        std::vector<Rate> forwardRates_;
        std::vector<DiscountFactor> discRatios_;
        mutable std::vector<Rate> cotSwaps_;
        mutable std::vector<Real> cotAnnuities_;
    };

    // inline definitions

    inline const std::vector<Time>& CurveState::rateTimes() const {
        return rateTimes_;
    }

    inline const std::vector<Time>& CurveState::rateTaus() const {
        return taus_;
    }

    inline const std::vector<Rate>& CurveState::forwardRates() const {
        QL_REQUIRE(first_<nRates_, "curve state not initialized yet");
        return forwardRates_;
    }

    inline const std::vector<DiscountFactor>&
    CurveState::discountRatios() const {
        QL_REQUIRE(first_<nRates_, "curve state not initialized yet");
        return discRatios_;
    }

    inline const std::vector<Rate>&
    CurveState::coterminalSwapAnnuities() const {
        QL_REQUIRE(first_<nRates_, "curve state not initialized yet");
        if (firstCotSwap_>first_)
            computeCoterminalSwap(first_);
        return cotAnnuities_;
    }

    inline const std::vector<Rate>& CurveState::coterminalSwapRates() const {
        QL_REQUIRE(first_<nRates_, "curve state not initialized yet");
        if (firstCotSwap_>first_)
            computeCoterminalSwap(first_);
        return cotSwaps_;
    }

    inline Rate CurveState::forwardRate(Size i) const {
        QL_REQUIRE(i>=first_, "index too low");
        QL_REQUIRE(i<nRates_, "index too high");
        return forwardRates_[i];
    }

    inline Real CurveState::discountRatio(Size i, Size j) const {
        Size iMin = std::min(i, j);
        QL_REQUIRE(iMin>=first_, "index too low");
        QL_REQUIRE(std::max(i, j)<=nRates_, "index too high");
        return discRatios_[i]/discRatios_[j];
    }

    inline Rate CurveState::coterminalSwapAnnuity(Size i) const {
        QL_REQUIRE(i>=first_, "index too low");
        QL_REQUIRE(i<nRates_, "index too high");
        if (firstCotSwap_>i)
            computeCoterminalSwap(i);
        return cotAnnuities_[i];
    }

    inline Rate CurveState::coterminalSwapRate(Size i) const {
        QL_REQUIRE(i>=first_, "index too low");
        QL_REQUIRE(i<nRates_, "index too high");
        if (firstCotSwap_>i)
            computeCoterminalSwap(i);
        return cotSwaps_[i];
    }

}

#endif
