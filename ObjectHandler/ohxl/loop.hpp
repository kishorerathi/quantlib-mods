
/*
 Copyright (C) 2006, 2007 Eric Ehlers

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

#ifndef ohxl_loop_hpp
#define ohxl_loop_hpp

#include <ohxl/objhandlerxl.hpp>
#include <ohxl/Utilities/utilities.hpp>

namespace ObjHandler {

    template<class LoopFunction, class InputType, class OutputType>
    void loopIteration(
            LoopFunction &loopFunction, 
            XLOPER &xIn, 
            XLOPER &xOut) {
        InputType inputItem;
        operToScalar(xIn, inputItem);
        OutputType returnItem = loopFunction(inputItem);
        scalarToOper(returnItem, xOut);
    }

    template<class LoopFunction, class InputType, class OutputType>
    void loop(
#ifdef OHXL_ENABLE_GARBAGE_COLLECTION
              FunctionCall &functionCall,
#endif // OHXL_ENABLE_GARBAGE_COLLECTION
              LoopFunction &loopFunction, 
              OPER *xIn, 
              XLOPER &xOut) {

        OPER xTemp, *xMulti;
        bool excelToFree = false;
        bool xllToFree = false;
        bool errorInitialized = false;

        // if the input is an array then take its address & carry on
        if (xIn->xltype == xltypeMulti) {
            xMulti = xIn;
        // if the input is a string then call split on it
        } else if (xIn->xltype == xltypeStr) {
            splitOper(xIn, &xTemp);
            xMulti = &xTemp;
            xllToFree = true;
        // if the input is a scalar then just call the function once & return
        } else if (xIn->xltype == xltypeNum
        ||  xIn->xltype == xltypeBool) {
            loopIteration<LoopFunction, InputType, OutputType>(
                loopFunction, *xIn, xOut);
            return;
        // some other input (e.g. a reference) - try to convert to an array
        } else {
            Excel(xlCoerce, &xTemp, 2, xIn, TempInt(xltypeMulti));
            xMulti = &xTemp;
            excelToFree = true;
        }

        xOut.val.array.rows = xMulti->val.array.rows;
        xOut.val.array.columns = xMulti->val.array.columns;
        int numCells = xMulti->val.array.rows * xMulti->val.array.columns;
        xOut.val.array.lparray = new XLOPER[numCells]; 
        if (!xOut.val.array.lparray) {
            if (excelToFree) {
                Excel(xlFree, 0, 1, &xTemp);
            } else if (xllToFree) {
                freeOper(&xTemp);
            }
            OH_FAIL("error on call to new");
        }
        xOut.xltype = xltypeMulti | xlbitDLLFree;

        for (int i=0; i<numCells; ++i) {
            try {
                loopIteration<LoopFunction, InputType, OutputType>(
                    loopFunction, 
                    xMulti->val.array.lparray[i],
                    xOut.val.array.lparray[i]);
            } catch(const std::exception &e) {
                std::ostringstream err;
#ifdef OHXL_ENABLE_GARBAGE_COLLECTION
                if (!errorInitialized) {
                    ObjectHandlerXL::instance().clearError();
                    err << functionCall.getFunctionName() << " - " 
                        << functionCall.getAddressString() 
                        << std::endl << std::endl;
                    errorInitialized = true;
                }
#endif // OHXL_ENABLE_GARBAGE_COLLECTION
                err << "iteration #" << i << " - " << e.what();
                ObjectHandlerXL::instance().logError(err.str(), true);
                xOut.val.array.lparray[i].xltype = xltypeErr;
                xOut.val.array.lparray[i].val.err = xlerrNum;
            }
        }

        // free memory

        if (excelToFree) {
            Excel(xlFree, 0, 1, &xTemp);
        } else if (xllToFree) {
            freeOper(&xTemp);
        }

    }
}

#endif
