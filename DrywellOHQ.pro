

QT -= gui
QT += core widgets

CONFIG += console
CONFIG -= app_bundle

CONFIG += c++14

CONFIG += Arash
DEFINES += Arash

Arash {
    OHQPATH = /home/arash/Projects/QAquifolium/aquifolium
    VTKBUILDPATH = /home/arash/Projects/VTK/VTK-build
    VTKHEADERPATH = /home/arash/Projects/VTK
    VTK_V = -9.0
}


INCLUDEPATH += $$OHQPATH/include
INCLUDEPATH += $$OHQPATH/src
INCLUDEPATH += $$OHQPATH/include/GA
INCLUDEPATH += $$OHQPATH/include/MCMC
INCLUDEPATH += ../jsoncpp/include/



if==macx:CONFIG += staticlib
macx: DEFINES +=mac_version
linux: DEFINES +=ubuntu_version
win32: DEFINES +=windows_version

DEFINES += Terminal_version

TARGET = OpenHydroQual-Console
TEMPLATE = app
win32:QMAKE_CXXFLAGS += /MP

macx: {
    QMAKE_CXXFLAGS += -Xpreprocessor -fopenmp -lomp -Iusr/local/lib/
}

macx: {
    QMAKE_LFLAGS += -lomp
}

macx: {
    LIBS += -L /usr/local/lib /usr/local/lib/libomp.dylib
}

macx: {
    INCLUDEPATH += /usr/local/include/
}


CONFIG(debug, debug|release) {
    message(Building in debug mode)
    !macx: QMAKE_CXXFLAGS *= "-Xpreprocessor -fopenmp"
    !macx: QMAKE_LFLAGS +=  -fopenmp
    !macx: LIBS += -lgomp -lpthread
    LIBS += -lpthread
    DEFINES += NO_OPENMP DEBUG

} else {
    message(Building in release mode)
    !macx: QMAKE_CXXFLAGS *= "-Xpreprocessor -fopenmp"
    !macx: QMAKE_LFLAGS +=  -fopenmp
    # QMAKE_CFLAGS+=-pg
    # QMAKE_CXXFLAGS+=-pg
    # QMAKE_LFLAGS+=-pg
    # macx: DEFINES += NO_OPENMP
    ! macx: LIBS += -lgomp -lpthread
    macx: LIBS += -lpthread
}




SOURCES += \
        $$OHQPATH/src/Block.cpp \
        $$OHQPATH/src/Command.cpp \
        $$OHQPATH/src/Condition.cpp \
        $$OHQPATH/src/ErrorHandler.cpp \
        $$OHQPATH/src/Expression.cpp \
        $$OHQPATH/src/Link.cpp \
        $$OHQPATH/src/Matrix.cpp \
        $$OHQPATH/src/Matrix_arma.cpp \
        $$OHQPATH/src/MetaModel.cpp \
        $$OHQPATH/src/NormalDist.cpp \
        $$OHQPATH/src/Object.cpp \
        $$OHQPATH/src/Objective_Function.cpp \
        $$OHQPATH/src/Objective_Function_Set.cpp \
        $$OHQPATH/src/Parameter.cpp \
        $$OHQPATH/src/Parameter_Set.cpp \
        $$OHQPATH/src/Precipitation.cpp \
        $$OHQPATH/src/Quan.cpp \
        $$OHQPATH/src/QuanSet.cpp \
        $$OHQPATH/src/QuickSort.cpp \
        $$OHQPATH/src/Rule.cpp \
        $$OHQPATH/src/RxnParameter.cpp \
        $$OHQPATH/src/Script.cpp \
        $$OHQPATH/src/Source.cpp \
        $$OHQPATH/src/System.cpp \
        $$OHQPATH/src/Utilities.cpp \
        $$OHQPATH/src/Vector.cpp \
        $$OHQPATH/src/Vector_arma.cpp \
        $$OHQPATH/src/constituent.cpp \
        $$OHQPATH/src/observation.cpp \
        $$OHQPATH/src/precalculatedfunction.cpp \
        $$OHQPATH/src/reaction.cpp \
        $$OHQPATH/src/restorepoint.cpp \
        $$OHQPATH/src/solutionlogger.cpp \
        $$OHQPATH/src/GA/Binary.cpp \
        $$OHQPATH/src/GA/Individual.cpp \
        $$OHQPATH/src/GA/DistributionNUnif.cpp \
        $$OHQPATH/src/GA/Distribution.cpp \
        ../jsoncpp/src/lib_json/json_reader.cpp \
        ../jsoncpp/src/lib_json/json_value.cpp \
        ../jsoncpp/src/lib_json/json_writer.cpp \
        main.cpp \
        modelcreator.cpp

