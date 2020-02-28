class CallBack
{
   virtual callMeBack () {};
};

class Caller : public CallBack
{
    void DoSomething ()
    {
    }

    void callMeBack()
    {
       std::cout << "I got your message" << std::endl;
    }
};

class AnotherClass ()
{
     public void RegisterMe(CallBack *callback)
     {
         m_callback = callback;
     }

     public void DoSomething ()
     {
        // DO STUFF
        // .....
        // then call
        if (m_callback) m_callback->callMeBack();
     }
     private CallBack *m_callback = NULL;
};

class Callbacker
{
    public:
        Callbacker(void) {
            callback = NULL;
            callback_obj = NULL;
            object = NULL;
        };
        ~Callbacker(void);

        void setCallback(void (*cb)(int)) {
            callback_obj = NULL;
            object = NULL;
            callback = cb;
        };
        void setCallback(void (*cb)(void*,int),void* obj) {
            callback = NULL;
            object = obj;
            callback_obj = cb;
        };
        void call() {
            if(callback != NULL)
            {
                callback(9001);
            }
            else if((callback_obj != NULL) && (object != NULL))
            {
                callback_obj(object,9002);
            }
        };

    private:
        void (*callback)(int);
        void (*callback_obj)(void*,int);
        void* object;

};
