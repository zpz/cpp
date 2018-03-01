#ifndef _zpz_utilities_avro_h_
#define _zpz_utilities_avro_h_


#include "avro/DataFile.hh"
#include "avro/Generic.hh"

#include "exception.h"
#include "string.h"

#include <exception>
#include <string>
#include <vector>


namespace zpz
{


class AvroReader {
    // Reads data from Avro file that contains a single record at the top level.
    //
    // The class contains an internal 'cursor' which points to an element in the object.
    // The cursor is moved around by `seek`, not unlike a `seek` operation in an opened file object.
    // Usually, you should call `save_cursor` before calling `seek`, and `restore_cursor` after
    // you're done with the object at the new location, so to restore this pointer to point
    // to the original element prior to you moving it around.
    //
    // The 'current element' means the element being pointed to by the current value
    // of the internal cursor.
    //
    // Most functions in this class accept a 'name hierarchy' in the arguments, called `names`, like
    //
    //    "ab", "cd", "ef"
    //
    // This specifies element "ab" under the current element, and "cd" under "ab", and "ef" under "cd".
    // For this to work, it's necessary that the current element as well as "ab" and "cd"
    // are of `record` type.
    //
    // If any name on the hierarchy is "/", it resets the cursor to the very root of the document.
    //
    // `save_cursor` and `restore_cursor` maintains a stack, hence the cursor save/restore operations
    // can be nested in many levels.
    //
    // This class is not optimized for speed.

  public:
    using Datum = avro::GenericDatum;

    AvroReader(char const* data_file)
    {
        avro::DataFileReader<Datum> reader(data_file);
        auto schema = reader.readerSchema();
        _root = Datum(schema);
        reader.read(_root);
        if (_root.type() != avro::AVRO_RECORD) {
            throw Error("top level of the AVRO data is not a record type");
        }
        auto const& r = _root.value<avro::GenericRecord>();
        auto name = string(r.schema()->name());
        auto pos = name.find('.');
        while (pos != std::string::npos) {
            name = name.substr(pos + 1);
            pos = name.find('.');
        } // strip off 'namespace' in 'name'
        _root_name = name;
        reader.close();

        _cursor = &_root;
    }

    ~AvroReader() 
    {
        _check_cursor();
    }

    // `name` element at the top level, i.e. the entire Avro file.
    string name() const
    {
        return _root_name;
    }

    // Move internal cursor to point to the element with the specified name hierarchy.
    template <typename... Names>
    void seek(Names&&... names)
    {
        _cursor = _cseek(_cursor, std::forward<Names>(names)...);
    }

    // Move internal cursor to point to the array element at the specified index in the array
    // that is specified by the name hierarchy.
    template <typename... Names>
    void seek_in_array(size_t pos, Names&&... names)
    {
        _cursor = _cseek_in_array(_cursor, pos, std::forward<Names>(names)...);
    }

    // Save current cursor so that the location can be restored later.
    void save_cursor()
    {
        _cursor_stack.push_back(_cursor);
    }

    // Restore internal cursor to point to the location that is saved
    // in the immediately previous `save_cursor`.
    // A stack of saved cursors are maintained, so that this 'save', 'restore'
    // can be nested.
    void restore_cursor()
    {
        if (_cursor_stack.empty()) {
            throw Error("you are trying to restore cursor while no cursor is saved");
        }
        _cursor = _cursor_stack.back();
        _cursor_stack.pop_back();
    }

    template <typename... Names>
    bool is_record(Names&&... names) const
    {
        return _cseek(std::forward<Names>(names)...)->type() == avro::AVRO_RECORD;
    }

    template <typename... Names>
    bool is_array(Names&&... names) const
    {
        return _cseek(std::forward<Names>(names)...)->type() == avro::AVRO_ARRAY;
    }

    template <typename... Names>
    bool is_string(Names&&... names) const
    {
        return _cseek(std::forward<Names>(names)...)->type() == avro::AVRO_STRING;
    }

    template <typename... Names>
    bool is_int(Names&&... names) const
    {
        return _cseek(std::forward<Names>(names)...)->type() == avro::AVRO_INT;
    }

    template <typename... Names>
    bool is_long(Names&&... names) const
    {
        return _cseek(std::forward<Names>(names)...)->type() == avro::AVRO_LONG;
    }

    template <typename... Names>
    bool is_double(Names&&... names) const
    {
        return _cseek(std::forward<Names>(names)...)->type() == avro::AVRO_DOUBLE;
    }

