
/*
 Copyright (C) 2005, 2006 Eric Ehlers

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

#include <ohxl/register.hpp>

void ohRegisterFunctions(const XLOPER &xDll) {

    Excel(xlfRegister, 0, 10, &xDll,
        TempStrNoSize("\x09""ohVersion"),       // function code name
        TempStrNoSize("\x01""C"),               // parameter codes
        TempStrNoSize("\x09""ohVersion"),       // function display name
        TempStrNoSize("\x00"""),                // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x2D""returns the version number of ObjectHandler  "));// function description

    Excel(xlfRegister, 0, 10, &xDll,
        TempStrNoSize("\x0D""ohObjectCount"),   // function code name
        TempStrNoSize("\x01""N"),               // parameter codes
        TempStrNoSize("\x0D""ohObjectCount"),   // function display name
        TempStrNoSize("\x00"""),                // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x19""#/objects in repository  "));// function description

    Excel(xlfRegister, 0, 10, &xDll,
        TempStrNoSize("\x0C""ohHandleList"),    // function code name
        TempStrNoSize("\x01""R"),               // parameter codes
        TempStrNoSize("\x0C""ohHandleList"),    // function display name
        TempStrNoSize("\x00"""),                // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x1C""list of all object handles  "));// function description

    Excel(xlfRegister, 0, 11, &xDll,
        TempStrNoSize("\x0F""ohPropertyNames"), // function code name
        TempStrNoSize("\x02""RC"),              // parameter codes
        TempStrNoSize("\x0F""ohPropertyNames"), // function display name
        TempStrNoSize("\x06""handle"),          // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x1D""display object property names"), // function description
        TempStrNoSize("\x20""handle of object to be queried  "));// description param 0

    Excel(xlfRegister, 0, 13, &xDll,
        TempStrNoSize("\x0F""ohPropertyValue"), // function code name
        TempStrNoSize("\x04""RCCP"),            // parameter codes
        TempStrNoSize("\x0F""ohPropertyValue"), // function display name
        TempStrNoSize("\x21""handleObject,propertyName,trigger"),// comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x26""retrieve the value of a named property"),// function description
        TempStrNoSize("\x1E""handle of object to be queried"),// description param 0
        TempStrNoSize("\x10""name of property"),// description param 1
        TempStrNoSize("\x1D""dependency tracking trigger  "));// description param 2

    Excel(xlfRegister, 0, 11, &xDll,
        TempStrNoSize("\x0E""ohDeleteObject"),  // function code name
        TempStrNoSize("\x03""LC#"),             // parameter codes
        TempStrNoSize("\x0E""ohDeleteObject"),  // function display name
        TempStrNoSize("\x0C""handleObject"),    // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x1D""delete object from repository"),// function description
        TempStrNoSize("\x20""handle of object to be deleted  "));// description param 0

    Excel(xlfRegister, 0, 10, &xDll,
        TempStrNoSize("\x12""ohDeleteAllObjects"),// function code name
        TempStrNoSize("\x02""L#"),              // parameter codes
        TempStrNoSize("\x12""ohDeleteAllObjects"),// function display name
        TempStrNoSize("\x00"""),                // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x24""delete all objects from repository  "));// function description

    Excel(xlfRegister, 0, 20, &xDll,
        TempStrNoSize("\x0B""ohDependsOn"),     // function code name
        TempStrNoSize("\x0B""LPPPPPPPPPP"),     // parameter codes
        TempStrNoSize("\x0B""ohDependsOn"),     // function display name
        TempStrNoSize("\x45""dummy0,dummy1,dummy2,dummy3,dummy4,dummy5,dummy6,dummy7,dummy8,dummy9"),// comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x28""force a dependency between two functions"),// function description
        TempStrNoSize("\x19""dummy parameter (ignored)"),// description param 0
        TempStrNoSize("\x19""dummy parameter (ignored)"),// description param 1
        TempStrNoSize("\x19""dummy parameter (ignored)"),// description param 2
        TempStrNoSize("\x19""dummy parameter (ignored)"),// description param 3
        TempStrNoSize("\x19""dummy parameter (ignored)"),// description param 4
        TempStrNoSize("\x19""dummy parameter (ignored)"),// description param 5
        TempStrNoSize("\x19""dummy parameter (ignored)"),// description param 6
        TempStrNoSize("\x19""dummy parameter (ignored)"),// description param 7
        TempStrNoSize("\x19""dummy parameter (ignored)"),// description param 8
        TempStrNoSize("\x1B""dummy parameter (ignored)  "));// description param 9

    Excel(xlfRegister, 0, 10, &xDll,
        TempStrNoSize("\x0E""ohGetGcEnabled"),  // function code name
        TempStrNoSize("\x01""L"),               // parameter codes
        TempStrNoSize("\x0E""ohGetGcEnabled"),  // function display name
        TempStrNoSize("\x00"""),                // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x34""get value of ObjectHandler garbage collection flag  "));// function description

    Excel(xlfRegister, 0, 11, &xDll,
        TempStrNoSize("\x0E""ohSetGcEnabled"),  // function code name
        TempStrNoSize("\x03""LL#"),             // parameter codes
        TempStrNoSize("\x0E""ohSetGcEnabled"),  // function display name
        TempStrNoSize("\x00"""),                // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x32""set value of ObjectHandler garbage collection flag"),// function description
        TempStrNoSize("\x21""true (enable) / false (disable)  "));// description param 0

    Excel(xlfRegister, 0, 10, &xDll,
        TempStrNoSize("\x08""ohCallGC"),        // function code name
        TempStrNoSize("\x02""L#"),              // parameter codes
        TempStrNoSize("\x08""ohCallGC"),        // function display name
        TempStrNoSize("\x00"""),                // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x29""delete orphaned objects from repository  "));// function description

    Excel(xlfRegister, 0, 12, &xDll,
        TempStrNoSize("\x0C""ohSetLogFile"),    // function code name
        TempStrNoSize("\x03""CCP"),             // parameter codes
        TempStrNoSize("\x0C""ohSetLogFile"),    // function display name
        TempStrNoSize("\x14""logFileName,logLevel"),// comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x1B""begin logging to named file"),// function description
        TempStrNoSize("\x19""path and name of log file"),// description param 0
        TempStrNoSize("\x1C""threshold for log messages  "));// description param 1

    Excel(xlfRegister, 0, 11, &xDll,
        TempStrNoSize("\x0D""ohSetLogLevel"),   // function code name
        TempStrNoSize("\x02""NN"),              // parameter codes
        TempStrNoSize("\x0D""ohSetLogLevel"),   // function display name
        TempStrNoSize("\x08""logLevel"),        // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x1E""set threshold for log messages"),// function description
        TempStrNoSize("\x1C""threshold for log messages  "));// description param 0

    Excel(xlfRegister, 0, 12, &xDll,
        TempStrNoSize("\x0C""ohLogMessage"),    // function code name
        TempStrNoSize("\x03""CCP"),             // parameter codes
        TempStrNoSize("\x0C""ohLogMessage"),    // function display name
        TempStrNoSize("\x13""logMessage,logLevel"),// comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x0D""log a message"),   // function description
        TempStrNoSize("\x14""message to be logged"),// description param 0
        TempStrNoSize("\x1C""threshold for log messages  "));// description param 1

    Excel(xlfRegister, 0, 11, &xDll,
        TempStrNoSize("\x0B""ohLogObject"),     // function code name
        TempStrNoSize("\x02""LC"),              // parameter codes
        TempStrNoSize("\x0B""ohLogObject"),     // function display name
        TempStrNoSize("\x0C""handleObject"),    // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x24""write object description to log file"),// function description
        TempStrNoSize("\x1F""handle of object to be logged  "));// description param 0

    Excel(xlfRegister, 0, 10, &xDll,
        TempStrNoSize("\x0F""ohLogAllObjects"), // function code name
        TempStrNoSize("\x01""L"),               // parameter codes
        TempStrNoSize("\x0F""ohLogAllObjects"), // function display name
        TempStrNoSize("\x00"""),                // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (1 = worksheet function)
        TempStrNoSize("\x0D""ObjectHandler"),   // function category
        TempStrNoSize("\x00"""),                // shortcut text (command macros only)
        TempStrNoSize("\x00"""),                // path to help file
        TempStrNoSize("\x2B""write all object descriptions to log file  "));// function description

}

