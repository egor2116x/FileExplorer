#pragma once
#include "stdafx.h"
#include "Common.h"
    
class UserInputException : public std::exception
{
public:
    UserInputException(ERROR_CODE code, const char * const message) :
        exception(message), m_code(code) {}
    ERROR_CODE GetErrorCode() const { return m_code; }
private:
    ERROR_CODE m_code;
};

 