#ifndef _zpz_utilities_json_h_
#define _zpz_utilities_json_h_

#include "exception.h"
#include "string.h"
#include "typing.h"

#include <rapidjson/document.h>

namespace zpz {

class JsonReader {
    // Design of this class is similar to that of `AvroReader`.
    //
    // This class is not optimized for speed.

  public:
    using JsonDoc = rapidjson::Document;
    using JsonValue = rapidjson::Value;
    using Cursor = JsonValue const*;

    JsonReader(string json)
    {
        _root.Parse(json.c_str());
        _cursor = &_root;
    }

    JsonReader(char const* filename)
        : JsonReader(read_text_file(filename))
    {
    }

    ~JsonReader()
    {
        _check_cursor();
    }

    template <typename... Names>
    void seek(Names&&... names)
    {
        _cursor = _cseek(_cursor, std::forward<Names>(names)...);
    }

    template <typename... Names>
    void seek_in_array(size_t pos, Names&&... names)
    {
        _cursor = _cseek_in_array(_cursor, pos, std::forward<Names>(names)...);
    }

    void save_cursor()
    {
        _cursor_stack.push_back(_cursor);
    }

    void restore_cursor()
    {
        if (_cursor_stack.empty()) {
            throw Error("you are trying to restore cursor while no cursor is saved");
        }
        _cursor = _cursor_stack.back();
        _cursor_stack.pop_back();
    }

    template <typename... Names>
    bool is_string(Names&&... names) const
    {
        return _type_name(_cseek(_cursor, std::forward<Names>(names)...))->IsString();
    }

    template <typename... Names>
    bool is_int(Names&&... names) const
    {
        return _cseek(_cursor, std::forward<Names>(names)...)->IsInt();
    }

    template <typename... Names>
    bool is_uint(Names&&... names) const
    {
        return _cseek(_cursor, std::forward<Names>(names)...)->IsUint();
    }

    template <typename... Names>
    bool is_long(Names&&... names) const
    {
        return _cseek(_cursor, std::forward<Names>(names)...)->IsInt64();
    }

    template <typename... Names>
    bool is_double(Names&&... names) const
    {
        return _cseek(_cursor, std::forward<Names>(names)...)->IsDouble();
    }

    template <typename... Names>
    bool is_float(Names&&... names) const
    {
        return _cseek(_cursor, std::forward<Names>(names)...)->IsFloat();
    }

    template <typename... Names>
    bool is_number(Names&&... names) const
    {
        return _cseek(_cursor, std::forward<Names>(names)...)->IsNumber();
    }

    template <typename... Names>
    bool is_bool(Names&&... names) const
    {
        return _cseek(_cursor, std::forward<Names>(names)...)->IsBool();
    }

    template <typename... Names>
    bool is_array(Names&&... names) const
    {
        return _cseek(_cursor, std::forward<Names>(names)...)->IsArray();
    }

    template <typename... Names>
    bool is_object(Names&&... names) const
    {
        return _cseek(_cursor, std::forward<Names>(names)...)->IsObject();
    }

    template <typename... Names>
    bool is_null(Names&&... names) const
    {
        return _cseek(_cursor, std::forward<Names>(names)...)->IsNull();
    }

