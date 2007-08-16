
/*
 Copyright (C) 2007 Marco Bianchetti

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

#ifndef qla_alpha_form_hpp
#define qla_alpha_form_hpp

#include <oh/libraryobject.hpp>

#include <ql/types.hpp>

namespace QuantLib {
    class AlphaForm;
}

namespace QuantLibAddin {

    OH_LIB_CLASS(AlphaForm, QuantLib::AlphaForm)

    class AlphaFormInverseLinear : public AlphaForm {
      public:
        AlphaFormInverseLinear(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                               const std::vector<QuantLib::Time>& times, 
                               QuantLib::Real alpha,
                               bool permanent);
    };

    class AlphaFormLinearHyperbolic : public AlphaForm {
      public:
        AlphaFormLinearHyperbolic(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                                  const std::vector<QuantLib::Time>& times, 
                                  QuantLib::Real alpha,
                                  bool permanent);
    };

}

#endif

