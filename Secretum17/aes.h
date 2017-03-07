#pragma once
#ifndef AES_H
#define AES_H

#include "encryption_AL.h"

class AES : public Block_AL {
public:
	AES() { cout << "AES" << endl; };
	virtual ~AES() {};
};

#endif 