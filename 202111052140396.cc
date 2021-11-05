#include <string.h>
#include <iostream>

using std::cout;
using std::endl;

//设计需求：只能创建栈对象
//解决方法：将operator new/delete设置为私有的

class Student
{
public:
    Student(int id, const char *name)
    : _id(id)
    , _name(new char[strlen(name) + 1]())
    {
        cout << "Student(int, const char *)" << endl;
        strcpy(_name, name);
    }
private:
    void *operator new(size_t sz)
    {
        cout << "void *operator new(size_t)" << endl;

        void *pret = malloc(sz);
        return pret;
    }

    void operator delete(void *pret)
    {
        cout << "void operator delete(void  *)" << endl;

        free(pret);
    }
public:

    void print() const
    {
        cout << "id: " << _id << endl
             << "name: " << _name << endl;
    }

public:
    void A()
    {
        this->~Student();
    }
/* private: */
    ~Student()
    {
        cout << "~Student() " << endl;
        if(_name)
        {
            delete [] _name;
            _name = nullptr;
        }
    }
private:
    int _id;
    char *_name;
};

int main(int argc, char **argv)
{
    Student stu(1201, "Lucy");//栈对象,ok

    /* Student *pstu = new Student(4201, "xiaohong");//堆对象,error */
    /* pstu->print(); */

    /* delete pstu; */
    return 0;
}

