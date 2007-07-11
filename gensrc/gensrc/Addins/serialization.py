
"""
 Copyright (C) 2007 Eric Ehlers

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
"""

"""Generate source code for Serialization."""

from gensrc.Addins import addin
from gensrc.Functions import function
from gensrc.Utilities import outputfile
from gensrc.Utilities import common
from gensrc.Utilities import log
from gensrc.Categories import category
from gensrc.Configuration import environment

class Serialization(addin.Addin):
    """Generate source code for Serialization."""

    # class constants

    DECLARE_CREATOR = '''\
    boost::shared_ptr<ObjectHandler::Object> create_%(functionName)s(
        const boost::shared_ptr<ObjectHandler::ValueObject>&);\n\n'''
    REGISTER_CREATOR = '''\
        registerCreator("%(functionName)s", create_%(functionName)s);\n'''
    REGISTER_TYPE = '''\
            // %(categoryDisplayName)s\n
            register_%(categoryName)s(ar);\n\n'''
    REGISTER_CALL = '''\
            ar.register_type<%(namespaceObjects)s::ValueObjects::%(functionName)s>();\n'''
    INCLUDE_CREATOR = '''\
#include <%(libRootDirectory)s/Serialization/create_%(categoryName)s.hpp>\n'''
    REGISTER_INCLUDE = '''\
#include <%(addinDirectory)s/Serialization/serialization_%(categoryName)s.hpp>\n'''

    #############################################
    # public interface
    #############################################

    def generate(self, categoryList, enumerationList):
        """Generate source code for Serialization."""

        self.categoryList_ = categoryList
        self.enumerationList_ = enumerationList

        log.Log.instance().logMessage(' begin generating %s...' % self.name_)
        self.generateCreators()
        self.generateFactory()
        log.Log.instance().logMessage(' done generating %s.' % self.name_)

    def generateFactory(self):
        """Generate source code for all functions in all categories."""

        bufferCreators = ''

        for cat in self.categoryList_.categories('*'):
            if not cat.generateVOs(): continue

            bufferCreators += '\n        // %s\n\n' % cat.displayName()

            for func in cat.functions('*'):
                if not func.generateVOs(): continue

                bufferCreators += Serialization.REGISTER_CREATOR % {
                    'functionName' : func.name() }

        factoryBuffer = self.bufferFactory_.text() % {
            'bufferCreators' : bufferCreators,
            'libRootDirectory' : environment.config().libRootDirectory(),
            'namespaceObjects' : environment.config().namespaceObjects() }
        factoryFile = self.rootPath_ + 'serializationfactory.cpp'
        outputfile.OutputFile(self, factoryFile, self.copyright_, factoryBuffer)

    def generateCreators(self):
        """Generate source code for all functions in all categories."""

        bufferAll = ''

        for cat in self.categoryList_.categories('*'):
            if not cat.generateVOs(): continue

            bufferDeclarations = ''
            bufferCreators = ''

            bufferAll += Serialization.INCLUDE_CREATOR % {
                'categoryName' : cat.name(),
                'libRootDirectory' : environment.config().libRootDirectory() }

            for func in cat.functions('*'):
                if not func.generateVOs(): continue
                bufferDeclarations += Serialization.DECLARE_CREATOR % {
                    'functionName' : func.name() }
                bufferCreators += self.bufferCreator_.text() % {
                    'cppConversions' : func.parameterList().generate(self.cppConversions_),
                    'enumConversions' : func.parameterList().generate(self.enumConversions_),
                    'functionName' : func.name(),
                    'libraryCall' : func.parameterList().generate(self.libraryCall_),
                    'libraryConversions' : func.parameterList().generate(self.libraryConversions_),
                    'libraryFunction' : func.libraryFunction(),
                    'objectConversions' : func.parameterList().generate(self.objectConversions_),
                    'namespaceObjects' : environment.config().namespaceObjects(),
                    'referenceConversions' : func.parameterList().generate(self.referenceConversions_) }

            createHeaderBuffer = self.bufferHeader_.text() % {
                'categoryName' : cat.name(),
                'bufferDeclarations' : bufferDeclarations,
                'libRootDirectory' : environment.config().libRootDirectory(),
                'namespaceObjects' : environment.config().namespaceObjects() }
            createHeaderFile = self.rootPath_ + 'create_' + cat.name() + '.hpp'
            outputfile.OutputFile(self, createHeaderFile, self.copyright_, createHeaderBuffer)

            createBodyBuffer = self.bufferIncludes_.text() % {
                'bufferCreators' : bufferCreators,
                'serializationIncludes' : cat.serializationIncludes(),
                'categoryName' : cat.name() }
            createBodyFile = self.rootPath_ + 'create_' + cat.name() + '.cpp'
            outputfile.OutputFile(self, createBodyFile, self.copyright_, createBodyBuffer)

        createAllBuffer = self.bufferAll_.text() % {
            'bufferAll' : bufferAll,
            'libRootDirectory' : environment.config().libRootDirectory() }
        createAllFile = self.rootPath_ + 'create_all.hpp'
        outputfile.OutputFile(self, createAllFile, self.copyright_, createAllBuffer)

    #############################################
    # serializer interface
    #############################################

    def serialize(self, serializer):
        """load/unload class state to/from serializer object."""
        super(Serialization, self).serialize(serializer)

