template <typename T>
using SignalCallback = void (*)(T);

template <typename T>
class Signal
{
private:
    T *value;

public:
    SignalCallback<T> callback = NULL;

    Signal(T *value)
    {
        this->value = value;
        this->callback = NULL;
    }

    T getValue()
    {
        return this->value;
    }

    void setValue(T newValue)
    {
        this->value = newValue;
        this->emit();
    }

    void emit()
    {
        if (callback)
        {
            callback(this->value);
        }
    }

    void subscribe(SignalCallback<T> callback)
    {
        this->callback = callback;
    }

    void unSubscribe()
    {
        this->callback = NULL;
    }
};