
/*
 Copyright (C) 2004, 2005 StatPro Italia srl

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

#ifndef quantlib_settings_i
#define quantlib_settings_i

%include date.i

%{
using QuantLib::Settings;
%}

class Settings {
    #if defined(SWIGRUBY)
    %rename("evaluationDate") getEvaluationDate;
    %rename("evaluationDate=") setEvaluationDate;
    #elif defined(SWIGMZSCHEME) || defined(SWIGGUILE)
    %rename("evaluation-date-get")  evaluationDate;
    %rename("evaluation-date-set!") setEvaluationDate;
    #endif
  private:
    Settings();
  public:
    static Settings& instance();
    %extend {
        Date getEvaluationDate() {
            return self->evaluationDate();
        }
        void setEvaluationDate(const Date& d) {
            self->evaluationDate() = d;
        }
    }
    #if defined(SWIGPYTHON)
    %pythoncode %{
    evaluationDate = property(getEvaluationDate,setEvaluationDate,None)
    %}
    #endif
};


#endif
