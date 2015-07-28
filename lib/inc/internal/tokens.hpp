#pragma once

#include "token.hpp"
#include <internal/values/abstract_config_value.hpp>

namespace hocon {

    class value : public token {
    public:
        value(std::unique_ptr<abstract_config_value> value);
        value(std::unique_ptr<abstract_config_value> value, std::string original_text);

        std::string to_string() const override;
        simple_config_origin origin() const override;

        abstract_config_value config_value() const;

    private:
        std::unique_ptr<abstract_config_value> _value;
    };

    class line : public token {
    public:
        line(std::unique_ptr<simple_config_origin> origin);

        std::string to_string() const override;

        bool operator==(token const& other) const override;
    };

    class unquoted_text : public token {
    public:
        unquoted_text(std::unique_ptr<simple_config_origin> origin, std::string text);

        std::string to_string() const override;

        bool operator==(const token& other) const override;

    };

    class ignored_whitespace : public token {
    public:
        ignored_whitespace(std::unique_ptr<simple_config_origin> origin, std::string whitespace);

        std::string to_string() const override;

        bool operator==(const token& other) const;
    };

    class problem : public token {
    public:
        problem(std::unique_ptr<simple_config_origin> origin, std::string what, std::string message,
            bool suggest_quotes);

        std::string what() const;
        std::string message() const;
        bool suggest_quotes() const;

        std::string to_string() const override;

        bool operator==(const token& other) const override;

    private:
        std::string _what;
        std::string _message;
        bool _suggest_quotes;
    };

    class comment : public token {
    public:
        comment(std::unique_ptr<simple_config_origin> origin, std::string text);

        std::string text() const;

        std::string to_string() const override;
        bool operator==(const token& other) const override;

    private:
        std::string _text;
    };

    class double_slash_comment : public comment {
    public:
        double_slash_comment(std::unique_ptr<simple_config_origin> origin, std::string text);

        std::string token_text() const override;
    };

    class hash_comment : public comment {
    public:
        hash_comment(std::unique_ptr<simple_config_origin> origin, std::string text);

        std::string token_text() const override;
    };

    class substitution : public token {
    public:
        substitution(std::unique_ptr<simple_config_origin> origin,
                     bool optional, std::vector<token> expression);

        bool optional() const;
        std::vector<token> const& expression() const;

        std::string token_text() const override;
        std::string to_string() const override;

        bool operator==(const token& other) const override;

    private:
        bool _optional;
        std::vector<token> _expression;
    };


    /** Free functions */
    bool isValueWithType(token t, config_value_type type);

    class tokens {
    public:
        /** Singleton tokens */
        static token& start_token();
        static token& end_token();
        static token& comma_token();
        static token& equals_token();
        static token& colon_token();
        static token& open_curly_token();
        static token& close_curly_token();
        static token& open_square_token();
        static token& close_square_token();
        static token& plus_equals_token();
    };

}  // namespace hocon
