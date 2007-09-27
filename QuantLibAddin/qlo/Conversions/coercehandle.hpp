
/*
 Copyright (C) 2006, 2007 Eric Ehlers

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

#ifndef qlo_conversions_coercehandle_hpp
#define qlo_conversions_coercehandle_hpp

#include <oh/Conversions/coerce.hpp>
#include <oh/exception.hpp>
#include <qlo/handleimpl.hpp>

namespace QuantLibAddin {

    // Accept an id of an Object in the Repository and return a QuantLib::RelinkableHandle<LibraryClass>.
    // 1) If the id is an empty string then return an empty handle
    // 2) If the Object is of class QuantLibAddin::RelinkableHandle then return the contained QuantLib::RelinkableHandle<LibraryClass>
    // 3) If the Object is of class QuantLibAddin::ObjectClass then convert it to a QuantLib::RelinkableHandle<LibraryClass>
    // 4) Otherwise the Object is of an unexpected class so raise an exception

    template <class ObjectClass, class LibraryClass>
    bool objectToHandle(
        const std::string &in,
        QuantLib::Handle<LibraryClass> &out) {

        OH_GET_OBJECT(object, in, ObjectHandler::Object)
        boost::shared_ptr<RelinkableHandleImpl<ObjectClass, LibraryClass> > handlePointer =
            boost::dynamic_pointer_cast<RelinkableHandleImpl<ObjectClass, LibraryClass> >(object);
        if (handlePointer) {
            out = handlePointer->getHandle();
            return true;
        } else {
            return false;
        }
    }

    template <class ObjectClass, class LibraryClass>
    bool wrapObject(
        const std::string &in,
        QuantLib::Handle<LibraryClass> &out) {

        // FIXME this same get has already been performed by objectToHandle().
        // Need to find a way to cache the return value.
        OH_GET_OBJECT(object, in, ObjectHandler::Object)

        boost::shared_ptr<ObjectClass> qloPointer =
            boost::dynamic_pointer_cast<ObjectClass>(object);
        if (qloPointer) {
            boost::shared_ptr<LibraryClass> ret;
            qloPointer->getLibraryObject(ret);
            //out.linkTo(ret);
            out = QuantLib::Handle<LibraryClass>(ret);
            return true;
        } else {
            return false;
        }
    }

    template <class ObjectClass, class LibraryClass>
    class CoerceHandle : public ObjectHandler::Coerce<
        std::string, 
        QuantLib::Handle<LibraryClass> > {

        typedef typename ObjectHandler::Coerce<
            std::string,
            QuantLib::Handle<LibraryClass> >::Conversion Conversion;

        Conversion *getConversions() {
            static Conversion conversions[] = {
                objectToHandle<ObjectClass, LibraryClass>, 
                wrapObject<ObjectClass, LibraryClass>, 
                0
            };
            return conversions; 
        };

        virtual bool inputMissing(const std::string &objectID) { return objectID.empty(); }
    };
}

#endif

