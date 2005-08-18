
/*
 Copyright (C) 2004, 2005 Eric Ehlers

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

/*! \file
    \brief ObjectHandler class
*/

#ifndef oh_objecthandler_hpp
#define oh_objecthandler_hpp

#include <oh/singleton.hpp>
#include <oh/object.hpp>
#include <map>

//! ObjHandler
/*! name space for the Object Handler
*/
namespace ObjHandler {
    //! Object pointer
    /*! A boost shared pointer to an Object.
    */
    typedef boost::shared_ptr<Object> obj_ptr;

    //! Object list
    /*! A map of string/obj_ptr pairs
        representing all of the Objects
        maintained in the ObjectHandler.
    */
    typedef std::map<std::string, obj_ptr> ObjectList;

    //! Global Object repository.
    /*! Maintains a repository of Objects.
        Objects may be created/amended/destroyed
        by the client application.
    */
    class ObjectHandler : public Singleton<ObjectHandler> {
        friend class Singleton<ObjectHandler>;
    public:
        //! \name storing / retrieving / deleting Objects
        //@{
        //! Store Object with given handle.
        /*! Any existing Object with that handle
            is deleted.
        */
        void storeObject(const std::string &handle,
                         const obj_ptr &object);
        //! Retrieve Object with given handle.
        /*! Returns null if no Object exists
            with that handle.
        */
        obj_ptr retrieveObject(const std::string &handle) const;
        //! Delete Object with given handle.
        /*! Does nothing if no Object exists
            with that handle.
        */
        void deleteObject(const std::string &handle);
        //! Delete all Objects in repository.
        /*! Does nothing if repository
            is already empty.
        */
        void deleteAllObjects();
        //@}
        //! \name utilities
        //@{
        //! Log dump of ObjectHandler.
        /*! Write all objects in ObjectHandler 
            to output stream.
        */
        void dump(std::ostream&);
        //@}
    private:
        ObjectHandler() {}
        // ~ObjectHandler() {}
        ObjectList objectList_;
    };

}

#endif

