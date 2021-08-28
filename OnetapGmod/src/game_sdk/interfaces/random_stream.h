#pragma once

class c_uniform_random_stream
{
public:
	/*0*/	virtual void set_seed(int) = 0;
	/*1*/	virtual float random_float(float, float) = 0;
	/*2*/	virtual int random_int(int, int) = 0;
	/*3*/	virtual float random_float_exp(float, float, float) = 0;
};