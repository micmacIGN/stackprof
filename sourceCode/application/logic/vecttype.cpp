#include "vecttype.h"

#include <vector>

using namespace std;

//inheritance way with virtual
VectorType::~VectorType() { }



void VectorUint8::resize(uint32_t count) { _vectUint8.resize(count); /*cout << "_vectUint8.data() @" << (void*)_vectUint8.data() << endl;*/}

void* VectorUint8::data() { return _vectUint8.data();}

void VectorUint8::clear() { _vectUint8.clear();}

bool VectorUint8::empty() { return _vectUint8.empty();}

size_t VectorUint8::size() { return(_vectUint8.size()); }

void VectorUint8::fillZero() { fill(_vectUint8.begin(), _vectUint8.end(), 0); }

VectorUint8::~VectorUint8() { _vectUint8.clear(); }



void VectorFloat::resize(uint32_t count) { _vectFloat.resize(count); /*cout << "_vectFloat @" << (void*)_vectFloat.data() << endl;*/ }

void* VectorFloat::data() { return _vectFloat.data();}

void VectorFloat::clear() { _vectFloat.clear();}

bool VectorFloat::empty() { return _vectFloat.empty();}

size_t VectorFloat::size() { return(_vectFloat.size()); }

void VectorFloat::fillZero() { fill(_vectFloat.begin(), _vectFloat.end(), 0.0); }

VectorFloat::~VectorFloat() { _vectFloat.clear(); }



void VectorSignedInt16::resize(uint32_t count) { _vectSignedInt16.resize(count); }

void* VectorSignedInt16::data() { return _vectSignedInt16.data(); }

void VectorSignedInt16::clear() { _vectSignedInt16.clear(); }

bool VectorSignedInt16::empty() { return _vectSignedInt16.empty(); }

size_t VectorSignedInt16::size() { return(_vectSignedInt16.size()); }

void VectorSignedInt16::fillZero() { fill(_vectSignedInt16.begin(), _vectSignedInt16.end(), 0); }

VectorSignedInt16::~VectorSignedInt16() { _vectSignedInt16.clear(); }



void VectorUnsignedInt16::resize(uint32_t count) { _vectUnsignedInt16.resize(count); }

void* VectorUnsignedInt16::data() { return _vectUnsignedInt16.data(); }

void VectorUnsignedInt16::clear() { _vectUnsignedInt16.clear(); }

bool VectorUnsignedInt16::empty() { return _vectUnsignedInt16.empty(); }

size_t VectorUnsignedInt16::size() { return(_vectUnsignedInt16.size()); }

void VectorUnsignedInt16::fillZero() { fill(_vectUnsignedInt16.begin(), _vectUnsignedInt16.end(), 0); }

VectorUnsignedInt16::~VectorUnsignedInt16() { _vectUnsignedInt16.clear(); }

