
#including common variable definition, SOURCECODE_ROOT typically
include ../Makefile_commonDef

OPTION = -std=c++11 
ROOTPATH_GSL = $(THIRD_PARTY_LIBS_ROOT)/gsl-2.6
INCLUDE_GSL = $(ROOTPATH_GSL)

#the full .a lib:
STATIC_LIB_GSL = $(ROOTPATH_GSL)/.libs/libgsl.a
STATIC_LIB_CBLAS = $(ROOTPATH_GSL)/cblas/.libs/libgslcblas.a

#-lm : not needed on MacOS X; see https://www.gnu.org/software/gsl/doc/html/usage.html
DYNAMIC_LIB = m

SOURCECODE_SUBDIR_MEANMEDIAN = logic/mathComputation
SOURCECODE_DIR_MEANMEDIAN = $(APP_SOURCECODE_ROOT)/$(SOURCECODE_SUBDIR_MEANMEDIAN)
INCLUDE_DIR_MEANMEDIAN = $(SOURCECODE_DIR_MEANMEDIAN)

FLAGS_CONSOLE_OUT_TYPE = DONT_USE_QDEBUG__USE_COUT_INSTEAD

target_7:
	g++ $(OPTION) -D$(FLAGS_CONSOLE_OUT_TYPE) -c -I$(INCLUDE_GSL) -I$(INCLUDE_DIR_MEANMEDIAN) $(SOURCECODE_DIR_MEANMEDIAN)/initGSL.cpp
	g++ $(OPTION) -D$(FLAGS_CONSOLE_OUT_TYPE) -c -I$(INCLUDE_GSL) -I$(INCLUDE_DIR_MEANMEDIAN) $(SOURCECODE_DIR_MEANMEDIAN)/meanMedian.cpp
	g++ $(OPTION) -D$(FLAGS_CONSOLE_OUT_TYPE) -c -I$(INCLUDE_GSL) -I$(INCLUDE_DIR_MEANMEDIAN) ./test_mean_median_2.cpp
	g++ $(OPTION) -o testmm2 initGSL.o meanMedian.o test_mean_median_2.o $(STATIC_LIB_GSL) $(STATIC_LIB_CBLAS) -l$(DYNAMIC_LIB)
	
clean:	
	rm -rf initGSL.o
	rm -rf meanMedian.o
	rm -rf test_mean_median_2.o
	rm -rf testmm2