    template <typename... Names>
    bool is_float(Names&&... names) const
    {
        return _cseek(std::forward<Names>(names)...)->type() == avro::AVRO_FLOAT;
    }

    template <typename... Names>
    bool is_bool(Names&&... names) const
    {
        return _cseek(std::forward<Names>(names)...)->type() == avro::AVRO_BOOL;
    }

    template <typename... Names>
    bool is_string_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), avro::AVRO_STRING);
    }

    template <typename... Names>
    bool is_int_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), avro::AVRO_INT);
    }

    template <typename... Names>
    bool is_long_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), avro::AVRO_LONG);
    }

    template <typename... Names>
    bool is_double_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), avro::AVRO_DOUBLE);
    }

    template <typename... Names>
    bool is_float_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), avro::AVRO_FLOAT);
    }

    template <typename... Names>
    bool is_bool_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), avro::AVRO_BOOL);
    }

    // Whether the element specified by the name hierarchy exists.
    template <typename... Names>
    bool has_member(string const& name, Names&&... names) const
    {
        return _has_member(_cursor, name, std::forward<Names>(names)...);
    }

    // Size of the array element specified by the name hierarchy.
    template <typename... Names>
    size_t get_array_size(Names&&... names) const
    {
        return _get_array_size(_cseek(_cursor, std::forward<Names>(names)...));
    }

    // Get the scalar value of the element specified by the name hierarchy.
    // The element must be a scalar that is compatible with type `T`.
    // `T` should be one of `int`, `double`, `std::string`.
    template <typename T, typename... Names>
    const T get_scalar(Names&&... names) const
    {
        auto cursor = _cseek(_cursor, std::forward<Names>(names)...);
        return _get_scalar<T>(cursor);
    }

    // Get the scalar value at the specified index in the specified array element.
    template <typename T, typename... Names>
    T get_scalar_in_array(size_t pos, Names&&... names) const
    {
        auto cursor = _seek_in_array(_cursor, pos, std::forward<Names>(names)...);
        return _get_scalar<T>(cursor);
    }

    // Get the vector value of the specified array element.
    // The specified element must be an array and contains elements of a type compatible with `T`.
    template <typename T, typename... Names>
    vector<T> get_vector(Names&&... names) const
    {
        auto cursor = _cseek(_cursor, std::forward<Names>(names)...);
        return _get_vector<T>(cursor);
    }

    // Get the vector value at the specified index in the specified array element.
    // The specified element must be an array and contains array elements of type compatible with `vector<T>`.
    template <typename T, typename... Names>
    vector<T> get_vector_in_array(size_t pos, Names&&... names)
    {
        auto cursor = _cseek_in_array(_cursor, pos, std::forward<Names>(names)...);
        return _get_vector<T>(cursor);
    }

  private:
    Datum _root;
    Datum const* _cursor;
    vector<Datum const*> _cursor_stack;
    string _root_name;

    Datum const* _cseek(Datum const* cursor) const
    {
        return cursor;
    }

    template <typename... Names>
    Datum const* _cseek(Datum const* cursor, string const& name, Names&&... names) const
    {
        if ("" == name) {
            throw Error("can not seek an element with empty name");
        }

        // Re-position the cursor at the root of the doc.
        if ("/" == name) {
            cursor = &_root;
        } else {
            if (cursor->type() != avro::AVRO_RECORD) {
                throw Error(make_string(
                    "can not seek element '",
                    name,
                    "' because curent element is not pointing at a AVRO_RECORD; actual type is '",
                    avro::toString(cursor->type()),
                    "'"));
            }
            auto const& rec = cursor->value<avro::GenericRecord>();
            if (!rec.hasField(name)) {
                throw Error(make_string(
                    "current record does not have field named '",
                    name,
                    "'"));
            }
            cursor = &rec.field(name);
        }
        return _cseek(cursor, std::forward<Names>(names)...);
    }

    template <typename... Names>
    Datum const* _cseek_in_array(Datum const* cursor, size_t pos, Names&&... names) const
    {
        cursor = _cseek(cursor, std::forward<Names>(names)...);
        _assert_type(cursor, avro::AVRO_ARRAY);
        auto const& data = cursor->value<avro::GenericArray>().value(); // vector<GenericDatum>
        auto n = data.size();
        if (pos >= n) {
            throw Error(make_string(
                "out of bounds: can not seek item <",
                pos,
                "> in array because array size is ",
                n));
        }
        return &data[pos];
    }

    size_t _get_array_size(Datum const* cursor) const
    {
        _assert_type(cursor, avro::AVRO_ARRAY);
        auto const& data = cursor->value<avro::GenericArray>().value();
        return data.size();
    }

    avro::Type _get_array_elem_type(Datum const* cursor) const
    {
        _assert_type(cursor, avro::AVRO_ARRAY);
        auto n = _get_array_size(cursor);
        if (n < 1) {
            throw Error("can not determine element type of an empty array");
        }
        return _cseek_in_array(cursor, 0)->type();
    }

    void _assert_type(Datum const* cursor, avro::Type const& t) const
    {
        if (cursor->type() != t) {
            throw Error(make_string(
                "encountered element of type '", avro::toString(cursor->type()),
                "' while type '", avro::toString(t), "' is expected"));
        }
    }

    bool _is_typed_array(Datum const* cursor, avro::Type const& t) const
    {
        if (cursor->type() != avro::AVRO_ARRAY) {
            return false;
        }
        return _get_array_elem_type(cursor) == t;
    }

    void _assert_array_elem_type(Datum const* cursor, avro::Type const& t) const
    {
        auto tt = _get_array_elem_type(cursor);
        if (t != tt) {
            throw Error(make_string(
                "encountered array with elements of type '", avro::toString(tt),
                "' while type '", avro::toString(t), "' is expected"));
        }
    }

    template <typename... Names>
    bool _has_member(Datum const* cursor, string const& name, Names&&... names) const
    {
        if constexpr (sizeof...(names) == 0) {
            if (cursor->type() == avro::AVRO_RECORD) {
                auto rec = cursor->value<avro::GenericRecord>();
                return rec.hasField(name);
            }
            throw Error(make_string(
                "'has_member' needs an element of type 'record', but current element has type '",
                avro::toString(cursor->type()),
                "'"));
        } else {
            return _has_member(_cseek(cursor, name), std::forward<Names>(names)...);
        }
    }

    template <typename T>
    T _get_scalar(Datum const* cursor) const
    {
        if (type_equals<T, string>()) {
            _assert_type(cursor, avro::AVRO_STRING);
        } else if (type_equals<T, int>()) {
            _assert_type(cursor, avro::AVRO_INT);
        } else if (type_equals<T, long>()) {
            _assert_type(cursor, avro::AVRO_LONG);
        } else if (type_equals<T, double>()) {
            _assert_type(cursor, avro::AVRO_DOUBLE);
        } else if (type_equals<T, float>()) {
            _assert_type(cursor, avro::AVRO_FLOAT);
        } else if (type_equals<T, bool>()) {
            _assert_type(cursor, avro::AVRO_BOOL);
        } else {
            throw Error("unknown type");
        }

        return cursor->value<T>();
    }

    template <typename T>
    vector<T> _get_vector(Datum const* cursor) const
    {
        if (cursor->type() != avro::AVRO_ARRAY) {
            throw Error(make_string(
                "can not get vector value because current element is not AVRO_ARRAY, ",
                "but rather ",
                avro::toString(cursor->type())));
        }

        if (type_equals<T, string>()) {
            _assert_array_elem_type(cursor, avro::AVRO_STRING);
        } else if (type_equals<T, int>()) {
            _assert_array_elem_type(cursor, avro::AVRO_INT);
        } else if (type_equals<T, long>()) {
            _assert_array_elem_type(cursor, avro::AVRO_LONG);
        } else if (type_equals<T, double>()) {
            _assert_array_elem_type(cursor, avro::AVRO_DOUBLE);
        } else if (type_equals<T, float>()) {
            _assert_array_elem_type(cursor, avro::AVRO_FLOAT);
        } else if (type_equals<T, bool>()) {
            _assert_array_elem_type(cursor, avro::AVRO_BOOL);
        } else {
            throw Error("unknown type");
        }

        auto const& data = cursor->value<avro::GenericArray>().value();
        vector<T> value;
        value.reserve(_get_array_size(cursor));
        for (auto const& v : data) {
            value.push_back(v.value<T>());
        }
        return std::move(value);
    }

    void _check_cursor() const {
        if (!_cursor_stack.empty()) {
            throw Error("`save_cursor` is not balanced out by `restore_cursor`");
        }
    }
};

}  // namespace zpz
#endif  // _zpz_utilities_avro_h_
