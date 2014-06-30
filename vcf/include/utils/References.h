//References.h

class RefCount {    

public:
    RefCount(){ 
		m_refCount = 0; 
	};

    virtual ~RefCount() { 
		//TRACE("goodbye(%d)\n", crefs); 
	};

    void addRef() { 
		++this->m_refCount;
	};

    void removeRef(){
		if (--m_refCount == 0){
			delete this;
		}
		else{
			
		}
	};

private:
	int m_refCount;
};

//template <class T> itNew
