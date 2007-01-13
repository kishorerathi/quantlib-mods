
/*
 Copyright (C) 2006 Francois du Vignaud
 Copyright (C) 2006 Giorgio Facchinetti

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


#include <qlo/quotes.hpp>

namespace QuantLibAddin {

    SimpleQuote::SimpleQuote(QuantLib::Real value)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::Quote>(new
            QuantLib::SimpleQuote(value));
    }

    ForwardValueQuote::ForwardValueQuote(
        const boost::shared_ptr<QuantLib::IborIndex>& index,
        const QuantLib::Date& fixingDate)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::Quote>(new
            QuantLib::ForwardValueQuote(index, fixingDate));
    }
    
    ImpliedStdDevQuote::ImpliedStdDevQuote(
                            const QuantLib::Option::Type& optionType,
                            const QuantLib::Handle<QuantLib::Quote>& forward,
                            const QuantLib::Handle<QuantLib::Quote>& price,
                            QuantLib::Real strike,
                            QuantLib::Real guess,
                            QuantLib::Real accuracy)
    {
    libraryObject_ = boost::shared_ptr<QuantLib::Quote>(new
        QuantLib::ImpliedStdDevQuote(optionType, forward, price, strike,
                                     guess, accuracy));
    }

    EurodollarFuturesImpliedStdDevQuote::EurodollarFuturesImpliedStdDevQuote(
                        const QuantLib::Handle<QuantLib::Quote>& forward,
                        const QuantLib::Handle<QuantLib::Quote>& callPrice,
                        const QuantLib::Handle<QuantLib::Quote>& putPrice,
                        QuantLib::Real strike,
                        QuantLib::Real guess,
                        QuantLib::Real accuracy)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::Quote>(new
            QuantLib::EurodollarFuturesImpliedStdDevQuote(forward,
                callPrice, putPrice, strike, guess, accuracy));
    }

    FuturesConvAdjustmentQuote::FuturesConvAdjustmentQuote(
                               const boost::shared_ptr<QuantLib::IborIndex>& index,
                               const std::string& immCode, 
                               const QuantLib::Handle<QuantLib::Quote>& futuresQuote,
                               const QuantLib::Handle<QuantLib::Quote>& volatility,
                               const QuantLib::Handle<QuantLib::Quote>& meanReversion)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::Quote>(new
            QuantLib::FuturesConvAdjustmentQuote(index, immCode,
                                                 futuresQuote,
                                                 volatility,
                                                 meanReversion));
    }

}
