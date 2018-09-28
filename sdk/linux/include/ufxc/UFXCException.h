//=============================================================================
// UFXCException.h
//=============================================================================
// abstraction.......UFXC Application Programming Interface
// class.............UFXC Error & Exception specification
// original author...NEXEYA
//=============================================================================

#ifndef _UFXC_EXCEPTION_H_
#define _UFXC_EXCEPTION_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <vector>
#include <yat/utils/Logging.h>

namespace ufxclib {

// ============================================================================
// UFXC error severities 
// ============================================================================
typedef enum {
  WARN, 
  ERR, 
  PANIC
} Severity;

// ============================================================================
// The UFXC Error abstraction base class.  
//
// Contains 5 fields:
// ° reason
// ° description
// ° origin
// ° error code
// ° severity
// ============================================================================
class Error
{
public:

  // Constructor. 
  Error ();

  // Constructor with parameters.
  Error (const char *_reason,
				 const char *_desc,
				 const char *_origin,
	       int _code = -1, 
	       ufxclib::Severity _severity = ufxclib::ERR);
  

  // Constructor with parameters.
  Error (const std::string& _reason,
				 const std::string& _desc,
				 const std::string& _origin, 
	       int _code = -1, 
	       ufxclib::Severity _severity = ufxclib::ERR);

  // Copy constructor.
  Error (const Error& _src);

  // Destructor.
  virtual ~Error ();

  // Operator=
  Error& operator= (const Error& _src);

  // Error details: reason
  std::string reason;

  // Error details: description
  std::string desc;

  // Error details: origin
  std::string origin;

  // Error details: code
  int code;

  // Error details: severity
  ufxclib::Severity severity;
};

// ============================================================================
// The UFXC error list.	
// ============================================================================
typedef std::vector<Error> ErrorList;

// ============================================================================
// The UFXC Exception abstraction base class.  
//  
// Contains a list of UFXC Errors.
// 
// ============================================================================
class Exception
{
public:

  // Constructor.
  Exception ();

  // Constructor with parameters.
  Exception (const char *_reason,
					   const char *_desc,
					   const char *_origin,
	           int _code = -1, 
	           ufxclib::Severity _severity = ufxclib::ERR);
  
  // Constructor with parameters.
  Exception (const std::string& _reason,
					   const std::string& _desc,
					   const std::string& _origin, 
	           int _code = -1, 
	           ufxclib::Severity _severity = ufxclib::ERR);

  // Constructor from Error class.
  Exception (const Error& error);


  // Copy constructor.
  Exception (const Exception& _src);

  // Operator=
  Exception& operator= (const Exception& _src); 

  // Destructor.
  virtual ~Exception ();

  // Pushes the specified error into the errors list.
  void push_error (const char *_reason,
					         const char *_desc,
						       const char *_origin, 
		               int _code = -1, 
		               ufxclib::Severity _severity = ufxclib::ERR);

  // Pushes the specified error into the errors list.
  void push_error (const std::string& _reason,
                   const std::string& _desc,
                   const std::string& _origin, 
                   int _code = -1, 
                   ufxclib::Severity _severity = ufxclib::ERR);

  // Pushes the specified error into the errors list.
  void push_error (const Error& _error);

  // The error list.
  ErrorList errors;
};

} // namespace ufxclib

#endif // _UFXC_EXCEPTION_H_

