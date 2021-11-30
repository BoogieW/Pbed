 ///
 /// @file    Observer.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2021-11-29 16:07:28
 ///
 
#include "Observer.hpp" //第一个头文件必须是自己相应的头文件

#include <stdio.h>  //C标准库的头文件

#include <iostream> //C++标准库头文件

//#include <log4cpp/Category.hh> //第三方库头文件

using std::cout;
using std::endl;
 

void Baby::update()
{
	//听到铃声之后的反应(70%的概率睡觉，30%的概率哭)
	::srand(::clock());
	int number = ::rand() % 100;
	if(number < 70) {
		cout << ">> Baby " << _name << " is sleeping sweetly!" << endl;
	} else {
		cout << ">> Baby " << _name << " is crying loundly!" << endl;
	}
}

void Nurse::update()
{
	//听到铃声之后的反应(70%的概率去开门，30%的概率睡觉)
	::srand(::clock());
	int number = ::rand() % 100;
	if(number < 70) {
		cout << ">> Nurse " << _name << " is opening the door!" << endl;
	} else {
		cout << ">> Nurse " << _name << " is sleeping sweetly!" << endl;
	}
}
