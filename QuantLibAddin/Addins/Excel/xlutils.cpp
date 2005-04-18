
/*
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

#include <qla/qladdin.hpp>
#include <Addins/Excel/xlutils.hpp>
#include <exception>
#include <sstream>

using namespace ObjHandler;
using namespace QuantLibAddin;

void anyToXLOPER(const any_ptr &any, 
                 XLOPER &xOp, 
                 const bool &expandVectors) {
    if (any->type() == typeid(int)) {
        xOp.xltype = xltypeInt;
        xOp.val.w = boost::any_cast<int>(*any);
    } else if (any->type() == typeid(double)) {
        xOp.xltype = xltypeNum;
        xOp.val.num = boost::any_cast<double>(*any);
    } else if (any->type() == typeid(std::string)) {
        std::string s = boost::any_cast<std::string>(*any);
        stringToXLOPER(xOp, s.c_str());
    } else if (any->type() == typeid(std::vector<long>)) {
        if (expandVectors) {
            std::vector<long> v= boost::any_cast< std::vector<long> >(*any);
            xOp.xltype = xltypeMulti;
            xOp.xltype |= xlbitDLLFree;
            xOp.val.array.rows = v.size();
            xOp.val.array.columns = 1;
            xOp.val.array.lparray = new XLOPER[v.size()];
            if (!xOp.val.array.lparray)
                throw Exception("error on call to new");
            for (int i=0; i<v.size(); i++) {
                xOp.val.array.lparray[i].xltype = xltypeNum;
                xOp.val.array.lparray[i].val.num = v[i];
            }
        } else {
            std::string s("<VECTOR>");
            stringToXLOPER(xOp, s.c_str());
        }
    } else if (any->type() == typeid(std::vector<double>)) {
        if (expandVectors) {
            std::vector<double> v= boost::any_cast< std::vector<double> >(*any);
            xOp.xltype = xltypeMulti;
            xOp.xltype |= xlbitDLLFree;
            xOp.val.array.rows = v.size();
            xOp.val.array.columns = 1;
            xOp.val.array.lparray = new XLOPER[v.size()];
            if (!xOp.val.array.lparray)
                throw Exception("error on call to new");
            for (int i=0; i<v.size(); i++) {
                xOp.val.array.lparray[i].xltype = xltypeNum;
                xOp.val.array.lparray[i].val.num = v[i];
            }
        } else {
            std::string s("<VECTOR>");
            stringToXLOPER(xOp, s.c_str());
        }
    } else
        xOp.xltype = xltypeErr;
}

void setArray(LPXLOPER xArray, Properties properties, const std::string &handle) {
    xArray->xltype = xltypeMulti;
    xArray->xltype |= xlbitDLLFree;
    xArray->val.array.rows = 1;
    xArray->val.array.columns = properties.size() + 1;
    xArray->val.array.lparray = new XLOPER[properties.size() + 1]; 
    if (!xArray->val.array.lparray)
        throw("setValues: error on call to new");
    stringToXLOPER(xArray->val.array.lparray[0], handle.c_str());
    for (unsigned int i=0; i<properties.size(); i++) {
        ObjectProperty property = properties[i];
        any_ptr a = property();
        anyToXLOPER(a, xArray->val.array.lparray[i + 1]);
    }
}

std::string XLOPERtoString(const XLOPER &xOp) {
    XLOPER xStr;
    if (xlretSuccess != Excel(xlCoerce, &xStr, 2, &xOp, TempInt(xltypeStr))) 
        throw exception("XLOPERtoString: error on call to xlCoerce");
    std::string s;
    if (xStr.val.str[0])
        s.assign(xStr.val.str + 1, xStr.val.str[0]);
    Excel(xlFree, 0, 1, &xStr);
    return s;
}

void stringToXLOPER(XLOPER &xStr, const char *s) {
    xStr.xltype = xltypeStr;
    xStr.xltype |= xlbitDLLFree;
    int len = __min(XL_MAX_STR_LEN, strlen(s));
    xStr.val.str = new char[ len + 1 ];
    if (!xStr.val.str) 
        throw exception("error calling new in function stringToXLOPER");
    if (len)
        strncpy(xStr.val.str + 1, s, len);
    xStr.val.str[0] = len;
}

DLLEXPORT LPXLOPER GetAddress(LPXLOPER xCaller) {
    XLOPER xRef;
    static XLOPER xStr;
    if (xlretSuccess != Excel(xlfVolatile, 0, 1, TempBool(0)))
        throw exception("error on call to xlfVolatile");
    if (xlretSuccess != Excel(xlfGetCell, &xRef, 2, TempInt(1), xCaller))
        throw exception("error on call to xlfGetCell");
    if (xlretSuccess != Excel(xlCoerce, &xStr, 2, &xRef, TempInt(xltypeStr))) 
        throw exception("error on call to xlCoerce");
    Excel(xlFree, 0, 1, &xRef);
    xStr.xltype |= xlbitXLFree;
    return &xStr;
}

std::string getHandleFull(const std::string &handle) {
    XLOPER xCaller, xStr;
    try {
        if (xlretSuccess != Excel(xlfCaller, &xCaller, 0))
            throw exception("error on call to xlfCaller");
        if (xlretSuccess != Excel(xlUDF, &xStr, 2, TempStr(" GetAddress"), &xCaller))
            throw exception("error on call to GetAddress");
    } catch(const exception &e) {
        Excel(xlFree, 0, 2, &xCaller, &xStr);
        std::ostringstream s1;
        s1 << "getHandleFull: " << e.what();
        throw exception(s1.str().c_str());
    }
    std::string ret(handle + '#' + XLOPERtoString(xStr));
    Excel(xlFree, 0, 2, &xCaller, &xStr);
    return ret;
}

