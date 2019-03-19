/**
 *  \file UFXCException.h
 *  \brief header file
 *  \author
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: June 29 2018
 */

#ifndef UFXCLIB_EXCEPTION_H_
#define UFXCLIB_EXCEPTION_H_

#include <vector>
#include <yat/utils/Logging.h>

/**
 * \namespace ufxclib
 */
namespace ufxclib
{
/**
 * \enum Severity
 * \brief UFXC error severities
 */
typedef enum
{
    WARN, ERR, PANIC

} EnumSeverity;

/**
 * \class Error
 * \brief The UFXC Error abstraction base class.
 * Contains 5 fields:
 * ° reason
 * ° description
 * ° origin
 * ° error code
 * ° severity
 */
class Error
{
public:

    /**
    * \fn Error()
    * \brief default constructor
    * \param none
	* \return none
    */
    Error();

    /**
    * \fn Error(const char *_reason, const char *_desc, const char *_origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR)
    * \brief Constructor with parameters.
    * \param _reason
    * * \param _desc
    * * \param _origin
    * * \param _code
    * * \param _severity
	* \return none
    */
    Error(const char *_reason, const char *_desc, const char *_origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR);

    /**
    * \fn Error(const std::string& _reason, const std::string& _desc, const std::string& _origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR)
    * \brief Constructor with parameters.
    * \param _reason
    * * \param _desc
    * * \param _origin
    * * \param _code
    * * \param _severity
	* \return none
    */
    Error(const std::string& _reason, const std::string& _desc, const std::string& _origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR);

    /**
    * \fn Error(const Error& _src)
    * \brief Copy constructor
    * \param _src
	* \return none
    */
    Error(const Error& _src);

    /**
    * \fn virtual ~Error()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~Error();

    /**
    * \fn Error& operator=(const Error& _src)
    * \brief Operator=
    * \param _src Error object
	* \return reference to an Error object
    */
    Error& operator=(const Error& _src);

    /// Error details: reason
    std::string reason;

    /// Error details: description
    std::string desc;

    /// Error details: origin
    std::string origin;

    /// Error details: code
    int code;

    /// Error details: severity
    ufxclib::EnumSeverity severity;
};

/**
 * The UFXC error list.
 *
 */
typedef std::vector<Error> ErrorList;

/**
 * \class Exception
 * \brief The UFXC Exception abstraction base class.
 *  Contains a list of UFXC Errors.
 */
class Exception
{
public:

    /**
    * \fn Exception()
    * \brief default constructor
    * \param none
	* \return none
    */
    Exception();

    // Constructor with parameters.
    /**
    * \fn Exception(const char *_reason, const char *_desc, const char *_origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR)
    * \brief Constructor with parameters.
    * \param _reason
    * * \param _desc
    * * \param _origin
    * * \param _code
    * * \param _severity
	* \return none
    */
    Exception(const char *_reason, const char *_desc, const char *_origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR);

    /**
    * \fn Exception(const std::string& _reason, const std::string& _desc, const std::string& _origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR)
    * \brief Constructor with parameters.
    * \param _reason
    * * \param _desc
    * * \param _origin
    * * \param _code
    * * \param _severity
	* \return none
    */
    Exception(const std::string& _reason, const std::string& _desc, const std::string& _origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR);

    //
    /**
    * \fn Exception(const Error& error)
    * \brief Constructor from Error class.
    * \param error Error object
	* \return none
    */
    Exception(const Error& error);

    /**
    * \fnException(const Exception& _src)
    * \brief Copy constructor
    * \param _src
	* \return none
    */
    Exception(const Exception& _src);

    /**
    * \fn Exception& operator=(const Exception& _src)
    * \brief Operator=
    * \param _src Error object
	* \return reference to an Exception object
    */
    Exception& operator=(const Exception& _src);

    /**
    * \fn virtual ~Exception()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~Exception();

    /**
    * \fn void push_error(const char *_reason, const char *_desc, const char *_origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR)
    * \brief Pushes the specified error into the errors list.
    * \param _reason
    * \param _desc
    * \param _origin
    * \param _code
    * \param _severity
	* \return void
    */
    void push_error(const char *_reason, const char *_desc, const char *_origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR);

    /**
    * \fn void push_error(const std::string& _reason, const std::string& _desc, const std::string& _origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR)
    * \brief Pushes the specified error into the errors list.
    * \param _reason
    * \param _desc
    * \param _origin
    * \param _code
    * \param _severity
	* \return void
    */
    void push_error(const std::string& _reason, const std::string& _desc, const std::string& _origin, int _code = -1, ufxclib::EnumSeverity _severity = ufxclib::ERR);

    /**
    * \fn void push_error(const Error& _error)
    * \brief Pushes the specified error into the errors list.
    * \param _error an Error object
	* \return void
    */
    void push_error(const Error& _error);

    /// The error list.
    ErrorList errors;
};

} /// namespace ufxclib

#endif /// UFXCLIB_EXCEPTION_H_

