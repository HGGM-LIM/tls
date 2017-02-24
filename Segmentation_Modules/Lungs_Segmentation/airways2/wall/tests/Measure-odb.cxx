// This file was generated by ODB, object-relational mapping (ORM)
// compiler for C++.
//

#include <odb/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#include "Measure-odb.hxx"

#include <odb/cache-traits.hxx>

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/traits.hxx>
#include <odb/mysql/database.hxx>
#include <odb/mysql/transaction.hxx>
#include <odb/mysql/connection.hxx>
#include <odb/mysql/statement.hxx>
#include <odb/mysql/statement-cache.hxx>
#include <odb/mysql/object-statements.hxx>
#include <odb/mysql/container-statements.hxx>
#include <odb/mysql/exceptions.hxx>
#include <odb/mysql/result.hxx>

#include <odb/details/unused.hxx>
#include <odb/details/shared-ptr.hxx>

namespace odb
{
  // Measure
  //

  struct access::object_traits< ::Measure >::container_statement_cache_type
  {
    container_statement_cache_type (mysql::connection&)
    {
    }
  };

  const mysql::query_column<
    mysql::value_traits< long unsigned int, unsigned long long, mysql::id_ulonglong >::query_type,
    mysql::id_ulonglong>
  access::object_traits< ::Measure >::query_type::id (
    "`Measure`.`id`");

  const mysql::query_column<
    mysql::value_traits< ::std::string, details::buffer, mysql::id_string >::query_type,
    mysql::id_string>
  access::object_traits< ::Measure >::query_type::name (
    "`Measure`.`name`");

  const char* const access::object_traits< ::Measure >::persist_statement =
  "INSERT INTO `Measure` ("
  "`Measure`.`id`,"
  "`Measure`.`name`"
  ") VALUES (?,?)";

  const char* const access::object_traits< ::Measure >::find_statement =
  "SELECT "
  "`Measure`.`id`,"
  "`Measure`.`name`"
  " FROM `Measure`"
  " WHERE `Measure`.`id` = ?";

  const char* const access::object_traits< ::Measure >::update_statement =
  "UPDATE `Measure` SET "
  "`Measure`.`id` = ?,"
  "`Measure`.`name` = ?"
  " WHERE `Measure`.`id` = ?";

  const char* const access::object_traits< ::Measure >::erase_statement =
  "DELETE FROM `Measure`"
  " WHERE `Measure`.`id` = ?";

  const char* const access::object_traits< ::Measure >::query_clause =
  "SELECT "
  "`Measure`.`id`,"
  "`Measure`.`name`"
  " FROM `Measure`"
  " ";

  access::object_traits< ::Measure >::id_type
  access::object_traits< ::Measure >::
  id (const image_type& i)
  {
    id_type id;
    mysql::value_traits<
        long unsigned int,
        unsigned long long,
        mysql::id_ulonglong >::set_value (
      id, i.id_value, i.id_null);

    return id;
  }

  void access::object_traits< ::Measure >::
  grow (image_type& i, my_bool* e)
  {
    bool grew (false);

    // id_
    //
    e[0UL] = 0;

    // name_
    //
    if (e[1UL])
    {
      i.name_value.capacity (i.name_size);
      grew = true;
    }

    if (grew)
      i.version++;
  }

  void access::object_traits< ::Measure >::
  bind (MYSQL_BIND* b, image_type& i, bool out)
  {
    ODB_POTENTIALLY_UNUSED (out);

    std::size_t n (0);

    // id_
    //
    b[n].buffer_type = MYSQL_TYPE_LONGLONG;
    b[n].is_unsigned = 1;
    b[n].buffer = &i.id_value;
    b[n].is_null = &i.id_null;
    n++;

    // name_
    //
    b[n].buffer_type = MYSQL_TYPE_STRING;
    b[n].buffer = i.name_value.data ();
    b[n].buffer_length = static_cast<unsigned long> (
      i.name_value.capacity ());
    b[n].length = &i.name_size;
    b[n].is_null = &i.name_null;
    n++;
  }

  void access::object_traits< ::Measure >::
  bind (MYSQL_BIND* b, id_image_type& i)
  {
    std::size_t n (0);
    b[n].buffer_type = MYSQL_TYPE_LONGLONG;
    b[n].is_unsigned = 1;
    b[n].buffer = &i.id_value;
    b[n].is_null = &i.id_null;
  }

