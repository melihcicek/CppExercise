# CppExercise
<p> Vector.h is a kind of std::vector's mimic </p>
<p> Allocator.h is a fixed size custom allocator class</p>
<p> FactoryClass.h is a General purpose factory class class</p>
```cpp
struct image {
    virtual void print_img() = 0;
};

struct bmpimage : image {
    string m_name = "no name";
    bmpimage(string s) : m_name{ s } { }
    bmpimage() {}
    
    void print_img() override
    {
        cout << "view bmp "<< m_name <<"\n";
    }
};

struct jpegimage : image {
    void print_img() override
    {
        cout << "view jpg\n";
    }
};
```
