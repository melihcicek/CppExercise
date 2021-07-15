# CppExercise
<p> Vector.h is a kind of std::vector's mimic </p>
<p> Allocator.h is a fixed size custom allocator class</p>
<p> FactoryClass.h is a General purpose factory class</p>

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

int main()
{
    mc::general_factory<image> img_fac;

    img_fac.register_new_type<bmpimage>("bmp", "manzara"s);
    img_fac.register_new_type<jpegimage>("jpg");

    auto bimg = img_fac.get_instance("bmp");
    bimg.get()->print_img();

    auto jimg = img_fac.get_instance("jpg");
    jimg.get()->print_img();

    try
    {
        auto xx = img_fac.get_instance("xxx");
    }
    catch (const std::exception& ex)
    {
        cout << ex.what();
    }

}

```

