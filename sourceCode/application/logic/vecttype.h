#ifndef VECTTYPE_H
#define VECTTYPE_H

#include <vector>

#include <stddef.h>
#include <memory>

using namespace std;

//inheritance way with virtual
class VectorType {
  public:
    virtual void resize(uint32_t count) = 0; //@LP:because size_type is vector<T> dependant, we can not use size_type as method parameter here, and then in the children :|

    virtual void* data() = 0;
    virtual void clear() = 0;
    virtual bool empty() = 0;
    //virtual void showContent() = 0;

    virtual size_t size() = 0;

    virtual void fillZero() = 0;

    virtual ~VectorType();
};

class VectorUint8 : public VectorType {

  public:
    //void resize(vector<char>::size_type count) { _vectChar.resize(count); }
    void resize(uint32_t count);
    void* data();
    void clear();
    bool empty();
    //void showContent() { for (auto iter: _vectChar) cout << (int)iter << endl; }

    //char at(uint32_t idx);
    size_t size();

    void fillZero();

    ~VectorUint8();

  private:
    vector<unsigned char> _vectUint8;
};

class VectorFloat : public VectorType {

  public:
    //void resize(vector<float>::size_type count) { _vectFloat.resize(count); }
    void resize(uint32_t count);
    void* data();
    void clear();
    bool empty();
    //void showContent() { for (auto iter: _vectFloat) cout << (float)iter << endl; }

    //float at(uint32_t idx);

    size_t size();

    void fillZero();

    ~VectorFloat();

  private:
    vector<float> _vectFloat;
};

//@#LP check that 16 bits is constant on the different platforms and compilers
class VectorSignedInt16 : public VectorType {

  public:
    //void resize(vector<float>::size_type count) { _vectFloat.resize(count); }
    void resize(uint32_t count);
    void* data();
    void clear();
    bool empty();
    //void showContent() { for (auto iter: _vectFloat) cout << (float)iter << endl; }

    // /*un*/signed short int at(uint32_t idx);
    size_t size();

    void fillZero();

    ~VectorSignedInt16();
  private:
    vector<signed short int> _vectSignedInt16; //@#LP check that 16 bits is constant on the different platforms and compilers
};

//@#LP check that 16 bits is constant on the different platforms and compilers
class VectorUnsignedInt16 : public VectorType {

  public:
    //void resize(vector<float>::size_type count) { _vectFloat.resize(count); }
    void resize(uint32_t count);
    void* data();
    void clear();
    bool empty();
    //void showContent() { for (auto iter: _vectFloat) cout << (float)iter << endl; }

    // /*un*/signed short int at(uint32_t idx);
    size_t size();

    void fillZero();

    ~VectorUnsignedInt16();
  private:
    vector<unsigned short int> _vectUnsignedInt16; //@#LP check that 16 bits is constant on the different platforms and compilers
};

#endif // VECTTYPE_H
