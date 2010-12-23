/*==============================================================================
    Copyright (c) 2001-2010 Joel de Guzman
    Copyright (c) 2004 Daniel Wallin
    Copyright (c) 2010 Thomas Heller

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef PHOENIX_SCOPE_DYNAMIC_HPP
#define PHOENIX_SCOPE_DYNAMIC_HPP

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/phoenix/core/expression.hpp>
#include <boost/phoenix/support/iterate.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>

#define BOOST_PHOENIX_DYNAMIC_TEMPLATE_PARAMS(R, DATA, I, ELEM)                 \
      BOOST_PP_COMMA_IF(I) BOOST_PP_TUPLE_ELEM(2, 0, ELEM)                      \
/**/

#define BOOST_PHOENIX_DYNAMIC_CTOR_INIT(R, DATA, I, ELEM)                       \
    BOOST_PP_COMMA_IF(I) BOOST_PP_TUPLE_ELEM(2, 1, ELEM)(init<I>(this))         \
/**/

#define BOOST_PHOENIX_DYNAMIC_MEMBER(R, DATA, I, ELEM)                          \
    BOOST_PP_CAT(member, BOOST_PP_INC(I)) BOOST_PP_TUPLE_ELEM(2, 1, ELEM);      \
/**/

#define BOOST_PHOENIX_DYNAMIC_FILLER_0(X, Y)                                    \
    ((X, Y)) BOOST_PHOENIX_DYNAMIC_FILLER_1                                     \
/**/

#define BOOST_PHOENIX_DYNAMIC_FILLER_1(X, Y)                                    \
    ((X, Y)) BOOST_PHOENIX_DYNAMIC_FILLER_0                                     \
/**/

#define BOOST_PHOENIX_DYNAMIC_FILLER_0_END
#define BOOST_PHOENIX_DYNAMIC_FILLER_1_END

#define BOOST_PHOENIX_DYNAMIC_BASE(NAME, MEMBER)                                \
struct NAME                                                                     \
    : ::boost::phoenix::dynamic<                                                \
        BOOST_PP_SEQ_FOR_EACH_I(                                                \
                BOOST_PHOENIX_DYNAMIC_TEMPLATE_PARAMS                           \
              , _                                                               \
              , MEMBER)                                                         \
    >                                                                           \
{                                                                               \
    NAME()                                                                      \
        : BOOST_PP_SEQ_FOR_EACH_I(BOOST_PHOENIX_DYNAMIC_CTOR_INIT, _, MEMBER)   \
    {}                                                                          \
                                                                                \
    BOOST_PP_SEQ_FOR_EACH_I(BOOST_PHOENIX_DYNAMIC_MEMBER, _, MEMBER)            \
}                                                                               \
/**/

#define BOOST_PHOENIX_DYNAMIC(NAME, MEMBER)                                     \
    BOOST_PHOENIX_DYNAMIC_BASE(                                                 \
        NAME                                                                    \
      , BOOST_PP_CAT(BOOST_PHOENIX_DYNAMIC_FILLER_0 MEMBER,_END)                \
    )                                                                           \
/**/

namespace boost { namespace phoenix
{
    template <typename DynamicScope>
    struct dynamic_frame : noncopyable
    {
        typedef typename DynamicScope::tuple_type tuple_type;

        dynamic_frame(DynamicScope const& scope)
            : tuple()
            , save(scope.frame)
            , scope(scope)
        {
            scope.frame = this;
        }

        template <typename Tuple>
        dynamic_frame(DynamicScope const& scope, Tuple const& init)
            : tuple(init)
            , save(scope.frame)
            , scope(scope)
        {
            scope.frame = this;
        }

        ~dynamic_frame()
        {
            scope.frame = save;
        }

        tuple_type& data() { return tuple; }
        tuple_type const& data() const { return tuple; }

        private:
            tuple_type tuple;
            dynamic_frame *save;
            DynamicScope const& scope;
    };

    PHOENIX_DEFINE_EXPRESSION(
        dynamic_member
      , (proto::terminal<proto::_>)
        (proto::terminal<proto::_>)
    )

