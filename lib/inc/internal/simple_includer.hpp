#pragma once

#include <hocon/config_includer.hpp>
#include <hocon/config_includer_file.hpp>
#include <hocon/config_parse_options.hpp>

namespace hocon {

    class name_source;
    class full_includer;
    class proxy;

    class simple_includer : public config_includer, public config_includer_file, public std::enable_shared_from_this<simple_includer> {
    public:
        simple_includer(shared_includer fallback);

        shared_includer with_fallback(shared_includer fallback) const override;

        shared_object include(shared_include_context context, std::string what) const override;

        shared_object include_without_fallback(shared_include_context context, std::string what) const;

        shared_object include_file(shared_include_context context, std::string what) const override;

        shared_object include_file_without_fallback(shared_include_context context, std::string what) const;

        static config_parse_options clear_for_include(shared_parse_options options);

        static shared_object from_basename(std::shared_ptr<name_source> source,
                                           std::string name,
                                           shared_parse_options options);

    private:
        shared_includer _fallback;
    };

    class name_source {
    public:
        virtual shared_parseable name_to_parseable(std::string name,
                                                   shared_parse_options parse_options) const = 0;
    };

    class relative_name_source : public name_source {
    public:
        relative_name_source(shared_include_context context);

        shared_parseable name_to_parseable(std::string name,
                                           shared_parse_options parse_options) const override;
    private:
        const shared_include_context _context;
    };

    class file_name_source : public name_source {
    public:
        shared_parseable name_to_parseable(std::string name,
                                           shared_parse_options parse_options) const override;
    };

}  // namespace hocon
