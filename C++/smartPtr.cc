#include <bits/stdc++.h>
using namespace std;

template<typename T>
class RefCnt{
public:
    RefCnt(T* ptr = nullptr)
        : mptr(ptr)
    {
        if(mptr != nullptr) mcount = 1;
    }
    void addRef() { mcount ++; } 
    int delRef() { return -- mcount; }
private:
    T* mptr;
    int mcount;   //shared_ptr 和 weak_ptr实现了线程安全，这里使用的是autmic_int
};

template<typename T>
class CSmartPtr {
public:
    CSmartPtr(T* ptr == nullptr)
        : mptr(ptr)
    {
        mpRefCnt = new RefCnt<T>(mptr);
    }
    ~CSmartPtr()
    {
        if(0 == mpRefCnt->delRef()){
            delete mptr;
            mptr = nullptr;
        }
    }

    T& operator* (){ return *mptr; }
    T* operator->(){ return mptr; }

    CSmartPtr(const CSmartPtr<T> &src)
        :mptr(src.mptr), mpRefCnt(src.mpRefCnt)
    {
        if(mptr != nullptr)
            mpRefCnt->addRef();
    }
    CSmartPtr<T>& operator=(const CSmartPtr<T> &src)
    {
        if(this == &src)
            return *this;

        if(0 == mpRefCnt->delRef()){
            delete mptr;
        }

        mptr = src.mptr;
        mpRefCnt = src.mpRefCnt;
        mpRefCnt->addRef();
    }
private:
    T* mptr;
    RefCnt<T> *mpRefCnt;
};

int main(){
    CSmartPtr<int> ptr1(new int);
    CSmartPtr<int> ptr2(ptr1);
}