    struct dynamic_member_eval
    {
        template <typename Sig>
        struct result;

        template <typename This, typename Env, typename N, typename Scope>
        struct result<This(Env, N, Scope)>
        {
            typedef
                typename boost::remove_pointer<
                    typename proto::detail::uncvref<Scope>::type
                >::type
                scope_type;
            typedef typename scope_type::dynamic_frame_type::tuple_type tuple_type;

            typedef typename fusion::result_of::at_c<tuple_type, proto::detail::uncvref<N>::type::value>::type type;

        };

        template <typename Env, typename N, typename Scope>
        typename result<dynamic_member_eval(Env, N, Scope)>::type
        operator()(Env & env, N, Scope s) const
        {
            return fusion::at_c<N::value>(s->frame->data());
        }
    };

    template <typename Dummy>
    struct default_actions::when<rule::dynamic_member, Dummy>
        : proto::call<dynamic_member_eval(_env, proto::_value(proto::_child_c<0>), proto::_value(proto::_child_c<1>))>
    {};
    
    template <PHOENIX_typename_A_void(PHOENIX_DYNAMIC_LIMIT), typename Dummy = void>
    struct dynamic;
    
    template <typename A0>
    struct dynamic<A0> : noncopyable
    {
        typedef fusion::vector1<A0> tuple_type;
        typedef dynamic<A0> self_type;
        typedef dynamic_frame<self_type> dynamic_frame_type;

        dynamic()
            : frame(0) {}

        template <int N>
        static
        void//typename make_dynamic_member<N, self_type>::type
        init(self_type& scope)
        {
            //return make_dynamic_member<N, self_type>()(dynamic_member_data<N, self_type>(static_cast<self_type &>(scope)));
        }

        //typedef typename make_dynamic_member<0, self_type>::type member1;
        typedef typename expression::dynamic_member<mpl::int_<0>, self_type>::type member1;

        mutable dynamic_frame_type* frame;
    };
    
    template <typename A0, typename A1>
    struct dynamic<A0, A1> : noncopyable
    {
        typedef fusion::vector2<A0, A1> tuple_type;
        typedef dynamic<A0, A1> self_type;
        typedef dynamic_frame<self_type> dynamic_frame_type;

        dynamic()
            : frame(0) {}

        template <int N>
        static 
        void//typename make_dynamic_member<N, self_type>::type
        init(self_type& scope)
        {
            //return make_dynamic_member<N, self_type>()(dynamic_member_data<N, self_type>(static_cast<self_type &>(scope)));
        }

        //typedef typename make_dynamic_member<0, self_type>::type member1;
        typedef typename expression::dynamic_member<mpl::int_<0>, self_type>::type member1;
        //typedef typename make_dynamic_member<1, self_type>::type member2;
        typedef typename expression::dynamic_member<mpl::int_<1>, self_type>::type member2;

        mutable dynamic_frame_type* frame;
    };
    
    template <typename A0, typename A1, typename A2>
    struct dynamic<A0, A1, A2> : noncopyable
    {
        typedef fusion::vector3<A0, A1, A2> tuple_type;
        typedef dynamic<A0, A1, A2> self_type;
        typedef dynamic_frame<self_type> dynamic_frame_type;

        dynamic()
            : frame(0) {}

        template <int N>
        static
        typename expression::dynamic_member<mpl::int_<N>, self_type *>::type
        init(self_type * scope)
        {
            return expression::dynamic_member<mpl::int_<N>, self_type *>::make(mpl::int_<N>(), scope);
        }

        typedef typename expression::dynamic_member<mpl::int_<0>, dynamic *>::type member1;
        typedef typename expression::dynamic_member<mpl::int_<1>, dynamic *>::type member2;
        typedef typename expression::dynamic_member<mpl::int_<2>, dynamic *>::type member3;

        mutable dynamic_frame_type* frame;
    };

    // Bring in the rest ...
    #include <boost/phoenix/scope/detail/dynamic.hpp>
}}

#endif
