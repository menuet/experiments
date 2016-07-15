
#pragma once


#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include <experimental/filesystem>


namespace vcard {

    class Contact
    {
    public:

        using Properties = std::unordered_map<std::string, std::string>;
        using PropertyNames = std::unordered_set<std::string>;

        void load(std::istream& file);

        void save(std::ostream& file, const PropertyNames& propertyNames) const;

        const Properties& getProperties() const;

    private:

        Properties m_properties;
    };

    enum class ContactFileType
    {
        Vcf = 1,
        Csv = 2,
    };

    class Directory
    {
    public:

        using Contacts = std::vector<Contact>;
        using FilePath = std::experimental::filesystem::path;

        void loadFile(ContactFileType fileType, const FilePath& fileName);

        void saveFile(ContactFileType fileType, const FilePath& fileName) const;

        const Contacts& getContacts() const;

    private:

        Contacts m_contacts;
    };

} // namespace vcard