def generateSerialization(addin, path):
    """Generate source code for all functions in all categories.

    This is a utility function, not part of the Serialization addin class.
    This function generates the source code required for the serialization
    factory.  The code is platform independent.  However, the generated source
    code must be compiled as part of the final Addin binary (not in a static
    library) because of issues related to linking and to template
    metaprogramming performed by the boost::serialization library.  Each Addin
    that supports serialization must call in to this function."""

    allIncludes = ''
    bufferRegister = ''
    includeGuard = 'addin_' + addin.name().lower()
    for cat in addin.categoryList_.categories('*'):

        if not cat.generateVOs(): continue

        bufferCpp = ''
        allIncludes += Serialization.REGISTER_INCLUDE % {
            'categoryName' : cat.name(),
            'addinDirectory' : path }

        bufferRegister += Serialization.REGISTER_TYPE % {
            'categoryDisplayName' : cat.displayName(),
            'categoryName' : cat.name() }

        bufferHpp = addin.bufferSerializeDeclaration_.text() % {
            'addinDirectory' : includeGuard,
            'categoryName' : cat.name(),
            'namespaceAddin' : addin.namespaceAddin_ }
        headerFile = '%sSerialization/serialization_%s.hpp' % ( addin.rootPath_, cat.name() )
        outputfile.OutputFile(addin, headerFile, addin.copyright_, bufferHpp)

        for func in cat.functions('*'):
            if not func.generateVOs(): continue

            bufferCpp += Serialization.REGISTER_CALL % {
                'functionName' : func.name(),
                'namespaceObjects' : environment.config().namespaceObjects() }

        bufferBody = addin.bufferSerializeBody_.text() % {
            'addinDirectory' : path,
            'bufferCpp' : bufferCpp,
            'categoryName' : cat.name(),
            'libRootDirectory' : environment.config().libRootDirectory(),
            'namespaceAddin' : addin.namespaceAddin_ }
        cppFile = '%sSerialization/serialization_%s.cpp' % ( addin.rootPath_, cat.name() )
        outputfile.OutputFile(addin, cppFile, addin.copyright_, bufferBody)
    allBuffer =  addin.bufferSerializeAll_.text() % {
        'addinDirectory' : includeGuard,
        'allIncludes' : allIncludes,
        'path' : path }
    allFilename = addin.rootPath_ + 'Serialization/serialization_all.hpp'
    outputfile.OutputFile(addin, allFilename, addin.copyright_, allBuffer)

    if addin.serializationBase_:
        callBaseIn = "%s::register_in(ia);" % addin.serializationBase_
        callBaseOut = "%s::register_out(oa);" % addin.serializationBase_
    else:
        callBaseIn =''
        callBaseOut =''

    bufferFactory = addin.bufferSerializeRegister_.text() % {
        'addinDirectory' : path,
        'bufferRegister' : bufferRegister,
        'libRootDirectory' : environment.config().libRootDirectory(),
        'namespaceAddin' : addin.namespaceAddin_,
        'callBaseIn': callBaseIn,
        'callBaseOut': callBaseOut}
    factoryFile = addin.rootPath_ + 'Serialization/serializationfactory.cpp'
    outputfile.OutputFile(addin, factoryFile, addin.copyright_, bufferFactory)

