#pragma once

namespace gridtools{
namespace _impl{
    template<typename T>
    struct is_expandable_parameters : boost::mpl::false_{};

    template<typename BaseStorage, ushort_t N>
    struct is_expandable_parameters<expandable_parameters<BaseStorage, N> > : boost::mpl::true_{};

    template<typename T>
    struct is_expandable_arg : boost::mpl::false_{};

    template<uint_t N, typename Storage, typename Condition>
    struct is_expandable_arg <arg<N, Storage, Condition > > : is_expandable_parameters<Storage>{};

    template<uint_t N, typename Storage>
    struct is_expandable_arg <arg<N, Storage> > : is_expandable_parameters<Storage>{};

    template <typename T>
    struct get_basic_storage{
        typedef typename T::storage_type::basic_type type;
    };

    template <typename T>
    struct get_storage{
        typedef typename T::storage_type type;
    };

    template <typename T>
    struct get_index{
        typedef typename T::index_type type;
        static const uint_t value = T::index_type::value;
    };

    template< typename T, typename ExpandFactor>
    struct create_arg{
        typedef arg<get_index<T>::value, expandable_parameters<
                                             typename get_basic_storage<T>::type
                                             , ExpandFactor::value>
        > type;
    };


        template<typename DomainFrom, typename Vec>
        struct initialize_storage{

        private:

            DomainFrom const& m_dom_from;
            Vec& m_vec_to;

        public:
            initialize_storage(DomainFrom const& dom_from_, Vec & vec_to_)
                :
                m_dom_from(dom_from_)
                , m_vec_to(vec_to_){}

            template <typename T>
            void operator()(T){
                boost::fusion::at<typename T::index_type>(m_vec_to)
                    =
                    //TODO: who deletes this new? The domain_type?
                    new
                    typename boost::remove_reference<
                        typename boost::fusion::result_of::at<
                            Vec
                            , typename T::index_type
                            >::type>::type::value_type (m_dom_from.template storage_pointer<T>()->meta_data(), "expandable params", false /*do_allocate*/);
            }
        };



        template<typename Vec>
        struct delete_storage{

        private:

            Vec& m_vec_to;

        public:
            delete_storage(Vec & vec_to_)
                :
                m_vec_to(vec_to_){}

            template <typename T>
            void operator()(T){
                // setting the flag "externally_managed" in order to avoid that the storage pointers
                // get deleted twice (once here and again when destructing the user-defined storage)
                boost::fusion::at<typename T::index_type>(m_vec_to)->set_externally_managed(true);
                delete_pointer deleter;
                deleter(boost::fusion::at<typename T::index_type>(m_vec_to));
            }
        };

        template<typename DomainFrom, typename DomainTo>
        struct assign_expandable_params{

        private:

            DomainFrom const& m_dom_from;
            DomainTo& m_dom_to;
            uint_t const& m_idx;

        public:

            assign_expandable_params(DomainFrom const & dom_from_, DomainTo & dom_to_, uint_t const& i_):m_dom_from(dom_from_), m_dom_to(dom_to_),  m_idx(i_){}

            template <typename T>
            void operator()(T){

                m_dom_to.template storage_pointer<T>()->assign_pointers(*m_dom_from.template storage_pointer<T>(), m_idx);
            }
        };

} //namespace _impl
} //namespace gridtools