HEADERS += \
    $$OHQPATH/include/Objective_Function.h \
    $$OHQPATH/include/Objective_Function_Set.h \
    $$OHQPATH/include/Precipitation.h \
    $$OHQPATH/include/RxnParameter.h \
    $$OHQPATH/include/constituent.h \
    $$OHQPATH/include/observation.h \
    $$OHQPATH/include/precalculatedfunction.h \
    $$OHQPATH/include/solutionlogger.h \
    $$OHQPATH/include/GA/GA.h \
    $$OHQPATH/include/MCMC/MCMC.h \
    $$OHQPATH/include/MCMC/MCMC.hpp \
    $$OHQPATH/include/Utilities.h \
    $$OHQPATH/include/restorepoint.h \
    $$OHQPATH/include/safevector.h \
    $$OHQPATH/include/safevector.hpp \
    $$OHQPATH/include/Block.h \
    $$OHQPATH/include/BTC.h \
    $$OHQPATH/include/BTCSet.h \
    $$OHQPATH/include/Expression.h \
    $$OHQPATH/include/Link.h \
    $$OHQPATH/include/Matrix.h \
    $$OHQPATH/include/Matrix_arma.h \
    $$OHQPATH/include/MetaModel.h \
    $$OHQPATH/include/NormalDist.h \
    $$OHQPATH/include/Object.h \
    $$OHQPATH/include/Quan.h \
    $$OHQPATH/include/QuanSet.h \
    $$OHQPATH/include/QuickSort.h \
    $$OHQPATH/include/StringOP.h \
    $$OHQPATH/include/System.h \
    $$OHQPATH/include/Vector.h \
    $$OHQPATH/include/Vector_arma.h \
    ../jsoncpp/include/json/allocator.h \
    ../jsoncpp/include/json/assertions.h \
    ../jsoncpp/include/json/autolink.h \
    ../jsoncpp/include/json/config.h \
    ../jsoncpp/include/json/features.h \
    ../jsoncpp/include/json/forwards.h \
    ../jsoncpp/include/json/json.h \
    ../jsoncpp/include/json/reader.h \
    ../jsoncpp/include/json/value.h \
    ../jsoncpp/include/json/version.h \
    ../jsoncpp/include/json/writer.h \
    ../jsoncpp/src/lib_json/json_tool.h \
    ../jsoncpp/src/lib_json/version.h.in \
    $$OHQPATH/include/Parameter.h \
    $$OHQPATH/include/Parameter_Set.h \
    $$OHQPATH/include/Command.h \
    $$OHQPATH/include/Script.h \
    $$OHQPATH/include/GA/Binary.h \
    $$OHQPATH/include/GA/Distribution.h \
    $$OHQPATH/include/GA/DistributionNUnif.h \
    $$OHQPATH/include/GA/Individual.h \
    $$OHQPATH/include/Objective_Function.h \
    $$OHQPATH/include/Objective_Function_Set.h \
    $$OHQPATH/include/GA/GA.hpp \
    $$OHQPATH/src/BTC.hpp \
    $$OHQPATH/src/BTCSet.hpp \
    $$OHQPATH/include/reaction.h \
    modelcreator.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# LAPACK — Linear Algebra PACKage lib and include locations

win32 {

    LAPACK_INCLUDE = $$PWD/include
    #64 bits build
    contains(QMAKE_TARGET.arch, x86_64) {
        #debug
        CONFIG(debug, debug|release) {
            LAPACK_LIB_DIR = $$PWD/libs/lapack-blas_lib_win64/debug
            LIBS +=  -L$${LAPACK_LIB_DIR} -llapack_win64_MTd \
                    -lblas_win64_MTd
        }
        #release
        CONFIG(release, debug|release) {
            LAPACK_LIB_DIR = $$PWD/libs/lapack-blas_lib_win64/release
            LIBS +=  -L$${LAPACK_LIB_DIR} -llapack_win64_MT \
                    -lblas_win64_MT
        }
    }

    INCLUDEPATH += $${LAPACK_INCLUDE}

    DEFINES += ARMA_USE_LAPACK ARMA_USE_BLAS

}

linux {
    #sudo apt-get install libblas-dev liblapack-dev
     DEFINES += ARMA_USE_LAPACK ARMA_USE_BLAS
     LIBS += -larmadillo -llapack -lblas -lgsl
}

macx {

    LIBS += /opt/homebrew/Cellar/armadillo/11.4.2/lib/libarmadillo.dylib
    INCLUDEPATH += $$PWD/../../../../opt/homebrew/Cellar/armadillo/11.4.2/include
    DEPENDPATH += $$PWD/../../../../opt/homebrew/Cellar/armadillo/11.4.2/include

}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    $$OHQPATH/src/BTC.hpp \
    $$OHQPATH/src/BTCSet.hpp



