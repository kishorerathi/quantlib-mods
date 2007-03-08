
"""
 Copyright (C) 2005, 2006, 2007 Eric Ehlers
 Copyright (C) 2005 Plamen Neykov
 Copyright (C) 2005 Aurelien Chanudet

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

"""Encapsulate state and behavior required to generate source code for a
function."""

import sys
from gensrc.Utilities import common
from gensrc.Utilities import buffer
from gensrc.Serialization import serializable
from gensrc.Functions import behavior
from gensrc.Parameters import parameterlist
from gensrc.Parameters import parameter
from gensrc.Configuration import environment

# possible states for the implemenation of a function on a given platform
DOC_ONLY = 0    # only autogenerate documentation for this function - no code
MANUAL = 1      # function hand-written - registration may be autogenerated
AUTO = 2        # autogenerate all code and documentation (the default)

class SupportedPlatform(serializable.Serializable):
    """Define the implementation of a particular function on a particular 
    platform."""

    groupName = 'SupportedPlatforms'

    # map to take the implementation as a string loaded from XML metadata
    # and convert it to one of the constants defined above
    implStrToInt = {
        'documentationOnly' : DOC_ONLY,
        'manual' : MANUAL,
        'auto' : AUTO }

    def serialize(self, serializer):
        """Load/unload class state to/from serializer object."""
        serializer.serializeAttribute(self, 'implementation', 'auto')
        serializer.serializeAttributeBoolean(self, 'xlMacro', True)
        serializer.serializeAttributeBoolean(self, common.CALC_IN_WIZ, False)

    def postSerialize(self):
        self.implNum = SupportedPlatform.implStrToInt[self.implementation]

class SupportedPlatforms(serializable.Serializable):
    """Define the implementation of a function across all of the platforms
    which that function supports."""

    def platformSupported(self, platformName, implementation):
        """Determine whether this function supported by given platform."""
        return self.SupportedPlatforms.has_key(platformName) \
            and self.SupportedPlatforms[platformName].implNum >= implementation

    def xlMacro(self):
        """Determine whether this function requires macro on excel platform."""
        return self.SupportedPlatforms.has_key('Excel') \
            and self.SupportedPlatforms['Excel'].xlMacro

    def xlCalcInWizard(self):
        """Determine whether this function should be enabled under excel wizard."""
        return self.SupportedPlatforms.has_key('Excel') \
            and self.SupportedPlatforms['Excel'].calcInWizard

class Function(serializable.Serializable):
    """Encapsulate state and behavior required
    to generate source code for a function."""

    groupName = 'Functions'
    loopParameter = None
    enumeration = None
    resetCaller = ''
    generateVOs = False
    validatePermanent = ''
    XL_WIZ_CHECK = '''
        if (functionCall.IsCalledByFuncWiz())
            return 0;'''
    VALIDATE_TRIGGER = '''
        ObjHandler::validateRange(trigger, "trigger");'''

    def serialize(self, serializer):
        """Load/unload class state to/from serializer object."""
        serializer.serializeAttribute(self, common.NAME)
        serializer.serializeProperty(self, common.DESCRIPTION)
        serializer.serializeProperty(self, common.LONG_DESC, self.description)
        serializer.serializeObjectContainer(self, SupportedPlatform)
        serializer.serializeProperty(self, common.ALIAS, 'QuantLibAddin::' + self.name)
        serializer.serializeObject(self, parameterlist.ParameterList)
        serializer.serializeBoolean(self, common.DOCUMENTATION_ONLY)
        serializer.serializeAttributeBoolean(self, common.TRIGGER, True)

    def postSerialize(self):
        # some fields required for the Excel addin
        if self.SupportedPlatforms.xlCalcInWizard():
            self.xlWizardCheck = ''
        else:
            self.xlWizardCheck = Function.XL_WIZ_CHECK
        if self.dependencyTrigger:
            self.xlTrigger = Function.VALIDATE_TRIGGER
        else:
            self.xlTrigger = ''

    def platformSupported(self, platformName, implementation):
        """Determine whether this function supported by given platform."""
        return self.SupportedPlatforms.platformSupported(
            platformName, implementation)

    def xlMacro(self):
        """Determine whether this function requires macro on excel platform."""
        return self.SupportedPlatforms.xlMacro()

    def generateVO(self, addin): return "/* no VO - not a constructor*/"

    def printDebug(self):
        self.ParameterList.printDebug()

class Constructor(Function):
    """Function which constructs a QuantLib object."""

    resetCaller = 'true'
    generateVOs = True
    validatePermanent = '''
        ObjHandler::validateRange(permanent, "permanent");'''
    SET_PERMANENT = '''
        if (permanentCpp)
            objectPointer->setPermanent();'''
    VO_CALL = '''
        objectPointer->setProperties(
            boost::shared_ptr<ObjHandler::ValueObject>(
            new %(namespaceObjects)s::ValueObjects::%(functionName)s(%(parameterList)s)));'''
    DESCRIPTION = 'Construct an object of class %s and return its id'

    funcCtorBuffer = None

    def __init__(self):
        self.returnValue = parameter.ConstructorReturnValue()
        if not Constructor.funcCtorBuffer:
            Constructor.funcCtorBuffer = buffer.loadBuffer('stub.func.constructor')

    def serialize(self, serializer):
        """Load/unload class state to/from serializer object."""
        super(Constructor, self).serialize(serializer)
        serializer.serializeProperty(self, common.LIBRARY_FUNCTION)

    def postSerialize(self):
        """Perform post serialization initialization."""
        Function.postSerialize(self)
        # implicit in the definition of a Constructor is that the first parameter
        # is a string to be used as the objectID of the new object
        self.ParameterList.prepend(parameter.ConstructorObjectID())
        # All ctors have a final optional boolean parameter 'permanent'
        self.ParameterList.append(parameter.PermanentFlag())
        # dependency tracking trigger
        if self.dependencyTrigger:
            self.ParameterList.append(parameter.DependencyTrigger())
        self.description = Constructor.DESCRIPTION % self.libraryFunction
        if not self.longDescription: self.longDescription = self.description

    def generateBody(self, addin):
        """Generate source code for function body."""
        if addin.voSupported:
            setPermanent = addin.convertPermanentFlag + Constructor.SET_PERMANENT
            voCall = Constructor.VO_CALL % {
                'functionName' : self.name,
                'namespaceObjects' : environment.config().namespaceObjects,
                'parameterList' : self.ParameterList.generate(addin.voCall)}
        else:
            setPermanent = ''
            voCall = ''
        return Constructor.funcCtorBuffer % {
            'libraryFunction' : self.libraryFunction,
            'namespaceObjects' : environment.config().namespaceObjects,
            'parameterList' : self.ParameterList.generate(addin.libraryCall), 
            'setPermanent' : setPermanent, 
            'suffix' : addin.objectIdSuffix, 
            'voCall' : voCall}

class Member(Function):
    """Function which invokes member function of existing QuantLib object."""

    deref = '->'

    def serialize(self, serializer):
        """Load/unload class state to/from serializer object."""
        super(Member, self).serialize(serializer)
        serializer.serializeProperty(self, common.LIBRARY_FUNCTION)
        serializer.serializeAttribute(self, common.TYPE)
        serializer.serializeAttribute(self, 'superType')
        serializer.serializeAttribute(self, common.LOOP_PARAMETER)
        serializer.serializeObject(self, parameter.ReturnValue)

    def postSerialize(self):
        """Perform post serialization initialization."""
        Function.postSerialize(self)
        # implicit in the definition of a Member is that the first parameter
        # is the objectID of the object to be retrieved

        # FIXME rework so not necessary to retain "self.parameterObjectID"
        # as reference to first parameter
        self.parameterObjectID = parameter.MemberObjectID(self.type, self.superType)
        self.ParameterList.prepend(self.parameterObjectID)
        # dependency tracking trigger
        if self.dependencyTrigger:
            self.ParameterList.append(parameter.DependencyTrigger())
        # determine behavior (normal or loop)
        if self.loopParameter:
            self.behavior = behavior.BehaviorMemberLoop(self)
        else:
            self.behavior = behavior.BehaviorMemberNormal(self)

    def generateBody(self, addin):
        """Generate source code for the body of the function."""
        return self.behavior.generateBody(addin)

class EnumerationMember(Member):
    """Function which invokes member function of Enumeration from the Registry."""
    deref = '.'

    def serialize(self, serializer):
        """Load/unload class state to/from serializer object."""
        super(EnumerationMember, self).serialize(serializer)

    def postSerialize(self):
        """Perform post serialization initialization."""
        Function.postSerialize(self)
        # implicit in the definition of an EnumerationMember is that the first parameter
        # is the ID of the enumeration to be retrieved

        # FIXME rework so not necessary to retain "self.parameterObjectID"
        # as reference to first parameter
        self.parameterObjectID = parameter.EnumerationId(self.type, self.superType)
        self.ParameterList.prepend(self.parameterObjectID)
        # dependency tracking trigger
        if self.dependencyTrigger:
            self.ParameterList.append(parameter.DependencyTrigger())
        # determine behavior (normal or loop)
        if self.loopParameter:
            self.behavior = behavior.BehaviorEnumerationLoop(self) 
        else:
            self.behavior = behavior.BehaviorMemberNormal(self)

class Procedure(Function):
    """Procedural function not associated with any QuantLib object."""

    def serialize(self, serializer):
        """Load/unload class state to/from serializer object."""
        super(Procedure, self).serialize(serializer)
        serializer.serializeObject(self, parameter.ReturnValue)
        serializer.serializeAttribute(self, common.LOOP_PARAMETER)

    def postSerialize(self):
        """Perform post serialization initialization."""
        Function.postSerialize(self)
        # dependency tracking trigger
        if self.dependencyTrigger:
            self.ParameterList.append(parameter.DependencyTrigger())
        # determine behavior (normal or loop)
        if self.loopParameter:
            self.behavior = behavior.BehaviorProcedureLoop(self) 
        else:
            self.behavior = behavior.BehaviorProcedureNormal(self)

    def generateBody(self, addin):
        """Generate source code for the body of the function."""
        return self.behavior.generateBody(addin)

