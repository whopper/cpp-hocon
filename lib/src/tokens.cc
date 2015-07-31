#include <internal/tokens.hpp>

using namespace std;

namespace hocon {

    /** Value token */
    value::value(unique_ptr<abstract_config_value> value) :
            token(token_type::VALUE), _value(move(value)) { }

    value::value(unique_ptr<abstract_config_value> value, string original_text) :
            token(token_type::VALUE, nullptr, original_text),
            _value(move(value)) { }

    abstract_config_value value::config_value() const {
        return *_value;
    }

    std::string value::to_string() const {
        return _value->transform_to_string();
    }

    simple_config_origin value::origin() const {
        return _value->origin();
    }

    bool value::operator==(const token& other) const {
        return other.get_token_type() == token_type::VALUE &&
               other.to_string() == to_string();
    }


    /** Line token */
    line::line(unique_ptr<simple_config_origin> origin) :
            token(token_type::NEWLINE, move(origin), "\n") { }

    string line::to_string() const {
        return "'\\n'@" + std::to_string(line_number());
    }

    bool line::operator==(const token& other) const {
        return (other.get_token_type() == token_type::NEWLINE) && (line_number() == other.line_number());
    }

    /** Unquoted text token */
    unquoted_text::unquoted_text(unique_ptr<simple_config_origin> origin, string text) :
            token(token_type::UNQUOTED_TEXT, move(origin), move(text)) { }

    string unquoted_text::to_string() const {
        return "'" + token_text() + "'";
    }

    bool unquoted_text::operator==(const token& other) const {
        return (other.get_token_type() == token_type::UNQUOTED_TEXT) &&
                (other.token_text() == token_text());
    }

    /** Ignored whitespace token */
    ignored_whitespace::ignored_whitespace(unique_ptr<simple_config_origin> origin, string whitespace) :
        token(token_type::IGNORED_WHITESPACE, move(origin), move(whitespace)) { }

    string ignored_whitespace::to_string() const {
        return "'" + token_text() + "' (WHITESPACE)";
    }

    bool ignored_whitespace::operator==(const token& other) const {
        return other.get_token_type() == token_type::IGNORED_WHITESPACE &&
                other.token_text() == token_text();
    }

    /** Problem token */
    problem::problem(unique_ptr<simple_config_origin> origin, string what, string message,
        bool suggest_quotes) : token(token_type::PROBLEM, move(origin)), _what(move(what)),
        _message(move(message)), _suggest_quotes(suggest_quotes) { }

    string problem::what() const {
        return _what;
    }

    string problem::message() const {
        return _message;
    }

    bool problem::suggest_quotes() const {
        return _suggest_quotes;
    }

    std::string problem::to_string() const {
        return "'" + _what + "' (" + _message + ")";
    }

    bool problem::operator==(const token& other) const {
        try {
            problem const& other_problem = dynamic_cast<problem const&>(other);
            return other_problem.what() == what() &&
                   other_problem.message() == message() &&
                   other_problem.suggest_quotes() == suggest_quotes();
        } catch (bad_cast ex){
            return false;
        }
    }

    /** Comment token */
    comment::comment(unique_ptr<simple_config_origin> origin, string text) :
        token(token_type::COMMENT, move(origin)), _text(move(text)) { }

    string comment::text() const {
        return _text;
    }

    string comment::to_string() const {
        return "'#" + _text + "' (COMMENT)";
    }

    bool comment::operator==(const token& other) const {
        return other.get_token_type() == token_type::COMMENT && to_string() == other.to_string();
    }

    /** Double-slash comment token */
    double_slash_comment::double_slash_comment(unique_ptr<simple_config_origin> origin, string text) :
        comment(move(origin), move(text)) { }

    string double_slash_comment::token_text() const {
        return "//" + text();
    }

    /** Hash comment token */
    hash_comment::hash_comment(unique_ptr<simple_config_origin> origin, string text) :
        comment(move(origin), move(text)) { }

    string hash_comment::token_text() const {
        return "#" + text();
    }

    /** Substitution token */
    substitution::substitution(unique_ptr<simple_config_origin> origin, bool optional,
        vector<token> expression) : token(token_type::SUBSTITUTION, move(origin)), _optional(optional),
        _expression(move(expression)) { }

    bool substitution::optional() const {
        return _optional;
    }

    vector<token> const& substitution::expression() const {
        return _expression;
    }

    string substitution::token_text() const {
        //TODO: this relies on tokenizer working to port properly
        return to_string();
    }

    string substitution::to_string() const {
        string result;
        for(auto&& t : _expression) {
            result += t.token_text();
        }
        return "'${" + result + "}'";
    }

    bool substitution::operator==(const token& other) const {
        return other.get_token_type() == token_type::SUBSTITUTION &&
               to_string() == other.to_string();
    }

    /** Singleton tokens */
    token& tokens::start_token() {
        static token _start(token_type::START, nullptr, "start of file", "");
        return _start;
    }

    token& tokens::end_token() {
        static token _end(token_type::END, nullptr, "start of file", "");
        return _end;
    }

    token& tokens::comma_token() {
        static token _comma(token_type::COMMA, nullptr, "','", ",");
        return _comma;
    }

    token& tokens::equals_token() {
        static token _equals(token_type::EQUALS, nullptr, "'='", "=");
        return _equals;
    }

    token& tokens::colon_token() {
        static token _colon(token_type::COLON, nullptr, "':'", ":");
        return _colon;
    }

    token& tokens::open_curly_token() {
        static token _open_curly(token_type::OPEN_CURLY, nullptr, "'{'", "{");
        return _open_curly;
    }

    token& tokens::close_curly_token() {
        static token _close_curly(token_type::CLOSE_CURLY, nullptr, "'}'", "}");
        return _close_curly;
    }

    token& tokens::open_square_token() {
        static token _open_square(token_type::OPEN_SQUARE, nullptr, "'['", "[");
        return _open_square;
    }

    token& tokens::close_square_token() {
        static token _close_square(token_type::CLOSE_SQUARE, nullptr, "']'", "]");
        return _close_square;
    }

    token& tokens::plus_equals_token() {
        static token _plus_equals(token_type::PLUS_EQUALS, nullptr, "'+='", "+=");
        return _plus_equals;
    }

}  // namespace hocon
