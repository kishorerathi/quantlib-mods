'output C source files'

import common
import utils

# constants

ROOT = common.ADDIN_ROOT + 'C/'
INCLUDES = 'stub.C.includes'
BODY = 'stub.C.body'

def generateFuncHeader(fileHeader, function, suffix):
    'generate source for prototype of given function'
    fileHeader.write('int %s(\n' % function[common.NAME])
    if function[common.CTOR]:
        fileHeader.write('        char* handle,\n')
    fileHeader.write(utils.generateParamList(function[common.PARAMS],
        2, True, convertString = 'char*', arrayCount = True,
        convertMatStr = 'char*'))
    fileHeader.write(',\n        VariesList *result)%s\n' % suffix)

def generateFuncHeaders(groupName, functionGroup):
    'generate source for function prototypes'
    fileName = ROOT + groupName + '.h' + common.TEMPFILE
    fileHeader = file(fileName, 'w')
    utils.printHeader(fileHeader)
    fileHeader.write('#ifndef qla_%s_h\n' % groupName)
    fileHeader.write('#define qla_%s_h\n\n' % groupName)
    for function in functionGroup[common.FUNCLIST]:
        generateFuncHeader(fileHeader, function, ';\n')
    fileHeader.write('#endif\n\n')
    fileHeader.close()
    utils.updateIfChanged(fileName)

def generateConversions(paramList):
    'generate code to convert arrays to vectors/matrices'
    ret = ''
    indent = 8 * ' ';
    bigIndent = 12 * ' ';
    for param in paramList:
        if param[common.TYPE] == common.STRING:
            type = 'std::string'
        else:
            type = param[common.TYPE]
        if param[common.TENSOR] == common.VECTOR: 
            nmArray = param[common.NAME] + 'Vector'
            nmSize = param[common.NAME] + 'Size'
            ret += indent + 'std::vector < ' + type + ' >' + nmArray + '\n' \
                + bigIndent + '= Conversion< ' + type + ' >::' \
                + 'convertVector(' + param[common.NAME] \
                + ', ' + nmSize + ');\n'
        elif param[common.TENSOR] == common.MATRIX: 
            nmMatrix = param[common.NAME] + 'Matrix'
            nmRows = param[common.NAME] + 'Rows'
            nmCols = param[common.NAME] + 'Cols'
            ret += indent + 'std::vector < std::vector < ' + type + ' > >' \
                + nmMatrix + '\n' \
                + bigIndent + '= Conversion< ' + type + ' >::' \
                + 'convertMatrix(' + param[common.NAME] \
                + ', ' + nmRows + ', ' + nmCols + ');\n'            
    return ret

def generateFuncDefs(groupName, functionGroup):
    'generate source for function implementations'
    fileName = ROOT + groupName + '.cpp' + common.TEMPFILE
    fileFunc = file(fileName, 'w')
    utils.printHeader(fileFunc)
    bufInclude = utils.loadBuffer(INCLUDES)
    bufBody = utils.loadBuffer(BODY)
    fileFunc.write(bufInclude % groupName)
    for function in functionGroup[common.FUNCLIST]:
        generateFuncHeader(fileFunc, function, ' {')
        conversions = generateConversions(function[common.PARAMS])
        if function[common.CTOR]:
            args = utils.generateArgList(function[common.PARAMS])
            fName = 'OH_OBJECT_MAKE(QuantLibAddin::%s)' % function[common.QLFUNC]
            handle = 'handle, args'
            paramList = ''
        else:
            args = ''
            fName = 'QuantLibAddin::' + function[common.NAME]
            handle = '\n'
            paramList = utils.generateParamList(function[common.PARAMS], 
                3, convertString = 'char*', arrayCount = True, convertMatStr = 'char*')
        fileFunc.write(bufBody % (conversions, args, fName, handle,
            paramList, function[common.NAME]))
    fileFunc.close()
    utils.updateIfChanged(fileName)

def generate(functionDefs):
    'generate source code for C addin'
    utils.logMessage('  begin generating C ...')
    functionGroups = functionDefs[common.FUNCGROUPS]
    for groupName in functionGroups.keys():
        functionGroup = functionGroups[groupName]
        if functionGroup[common.HDRONLY]:
            continue
        generateFuncHeaders(groupName, functionGroup)
        generateFuncDefs(groupName, functionGroup)
    utils.logMessage('  done generating C.')

