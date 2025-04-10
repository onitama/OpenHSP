////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 by Peter K�mmel
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
#ifndef LOKI_SEQUENCE_INC_
#define LOKI_SEQUENCE_INC_

// $Id$


#include <loki/Typelist.h>

namespace Loki
{

    template
    <
        class T01=NullType,class T02=NullType,class T03=NullType,class T04=NullType,class T05=NullType,
        class T06=NullType,class T07=NullType,class T08=NullType,class T09=NullType,class T10=NullType,
        class T11=NullType,class T12=NullType,class T13=NullType,class T14=NullType,class T15=NullType,
        class T16=NullType,class T17=NullType,class T18=NullType,class T19=NullType,class T20=NullType
    >
    struct Seq
    {
    private:
        typedef typename Seq<     T02, T03, T04, T05, T06, T07, T08, T09, T10,
                             T11, T12, T13, T14, T15, T16, T17, T18, T19, T20>::Type
                         TailResult;
    public:
        typedef Typelist<T01, TailResult> Type;
    };

    template<>
    struct Seq<>
    {
        typedef NullType Type;
    };

}   // namespace Loki

#endif // end file guardian

