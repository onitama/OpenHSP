////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2007 by Rich Sposato
//
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#ifndef LOKI_CHECK_RETURN_INC_
#define LOKI_CHECK_RETURN_INC_

// $Id$


#include <assert.h>
#include <stdio.h>
#include <stdexcept>


namespace Loki
{

// ----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
///  \class CheckReturn
///
///  \par Purpose
///  C++ provides no mechanism within the language itself to force code to
///  check the return value from a function call.  This simple class provides
///  a mechanism by which programmers can force calling functions to check the
///  return value.  Or at least make them consciously choose to disregard the
///  return value.  If the calling function fails to use or store the return
///  value, the destructor calls the OnError policy.
///
///  \par Template Parameters
///  CheckReturn has two template parameters, Value and OnError.
///  - Value is the return type from the function.  CheckReturn stores a copy of
///  it rather than a reference or pointer since return value could be local to
///  a function.  CheckReturn works best when the return type is a built-in
///  primitive (bool, int, etc...) a pointer, or an enum (such as an error
///  condition enum).  It can work with other types that have cheap copy
///  operations.
///  - OnError is a policy class indicating how to handle the situation when a
///  caller does not check or copy the returned value.  Loki provides some
///  policy classs and you may also write your own.  For example, you can write
///  a policy to create a message box when the function ignores the return value.
///  That would quickly tell you places where code ignores the function call.
///  If your write your own, you only need a templated class or struct with a
///  public function named "run" that accepts a reference to a const value.
///
/// @par Provided Policy Classes
///  - IgnoreReturnValue Deliberately ignores when the caller ignores the return value.
///  - TriggerAssert Asserts in debug builds if the caller ignores the return value.
///  - FprintfStderr Prints out an error message if the caller ignores the return value.
///  - ThrowTheValue Throws the ignored value as an exception.
///  - ThrowLogicError Throws a logic_error exception to indicate a programming error.
////////////////////////////////////////////////////////////////////////////////


template<class T>
struct IgnoreReturnValue
{
	static void run(const T&)
	{
		/// Do nothing at all.
	}
};

template<class T>
struct ThrowTheValue
{
	static void run(const T & value )
	{
		throw value;
	}
};

template<class T>
struct ThrowLogicError
{
	static void run( const T & )
	{
		throw ::std::logic_error( "CheckReturn: return value was not checked.\n" );
	}
};

template<class T>
struct TriggerAssert
{
	static void run(const T&)
	{
		assert( 0 );
	}
};

template<class T>
struct FprintfStderr
{
	static void run(const T&)
	{
		fprintf(stderr, "CheckReturn: return value was not checked.\n");
	}
};



template < class ValueType , template<class> class OnError = TriggerAssert >
class CheckReturn
{
public:

	/// Conversion constructor changes Value type to CheckReturn type.
	inline CheckReturn( const ValueType & value ) :
		m_value( value ), m_checked( false ) {}

	/// Copy-constructor allows functions to call another function within the
	/// return statement.  The other CheckReturn's m_checked flag is set since
	/// its duty has been passed to the m_checked flag in this one.
	inline CheckReturn( const CheckReturn & that ) :
		m_value( that.m_value ), m_checked( false )
	{ that.m_checked = true; }

	/// Destructor checks if return value was used.
	inline ~CheckReturn( void )
	{
		// If m_checked is false, then a function failed to check the
		// return value from a function call.
		if (!m_checked)
			OnError< ValueType >::run(m_value);
	}

	/// Conversion operator changes CheckReturn back to Value type.
	inline operator ValueType ( void )
	{
		m_checked = true;
		return m_value;
	}

private:
	/// Default constructor not implemented.
	CheckReturn( void );

	/// Copy-assignment operator not implemented.
	CheckReturn & operator = ( const CheckReturn & that );

	/// Copy of returned value.
	ValueType m_value;

	/// Flag for whether calling function checked return value yet.
	mutable bool m_checked;
};

// ----------------------------------------------------------------------------

} // namespace Loki

#endif // end file guardian

// $Log$

