
"""
/*
 * Copyright (C) 2000
 * Ferdinando Ametrano, Luigi Ballabio, Adolfo Benin, Marco Marchioro
 * 
 * This file is part of the QuantLib
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.sourceforge.net/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated 
 * in the QuantLib License: see the file LICENSE.TXT for details.
 * Contact ferdinando@ametrano.net if LICENSE.TXT was not distributed with this file.
 * LICENCE.TXT is also available at http://quantlib.sourceforge.net/LICENSE.TXT
*/
"""

from QuantLib import Statistics

tol = 1e-9

data =    [  3,   4,   5,   2,   3,   4,   5,   6,   4,   7]
weights = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]

s = Statistics()
s.addWeightedSequence(data, weights)

print 'samples', s.samples(),
if (s.samples()==10):
  print 'OK'
else:
  print 'wrong'

print 'sample Weight', s.sampleWeight(),
if (s.sampleWeight()==10):
  print 'OK'
else:
  print 'wrong'

print 'min', s.min(),
if (s.min()==2):
  print 'OK'
else:
  print 'wrong'

print 'max', s.max(),
if (s.max()==7):
  print 'OK'
else:
  print 'wrong'

print 'mean', s.mean(),
if (abs(s.mean()-4.3)<tol):
  print 'OK'
else:
  print 'wrong'

print 'variance', s.variance(),
if (abs(s.variance()-2.23333333333)<tol):
  print 'OK'
else:
  print 'wrong'

print 'stdDev', s.standardDeviation(),
if (abs(s.standardDeviation()-1.4944341181)<tol):
  print 'OK'
else:
  print 'wrong'

print 'skew', s.skewness(),
if (abs(s.skewness()-0.359543071407)<tol):
  print 'OK'
else:
  print 'wrong'

print 'kurt', s.kurtosis(), 
if (abs(s.kurtosis()+0.151799637209)<tol):
  print 'OK'
else:
  print 'wrong'


