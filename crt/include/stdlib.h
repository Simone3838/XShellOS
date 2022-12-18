// stdlib.h
// Copyright (C) 2022 iProgramInCpp
// The NanoShell Standard C Library
#ifndef _STDLIB__H
#define _STDLIB__H

#include <nanoshell/stdlib_types.h>

// Numeric conversion functions
int    atoi       (const char* str);
int    atox       (const char* str);
double atof       (const char *arr);
char*  itoa       (int value, char* buffer, int radix);
char*  ltoa       (long value, char* buffer, int radix);

// Memory management
void* malloc (size_t size);
void* calloc (size_t nmemb, size_t size);
void  free   (void*  ptr);
void* realloc(void*  ptr, size_t sz);

// Arithmetic utilities
int abs(int k);
double fabs(double x);

// Random number generator
int GetRandom(); // use the kernel RNG
int rand();
int rand_r(unsigned int * seed);
void srand(unsigned int seed);

#endif//_STDLIB__H