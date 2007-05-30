
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

#ifndef quantlib_stochastic_process_i
#define quantlib_stochastic_process_i

%include marketelements.i
%include termstructures.i
%include volatilities.i

%{
using QuantLib::StochasticProcess;
%}

%ignore StochasticProcess;
class StochasticProcess {};
%template(StochasticProcess) boost::shared_ptr<StochasticProcess>;
IsObservable(boost::shared_ptr<StochasticProcess>);

%{
using QuantLib::StochasticProcess1D;
typedef boost::shared_ptr<StochasticProcess> StochasticProcess1DPtr;
%}

%rename(StochasticProcess1D) StochasticProcess1DPtr;
class StochasticProcess1DPtr
    : public boost::shared_ptr<StochasticProcess> {
  private:
    StochasticProcess1DPtr();
};



%{
using QuantLib::GeneralizedBlackScholesProcess;
typedef boost::shared_ptr<StochasticProcess> GeneralizedBlackScholesProcessPtr;
%}

%rename(GeneralizedBlackScholesPress) GeneralizedBlackScholesProcessPtr;
class GeneralizedBlackScholesProcessPtr : public StochasticProcess1DPtr {
  public:
    %extend {
      GeneralizedBlackScholesProcessPtr(
                             const Handle<Quote>& s0,
                             const Handle<YieldTermStructure>& dividendTS,
                             const Handle<YieldTermStructure>& riskFreeTS,
                             const Handle<BlackVolTermStructure>& volTS) {
          return new GeneralizedBlackScholesProcessPtr(
                       new GeneralizedBlackScholesProcess(s0, dividendTS,
                                                          riskFreeTS, volTS));
      }
    }
};

%{
using QuantLib::BlackScholesProcess;
typedef boost::shared_ptr<StochasticProcess> BlackScholesProcessPtr;
%}

%rename(BlackScholesProcess) BlackScholesProcessPtr;
class BlackScholesProcessPtr : public StochasticProcess1DPtr {
  public:
    %extend {
      BlackScholesProcessPtr(const Handle<Quote>& s0,
                               const Handle<YieldTermStructure>& riskFreeTS,
                               const Handle<BlackVolTermStructure>& volTS) {
          return new BlackScholesProcessPtr(
                            new BlackScholesProcess(s0, riskFreeTS, volTS));
      }
    }
};

%{
using QuantLib::BlackScholesMertonProcess;
typedef boost::shared_ptr<StochasticProcess> BlackScholesMertonProcessPtr;
%}

%rename(BlackScholesMertonProcess) BlackScholesMertonProcessPtr;
class BlackScholesMertonProcessPtr : public StochasticProcess1DPtr {
  public:
    %extend {
      BlackScholesMertonProcessPtr(
                             const Handle<Quote>& s0,
                             const Handle<YieldTermStructure>& dividendTS,
                             const Handle<YieldTermStructure>& riskFreeTS,
                             const Handle<BlackVolTermStructure>& volTS) {
          return new BlackScholesMertonProcessPtr(
                            new BlackScholesMertonProcess(s0, dividendTS,
                                                          riskFreeTS, volTS));
      }
    }
};

%{
using QuantLib::BlackProcess;
typedef boost::shared_ptr<StochasticProcess> BlackProcessPtr;
%}

%rename(BlackProcess) BlackProcessPtr;
class BlackProcessPtr : public StochasticProcess1DPtr {
  public:
    %extend {
      BlackProcessPtr(const Handle<Quote>& s0,
                      const Handle<YieldTermStructure>& riskFreeTS,
                      const Handle<BlackVolTermStructure>& volTS) {
          return new BlackProcessPtr(new BlackProcess(s0, riskFreeTS, volTS));
      }
    }
};

%{
using QuantLib::GarmanKohlagenProcess;
typedef boost::shared_ptr<StochasticProcess> GarmanKohlagenProcessPtr;
%}

%rename(GarmanKohlagenProcess) GarmanKohlagenProcessPtr;
class GarmanKohlagenProcessPtr : public StochasticProcess1DPtr {
  public:
    %extend {
      GarmanKohlagenProcessPtr(
                         const Handle<Quote>& s0,
                         const Handle<YieldTermStructure>& foreignRiskFreeTS,
                         const Handle<YieldTermStructure>& domesticRiskFreeTS,
                         const Handle<BlackVolTermStructure>& volTS) {
          return new GarmanKohlagenProcessPtr(
                        new GarmanKohlagenProcess(s0, foreignRiskFreeTS,
                                                  domesticRiskFreeTS, volTS));
      }
    }
};



%{
using QuantLib::Merton76Process;
typedef boost::shared_ptr<StochasticProcess> Merton76ProcessPtr;
%}

%rename(Merton76Process) Merton76ProcessPtr;
class Merton76ProcessPtr : public StochasticProcess1DPtr {
  public:
    %extend {
      Merton76ProcessPtr(const Handle<Quote>& stateVariable,
                         const Handle<YieldTermStructure>& dividendTS,
                         const Handle<YieldTermStructure>& riskFreeTS,
                         const Handle<BlackVolTermStructure>& volTS,
                         const Handle<Quote>& jumpIntensity,
                         const Handle<Quote>& meanLogJump,
                         const Handle<Quote>& jumpVolatility) {
            return new Merton76ProcessPtr(
                              new Merton76Process(stateVariable, dividendTS,
                                                  riskFreeTS, volTS,
                                                  jumpIntensity, meanLogJump,
                                                  jumpVolatility));
      }
    }
};

%{
using QuantLib::StochasticProcessArray;
typedef boost::shared_ptr<StochasticProcess> StochasticProcessArrayPtr;
%}

%rename(StochasticProcessArray) StochasticProcessArrayPtr;
class StochasticProcessArrayPtr : public boost::shared_ptr<StochasticProcess> {
  public:
    %extend {
      StochasticProcessArrayPtr(
               const std::vector<boost::shared_ptr<StochasticProcess> >&array,
               const Matrix &correlation) {
          std::vector<boost::shared_ptr<StochasticProcess1D> > in_array;
          for (Size j=0; j < array.size(); j++)
              in_array.push_back(
                  boost::dynamic_pointer_cast<StochasticProcess1D>(array[j]));
          return new StochasticProcessArrayPtr(
                           new StochasticProcessArray(in_array, correlation));
      }
    }
};


// allow use of diffusion process vectors
#if defined(SWIGCSHARP)
SWIG_STD_VECTOR_SPECIALIZE( StochasticProcess, boost::shared_ptr<StochasticProcess> )
#endif
%template(StochasticProcessVector)
std::vector<boost::shared_ptr<StochasticProcess> >;

typedef std::vector<boost::shared_ptr<StochasticProcess> >
StochasticProcessVector;


#endif
