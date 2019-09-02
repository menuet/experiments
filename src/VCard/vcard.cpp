
#include "vcard.hpp"
#include <iostream>
#pragma warning(disable : 4348) // disable warning C4348: 'boost::spirit::terminal<boost::spirit::tag::lit>::result_helper': redefinition of default parameter: parameter 3
#pragma warning(disable : 4180) // disable warning C4180: qualifier applied to function type has no meaning; ignored
#include <boost/spirit/include/qi.hpp>

namespace vcard {

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    void printDouble(double d)
    {
        std::cout << d << "\n";
    }

    struct StoreDouble
    {
        StoreDouble(std::vector<double>& doubles)
            : m_doubles(doubles)
        {
        }
        void operator()(double d, qi::unused_type, qi::unused_type) const
        {
            m_doubles.get().push_back(d);
        }
        void print() const
        {
            for (const auto& d : m_doubles.get())
            {
                std::cout << d << ',';
            }
            std::cout << '\n';
        }
        std::reference_wrapper<std::vector<double>> m_doubles;
    };

    template <typename Iterator>
    static bool parse_numbers(Iterator first, Iterator last, StoreDouble storeDouble)
    {
        using qi::double_;
        using qi::phrase_parse;
        using ascii::space;

        bool r = phrase_parse(
            first,                          /*< start iterator >*/
            last,                           /*< end iterator >*/
            double_[storeDouble] >> *(',' >> double_[storeDouble]),   /*< the parser >*/
            space                           /*< the skip-parser >*/
        );
        if (first != last) // fail if we did not get a full match
            return false;
        return r;
    }

    static void load()
    {
        std::string s = "123.5,456,-7.5";
        std::vector<double> doubles;
        StoreDouble storeDouble(doubles);
        const auto result = parse_numbers(begin(s), end(s), storeDouble);
        storeDouble.print();
        std::cout << result << "\n";
    }

    static void decodeHexa(const std::string& hexa, std::string& decodedPropertyValue)
    {
        unsigned int number = 0;
        const auto hexaLength = hexa.length();
        for (std::string::size_type index = 0; index < hexaLength; ++index)
        {
            const auto& c = hexa[index];
            unsigned int subnumber = 0;
            if (c >= '0' && c <= '9')
                subnumber = c - '0';
            else if (c >= 'A' && c <= 'F')
                subnumber = c - 'A' + 10;
            const auto shift = (hexaLength - index - 1) << 2;
            subnumber <<= shift;
            number += subnumber;
        }
        decodedPropertyValue += static_cast<char>(number);
    }

    static void decodePropertyValuePart(const std::string& encodedPropertyValuePart, std::string& decodedPropertyValue)
    {
        if (encodedPropertyValuePart[0] != '=')
        {
            decodedPropertyValue += encodedPropertyValuePart;
            return;
        }
        auto equalPosStop = encodedPropertyValuePart.find('=', 1);
        decltype(equalPosStop) equalPosStart = 1;
        while (equalPosStop != std::string::npos)
        {
            if (equalPosStart < equalPosStop)
                decodeHexa(encodedPropertyValuePart.substr(equalPosStart, equalPosStop - equalPosStart), decodedPropertyValue);
            equalPosStart = equalPosStop + 1;
            equalPosStop = encodedPropertyValuePart.find('=', equalPosStart);
        }
        if (equalPosStart < encodedPropertyValuePart.length())
            decodeHexa(encodedPropertyValuePart.substr(equalPosStart, equalPosStop - equalPosStart), decodedPropertyValue);
    }

    static std::string decodePropertyValue(const std::string& encodedPropertyValue)
    {
        std::string decodedPropertyValue;
        decodedPropertyValue.reserve(encodedPropertyValue.length());
        auto semicolonPosStop = encodedPropertyValue.find(';');
        decltype(semicolonPosStop) semicolonPosStart = 0;
        while (semicolonPosStop != std::string::npos)
        {
            if (semicolonPosStart < semicolonPosStop)
                decodePropertyValuePart(encodedPropertyValue.substr(semicolonPosStart, semicolonPosStop - semicolonPosStart), decodedPropertyValue);
            decodedPropertyValue += ' ';
            semicolonPosStart = semicolonPosStop + 1;
            semicolonPosStop = encodedPropertyValue.find(';', semicolonPosStart);
        }
        if (semicolonPosStart < encodedPropertyValue.length())
            decodePropertyValuePart(encodedPropertyValue.substr(semicolonPosStart), decodedPropertyValue);
        return decodedPropertyValue;
    }

    static void loadProperty(std::istream& file, std::string& propertyName, std::string& propertyValue)
    {
        propertyName.clear();
        getline(file, propertyName);
        const auto colonPos = propertyName.find(':');
        if (colonPos == std::string::npos)
        {
            propertyName.clear();
            propertyValue.clear();
            return;
        }

        propertyValue = propertyName.substr(colonPos + 1);
        propertyValue = decodePropertyValue(propertyValue);

        propertyName.resize(colonPos);

        const auto semicolonPos = propertyName.find(';');
        if (semicolonPos != std::string::npos)
            propertyName.resize(semicolonPos);
    }
#if 0
    static void getUnionOfPropertyNames(const Contact& contact, Contact::PropertyNames& propertyNames)
    {
        for (const auto& property : contact.getProperties())
        {
            propertyNames.insert(property.first);
        }
    }

    static void getUnionOfPropertyNames(const Directory& directory, Contact::PropertyNames& propertyNames)
    {
        for (const auto& contact : directory.getContacts())
        {
            getUnionOfPropertyNames(contact, propertyNames);
        }
    }
#endif
    void Contact::load(std::istream& file)
    {
        std::string propertyName;
        std::string propertyValue;
        loadProperty(file, propertyName, propertyValue);
        while (file && (propertyName != "BEGIN" || propertyValue != "VCARD"))
            loadProperty(file, propertyName, propertyValue);
        if (!file)
            return;
        loadProperty(file, propertyName, propertyValue);
        while (file && (propertyName != "END" || propertyValue != "VCARD"))
        {
            if (!propertyName.empty())
                m_properties.emplace(propertyName, propertyValue);
            loadProperty(file, propertyName, propertyValue);
        }
    }

    void Contact::save(std::ostream& file, const PropertyNames& propertyNames) const
    {
        for (const auto& propertyName : propertyNames)
        {
            const auto iterProperty = m_properties.find(propertyName);
            if (iterProperty != end(m_properties))
                file << iterProperty->second;
            file << ";";
        }
        file << "\n";
    }

    const Contact::Properties& Contact::getProperties() const
    {
        return m_properties;
    }

    void Directory::loadFile(ContactFileType fileType, const FilePath& fileName)
    {
        load();
#if 0
        std::ifstream file(fileName);
        while (file)
        {
            Contact contact;
            contact.load(file);
            m_contacts.push_back(std::move(contact));
        }
#endif
    }

    void Directory::saveFile(ContactFileType fileType, const FilePath& fileName) const
    {
#if 0
        std::ofstream file(fileName);
        Contact::PropertyNames propertyNames;
        getUnionOfPropertyNames(*this, propertyNames);
        for (const auto& propertyName : propertyNames)
        {
            file << propertyName << ";";
        }
        file << "\n";
        for (const auto& contact : m_contacts)
        {
            contact.save(file, propertyNames);
        }
#endif
    }

    const Directory::Contacts& Directory::getContacts() const
    {
        return m_contacts;
    }

} // namespace vcard

