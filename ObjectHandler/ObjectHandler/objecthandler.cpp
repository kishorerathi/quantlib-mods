
/*
 Copyright (C) 2005 Ferdinando Ametrano
 Copyright (C) 2004 Eric Ehlers

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

#include <ObjectHandler/objecthandler.hpp>

namespace ObjHandler {

    void ObjectHandler::storeObject(const std::string &handle,
                                    const obj_ptr &object) {
	    objectList_[handle] = object;
    }

    obj_ptr ObjectHandler::retrieveObject(const std::string &handle) const {
	    /* return objectList_[handle];
           cannot be used as it is non-const
        */
        ObjectList::const_iterator result = objectList_.find(handle);
        if (result!=objectList_.end())
            return result->second;
        else
            return obj_ptr();
    }

}
