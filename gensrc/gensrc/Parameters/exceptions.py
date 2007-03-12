
"""
 Copyright (C) 2007 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
"""

from gensrc.Exceptions import exceptions

class ParameterException(exceptions.GensrcException):
    """Exceptions encountered when processing datatypes."""

class ParameterNameInvalidException(ParameterException):

    INVALID_NAME_ERROR = """
The string "%(parameterName)s" is not a valid parameter name."""

    def __init__(self, parameterName):
        self.value_ = ParameterNameInvalidException.INVALID_NAME_ERROR % {
            'parameterName' : parameterName }

