// This file was generated by ODB, object-relational mapping (ORM)
// compiler for C++.
//

// Begin prologue.
//
//
// End prologue.

namespace odb
{
  // ImageEntity
  //

  inline
  access::object_traits< ::ImageEntity >::query_type::
  query_type ()
  {
  }

  inline
  access::object_traits< ::ImageEntity >::query_type::
  query_type (const std::string& q)
    : query_base_type (q)
  {
  }

  inline
  access::object_traits< ::ImageEntity >::query_type::
  query_type (const query_base_type& q)
    : query_base_type (q)
  {
  }

  inline
  access::object_traits< ::ImageEntity >::id_type
  access::object_traits< ::ImageEntity >::
  id (const object_type& obj)
  {
    return obj.id_;
  }

  inline
  void access::object_traits< ::ImageEntity >::
  load_ (mysql::object_statements< object_type >&, object_type&)
  {
  }
}

// Begin epilogue.
//
//
// End epilogue.