#ifndef _zpz_avro_h_
#define _zpz_avro_h_


#include "avro/DataFile.hh"
#include "avro/Generic.hh"

#include <exception>
#include <string>
#include <vector>


namespace zpz
{

class AvroError: public std::runtime_error
{
    public:
        AvroError(std::string const & msg): std::runtime_error(msg) {}
        AvroError(char const * msg): std::runtime_error(msg) {}
};


class AvroReader
{
    // Reads data from Avro file that contains a single record at the top level.

    private:
        avro::GenericDatum _datum;
        avro::GenericDatum const *  _datum_cursor;
        std::vector<avro::GenericDatum const *>  _datum_cursor_saved;
        std::string _name;

    public:
        AvroReader(char const * data_file)
        {
            avro::DataFileReader<avro::GenericDatum> reader(data_file);
            auto schema = reader.readerSchema();
            _datum = avro::GenericDatum(schema);
            reader.read(_datum);
            if (_datum.type() != avro::AVRO_RECORD) {
                throw AvroError("top level of the AVRO data is not a record type");
            }
            auto const & r = _datum.value<avro::GenericRecord>();
            auto name = std::string(r.schema()->name());
            auto pos = name.find('.');
            while (pos != std::string::npos) {
                name = name.substr(pos + 1);
                pos = name.find('.');
            }  // strip off 'namespace' in 'name'
            _name = name;
            reader.close();

            _datum_cursor = & _datum;
            _datum_cursor_saved = {_datum_cursor};
        }

        std::string name() const
        {
            return _name;
        }

        std::string type_name() const
        {
            return avro::toString(_datum_cursor->type());
        }

        bool is_record() const
        {
            return _datum_cursor->type() == avro::AVRO_RECORD;
        }

        bool is_array() const
        {
            return _datum_cursor->type() == avro::AVRO_ARRAY;
        }

        AvroReader & seek(std::string const & name)
        {
            if ("" == name) {
                throw AvroError("can not seek an element with empty name");
            }

            // Re-position the cursor at the root of the doc.
            if ("/" == name) {
                _datum_cursor = &_datum;
                return *this;
            }

            if (! is_record()) {
                throw AvroError(
                    std::string("can not seek element '") + name
                    + "' because internal cursor of AvroReader is not pointing at a AVRO_RECORD; actual type is '"
                    + type_name() + "'");
            }

            auto const & rec = _datum_cursor->value<avro::GenericRecord>();
            if (!rec.hasField(name)) {
                throw AvroError(std::string("current record does not have field named '") + name + "'");
            }

            _datum_cursor = &rec.field(name);
            return *this;
        }

        template<typename... Names>
        AvroReader & seek(std::string const & name, Names const&... names)
        {
            this->seek(name);
            if constexpr(sizeof...(names) == 0) return *this;
            return this->seek(names...);
        }

        void save_cursor()
        {
            _datum_cursor_saved.push_back(_datum_cursor);
        }

        void restore_cursor()
        {
            _datum_cursor = _datum_cursor_saved.back();
            _datum_cursor_saved.pop_back();
        }

        std::size_t get_array_size() const
        {
            if (! is_array()) {
                throw AvroError(
                    std::string("can not get array size because internal cursor of AvroReader is not pointing at an AVRO_ARRAY; actual type is '")
                    + type_name() + "'");
            }

            auto const & data = _datum_cursor->value<avro::GenericArray>().value();
            return data.size();
        }

        AvroReader & seek_in_array(std::size_t pos)
        {
            auto n = this->get_array_size();
            if (pos >= n) {
                throw AvroError(
                    std::string("can not seek item <") + std::to_string(pos) + "> in array because array size is "
                    + std::to_string(n));
            }
            auto const & data = _datum_cursor->value<avro::GenericArray>().value();  // std::vector<GenericDatum>
            _datum_cursor = &data[pos];
            return *this;
        }

        template<typename T>
        T get_scalar() const
        {
            // `T` should be one of `int`, `double`, `std::string`.
            // `_datum_cursor` must be currently pointing at a scalar value of the correct type.
            return _datum_cursor->value<T>();
        }

        template<typename T>
        std::vector<T> get_vector() const
        {
            // `T` should be one of `int`, `double`, `std::string`.
            // `_datum_cursor` must be currently pointing at an array value of the correct type.
            if (! is_array()) {
                throw AvroError("can not get array size because internal cursor of AvroReader is not pointing at an AVRO_ARRAY");
            }
            auto const & data = _datum_cursor->value<avro::GenericArray>().value();
            std::vector<T> value;
            for (auto const & v : data) {
                value.push_back(v.value<T>());
            }
            return std::move(value);
        }
};



}  // namespace zpz
#endif  // _zpz_avro_h_