    template <typename... Names>
    bool is_string_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), "string");
    }

    template <typename... Names>
    bool is_int_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), "int");
    }

    template <typename... Names>
    bool is_uint_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), "uint");
    }

    template <typename... Names>
    bool is_long_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), "long");
    }

    template <typename... Names>
    bool is_double_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), "double");
    }

    template <typename... Names>
    bool is_float_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), "float");
    }

    template <typename... Names>
    bool is_bool_array(Names&&... names) const
    {
        return _is_typed_array(_cseek(_cursor, std::forward<Names>(names)...), "bool");
    }

    template <typename... Names>
    bool has_member(string const& name, Names&&... names) const
    {
        return _has_member(_cursor, name, std::forward<Names>(names)...);
    }

    template <typename... Names>
    size_t get_array_size(Names&&... names) const
    {
        auto cursor = _cseek(_cursor, std::forward<Names>(names)...);
        _assert_type(cursor, "array");
        return cursor->Size();
    }

    template <typename T, typename... Names>
    T get_scalar(Names&&... names) const
    {
        return _get_scalar<T>(_cseek(_cursor, std::forward<Names>(names)...));
    }

    template <typename T, typename... Names>
    T get_scalar_in_array(size_t pos, Names&&... names) const
    {
        return _get_scalar<T>(_cseek_in_array(_cursor, pos, std::forward<Names>(names)...));
    }

    template <typename T, typename... Names>
    vector<T> get_vector(Names&&... names) const
    {
        auto cursor = _cseek(_cursor, std::forward<Names>(names)...);
        return _get_vector<T>(cursor);
    }

    template <typename T, typename... Names>
    vector<T> get_vector_in_array(size_t pos, Names&&... names)
    {
        auto cursor = _cseek_in_array(_cursor, pos, std::forward<Names>(names)...);
        return _get_vector<T>(cursor);
    }

  private:
    JsonDoc _root;
    JsonValue const* _cursor;
    vector<JsonValue const*> _cursor_stack;

    Cursor _cseek(Cursor cursor) const
    {
        return cursor;
    }

    template <typename... Names>
    Cursor _cseek(Cursor cursor, string const& name, Names&&... names) const
    {
        if (name == "/") {
            cursor = &_root;
        } else if (name == "") {
            throw Error("can not seek an element with empty name");
        } else if (cursor->HasMember(name.c_str())) {
            cursor = &(*cursor)[name.c_str()];
        } else {
            throw Error(make_string("can not find member named '", name, "'"));
        }
        return _cseek(cursor, std::forward<Names>(names)...);
    }

    template <typename... Names>
    Cursor _cseek_in_array(Cursor cursor, size_t pos, Names&&... names) const
    {
        cursor = _cseek(cursor, std::forward<Names>(names)...);
        _assert_type(cursor, "array");
        auto n = cursor->Size();
        if (pos >= n) {
            throw Error(make_string(
                "can not seek item <",
                pos,
                "> in array because array size is ",
                n));
        }
        return &(cursor->GetArray()[pos]);
    }

    string _type_name(Cursor cursor) const
    {
        // auto const type = cursor->GetType();
        // if (type == rapidjson::kNullType) return "null";
        // if (type == rapidjson::kFalseType) return "false";
        // if (type == rapidjson::kTrueType) return "true";
        // if (type == rapidjson::kObjectType) return "object";
        // if (type == rapidjson::kArrayType) return "array";
        // if (type == rapidjson::kStringType) return "string";
        // if (type == rapidjson::kNumberType) return "number";
        // throw UNREACHABLE;

        if (cursor->IsString())
            return "string";
        else if (cursor->IsInt())
            return "int";
        else if (cursor->IsUint())
            return "uint";
        else if (cursor->IsInt64())
            return "long";
        else if (cursor->IsDouble())
            return "double";
        else if (cursor->IsFloat())
            return "float";
        else if (cursor->IsBool())
            return "bool";
        else if (cursor->IsNull())
            return "null";
        else if (cursor->IsArray())
            return "array";
        else if (cursor->IsObject())
            return "object";
        return "unknown";
    }

    void _assert_type(Cursor cursor, string_view type) const
    {
        if (_type_name(cursor) != type) {
            throw Error(make_string(
                "encountered element of type '",
                _type_name(cursor),
                "' while type '",
                type,
                "' is expected"));
        }
    }

    bool _is_typed_array(Cursor cursor, string_view type) const
    {
        if (!cursor->IsArray())
            return false;
        if (cursor->Size() < 1) {
            throw Error("can not determine element type of an empty array");
        }
        return (_type_name(_cseek_in_array(cursor, 0)) == type);
    }

    void _assert_array_elem_type(Cursor cursor, string_view type) const
    {
        if (!_is_typed_array(cursor, type)) {
            throw Error(make_string(
                "encountered array with elements of type '",
                _type_name(_cseek_in_array(cursor, 0)),
                "' while type '", type, "' is expected"));
        }
    }

    template <typename... Names>
    bool _has_member(Cursor cursor, string const& name, Names&&... names) const
    {
        if constexpr (sizeof...(names) == 0) {
            _assert_type(cursor, "object");
            return cursor->HasMember(name.c_str());
        } else {
            return _has_member(_cseek(cursor, name), std::forward<Names>(names)...);
        }
    }

    template <typename T>
    T _get_scalar(Cursor cursor) const;
    // To be specialized outside of the class.

    template <typename T>
    vector<T> _get_vector(Cursor cursor) const
    {
        _assert_type(cursor, "array");
        _assert_array_elem_type(cursor, zpz::type_name<T>());
        auto n = cursor->Size();
        auto values = vector<T>(n);
        for (size_t i = 0; i < n; i++) {
            values[i] = _get_scalar<T>(_cseek_in_array(cursor, i));
        }
        return std::move(values);
    }

    void _check_cursor() const
    {
        if (!_cursor_stack.empty()) {
            throw Error("`save_cursor` is not balanced out by `restore_cursor`");
        }
    }
};

// Explicit full specialization can not happen within a class;
// has to be place directly in the namespace scope.

template <>
string JsonReader::_get_scalar<string>(Cursor cursor) const
{
    _assert_type(cursor, "string");
    return cursor->GetString();
}

template <>
int JsonReader::_get_scalar<int>(Cursor cursor) const
{
    _assert_type(cursor, "int");
    return cursor->GetInt();
}

template <>
double JsonReader::_get_scalar<double>(Cursor cursor) const
{
    _assert_type(cursor, "double");
    return cursor->GetDouble();
}

template <>
float JsonReader::_get_scalar<float>(Cursor cursor) const
{
    _assert_type(cursor, "float");
    return cursor->GetFloat();
}

template <>
long JsonReader::_get_scalar<long>(Cursor cursor) const
{
    _assert_type(cursor, "long");
    return cursor->GetInt64();
}

template <>
bool JsonReader::_get_scalar<bool>(Cursor cursor) const
{
    _assert_type(cursor, "bool");
    return cursor->GetBool();
}

} // namespace zpz
#endif // _zpz_utilities_json_h_
