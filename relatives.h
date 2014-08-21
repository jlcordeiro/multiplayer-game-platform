#ifndef RELATIVES_H
#define RELATIVES_H

template <class T>
class Relatives {
public:
    typedef map<int, shared_ptr<T>> Container;

    Relatives(long unsigned int max_relatives)
        : _max(max_relatives),
          _size(0)
    {
    }

    void limit(long unsigned value)
    {
        _max = value;
    }

    bool add(shared_ptr<T> e)
    {
        auto contains = [&] () { return _relatives.find(e->getId()) != _relatives.end(); };

        if (contains() || _size >= _max) {
            return false;
        }

        _relatives[e->getId()] = e;
        _size++;
        return true;
    }

    bool remove(shared_ptr<T> e)
    {
        const auto n_removed = _relatives.erase(e->getId());
        _size -= n_removed;
        return (n_removed > 0);
    }

    const Container& get() const
    {
        return _relatives;
    }

private:
    Container _relatives;
    long unsigned _max;
    long unsigned _size;
};

#endif
