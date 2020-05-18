//---------------------------------------------------------------------------
#ifndef DCallbackHPP
#define DCallbackHPP
//---------------------------------------------------------------------------
#include <cstdint>

/*
// ---- Classe che chiama: MyClass ------------

GlobalCallback *callback;

//! Registra la callback
void MyClass::SetCallback(Callback::DCALLBACK_FUNC clbk) {
	callback=new GlobalCallback(clbk);
}

//! Esegue la chiamata di callback
void MyClass::DoCallback(uint8_t Command, void* Param) {
	if (callback == NULL) {
		return;
	}
	callback->call(Command, Param);
}
// --------------------------------------------

// ---- Main -------
void DProdigCallback(uint8_t Command, void *Param)
{
	// Handle callback
}

int main {
	MyClass myClass=new MyClass();
	myCass->SetCallback((DCallback::Callback::DCALLBACK_FUNC) DProdigCallback);
}

*/


// Abstract interface for all callbacks
class DCallback {
	public:
		typedef void (*DCALLBACK_FUNC)(uint8_t Command, void* Param);
		//virtual ~Callback() { }
		//virtual Callback* clone() const = 0;
		virtual void call(uint8_t, void*) = 0;
};

// Implementation for member functions
template <typename T>
class DClassCallback : public DCallback {
	private:
		T*      object;
		void (T::*callback)();

	public:
		DClassCallback(T* obj, void (T::*clbk)()) : object(obj), callback(clbk) {}
		virtual DCallback* clone() const { return new DClassCallback<T>(object,callback); }
		virtual void call() { (object->*callback)(); }
};

// Implementation for global functions
class DGlobalCallback : public DCallback {
	private:
		//void (*callback)();
		DCALLBACK_FUNC callback;

	public:
		DGlobalCallback(DCALLBACK_FUNC clbk) : DCallback() {
			callback=clbk;
		}
		virtual void call(uint8_t Command, void* Data) {
			(*callback)(Command,Data);
		}
};
#endif