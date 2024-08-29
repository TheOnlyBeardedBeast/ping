template <typename Key, typename Value>
class KeyMap
{
private:
    struct Entry
    {
        Key key;
        Value value;
        Entry *next;
    };

    Entry **table;
    int numBuckets;
    int size;

    int hash(const Key &key)
    {
        int hashCode = 0;
        for (const char &c : reinterpret_cast<const char *>(&key))
        {
            hashCode += c;
        }
        return hashCode % numBuckets;
    }

public:
    AssociativeArray(int numBuckets = 128) : numBuckets(numBuckets), size(0)
    {
        table = new Entry *[numBuckets];
        for (int i = 0; i < numBuckets; i++)
        {
            table[i] = nullptr;
        }
    }

    ~AssociativeArray()
    {
        for (int i = 0; i < numBuckets; i++)
        {
            Entry *current = table[i];
            while (current != nullptr)
            {
                Entry *next = current->next;
                delete current;
                current = next;
            }
        }
        delete[] table;
    }

    void insert(const Key &key, const Value &value)
    {
        int bucketIndex = hash(key);
        Entry *current = table[bucketIndex];
        while (current != nullptr)
        {
            if (current->key == key)
            {
                current->value = value;
                return;
            }
            current = current->next;
        }
        Entry *newEntry = new Entry;
        newEntry->key = key;
        newEntry->value = value;
        newEntry->next = table[bucketIndex];
        table[bucketIndex] = newEntry;
        size++;
    }

    Value get(const Key &key)
    {
        int bucketIndex = hash(key);
        Entry *current = table[bucketIndex];
        while (current != nullptr)
        {
            if (current->key == key)
            {
                return current->value;
            }
            current = current->next;
        }

        return NULL;
    }

    void remove(const Key &key)
    {
        int bucketIndex = hash(key);
        Entry *current = table[bucketIndex];
        Entry *previous = nullptr;
        while (current != nullptr)
        {
            if (current->key == key)
            {
                if (previous == nullptr)
                {
                    table[bucketIndex] = current->next;
                }
                else
                {
                    previous->next = current->next;
                }
                delete current;
                size--;
                return;
            }
            previous = current;
            current = current->next;
        }
    }

    bool contains(const Key &key)
    {
        int bucketIndex = hash(key);
        Entry *current = table[bucketIndex];
        while (current != nullptr)
        {
            if (current->key == key)
            {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    int getSize()
    {
        return size;
    }

    Value &operator[](const Key &key)
    {
        int bucketIndex = hash(key);
        Entry *current = table[bucketIndex];
        while (current != nullptr)
        {
            if (current->key == key)
            {
                return current->value;
            }
            current = current->next;
        }
        // Key not found, create a new entry
        Entry *newEntry = new Entry;
        newEntry->key = key;
        newEntry->value = Value(); // default value
        newEntry->next = table[bucketIndex];
        table[bucketIndex] = newEntry;
        size++;
        return newEntry->value;
    }
};
