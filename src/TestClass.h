/* ---------------------------------------------------------------------------
| Copyright (c) by David Perez 2014-2015                                         |
| Global_Error, CreationPolicy, GlobalRedis and SmartPtr Classes              |
| License by ApexDataSolutions, LLC                                           |
-----------------------------------------------------------------------------*/
#include "TGlobalContainer.h"
//____________________________________________________________________________//
class Global_Error : public std::exception {
public:
    Global_Error(const std::string & err) : err_(err) {
    }

    virtual ~Global_Error() throw() {
    }

    operator const std::string() const {
	return err_;
    }

    virtual const char* what() const throw() {
	return err_.c_str();
    }

private:
    std::string err_;
};
//____________________________________________________________________________//
//class CreationPolicy Prototype pattern
//____________________________________________________________________________//
template <typename TGlobalBase>
class CreationPolicy{
public:
	CreationPolicy(TGlobalBase* pObj = 0) : pPrototype_(pObj) {
	}

	TGlobalBase* Create() {
	    return pPrototype_ ? pPrototype_->Clone() : 0;
	}

	TGlobalBase* GetPrototype() {
	    return pPrototype_;
	}

	void SetPrototype(TGlobalBase* pObj) {
	    pPrototype_ = pObj;
	}

protected:
    TGlobalBase* pPrototype_;

    ~CreationPolicy() {
    }
};

//____________________________________________________________________________//
template<class T,
   template <class> class CheckingPolicy,
   template <class> class ThreadingModel,
   template <class> class Storage = DefaultSmartPtrStorage >
class SmartPtr : public CheckingPolicy<T>, public ThreadingModel<SmartPtr> {
    // ...
    T* operator->() {
	typename ThreadingModel<SmartPtr>::Lock guard(*this);
	CheckingPolicy<T>::Check(pointee_);
	return pointee_;
    }

private:
    T* pointee_;
};

//____________________________________________________________________________//
template<class T>
class DefaultSmartPtrStorage {
public:
    typedef T* PointerType;
    typedef T& ReferenceType;

protected:
    PointerType GetPointer() {
	return ptr_;
    }

    void SetPointer(PointerType ptr) {
	ptr_ = ptr;
    }

private:
    PointerType ptr_;
};

// ____________________________________________________________________________//
template<class CreatorPolicy, class TSubscript, class TValue>
class TGlobalRedis : public TGlobalBase<CreatorPolicy,  TSubscript,  TValue> {
public:

    TGlobalRedis() {
	aMySet = new std::vector<TGlobalRedis>();
    }

	TGlobalRedis(const CreatorPolicy& pGlobal, const TSubscript& pMySet, const TValue& pTypeRedis)
	: pGlobal_(pGlobal), aMySet_(pMySet), pType_(pTypeRedis) {
    }

    ~TGlobalRedis() {
    }

    // Access methods
    std::vector<T>GetRedisList() {
	return aMySet;
    }

    void SetRedisList(std::vector<T>pMySet) {
	aMySet = pMySet;
    }

    virtual std::string toString() {
	stringstream stm;
	stm << aGlobalName;
	for (int i = 0; i < aSubscripts.size(); ++i) {
	    stm << ": " << aSubscripts[i];
	}
	for (int j = 0; j < aValues.size(); ++j) {
	    stm << ": " << aValues[i];
	}
	for (int i = 0; i < aMySet.size(); ++i) {

	    stm << ": " << aMySet[i];
	}
	stm << ": " << end1;
	return stm.str();
    }

    virtual void draw(std::string pGlobalName, std::vector<T>pSubscripts,
	std::vector<T>pValues) {
	cout << "GlobalRedis : " << pGlobalName << "(" << std::for_each
	    (pSubscripts.begin(), pSubscripts.end()) << ")=(" << std::for_each
	    (pValues.begin(), pValues.end()) << ")" << end1;
    }

    void SwitchPrototype(TGlobalBase* pNewPrototype) {
	CreationPolicy<TGlobalBase>&myPolicy = *this;
	delete myPolicy.GetPrototype();
	myPolicy.SetPrototype(pNewPrototype);
    }

    virtual TGlobalRedis* Clone() const {
	return new TGlobalRedis(this*);
    }

    inline auto GetRedisType(TypeRedis EvalRedis){
        if (EvalRedis::None)
            return None;
        if (EvalRedis::String)
            return String;
        if (EvalRedis::List)
            return List;
        if (EvalRedis::Set)
            return Set;
        if (EvalRedis::ZSet)
            return ZSet;
        if (EvalRedis::Hash)
            return Hash;
    }

private:
    TGlobalBase pGlobal_;

    enum class TypeRedis {
	None, String, List, Set, ZSet, Hash
    };

    TypeRedis pType_;

    std::vector<TGlobalRedis>aMySet_;

    TGlobalRedis(const TGlobalRedis&);

    TGlobalRedis& operator = (const TGlobalRedis&);
};

// ____________________________________________________________________________//