  void access::object_traits< ::Measure >::
  init (image_type& i, const object_type& o)
  {
    bool grew (false);

    // id_
    //
    {
      bool is_null;
      mysql::value_traits<
          long unsigned int,
          unsigned long long,
          mysql::id_ulonglong >::set_image (
        i.id_value, is_null, o.id_);
      i.id_null = is_null;
    }

    // name_
    //
    {
      bool is_null;
      std::size_t size (0);
      std::size_t cap (i.name_value.capacity ());
      mysql::value_traits<
          ::std::string,
          details::buffer,
          mysql::id_string >::set_image (
        i.name_value,
        size,
        is_null,
        o.name_);
      i.name_size = static_cast<unsigned long> (size);
      grew = grew || (cap != i.name_value.capacity ());
      i.name_null = is_null;
    }

    if (grew)
      i.version++;
  }

  void access::object_traits< ::Measure >::
  init (object_type& o, const image_type& i, database& db)
  {
    ODB_POTENTIALLY_UNUSED (db);

    // id_
    //
    mysql::value_traits<
        long unsigned int,
        unsigned long long,
        mysql::id_ulonglong >::set_value (
      o.id_, i.id_value, i.id_null);

    // name_
    //
    mysql::value_traits<
        ::std::string,
        details::buffer,
        mysql::id_string >::set_value (
      o.name_,
      i.name_value,
      i.name_size,
      i.name_null);
  }

  void access::object_traits< ::Measure >::
  init (id_image_type& i, const id_type& id)
  {
    {
      bool is_null;
      mysql::value_traits<
          long unsigned int,
          unsigned long long,
          mysql::id_ulonglong >::set_image (
        i.id_value, is_null, id);
      i.id_null = is_null;
    }
  }

  void access::object_traits< ::Measure >::
  persist (database&, object_type& obj)
  {
    using namespace mysql;

    connection& conn (mysql::transaction::current ().connection ());
    object_statements< object_type >& sts (
      conn.statement_cache ().find<object_type> ());
    image_type& im (sts.image ());
    binding& imb (sts.in_image_binding ());

    obj.id_ = 0;
    init (im, obj);

    if (im.version != sts.in_image_version () || imb.version == 0)
    {
      bind (imb.bind, im, false);
      sts.in_image_version (im.version);
      imb.version++;
    }

    insert_statement& st (sts.persist_statement ());
    if (!st.execute ())
      throw object_already_persistent ();

    obj.id_ = static_cast<id_type> (st.id ());
  }

  void access::object_traits< ::Measure >::
  update (database&, const object_type& obj)
  {
    using namespace mysql;

    connection& conn (mysql::transaction::current ().connection ());
    object_statements< object_type >& sts (
      conn.statement_cache ().find<object_type> ());

    id_image_type& i (sts.id_image ());
    init (i, obj.id_);

    binding& idb (sts.id_image_binding ());
    if (i.version != sts.id_image_version () || idb.version == 0)
    {
      bind (idb.bind, i);
      sts.id_image_version (i.version);
      idb.version++;
    }

    image_type& im (sts.image ());
    binding& imb (sts.in_image_binding ());
    init (im, obj);

    if (im.version != sts.in_image_version () || imb.version == 0)
    {
      bind (imb.bind, im, false);
      sts.in_image_version (im.version);
      imb.version++;
    }

    sts.update_statement ().execute ();
  }

  void access::object_traits< ::Measure >::
  erase (database&, const id_type& id)
  {
    using namespace mysql;

    connection& conn (mysql::transaction::current ().connection ());
    object_statements< object_type >& sts (
      conn.statement_cache ().find<object_type> ());

    id_image_type& i (sts.id_image ());
    init (i, id);

    binding& idb (sts.id_image_binding ());
    if (i.version != sts.id_image_version () || idb.version == 0)
    {
      bind (idb.bind, i);
      sts.id_image_version (i.version);
      idb.version++;
    }

    if (sts.erase_statement ().execute () != 1)
      throw object_not_persistent ();
  }

