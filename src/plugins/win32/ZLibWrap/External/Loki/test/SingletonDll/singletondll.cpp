////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter K�mmel
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id$


#include "singletondll.h"

#include "foo.h"


typedef Loki::SingletonHolder<Foo> FooSingleton;

LOKI_SINGLETON_INSTANCE_DEFINITION(FooSingleton)
/*
This code will be generated by the macro,
(ObjectType is a typedef of SingletonHolder):

namespace Loki                                                          
{                                                                        
    template<>                                                          
    FooSingleton::ObjectType&  Singleton<FooSingleton::ObjectType>::Instance()    
    {                                                                   
        return FooSingleton::Instance();                                     
    }                                                                    
}
*/


template<>
Foo&  Singleton<Foo>::Instance()
{
    return Loki::SingletonHolder<Foo>::Instance();
}


// explicit instantiations
template class Singleton<Foo>;
template class Loki::Singleton<Foo>;

