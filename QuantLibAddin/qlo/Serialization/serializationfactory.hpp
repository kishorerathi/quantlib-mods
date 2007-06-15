
/*
 Copyright (C) 2007 Eric Ehlers

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

#ifndef qla_serialization_factory_hpp
#define qla_serialization_factory_hpp

#include <oh/ohdefines.hpp>
#include <oh/serializationfactory.hpp>

namespace QuantLibAddin {

    class SerializationFactory : public ObjectHandler::SerializationFactory {
    public:
        SerializationFactory();
    };

}

#endif