  access::object_traits< ::Measure >::pointer_type
  access::object_traits< ::Measure >::
  find (database& db, const id_type& id)
  {
    using namespace mysql;

    connection& conn (mysql::transaction::current ().connection ());
    object_statements< object_type >& sts (
      conn.statement_cache ().find<object_type> ());
    object_statements< object_type >::auto_lock l (sts);

    if (l.locked ())
    {
      if (!find_ (sts, id))
        return pointer_type ();
    }

    pointer_type p (
      access::object_factory< object_type, pointer_type  >::create ());
    pointer_traits< pointer_type >::guard pg (p);
    pointer_cache_traits< pointer_type >::insert_guard ig (
      pointer_cache_traits< pointer_type >::insert (db, id, p));
    object_type& obj (pointer_traits< pointer_type >::get_ref (p));

    if (l.locked ())
    {
      init (obj, sts.image (), db);
      load_ (sts, obj);
      sts.load_delayed ();
      l.unlock ();
    }
    else
      sts.delay_load (id, obj, ig.position ());

    ig.release ();
    pg.release ();
    return p;
  }

  bool access::object_traits< ::Measure >::
  find (database& db, const id_type& id, object_type& obj)
  {
    using namespace mysql;

    connection& conn (mysql::transaction::current ().connection ());
    object_statements< object_type >& sts (
      conn.statement_cache ().find<object_type> ());
    object_statements< object_type >::auto_lock l (sts);

    if (l.locked ())
    {
      if (!find_ (sts, id))
        return false;
    }

    reference_cache_traits< object_type >::insert_guard ig (
      reference_cache_traits< object_type >::insert (db, id, obj));

    if (l.locked ())
    {
      init (obj, sts.image (), db);
      load_ (sts, obj);
      sts.load_delayed ();
      l.unlock ();
    }
    else
      sts.delay_load (id, obj, ig.position ());

    ig.release ();
    return true;
  }

  bool access::object_traits< ::Measure >::
  find_ (mysql::object_statements< object_type >& sts, const id_type& id)
  {
    using namespace mysql;

    id_image_type& i (sts.id_image ());
    init (i, id);

    binding& idb (sts.id_image_binding ());
    if (i.version != sts.id_image_version () || idb.version == 0)
    {
      bind (idb.bind, i);
      sts.id_image_version (i.version);
      idb.version++;
    }

    image_type& im (sts.image ());
    binding& imb (sts.out_image_binding ());

    if (im.version != sts.out_image_version () || imb.version == 0)
    {
      bind (imb.bind, im, true);
      sts.out_image_version (im.version);
      imb.version++;
    }

    select_statement& st (sts.find_statement ());
    st.execute ();
    select_statement::result r (st.fetch ());

    if (r == select_statement::truncated)
    {
      grow (im, sts.out_image_error ());

      if (im.version != sts.out_image_version ())
      {
        bind (imb.bind, im, true);
        sts.out_image_version (im.version);
        imb.version++;
        st.refetch ();
      }
    }

    st.free_result ();
    return r != select_statement::no_data;
  }

  template<>
  result< access::object_traits< ::Measure >::object_type >
  access::object_traits< ::Measure >::
  query< access::object_traits< ::Measure >::object_type > (
    database& db,
    const query_type& q)
  {
    using namespace mysql;

    connection& conn (mysql::transaction::current ().connection ());

    object_statements< object_type >& sts (
      conn.statement_cache ().find<object_type> ());
    details::shared_ptr<select_statement> st;

    query_ (db, q, sts, st);

    details::shared_ptr<odb::result_impl<object_type> > r (
      new (details::shared) mysql::result_impl<object_type> (st, sts));
    return result<object_type> (r);
  }

  template<>
  result< const access::object_traits< ::Measure >::object_type >
  access::object_traits< ::Measure >::
  query< const access::object_traits< ::Measure >::object_type > (
    database& db,
    const query_type& q)
  {
    using namespace mysql;

    connection& conn (mysql::transaction::current ().connection ());

    object_statements< object_type >& sts (
      conn.statement_cache ().find<object_type> ());
    details::shared_ptr<select_statement> st;

    query_ (db, q, sts, st);

    details::shared_ptr<odb::result_impl<const object_type> > r (
      new (details::shared) mysql::result_impl<const object_type> (st, sts));
    return result<const object_type> (r);
  }

  void
  access::object_traits< ::Measure >::
  query_ (database&,
          const query_type& q,
          mysql::object_statements< object_type >& sts,details::shared_ptr<mysql::select_statement>& st)
  {
    using namespace mysql;

    image_type& im (sts.image ());
    binding& imb (sts.out_image_binding ());

    if (im.version != sts.out_image_version () || imb.version == 0)
    {
      bind (imb.bind, im, true);
      sts.out_image_version (im.version);
      imb.version++;
    }

    st.reset (new (details::shared) select_statement (
                sts.connection (),
                query_clause + q.clause (),
                q.parameters (),
                imb));
    st->execute ();
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <odb/post.hxx>