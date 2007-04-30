
/*!
 Copyright (C) 2004, 2005, 2006, 2007 Eric Ehlers

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

#ifndef account_hpp
#define account_hpp

#include <oh/objecthandler.hpp>
#include <oh/valueobject.hpp>

class Account {
public:
    Account(const int &accountNumber,
        const std::string &accountType,
        const int &balance = 0)
        : accountNumber_(accountNumber), accountType_(accountType), balance_(balance) {}
    void setBalance(const int &balance) {
        balance_ = balance;
    }
    const int &getBalance() {
        return balance_;
    }
private:
    int accountNumber_;
    std::string accountType_;
    int balance_;
};

class AccountObject : public ObjectHandler::LibraryObject<Account> {
public:
    AccountObject(
        const int &accountNumber,
        const std::string &accountType);
    void setBalance(const int &balance);
    const int &getBalance();
};

typedef boost::shared_ptr<AccountObject> AccountObjectPtr;

class AccountValueObject : public ObjectHandler::ValueObject {
public:
    AccountValueObject(
        const std::string &objectID,
        const int &accountNumber,
        const std::string &accountType) 
        : objectID_(objectID), accountNumber_(accountNumber), accountType_(accountType) {}
    std::vector<std::string> getPropertyNames() const;
    boost::any getProperty(const std::string& name) const;
protected:
    static const char* mPropertyNames[];
    std::string objectID_;
    int accountNumber_;
    std::string accountType_;
};

#endif

