#include "avro/DataFile.hh"
#include "avro/Generic.hh"

#include <exception>
#include <string>
#include <vector>
#include <experimental/string_view>


using string = std::string;
using string_view = std::experimental::string_view;
using size_t = std::size_t;


class AvroReader
{
    // Reads data from Avro file that contains a single record.

    private:
        avro::GenericDatum _datum;
        string _name;

    public:
        AvroReader(char const * data_file)
        {
            avro::DataFileReader<avro::GenericDatum> reader(data_file);
            auto schema = reader.readerSchema();
            _datum = avro::GenericDatum(schema);
            reader.read(_datum);
            if (_datum.type() != avro::AVRO_RECORD) {
                throw std::domain_error("top level of the AVRO data is not a record type");
            }
            auto const & r = _datum.value<avro::GenericRecord>();
            auto name = string(r.schema()->name());
            auto pos = name.find('.');
            while (pos != string::npos) {
                name = name.substr(pos + 1);
                pos = name.find('.');
            }
            _name = name;
            reader.close();
        }

        string name() const
        {
            return _name;
        }

        template<typename T>
        T get_scalar(string_view name)
        {
            // `T` should be one of `int`, `double`, `std::string`.
            auto const & rec = _datum.value<avro::GenericRecord>();
            for (size_t i = 0; i < rec.fieldCount(); i++) {
                if (rec.schema()->nameAt(i) == name) {
                    auto const & data = rec.fieldAt(i);
                    return data.value<T>();
                }
            }
            throw std::runtime_error("the requested datum '" + string(name) + "' was not found");
        }

        template<typename T>
        std::vector<T> get_vector(string_view name)
        {
            // `T` should be one of `int`, `double`, `std::string`.
            auto const & rec = _datum.value<avro::GenericRecord>();
            for (size_t i = 0; i < rec.fieldCount(); i++) {
                if (rec.schema()->nameAt(i) == name) {
                    auto const & data = rec.fieldAt(i).value<avro::GenericArray>().value();
                    std::vector<T> value;
                    for (auto const & v : data) {
                        value.push_back(v.value<T>());
                    }
                    return std::move(value);
                }
            }
            throw std::runtime_error("the requested datum '" + string(name) + "' was not found");
        }
};
