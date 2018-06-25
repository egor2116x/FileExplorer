#pragma once
#include "stdafx.h"
#include "Common.h"
    
class UserInputException : public std::exception
{
public:
    UserInputException(ExceptionErrorTypes code, const char * const message) :
        exception(message), m_code(code) {}
    ExceptionErrorTypes GetErrorCode() const { return m_code; }
private:
    ExceptionErrorTypes m_code;
};

class ThreadExeption : public std::exception
{
public:
    ThreadExeption(ExceptionErrorTypes code, const char * const message) :
        exception(message), m_code(code) {}
    ExceptionErrorTypes GetErrorCode() const { return m_code; }
private:
    ExceptionErrorTypes m_code;
};

class FileExeption : public std::exception
{
public:
    FileExeption(ExceptionErrorTypes code, const char * const message) :
        exception(message), m_code(code) {}
    ExceptionErrorTypes GetErrorCode() const { return m_code; }
private:
    ExceptionErrorTypes m_code;
};